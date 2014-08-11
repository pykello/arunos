#include <monitor.h>
#include <kalloc.h>
#include <klib.h>
#include <lib/string.h>
#include <system.h>
#include <types.h>

static void print_cpu_id(void);
static void print_cache_type(void);
static void print_tcm_type(void);
static void print_tlb_type(void);
static void print_control_register(void);
static void print_cpsr_status(void);
static void print_supervisor_sp(void);
static void print_irq_sp(void);
static void print_free_memory(void);
static uint32_t get_bits(uint32_t n, uint32_t bitmask);

/* mon_status displays information about the current status of system. */
int mon_status(int argc, char **argv)
{
	(void) argc;
	(void) argv;

	print_cpu_id();
	print_cache_type();
	print_tcm_type();
	print_tlb_type();
	print_control_register();
	print_cpsr_status();
	print_supervisor_sp();
	print_irq_sp();
	print_free_memory();

	return 0;
}

static void print_cpu_id(void)
{
	int cpu_id = read_cpu_id();
	int implementor = get_bits(cpu_id, CPU_ID_IMPLEMENTOR);
	int architecture = get_bits(cpu_id, CPU_ID_ARCH_CODE);
	int part_number = get_bits(cpu_id, CPU_ID_PART_NUMBER);

	kprintf("cpu: implementor=%s, architecture=%s, part_number=%x\n",
	        implementor_string[implementor],
	        architecture_code_string[architecture],
	        part_number);
}

static void print_cache_type(void)
{
	int cache_type = read_cache_type();
	bool separate = get_bits(cache_type, CACHE_TYPE_SEPARATE);
	int data_size = get_bits(cache_type, CACHE_TYPE_DATA_SIZE);
	int instruction_size = get_bits(cache_type, CACHE_TYPE_INSTRUCTION_SIZE);

	kprintf("cache: type=%s, data_size=%d, instruction_size=%d\n",
		separate ? "separate" : "unified",
		data_size, instruction_size);
}

static void print_tcm_type(void)
{
	int tcm_type = read_tcm_type();
	int data_count = get_bits(tcm_type, TCM_TYPE_DATA_COUNT);
	int instruction_count = get_bits(tcm_type, TCM_TYPE_INSTRUCTION_COUNT);

	kprintf("tightly coupled memory: data_count=%d, instruction_count=%d\n",
		data_count, instruction_count);
}

static void print_tlb_type(void)
{
	int tlb_type = read_tlb_type();
	int separate = get_bits(tlb_type, TLB_TYPE_SEPARATE);
	int instruction_locks = get_bits(tlb_type, TLB_TYPE_INSTRUCTION_LOCKS);
	int data_locks = get_bits(tlb_type, TLB_TYPE_DATA_LOCKS);

	kprintf("translation lookaside buffers: type=%s, instruction_locks=%d, "
		"data_locks=%d\n", separate ? "separate" : "unified",
		instruction_locks, data_locks);
}

static void print_control_register(void)
{
	int control = read_control_register();
	int mmu_enabled = get_bits(control, CONTROL_MMU_ENABLED);
	int alignment_strict = get_bits(control, CONTROL_ALIGNMENT_STRICT);
	int l1_data_enabled = get_bits(control, CONTROL_L1_DATA_CACHE_ENABLED);
	int l1_inst_enabled = get_bits(control, CONTROL_L1_INST_CACHE_ENABLED);
	int write_buffer_enabled = get_bits(control, CONTROL_WRITE_BUFFER_ENABLED);
	int high_exception_vectors = get_bits(control, CONTROL_HIGH_EXCEPTION_VECTORS);
	kprintf("control register: mmu %s, alignment %s, l1 data %s, "
		"l1 instruction %s, writer buffer %s, %s exception vectors\n",
		mmu_enabled ? "enabled" : "disabled",
		alignment_strict ? "strict" : "not strict",
		l1_data_enabled ? "enabled" : "disabled",
		l1_inst_enabled ? "enabled" : "disabled",
		write_buffer_enabled ? "enabled" : "disabled",
		high_exception_vectors ? "high" : "low");
}

static void print_cpsr_status(void)
{
	int cpsr = read_cpsr();
	int cpu_mode = get_bits(cpsr, CPSR_CPU_MODE);
	int instruction_set = get_bits(cpsr, CPSR_INSTRUCTION_SET);
	int irq_disabled = get_bits(cpsr, CPSR_IRQ_DISABLED);
	int fiq_disabled = get_bits(cpsr, CPSR_FIQ_DISABLED);

	kprintf("cpsr: cpu_mode=%s, instruction_set=%s, irq=%s, fiq=%s\n",
		cpu_mode_string[cpu_mode],
		instruction_set_string[instruction_set],
		irq_disabled ? "disabled" : "enabled",
		fiq_disabled ? "disabled" : "enabled");
}

static void print_supervisor_sp(void)
{
	extern char kernel_stack_start[];
	char *kernel_stack_current = get_stack_pointer(CPU_MODE_SUPERVISOR);
	kprintf("supervisor stack pointer: %x (start), %x (current), %x (size)\n",
		kernel_stack_start, kernel_stack_current,
		kernel_stack_start - kernel_stack_current);
}

static void print_irq_sp(void)
{
	extern char irq_stack_start[];
	char *irq_stack_current = get_stack_pointer(CPU_MODE_IRQ);
	kprintf("irq stack pointer: %x (start), %x (current), %x (size)\n",
		irq_stack_start, irq_stack_current,
		irq_stack_start - irq_stack_current);
}

static void print_free_memory(void)
{
	uint32_t free_memory_size = get_free_memory_size();
	kprintf("free memory size: %d (bytes)\n", free_memory_size);
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
