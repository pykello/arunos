#include <memory.h>
#include <lib/string.h>
#include <kalloc.h>
#include <system.h>
#include <types.h>

/* forward declarations for local functions */
static struct SectionTableEntry *setup_kernel_vm(void);
static void map_pages(struct SectionTableEntry *vm,
		      struct MemoryMapping *mapping);
static void map_page(struct SectionTableEntry *vm, uint32_t physical,
		     uint32_t virtual, int access_permissions);
static struct PageTableEntry *allocate_page_table();

/* kernel virtual to physical memory mappings */
static struct MemoryMapping kernel_mappings[] = {
	{KERNEL_BASE, 0, V2P(kernel_end), AP_RW_D},
	{UART0_BASE, UART0_PHYSICAL, UART0_PHYSICAL + PAGE_SIZE, AP_RW_D},
	{PIC_BASE, PIC_PHYSICAL, PIC_PHYSICAL + PAGE_SIZE, AP_RW_D},
	{GPIO_BASE, GPIO_PHYSICAL, GPIO_PHYSICAL + PAGE_SIZE, AP_RW_D},
	{INTERRUPT_VECTOR_BASE, 0, PAGE_SIZE, AP_RW_D},
	{ALLOCATABLE_MEMORY_START, V2P(ALLOCATABLE_MEMORY_START),
		TOTAL_MEMORY_SIZE, AP_RW_D}
};

const int kernel_mapping_count = 6;

static char *page_table_allocated_page = NULL;
static int page_table_page_offset = 0;

/*
 * memory_init sets up a two level page table. This function only sets
 * up the kernel part of the address space. The user part of the address
 * space will be setup later.
 */
void memory_init(void)
{
	struct SectionTableEntry *kernel_vm = NULL;

	page_table_allocated_page = NULL;
	page_table_page_offset = 0;

	kernel_vm = setup_kernel_vm();

	/*
	 * Use physical address of kernel virtual memory as the new virtual
	 * memory translation table base.
	 */
	set_translation_table_base((uint32_t) V2P(kernel_vm));
}

/*
 * setup_kernel_vm allocates and creates the kernel virtual memory and returns
 * a pointer to it. This function doesn't activate the virtual memory though.
 */
static struct SectionTableEntry *setup_kernel_vm(void)
{
	struct SectionTableEntry *kernel_vm = NULL;
	int i = 0;

	/* allocate initial section table */
	kernel_vm = (void *) ROUND_UP(KERNEL_SECTION_TABLE,
				      SECTION_TABLE_ALIGNMENT);
	memset(kernel_vm, 0, SECTION_TABLE_SIZE);

	/* add each of the mappings */
	for (i = 0; i < kernel_mapping_count; i++) {
		struct MemoryMapping *mapping = &kernel_mappings[i];
		map_pages(kernel_vm, mapping);
	}

	return kernel_vm;
}

/*
 * map_pages adds the given virtual to physical memory mapping to the given
 * virtual memory. A mapping can map multiple pages.
 */
static void map_pages(struct SectionTableEntry *vm, struct MemoryMapping *mapping)
{
	uint32_t physical_current = 0;
	uint32_t virtual_current = 0;

	uint32_t virtual_start = ROUND_DOWN(mapping->virtual_address, PAGE_SIZE);
	uint32_t physical_start = ROUND_DOWN(mapping->physical_start, PAGE_SIZE);
	uint32_t physical_end = ROUND_UP(mapping->physical_end, PAGE_SIZE);

	/* iterate over pages and map each page */
	virtual_current = virtual_start;
	for (physical_current = physical_start; physical_current != physical_end;
	     physical_current += PAGE_SIZE)
	{
		map_page(vm, physical_current, virtual_current,
			 mapping->access_permissions);
		virtual_current += PAGE_SIZE;
	}
}

/*
 * map_page adds to the given virtual memory the mapping of a single virtual page
 * to a physical page.
 */
static void map_page(struct SectionTableEntry *vm, uint32_t physical,
		     uint32_t virtual, int access_permissions)
{
	struct PageTableEntry *page_table = NULL;

	uint32_t section_index = SECTION_INDEX(virtual);
	uint32_t page_index = PAGE_INDEX(virtual);

	/* if this section is not mapped before, map it to a new page table */
	if (vm[section_index].base_address == 0) {
		page_table = allocate_page_table();
		memset(page_table, 0, PAGE_TABLE_SIZE);
		
		vm[section_index].base_address = PAGE_TABLE_TO_BASE(V2P(page_table));
		vm[section_index].desc_type = SECTION_DESC;
		vm[section_index].domain = 0;
	}
	/* otherwise use the previously allocated page table */
	else {
		page_table = (void *) P2V(BASE_TO_PAGE_TABLE(vm[section_index].base_address));
	}

	/* map the virtual page to physical page in page table */
	page_table[page_index].desc_type = PAGE_DESC,
	page_table[page_index].bufferable = 0;
	page_table[page_index].cacheable = 0;
	page_table[page_index].access_permissions = access_permissions;
	page_table[page_index].base_address = PAGE_TO_BASE(physical);
}

/*
 * allocate_page_table. this function assumes page size is an integer multiple
 * of page table size.
 */
static struct PageTableEntry *allocate_page_table()
{
	void *result = NULL;

	/*
	 * If this is the first call or the previous page is full, allocate
	 * a new page.
	 */
	if (page_table_allocated_page == NULL || page_table_page_offset == PAGE_SIZE) {
		page_table_allocated_page = kalloc();
		page_table_page_offset = 0;
	}

	result = page_table_allocated_page + page_table_page_offset;
	page_table_page_offset += PAGE_TABLE_SIZE;

	return result;
}

/*
 * resolve_physical_address simulates the virtual memory hardware and maps the
 * given virtual address to physical address. This function can be used for
 * debugging if given virtual memory is constructed correctly.
 */
uint32_t resolve_physical_address(struct SectionTableEntry *vm, uint32_t virtual)
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
