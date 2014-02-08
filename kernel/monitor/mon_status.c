#include <monitor.h>
#include <lib/stdio.h>
#include <lib/string.h>
#include <system.h>
#include <types.h>

static void print_cpu_id(void);
static void print_cpsr_status(void);
static void print_supervisor_sp(void);
static void print_irq_sp(void);
static uint32_t get_bits(uint32_t n, uint32_t bitmask);

/* mon_status displays information about the current status of system. */
int mon_status(int argc, char **argv)
{
	(void) argc;
	(void) argv;

	print_cpu_id();
	print_cpsr_status();
	print_supervisor_sp();
	print_irq_sp();

	return 0;
}

static void print_cpu_id(void)
{
	int cpu_id = read_cpu_id();
	int implementor = get_bits(cpu_id, CPU_ID_IMPLEMENTOR);
	int architecture = get_bits(cpu_id, CPU_ID_ARCH_CODE);
	int part_number = get_bits(cpu_id, CPU_ID_PART_NUMBER);

	printf("cpu: implementor=%s, architecture=%s, part_number=%x\n",
	       implementor_string[implementor],
	       architecture_code_string[architecture],
	       part_number);
}

static void print_cpsr_status(void)
{
	int cpsr = read_cpsr();
	int cpu_mode = get_bits(cpsr, CPSR_CPU_MODE);
	int instruction_set = get_bits(cpsr, CPSR_INSTRUCTION_SET);
	int irq_disabled = get_bits(cpsr, CPSR_IRQ_DISABLED);
	int fiq_disabled = get_bits(cpsr, CPSR_FIQ_DISABLED);

	printf("cpsr: cpu_mode=%s, instruction_set=%s, irq=%s, fiq=%s\n",
	       cpu_mode_string[cpu_mode],
	       instruction_set_string[instruction_set],
	       irq_disabled ? "disabled" : "enabled",
	       fiq_disabled ? "disabled" : "enabled");
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

static uint32_t get_bits(uint32_t n, uint32_t bitmask)
{
	uint32_t result = 0;
	int i = 0;

	for (i = 31; i >= 0; i--)
		if (bitmask & (1u << i))
			result = result * 2 + ((n & (1u << i)) ? 1 : 0);

	return result;
}
