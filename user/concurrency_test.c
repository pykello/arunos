#include <lib/stdio.h>
#include <lib/syscall.h>

void _start()
{
	int i = 0;
	fork();

	for (i = 0; i < 50000; i++) {
		printf("step %d at pid %d\n", i, getpid());
		//yield();
	}

	exit(0);
}
