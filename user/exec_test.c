#include <lib/stdio.h>
#include <lib/syscall.h>

void _start()
{
	if (fork() == 0) {
		printf("starting program 1 ...\n");
		exec(1);
	}
	else {
		printf("message from parent.\n");
	}

	exit(0);
}
