#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <openssl/ssl.h>

#include "codybot.h"

// Read and parse keyboard input from the console
void ConsoleReadInput(void) {
	char buffer_line[4096];
	while (!endmainloop) {
		memset(buffer_line, 0, 4096);
		fgets(buffer_line, 4095, stdin);
		char *cp;
		cp = buffer_line;
		if (buffer_line[0] == '\n')
			continue;
		else if (strcmp(buffer_line, "exit\n") == 0 || strcmp(buffer_line, "quit\n") == 0)
			endmainloop = 1;
		else if (strcmp(buffer_line, "cursh\n") == 0)
			printf("cursh = %s\n", current_channel);
		else if (strncmp(buffer_line, "cursh ", 6) == 0) {
			sprintf(current_channel, "%s", buffer_line+6);
		}
		else if (strcmp(buffer_line, "debug\n") == 0)
			printf("debug = %d\n", debug);
		else if (strcmp(buffer_line, "debug on\n") == 0) {
			debug = 1;
			Msg("debug = 1");
		}
		else if (strcmp(buffer_line, "debug off\n") == 0) {
			debug = 0;
			Msg("debug = 0");
		}
		else if (strcmp(buffer_line, "fortune\n") == 0) {
			sprintf(raw.channel, "%s", current_channel);
			Fortune(&raw);
		}
		else if (strncmp(buffer_line, "msg ", 4) == 0) {
			sprintf(buffer, "%s", buffer_line+4);
			sprintf(raw.channel, "%s", current_channel);
			target = raw.channel;
			Msg(buffer);
		}
		else if (strcmp(buffer_line, "sh_disable\n") == 0) {
			sh_disabled = 1;
			Msg("sh_disabled = 1");
		}
		else if (strcmp(buffer_line, "sh_enable\n") == 0) {
			sh_disabled = 0;
			Msg("sh_disabled = 1");
		}
		else if (strcmp(buffer_line, "sh_lock\n") == 0) {
			sh_locked = 1;
			Msg("sh_locked = 1");
		}
		else if (strcmp(buffer_line, "sh_unlock\n") == 0) {
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
		else if (strcmp(buffer_line, "trigger\n") == 0)
			printf("trigger = %c\n", trigger_char);
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

