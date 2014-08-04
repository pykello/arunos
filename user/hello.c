#include <lib/stdio.h>
#include <lib/syscall.h>

void _start()
{
	char s[128];
	printf("%s %s %d\n", "hello", "world", 1);

	printf("Please enter a string: ");
	gets(s);
	printf("%s\n", s);

	exit(0);
}
