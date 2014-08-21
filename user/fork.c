#include <lib/stdio.h>
#include <lib/syscall.h>

void _start()
{
	int pid = 0;

	fork();

	pid = getpid();
	printf("hello from pid: %d\n", pid);

	exit(0);
}
