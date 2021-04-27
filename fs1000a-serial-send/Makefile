BINARY := fs1000a-serial-send
DESTDIR ?= /usr/local
CFLAGS ?= -O2 \
	  -Wall -Wextra -pedantic -Werror \
	  -D_FORTIFY_SOURCE=2 \
	  -fpie -Wl,-pie

.PHONY: all install uninstall

all: $(BINARY)

$(BINARY): main.c
	$(CC) $(CFLAGS) $< -o $@

install: $(BINARY)
	install --target-directory=$(DESTDIR)/sbin $<

uninstall:
	rm $(DESTDIR)/sbin/$(BINARY)

