/*
 * vm.h
 *
 * Type and function declarations for virtual memory management.
 *
 * (c) 2014 Hadi Moshayedi <hadi@moshayedi.net>
 */

#ifndef MEMORY_H
#define MEMORY_H

#define KERNEL_BASE 0x80000000

#ifndef __ASSEMBLER__

#include <hardware.h>
#include <types.h>

/* conversion macros */
#define SECTION_INDEX(x) ((uint32_t)x >> 20)
#define PAGE_INDEX(x) (((uint32_t)x >> 12) & 255)
#define PAGE_TABLE_TO_BASE(x) ((uint32_t)x >> 10)
#define BASE_TO_PAGE_TABLE(x) ((void *) ((uint32_t)x << 10))
#define PAGE_TO_BASE(x) ((uint32_t)x >> 12)

#define P2V(x) (((uint32_t) x) + KERNEL_BASE)
#define V2P(x) (((uint32_t) x) - KERNEL_BASE)

#define ROUND_DOWN(x,alignment) ((x) & ~(alignment - 1))
#define ROUND_UP(x,alignment) (((x) + alignment - 1) & ~(alignment - 1))

/* linker symbols */
extern char kernel_end[];

/* memory layout */
#define KERNEL_STACK_BOTTOM (KERNEL_BASE - 2 * PAGE_SIZE)
#define USER_STACK_BOTTOM (KERNEL_BASE - 3 * PAGE_SIZE)
#define KERNEL_SECTION_TABLE ((uint32_t) kernel_end)
#define MMIO_BASE (KERNEL_BASE + 0x40000000)
#define ALLOCATABLE_MEMORY_START (KERNEL_SECTION_TABLE +  0x200000)
#define INTERRUPT_VECTOR_BASE 0xffff0000

#define MMIO_P2V(x) ((volatile unsigned int*) (MMIO_BASE + (x - MMIO_BASE_PHYSICAL)))

/* page table */
#define PAGES_PER_SECTION 1024
#define SECTION_COUNT 4096
#define PAGE_SIZE 4096
#define SECTION_SIZE (PAGE_SIZE * PAGES_PER_SECTION)
#define INITIAL_MEMORY_SIZE 0x2000000   /* 32MB  */

#define PAGE_TABLE_ALIGNMENT 1024
#define SECTION_TABLE_ALIGNMENT 16384
#define PAGE_TABLE_SIZE 1024
#define SECTION_TABLE_SIZE 16384

/* descriptor types */
#define PAGE_DESC 2
#define SECTION_DESC 1

/* access permissions */
#define AP_RW_D 0x55
#define AP_RW_R 0xaa
#define AP_RW_RW 0xff

/* a 32-bit entry in hardware's section table */
struct SectionTableEntry {
	unsigned int desc_type : 2;
	unsigned int : 3;
	unsigned int domain : 4;
	unsigned int : 1;
	unsigned int base_address : 22;
};

/* a 32-bit entry in hardware's page table */
struct PageTableEntry {
	unsigned int desc_type : 2;
	unsigned int bufferable : 1;
	unsigned int cacheable : 1;
	unsigned int access_permissions : 8;
	unsigned int base_address : 20;
};

/* to hold information about a mapping */
struct MemoryMapping {
	uint32_t virtual_address;
	uint32_t physical_start;
	uint32_t physical_end;
	int access_permissions;

};

extern struct SectionTableEntry *kernel_vm;

/* exported function declarations */
void vm_init(void);
void setup_kernel_vm(struct SectionTableEntry *kernel_vm);
void map_pages(struct SectionTableEntry *vm, struct MemoryMapping mapping);
void free_vm_page_tables(struct SectionTableEntry *vm);
void unmap_page(struct SectionTableEntry *vm, uint32_t virtual_addr);
uint32_t resolve_physical_address(struct SectionTableEntry *vm,
				  uint32_t virtual_address);

#endif
#endif
