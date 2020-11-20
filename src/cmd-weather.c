#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "codybot.h"

// Array containing time at which !weather have been run
unsigned long weather_usage[10];

// Pop the first item
void WeatherDecayUsage(void) {
	int cnt;
	for (cnt = 0; cnt < 9; cnt++)
		weather_usage[cnt] = weather_usage[cnt+1];

	weather_usage[cnt] = 0;
}

// Return true if permitted, false if quota reached
int WeatherCheckUsage(void) {
	int cnt;
	for (cnt = 0; cnt < 10; cnt++) {
		// If there's available slot
		if (weather_usage[cnt] == 0) {
			weather_usage[cnt] = time(NULL);
			return 1;
		}
		// If usage is complete and first item dates from over 10 minutes
		else if (cnt == 9 && weather_usage[0] < (time(NULL) - (60*30))) {
			WeatherDecayUsage();
			weather_usage[cnt] = time(NULL);
			return 1;
		}
	}

	return 0;
}

void Weather(struct raw_line *rawp) {
	if (!WeatherCheckUsage()) {
		Msg("Weather quota reached, maximum 10 times every 30 minutes.");
		return;
	}

	// Check for "kill" found in ",weather `pkill${IFS}codybot`" which kills the bot
	char *c = rawp->text;
	while (1) {
		if (*c == '\0' || *c == '\n')
			break;
		if (strlen(c) >= 5 && strncmp(c, "kill", 4) == 0) {
			Msg("weather: contains a blocked term...");
			return;
		}
		++c;
	}


	unsigned int cnt = 0, cnt_conv = 0;
	char city[128], city_conv[128], *cp = rawp->text + strlen("^weather ");
	memset(city, 0, 128);
	memset(city_conv, 0, 128);
	while (1) {
		if (*cp == '\n' || *cp == '\0' || cp - rawp->text >= 128)
			break;
		else if (cnt == 0 && *cp == ' ') {
			++cp;
			continue;
		}
		else if (*cp == '"') {
			++cp;
			continue;
		}
		else if (*cp == ' ') {
			city[cnt++] = ' ';
			city_conv[cnt_conv++] = '%';
			city_conv[cnt_conv++] = '2';
			city_conv[cnt_conv++] = '0';
			++cp;
			continue;
		}
		
		city[cnt] = *cp;
		city_conv[cnt_conv] = *cp;
		++cnt;
		++cnt_conv;
		++cp;
	}
	memset(rawp->text, 0, strlen(rawp->text));

	char filename[1024];
	sprintf(filename, "/tmp/codybot-weather-%s.txt", city_conv);
	sprintf(buffer, "wget -t 1 -T 24 https://wttr.in/%s?format=%%C:%%t:%%f:%%w:%%p "
		"-O %s", city_conv, filename);
	system(buffer);

	/*temp2[strlen(temp2)-1] = ' ';
	int deg_celsius = atoi(temp2);
	int deg_farenheit = (deg_celsius * 9 / 5) + 32;
	sprintf(buffer_cmd, "%s: %s %dC/%dF", city, temp, deg_celsius, deg_farenheit);
	Msg(buffer_cmd);*/

	FILE *fp = fopen(filename, "r");
	if (fp == NULL) {
		sprintf(buffer, "codybot error: Cannot open %s: %s",
			filename, strerror(errno));
		Msg(buffer);
		return;
	}
	fseek(fp, 0, SEEK_END);
	unsigned long filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *str = malloc(filesize+1);
	char *str2 = malloc(filesize+128);
	memset(str2, 0, filesize+128);
	fgets(str, filesize, fp);
	cnt = 0;
	int cnt2 = 0, step = 0;
	while (1) {
		if (str[cnt] == '\0') {
			str2[cnt2] = '\n';
			str2[cnt2+1] = '\0';
			break;
		}
		else if (str[cnt] == ':') {
			str2[cnt2] = ' ';
			++cnt;
			++cnt2;
			++step;
			if (step == 2) {
				strcat(str2, "feels like ");
				cnt2 += 11;
			}
			continue;
		}
		// The degree symbol doesn't display correctly, so replace
		else if (str[cnt] == -62 && str[cnt+1] == -80) {
			str2[cnt2] = '*';
			cnt += 2;
			++cnt2;
			continue;
		}
		else if (str[cnt] < 32 || str[cnt] > 126) {
			++cnt;
			continue;
		}

		str2[cnt2] = str[cnt];
		++cnt;
		++cnt2;
	}
	sprintf(buffer, "%s: %s", city, str2);
	Msg(buffer);

	/*FILE *fw = fopen("tmp/data", "w");
	for (c = str; *c != '\0'; c++)
		fprintf(fw, ":%c:%d\n", *c, (int)*c);
	fclose(fw);*/

	free(str);
	free(str2);
	
	if (!debug) {
		sprintf(buffer, "rm %s", filename);
		system(buffer);
		memset(buffer, 0, 4096);
	}
}
