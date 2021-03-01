#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>

#include "codybot.h"

void Log(unsigned int direction, char *text) {
	FILE *fp = fopen(log_filename, "a+");
	if (fp == NULL) {
		fprintf(stderr, "##codybot::Log() error: Cannot open %s: %s\n",
			log_filename, strerror(errno));
		return;
	}

	gettimeofday(&tv0, NULL);
	t0 = (time_t)tv0.tv_sec;
	tm0 = localtime(&t0);
	char *str = strdup(text);
	// remove trailing newline
	if (str[strlen(str)-1] == '\n')
		str[strlen(str)-1] = '\0';

	char dirstr[3];
	if (direction == LOCAL)
		sprintf(dirstr, "==");
	else if (direction == IN)
		sprintf(dirstr, "<<");
	else if (direction == OUT)
		sprintf(dirstr, ">>");
	else
		sprintf(dirstr, "::");

	// Log with timestamp..
	sprintf(buffer_log, "%02d%02d%02d-%02d:%02d:%02d.%03ld %s##%s##\n",
		tm0->tm_year+1900-2000, tm0->tm_mon+1,
		tm0->tm_mday, tm0->tm_hour, tm0->tm_min, tm0->tm_sec, tv0.tv_usec,
		dirstr, str);
	fputs(buffer_log, fp);

	// Show message in console with colors
	sprintf(buffer_log, "\e[00;36m%02d%02d%02d-%02d:%02d:%02d.%03ld %s##\e[00m%s\e[00;36m##\e[00m\n", 
		tm0->tm_year+1900-2000, tm0->tm_mon+1,
		tm0->tm_mday, tm0->tm_hour, tm0->tm_min, tm0->tm_sec, tv0.tv_usec,
		dirstr, str);
	fputs(buffer_log, stdout);
	
	memset(buffer_log, 0, 4096);

	fclose(fp);
}

