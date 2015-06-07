#ifndef MMC_H
#define MMC_H

#include <types.h>

/*
 * card type
 */
enum mmc_card_type_t {
	MMC_CARD_TYPE_NONE,
	MMC_CARD_TYPE_MMC,
	MMC_CARD_TYPE_SD,
	MMC_CARD_TYPE_SD20,
	MMC_CARD_TYPE_SDHC,
};

/*
 * mmc's cid
 */
struct mmc_cid_t {
	uint8_t	mid;			/* card manufacturer id */
	uint16_t	oid;			/* oem / application id */
	uint8_t	pnm[8];			/* product name */
	uint8_t	hwrev;			/* product hardware revision */
	uint8_t	fwrev;			/* product firmware revision */
	uint32_t	serial;			/* serial number */
	uint16_t	year;			/* year of manufacture date */
	uint16_t	month;			/* month of manufacture date */
};

/*
 * mmc's csd
 */
struct mmc_csd_t {
	uint8_t	mmca_vsn;
	uint16_t	cmdclass;
	uint16_t	tacc_clks;
	uint32_t	tacc_ns;
	uint32_t	r2w_factor;
	uint32_t	max_dtr;
	uint32_t	read_blkbits;
	uint32_t	write_blkbits;
	uint32_t	capacity;
	uint32_t	read_partial	:1,
			read_misalign	:1,
			write_partial	:1,
			write_misalign	:1;
};

/*
 * mmc card information
 */
struct mmc_card_info_t
{
	/*
	 * raw data, filled by mmc host controller.
	 */
	enum mmc_card_type_t	type;
	uint16_t					rca;
	uint32_t					raw_cid[4];
	uint32_t					raw_csd[4];

	/*
	 * auto decoded when probed
	 */
	struct mmc_cid_t 		cid;
	struct mmc_csd_t 		csd;

	size_t					sector_size;
	size_t					sector_count;
	size_t					capacity;
};


struct mmc_card_t
{
	/* the name of mmc card */
	char name[32 + 1];

	/* mmc card information */
	struct mmc_card_info_t * info;

	/* the driver of mmc host controller */
	//struct mmc_host * host;

	/* priv data */
	void * priv;
};


void realview_mmc_init(void);
void realview_mmc_exit(void);
bool realview_mmc_probe(struct mmc_card_info_t * info);
bool realview_mmc_read_one_sector(struct mmc_card_t * card, uint8_t * buf, uint32_t sector);
bool realview_mmc_write_one_sector(struct mmc_card_t * card, const uint8_t * buf, uint32_t sector);
bool realview_mmc_read_sectors(struct mmc_card_t * card, uint8_t * buf, uint32_t sector, uint32_t count);
bool realview_mmc_write_sectors(struct mmc_card_t * card, const uint8_t * buf, uint32_t sector, uint32_t count);


#endif