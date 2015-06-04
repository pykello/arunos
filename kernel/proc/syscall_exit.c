#include <ksyscall.h>
#include <proc.h>
#include <system.h>

int syscall_exit(int arg1)
{
	int pid;

	(void) arg1;
	if (current_process == NULL)
		return -1;

	set_translation_table_base((uint32_t) V2P(kernel_vm));
	__asm__ volatile("ldr sp, =kernel_stack_start");

	for (pid = 0; pid < PROCESS_COUNT_MAX; pid++) {
		struct Process *proc = &process_table[pid];

		if (proc->state == SLEEPING &&
		    proc->wait_pid == current_process->pid) {
		    	
			proc->wait_pid = -1;
			proc->state = READY;
		}
	}

	proc_free(current_process);
	current_process = NULL;

	schedule();

	return 0;
}
