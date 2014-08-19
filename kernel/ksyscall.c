#include <console.h>
#include <klib.h>
#include <ksyscall.h>
#include <lib/string.h>
#include <lib/syscall.h>
#include <monitor.h>
#include <proc.h>
#include <system.h>

static int syscall_exit(int arg1);
static int syscall_putch(int arg1);
static int syscall_getch(void);

static int (*const syscall_handler[])() = {
	[SYSCALL_EXIT] = syscall_exit,
	[SYSCALL_PUTCH] = syscall_putch,
	[SYSCALL_GETCH] = syscall_getch
};

/* kernel side of system calls. */
int handle_syscall(enum SystemCallCode code, int arg1, int arg2, int arg3)
{
	return syscall_handler[code](arg1, arg2, arg3);
}

static int syscall_exit(int arg1)
{
	(void) arg1;
	if (current_process == NULL)
		return -1;

	set_translation_table_base((uint32_t) V2P(kernel_vm));
	proc_free(current_process);
	current_process = NULL;

	__asm__ volatile("ldr sp, =kernel_stack_start");
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
