#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "codybot.h"

// There's only 1 admin list. See codybot.h for declaration.
struct AdminList admin_list;

// host can be NULL
void AddAdmin(char *newnick, char *host) {
	if (debug) {
		sprintf(buffer, "AddAdmin(): '%s' '%s'", newnick, host);
		Log(LOCAL, buffer);
	}
	
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
	if (host == NULL)
		admin->host = NULL;
	else {
		admin->host = malloc(strlen(host)+1);
		sprintf(admin->host, "%s", host);
	}

	admin_list.last_admin = admin;
	++admin_list.total_admins;
}

void DestroyAdminList(void) {
	struct Admin *admin = admin_list.last_admin;
	if (admin == NULL)
		return;
	
	while (1) {
		free(admin->host);
		free(admin->nick);

		if (admin->prev == NULL) {
			free(admin);
			break;
		}
		else {
			admin = admin->prev;
			free(admin->next);
		}
	}

	admin_list.first_admin = NULL;
	admin_list.last_admin = NULL;
}

// Returns a comma-separated list of admins.
char *EnumerateAdmins(void) {
	struct Admin *admin = admin_list.first_admin;
	char *str = malloc(4096);
	memset(str, 0, 4096);
	
	if (admin == NULL) {
		sprintf(str, "(No admins in the list)");
		return str;
	}

	while (1) {
		strcat(str, admin->nick);
		strcat(str, "@");
		strcat(str, admin->host);
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

// Returns 1 if found in the list, 0 otherwise.
int IsAdmin(char *newnick, char *host) {
	struct Admin *admin = admin_list.first_admin;
	if (admin == NULL)
		return 0;
	
	while (1) {
		if (strcmp(newnick, admin->nick) == 0) {
			if (admin->host != NULL) {
				if (strcmp(host, admin->host) == 0)
					return 1;
				else { // could catch the same nick in the list
					   // with different host
					if (admin->next == NULL)
						break;
					else
						admin = admin->next;
					continue;
				}
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

// Populate the admin list.
void ParseAdminFile(void) {
	FILE *fp = fopen("admins.txt", "r");
	if (fp == NULL) {
		sprintf(buffer, "codybot::ParseAdminFile() error: Cannot open admins.txt: %s",
			strerror(errno));
		Log(LOCAL, buffer);
		return;
	}

	char newnick[1024], host[1024];
	memset(newnick, 0, 1024);
	memset(host, 0, 1024);
	char c;
	unsigned int recording_nick = 1, recording_host = 0,
		in_comment = 0, cnt = 0;
	while (1) {
		c = fgetc(fp);
		if (c == EOF) {
			if (!in_comment) {
				if (recording_nick && strlen(newnick))
					AddAdmin(newnick, NULL);
				else if (recording_host && strlen(host))
					AddAdmin(newnick, host);
			}
			break;
		}
		else if (c == '#') {
			in_comment = 1;
			if (recording_nick) {
				if (strlen(newnick))
					AddAdmin(newnick, NULL);
			}
			else if (recording_host) {
				recording_host = 0;
				recording_nick = 1;
				if (strlen(host))
					AddAdmin(newnick, host);
				else
					AddAdmin(newnick, NULL);
			}
			memset(newnick, 0, 1024);
			memset(host, 0, 1024);
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
			else if (recording_host) {
				if (strlen(newnick)) {
					if (strlen(host))
						AddAdmin(newnick, host);
					else
						AddAdmin(newnick, NULL);
				}
				memset(newnick, 0, 1024);
				memset(host, 0, 1024);
				cnt = 0;
				recording_host = 0;
				recording_nick = 1;
			}
			continue;
		}
		else if (c == ' ') {
			if (recording_nick) {
				if (strlen(newnick)) {
					recording_nick = 0;
					recording_host = 1;
					cnt = 0;
				}
				continue;
			}
			else if (recording_host) {
				if (strlen(host)) {
					AddAdmin(newnick, host);
					memset(newnick, 0, 1024);
					memset(host, 0, 1024);
					cnt = 0;
					recording_host = 0;
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
		else if (recording_host)
			host[cnt] = c;

		++cnt;
	}

	fclose(fp);
}

