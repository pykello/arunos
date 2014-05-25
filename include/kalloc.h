/*
 * kalloc.h
 *
 * Type and function declarations for kernel allocator.
 *
 * (c) 2014 Hadi Moshayedi <hadi@moshayedi.net>
 */

#ifndef KALLOC_H
#define KALLOC_H

#include <types.h>

struct PageList {
	struct PageList *next;
};

/* exported function declarations */
void kalloc_init(uint32_t start, uint32_t end);
void *kalloc();
void kfree(void *page);

#endif