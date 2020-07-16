#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "codybot.h"

struct AdminList rootAdminList;

void AddAdmin(char *nick2) {
	struct Admin *admin = malloc(sizeof(struct Admin));
	
	if (rootAdminList.first_admin == NULL) {
		rootAdminList.first_admin = admin;
		admin->prev = NULL;
	}
	else {
		admin->prev = rootAdminList.last_admin;
		rootAdminList.last_admin->next = admin;
	}

	admin->next = NULL;
	admin->nick = malloc(strlen(nick2)+1);
	sprintf(admin->nick, "%s", nick2);

	rootAdminList.last_admin = admin;
	++rootAdminList.total_admins;
}

int IsAdmin(char *nick2) {
	struct Admin *admin = rootAdminList.first_admin;
	if (admin == NULL)
		return 0;
	
	while (1) {
		if (strcmp(nick2, admin->nick) == 0)
			return 1;

		if (admin->next == NULL)
			break;
		else
			admin = admin->next;
	}

	return 0;
}

