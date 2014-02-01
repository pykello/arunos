#include <monitor.h>
#include <lib/stdio.h>
#include <lib/string.h>
#include <system.h>
#include <types.h>

static void print_cpu_mode(int cpsr);
static void print_instruction_set(int cpsr);
static void print_irq_status(int cpsr);
static void print_fiq_status(int cpsr);
static void print_supervisor_sp(void);
static void print_irq_sp(void);
static void get_cpu_mode_string(int cpu_mode, char *result);
static void get_instruction_set_string(bool jazelle, bool thumb, char *result);

/* mon_status displays information about the current status of system. */
int mon_status(int argc, char **argv)
{
	int cpsr = 0;

	(void) argc;
	(void) argv;

	cpsr = read_cpsr();

	print_cpu_mode(cpsr);
	print_instruction_set(cpsr);
	print_irq_status(cpsr);
	print_fiq_status(cpsr);
	print_supervisor_sp();
	print_irq_sp();

	return 0;
}

static void print_cpu_mode(int cpsr)
{
	char cpu_mode_string[16];
	int cpu_mode = cpsr & 31;
	get_cpu_mode_string(cpu_mode, cpu_mode_string);
	printf("cpu mode: %s\n", cpu_mode_string);
}

static void print_instruction_set(int cpsr)
{
	char instruction_set_string[16];
	int jazelle = cpsr & (1 << 24);
	int thumb = cpsr & (1 << 5);
	get_instruction_set_string(jazelle, thumb, instruction_set_string);
	printf("instruction set: %s\n", instruction_set_string);
}

static void print_irq_status(int cpsr)
{
	int irq_disabled = cpsr & (1 << 7);
	char *irq_status = (irq_disabled ? "disabled" : "enabled");
	printf("irq: %s\n", irq_status);
}

static void print_fiq_status(int cpsr)
{
	int fiq_disabled = cpsr & (1 << 6);
	char *fiq_status = (fiq_disabled ? "disabled" : "enabled");
	printf("fiq: %s\n", fiq_status);
}

static void print_supervisor_sp(void)
{
	extern char kernel_stack_start[];
	char *kernel_stack_current = get_stack_pointer(CPU_MODE_SUPERVISOR);
	printf("supervisor stack pointer: %x (start), %x (current), %x (size)\n",
	       kernel_stack_start, kernel_stack_current,
	       kernel_stack_start - kernel_stack_current);
}

static void print_irq_sp(void)
{
	extern char irq_stack_start[];
	char *irq_stack_current = get_stack_pointer(CPU_MODE_IRQ);
	printf("irq stack pointer: %x (start), %x (current), %x (size)\n",
	      irq_stack_start, irq_stack_current,
	      irq_stack_start - irq_stack_current);
}

static void get_cpu_mode_string(int cpu_mode, char *result)
{
	if (cpu_mode == CPU_MODE_USER)
		strcpy(result, "user");
	else if (cpu_mode == CPU_MODE_SUPERVISOR)
		strcpy(result, "supervisor");
	else if (cpu_mode == CPU_MODE_FIQ)
		strcpy(result, "fiq");
	else if (cpu_mode == CPU_MODE_IRQ)
		strcpy(result, "irq");
	else if (cpu_mode == CPU_MODE_ABORT)
		strcpy(result, "abort");
	else if (cpu_mode == CPU_MODE_UNDEFINED)
		strcpy(result, "undefined");
	else if (cpu_mode == CPU_MODE_SYSTEM)
		strcpy(result, "system");
	else
		strcpy(result, "N/A");
}

static void get_instruction_set_string(bool jazelle, bool thumb, char *result)
{
	if (jazelle && !thumb)
		strcpy(result, "jazelle");
	else if (!jazelle && thumb)
		strcpy(result, "thumb");
	else if (!jazelle && !thumb)
		strcpy(result, "arm");
	else
		strcpy(result, "N/A");
}
