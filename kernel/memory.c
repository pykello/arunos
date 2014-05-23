#include <memory.h>
#include <lib/string.h>
#include <system.h>
#include <types.h>

static struct SectionTableEntry *setup_kernel_vm(void);
static void map_pages(struct SectionTableEntry *vm, struct MemoryMapping *mapping);
static void map_page(struct SectionTableEntry *vm, uint32_t physical, uint32_t virtual,
		     int access_permissions);
static void switch_vm(struct SectionTableEntry *vm);
static void *boot_alloc(uint32_t n, uint32_t alignment);
static uint32_t resolve_physical_address(struct SectionTableEntry *vm, uint32_t virtual);

extern char kernel_end[];

struct MemoryMapping kernel_mappings[] = {
	{KERNEL_BASE, 0, V2P(kernel_end), AP_RW_R},
	{UART0_BASE, UART0_PHYSICAL, UART0_PHYSICAL + PAGE_SIZE, AP_RW_R},
	{PIC_BASE, PIC_PHYSICAL, PIC_PHYSICAL + PAGE_SIZE, AP_RW_R},
	{INTERRUPT_VECTOR_BASE, 0, PAGE_SIZE, AP_RW_R}
};

const int kernel_mapping_count = 4;

/*
 * memory_init sets up a two level page table. This function only sets
 * up the kernel part of the address space. The user part of the address
 * space will be setup later.
 */
void memory_init(void)
{
	struct SectionTableEntry *kernel_vm = setup_kernel_vm();
	uint32_t a = resolve_physical_address(kernel_vm, KERNEL_BASE);
	a = resolve_physical_address(kernel_vm, UART0_BASE);
	a = resolve_physical_address(kernel_vm, PIC_BASE);
	switch_vm((struct SectionTableEntry *) V2P(kernel_vm));
}

static struct SectionTableEntry *setup_kernel_vm(void)
{
	struct SectionTableEntry *kernel_vm = NULL;
	int i = 0;

	/* allocate initial section table */
	kernel_vm = boot_alloc(SECTION_TABLE_SIZE, SECTION_TABLE_ALIGNMENT);
	memset(kernel_vm, 0, SECTION_TABLE_SIZE);

	for (i = 0; i < kernel_mapping_count; i++) {
		struct MemoryMapping *mapping = &kernel_mappings[i];
		map_pages(kernel_vm, mapping);
	}

	return kernel_vm;
}

static void map_pages(struct SectionTableEntry *vm, struct MemoryMapping *mapping)
{
	uint32_t physical_current = 0;
	uint32_t virtual_current = 0;

	uint32_t virtual_start = (mapping->virtual_address) & ~(PAGE_SIZE - 1);
	uint32_t physical_start = (mapping->physical_start) & ~(PAGE_SIZE - 1);
	uint32_t physical_end = (mapping->physical_end + PAGE_SIZE - 1) &
				~(PAGE_SIZE - 1);

	virtual_current = virtual_start;
	for (physical_current = physical_start; physical_current != physical_end;
	     physical_current += PAGE_SIZE)
	{
		map_page(vm, physical_current, virtual_current,
			 mapping->access_permissions);
		virtual_current += PAGE_SIZE;
	}
}

static void map_page(struct SectionTableEntry *vm, uint32_t physical, uint32_t virtual,
		     int access_permissions)
{
	struct PageTableEntry *page_table = NULL;

	uint32_t section_index = SECTION_INDEX(virtual);
	uint32_t page_index = PAGE_INDEX(virtual);
	uint32_t base_address = vm[section_index].base_address;

	if (base_address == 0) {
		page_table = boot_alloc(PAGE_TABLE_SIZE, PAGE_TABLE_ALIGNMENT);
		vm[section_index].base_address = PAGE_TABLE_TO_BASE(V2P(page_table));
		vm[section_index].desc_type = SECTION_DESC;
		vm[section_index].domain = 0;
	}
	else {
		page_table = (void *) P2V(BASE_TO_PAGE_TABLE(vm[section_index].base_address));
	}

	page_table[page_index].desc_type = PAGE_DESC,
	page_table[page_index].bufferable = 0;
	page_table[page_index].cacheable = 0;
	page_table[page_index].access_permissions = access_permissions;
	page_table[page_index].base_address = (physical >> 12);
}

static void switch_vm(struct SectionTableEntry *vm)
{
	set_translation_table_base((uint32_t) vm);
}

/* physical memory allocator used while setting up virtual memory system. */
static void *boot_alloc(uint32_t n, uint32_t alignment)
{
	char *result = NULL;
	static char *next_free = NULL;
	extern char kernel_end[];

	if (next_free == NULL) {
		next_free = kernel_end;
	}

	next_free = (char *) (((uint32_t) next_free + alignment - 1) & ~(alignment - 1));
	result = next_free;
	next_free += n;

	return result;
}

static uint32_t resolve_physical_address(struct SectionTableEntry *vm, uint32_t virtual)
{
	struct SectionTableEntry *section = NULL;
	struct PageTableEntry *page = NULL;
	uint32_t result = 0;
	void *base_address = 0;

	section = (void *)((uint32_t) vm | ((virtual >> 20) << 2));
	base_address = (void *) (section->base_address << 10);
	page = (void *)((uint32_t)base_address | ((virtual >> 10) & 0x3fc));
	page = (void *)P2V(page);
	result = (page->base_address << 12) | (virtual & 0xfff);

	return result;
}