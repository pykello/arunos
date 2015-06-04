#include <ksyscall.h>
#include <proc.h>
#include <system.h>

int syscall_wait(int arg1)
{
	current_process->wait_pid = arg1;
	current_process->state = SLEEPING;
	schedule();

	return 0;
}
