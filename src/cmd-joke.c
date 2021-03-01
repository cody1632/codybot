#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "codybot.h"

void Joke(struct raw_line *rawp) {
	FILE *fp = fopen("data/jokes.txt", "r");
	if (fp == NULL) {
		sprintf(buffer, "codybot::Joke() error: cannot open data/jokes.txt: %s",
			strerror(errno));
		Msg(buffer);
		return;
	}

	fseek(fp, 0, SEEK_END);
	unsigned long filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	gettimeofday(&tv0, NULL);
	srand((unsigned int)tv0.tv_usec/((rand()%10)+1));
	unsigned int rnd = rand()%(filesize-100);
	fseek(fp, rnd, SEEK_CUR);
	if (debug) {
		sprintf(buffer, "##filesize: %lu\n##rnd: %u", filesize, rnd);
		Log(LOCAL, buffer);
	}

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

	char joke_line[4096];
	memset(joke_line, 0, 4096);
	cnt = 0, c = ' ';
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
			joke_line[cnt++] = ' ';
		else if (c == '\n' && cprev != '\n')
			joke_line[cnt++] = ' ';
		else
			joke_line[cnt++] = c;
	}

	RawGetTarget(rawp);
	if (strlen(joke_line) > 0) {
		sprintf(buffer, "joke: %s", joke_line);
		Msg(buffer);
	}
	else {
		Msg("codybot::Joke(): joke_line is empty!");
	}

	fclose(fp);
}

