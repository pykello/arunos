#include <console.h>
#include <fat.h>
#include <klib.h>
#include <lib/syscall.h>
#include <proc.h>

/* Print the root files without adding directory handles or user buffers. */
static int syscall_dir(void)
{
	char name[13];
	int cursor = 0;

	while ((cursor = fat_next(cursor, name)) > 0)
		kprintf("%s\n", name);
	return cursor;
}

static int (*const syscall_handler[])() = {
	[SYSCALL_EXIT] = syscall_exit,
	[SYSCALL_PUTCH] = syscall_putch,
	[SYSCALL_GETCH] = syscall_getch,
	[SYSCALL_GETPID] = syscall_getpid,
	[SYSCALL_FORK] = syscall_fork,
	[SYSCALL_EXEC] = syscall_exec,
	[SYSCALL_YIELD] = syscall_yield,
	[SYSCALL_WAIT] = syscall_wait,
	[SYSCALL_DIR] = syscall_dir
};

/* kernel side of system calls. */
int handle_syscall(enum SystemCallCode code, int arg1, int arg2, int arg3)
{
	return syscall_handler[code](arg1, arg2, arg3);
}
