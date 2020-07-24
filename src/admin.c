#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "codybot.h"

struct AdminList admin_list;

void AddAdmin(char *newnick, char *hostmask) {
	if (debug)
		printf("## AddAdmin(): \"%s\" \"%s\"\n", newnick, hostmask);
	
	struct Admin *admin = malloc(sizeof(struct Admin));
	
	if (admin_list.first_admin == NULL) {
		admin_list.first_admin = admin;
		admin->prev = NULL;
	}
	else {
		admin->prev = admin_list.last_admin;
		admin_list.last_admin->next = admin;
	}

	admin->next = NULL;
	admin->nick = malloc(strlen(newnick)+1);
	sprintf(admin->nick, "%s", newnick);
	if (hostmask == NULL)
		admin->hostmask = NULL;
	else {
		admin->hostmask = malloc(strlen(hostmask)+1);
		sprintf(admin->hostmask, "%s", hostmask);
	}

	admin_list.last_admin = admin;
	++admin_list.total_admins;
}

char *EnumerateAdmins(void) {
	struct Admin *admin = admin_list.first_admin;
	char *str = malloc(4096);
	memset(str, 0, 4096);
	
	if (admin == NULL) {
		sprintf(str, "(no admin in the list)");
		return str;
	}

	while (1) {
		strcat(str, admin->nick);
		if (admin->next != NULL) {
			if (admin->next->next == NULL)
				strcat(str, " and ");
			else
				strcat(str, ", ");
		}

		if (admin->next == NULL)
			break;
		else
			admin = admin->next;
	}

	return str;
}

int IsAdmin(char *newnick, char *hostmask) {
	struct Admin *admin = admin_list.first_admin;
	if (admin == NULL)
		return 0;
	
	while (1) {
		if (strcmp(newnick, admin->nick) == 0) {
			if (admin->hostmask != NULL) {
				if (strcmp(hostmask, admin->hostmask) == 0)
					return 1;
				else
					return 0;
			}
			else
				return 1;
		}

		if (admin->next == NULL)
			break;
		else
			admin = admin->next;
	}

	return 0;
}

void ParseAdminFile(void) {
	FILE *fp = fopen("admins.txt", "r");
	if (fp == NULL) {
		sprintf(buffer, "codybot::ParseAdminFile() error: Cannot open admins.txt: %s",
			strerror(errno));
		Msg(buffer);
		return;
	}

	char newnick[1024], hostmask[1024];
	memset(newnick, 0, 1024);
	memset(hostmask, 0, 1024);
	char c;
	unsigned int recording_nick = 1, recording_hostmask = 0,
		in_comment = 0, cnt = 0;
	while (1) {
		c = fgetc(fp);
		if (c == EOF) {
			if (!in_comment) {
				if (recording_nick && strlen(newnick))
					AddAdmin(newnick, NULL);
				else if (recording_hostmask && strlen(hostmask))
					AddAdmin(newnick, hostmask);
			}
			break;
		}
		else if (c == '#') {
			in_comment = 1;
			if (recording_nick) {
				if (strlen(newnick))
					AddAdmin(newnick, NULL);
			}
			else if (recording_hostmask) {
				recording_hostmask = 0;
				recording_nick = 1;
				if (strlen(hostmask))
					AddAdmin(newnick, hostmask);
				else
					AddAdmin(newnick, NULL);
			}
			memset(newnick, 0, 1024);
			memset(hostmask, 0, 1024);
			cnt = 0;
			continue;
		}
		else if (c == '\n') {
			if (in_comment)
				in_comment = 0;
			else if (recording_nick) {
				if (strlen(newnick))
					AddAdmin(newnick, NULL);
				memset(newnick, 0, 1024);
				cnt = 0;
			}
			else if (recording_hostmask) {
				if (strlen(newnick)) {
					if (strlen(hostmask))
						AddAdmin(newnick, hostmask);
					else
						AddAdmin(newnick, NULL);
				}
				memset(newnick, 0, 1024);
				memset(hostmask, 0, 1024);
				cnt = 0;
				recording_hostmask = 0;
				recording_nick = 1;
			}
			continue;
		}
		else if (c == ' ') {
			if (recording_nick) {
				if (strlen(newnick)) {
					recording_nick = 0;
					recording_hostmask = 1;
					cnt = 0;
				}
				continue;
			}
			else if (recording_hostmask) {
				if (strlen(hostmask)) {
					AddAdmin(newnick, hostmask);
					memset(newnick, 0, 1024);
					memset(hostmask, 0, 1024);
					cnt = 0;
					recording_hostmask = 0;
					recording_nick = 1;
					continue;
				}
				else
					continue;
			}
		}

		if (in_comment)
			continue;
		else if (recording_nick)
			newnick[cnt] = c;
		else if (recording_hostmask)
			hostmask[cnt] = c;

		++cnt;
	}

	fclose(fp);
}

