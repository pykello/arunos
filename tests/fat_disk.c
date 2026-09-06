#include <block.h>

extern long pread(int fd, void *buffer, unsigned long count, long offset);
static int disk_fd;
static uint32_t disk_sectors;

void test_disk(int fd, uint32_t sectors)
{
	disk_fd = fd;
	disk_sectors = sectors;
}

uint32_t block_sector_count(void)
{
	return disk_sectors;
}

bool block_read(uint32_t sector, void *buffer)
{
	return sector < disk_sectors &&
		pread(disk_fd, buffer, 512, (long)sector * 512) == 512;
}
