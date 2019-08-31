#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

const char *codybot_version_string = "0.1.17";

static const struct option long_options[] = {
	{"help", no_argument, NULL, 'h'},
	{"version", no_argument, NULL, 'V'},
	{"debug", no_argument, NULL, 'd'},
	{"blinkenshell", no_argument, NULL, 'b'},
	{"freenode", no_argument, NULL, 'f'},
	{"nick", required_argument, NULL, 'n'},
	{"port", required_argument, NULL, 'p'},
	{"server", required_argument, NULL, 's'},
	{NULL, 0, NULL, 0}
};
static const char *short_options = "hVdbfn:P:p:s:";

int debug, socket_fd, ret, endmainloop;
unsigned long long fortune_total;
struct timeval tv0;
struct tm *tm0;
time_t t0;
unsigned int server_port;
char *log_filename = "codybot.log";
char *buffer, *buffer_rx, *buffer_cmd, *buffer_log, *server_ip;
char *nick;
char *FullUserName = "codybot";
char *hostname = "BSFC-VMLUNAR";
char *target;

// sao.blinkenshell.org
char *server_ip_blinkenshell = "194.14.45.5";

// medusa.blinkenshell.org
//char *server_ip_blinkenshell = "69.164.197.11";

// livingstone.freenode.net
char *server_ip_freenode = "107.182.226.199";

SSL *pSSL;

void HelpShow(void) {
	printf("Usage: codybot { -h/--help | -V/--version | -b/--blinkenshell | -f/--freenode }\n");
	printf("Usage: codybot { -d/--debug | -n/--nick NICK | -p/--port NUM | -s/--server URL }\n");
}

void Log(char *text) {
	FILE *fp = fopen(log_filename, "a+");
	if (fp == NULL) {
		fprintf(stderr, "##codybot error: Cannot open %s: %s\n", log_filename, strerror(errno));
		return;
	}

	gettimeofday(&tv0, NULL);
	t0 = (time_t)tv0.tv_sec;
	tm0 = gmtime(&t0);
	sprintf(buffer_log, "%02d:%02d:%02d.%03ld ##%s##\n", tm0->tm_hour, tm0->tm_min, tm0->tm_sec,
		tv0.tv_usec, text);
	fputs(buffer_log, fp);
	fputs(buffer_log, stdout);

	fclose(fp);
}

void Logr(char *text) {
	FILE *fp = fopen(log_filename, "a+");
	if (fp == NULL) {
		fprintf(stderr, "##codybot error: Cannot open %s: %s\n", log_filename, strerror(errno));
		return;
	}

	gettimeofday(&tv0, NULL);
	t0 = (time_t)tv0.tv_sec;
	tm0 = gmtime(&t0);
	sprintf(buffer_log, "%02d:%02d:%02d.%03ld <<%s>>\n", tm0->tm_hour, tm0->tm_min, tm0->tm_sec,
		tv0.tv_usec, text);
	fputs(buffer_log, fp);
	fputs(buffer_log, stdout);

	fclose(fp);
}

void Logx(char *text) {
	FILE *fp = fopen(log_filename, "a+");
	if (fp == NULL) {
		fprintf(stderr, "##codybot error: Cannot open %s: %s\n", log_filename, strerror(errno));
		return;
	}

	gettimeofday(&tv0, NULL);
	t0 = (time_t)tv0.tv_sec;
	tm0 = gmtime(&t0);
	sprintf(buffer_log, "%02d:%02d:%02d.%03ld $$%s$$\n", tm0->tm_hour, tm0->tm_min, tm0->tm_sec,
		tv0.tv_usec, text);
	fputs(buffer_log, fp);
	fputs(buffer_log, stdout);

	fclose(fp);
}

