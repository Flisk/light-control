# light-control

This a bunch of simple hacked-together programs I use for my homebrew light
control system.

## How It Works

`remote` is a Flask app running on a Pi in my home network. I keep a tab with
this app open in all my browsers, and I have it set up as one of those
"progressive web apps" on my iOS device. I press "A ON" in that app and the
command goes out to the Pi.

`remote` contains a hard-coded dictionary mapping allowed user commands to
shell commands. A rule for doas (a sudo alternative that emerged from OpenBSD)
allows `remote` to elevate its privileges, which is required for serial access.
It sends the button press off to a script called `brenn` (short for
Brennenstuhl, the brand of RF outlets I'm using) which then maps the press to
its corresponding RF code, which is finally handed over to
`fs1000a-serial-send`:

```sh
#!/usr/bin/env bash
set -eu

fail_usage() {
        >&2 echo "$*"
        >&2 echo "usage: $0 [ a | b | c | d ] [ on | off]"
        exit 1
}

if [[ "$(id --user)" != 0 ]]; then
        >&2 echo "must run as root"
        exit 1
fi

if [[ $# != 2 ]]; then
        fail_usage "$0: invalid number of arguments"
fi

case "$1-$2" in
        a-on)  rf_code=[redacted] ;;
        a-off) rf_code=[redacted] ;;
        b-on)  rf_code=[redacted] ;;
        b-off) rf_code=[redacted] ;;
        c-on)  rf_code=[redacted] ;;
        c-off) rf_code=[redacted] ;;
        d-on)  rf_code=[redacted] ;;
        d-off) rf_code=[redacted] ;;

        a-*|b-*|c-*|d-*)
                fail_usage "invalid command"
                ;;

        *)
                fail_usage "invalid outlet id"
esac

fs1000a-serial-send "$rf_code"
```

`fs1000a-serial-send` then opens `/dev/ttyUSB0`, the virtual serial port of an
attached Arduino Nano, writes the RF code to it and reads back a response.

The Arduino has an FS1000A ISM sender attached to its fifth digital pin and
runs the following program using the RCSwitch library:

```
#include <RCSwitch.h>

RCSwitch s = RCSwitch();

void setup() {
  s.enableTransmit(5);
  s.setProtocol(4);

  Serial.begin(9600);
}

void loop() {
  long codeToSend = Serial.parseInt();

  if (codeToSend == 0) {
    return;
  }

  s.send(codeToSend, 24);
  Serial.print(codeToSend);
  Serial.println(" ok");
}
```

Finally, the RF code is blown into the aether by the FS1000A, one out of a set
of my ISM-controlled power sockets responds, and then the light turns on.

Or off, or on and off in rapid succession because pressing these buttons and
having things happen is too fun.

## Some More Thoughts

There's some not-that-critical things that could be better here which I
may or may not address at some point:

- `brenn` is invoked as root, but doesn't really need to be. Privilege
  elevation is technically only necessary once `fs1000a-serial-send` is
  invoked.

- Privilege elevation could be completely obsoleted by turning
  `fs1000a-serial-send` into a daemon that takes its commands from unprivileged
  client processes over a unix socket.

- The response code written back by the Arduino program is a debugging leftover
  that serves no real purpose; I was just too lazy to flash a modified version
  while I was working on `fs1000a-serial-send`.

## Copyright

For the peace of mind of anyone who's a bit of a copyright stickler and would
like to reuse code from this project:

I hereby place my work in this repository into the public domain. Go nuts.

