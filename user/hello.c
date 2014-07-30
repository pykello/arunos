#include <lib/stdio.h>
#include <lib/syscall.h>

void _start()
{
	int i = 0;
	char c[128];
	sprintf(c, "%s %s %d\n", "hello", "world", 1);

	while (c[i])
		syscall1(SYSCALL_PUTCH, c[i++]);

	syscall1(SYSCALL_EXIT, 0);
}