// a raw line from the server should hold something like one of these:
// :esselfe!~bsfc@unaffiliated/esselfe PRIVMSG #codybot :^stats
// :codybot!~user@unaffiliated/esselfe PRIVMSG ##linux-offtopic :^fortune
// :NickServ!NickServ@services. NOTICE codybot :Invalid password for codybot.
// :freenode-connect!frigg@freenode/utility-bot/frigg NOTICE codybot :Welcome to freenode.
// :PING :livingstone.freenode.net
// :codybot MODE codybot :+Zi
// :livingstone.freenode.net 372 codybot :- Thank you for using freenode!
struct raw_line {
	char *nick;
	char *username;
	char *host;
	char *command;
	char *channel;
	char *text;
};
struct raw_line raw;

// :esselfe!~bsfc@unaffiliated/esselfe PRIVMSG #codybot :!codybot_version
void RawLineParse(struct raw_line *raw, char *line) {
// Getting a double free error with this, weird...
/*	if (raw->nick) free(raw->nick);
	if (raw->username) free(raw->username);
	if (raw->host) free(raw->host);
	if (raw->command) free(raw->command);
	if (raw->channel) free(raw->channel);
	if (raw->text) free(raw->text); */

	char *c = line, word[4096];
	unsigned int cnt = 0, rec_nick = 1, rec_username = 0, rec_host = 0, rec_command = 0,
		rec_channel = 0, rec_text = 0;
	
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
		printf("\n##RawLineParse() started\n");
	
	// remove multi-lines buffer
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
			raw->nick = (char *)malloc(strlen(word)+1);
			sprintf(raw->nick, "%s", word);
			memset(word, 0, 4096);
			rec_nick = 0;
			rec_username = 1;
			cnt = 0;
			if (debug)
				printf("&&nick: <%s>&&\n", raw->nick);
		}
		else if (rec_username && cnt == 0 && *c == '~') {
			++c;
			continue;
		}
		else if (rec_username && *c == '@') {
			raw->username = (char *)malloc(strlen(word)+1);
			sprintf(raw->username, "%s", word);
			memset(word, 0, 4096);
			rec_username = 0;
			rec_host = 1;
			cnt = 0;
			if (debug)
				printf("&&username: <%s>&&\n", raw->username);
		}
		else if (rec_host && *c == ' ') {
			raw->host = (char *)malloc(strlen(word)+1);
			sprintf(raw->host, "%s", word);
			memset(word, 0, 4096);
			rec_host = 0;
			rec_command = 1;
			cnt = 0;
			if (debug)
				printf("&&host: <%s>&&\n", raw->host);
		}
		else if (rec_command && *c == ' ') {
			raw->command = (char *)malloc(strlen(word)+1);
			sprintf(raw->command, "%s", word);
			memset(word, 0, 4096);
			rec_command = 0;
			rec_channel = 1;
			cnt = 0;
			if (debug)
				printf("&&command: <%s>&&\n", raw->command);
		}
// :esselfe!~bsfc@unaffiliated/esselfe PRIVMSG #codybot :!codybot_version
		else if (rec_channel && *c == ' ') {
			raw->channel = (char *)malloc(strlen(word)+1);
			sprintf(raw->channel, "%s", word);
			memset(word, 0, 4096);
			rec_channel = 0;
			if (strcmp(raw->command, "PRIVMSG")==0)
				rec_text = 1;
			cnt = 0;
			if (debug)
				printf("&&channel: <%s>&&\n", raw->channel);
		}
		else if (rec_text && *c == '\0') {
			raw->text = (char *)malloc(strlen(word)+1);
			sprintf(raw->text, "%s", word);
			memset(word, 0, 4096);
			rec_text = 0;
			cnt = 0;
			if (debug)
				printf("&&text: <%s>&&\n", raw->text);
			break;
		}
		else {
			if (*c != ':')
				word[cnt++] = *c;
		}

		++cnt_total;
		++c;
		if (!rec_text && (*c == '\0' || *c == '\n'))
			break;
	}

	if (debug)
		printf("##RawLineParse() ended\n\n");
}

char *GetTarget(struct raw_line *rawp) {
	if (strcmp(raw.channel, nick)==0)
		target = raw.nick;
	else
		target = raw.channel;
	return target;
}

