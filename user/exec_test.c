#include <lib/stdio.h>
#include <lib/syscall.h>

void _start()
{
	if (fork() == 0) {
		printf("starting program 0 ...\n");
		exec(0);
	}
	else {
		printf("message from parent.\n");
	}

	exit(0);
}
