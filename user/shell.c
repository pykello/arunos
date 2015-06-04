#include <lib/stdio.h>
#include <lib/syscall.h>

void _start()
{
	char command[128];
	int program_idx = 0;
	int child_pid = 0;

	while (1) {
		printf("$ ");
		gets(command);
		if (command[0] < '0' || command[0] > '9')
			continue;

		program_idx = command[0] - '0';
		child_pid = fork();
		if (child_pid == 0) {
			exec(program_idx);
		}
		else {
			wait(child_pid);
		}
	}

	exit(0);
}
