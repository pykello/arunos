#include <ksyscall.h>
#include <klib.h>
#include <lib/syscall.h>
#include <proc.h>
#include <monitor.h>
#include <system.h>

static void syscall_exit(int arg1);

void handle_syscall(enum SystemCallCode code, int arg1, int arg2, int arg3)
{
	switch (code)
	{
	case SYSCALL_EXIT:
		syscall_exit(arg1);
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
	enable_interrupts();
	monitor();
}
