#ifndef CODYBOT_H
#define CODYBOT_H 1

#include <sys/time.h>
#include <openssl/ssl.h>

// To control which compiler !cc use. (via var cc_compiler)
#define CC_COMPILER_GCC 1
#define CC_COMPILER_TCC 2

// Globals from codybot.c
extern const char *codybot_version_string;
extern int debug, socket_fd, ret, endmainloop, cc_compiler, cc_disabled, sh_disabled, 
	sh_locked, wttr_disabled, cmd_timeout, use_ssl;
extern unsigned long long fortune_total; // set with $srcdir/stats content
extern struct timeval tv0, tv_start; // 0 sort of means now and "start" is program start time
extern struct tm *tm0;
extern time_t t0, ctcp_prev_time; // ctcp antiflood considerations
extern char ctcp_prev_nick[128];
extern char *log_filename; // $srcdir/codybot.log if not set, see main()
// 'buffer' is always for immediate use. 'rx' is for received raw data. 'cmd' is
// mostly used in system() calls. 'log' is used for data to $srcdir/codybot.log
extern char *buffer, *buffer_rx, *buffer_cmd, *buffer_log;
extern char trigger_char; // character which is prefixed to commands in IRC chat readout
extern char *current_channel; // used as changeable default channel for console 
                              // 'msg/fortune' cmd
extern char *nick; // running bot's IRC nick
extern char *full_user_name; // name displayed in IRC /whois responses
extern char *hostname;
extern char *target; // this one should be worked upon a little bit :P
                     // used to differentiate channel-wide vs private messages
extern char *colors[]; // IRC color codes used in '!colorize/!rainbow'

// Log() directions
#define LOCAL 0
#define IN 1
#define OUT 2

// Globals from server.c
extern unsigned int server_port, local_port;
extern char *server_ip, *server_name;
extern SSL *pSSL;

// Globals from raw.c
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
extern struct raw_line raw;

// from admin.c
struct Admin {
	struct Admin *prev, *next; // used to navigate the list in a loop
	char *nick, *host;
};

struct AdminList {
	unsigned int total_admins; // simple (unused yet???) counter
	struct Admin *first_admin, *last_admin; // loop startpoints
};
extern struct AdminList admin_list;

void AddAdmin(char *newnick, char *host);
void DestroyAdminList(void);
char *EnumerateAdmins(void);
int IsAdmin(char *newnick, char *host); // used to check who can run specific commands
void ParseAdminFile(void);

// from cmd-*.c
void AsciiArt(struct raw_line *rawp);
void Calc(struct raw_line *rawp);
void CC(struct raw_line *rawp);
void Chars(struct raw_line *rawp);
void Colorize(struct raw_line *rawp);
void Dict(struct raw_line *rawp);
void Foldoc(struct raw_line *rawp);
void Fortune(struct raw_line *rawp);
void Joke(struct raw_line *rawp);
void Rainbow(struct raw_line *rawp);
void SlapCheck(struct raw_line *rawp);
void Stats(struct raw_line *rawp);
void Weather(struct raw_line *rawp);

// from console.c
void ConsoleReadInput(void);

// from dict.c
void Dict(struct raw_line *rawp);

// from log.c
void Log(unsigned int direction, char *text);

// from msg.c
void Msg(char *text);
void MsgRaw(char *text);

// from raw.c
char *RawGetTarget(struct raw_line *rawp);
void RawLineClear(struct raw_line *rawp);
int RawLineParse(struct raw_line *rawp, char *line);
void RawMsg(struct raw_line *rawp);

// from server.c
void ServerGetIP(char *hostname);
void ServerConnect(void);
void ServerClose(void);

// from thread.c
void ThreadRunStart(char *command);
void *ThreadRunFunc(void *argp);
void ThreadRXStart(void);
void *ThreadRXFunc(void *argp);

#endif /* CODYBOT_H */
