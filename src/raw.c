#include <stdio.h>
#include <string.h>

#include "codybot.h"

struct raw_line raw;
char *target;

// ":esselfe!~codybot@unaffiliated/codybot PRIVMSG ##c-offtopic :message"
// ":esselfe!~codybot@unaffiliated/codybot PRIVMSG SpringSprocket :message"
// If sender sends to channel, set target to channel
// If sender sends to PM/nick, set target to nick
char *RawGetTarget(struct raw_line *rawp) {
	if (strcmp(rawp->channel, nick) == 0)
		target = rawp->nick;
	else
		target = rawp->channel;
	return target;
}

void RawLineClear(struct raw_line *rawp) {
	memset(rawp->nick, 0, 1024);
	memset(rawp->username, 0, 1024);
	memset(rawp->host, 0, 1024);
	memset(rawp->command, 0, 1024);
	memset(rawp->channel, 0, 1024);
	memset(rawp->text, 0, 4096);
}

// Type of message to be parsed:
// :esselfe!~bsfc@unaffiliated/esselfe PRIVMSG #codybot :^codybot_version
void RawLineParse(struct raw_line *rawp, char *line) {
	RawLineClear(rawp);

	char *c = line;
	unsigned int cnt = 0, rec_nick = 1, rec_username = 0, rec_host = 0, rec_command = 0,
		rec_channel = 0, rec_text = 0; // recording flags
	
	// messages to skip:
// :livingstone.freenode.net 372 codybot :- Thank you for using freenode!
// :codybot MODE codybot :+Zi
// :ChanServ!ChanServ@services. MODE #codybot +o esselfe
// :NickServ!NickServ@services. NOTICE codybot :Invalid password for codybot.
// :freenode-connect!frigg@freenode/utility-bot/frigg NOTICE codybot :Welcome to freenode.
// :PING :livingstone.freenode.net
// ERROR :Closing Link: mtrlpq69-157-190-235.bell.ca (Quit: codybot)
	if (*c==':' && *(c+1)=='l' && *(c+2)=='i' && *(c+3)=='v' && *(c+4)=='i' && *(c+5)=='n' &&
		*(c+6)=='g' && *(c+7)=='s' && *(c+8)=='t' && *(c+9)=='o' && *(c+10)=='n' &&
		*(c+11)=='e' && *(c+12)=='.')
		return;
	else if (*(c+8)==' ' && *(c+9)=='M' && *(c+10)=='O' && *(c+11)=='D' && *(c+12)=='E')
		return;
	else if (*(c+1)=='C' && *(c+2)=='h' && *(c+3)=='a' && *(c+4)=='n' && *(c+5)=='S' &&
		*(c+6)=='e' && *(c+7)=='r' && *(c+8)=='v' && *(c+9)=='!')
		return;
	else if (*(c+1)=='N' && *(c+2)=='i' && *(c+3)=='c' && *(c+4)=='k' && *(c+5)=='S' &&
		*(c+6)=='e' && *(c+7)=='r' && *(c+8)=='v' && *(c+9)=='!')
		return;
	else if (*c==':' && *(c+1)=='f' && *(c+2)=='r' && *(c+3)=='e' && *(c+4)=='e' && *(c+5)=='n' &&
		*(c+6)=='o' && *(c+7)=='d' && *(c+8)=='e' && *(c+9)=='-' && *(c+10)=='c' &&
		*(c+11)=='o' && *(c+12)=='n')
		return;
	else if (*c=='P' && *(c+1)=='I' && *(c+2)=='N' && *(c+3)=='G' && *(c+4)==' ')
		return;
	else if (*c=='E' && *(c+1)=='R' && *(c+2)=='R' && *(c+3)=='O' && *(c+4)=='R')
		return;

	if (debug)
		printf("\n\e[01;32m##RawLineParse() started\e[00m\n");
	
	while (1) {
		if (*c == '\0')
			break;
		else if (*c == '\n') {
			*c = '\0';
			break;
		}
		else
			++c;
	}

	c = line;
	char word[4096];
	unsigned int cnt_total = 0;
	while (1) {
		if (*c == ':' && cnt_total == 0) {
			memset(word, 0, 4096);
			++c;
			if (debug)
				printf("&&raw: <<%s>>&&\n", line);
			continue;
		}
		else if (rec_nick && *c == '!') {
			sprintf(rawp->nick, "%s", word);
			memset(word, 0, 4096);
			rec_nick = 0;
			rec_username = 1;
			cnt = 0;
			if (debug)
				printf("&&nick: <%s>&&\n", rawp->nick);
		}
		else if (rec_username && cnt == 0 && *c == '~') {
			++c;
			continue;
		}
		else if (rec_username && *c == '@') {
			sprintf(rawp->username, "%s", word);
			memset(word, 0, 4096);
			rec_username = 0;
			rec_host = 1;
			cnt = 0;
			if (debug)
				printf("&&username: <%s>&&\n", rawp->username);
		}
		else if (rec_host && *c == ' ') {
			sprintf(rawp->host, "%s", word);
			memset(word, 0, 4096);
			rec_host = 0;
			rec_command = 1;
			cnt = 0;
			if (debug)
				printf("&&host: <%s>&&\n", rawp->host);
		}
		else if (rec_command && *c == ' ') {
			sprintf(rawp->command, "%s", word);
			memset(word, 0, 4096);
			rec_command = 0;
			rec_channel = 1;
			cnt = 0;
			if (debug)
				printf("&&command: <%s>&&\n", rawp->command);
		}
		else if (rec_channel && *c == ' ') {
			sprintf(rawp->channel, "%s", word);
			memset(word, 0, 4096);
			rec_channel = 0;
			if (strcmp(rawp->command, "PRIVMSG") == 0)
				rec_text = 1;
			cnt = 0;
			if (debug)
				printf("&&channel: <%s>&&\n", rawp->channel);
		}
		else if (rec_text && *c == '\0') {
			sprintf(rawp->text, "%s", word);
			memset(word, 0, 4096);
			rec_text = 0;
			cnt = 0;
			if (debug)
				printf("&&text: <%s>&&\n", rawp->text);
			break;
		}
		else {
			if (rec_text && *c == ':' && strlen(word) == 0) {
				++c;
				continue;
			}
			else
				word[cnt++] = *c;
		}

		++cnt_total;
		++c;
		if (!rec_text && (*c == '\0' || *c == '\n'))
			break;
	}

	if (debug)
		printf("\e[01;32m##RawLineParse() ended\e[00m\n\n");
}

