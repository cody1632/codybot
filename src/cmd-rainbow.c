#include <string.h>

#include "codybot.h"

void Rainbow(struct raw_line *rawp) {
	char *cp = raw.text;

	while (*cp != ' ')
		++cp;
	++cp;
	
	char *colors2[8] = {
	"\003", // restore/default
	"\00305", // red
	"\00304", // orange
	"\00308", // yellow
	"\00310", // green
	"\00311", // light cyan
	"\00312", // purple
	"\00302"}; // light cyan
	unsigned int cnt = 1;
	char result[4096];
	memset(result, 0, 4096);
	while (1) {
		strcat(result, colors2[cnt]);
		strncat(result, cp++, 1);
		if (*cp != ' ')
			++cnt;
		if (cnt >= 8)
			cnt = 1;
		if (*cp == '\0') {
			strcat(result, colors[0]);
			break;
		}
	}

	Msg(result);
}

