#include <block.h>
#include <lib/string.h>
#include <vm.h>

#define PCI_CONFIG 0xd0000000
#define PCI_IO 0xd0010000
#define REG(type, offset) (*(volatile type *)(PCI_IO + (offset)))

struct Descriptor {
	uint32_t addr, addr_high, len;
	uint16_t flags, next;
};

/* Legacy queues require page alignment and a page-aligned used ring. */
static uint8_t queue[12288] __attribute__((aligned(4096)));
static struct {
	uint32_t type, reserved, sector, sector_high;
	uint8_t data[512];
	volatile uint8_t status;
} request;
static volatile uint16_t *avail, *used;
static uint16_t queue_size, completed;
static uint32_t sectors;

static void barrier(void)
{
	uint32_t zero = 0;
	/* ARM926: drain the write buffer; all DMA mappings are uncached. */
	__asm__ volatile("mcr p15, 0, %0, c7, c10, 4" :: "r"(zero)
			 : "memory");
}

bool block_init(void)
{
	volatile uint32_t *config = NULL;
	uint32_t slot, features;

	sectors = 0;
	for (slot = 0; slot < 32; slot++) {
		config = (void *)(PCI_CONFIG + (slot << 11));
		if (config[0] == 0x10011af4)
			break;
	}
	if (slot == 32)
		return false;

	/* Assign BAR0 to PCI I/O port 0x1000; enable I/O and bus mastering. */
	config[4] = 0x1001;
	*(volatile uint16_t *)(config + 1) = 5;
	REG(uint8_t, 18) = 0;
	REG(uint8_t, 18) = 1;
	REG(uint8_t, 18) = 3;
	features = REG(uint32_t, 0);
	/* Accept only the read-only feature; no optional queue features. */
	REG(uint32_t, 4) = features & (1 << 5);
	REG(uint16_t, 14) = 0;
	queue_size = REG(uint16_t, 12);
	if (queue_size < 3 || queue_size > 256 ||
	    (queue_size & (queue_size - 1)))
		goto fail;
	memset(queue, 0, sizeof(queue));
	avail = (void *)(queue + 16 * queue_size);
	used = (void *)(queue + ROUND_UP(18 * queue_size + 6, 4096));
	avail[0] = 1; /* Poll completions instead of requesting interrupts. */
	completed = 0;
	barrier();
	REG(uint32_t, 8) = V2P(queue) >> 12;
	/* This small driver supports disks with a 32-bit sector count. */
	if (REG(uint32_t, 24) != 0)
		goto fail;
	sectors = REG(uint32_t, 20);
	if (!sectors)
		goto fail;
	REG(uint8_t, 18) = 7;
	return true;
fail:
	REG(uint8_t, 18) = 0x80;
	sectors = 0;
	return false;
}

uint32_t block_sector_count(void)
{
	return sectors;
}

bool block_read(uint32_t sector, void *buffer)
{
	struct Descriptor *desc = (void *)queue;
	uint32_t timeout;

	if (sector >= sectors)
		return false;
	request.type = 0; /* VIRTIO_BLK_T_IN */
	request.sector = sector;
	request.status = 0xff;
	desc[0] = (struct Descriptor){V2P(&request), 0, 16, 1, 1};
	desc[1] = (struct Descriptor){V2P(request.data), 0, 512, 3, 2};
	desc[2] = (struct Descriptor){V2P(&request.status), 0, 1, 2, 0};
	avail[2 + (completed & (queue_size - 1))] = 0;
	barrier();
	avail[1] = completed + 1;
	barrier();
	REG(uint16_t, 16) = 0;
	for (timeout = 0; timeout < 10000000; timeout++)
		if (used[1] != completed)
			break;
	if (timeout == 10000000) {
		/* Never reuse buffers while a timed-out DMA may still be live. */
		REG(uint8_t, 18) = 0;
		sectors = 0;
		return false;
	}
	barrier();
	completed++;
	(void)REG(uint8_t, 19);
	if (request.status != 0)
		return false;
	memcpy(buffer, request.data, 512);
	return true;
}
