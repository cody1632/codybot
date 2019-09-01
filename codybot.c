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
#include <sys/stat.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "codybot.h"

const char *codybot_version_string = "0.2.1";

static const struct option long_options[] = {
	{"help", no_argument, NULL, 'h'},
	{"version", no_argument, NULL, 'V'},
	{"blinkenshell", no_argument, NULL, 'b'},
	{"debug", no_argument, NULL, 'd'},
	{"freenode", no_argument, NULL, 'f'},
	{"hostname", required_argument, NULL, 'H'},
	{"log", required_argument, NULL, 'l'},
	{"fullname", required_argument, NULL, 'N'},
	{"nick", required_argument, NULL, 'n'},
	{"localport", required_argument, NULL, 'P'},
	{"port", required_argument, NULL, 'p'},
	{"server", required_argument, NULL, 's'},
	{NULL, 0, NULL, 0}
};
static const char *short_options = "hVdbfH:l:N:n:P:p:s:";

void HelpShow(void) {
	printf("Usage: codybot { -h/--help | -V/--version | -b/--blinkenshell | -f/--freenode }\n");
	printf("               { -d/--debug | -H/--hostname HOST | -l/--log FILENAME | -N/--fullname NAME }\n");
	printf("               { -n/--nick NICK | -P/--localport PORTNUM | -p/--port PORTNUM | -s/--server ADDR }\n");
}

int debug, socket_fd, ret, endmainloop, sh_disabled;
unsigned long long fortune_total;
struct timeval tv0;
struct tm *tm0;
time_t t0;
char *log_filename;
char *buffer, *buffer_rx, *buffer_cmd, *buffer_log;
char *nick;
char *full_user_name;
char *hostname;
char *target;
struct raw_line raw;

void Log(char *text) {
	FILE *fp = fopen(log_filename, "a+");
	if (fp == NULL) {
		fprintf(stderr, "##codybot::Log() error: Cannot open %s: %s\n", log_filename, strerror(errno));
		return;
	}

	gettimeofday(&tv0, NULL);
	t0 = (time_t)tv0.tv_sec;
	tm0 = gmtime(&t0);
	sprintf(buffer_log, "%02d:%02d:%02d.%03ld ##%s##\n", tm0->tm_hour, tm0->tm_min, tm0->tm_sec,
		tv0.tv_usec, text);
	fputs(buffer_log, fp);
	fputs(buffer_log, stdout);
	memset(buffer_log, 0, 4096);

	fclose(fp);
}

void Logr(char *text) {
	FILE *fp = fopen(log_filename, "a+");
	if (fp == NULL) {
		fprintf(stderr, "##codybot::Logr() error: Cannot open %s: %s\n", log_filename, strerror(errno));
		return;
	}

	gettimeofday(&tv0, NULL);
	t0 = (time_t)tv0.tv_sec;
	tm0 = gmtime(&t0);
	sprintf(buffer_log, "%02d:%02d:%02d.%03ld <<%s>>\n", tm0->tm_hour, tm0->tm_min, tm0->tm_sec,
		tv0.tv_usec, text);
	fputs(buffer_log, fp);
	fputs(buffer_log, stdout);
	memset(buffer_log, 0, 4096);

	fclose(fp);
}

void Logx(char *text) {
	FILE *fp = fopen(log_filename, "a+");
	if (fp == NULL) {
		fprintf(stderr, "##codybot::Logx() error: Cannot open %s: %s\n", log_filename, strerror(errno));
		return;
	}

	gettimeofday(&tv0, NULL);
	t0 = (time_t)tv0.tv_sec;
	tm0 = gmtime(&t0);
	sprintf(buffer_log, "%02d:%02d:%02d.%03ld $$%s$$\n", tm0->tm_hour, tm0->tm_min, tm0->tm_sec,
		tv0.tv_usec, text);
	fputs(buffer_log, fp);
	fputs(buffer_log, stdout);
	memset(buffer_log, 0, 4096);

	fclose(fp);
}

