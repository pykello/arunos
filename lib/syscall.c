#include <lib/syscall.h>

int syscall0(enum SystemCallCode code)
{
	int result;

	__asm__ volatile("ldr r0, %0" : : "m" (code));
	__asm__ volatile("swi #0");
	__asm__ volatile("str r0, %0" : "=m" (result));
	
	return result;
}

int syscall1(enum SystemCallCode code, int arg1)
{
	int result;

	__asm__ volatile("ldr r0, %0" : : "m" (code));
	__asm__ volatile("ldr r1, %0" : : "m" (arg1));
	__asm__ volatile("swi #0");
	__asm__ volatile("str r0, %0" : "=m" (result));
	
	return result;
}

int syscall2(enum SystemCallCode code, int arg1, int arg2)
{
	int result;

	__asm__ volatile("ldr r0, %0" : : "m" (code));
	__asm__ volatile("ldr r1, %0" : : "m" (arg1));
	__asm__ volatile("ldr r2, %0" : : "m" (arg2));
	__asm__ volatile("swi #0");
	__asm__ volatile("str r0, %0" : "=m" (result));
	
	return result;
}

void putch(int c)
{
	syscall1(SYSCALL_PUTCH, c);
}

int getch(void)
{
	return syscall0(SYSCALL_GETCH);
}

void exit(int code)
{
	syscall1(SYSCALL_EXIT, code);
}

int getpid(void)
{
	return syscall0(SYSCALL_GETPID);
}

int fork(void)
{
	return syscall0(SYSCALL_FORK);
}

void exec(int id)
{
	syscall1(SYSCALL_EXEC, id);
}

void yield(void)
{
	syscall0(SYSCALL_YIELD);
}

void wait(int id)
{
	syscall1(SYSCALL_WAIT, id);
}
