#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "codybot.h"

void AsciiArt(struct raw_line *rawp) {
	FILE *fp = fopen("data/ascii.txt", "r");
	if (fp == NULL) {
		sprintf(buffer, "codybot::AsciiArt() error: cannot open data/ascii.txt: %s",
			strerror(errno));
		Msg(buffer);
		return;
	}

	fseek(fp, 0, SEEK_END);
	unsigned long filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	gettimeofday(&tv0, NULL);
	srand((unsigned int)tv0.tv_usec/((rand()%10)+1));
	fseek(fp, rand()%(filesize-100), SEEK_CUR);

	int c = 0, cprev, cnt = 0;
	while (1) {
		cprev = c;
		c = fgetc(fp);
		if (c == -1) {
			break;
		}
		if (cprev == '\n' && c == '%') {
			// Skip the newline
			fgetc(fp);
			break;
		}
	}

	char line[1024];
	memset(line, 0, 1024);
	cnt = 0, c = ' ';
	while (1) {
		cprev = c;
		c = fgetc(fp);
		if (c == -1)
			break;
		else if (c == '%' && cprev == '\n')
			break;
		else if (c == '\n') {
			Msg(line);
			// Throttled due to server notice of flooding
			sleep(2);
			memset(line, 0, 1024);
			cnt = 0;
		}
		else
			line[cnt++] = c;
	}

	fclose(fp);
}

