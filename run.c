#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
	char cmd[4096];
	memset(cmd, 0, 4096);
	sprintf(cmd, "timeout 30s ");

	int cnt;
	for (cnt = 1; cnt < argc; cnt++) {
		strcat(cmd, argv[cnt]);
		strcat(cmd, " ");
	}

	strcat(cmd, "&>cmd.output; echo $? >cmd.ret");
	printf("argv[%d]:%s:\n", cnt, cmd);
	system(cmd);

	return 0;
}

