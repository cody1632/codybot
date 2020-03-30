#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "codybot.h"

const char *codybot_version_string = "0.2.24";

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
	{"trigger", required_argument, NULL, 't'},
	{NULL, 0, NULL, 0}
};
static const char *short_options = "hVdbfH:l:N:n:P:p:s:t:";

void HelpShow(void) {
	printf("Usage: codybot { -h/--help | -V/--version | -b/--blinkenshell | -f/--freenode | -d/--debug }\n");
	printf("               { -H/--hostname HOST | -l/--log FILENAME | -N/--fullname NAME | -n/--nick NICK }\n");
	printf("               { -P/--localport PORTNUM | -p/--port PORTNUM | -s/--server ADDR | -t/--trigger CHAR }\n");
}

int debug, socket_fd, ret, endmainloop, cc_disabled, sh_disabled,
	sh_locked, wttr_disabled, cmd_timeout = 10, use_ssl;
unsigned long long fortune_total;
struct timeval tv0, tv_start;
struct tm *tm0;
time_t t0;
char *log_filename;
char *buffer, *buffer_rx, *buffer_cmd, *buffer_log;
char trigger_char, trigger_char_default = ',';
char *current_channel;
char *nick, // nick used by the bot
	*nick_admin = "esselfe"; // change this to your usual/normal nick for controlling the bot
char *full_user_name;
char *hostname;

void Log(char *text) {
	FILE *fp = fopen(log_filename, "a+");
	if (fp == NULL) {
		fprintf(stderr, "##codybot::Log() eriror: Cannot open %s: %s\n",
			log_filename, strerror(errno));
		return;
	}

	gettimeofday(&tv0, NULL);
	t0 = (time_t)tv0.tv_sec;
	tm0 = gmtime(&t0);
	char *str = strdup(text);
	// remove trailing newline
	if (str[strlen(str)-1] == '\n')
		str[strlen(str)-1] = '\0';
	if (str[strlen(str)] == '\n')
		str[strlen(str)] = '\0';

	sprintf(buffer_log, "%02d%02d%02d-%02d:%02d:%02d.%03ld ##%s##\n",
		tm0->tm_year+1900-2000, tm0->tm_mon+1,
		tm0->tm_mday, tm0->tm_hour, tm0->tm_min, tm0->tm_sec, tv0.tv_usec, str);
	fputs(buffer_log, fp);

	sprintf(buffer_log, "\e[00;36m%02d%02d%02d-%02d:%02d:%02d.%03ld ##\e[00m%s\e[00;36m##\e[00m\n", 
		tm0->tm_year+1900-2000, tm0->tm_mon+1,
		tm0->tm_mday, tm0->tm_hour, tm0->tm_min, tm0->tm_sec, tv0.tv_usec, str);
	fputs(buffer_log, stdout);
	
	memset(buffer_log, 0, 4096);

	fclose(fp);
}

void Msg(char *text) {
	unsigned int total_len = strlen(text);
	if (total_len <= 400) {
		sprintf(buffer_log, "PRIVMSG %s :%s\n", target, text);
		if (use_ssl)
			SSL_write(pSSL, buffer_log, strlen(buffer_log));
		else
			write(socket_fd, buffer_log, strlen(buffer_log));
		Log(buffer_log);
		memset(buffer_log, 0, 4096);
	}
	else if (total_len > 400) {
		char str[400], *cp = text;
		unsigned int cnt, cnt2 = 0;
		memset(str, 0, 400);
		sprintf(str, "PRIVMSG %s :", target);
		cnt = strlen(str);
		while (1) {
			str[cnt] = *(cp+cnt2);
			++cnt;
			++cnt2;
			if (cnt2 >= total_len) {
				str[cnt] = '\n';
				str[cnt+1] = '\0';
				if (use_ssl)
					SSL_write(pSSL, str, strlen(str));
				else
					write(socket_fd, str, strlen(str));
				Log(str);
				memset(str, 0, 400);
				break;
			}
			else if (cnt >= 399) {
				str[cnt] = '\n';
				str[cnt+1] = '\0';
				if (use_ssl)
					SSL_write(pSSL, str, strlen(str));
				else
					write(socket_fd, str, strlen(str));
				Log(str);
				memset(str, 0, 400);
				sprintf(str, "PRIVMSG %s :", target);
				cnt = strlen(str);
			}
		}
		return;
	}
}

