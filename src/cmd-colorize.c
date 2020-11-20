#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

#include "codybot.h"

void Colorize(struct raw_line *rawp) {
	char *cp = raw.text;

	while (*cp != ' ')
		++cp;
	++cp;
	
	char result[4096];
	memset(result, 0, 4096);
	while (1) {
		usleep((rand()%1000)+1);
		gettimeofday(&tv0, NULL);
		srand((unsigned int)tv0.tv_usec/((rand()%10)+1));
		strcat(result, colors[(rand()%13)+2]);
		strncat(result, cp++, 1);
		if (*cp == '\0')
			break;
	}
	strcat(result, colors[0]);

	Msg(result);
}

