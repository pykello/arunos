#ifndef FAT_H
#define FAT_H

#include <types.h>

struct FatFile {
	uint32_t size;
	uint16_t cluster;
};

/* FAT16 superfloppy, root directory only; names are padded 11-byte 8.3. */
bool fat_init(void);
bool fat_open(const char name[11], struct FatFile *file);
bool fat_read(const struct FatFile *file, uint32_t offset, void *buffer,
	      uint32_t length);

#endif
