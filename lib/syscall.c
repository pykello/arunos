#include <lib/syscall.h>

void syscall1(enum SystemCallCode code, int arg1)
{
	(void) code;
	(void) arg1;
	__asm__ volatile("swi #1");
}

void syscall2(enum SystemCallCode code, int arg1, int arg2)
{
	(void) code;
	(void) arg2;
	__asm__ volatile("swi #2");
}

