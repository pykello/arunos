#include <lib/stdio.h>
#include <lib/syscall.h>

void _start()
{
	int i = 0;
	char c[128];
	char ch = 0;

	sprintf(c, "%s %s %d\n", "hello", "world", 1);

	while (c[i])
		syscall1(SYSCALL_PUTCH, c[i++]);

	while (ch != 'q') {
		ch = syscall0(SYSCALL_GETCH);
		if (ch != 0)
			syscall1(SYSCALL_PUTCH, ch);
	}

	syscall1(SYSCALL_PUTCH, '\n');
	syscall1(SYSCALL_EXIT, 0);
}
