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

void clean_low_mem(void)
{	
	memset(page_table, 0, PAGE_COUNT * 2);
}

static void setup_section_table(void)
{
	struct SectionTableEntry *section_table_physical = NULL;
	struct PageTableEntry *page_table_physical = NULL;

	section_table_physical = (struct SectionTableEntry *) KERNEL_V2P(section_table);
	page_table_physical = (struct PageTableEntry *) KERNEL_V2P(page_table);

	int i = 0;
	memset(section_table_physical, 0, SECTION_COUNT * 4);

	for (i = 0; i < SECTION_COUNT; i++) {
		section_table_physical[i].desc_type = SECTION_DESC_TYPE_COARSE;
		section_table_physical[i].domain = 0;
		section_table_physical[i].base_address = ((int) page_table_physical >> 10) + i;
	}
}

static void setup_page_table(void)
{
	struct PageTableEntry *page_table_physical = NULL;
	int i = 0;

	page_table_physical = (struct PageTableEntry *) KERNEL_V2P(page_table);
	memset(page_table_physical, 0, PAGE_COUNT * 4);

	for (i = 0; i < PAGE_COUNT; i++) {
		page_table_physical[i].desc_type = PAGE_DESC_TYPE_SMALL;
		page_table_physical[i].bufferable = 0;
		page_table_physical[i].cacheable = 0;
		page_table_physical[i].access_permissions_0 = ACCESS_PERMISSION_RW_R;
		page_table_physical[i].access_permissions_1 = ACCESS_PERMISSION_RW_R;
		page_table_physical[i].access_permissions_2 = ACCESS_PERMISSION_RW_R;
		page_table_physical[i].access_permissions_3 = ACCESS_PERMISSION_RW_R;

		if (i < PAGE_COUNT - 16)
			page_table_physical[i].base_address = i % (PAGE_COUNT / 2);
		else
			page_table_physical[i].base_address = i + 16 - PAGE_COUNT;
	}
}

static void setup_mmu_registers(void)
{
	uint32_t control_register = 0;

	set_domain_access_control(1);
	set_translation_table_base(KERNEL_V2P(section_table));

	control_register = read_control_register();
	control_register |= 1;
	set_control_register(control_register);
}
