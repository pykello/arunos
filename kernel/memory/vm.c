#include <vm.h>
#include <hardware.h>
#include <lib/string.h>
#include <kalloc.h>
#include <klib.h>
#include <system.h>
#include <types.h>

/* forward declarations for local functions */
static void map_page(struct SectionTableEntry *vm, uint32_t physical,
		     uint32_t virtual, int access_permissions);

/* kernel virtual to physical memory mappings */
static struct MemoryMapping kernel_mappings[] = {
	{KERNEL_BASE, 0, V2P(kernel_end), AP_RW_D},
	{MMIO_BASE, MMIO_BASE_PHYSICAL, MMIO_BASE_PHYSICAL + 0x400000, AP_RW_D},
	{INTERRUPT_VECTOR_BASE, 0, PAGE_SIZE, AP_RW_D},
	{ALLOCATABLE_MEMORY_START, V2P(ALLOCATABLE_MEMORY_START),
		TOTAL_MEMORY_SIZE, AP_RW_D}
};

const int kernel_mapping_count = 4;

struct SectionTableEntry *kernel_vm = NULL;

/*
 * vm_init sets up a two level page table. This function only sets
 * up the kernel part of the address space. The user part of the address
 * space will be setup later.
 */
void vm_init(void)
{
	kernel_vm = (void *) ROUND_UP(KERNEL_SECTION_TABLE,
				      SECTION_TABLE_ALIGNMENT);
	setup_kernel_vm(kernel_vm);

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
void setup_kernel_vm(struct SectionTableEntry *kernel_vm)
{
	int i = 0;
	memset(kernel_vm, 0, SECTION_TABLE_SIZE);

	/* add each of the mappings */
	for (i = 0; i < kernel_mapping_count; i++)
		map_pages(kernel_vm, kernel_mappings[i]);
}

/*
 * map_pages adds the given virtual to physical memory mapping to the given
 * virtual memory. A mapping can map multiple pages.
 */
void map_pages(struct SectionTableEntry *vm, struct MemoryMapping mapping)
{
	uint32_t physical_current = 0;
	uint32_t virtual_current = 0;

	uint32_t virtual_start = ROUND_DOWN(mapping.virtual_address, PAGE_SIZE);
	uint32_t physical_start = ROUND_DOWN(mapping.physical_start, PAGE_SIZE);
	uint32_t physical_end = ROUND_UP(mapping.physical_end, PAGE_SIZE);

	/* iterate over pages and map each page */
	virtual_current = virtual_start;
	for (physical_current = physical_start; physical_current != physical_end;
	     physical_current += PAGE_SIZE)
	{
		map_page(vm, physical_current, virtual_current,
			 mapping.access_permissions);
		virtual_current += PAGE_SIZE;
	}
}

void free_vm_page_tables(struct SectionTableEntry *vm)
{
	int i = 0;
	for (i = 0; i < 4096; i++) {
		if (vm[i].desc_type != 0) {
			void *page_table = (void *) P2V(BASE_TO_PAGE_TABLE(vm[i].base_address));
			kfree1k(page_table);
		}
	}
}

/*
 * map_page adds to the given virtual memory the mapping of a single virtual page
 * to a physical page.
 */
static void map_page(struct SectionTableEntry *vm, uint32_t physical,
		     uint32_t virtual_addr, int access_permissions)
{
	struct PageTableEntry *page_table = NULL;

	uint32_t section_index = SECTION_INDEX(virtual_addr);
	uint32_t page_index = PAGE_INDEX(virtual_addr);

	/* if this section is not mapped before, map it to a new page table */
	if (vm[section_index].desc_type == 0) {
		page_table = kalloc1k();
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

/* unmap_page clears the mapping for the given virtual address */
void unmap_page(struct SectionTableEntry *vm, uint32_t virtual_addr)
{
	struct PageTableEntry *page_table = NULL;

	uint32_t section_index = SECTION_INDEX(virtual_addr);
	uint32_t page_index = PAGE_INDEX(virtual_addr);

	page_table = (void *) P2V(BASE_TO_PAGE_TABLE(vm[section_index].base_address));
	page_table[page_index].desc_type = 0;
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

	section = (void *) ((uint32_t) vm | ((virtual >> 20) << 2));
	base_address = (void *) (section->base_address << 10);
	page = (void *) ((uint32_t) base_address | ((virtual >> 10) & 0x3fc));
	page = (void *) P2V(page);
	result = (page->base_address << 12) | (virtual & 0xfff);

	return result;
}
