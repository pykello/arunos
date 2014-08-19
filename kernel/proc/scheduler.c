#include <proc.h>
#include <monitor.h>
#include <system.h>
#include <vm.h>

static int round_robin_index;

void scheduler_init(void)
{
	round_robin_index = 0;
}

void schedule(void)
{
	int i;
	for (i = 0; i < PROCESS_COUNT_MAX; i++) {
		struct Process *proc = NULL;

		round_robin_index++;
		if (round_robin_index == PROCESS_COUNT_MAX)
		{
			round_robin_index = 0;
		}

		proc = &process_table[round_robin_index];
		if (proc->state == READY) {
			current_process = proc;
			proc_start(proc);
		}
	}

	/* no process is ready to run, run monitor */
	set_translation_table_base((uint32_t) V2P(kernel_vm));
	current_process = NULL;
	__asm__ volatile("ldr sp, =kernel_stack_start");
	enable_interrupts();
	monitor();
}
