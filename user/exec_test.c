#include <lib/stdio.h>
#include <lib/syscall.h>

void _start()
{
	int child_pid = fork();
	if (child_pid == 0) {
		printf("starting program 1 ...\n");
		exec(1);
	}
	else {
		printf("message from parent.\n");
		wait(child_pid);
	}

	exit(0);
}
