#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/stat.h>

#include "codybot.h"

const char *codybot_version_string = "1.0.6";

static const struct option long_options[] = {
	{"help", no_argument, NULL, 'h'},
	{"version", no_argument, NULL, 'V'},
	{"compiler", required_argument, NULL, 'c'},
	{"debug", no_argument, NULL, 'd'},
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
static const char *short_options = "hVcdH:l:N:n:P:p:s:t:";

void HelpShow(void) {
	printf("Usage: codybot { -h/--help | -V/--version | -d/--debug }\n");
	printf("			   { -H/--hostname HOST | -l/--log FILENAME | -N/--fullname NAME | -n/--nick NICK }\n");
	printf("			   { -P/--localport PORTNUM | -p/--port PORTNUM | -s/--server ADDR | -t/--trigger CHAR }\n");
}

int debug, socket_fd, ret, endmainloop, cc_disabled, sh_disabled,
	sh_locked, wttr_disabled, cmd_timeout = 10, use_ssl, cc_compiler = CC_COMPILER_GCC;
unsigned long long fortune_total;
struct timeval tv0, tv_start;
struct tm *tm0;
time_t t0, ctcp_prev_time;
char ctcp_prev_nick[128];
char *log_filename;
char *buffer, *buffer_rx, *buffer_cmd, *buffer_log;
char trigger_char, trigger_char_default = '!';
char *current_channel;
char *nick; // nick used by the bot
char *full_user_name;
char *hostname;
char *colors[] = {
	"\003",	// default/restore
	"\00301",  //black
//	"\00302",  // blue --> too dark/unreadable (OS-specific)
	"\00303",  // green
	"\00304",  // red
	"\00305",  // brown
	"\00306",  // purple
	"\00307",  // orange
	"\00308",  // yellow
	"\00309",  // light green
	"\00310",  // cyan
	"\00311",  // light cyan
	"\00312",  // light blue
	"\00313",  // pink
	"\00314",  // grey
	"\00315"}; // light grey

static void CheckLoginuid(void) {
	FILE *fp = fopen("/proc/self/loginuid", "r");
	if (fp == NULL) {
		sprintf(buffer, "codybot::CheckLoginuid() error: "
			"Cannot open /proc/self/loginuid: %s",
			strerror(errno));
		Log(buffer);
		return;
	}

	char line[128];
	fgets(line, 128, fp);
	int uid = atoi(line);
	fclose(fp);
	fp = fopen("/proc/self/loginuid", "w");
	if (fp == NULL) {
		sprintf(buffer, "codybot::CheckLoginuid() error: "
			"Cannot open /proc/self/loginuid: %s",
			strerror(errno));
		Log(buffer);
		return;
	}
	if (strcmp(line, "4294967295") == 0 && uid == -1) {
		uid = getuid();
		sprintf(buffer, "/proc/self/loginuid is 4294967295/-1, setting to %d", uid);
		Log(buffer);
		fprintf(fp, "%d", uid);
	}

	fclose(fp);

	if (debug)
		system("echo \"loginuid: $(cat /proc/self/loginuid)\"");
}

void SignalFunc(int signum) {
	if (signum == SIGINT)
		ServerClose();
}

int main(int argc, char **argv) {
	// initialize time for !uptime
	gettimeofday(&tv_start, NULL);

	ctcp_prev_time = tv_start.tv_sec - 5;
	sprintf(ctcp_prev_nick, "codybot");

	signal(SIGINT, SignalFunc);

	int c;
	while (1) {
		c = getopt_long(argc, argv, short_options, long_options, NULL);
		if (c == -1)
			break;
		switch (c) {
		case 'h': // --help
			HelpShow();
			exit(0);
		case 'V': // --version
			printf("codybot %s\n", codybot_version_string);
			exit(0);
		case 'c': // --compiler
			if (strcmp(optarg, "gcc") == 0)
				cc_compiler = CC_COMPILER_GCC;
			else if (strcmp(optarg, "tcc") == 0)
				cc_compiler = CC_COMPILER_TCC;
			else {
				printf("codybot error: the compiler must be gcc or tcc\n");
				exit(1);
			}
			break;
		case 'd': // --debug
			debug = 1;
			break;
		case 'H': // --hostname
			hostname = (char *)malloc(strlen(optarg)+1);
			sprintf(hostname, optarg);
			break;
		case 'l': // --log
			log_filename = (char *)malloc(strlen(optarg)+1);
			sprintf(log_filename, "%s", optarg);
			break;
		case 'N': // --fullname
			full_user_name = (char *)malloc(strlen(optarg)+1);
			sprintf(full_user_name, optarg);
			break;
		case 'n': // --nick
			nick = (char *)malloc(strlen(optarg)+1);
			sprintf(nick, "%s", optarg);
			break;
		case 'P': // --localport
			local_port = atoi(optarg);
			break;
		case 'p': // --port
			server_port = atoi(optarg);
			if (server_port == 6697)
				use_ssl = 1;
			break;
		case 's': // --server
			ServerGetIP(optarg);
			break;
		case 't': // --trigger
			trigger_char = *optarg;
			break;
		default:
			fprintf(stderr, "##codybot::main() error: Unknown argument: %c/%d\n", (char)c, c);
			break;
		}
	}

	// Initialize buffers.
	buffer_rx = (char *)malloc(4096);
	memset(buffer_rx, 0, 4096);
	buffer_cmd = (char *)malloc(4096);
	memset(buffer_cmd, 0, 4096);
	buffer_log = (char *)malloc(4096);
	memset(buffer_log, 0, 4096);
	buffer = (char *)malloc(4096);
	memset(buffer, 0, 4096);

	if (!log_filename) {
		log_filename = (char *)malloc(strlen("codybot.log")+1);
		sprintf(log_filename, "codybot.log");
	}

	CheckLoginuid();
	ParseAdminFile();

	if (!full_user_name) {
		char *name = getlogin();
		full_user_name = (char *)malloc(strlen(name)+1);
		sprintf(full_user_name, name);
	}
	if (!hostname) {
		hostname = (char *)malloc(1024);
		gethostname(hostname, 1023);
	}
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
		ServerGetIP("chat.freenode.net");
	if (!trigger_char)
		trigger_char = trigger_char_default;

	if (strcmp(server_name, "irc.blinkenshell.org") == 0) {
		// Don't wanna replace blinken's shell service
		// Note that those features are clearly refused by the owner
		sh_disabled = 1;
		cc_disabled = 1;
	}

	raw.nick = (char *)malloc(1024);
	raw.username = (char *)malloc(1024);
	raw.host = (char *)malloc(1024);
	raw.command = (char *)malloc(1024);
	raw.channel = (char *)malloc(1024);
	raw.text = (char *)malloc(4096);

	struct stat st;
	if(stat("sh_locked", &st) == 0)
		sh_locked = 1;
	
	FILE *fp = fopen("stats", "r");
	if (fp == NULL) {
		fprintf(stderr, "##codybot::main() error: Cannot open stats file: %s\n",
			strerror(errno));
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

	// Mainloop
	ThreadRXStart();
	ConsoleReadInput();
	
	ServerClose();

	return 0;
}

