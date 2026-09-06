#include <lib/stdio.h>
#include <lib/syscall.h>

void _start()
{
	int pid = 0;
	int fork_result = 0;

	fork_result = fork();
	pid = getpid();
	printf("hello from pid: %d, fork returned: %d\n", pid, fork_result);

	exit(0);
}
