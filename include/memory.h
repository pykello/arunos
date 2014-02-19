/*
 * memory.h
 *
 * Type and function declarations for memory management.
 *
 * (c) 2014 Hadi Moshayedi <hadi@moshayedi.net>
 */

#ifndef MEMORY_H
#define MEMORY_H

#include <types.h>

#define KERNEL_BASE 0x80000000
#define KERNEL_P2V(x) ((x) + KERNEL_BASE)
#define KERNEL_V2P(x) ((x) - KERNEL_BASE)

#define PAGE_DESC_TYPE_SMALL 2
#define SECTION_DESC_TYPE_COARSE 1
#define ACCESS_PERMISSION_RW_R 2

#define PAGE_SIZE 4096
#define PAGE_COUNT 1048576
#define SECTION_COUNT 4096

struct SectionTableEntry {
	unsigned int desc_type : 2;
	unsigned int : 3;
	unsigned int domain : 4;
	unsigned int : 1;
	unsigned int base_address : 20;
};

struct PageTableEntry {
	unsigned int desc_type : 2;
	unsigned int bufferable : 1;
	unsigned int cacheable : 1;
	unsigned int access_permissions_0 : 2;
	unsigned int access_permissions_1 : 2;
	unsigned int access_permissions_2 : 2;
	unsigned int access_permissions_3 : 2;
	unsigned int base_address : 20;
};

void memory_init(void);

#endif
