#include <console.h>
#include <klib.h>
#include <ksyscall.h>
#include <lib/syscall.h>
#include <monitor.h>
#include <proc.h>
#include <system.h>

static int syscall_exit(int arg1);
static int syscall_putch(int arg1);
static int syscall_getch(void);

int handle_syscall(enum SystemCallCode code, int arg1, int arg2, int arg3)
{
	switch (code)
	{
	case SYSCALL_EXIT:
		return syscall_exit(arg1);
	case SYSCALL_PUTCH:
		return syscall_putch(arg1);
	case SYSCALL_GETCH:
		return syscall_getch();
	default:
		kprintf("handling syscall %d %d %d %d\n", code, arg1, arg2, arg3);
	}

	return -1;
}

static int syscall_exit(int arg1)
{
	(void) arg1;
	if (current_process == NULL)
		return -1;

	proc_free(current_process);
	current_process = NULL;

	enable_interrupts();
	monitor();

	return 0;
}

static int syscall_putch(int arg1)
{
	kputch(arg1);
	return 0;
}

static int syscall_getch(void)
{
	return kgetch();
}
