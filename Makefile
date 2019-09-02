
CFLAGS = -std=c11 -Wall -Werror -D_GNU_SOURCE
LDFLAGS = -lpthread -lssl -lcrypto
OBJDIR = obj
OBJS = $(OBJDIR)/commands.o $(OBJDIR)/codybot.o $(OBJDIR)/server.o \
$(OBJDIR)/raw.o $(OBJDIR)/thread.o
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

$(OBJDIR)/raw.o: raw.c
	gcc -c $(CFLAGS) raw.c -o $(OBJDIR)/raw.o

$(OBJDIR)/server.o: server.c
	gcc -c $(CFLAGS) server.c -o $(OBJDIR)/server.o

$(OBJDIR)/thread.o: thread.c
	gcc -c $(CFLAGS) thread.c -o $(OBJDIR)/thread.o

$(PROGNAME): $(OBJS)
	gcc $(CFLAGS) $(LDFLAGS) $(OBJS) -o $(PROGNAME)

clean:
	@rm -rv $(OBJDIR) $(PROGNAME) $(PROGRUN) 2>/dev/null || true

