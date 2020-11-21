#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "codybot.h"

void Calc(struct raw_line *rawp) {
	strcat(rawp->text, "\n");

	FILE *fi = fopen("cmd.input", "w+");
	if (fi == NULL) {
		sprintf(buffer, "codybot::calc() error: Cannot open cmd.input for writing: %s",
			strerror(errno));
		Msg(buffer);
		return;
	}
	fputs(rawp->text+6, fi);
	fclose(fi);

	system("bc -l &> cmd.output < cmd.input");

	FILE *fp = fopen("cmd.output", "r");
	if (fp == NULL) {
		sprintf(buffer, "##codybot::Calc() error: Cannot open cmd.output: %s",
			strerror(errno));
		Msg(buffer);
		return;
	}

	char line[400];
	char *str;
	unsigned int cnt = 0;
	while (1) {
		str = fgets(line, 399, fp);
		if (str == NULL)
			break;
		Msg(line);
		++cnt;
		if (cnt >= 4) {
			system("cat cmd.output | nc termbin.com 9999 > cmd.url");
			FILE *fu = fopen("cmd.url", "r");
			if (fu == NULL) {
				sprintf(buffer, "##codybot::Calc() error: Cannot open cmd.url: %s",
					strerror(errno));
				Msg(buffer);
				break;
			}
			fgets(line, 399, fu);
			fclose(fu);
			Msg(line);
			break;
		}
	}

	fclose(fp);
}