void Fortune(struct raw_line *rawp) {
	FILE *fp = fopen("linux.fortune", "r");
	if (fp == NULL) {
		fprintf(stderr, "##codybot error: Cannot open linux.fortune database: %s\n", strerror(errno));
		if (server_ip == server_ip_blinkenshell)
			sprintf(buffer_cmd, "privmsg #blinkenshell :fortune error: cannot open linux.fortune database\n");
		else
			sprintf(buffer_cmd, "privmsg #codybot :fortune error: cannot open linux.fortune database\n");
		SSL_write(pSSL, buffer_cmd, strlen(buffer_cmd));
		Log(buffer_cmd);
		memset(buffer_cmd, 0, 4096);
		return;
	}

	fseek(fp, 0, SEEK_END);
	unsigned long filesize = (unsigned long)ftell(fp);
	fseek(fp, 0, SEEK_SET);
	gettimeofday(&tv0, NULL);
	srand((unsigned int)tv0.tv_usec);
	fseek(fp, rand()%(filesize-500), SEEK_CUR);

	int c = 0, cprev, cnt = 0;
	char fortune_line[4096];
	memset(fortune_line, 0, 4096);
	while (1) {
		cprev = c;
		c = fgetc(fp);
		if (c == -1) {
			c = ' ';
			break;
		}
		if (cprev == '\n' && c == '%') {
			//skip the newline
			fgetc(fp);
			c = ' ';
			break;
		}
	}

	while (1) {
		cprev = c;
		c = fgetc(fp);
		if (c == -1)
			break;
		else if (c == '\t' && cprev == '\n')
			break;
		else if (c == '%' && cprev == '\n')
			break;
		else if (c == '\n' && cprev == '\n')
			fortune_line[cnt++] = ' ';
		else if (c == '\n' && cprev != '\n')
			fortune_line[cnt++] = ' ';
		else
			fortune_line[cnt++] = c;
	}

	if (strlen(fortune_line) > 0) {
		GetTarget(rawp);
		sprintf(buffer_cmd, "privmsg %s :fortune: %s\n", target, fortune_line);
		SSL_write(pSSL, buffer_cmd, strlen(buffer_cmd));
		Log(buffer_cmd);
		memset(buffer_cmd, 0, 4096);
	}

	fclose(fp);

	fp = fopen("stats", "w");
	if (fp == NULL) {
		fprintf(stderr, "##codybot error: Cannot open stats file\n");
		return;
	}

	char str[1024];
	sprintf(str, "%llu\n", ++fortune_total);
	fputs(str, fp);

	fclose(fp);
}

char *slap_items[20] = {
"an USB cord", "a power cord", "a laptop", "a slice of ham", "a keyboard", "a laptop cord",
"a banana peel", "a dictionary", "an atlas book", "a biography book", "an encyclopedia",
"a rubber band", "a large trout", "a rabbit", "a lizard", "a dinosaur",
"a chair", "a mouse pad", "a C programming book", "a belt"
};
void SlapCheck(struct raw_line *rawp) {
	char *c = rawp->text;
	if ((*c==1 && *(c+1)=='A' && *(c+2)=='C' && *(c+3)=='T' && *(c+4)=='I' &&
	  *(c+5)=='O' && *(c+6)=='N' && *(c+7)==' ' &&
	  *(c+8)=='s' && *(c+9)=='l' && *(c+10)=='a' && *(c+11)=='p' && *(c+12)=='s' &&
	  *(c+13)==' ') && 
	  ((*(c+14)=='c' && *(c+15)=='o' && *(c+16)=='d' && *(c+17)=='y' &&
	  *(c+18)=='b' && *(c+19)=='o' && *(c+20)=='t' && *(c+21)==' ') ||
	  (*(c+14)=='S' && *(c+15)=='p' && *(c+16)=='r' && *(c+17)=='i' &&
	  *(c+18)=='n' && *(c+19)=='g' && *(c+20)=='S' && *(c+21)=='p' && *(c+22)=='r' &&
	  *(c+23)=='o' && *(c+24)=='c' && *(c+25)=='k' && *(c+26)=='e' && *(c+27)=='t' &&
	  *(c+28)==' '))) {
		GetTarget(rawp);
		gettimeofday(&tv0, NULL);
		srand((unsigned int)tv0.tv_usec);
		sprintf(buffer_cmd, "privmsg %s :%cACTION slaps %s with %s%c\n", 
			target, 1, rawp->nick, slap_items[rand()%20], 1);
		SSL_write(pSSL, buffer_cmd, strlen(buffer_cmd));
		Log(buffer_cmd);
		memset(buffer_cmd, 0, 4096);
	}
}

