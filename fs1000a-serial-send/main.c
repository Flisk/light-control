#include <stdlib.h>
#include <stdio.h>

#include <sys/file.h>
#include <error.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#define SERIAL_PORT "/dev/ttyUSB0"

#define EXPECT(condition, _errno, error_format) \
	if (!(condition)) { \
		error(1, (_errno), (error_format)); \
	}

#define NO_ERRNO 0

int main(int argc, char **argv)
{
	EXPECT(argc == 2, NO_ERRNO, "invalid usage; provide a single code"); 

	int c = atoi(argv[1]);
	EXPECT(c != 0, NO_ERRNO, "invalid arg; 0 or not an integer");

	int fd = open(SERIAL_PORT, O_RDWR);
	EXPECT(fd >= 0, errno, "failed to open " SERIAL_PORT);

	EXPECT(flock(fd, LOCK_EX) == 0, errno, "flock failed");

	struct termios t;
	EXPECT(tcgetattr(fd, &t) == 0, errno, "tcgetattr failed");

	t.c_cflag &= ~PARENB;        // no parity
	t.c_cflag &= ~CSTOPB;        // one stop bit
	t.c_cflag &= ~CSIZE;         // clear size bits
	t.c_cflag |= CS8;            // 8 bits per byte
	t.c_cflag &= ~CRTSCTS;       // no flow control
	t.c_cflag |= CREAD | CLOCAL; // enable reading and ignore ctrl lines

	EXPECT(tcsetattr(fd, TCSANOW, &t) == 0, errno, "failed to set termio settings");
	EXPECT(cfsetspeed(&t, B9600) == 0, errno, "failed to set baud rate");

	EXPECT(dprintf(fd, "%d\n", c) > 0, NO_ERRNO, "failed to write rf code");

	while (1) {
		char cc;
		ssize_t n = read(fd, &cc, 1);
		EXPECT(n == 1, errno, "failed to read response");
		putchar(cc);

		if (cc == '\n') {
			break;
		}
	}

	// FDs and locks intentionally not cleaned up here. the kernel does
	// this for us, and if you copy this code to a function without
	// reviewing it (or even happening to read this comment), then you're a
	// fool and deserve whatever pain will follow.

	exit(EXIT_SUCCESS);
}
