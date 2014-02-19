#include <memory.h>
#include <lib/string.h>
#include <system.h>
#include <types.h>

static void setup_section_table(void);
static void setup_page_table(void);
static void setup_mmu_registers(void);

void memory_init(void)
{
	setup_section_table();
	setup_page_table();
	setup_mmu_registers();
}

static void setup_section_table(void)
{
	extern struct SectionTableEntry section_table[];
	extern struct PageTableEntry page_table[];
	int i = 0;
	memset(section_table, 0, SECTION_COUNT * 4);

	for (i = 0; i < SECTION_COUNT; i++) {
		section_table[i].desc_type = SECTION_DESC_TYPE_COARSE;
		section_table[i].domain = 0;
		section_table[i].base_address = ((int) page_table >> 10) + i;
	}
}

static void setup_page_table(void)
{
	extern struct PageTableEntry page_table[];
	int i = 0;
	memset(page_table, 0, PAGE_COUNT * 4);

	for (i = 0; i < PAGE_COUNT; i++) {
		page_table[i].desc_type = PAGE_DESC_TYPE_SMALL;
		page_table[i].bufferable = 0;
		page_table[i].cacheable = 0;
		page_table[i].access_permissions_0 = ACCESS_PERMISSION_RW_R;
		page_table[i].access_permissions_1 = ACCESS_PERMISSION_RW_R;
		page_table[i].access_permissions_2 = ACCESS_PERMISSION_RW_R;
		page_table[i].access_permissions_3 = ACCESS_PERMISSION_RW_R;
		page_table[i].base_address = i % (PAGE_COUNT / 2);
	}
}

static void setup_mmu_registers(void)
{
	extern struct SectionTableEntry section_table[];
	uint32_t control_register = 0;

	set_domain_access_control(1);
	set_translation_table_base((uint32_t) section_table);

	control_register = read_control_register();
	control_register |= 1;
	set_control_register(control_register);
}