void Stats(struct raw_line *rawp) {
	FILE *fp = fopen("stats", "r");
	if (fp == NULL) {
		fprintf(stderr, "##codybot error: Cannot open stats file\n");
		return;
	}
	else {
		char str[1024];
		fgets(str, 1023, fp);
		fclose(fp);
		fortune_total = atoi(str);
	}
	GetTarget(rawp);
	sprintf(buffer_cmd, "privmsg %s :Given fortune cookies: %llu\n", target, fortune_total);
	SSL_write(pSSL, buffer_cmd, strlen(buffer_cmd));
	Log(buffer_cmd);
	memset(buffer_cmd, 0, 4096);
}

void Weather(struct raw_line *rawp) {
	unsigned int cnt = 0;
	char city[1024], *cp = rawp->text + strlen("^weather ");
	memset(city, 0, 1024);
	while (1) {
		if (*cp == '\n' || *cp == '\0')
			break;
		else if (cnt == 0 && *cp == ' ') {
			++cp;
			continue;
		}
		else if (*cp == ' ')
			break;
		
		city[cnt] = *cp;
		++cnt;
		++cp;
	}
	memset(raw.text, 0, strlen(raw.text));

	sprintf(buffer, "wget -t 1 -T 24 https://wttr.in/%s -O %s.html\n", city, city);
	system(buffer);
	sprintf(buffer,
		"sed -n \"3p\" %s.html |sed 's///g;s/\\[0m//g;s/\\[38\\;5\\;[0-9][0-9][0-9]m//g;s@\\\\@@g;s@/@@g;s/^ *//g' > %s.temp", city, city);
	system(buffer);
	sprintf(buffer, 
		"sed -n \"4p\" %s.html |sed 's/\\[0m//g;s/\\[38\\;5\\;[0-9][0-9][0-9]m//g' |grep -o '[0-9]*' > %s.temp2", city, city);
	system(buffer);

	char temp[1024], temp2[1024];
	sprintf(buffer, "%s.temp", city);
	FILE *fp = fopen(buffer, "r");
	if (fp == NULL) {
		sprintf(buffer_cmd, "##codybot error: Cannot open %s: %s\n", buffer, strerror(errno));
		SSL_write(pSSL, buffer_cmd, strlen(buffer_cmd));
		Log(buffer_cmd);
		memset(buffer_cmd, 0, 4096);
	}
	fgets(temp2, 1023, fp);
	fclose(fp);
	
	// remove blanks at the start of the line
	// ie "             Partly cloudy"
	cp = temp2;
	cnt = 0;
	while (1) {
		if (*cp == '\n' || *cp == '\0')
			break;
		else if (cnt == 0 && (*cp == ' ' || *cp == '\t' || *cp == 27)) {
			++cp;
			continue;
		}
		else {
			temp[cnt] = *cp;
			++cnt;
			++cp;
		}
	}
	temp[cnt+1] = '\0';

	sprintf(buffer, "%s.temp2", city);
	fp = fopen(buffer, "r");
	if (fp == NULL) {
		sprintf(buffer_cmd, "##codybot error: Cannot open %s: %s\n", buffer, strerror(errno));
		SSL_write(pSSL, buffer_cmd, strlen(buffer_cmd));
		Log(buffer_cmd);
		memset(buffer_cmd, 0, 4096);
	}
	fgets(temp2, 1023, fp);
	fclose(fp);

	temp2[strlen(temp2)-1] = ' ';
	int deg_celsius = atoi(temp2);
	int deg_farenheit = (deg_celsius * 9 / 5) + 32;
	sprintf(buffer_cmd, "privmsg %s :%s: %s %dC/%dF\n", target, city, temp, deg_celsius, deg_farenheit);
	SSL_write(pSSL, buffer_cmd, strlen(buffer_cmd));
	Log(buffer_cmd);
	memset(buffer_cmd, 0, 4096);

	sprintf(buffer, "rm %s.*\n", city);
	system(buffer);
	memset(buffer, 0, 4096);
}

