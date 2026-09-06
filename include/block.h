#ifndef BLOCK_H
#define BLOCK_H

#include <types.h>

/* One read-only disk, 512-byte sectors, synchronous kernel callers. */
bool block_init(void);
uint32_t block_sector_count(void);
bool block_read(uint32_t sector, void *buffer);

#endif
