#include <lib/stdio.h>
#include <lib/string.h>
#include <lib/syscall.h>

void _start()
{
	char command[128];
	int child_pid = 0;

	while (1) {
		printf("$ ");
		gets(command);
		if (!command[0])
			continue;

		if (!strcmp(command, "ls") || !strcmp(command, "dir")) {
			if (dir() < 0)
				printf("Cannot list directory\n");
			continue;
		}

		child_pid = fork();
		if (child_pid == 0) {
			if (exec(command) < 0)
				printf("Command not found\n");
			exit(1);
		}
		else {
			wait(child_pid);
		}
	}

	exit(0);
}
