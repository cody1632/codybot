#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>

#include "codybot.h"

void ThreadRunStart(char *command) {
	pthread_t thr;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&thr, &attr, ThreadRunFunc, (void *)command);
    pthread_detach(thr);
    pthread_attr_destroy(&attr);
}

void *ThreadRunFunc(void *argp) {
	char *text = strdup(raw.text);
	printf("&& Thread started ::%s::\n", text);
	//char *cp = argp;
	char *cp = text;
	char cmd[4096];
	sprintf(cmd, "timeout %ds bash -c \"", cmd_timeout);
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
	strcat(cmd, "\" &> cmd.output; echo $? >cmd.ret");
	Logx(cmd);
	system(cmd);

	FILE *fp = fopen("cmd.ret", "r");
	if (fp == NULL) {
		sprintf(buffer, "codybot::ThreadRunFunc() error: Cannot open cmd.ret: %s", strerror(errno));
		Msg(buffer);
	}
	fgets(buffer, 4096, fp);
	fclose(fp);

	ret = atoi(buffer);
	if (ret == 124) {
		Msg("sh: timed out");
		return NULL;
	}

	fp = fopen("cmd.output", "r");
	if (fp == NULL) {
		sprintf(buffer, "codybot::ThreadRunFunc() error: Cannot open cmd.output: %s", strerror(errno));
		Msg(buffer);
		return NULL;
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

	if (lines_total <= 10) {
		char *result = (char *)malloc(4096);
		memset(result, 0, 4096);
		size_t size = 4095;
		while (1) {
			int ret2 = getline(&result, &size, fp);
			if (ret2 < 0) break;
			result[strlen(result)-1] = '\0';
			Msg(result);
		}
	}
	else if (lines_total > 10) {
		system("cat cmd.output |nc termbin.com 9999 > cmd.url");
		FILE *fp2 = fopen("cmd.url", "r");
		if (fp2 == NULL)
			fprintf(stderr, "##codybot::ThreadRXFunc() error: Cannot open cmd.url: %s\n", strerror(errno));
		else {
			char url[1024];
			fgets(url, 1023, fp2);
			fclose(fp2);
			Msg(url);
		}
	}

	fclose(fp);

	printf("&& Thread stopped, ret: %d ::%s::\n", ret, text);

	return NULL;
}

void *ThreadRXFunc(void *argp);
void ThreadRXStart(void) {
	pthread_t thr;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&thr, &attr, ThreadRXFunc, NULL);
    pthread_detach(thr);
    pthread_attr_destroy(&attr);
}

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
					Log("PONG\n");
			}
			continue;
		}

		RawLineParse(&raw, buffer_rx);
		RawGetTarget(&raw);
