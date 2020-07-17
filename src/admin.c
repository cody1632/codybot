#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "codybot.h"

struct AdminList admin_list;

void AddAdmin(char *newnick) {
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

int IsAdmin(char *newnick) {
	struct Admin *admin = admin_list.first_admin;
	if (admin == NULL)
		return 0;
	
	while (1) {
		if (strcmp(newnick, admin->nick) == 0)
			return 1;

		if (admin->next == NULL)
			break;
		else
			admin = admin->next;
	}

	return 0;
}

