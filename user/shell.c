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
			if (exec(program_idx) < 0)
				printf("cannot execute %d\n", program_idx);
			exit(1);
		}
		else {
			wait(child_pid);
		}
	}

	exit(0);
}
