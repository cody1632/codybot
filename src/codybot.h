#ifndef CODYBOT_H
#define CODYBOT_H 1

#include <sys/time.h>
#include <openssl/ssl.h>

#define CC_COMPILER_GCC 1
#define CC_COMPILER_TCC 2

// Globals from codybot.c
extern const char *codybot_version_string;
extern int debug, socket_fd, ret, endmainloop, cc_disabled, sh_disabled, 
	sh_locked, wttr_disabled, cmd_timeout, use_ssl, cc_compiler;
extern unsigned long long fortune_total;
extern struct timeval tv0, tv_start;
extern struct tm *tm0;
extern time_t t0, ctcp_prev_time;
extern char ctcp_prev_nick[128];
extern char *log_filename;
extern char *buffer, *buffer_rx, *buffer_cmd, *buffer_log;
extern char trigger_char;
extern char *current_channel;
extern char *nick;
extern char *full_user_name;
extern char *hostname;
extern char *target;
extern char *colors[];

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
	struct Admin *prev, *next;
	char *nick, *host;
};

struct AdminList {
	unsigned int total_admins;
	struct Admin *first_admin, *last_admin;
};
extern struct AdminList admin_list;

void AddAdmin(char *newnick, char *host);
void DestroyAdminList(void);
char *EnumerateAdmins(void);
int IsAdmin(char *newnick, char *host);
void ParseAdminFile(void);

// from cmd-*.c
void AsciiArt(struct raw_line *rawp);
void Calc(struct raw_line *rawp);
void CC(struct raw_line *rawp);
void Chars(struct raw_line *rawp);
void Colorize(struct raw_line *rawp);
void Dict(struct raw_line *rawp);
void Fortune(struct raw_line *rawp);
void Joke(struct raw_line *rawp);
void Rainbow(struct raw_line *rawp);
void SlapCheck(struct raw_line *rawp);
void Stats(struct raw_line *rawp);
void Weather(struct raw_line *rawp);

// from console.c
void ConsoleReadInput(void);

// from log.c
void Log(char *text);

// from msg.c
void Msg(char *text);
void MsgRaw(char *text);

// from raw.c
char *RawGetTarget(struct raw_line *rawp);
void RawLineClear(struct raw_line *rawp);
void RawLineParse(struct raw_line *rawp, char *line);

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
