#include <stdio.h>
#include <unistd.h>
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
// Return 0 if no more processing needs to be made and 1 if the raw struct will
// be populated
int version_once;
int RawLineParse(struct raw_line *rawp, char *line) {
	if (debug)
		Log(LOCAL, "##RawLineParse() started");
	
	RawLineClear(rawp);

	char *c = line;
	unsigned int cnt = 0, rec_nick = 1, rec_username = 0, rec_host = 0, rec_command = 0,
		rec_channel = 0, rec_text = 0; // recording flags
	
	// messages to skip:
// :freenode-connect!frigg@freenode/utility-bot/frigg NOTICE codybot :Welcome to freenode.
// :NickServ!NickServ@services. NOTICE codybot :Invalid password for codybot.
// :PING :livingstone.freenode.net
// ERROR :Closing Link: mtrlpq69-157-190-235.bell.ca (Quit: codybot)
// :livingstone.freenode.net 372 codybot :- Thank you for using freenode!
// :codybot MODE codybot :+Zi
// :ChanServ!ChanServ@services. MODE #codybot +o esselfe
	if (strncmp(line, ":freenode-connect", 17) == 0 && !version_once) {
		version_once = 1;
		sprintf(buffer, "NOTICE freenode-connect :\x01VERSION codybot %s\x01",
			codybot_version_string);
		if (use_ssl)
			SSL_write(pSSL, buffer, strlen(buffer));
		else
			write(socket_fd, buffer, strlen(buffer));
		Log(OUT, buffer);
		return 0;
	}
	else if (strncmp(line, "NickServ!", 9) == 0 || strncmp(line, "ChanServ!", 9) == 0)
		return 0;
	else if (strncmp(line, "PING :", 6) == 0)
		return 0;
	else if (strncmp(line, "ERROR :", 7) == 0)
		return 0;

	// Check the theorical raw.command field for raw lines to skip
	while (1) {
		if (*c == '\0')
			break;
		else if (*c == ' ') { // process at the first space encountered,
			++c;
			if ((*c >= '0' && *c <= '9') || strncmp(c, "MODE ", 5) == 0 ||
				strncmp(c, "NOTICE ", 7) == 0)
				return 0;
			else
				break;
		}
		++c;
	}

	// Remove newline
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
			if (debug) {
				sprintf(buffer, "  raw: <<%s>>", line);
				Log(LOCAL, buffer);
			}
			continue;
		}
		else if (rec_nick && *c == '!') {
			sprintf(rawp->nick, "%s", word);
			memset(word, 0, 4096);
			rec_nick = 0;
			rec_username = 1;
			cnt = 0;
			if (debug) {
				sprintf(buffer, "  nick: <%s>", rawp->nick);
				Log(LOCAL, buffer);
			}
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
			if (debug) {
				sprintf(buffer, "  username: <%s>", rawp->username);
				Log(LOCAL, buffer);
			}
		}
		else if (rec_host && *c == ' ') {
			sprintf(rawp->host, "%s", word);
			memset(word, 0, 4096);
			rec_host = 0;
			rec_command = 1;
			cnt = 0;
			if (debug) {
				sprintf(buffer, "  host: <%s>", rawp->host);
				Log(LOCAL, buffer);
			}
		}
		else if (rec_command && *c == ' ') {
			sprintf(rawp->command, "%s", word);
			memset(word, 0, 4096);
			rec_command = 0;
			rec_channel = 1;
			cnt = 0;
			if (debug) {
				sprintf(buffer, "  command: <%s>", rawp->command);
				Log(LOCAL, buffer);
			}
		}
		else if (rec_channel && *c == ' ') {
			sprintf(rawp->channel, "%s", word);
			memset(word, 0, 4096);
			rec_channel = 0;
			if (strcmp(rawp->command, "PRIVMSG") == 0)
				rec_text = 1;
			cnt = 0;
			if (debug) {
				sprintf(buffer, "  channel: <%s>", rawp->channel);
				Log(LOCAL, buffer);
			}
		}
		else if (rec_text && *c == '\0') {
			sprintf(rawp->text, "%s", word);
			memset(word, 0, 4096);
			rec_text = 0;
			cnt = 0;
			if (debug) {
				sprintf(buffer, "  text: <%s>", rawp->text);
				Log(LOCAL, buffer);
			}
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
		Log(LOCAL, "##RawLineParse() ended\n");
	
	return 1;
}

void RawMsg(struct raw_line *rawp) {
	if (!IsAdmin(rawp->nick, rawp->host)) {
		Msg("Only an admin can run this command.");
		return;
	}

	Log(OUT, rawp->text+8);

	// Took me a while to realize the message would be ignored without this,
	// which was previously stripped in RawLineParse(), so add it back here.
	rawp->text[strlen(rawp->text)] = '\n';

	if (use_ssl)
		SSL_write(pSSL, rawp->text+8, strlen(rawp->text+8));
	else
		write(socket_fd, rawp->text+8, strlen(rawp->text+8));
}

