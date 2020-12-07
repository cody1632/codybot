#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "codybot.h"

// see https://tools.ietf.org/html/rfc2229 (not fully implemented)
void Foldoc(struct raw_line *rawp) {
	char *cp = rawp->text + strlen("!foldoc ");
	char word[128];
	memset(word, 0, 128);
	int cnt;
	for (cnt=0; cnt<127; cp++,cnt++) {
		if (*cp == '\0')
			break;
		else if (*cp == ' ')
			break;

		word[cnt] = *cp;
	}

	sprintf(buffer_cmd, "curl dict.org/d:%s:foldoc -o dict.output", word);
	Log(buffer_cmd);
	system(buffer_cmd);

	FILE *fp = fopen("dict.output", "r");
	if (fp == NULL) {
		sprintf(buffer, "##codybot::Foldoc() error: Cannot open dict.output: %s",
			strerror(errno));
		Msg(buffer);
		return;
	}
	FILE *fw = fopen("cmd.output", "w");
	if (fw == NULL) {
		sprintf(buffer, "##codybot::Foldoc() error: Cannot open cmd.output: %s",
			strerror(errno));
		Msg(buffer);
		fclose(fp);
		return;
	}

	size_t size = 1024;
	ssize_t bytes_read;
	char *line = malloc(size);
	int line_cnt = 0;
	while (1) {
		memset(line, 0, 1024);
		bytes_read = getline(&line, &size, fp);
		if (bytes_read == -1)
			break;

		if (strncmp(line, "220 ", 4) == 0 || strncmp(line, "250 ", 4) == 0 ||
			strncmp(line, "150 ", 4) == 0 || strncmp(line, "151 ", 4) == 0 ||
			strncmp(line, "221 ", 4) == 0 ||
			((line[0] > 65 && line[0] < 122) && line[0] != '.') || 
			strcmp(line, ".\r\n") == 0)
			continue;
		else if (strncmp(line, "552 ", 4) == 0) {
			Msg("No match");
			break;
		}
		else {
			if (strlen(line+3) > 0) {
				++line_cnt;

				if (line_cnt <= 4)
					Msg(line+3);

				fputs(line+3, fw);
			}
		}
	}
	fclose(fp);
	fclose(fw);

	if (line_cnt >= 5) {
		system("cat cmd.output | nc termbin.com 9999 > cmd.url");
		fp = fopen("cmd.url", "r");
		if (fp == NULL) {
			sprintf(buffer, "##codybot::Foldoc() error: Cannot open cmd.url: %s",
				strerror(errno));
			Msg(buffer);
			return;
		}

		bytes_read = getline(&line, &size, fp);
		if (bytes_read > -1)
			Msg(line);
		
		fclose(fp);
	}
}

