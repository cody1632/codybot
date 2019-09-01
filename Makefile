
CFLAGS = -std=c11 -Wall -Werror -D_GNU_SOURCE
LDFLAGS = -lpthread -lssl -lcrypto
OBJDIR = obj
OBJS = $(OBJDIR)/commands.o $(OBJDIR)/codybot.o
PROGNAME = codybot

.PHONY: default all prepare clean

default: all

all: prepare $(OBJS) $(PROGNAME)

prepare:
	@[[ -d $(OBJDIR) ]] || mkdir $(OBJDIR)

$(OBJDIR)/commands.o: commands.c
	gcc -c $(CFLAGS) commands.c -o $(OBJDIR)/commands.o

$(OBJDIR)/codybot.o: codybot.c
	gcc -c $(CFLAGS) codybot.c -o $(OBJDIR)/codybot.o

$(PROGNAME): $(OBJS)
	gcc $(CFLAGS) $(LDFLAGS) $(OBJS) -o $(PROGNAME)

clean:
	@rm -rv $(OBJDIR) $(PROGNAME) 2>/dev/null || true