// function to process messages received from server
void *RXThreadFunc(void *argp) {
	while (!endmainloop) {
		memset(buffer_rx, 0, 4096);
		SSL_read(pSSL, buffer_rx, 4095);
		if (strlen(buffer_rx) == 0) {
			endmainloop = 1;
			break;
		}
		buffer_rx[strlen(buffer_rx)-2] = '\0';
		if (buffer_rx[0] != 'P' && buffer_rx[1] != 'I' && buffer_rx[2] != 'N' &&
		  buffer_rx[3] != 'G' && buffer_rx[4] != ' ')
			Logr(buffer_rx);
		// respond to ping request from the server
		if (buffer_rx[0] == 'P' && buffer_rx[1] == 'I' && buffer_rx[2] == 'N' &&
			buffer_rx[3] == 'G' && buffer_rx[4] == ' ' && buffer_rx[5] == ':') {
			if (server_ip == server_ip_blinkenshell) {
				sprintf(buffer_cmd, "%s\n", buffer_rx);
				buffer_cmd[1] = 'O';
				SSL_write(pSSL, buffer_cmd, strlen(buffer_cmd));
				if (debug)
					Log(buffer_cmd);
				memset(buffer_cmd, 0, 4096);
			}
			else {
				SSL_write(pSSL, "PONG\n", 5);
				if (debug)
					Log("PONG");
			}
			continue;
		}

		RawLineParse(&raw, buffer_rx);
if (raw.text != NULL && raw.nick != NULL && strcmp(raw.command, "JOIN")!=0 &&
strcmp(raw.command, "NICK")!=0) {
		SlapCheck(&raw);
		GetTarget(&raw);
		if (strcmp(raw.text, "^about")==0) {
			sprintf(buffer_cmd, "privmsg %s :codybot(%s) is an IRC bot written in C by esselfe, "
				"sources @ https://github.com/cody1632/codybot\n", target, nick);	
			SSL_write(pSSL, buffer_cmd, strlen(buffer_cmd));
			Log(buffer_cmd);
			memset(buffer_cmd, 0, 4096);
		}
		else if (strcmp(raw.text, "^help")==0) {
			sprintf(buffer_cmd, "privmsg %s :commands: about codybot_version help fortune sh stats weather\n",
				target);
			SSL_write(pSSL, buffer_cmd, strlen(buffer_cmd));
			Log(buffer_cmd);
			memset(buffer_cmd, 0, 4096);
		}
		else if (strcmp(raw.text, "^fortune")==0)
			Fortune(&raw);
		else if (strcmp(raw.text, "^codybot_version")==0) {
			sprintf(buffer_cmd, "privmsg %s :codybot %s\n", target, codybot_version_string);
			SSL_write(pSSL, buffer_cmd, strlen(buffer_cmd));
			Log(buffer_cmd);
			memset(buffer_cmd, 0, 4096);
		}
		else if (strcmp(raw.text, "^debug on")==0) {
			debug = 1;
			Log("##debug on##");
		}
		else if (strcmp(raw.text, "^debug off")==0) {
			debug = 0;
			Log("##debug off##");
		}
		else if (strcmp(raw.text, "^stats")==0)
			Stats(&raw);
		else if (strcmp(raw.text, "^weather")==0) {
			sprintf(buffer_cmd, "privmsg %s :weather: missing argument, example: '^weather montreal'\n", target);
			SSL_write(pSSL, buffer_cmd, strlen(buffer_cmd));
			Log(buffer_cmd);
			memset(buffer_cmd, 0, 4096);
		}
		else if (raw.text[0]=='^' && raw.text[1]=='w' && raw.text[2]=='e' && raw.text[3]=='a' &&
			raw.text[4]=='t' && raw.text[5]=='h' && raw.text[6]=='e' && raw.text[7]=='r' && raw.text[8]==' ')
			Weather(&raw);
		else if (strcmp(raw.text, "^sh")==0) {
			sprintf(buffer_cmd, "privmsg %s :sh: missing argument, example: '^sh ls -ld /tmp'\n", target);
			SSL_write(pSSL, buffer_cmd, strlen(buffer_cmd));
			Log(buffer_cmd);
			memset(buffer_cmd, 0, 4096);
		}
		else if (raw.text[0]=='^' && raw.text[1]=='s' && raw.text[2]=='h' && raw.text[3]==' ') {
			char *cp = raw.text + 4;
// check for the kill command
unsigned int dontrun = 0;
while (1) {
	if (*cp == '\n' || *cp == '\0')
		break;
	else if (*cp == ' ') {
		++cp;
		continue;
	}
	else if (*cp=='k' && *(cp+1)=='i' && *(cp+2)=='l' && *(cp+3)=='l' && *(cp+4)==' ') {
		dontrun = 1;
		break;
	}

	++cp;
}
			if (dontrun) {
				Log("#### Will not run kill!\n");
				sprintf(raw.text, "^stats");
				continue;
			}
			cp = raw.text + 4;
			char cmd[1024];
			unsigned int cnt = 0;
			while (1) {
				if (*cp == '\n' || *cp == '\0') {
					cmd[cnt] = '\0';
					break;
				}
				cmd[cnt] = *cp;
				++cp;
				++cnt;
			}
			strcat(cmd, " &> cmd.output");
			Logx(cmd);
			system(cmd);

			FILE *fp = fopen("cmd.output", "r");
			if (fp == NULL) {
				fprintf(stderr, "\n##codybot error: Cannot open cmd.output: %s\n", strerror(errno));
				continue;
			}

			// count the line number
			char c;
			unsigned int lines_total = 0;
			while (1) {
				c = fgetc(fp);
				if (c == -1)
					break;
				else if (c == '\n')
					++lines_total;
			}
			fseek(fp, 0, SEEK_SET);

			GetTarget(&raw);
			if (lines_total == 1) {
				char result[4096];
				fgets(result, 4095, fp);
				sprintf(buffer_cmd, "privmsg %s :%s\n", target, result);
				SSL_write(pSSL, buffer_cmd, strlen(buffer_cmd));
				Log(buffer_cmd);
				memset(buffer_cmd, 0, 4096);
			}
			else {
				system("cat cmd.output |nc termbin.com 9999 > cmd.url");
				FILE *fp2 = fopen("cmd.url", "r");
				if (fp2 == NULL) {
					fprintf(stderr, "##codybot error: Cannot open cmd.url: %s\n", strerror(errno));
				}
				else {
					char url[1024];
					fgets(url, 1023, fp2);
					fclose(fp2);
					sprintf(buffer_cmd, "privmsg %s :%s\n", target, url);
					SSL_write(pSSL, buffer_cmd, strlen(buffer_cmd));
					Log(buffer_cmd);
					memset(buffer_cmd, 0, 4096);
				}
			}

			fclose(fp);

			memset(raw.text, 0, strlen(raw.text));
		}

		usleep(10000);
}
	}
	return NULL;
}

