#include <lib/stdio.h>
#include <lib/syscall.h>

void _start()
{
	char c = 0;
	printf("%s %s %d\n", "hello", "world", 1);

	while (c != 'q') {
		c = getch();
		putch(c);
	}
	putch('\n');

	exit(0);
}
