#ifndef CODYBOT_H
#define CODYBOT_H 1

#include <sys/time.h>
#include <openssl/ssl.h>

// Globals from codybot.c
extern const char *codybot_version_string;
extern int debug, socket_fd, ret, endmainloop, sh_disabled, sh_locked, cmd_timeout, use_ssl;
extern unsigned long long fortune_total;
extern struct timeval tv0;
extern struct tm *tm0;
extern time_t t0;
extern char *log_filename;
extern char *buffer, *buffer_rx, *buffer_cmd, *buffer_log;
extern char trigger_char;
extern char *nick, *nick_admin;
extern char *full_user_name;
extern char *hostname;
extern char *target;

// Globals from server.c
extern unsigned int server_port, local_port;
extern char *server_ip;
extern char *server_ip_blinkenshell;
extern char *server_ip_freenode;
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

// from codybot.c
void Log(char *text);
void Logr(char *text);
void Logx(char *text);
void Msg(char *text);
void *ThreadRXFunc(void *argp);
// from commands.c
void AsciiArt(struct raw_line *rawp);
void Calc(struct raw_line *rawp);
void Chars(struct raw_line *rawp);
void Fortune(struct raw_line *rawp);
void Joke(struct raw_line *rawp);
void Rainbow(struct raw_line *rawp);
void SlapCheck(struct raw_line *rawp);
void Stats(struct raw_line *rawp);
void Weather(struct raw_line *rawp);
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
