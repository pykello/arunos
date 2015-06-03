#include <proc.h>
#include <monitor.h>
#include <system.h>
#include <klib.h>
#include <timer.h>
#include <vm.h>
#include <irq.h>

static int round_robin_index;

void handle_timer(void)
{
	//schedule();
	kprintf("timer\n");
	timer_clear_interrupt();
	schedule();
}

void scheduler_init(void)
{
	round_robin_index = 0;
	timer_set_interval(100000);
	register_interrupt_handler(0x5, handle_timer);
}

void schedule(void)
{
	int i;
	kprintf("schedule \n");
	for (i = 0; i < PROCESS_COUNT_MAX; i++) {
		struct Process *proc = NULL;

		//kprintf("round_robin_index = %d\n", round_robin_index);
		//kprintf("&round_robin_index = %x\n", &round_robin_index);

		round_robin_index++;
		if (round_robin_index == PROCESS_COUNT_MAX)
		{
			round_robin_index = 0;
		}

		proc = &process_table[round_robin_index];
		kprintf("process[%d]->state = %d\n", round_robin_index, proc->state);

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
