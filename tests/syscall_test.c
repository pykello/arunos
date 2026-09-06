#include <lib/stdio.h>
#include <lib/syscall.h>

void _start(void)
{
	int invalid[] = {-1, 9, 255, 256, 0x10003, 0x7fffffff};
	unsigned int i;
	int pid = getpid();

	for (i = 0; i < sizeof(invalid) / sizeof(invalid[0]); i++)
		if (syscall0(invalid[i]) != -1 || getpid() != pid) {
			printf("FAIL: invalid syscall\n");
			while (1) yield();
		}
	printf("invalid syscall numbers rejected\n");
	while (1) yield();
}