void ReadCommandLoop(void) {
	while (!endmainloop) {
		memset(buffer_cmd, 0, 4096);
		fgets(buffer_cmd, 4095, stdin);
		if (buffer_cmd[0] == '\n')
			continue;
		else if (strcmp(buffer_cmd, "exit")==0 || strcmp(buffer_cmd, "quit")==0)
			endmainloop = 1;
		else if (buffer_cmd[0]=='i' && buffer_cmd[1]=='d' && buffer_cmd[2]==' ') {
			char *cp;
			cp = buffer_cmd+3;
			char pass[1024];
			unsigned int cnt = 0;
			while (1) {
				pass[cnt] = *cp;

				++cnt;
				++cp;
				if (*cp == '\n' || *cp == '\0')
					break;
			}
			pass[cnt] = '\0';

			sprintf(buffer_cmd, "privmsg nickserv :identify %s\n", pass);
			SSL_write(pSSL, buffer_cmd, strlen(buffer_cmd));
			Log(buffer_cmd);
			memset(buffer_cmd, 0, 4096);
		}
		else {
			SSL_write(pSSL, buffer_cmd, strlen(buffer_cmd));
			Log(buffer_cmd);
			memset(buffer_cmd, 0, 4096);
		}
	}
}

void ConnectClient(void) {
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0) {
		fprintf(stderr, "||codybot error: Cannot socket(): %s\n", strerror(errno));
		exit(1);
	}
	else {
		if (debug)
			printf("||socket_fd: %d\n", socket_fd);
	}
	
	struct sockaddr_in addr;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_family = AF_INET;
	if (server_ip == server_ip_blinkenshell)
		addr.sin_port = htons(16423);
	else
		addr.sin_port = htons(16424);
	if (bind(socket_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		fprintf(stderr, "||codybot error: Cannot bind(): %s\n", strerror(errno));
		close(socket_fd);
		exit(1);
	}
	else {
		if (debug)
			printf("||bind() 0.0.0.0 successful\n");
	}
	
	struct sockaddr_in host;
	host.sin_addr.s_addr = inet_addr(server_ip);
	host.sin_family = AF_INET;
	host.sin_port = htons(server_port);
	if (connect(socket_fd, (struct sockaddr *)&host, sizeof(host)) < 0) {
		fprintf(stderr, "||codybot error: Cannot connect(): %s\n", strerror(errno));
		close(socket_fd);
		exit(1);
	}
	else {
		if (debug)
			printf("||connect() %s successful\n", server_ip);
	}

	SSL_load_error_strings();
	SSL_library_init();
	OpenSSL_add_all_algorithms();

	const SSL_METHOD *method = TLS_method();
	SSL_CTX *ctx = SSL_CTX_new(method);
	if (!ctx) {
		fprintf(stderr, "||codybot error: Cannot create SSL context\n");
		close(socket_fd);
		exit(1);
	}
	long opt_ctx;
	if (debug) {
		opt_ctx = SSL_CTX_get_options(ctx);
		printf("||opt_ctx: %ld\n", opt_ctx);
		SSL_CTX_set_options(ctx, SSL_OP_NO_COMPRESSION);
		opt_ctx = SSL_CTX_get_options(ctx);
		printf("||opt_ctx: %ld\n", opt_ctx);
		SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, 0);
	}
	else
		SSL_CTX_set_options(ctx, SSL_OP_NO_COMPRESSION);

	pSSL = SSL_new(ctx);
	long opt_ssl;
	if (debug) {
		opt_ssl = SSL_get_options(pSSL);
		printf("||opt_ssl: %ld\n", opt_ssl);
		SSL_set_options(pSSL, SSL_OP_NO_COMPRESSION);
		opt_ssl = SSL_get_options(pSSL);
		printf("||opt_ssl: %ld\n", opt_ssl);
		SSL_set_fd(pSSL, socket_fd);
		opt_ssl = SSL_get_options(pSSL);
		printf("||opt_ssl: %ld\n", opt_ssl);
	}
	else
		SSL_set_options(pSSL, SSL_OP_NO_COMPRESSION);

	BIO *bio = BIO_new_socket(socket_fd, BIO_CLOSE);
	SSL_set_bio(pSSL, bio, bio);
	SSL_connect(pSSL);
	ret = SSL_accept(pSSL);
	if (ret <= 0) {
		fprintf(stderr, "||codybot error: SSL_accept() failed, ret: %d\n", ret);
		fprintf(stderr, "||SSL error number: %d\n", SSL_get_error(pSSL, 0));
		close(socket_fd);
		exit(1);
	}

	// start the thread here, as soon as we can
	pthread_t thr;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&thr, &attr, RXThreadFunc, NULL);
	pthread_detach(thr);
	pthread_attr_destroy(&attr);

	sprintf(buffer_cmd, "PASS none\n");
	SSL_write(pSSL, buffer_cmd, strlen(buffer_cmd));
	Log(buffer_cmd);

	sprintf(buffer_cmd, "NICK %s\n", nick);
	SSL_write(pSSL, buffer_cmd, strlen(buffer_cmd));
	Log(buffer_cmd);

	if (server_ip == server_ip_freenode) {
		sprintf(buffer_cmd, "USER %s %s irc.freenode.net %s\n", nick, hostname, FullUserName);
		SSL_write(pSSL, buffer_cmd, strlen(buffer_cmd));
	}
	else {
		sprintf(buffer_cmd, "USER %s %s irc.blinkenshell.org :%s\n", nick, hostname, FullUserName);
		SSL_write(pSSL, buffer_cmd, strlen(buffer_cmd));
	}
	Log(buffer_cmd);
	memset(buffer_cmd, 0, 4096);
}

