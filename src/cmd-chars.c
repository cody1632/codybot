#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "codybot.h"

void Chars(struct raw_line *rawp) {
	Msg("https://esselfe.ca/chars.html");

	FILE *fp = fopen("data/chars.txt", "r");
	if (fp == NULL) {
		sprintf(buffer, "codybot::Chars() error: Cannot open data/chars.txt: %s",
			strerror(errno));
		Msg(buffer);
		return;
	}

	char chars_line[4096];
	char *str;
	while (1) {
		str = fgets(chars_line, 4095, fp);
		if (str == NULL) break;
		sprintf(buffer, "%s", chars_line);
		Msg(buffer);
	}

	fclose(fp);
}

