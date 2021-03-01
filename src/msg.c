#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <openssl/ssl.h>

#include "codybot.h"

// Send 'text' to 'target' channel or user.
void Msg(char *text) {
	unsigned int total_len = strlen(text);
	if (total_len <= 400) {
		sprintf(buffer_log, "PRIVMSG %s :%s\n", target, text);
		if (use_ssl)
			SSL_write(pSSL, buffer_log, strlen(buffer_log));
		else
			write(socket_fd, buffer_log, strlen(buffer_log));
		Log(OUT, buffer_log);
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
				Log(OUT, str);
				memset(str, 0, 400);
				break;
			}
			else if (cnt >= 398) {
				str[cnt] = '\n';
				str[cnt+1] = '\0';
				if (use_ssl)
					SSL_write(pSSL, str, strlen(str));
				else
					write(socket_fd, str, strlen(str));
				Log(OUT, str);
				memset(str, 0, 400);
				sprintf(str, "PRIVMSG %s :", target);
				cnt = strlen(str);
			}
		}
		return;
	}
}

// Send raw data to server directly.
void MsgRaw(char *text) {
	if (use_ssl)
		SSL_write(pSSL, text, strlen(text));
	else
		write(socket_fd, text, strlen(text));
	
	// Don't log PONGs
	if (strncmp(text, "PONG :", 6) != 0)
		Log(OUT, text);
}