void GetServerIP(char *hostname) {
	struct hostent *he;
	struct in_addr **addr_list;
	int cnt = 0;

	he = gethostbyname(hostname);
	if (he == NULL) {
		fprintf(stderr, "##codybot error: Cannot gethostbyname()\n");
		exit(1);
	}

	addr_list = (struct in_addr **)he->h_addr_list;

	char *tmpstr = inet_ntoa(*addr_list[0]);
	server_ip = (char *)malloc(strlen(tmpstr)+1);
	sprintf(server_ip, "%s", tmpstr);

	if (debug) {
		for (cnt = 0; addr_list[cnt] != NULL; cnt++) {
			printf("%s\n", inet_ntoa(*addr_list[cnt]));
		}
	}
}

void SignalFunc(int signum) {
	close(socket_fd);
}

int main(int argc, char **argv) {
	signal(SIGINT, SignalFunc);
	int c;
	while (1) {
		c = getopt_long(argc, argv, short_options, long_options, NULL);
		if (c == -1)
			break;
		switch (c) {
		case 'h':
			HelpShow();
			exit(0);
		case 'V':
			printf("codybot %s\n", codybot_version_string);
			exit(0);
		case 'd':
			debug = 1;
			break;
		case 'b':
			server_ip = server_ip_blinkenshell;
			break;
		case 'f':
			server_ip = server_ip_freenode;
			break;
		case 'n':
			nick = (char *)malloc(strlen(optarg)+1);
			sprintf(nick, "%s", optarg);
			break;
		case 'p':
			server_port = atoi(optarg);
			break;
		case 's':
			// set server addr
			GetServerIP(optarg);
			break;
		default:
			fprintf(stderr, "codybot error: Unknown argument: %c/%d\n", (char)c, c);
			break;
		}
	}

	if (!nick) {
		nick = (char *)malloc(strlen("codybot")+1);
		sprintf(nick, "codybot");
	}
	if (!server_port)
		server_port = 6697;
	if (!server_ip)
		server_ip = server_ip_freenode;

	buffer_rx = (char *)malloc(4096);
	memset(buffer_rx, 0, 4096);
	buffer_cmd = (char *)malloc(4096);
	memset(buffer_cmd, 0, 4096);
	buffer_log = (char *)malloc(4096);
	memset(buffer_log, 0, 4096);
	buffer = (char *)malloc(4096);
	memset(buffer, 0, 4096);
	
	FILE *fp = fopen("stats", "r");
	if (fp == NULL) {
		fprintf(stderr, "##codybot error: Cannot open stats file\n");
	}
	else {
		char str[1024];
		fgets(str, 1023, fp);
		fclose(fp);
		fortune_total = atoi(str);
	}
	if (debug)
		printf("##fortune_total: %llu\n", fortune_total);

	ConnectClient();
	ReadCommandLoop();

	SSL_shutdown(pSSL);
	SSL_free(pSSL);
	ret = close(socket_fd);

	return 0;
}