if (raw.text != NULL && raw.nick != NULL && strcmp(raw.command, "JOIN")!=0 &&
strcmp(raw.command, "NICK")!=0) {
		SlapCheck(&raw);
		if (raw.text[0]==trigger_char && strcmp(raw.text+1, "help")==0) {
			char c = trigger_char;
			sprintf(buffer, "commands: %cabout %cascii %cchars %chelp %cfortune"
				" %cjoke %crainbow %csh %cstats %cversion %cweather\n", c,c,c,c,c,c,c,c,c,c,c);
			Msg(buffer);
			continue;
		}
		else if (raw.text[0]==trigger_char && strcmp(raw.text+1, "ascii")==0)
			AsciiArt(&raw);
		else if (raw.text[0]==trigger_char && strcmp(raw.text+1, "about")==0) {
			if (strcmp(nick, "codybot")==0)
				Msg("codybot is an IRC bot written in C by esselfe, "
					"sources @ https://github.com/cody1632/codybot");
			else {
				sprintf(buffer, "codybot(%s) is an IRC bot written in C by esselfe, "
					"sources @ https://github.com/cody1632/codybot\n", nick);
				Msg(buffer);
			}
		}
		else if (raw.text[0]==trigger_char && strcmp(raw.text+1, "chars")==0)
			Chars(&raw);
		else if (raw.text[0]==trigger_char&&raw.text[1]=='r'&&raw.text[2]=='a'&&raw.text[3]=='i'&&raw.text[4]=='n'&&
		  raw.text[5]=='b'&&raw.text[6]=='o'&&raw.text[7]=='w'&&raw.text[8]==' ')
			Rainbow(&raw);
		else if (raw.text[0]==trigger_char && strcmp(raw.text+1, "fortune")==0)
			Fortune(&raw);
		else if (raw.text[0]==trigger_char && strcmp(raw.text+1, "debug on")==0) {
			debug = 1;
			Msg("debug = 1");
		}
		else if (raw.text[0]==trigger_char && strcmp(raw.text+1, "debug off")==0) {
			debug = 0;
			Msg("debug = 0");
		}
		else if(raw.text[0]==trigger_char && strcmp(raw.text+1, "joke")==0)
			Joke(&raw);
		else if (raw.text[0]==trigger_char && strcmp(raw.text+1, "stats")==0)
			Stats(&raw);
		else if (raw.text[0]==trigger_char&&raw.text[1]=='t'&&raw.text[2]=='i'&&raw.text[3]=='m'&&raw.text[4]=='e'&&
		  raw.text[5]=='o'&&raw.text[6]=='u'&&raw.text[7]=='t'&&raw.text[8]=='\0') {
			sprintf(buffer, "timeout = %d", cmd_timeout);
			Msg(buffer);
		}
		else if (raw.text[0]==trigger_char&&raw.text[1]=='t'&&raw.text[2]=='i'&&raw.text[3]=='m'&&raw.text[4]=='e'&&
			raw.text[5]=='o'&&raw.text[6]=='u'&&raw.text[7]=='t'&&raw.text[8]==' ') {
			if (strcmp(raw.nick, "codybot")==0 || strcmp(raw.nick, "esselfe")==0) {
				raw.text[0] = ' ';
				raw.text[1] = ' ';
				raw.text[2] = ' ';
				raw.text[3] = ' ';
				raw.text[4] = ' ';
				raw.text[5] = ' ';
				raw.text[6] = ' ';
				raw.text[7] = ' ';
				raw.text[8] = ' ';
				cmd_timeout = atoi(raw.text);
				if (cmd_timeout == 0)
					cmd_timeout = 5;
				sprintf(buffer, "timeout = %d", cmd_timeout);
				Msg(buffer);
			}
			else
				Msg("timeout can only be set by codybot and esselfe");
		}
		else if (raw.text[0]==trigger_char&&raw.text[1]=='t'&&raw.text[2]=='r'&&raw.text[3]=='i'&&raw.text[4]=='g'&&
			raw.text[5]=='g'&&raw.text[6]=='e'&&raw.text[7]=='r'&&raw.text[8]==' '&&raw.text[9]!='\n') {
			if (strcmp(raw.nick, "esselfe")==0)
				trigger_char = raw.text[9];
			sprintf(buffer, "trigger = %c", trigger_char);
			Msg(buffer);
		}
		else if (raw.text[0]==trigger_char && strcmp(raw.text+1, "version")==0) {
			sprintf(buffer, "codybot %s", codybot_version_string);
			Msg(buffer);
		}
		else if (raw.text[0]==trigger_char && strcmp(raw.text+1, "weather")==0) {
			sprintf(buffer, "weather: missing city argument, example: '%cweather montreal'", trigger_char);
			Msg(buffer);
		}
		else if (raw.text[0]==trigger_char && raw.text[1]=='w' && raw.text[2]=='e' && raw.text[3]=='a' &&
			raw.text[4]=='t' && raw.text[5]=='h' && raw.text[6]=='e' && raw.text[7]=='r' && raw.text[8]==' ')
			Weather(&raw);
		else if (strcmp(raw.text, "^sh")==0) {
			sprintf(buffer, "sh: missing argument, example: '%csh ls -ld /tmp'", trigger_char);
			Msg(buffer);
		}
		else if (raw.text[0]==trigger_char && strcmp(raw.text+1, "sh_lock")==0 && strcmp(raw.nick, "esselfe")==0) {
			sh_locked = 1;
			Msg("sh_locked = 1");
		}
		else if (raw.text[0]==trigger_char && strcmp(raw.text+1, "sh_unlock")==0 && strcmp(raw.nick, "esselfe")==0) {
			sh_locked = 0;
			Msg("sh_locked = 0");
		}
		else if (raw.text[0]==trigger_char && raw.text[1]=='s' && raw.text[2]=='h' && raw.text[3]==' ') {
			struct stat st;
			if (sh_disabled || stat("sh_disable", &st) == 0) {
				sprintf(buffer,
					"%s: sh is temporarily disabled, try again later or ask esselfe to enable it", raw.nick);
				Msg(buffer);
				continue;
			}
			// rem touch $srcdir/sh_lock to have ^sh commands run in a chroot
			else if(sh_locked || (stat("sh_lock", &st) == 0)) {
				raw.text[0] = ' ';
				raw.text[1] = ' ';
				raw.text[2] = ' ';
				sprintf(buffer_cmd, "echo '%s' > lunar/home/dummy/run.fifo", raw.text);
				system(buffer_cmd);

				// wait for lunar/home/dummy/run.sh to write in cmd.output
				printf("++tail lunar/home/dummy/run.status++\n");
				sprintf(buffer_cmd, "tail lunar/home/dummy/run.status");
				system(buffer_cmd);
				printf("++tail lunar/home/dummy/run.status++ done\n");

				FILE *fr = fopen("lunar/home/dummy/cmd.output", "r");
				if (fr == NULL) {
					sprintf(buffer, "codybot::ThreadRXFunc() error: Cannot open lunar/home/dummy/cmd.output: %s",
						strerror(errno));
					Msg(buffer);
					continue;
				}

				// count lines total
				int c;
				unsigned int line_total = 0;
				while (1) {
					c = fgetc(fr);
					if (c == -1)
						break;
					else if (c == '\n')
						++line_total;
				}
				fseek(fr, 0, SEEK_SET);

				size_t size = 4096;
				char *output = (char *)malloc(size);
				memset(output, 0, size);
				fgets(output, 4096, fr);
				fclose(fr);
				Msg(output);

				if (line_total >= 2) {
					system("cat lunar/home/dummy/cmd.output |nc termbin.com 9999 >cmd.url");
					fr = fopen("cmd.url", "r");
					if (fr == NULL) {
						sprintf(buffer, "codybot::ThreadRXFunc() error: Cannot open cmd.url: %s", strerror(errno));
						Msg(buffer);
						continue;
					}
					fgets(output, 4096, fr);
					Msg(output);
				
					fclose(fr);
				}

				continue;
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
				Msg("Will not run kill!");
				continue;
			}

			raw.text[0] = ' ';
			raw.text[1] = ' ';
			raw.text[2] = ' ';
			
			if (debug)
				printf("codybot::ThreadRXFunc() starting thread for ::%s::\n", raw.text);
			ThreadRunStart(raw.text);

//			RawLineClear(&raw);
		}

		usleep(10000);
}
	}
	return NULL;
}

