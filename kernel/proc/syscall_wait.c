#include <ksyscall.h>
#include <proc.h>
#include <system.h>
#include <klib.h>


int syscall_wait(int arg1)
{
	struct Process *proc = proc_get(arg1);
	if (proc && proc->state != UNUSED) {
		current_process->wait_pid = arg1;
		current_process->state = SLEEPING;
		schedule();
	}

	return 0;
}
