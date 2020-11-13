
CFLAGS = -std=c11 -Wall -Werror -D_GNU_SOURCE
LDFLAGS = -lpthread -lssl -lcrypto -lmagic
OBJDIR = obj
OBJS = $(OBJDIR)/admin.o $(OBJDIR)/commands.o $(OBJDIR)/codybot.o \
$(OBJDIR)/server.o $(OBJDIR)/raw.o $(OBJDIR)/thread.o
PROGNAME = codybot

.PHONY: tcc default all prepare clean

default: all

all: prepare $(OBJS) $(PROGNAME)

prepare:
	@[ -d $(OBJDIR) ] || mkdir $(OBJDIR) 2>/dev/null || true

$(OBJDIR)/admin.o: src/admin.c
	gcc -c $(CFLAGS) src/admin.c -o $(OBJDIR)/admin.o

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
	gcc $(CFLAGS) $(OBJS) -o $(PROGNAME) $(LDFLAGS)

clean:
	@rm -rv $(OBJDIR) $(PROGNAME) $(PROGRUN) 2>/dev/null || true

tcc:
	tcc -lmagic -lssl -o $(PROGNAME) src/*.c

