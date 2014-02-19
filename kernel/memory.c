#include <memory.h>
#include <lib/string.h>
#include <system.h>
#include <types.h>

static void setup_section_table(void);
static void setup_mmu_registers(void);

void memory_init(void)
{
	setup_section_table();
	setup_mmu_registers();
}

static void setup_section_table(void)
{
	extern struct L1SectionVirtPageDesc section_table[];
	int i = 0;
	memset(section_table, 0, 4096 * 4);

	for (i = 0; i < 4096; i++) {
		section_table[i].desc_type = 2;
		section_table[i].domain = 0;
		section_table[i].access_permissions = 3;
		section_table[i].physical_base_address = i % 2048;
	}
}

static void setup_mmu_registers(void)
{
	extern struct L1SectionVirtPageDesc section_table[];
	uint32_t control_register = 0;

	set_domain_access_control(1);
	set_translation_table_base((uint32_t) section_table);

	control_register = read_control_register();
	control_register |= 1;
	set_control_register(control_register);
}
