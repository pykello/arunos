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
	timer_clear_interrupt();
	schedule();
}

void scheduler_init(void)
{
	round_robin_index = 0;
	timer_set_interval(200000);
	register_interrupt_handler(TIMER_IRQ, handle_timer);
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

	/* no process? panic */
	mon_status(0, 0);
	dump_scheduler_status();
	kprintf("PANIC");
	while(1);
}

void dump_scheduler_status(void)
{
	int i;
	for (i = 0; i < PROCESS_COUNT_MAX; i++)
		if (i == round_robin_index)
			kprintf("v");
		else
			kprintf(" ");
	kprintf("\n");
	for (i = 0; i < PROCESS_COUNT_MAX; i++)
		kprintf("%d", i);
	kprintf("\n");
	for (i = 0; i < PROCESS_COUNT_MAX; i++)
		kprintf("%d", process_table[i].state);
	kprintf("\n");
}
