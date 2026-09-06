#ifndef FAT_H
#define FAT_H

#include <types.h>

struct FatFile {
	uint32_t size;
	uint16_t cluster;
};

/* FAT16 superfloppy, root directory only; case-insensitive ASCII 8.3 filenames. */
bool fat_init(void);
/* Start at cursor 0; returns next cursor, 0 at EOF, or -1 on error. */
int fat_next(uint32_t cursor, char name[13]);
bool fat_open(const char *name, struct FatFile *file);
bool fat_read(const struct FatFile *file, uint32_t offset, void *buffer,
	      uint32_t length);

#endif
