#include "monitor.h"
#include "lib/stdio.h"
#include "lib/string.h"
#include "system.h"
#include "types.h"


static void get_cpu_mode_string(CPUModeType cpu_mode, char *cpu_mode_string);
static void get_instruction_set_string(bool jazelle, bool thumb,
									   char *instruction_set_string);


/* mon_status displays information about the current status of system. */
int
mon_status(int argc, char **argv)
{
	int cpsr = 0;

	(void) argc;
	(void) argv;

	cpsr = read_cpsr();

	/* print cpu mode */
	{
		char cpu_mode_string[10];
		CPUModeType cpu_mode = cpsr & 31;
		get_cpu_mode_string(cpu_mode, cpu_mode_string);
		printf("cpu mode: %s\n", cpu_mode_string);
	}

	/* print instruction set */
	{
		char instruction_set_string[10];
		int jazelle = cpsr & (1 << 24);
		int thumb = cpsr & (1 << 5);
		get_instruction_set_string(jazelle, thumb, instruction_set_string);
		printf("instruction set: %s\n", instruction_set_string);
	}

	/* print irq status */
	{
		int irq_disabled = cpsr & (1 << 7);
		char *irq_status = (irq_disabled ? "disabled" : "enabled");
		printf("irq: %s\n", irq_status);
	}

	/* print fiq status */
	{
		int fiq_disabled = cpsr & (1 << 6);
		char *fiq_status = (fiq_disabled ? "disabled" : "enabled");
		printf("fiq: %s\n", fiq_status);
	}

	/* print supervisor stack pointer */
	{
		extern char kernel_stack_start[];
		char *kernel_stack_current = get_stack_pointer(CPU_MODE_SUPERVISOR);
		printf("supervisor stack pointer: %x (start), %x (current), %x (size)\n",
			   kernel_stack_start, kernel_stack_current,
			   kernel_stack_start - kernel_stack_current);
	}

	/* print irq stack pointer */
	{
		extern char irq_stack_start[];
		char *irq_stack_current = get_stack_pointer(CPU_MODE_IRQ);
		printf("irq stack pointer: %x (start), %x (current), %x (size)\n",
			   irq_stack_start, irq_stack_current,
			   irq_stack_start - irq_stack_current);
	}

	return 0;
}


static void
get_cpu_mode_string(CPUModeType cpu_mode, char *cpu_mode_string)
{
	if (cpu_mode == CPU_MODE_USER)
	{
		strcpy(cpu_mode_string, "user");
	}
	else if (cpu_mode == CPU_MODE_SUPERVISOR)
	{
		strcpy(cpu_mode_string, "supervisor");
	}
	else if (cpu_mode == CPU_MODE_FIQ)
	{
		strcpy(cpu_mode_string, "fiq");
	}
	else if (cpu_mode == CPU_MODE_IRQ)
	{
		strcpy(cpu_mode_string, "irq");
	}
	else if (cpu_mode == CPU_MODE_ABORT)
	{
		strcpy(cpu_mode_string, "abort");
	}
	else if (cpu_mode == CPU_MODE_UNDEFINED)
	{
		strcpy(cpu_mode_string, "undefined");
	}
	else if (cpu_mode == CPU_MODE_SYSTEM)
	{
		strcpy(cpu_mode_string, "system");
	}
	else
	{
		strcpy(cpu_mode_string, "N/A");
	}
}


static void
get_instruction_set_string(bool jazelle, bool thumb, char *instruction_set_string)
{
	if (jazelle && !thumb)
	{
		strcpy(instruction_set_string, "jazelle");
	}
	else if (!jazelle && thumb)
	{
		strcpy(instruction_set_string, "thumb");
	}
	else if (!jazelle && !thumb)
	{
		strcpy(instruction_set_string, "arm");
	}
	else
	{
		strcpy(instruction_set_string, "N/A");
	}
}
