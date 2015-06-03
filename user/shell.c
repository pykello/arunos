#include <lib/stdio.h>
#include <lib/syscall.h>

void _start()
{
	int i = 0, j = 0;
	fork();
	fork();
	fork();

	while (1) {
		printf("step %d at pid %d\n", i, getpid());
		yield();
		i++;
	}

	exit(0);
}
