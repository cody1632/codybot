#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "codybot.h"

void CC(struct raw_line *rawp) {
	rawp->text[0] = ' ';
	rawp->text[1] = ' ';
	rawp->text[2] = ' ';

	// check for the system() call and cancel if found
	char *c = rawp->text;
	while (1) {
		if (*c == '\0' || *c == '\n')
			break;
		if ((strlen(c) >= 7 && strncmp(c, "system", 6) == 0) ||
			(strlen(c) >= 5 && strncmp(c, "exec", 4) == 0)) {
			Msg("won't run system() nor exec() calls...");
			return;
		}
		++c;
	}

	FILE *fr = fopen("prog-head.c", "r");
	if (fr == NULL) {
		sprintf(buffer, "codybot error: Cannot open prog-head.c: %s", 
			strerror(errno));
		Msg(buffer);
		return;
	}

	FILE *fp = fopen("prog.c", "w+");
	if (fp == NULL) {
		sprintf(buffer, "codybot error: Cannot open prog.c: %s",
			strerror(errno));
		Msg(buffer);
		return;
	}

	//fprintf(fp, "#include <stdio.h>\n#include <stdlib.h>\n#include <unistd.h>\n");
	//fprintf(fp, "#include <string.h>\n#include <errno.h>\n#include <sys/types.h>\n");
	//fprintf(fp, "#include <time.h>\n#include <sys/time.h>\n#include <math.h>\n\n");
	//fprintf(fp, "int main(int argc, char **argv) {\n");
	while (fgets(buffer, 1024, fr) != NULL)
		fputs(buffer, fp);

	fprintf(fp, "%s\n", rawp->text);

	fclose(fr);
	fr = fopen("prog-tail.c", "r");
	if (fr == NULL) {
		sprintf(buffer, "codybot error: Cannot open prog-tail.c: %s",
			strerror(errno));
		Msg(buffer);
		return;
	}
	while (fgets(buffer, 1024, fr) != NULL)
		fputs(buffer, fp);

	fclose(fr);
	fclose(fp);

	if (cc_compiler == CC_COMPILER_GCC)
		ret = system("gcc -std=c11 -Wall -Werror -D_GNU_SOURCE -O2 -g "
			"prog.c -o prog 2>cmd.output");
	else if (cc_compiler == CC_COMPILER_TCC)
		ret = system("tcc -lm -o prog prog.c 2>cmd.output");

	if (ret == 0) {
		sprintf(buffer, "timeout %ds ./prog &> cmd.output; echo $? > cmd.ret",
			cmd_timeout);
		system(buffer);
	}
	else {
		char chars_line[4096];
		char *str;
		fp = fopen("cmd.output", "r");
		while (1) {
			str = fgets(chars_line, 4095, fp);
			if (str == NULL) break;
			sprintf(buffer, "%s", chars_line);
			Msg(buffer);
		}
		fclose(fp);
	}

	fp = fopen("cmd.ret", "r");
	if (fp == NULL) {
		sprintf(buffer, "codybot::CC() error: Cannot open cmd.ret: %s",
			strerror(errno));
		Msg(buffer);
		return;
	}
	fgets(buffer, 4096, fp);
	fclose(fp);

	ret = atoi(buffer);
	if (ret == 124) {
		sprintf(buffer_cmd, "cc: timed out");
		Msg(buffer_cmd);
		return;
	}

	fp = fopen("cmd.output", "r");
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
		if (cnt >= 10) {
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

