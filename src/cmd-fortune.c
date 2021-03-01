#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>

#include "codybot.h"

void Fortune(struct raw_line *rawp) {
	FILE *fp = fopen("data/fortunes.txt", "r");
	if (fp == NULL) {
		sprintf(buffer, "##codybot::Fortune() error: Cannot open data/fortunes.txt: %s",
			strerror(errno));
		Msg(buffer);
		return;
	}

	fseek(fp, 0, SEEK_END);
	unsigned long filesize = (unsigned long)ftell(fp);
	fseek(fp, 0, SEEK_SET);
	gettimeofday(&tv0, NULL);
	srand((unsigned int)tv0.tv_usec/((rand()%10)+1));
	fseek(fp, rand()%(filesize-500), SEEK_CUR);

	int c = 0, cprev, cnt = 0;
	char fortune_line[4096];
	memset(fortune_line, 0, 4096);
	while (1) {
		cprev = c;
		c = fgetc(fp);
		if (c == -1) {
			c = ' ';
			break;
		}
		if (cprev == '\n' && c == '%') {
			// Skip the newline
			fgetc(fp);
			c = ' ';
			break;
		}
	}

	if (debug) {
		sprintf(buffer, "&&&&fortune pos: %ld&&&&", ftell(fp));
		Log(LOCAL, buffer);
	}

	while (1) {
		cprev = c;
		c = fgetc(fp);
		if (c == -1)
			break;
		else if (c == '\t' && cprev == '\n')
			break;
		else if (c == '%' && cprev == '\n')
			break;
		else if (c == '\n' && cprev == '\n')
			fortune_line[cnt++] = ' ';
		else if (c == '\n' && cprev != '\n')
			fortune_line[cnt++] = ' ';
		else
			fortune_line[cnt++] = c;
	}

	fclose(fp);

	if (strlen(fortune_line) > 0) {
		RawGetTarget(rawp);
		sprintf(buffer, "fortune: %s", fortune_line);
		Msg(buffer);

		fp = fopen("stats", "r");
		if (fp == NULL) {
			sprintf(buffer, "codybot::Fortune() error: Cannot open stats file: %s",
				strerror(errno));
			Msg(buffer);
			return;
		}
		fgets(buffer, 4095, fp);
		fortune_total = atoi(buffer);
		fclose(fp);
	
		fp = fopen("stats", "w");
		if (fp == NULL) {
			sprintf(buffer, "codybot::Fortune() error: Cannot open stats file: %s",
				strerror(errno));
			Msg(buffer);
			return;
		}

		fprintf(fp, "%llu\n", ++fortune_total);

		fclose(fp);
	}

}

