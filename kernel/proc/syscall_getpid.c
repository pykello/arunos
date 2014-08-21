#include <ksyscall.h>
#include <proc.h>

int syscall_getpid(void)
{
	return current_process->pid;
}
