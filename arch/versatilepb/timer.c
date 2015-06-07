#include <timer.h>
#include <types.h>
#include <vm.h>
#include <mmc.h>
#include <klib.h>

/* memory mapping for timer */
#define TIMER MMIO_P2V(0x101e3000)

#define TIMER_LOAD    0x00
#define TIMER_VALUE   0x01
#define TIMER_CONTROL 0x02
#define TIMER_INTCTL  0x03
#define TIMER_BGLOAD  0x06

struct mmc_card_t mmc_card;
struct mmc_card_info_t mmc_card_info;

uint8_t buffer[40960];

#define UNSTUFF_BITS(resp, start, size)								\
	({																\
		const int32_t __size = size;									\
		const uint32_t __mask = (__size < 32 ? 1 << __size : 0) - 1;	\
		const int32_t __off = 3 - ((start) / 32);						\
		const int32_t __shft = (start) & 31;							\
		uint32_t __res;												\
																	\
		__res = resp[__off] >> __shft;								\
		if (__size + __shft > 32)									\
			__res |= resp[__off-1] << ((32 - __shft) % 32);			\
		__res & __mask;												\
	})

void timer_set_interval(uint32_t interval_microsecond)
{
	bool ret;
	uint32_t i;

	TIMER[TIMER_CONTROL] = 0;
	TIMER[TIMER_BGLOAD] = 0;
	TIMER[TIMER_LOAD] = interval_microsecond;
	TIMER[TIMER_CONTROL] = 0xe2;

	mmc_card.info = &mmc_card_info;
	realview_mmc_init();
	ret = realview_mmc_probe(&mmc_card_info);
	kprintf("prob_result: %d\n", ret);

	ret = mmc_card_decode(&mmc_card);
	kprintf("decode result: %d\n", ret);

	kprintf("sector size: %d, sector count: %d, capacity: %d\n",
		mmc_card_info.sector_size, mmc_card_info.sector_count,
		mmc_card_info.capacity);

	ret = realview_mmc_read_one_sector(&mmc_card, buffer, 0);
	kprintf("read result: %d\n", ret);

	for (i = 0; i < 32; i++)
		kprintf("%x ", buffer[i]);
	kprintf("\n\n");
}

void timer_clear_interrupt(void)
{
	TIMER[TIMER_INTCTL] = 0;
}
