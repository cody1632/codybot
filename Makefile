
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

$(OBJDIR)/commands.o: src/commands.c
	gcc -c $(CFLAGS) src/commands.c -o $(OBJDIR)/commands.o

$(OBJDIR)/codybot.o: src/codybot.c
	gcc -c $(CFLAGS) src/codybot.c -o $(OBJDIR)/codybot.o

$(OBJDIR)/raw.o: src/raw.c
	gcc -c $(CFLAGS) src/raw.c -o $(OBJDIR)/raw.o

$(OBJDIR)/server.o: src/server.c
	gcc -c $(CFLAGS) src/server.c -o $(OBJDIR)/server.o

$(OBJDIR)/thread.o: src/thread.c
	gcc -c $(CFLAGS) src/thread.c -o $(OBJDIR)/thread.o

$(PROGNAME): $(OBJS)
	gcc $(CFLAGS) $(LDFLAGS) $(OBJS) -o $(PROGNAME)

clean:
	@rm -rv $(OBJDIR) $(PROGNAME) $(PROGRUN) 2>/dev/null || true

