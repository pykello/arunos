#include <console.h>
#include <klib.h>
#include <ksyscall.h>
#include <lib/syscall.h>
#include <monitor.h>
#include <proc.h>
#include <system.h>

static void syscall_exit(int arg1);
static void syscall_putch(int arg1);

void handle_syscall(enum SystemCallCode code, int arg1, int arg2, int arg3)
{
	switch (code)
	{
	case SYSCALL_EXIT:
		syscall_exit(arg1);
		break;
	case SYSCALL_PUTCH:
		syscall_putch(arg1);
		break;
	default:
		kprintf("handling syscall %d %d %d %d\n", code, arg1, arg2, arg3);
	}
}

static void syscall_exit(int arg1)
{
	(void) arg1;
	if (current_process == NULL)
		return;

	proc_free(current_process);
	current_process = NULL;

	enable_interrupts();
	monitor();
}

static void syscall_putch(int arg1)
{
	putch(arg1);
}