void RawLineClear(struct raw_line *rawp) {
	memset(rawp->nick, 0, 1024);
	memset(rawp->username, 0, 1024);
	memset(rawp->host, 0, 1024);
	memset(rawp->command, 0, 1024);
	memset(rawp->channel, 0, 1024);
	memset(rawp->text, 0, 4096);	
}

// :esselfe!~bsfc@unaffiliated/esselfe PRIVMSG #codybot :^codybot_version
void RawLineParse(struct raw_line *rawp, char *line) {
// Getting a double free error with this, weird...
/*	if (raw->nick) free(raw->nick);
	if (raw->username) free(raw->username);
	if (raw->host) free(raw->host);
	if (raw->command) free(raw->command);
	if (raw->channel) free(raw->channel);
	if (raw->text) free(raw->text); */
	RawLineClear(rawp);

	char *c = line;
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
// :esselfe!~bsfc@unaffiliated/esselfe PRIVMSG #codybot :!codybot_version
		else if (rec_channel && *c == ' ') {
			sprintf(rawp->channel, "%s", word);
			memset(word, 0, 4096);
			rec_channel = 0;
			if (strcmp(rawp->command, "PRIVMSG")==0)
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

// ":esselfe!~codybot@unaffiliated/codybot PRIVMSG ##c-offtopic :message"
// ":esselfe!~codybot@unaffiliated/codybot PRIVMSG SpringSprocket :message"
// If sender sends to channel, set target to channel
// If sender sends to PM/nick, set target to nick
char *GetTarget(struct raw_line *rawp) {
	if (strcmp(rawp->channel, nick)==0)
		target = rawp->nick;
	else
		target = rawp->channel;
	return target;
}

// function to process messages received from server
void *ThreadRXFunc(void *argp) {
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
		// respond to ping request from the server with a pong
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
		if (strcmp(raw.text, "^ascii")==0)
			AsciiArt(&raw);
		else if (strcmp(raw.text, "^about")==0) {
			sprintf(buffer_cmd, "privmsg %s :codybot(%s) is an IRC bot written in C by esselfe, "
				"sources @ https://github.com/cody1632/codybot\n", target, nick);	
			SSL_write(pSSL, buffer_cmd, strlen(buffer_cmd));
			Log(buffer_cmd);
			memset(buffer_cmd, 0, 4096);
		}
		else if (strcmp(raw.text, "^chars")==0)
			Chars(&raw);
		else if (strcmp(raw.text, "^help")==0) {
			sprintf(buffer_cmd, "privmsg %s :commands: ^about ^ascii ^chars ^codybot_version ^help"
				" ^fortune ^joke ^sh ^stats ^weather\n", target);
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
		else if(strcmp(raw.text, "^joke")==0)
			Joke(&raw);
		else if (strcmp(raw.text, "^stats")==0)
			Stats(&raw);
		else if (strcmp(raw.text, "^weather")==0) {
			sprintf(buffer_cmd, "privmsg %s :weather: missing city argument, example: '^weather montreal'\n", target);
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
			if (strcmp(raw.nick, "esselfe")!=0) {
			if (sh_disabled) {
				sprintf(buffer_cmd, 
					"privmsg %s :%s: sh is temporarily disabled, try again later or ask esselfe to enable it\n",
					target, raw.nick);
				SSL_write(pSSL, buffer_cmd, strlen(buffer_cmd));
				Log(buffer_cmd);
				memset(buffer_cmd, 0, 4096);
				continue;
			}
			struct stat st;
			if (stat("sh_disable", &st) == 0) {
				sprintf(buffer_cmd,
					"privmsg %s :%s: sh is temporarily disabled, try again later or ask esselfe to enable it\n",
					target, raw.nick);
				SSL_write(pSSL, buffer_cmd, strlen(buffer_cmd));
				Log(buffer_cmd);
				memset(buffer_cmd, 0, 4096);
				continue;
			}
			}

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
			sprintf(cmd, "timeout 30s ");
			unsigned int cnt = strlen(cmd);
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
				fprintf(stderr, "\n##codybot::ThreadRXFunc() error: Cannot open cmd.output: %s\n", strerror(errno));
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
			if (lines_total <= 9) {
				char *retstr;
				char result[4096];
				while (1) {
					retstr = fgets(result, 4095, fp);
					if (retstr == NULL) break;
					sprintf(buffer_cmd, "privmsg %s :%s\n", target, result);
					SSL_write(pSSL, buffer_cmd, strlen(buffer_cmd));
					Log(buffer_cmd);
					memset(buffer_cmd, 0, 4096);
				}
			}
			else if (lines_total > 10) {
				system("cat cmd.output |nc termbin.com 9999 > cmd.url");
				FILE *fp2 = fopen("cmd.url", "r");
				if (fp2 == NULL) {
					fprintf(stderr, "##codybot::ThreadRXFunc() error: Cannot open cmd.url: %s\n", strerror(errno));
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
			//system("rm cmd.output cmd.url 2>/dev/null");

			fclose(fp);

			RawLineClear(&raw);
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
		else if (strcmp(buffer_cmd, "exit\n")==0 || strcmp(buffer_cmd, "quit\n")==0)
			endmainloop = 1;
		else if (strcmp(buffer_cmd, "debug on\n")==0) {
			debug = 1;
			printf("##debug on\n");
		}
		else if (strcmp(buffer_cmd, "debug off\n")==0) {
			debug = 0;
			printf("##debug off\n");
		}
		else if (strcmp(buffer_cmd, "sh_disable\n")==0) {
			sh_disabled = 1;
			printf("##sh disabled\n");
		}
		else if (strcmp(buffer_cmd, "sh_enable\n")==0) {
			sh_disabled = 0;
			printf("##sh enabled\n");
		}
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
			memset(buffer_cmd, 0, 4096);
		}
		else {
			SSL_write(pSSL, buffer_cmd, strlen(buffer_cmd));
			Log(buffer_cmd);
			memset(buffer_cmd, 0, 4096);
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
		case 'H':
			hostname = (char *)malloc(strlen(optarg)+1);
			sprintf(hostname, optarg);
			break;
		case 'l':
			log_filename = (char *)malloc(strlen(optarg)+1);
			sprintf(log_filename, "%s", optarg);
			break;
		case 'N':
			full_user_name = (char *)malloc(strlen(optarg)+1);
			sprintf(full_user_name, optarg);
			break;
		case 'n':
			nick = (char *)malloc(strlen(optarg)+1);
			sprintf(nick, "%s", optarg);
			break;
		case 'P':
			local_port = atoi(optarg);
			break;
		case 'p':
			server_port = atoi(optarg);
			break;
		case 's':
			ServerGetIP(optarg);
			break;
		default:
			fprintf(stderr, "##codybot::main() error: Unknown argument: %c/%d\n", (char)c, c);
			break;
		}
	}

	if (!full_user_name) {
		char *name = getlogin();
		full_user_name = (char *)malloc(strlen(name)+1);
		sprintf(full_user_name, name);
	}
	if (!hostname) {
		hostname = (char *)malloc(1024);
		gethostname(hostname, 1023);
	}
	if (!local_port)
		local_port = 16384;
	if (!log_filename) {
		log_filename = (char *)malloc(strlen("codybot.log")+1);
		sprintf(log_filename, "codybot.log");
	}
	if (!nick) {
		nick = (char *)malloc(strlen("codybot")+1);
		sprintf(nick, "codybot");
	}
	if (!server_port)
		server_port = 6697;
	if (!server_ip)
		server_ip = server_ip_freenode;

	raw.nick = (char *)malloc(1024);
	raw.username = (char *)malloc(1024);
	raw.host = (char *)malloc(1024);
	raw.command = (char *)malloc(1024);
	raw.channel = (char *)malloc(1024);
	raw.text = (char *)malloc(4096);

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
		fprintf(stderr, "##codybot::main() error: Cannot open stats file\n");
	}
	else {
		char str[1024];
		fgets(str, 1023, fp);
		fclose(fp);
		fortune_total = atoi(str);
	}
	if (debug)
		printf("##fortune_total: %llu\n", fortune_total);

	ServerConnect();
	pthread_t thr;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&thr, &attr, ThreadRXFunc, NULL);
    pthread_detach(thr);
    pthread_attr_destroy(&attr);
	ReadCommandLoop();
	ServerClose();

	return 0;
}

