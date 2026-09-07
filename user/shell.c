#include <lib/stdio.h>
#include <lib/string.h>
#include <lib/syscall.h>

void _start()
{
	char command[128];
	int child_pid = 0;

	printf("\nArunOS shell\n"
	       "  ls / dir  - list files in the root directory\n"
	       "  hi_test   - enter a line and echo it back\n"
	       "  fk_test   - see fork create a child process\n"
	       "Run programs by typing their filename, then Enter.\n"
	       "To exit QEMU, press Ctrl-A, then X.\n\n");

	while (1) {
		printf("$ ");
		gets(command, sizeof(command));
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
		else if (child_pid < 0) {
			printf("Cannot create process\n");
		}
		else {
			wait(child_pid);
		}
	}

	exit(0);
}
