#include <ksyscall.h>
#include <proc.h>
#include <system.h>

int syscall_exit(int arg1)
{
	(void) arg1;
	if (current_process == NULL)
		return -1;

	set_translation_table_base((uint32_t) V2P(kernel_vm));
	__asm__ volatile("ldr sp, =kernel_stack_start");

	proc_free(current_process);
	current_process = NULL;

	schedule();

	return 0;
}
