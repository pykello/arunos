#include <ksyscall.h>
#include <proc.h>
#include <monitor.h>
#include <system.h>
#include <vm.h>

int syscall_exit(int arg1)
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