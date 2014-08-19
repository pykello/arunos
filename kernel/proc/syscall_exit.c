#include <ksyscall.h>
#include <proc.h>

int syscall_exit(int arg1)
{
	(void) arg1;
	if (current_process == NULL)
		return -1;

	proc_free(current_process);
	schedule();

	return 0;	
}