void ReadCommandLoop(void) {
	char buffer_line[4096];
	while (!endmainloop) {
		memset(buffer_line, 0, 4096);
		fgets(buffer_line, 4095, stdin);
		char *cp;
		cp = buffer_line;
		if (buffer_line[0] == '\n')
			continue;
		else if (strcmp(buffer_line, "exit\n")==0 || strcmp(buffer_line, "quit\n")==0)
			endmainloop = 1;
		else if (strncmp(buffer_line, "cursh", 5)==0) {
			sprintf(current_channel, "%s", buffer_line+6);
		}
		else if (strcmp(buffer_line, "debug on\n")==0) {
			debug = 1;
			Msg("debug = 1");
		}
		else if (strcmp(buffer_line, "debug off\n")==0) {
			debug = 0;
			Msg("debug = 0");
		}
		else if (strncmp(buffer_line, "msg", 3)==0) {
			sprintf(buffer, "%s", buffer_line+4);
			sprintf(raw.channel, "%s", current_channel);
			target = raw.channel;
			Msg(buffer);
		}
		else if (strcmp(buffer_line, "sh_disable\n")==0) {
			sh_disabled = 1;
			Msg("sh_disabled = 1");
		}
		else if (strcmp(buffer_line, "sh_enable\n")==0) {
			sh_disabled = 0;
			Msg("sh_disabled = 1");
		}
		else if (strcmp(buffer_line, "sh_lock\n")==0) {
			sh_locked = 1;
			Msg("sh_locked = 1");
		}
		else if (strcmp(buffer_line, "sh_unlock\n")==0) {
			sh_locked = 0;
			Msg("sh_locked = 0");
		}
		else if (strncmp(buffer_line, "id ", 3) == 0) {
			char *cp;
			cp = buffer_line+3;
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

			sprintf(buffer_cmd, "PRIVMSG NickServ :identify %s\n", pass);
			if (use_ssl)
				SSL_write(pSSL, buffer_cmd, strlen(buffer_cmd));
			else
				write(socket_fd, buffer_cmd, strlen(buffer_cmd));
			Log("PRIVMSG NickServ :identify *********");
			memset(buffer_cmd, 0, 4096);
		}
		else if (strncmp(buffer_line, "timeout\n", 8) == 0) {
			sprintf(buffer, "timeout = %d", cmd_timeout);
			Msg(buffer);
		}
		else if (strncmp(buffer_line, "timeout ", 8) == 0) {
			char str[1024];
			memset(str, 0, 1024);
			unsigned int cnt = 0;
			cp += 8;
			while (1) {
				if (*cp=='\n' || *cp=='\0') {
					str[cnt] = '\0';
					break;
				}
				else {
					str[cnt] = *cp;
					++cp;
					++cnt;
				}
			}
			cmd_timeout = atoi(str);
			if (cmd_timeout == 0)
				cmd_timeout = 10;
			sprintf(buffer, "timeout = %d\n", cmd_timeout);
			Msg(buffer);
		}
		else if (strncmp(buffer_line, "trigger ", 8) == 0) {
			if (*(cp+8)!='\n')
				trigger_char = *(cp+8);
			sprintf(buffer, "trigger_char = %c", trigger_char);
			Msg(buffer);
		}
		else {
			if (use_ssl)
				SSL_write(pSSL, buffer_line, strlen(buffer_line));
			else
				write(socket_fd, buffer_line, strlen(buffer_line));
			memset(buffer_line, 0, 4096);
		}
	}
}

void SignalFunc(int signum) {
	ServerClose();
	close(socket_fd);
}

int main(int argc, char **argv) {
	gettimeofday(&tv_start, NULL);
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
			if (server_port == 6697)
				use_ssl = 1;
			break;
		case 's':
			ServerGetIP(optarg);
			break;
		case 't':
			trigger_char = *optarg;
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
	if (!log_filename) {
		log_filename = (char *)malloc(strlen("codybot.log")+1);
		sprintf(log_filename, "codybot.log");
	}
	if (!trigger_char)
		trigger_char = trigger_char_default;
	if (!current_channel) {
		current_channel = malloc(1024);
		sprintf(current_channel, "#codybot");
	}
	if (!nick) {
		nick = (char *)malloc(strlen("codybot")+1);
		sprintf(nick, "codybot");
	}
	if (!server_port) {
		server_port = 6697;
		use_ssl = 1;
	}
	if (!server_ip)
		server_ip = server_ip_freenode;

	if (strcmp(server_ip, server_ip_blinkenshell)==0) {
		sh_disabled = 1;
		cc_disabled = 1;
	}

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

	struct stat st;
	if(stat("sh_locked", &st)==0)
		sh_locked = 1;
	
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
	ThreadRXStart();
	ReadCommandLoop();
	ServerClose();

	return 0;
}

