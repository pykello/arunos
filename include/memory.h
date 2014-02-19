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

#define PAGE_DESC_TYPE_SMALL 2
#define SECTION_DESC_TYPE_COARSE 1
#define ACCESS_PERMISSION_KERNEL_ONLY 1

struct L1CoarseVirtPageDesc {
	unsigned int desc_type : 2;
	unsigned int : 3;
	unsigned int domain : 4;
	unsigned int : 1;
	unsigned int level_2_base_address : 20;
};

struct L1SectionVirtPageDesc {
	unsigned int desc_type : 2;
	unsigned int bufferable : 1;
	unsigned int cacheable : 1;
	unsigned int : 1;
	unsigned int domain : 4;
	unsigned int : 1;
	unsigned int access_permissions : 2;
	unsigned int : 8;
	unsigned int physical_base_address : 12;
};

struct L2SmallVirtPageDesc {
	unsigned int desc_type : 2;
	unsigned int bufferable : 1;
	unsigned int cacheable : 1;
	unsigned int access_permissions_0 : 2;
	unsigned int access_permissions_1 : 2;
	unsigned int access_permissions_2 : 2;
	unsigned int access_permissions_3 : 2;
	unsigned int physical_base_address : 20;
};

#define PHYSICAL_PAGE_COUNT 32768
#define PHYSICAL_SECTION_COUNT 128
#define PAGE_SIZE 4096

void memory_init(void);

#endif
