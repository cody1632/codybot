
CFLAGS = -std=c11 -Wall -Werror -O2
LDFLAGS = -lssl -lcrypto
PROGNAME = codybot

.PHONY: default all clean

default: all

all: $(PROGNAME)

$(PROGNAME): codybot.c
	gcc $(CFLAGS) $(LDFLAGS) codybot.c -o $(PROGNAME)

clean:
	@rm -v $(PROGNAME) 2>/dev/null || true

