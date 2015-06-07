#include <mmc.h>
#include <types.h>
#include <vm.h>
#include <klib.h>


/* Ported from xboot. */

typedef uint32_t u32_t;
typedef uint8_t u8_t;
typedef bool bool_t;
typedef int32_t s32_t;

#define TRUE true
#define FALSE false

/*
 * prime cell pl181 mci register
 */
#define REALVIEW_MCI_POWER				(0x10005000 + 0x000)
#define REALVIEW_MCI_CLOCK				(0x10005000 + 0x004)
#define REALVIEW_MCI_ARGUMENT			(0x10005000 + 0x008)
#define REALVIEW_MCI_COMMAND			(0x10005000 + 0x00c)

#define REALVIEW_MCI_RESPCMD			(0x10005000 + 0x010)
#define REALVIEW_MCI_RESP0				(0x10005000 + 0x014)
#define REALVIEW_MCI_RESP1				(0x10005000 + 0x018)
#define REALVIEW_MCI_RESP2				(0x10005000 + 0x01c)
#define REALVIEW_MCI_RESP3				(0x10005000 + 0x020)

#define REALVIEW_MCI_DATA_TIMER			(0x10005000 + 0x024)
#define REALVIEW_MCI_DATA_LENGTH		(0x10005000 + 0x028)
#define REALVIEW_MCI_DATA_CTRL			(0x10005000 + 0x02c)
#define REALVIEW_MCI_DATA_CNT			(0x10005000 + 0x030)

#define REALVIEW_MCI_STATUS				(0x10005000 + 0x034)
#define REALVIEW_MCI_CLEAR				(0x10005000 + 0x038)
#define REALVIEW_MCI_MASK0				(0x10005000 + 0x03c)
#define REALVIEW_MCI_MASK1				(0x10005000 + 0x040)
#define REALVIEW_MCI_SELECT				(0x10005000 + 0x044)
#define REALVIEW_MCI_FIFO_CNT			(0x10005000 + 0x048)
#define REALVIEW_MCI_FIFO				(0x10005000 + 0x080)

/*
 * bit masks for command
 */
#define REALVIEW_MCI_CMD_RESPONSE		(1 << 6)
#define REALVIEW_MCI_CMD_LONGRSP		(1 << 7)
#define REALVIEW_MCI_CMD_INTERRUPT		(1 << 8)
#define REALVIEW_MCI_CMD_PENDING		(1 << 9)
#define REALVIEW_MCI_CMD_ENABLE			(1 << 10)

/*
 * bit masks for status
 */
#define	REALVIEW_MCI_STAT_CMD_CRC_FAIL	(1 << 0)
#define	REALVIEW_MCI_STAT_DAT_CRC_FAIL  (1 << 1)
#define	REALVIEW_MCI_STAT_CMD_TIME_OUT  (1 << 2)
#define	REALVIEW_MCI_STAT_DAT_TIME_OUT  (1 << 3)
#define	REALVIEW_MCI_STAT_TX_UNDERRUN	(1 << 4)
#define	REALVIEW_MCI_STAT_RX_OVERRUN	(1 << 5)
#define	REALVIEW_MCI_STAT_CMD_RESP_END	(1 << 6)
#define	REALVIEW_MCI_STAT_CMD_SENT		(1 << 7)
#define	REALVIEW_MCI_STAT_DAT_END		(1 << 8)
#define	REALVIEW_MCI_STAT_DAT_BLK_END	(1 << 10)
#define	REALVIEW_MCI_STAT_CMD_ACT		(1 << 11)
#define	REALVIEW_MCI_STAT_TX_ACT		(1 << 12)
#define	REALVIEW_MCI_STAT_RX_ACT		(1 << 13)
#define	REALVIEW_MCI_STAT_TX_FIFO_HALF	(1 << 14)
#define	REALVIEW_MCI_STAT_RX_FIFO_HALF	(1 << 15)
#define	REALVIEW_MCI_STAT_TX_FIFO_FULL	(1 << 16)
#define	REALVIEW_MCI_STAT_RX_FIFO_FULL	(1 << 17)
#define	REALVIEW_MCI_STAT_TX_FIFO_ZERO	(1 << 18)
#define	REALVIEW_MCI_STAT_RX_DAT_ZERO	(1 << 19)
#define	REALVIEW_MCI_STAT_TX_DAT_AVL	(1 << 20)
#define	REALVIEW_MCI_STAT_RX_FIFO_AVL	(1 << 21)

/*
 * bit masks for clear status
 */
#define REALVIEW_MCI_CLR_CMD_CRC_FAIL	(1 << 0)
#define REALVIEW_MCI_CLR_DAT_CRC_FAIL	(1 << 1)
#define REALVIEW_MCI_CLR_CMD_TIMEOUT	(1 << 2)
#define REALVIEW_MCI_CLR_DAT_TIMEOUT	(1 << 3)
#define REALVIEW_MCI_CLR_TX_UNDERRUN	(1 << 4)
#define REALVIEW_MCI_CLR_RX_OVERRUN		(1 << 5)
#define REALVIEW_MCI_CLR_CMD_RESP_END	(1 << 6)
#define REALVIEW_MCI_CLR_CMD_SENT		(1 << 7)
#define REALVIEW_MCI_CLR_DAT_END		(1 << 8)
#define REALVIEW_MCI_CLR_DAT_BLK_END	(1 << 10)


/*
 * standard mmc commands
 */

/* class 1 */
#define MMC_GO_IDLE_STATE			0	/* bc                          */
#define MMC_SEND_OP_COND			1	/* bcr  [31:0] OCR         R3  */
#define MMC_ALL_SEND_CID			2	/* bcr                     R2  */
#define MMC_SET_RELATIVE_ADDR		3	/* ac   [31:16] RCA        R1  */
#define MMC_SET_DSR					4	/* bc   [31:16] RCA            */
#define MMC_SWITCH					6	/* ac   [31:0] See below   R1b */
#define MMC_SELECT_CARD				7	/* ac   [31:16] RCA        R1  */
#define MMC_SEND_EXT_CSD			8	/* adtc                    R1  */
#define MMC_SEND_CSD				9	/* ac   [31:16] RCA        R2  */
#define MMC_SEND_CID				10	/* ac   [31:16] RCA        R2  */
#define MMC_READ_DAT_UNTIL_STOP		11	/* adtc [31:0] dadr        R1  */
#define MMC_STOP_TRANSMISSION		12	/* ac                      R1b */
#define MMC_SEND_STATUS				13	/* ac   [31:16] RCA        R1  */
#define MMC_GO_INACTIVE_STATE		15	/* ac   [31:16] RCA            */
#define MMC_SPI_READ_OCR			58	/* spi                  spi_R3 */
#define MMC_SPI_CRC_ON_OFF			59	/* spi  [0:0] flag      spi_R1 */

/* class 2 */
#define MMC_SET_BLOCKLEN			16	/* ac   [31:0] block len   R1  */
#define MMC_READ_SINGLE_BLOCK		17	/* adtc [31:0] data addr   R1  */
#define MMC_READ_MULTIPLE_BLOCK		18	/* adtc [31:0] data addr   R1  */

/* class 3 */
#define MMC_WRITE_DAT_UNTIL_STOP 	20	/* adtc [31:0] data addr   R1  */

/* class 4 */
#define MMC_SET_BLOCK_COUNT			23	/* adtc [31:0] data addr   R1  */
#define MMC_WRITE_SINGLE_BLOCK		24	/* adtc [31:0] data addr   R1  */
#define MMC_WRITE_MULTIPLE_BLOCK	25	/* adtc                    R1  */
#define MMC_PROGRAM_CID				26	/* adtc                    R1  */
#define MMC_PROGRAM_CSD				27	/* adtc                    R1  */

/* class 5 */
#define MMC_ERASE_GROUP_START		35	/* ac   [31:0] data addr   R1  */
#define MMC_ERASE_GROUP_END			36	/* ac   [31:0] data addr   R1  */
#define MMC_ERASE					38	/* ac                      R1b */

/* class 6 */
#define MMC_SET_WRITE_PROT			28	/* ac   [31:0] data addr   R1b */
#define MMC_CLR_WRITE_PROT			29	/* ac   [31:0] data addr   R1b */
#define MMC_SEND_WRITE_PROT			30	/* adtc [31:0] wpdata addr R1  */

/* class 7 */
#define MMC_LOCK_UNLOCK				42	/* adtc                    R1b */

/* class 8 */
#define MMC_APP_CMD					55	/* ac   [31:16] RCA        R1  */
#define MMC_GEN_CMD					56	/* adtc [0] RD/WR          R1  */

/* class 9 */
#define MMC_FAST_IO					39	/* ac   <Complex>          R4  */
#define MMC_GO_IRQ_STATE			40	/* bcr                     R5  */


/*
 * standard sd commands
 */
#define SD_SEND_RELATIVE_ADDR		3
#define SD_SWITCH_FUNC				6
#define SD_SEND_IF_COND				8

#define SD_APP_SET_BUS_WIDTH		6
#define SD_APP_SEND_OP_COND			41
#define SD_APP_SEND_SCR				51

/*
 * define mmc's voltage
 */
#define MMC_VDD_165_195				0x00000080	/* vdd voltage 1.65 - 1.95 */
#define MMC_VDD_20_21				0x00000100	/* vdd voltage 2.0 ~ 2.1 */
#define MMC_VDD_21_22				0x00000200	/* vdd voltage 2.1 ~ 2.2 */
#define MMC_VDD_22_23				0x00000400	/* vdd voltage 2.2 ~ 2.3 */
#define MMC_VDD_23_24				0x00000800	/* vdd voltage 2.3 ~ 2.4 */
#define MMC_VDD_24_25				0x00001000	/* vdd voltage 2.4 ~ 2.5 */
#define MMC_VDD_25_26				0x00002000	/* vdd voltage 2.5 ~ 2.6 */
#define MMC_VDD_26_27				0x00004000	/* vdd voltage 2.6 ~ 2.7 */
#define MMC_VDD_27_28				0x00008000	/* vdd voltage 2.7 ~ 2.8 */
#define MMC_VDD_28_29				0x00010000	/* vdd voltage 2.8 ~ 2.9 */
#define MMC_VDD_29_30				0x00020000	/* vdd voltage 2.9 ~ 3.0 */
#define MMC_VDD_30_31				0x00040000	/* vdd voltage 3.0 ~ 3.1 */
#define MMC_VDD_31_32				0x00080000	/* vdd voltage 3.1 ~ 3.2 */
#define MMC_VDD_32_33				0x00100000	/* vdd voltage 3.2 ~ 3.3 */
#define MMC_VDD_33_34				0x00200000	/* vdd voltage 3.3 ~ 3.4 */
#define MMC_VDD_34_35				0x00400000	/* vdd voltage 3.4 ~ 3.5 */
#define MMC_VDD_35_36				0x00800000	/* vdd voltage 3.5 ~ 3.6 */



static void write32(uint32_t address, uint32_t value) {
	*MMIO_P2V(address) = value;
}

static void write8(uint32_t address, uint8_t value) {
	*((uint8_t *)MMIO_P2V(address)) = value;
}

static uint32_t read32(uint32_t address) {
	return *MMIO_P2V(address);
}


/*
 * send command flags
 */
#define REALVIEW_MCI_RSP_NONE			(0 << 0)
#define REALVIEW_MCI_RSP_PRESENT		(1 << 0)
#define REALVIEW_MCI_RSP_136BIT			(1 << 1)
#define REALVIEW_MCI_RSP_CRC			(1 << 2)

static bool_t mmc_send_cmd(u32_t cmd, u32_t arg, u32_t * resp, u32_t flags)
{
	u32_t status;
	bool_t ret = TRUE;

	if(read32(REALVIEW_MCI_COMMAND) & REALVIEW_MCI_CMD_ENABLE)
	{
		write32(REALVIEW_MCI_COMMAND, 0x0);
	}

	cmd = (cmd & 0x3f) | REALVIEW_MCI_CMD_ENABLE;

	if(flags & REALVIEW_MCI_RSP_PRESENT)
	{
		if(flags & REALVIEW_MCI_RSP_136BIT)
			cmd |= REALVIEW_MCI_CMD_LONGRSP;
		cmd |= REALVIEW_MCI_CMD_RESPONSE;
	}

	write32(REALVIEW_MCI_ARGUMENT, arg);
	write32(REALVIEW_MCI_COMMAND, cmd);

	/*
	 * wait for a while
	 */
	do {
		status = read32(REALVIEW_MCI_STATUS);
	} while(!(status & (REALVIEW_MCI_STAT_CMD_SENT | REALVIEW_MCI_STAT_CMD_RESP_END |
			REALVIEW_MCI_STAT_CMD_TIME_OUT | REALVIEW_MCI_STAT_CMD_CRC_FAIL)));

	if(flags & REALVIEW_MCI_RSP_PRESENT)
	{
		resp[0] = read32(REALVIEW_MCI_RESP0);

		if(flags & REALVIEW_MCI_RSP_136BIT)
		{
			resp[1] = read32(REALVIEW_MCI_RESP1);
			resp[2] = read32(REALVIEW_MCI_RESP2);
			resp[3] = read32(REALVIEW_MCI_RESP3);
		}
	}

	if(status & REALVIEW_MCI_STAT_CMD_TIME_OUT)
		ret = FALSE;
	else if ((status & REALVIEW_MCI_STAT_CMD_CRC_FAIL) && (flags & REALVIEW_MCI_RSP_CRC))
		ret = FALSE;

	write32(REALVIEW_MCI_CLEAR, (REALVIEW_MCI_CLR_CMD_SENT | REALVIEW_MCI_CLR_CMD_RESP_END |
			REALVIEW_MCI_CLR_CMD_TIMEOUT | REALVIEW_MCI_CLR_CMD_CRC_FAIL));

	return ret;
}

static bool_t mmc_send_acmd(u32_t cmd, u32_t arg, u32_t * resp, u32_t flags)
{
	u32_t aresp;
	bool_t ret = TRUE;

	ret = mmc_send_cmd(MMC_APP_CMD, 0, &aresp, REALVIEW_MCI_RSP_PRESENT);
	if(!ret)
		return ret;

	if((aresp & ((1 << 22) | (1 << 5))) != (1 << 5))
		return FALSE;

	return mmc_send_cmd(cmd, arg, resp, flags);
}

static bool_t mmc_idle_cards(void)
{
	bool_t ret = TRUE;
	int i;

	/*
	 * reset all cards
	 */
	ret = mmc_send_cmd(MMC_GO_IDLE_STATE, 0, NULL, REALVIEW_MCI_RSP_NONE);
	if(ret)
		return ret;

	/*
	 * wait a moment
	 */
	//udelay(1000);
	for (i = 0; i < 100000; i++);

	return mmc_send_cmd(MMC_GO_IDLE_STATE, 0, NULL, 0);
}

static bool_t mmc_send_if_cond(struct mmc_card_info_t * info)
{
	u32_t resp[4];

	if(!mmc_idle_cards())
		return FALSE;

	if(mmc_send_cmd(SD_SEND_IF_COND, MMC_VDD_33_34, &resp[0], REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_CRC))
	{
		if((resp[0] & 0xff) == 0xaa)
		{
			info->type = MMC_CARD_TYPE_SD20;
			return TRUE;
		}
	}

	info->type = MMC_CARD_TYPE_NONE;
	return FALSE;
}

static bool_t sd_send_op_cond(struct mmc_card_info_t * info)
{
	u32_t resp[4];
	u32_t arg;
	u32_t i;

	if(!mmc_idle_cards())
		return FALSE;

	for(i=0; i<100; i++)
	{
		arg = MMC_VDD_33_34;
		if(info->type == MMC_CARD_TYPE_SD20)
			arg |= 0x40000000;

		if(mmc_send_acmd(SD_APP_SEND_OP_COND, arg, &resp[0], REALVIEW_MCI_RSP_PRESENT))
		{
			if(resp[0] & 0x80000000)
			{
				if((resp[0] & 0x40000000) == 0x40000000)
					info->type = MMC_CARD_TYPE_SDHC;
				else if(info->type != MMC_CARD_TYPE_SD20)
					info->type = MMC_CARD_TYPE_SD;
				return TRUE;
			}
		}
	}

	info->type = MMC_CARD_TYPE_NONE;
	return FALSE;
}

static bool_t mmc_send_op_cond(struct mmc_card_info_t * info)
{
	u32_t resp[4];
	u32_t i;

	if(!mmc_idle_cards())
		return FALSE;

	for(i=0; i<100; i++)
	{
		if(mmc_send_cmd(MMC_SEND_OP_COND, MMC_VDD_33_34 | 0x40000000, &resp[0], REALVIEW_MCI_RSP_PRESENT))
		{
			if(resp[0] & 0x80000000)
			{
				info->type = MMC_CARD_TYPE_MMC;
				return TRUE;
			}
		}
	}

	info->type = MMC_CARD_TYPE_NONE;
	return FALSE;
}

void realview_mmc_init(void)
{
	/* power on and rod control */
	write32(REALVIEW_MCI_POWER, 0x83);

	write32(REALVIEW_MCI_MASK0, 0x0);
	write32(REALVIEW_MCI_MASK1, 0x0);

	write32(REALVIEW_MCI_COMMAND, 0x0);
	write32(REALVIEW_MCI_DATA_CTRL, 0x0);
}

void realview_mmc_exit(void)
{
	write32(REALVIEW_MCI_MASK0, 0x0);
	write32(REALVIEW_MCI_MASK1, 0x0);

	write32(REALVIEW_MCI_COMMAND, 0x0);
	write32(REALVIEW_MCI_DATA_CTRL, 0x0);

	/* power off */
	write32(REALVIEW_MCI_POWER, 0x0);
}

bool_t realview_mmc_probe(struct mmc_card_info_t * info)
{
	u32_t resp[4];
	bool_t ret;

	/*
	 * go idle mode
	 */
	if(!mmc_idle_cards())
		return FALSE;

	/*
	 * test for sd version 2
	 */
	mmc_send_if_cond(info);

	/* try to get the sd card's operating condition */
	if(!sd_send_op_cond(info))
	{
		if(!mmc_send_op_cond(info))
			return FALSE;
	}

	/*
	 * get the attached card's cid
	 */
	ret = mmc_send_cmd(MMC_ALL_SEND_CID, 0, resp, REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_136BIT | REALVIEW_MCI_RSP_CRC);
	if(!ret)
		return FALSE;

	info->raw_cid[0] = resp[0];
	info->raw_cid[1] = resp[1];
	info->raw_cid[2] = resp[2];
	info->raw_cid[3] = resp[3];

	/*
	 * send relative card address
	 */
	switch(info->type)
	{
	case MMC_CARD_TYPE_MMC:
		info->rca = 0x0001;
		ret = mmc_send_cmd(MMC_SET_RELATIVE_ADDR, info->rca << 16, resp, REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_CRC);
		if(!ret)
			return FALSE;
		break;

	case MMC_CARD_TYPE_SD:
	case MMC_CARD_TYPE_SD20:
	case MMC_CARD_TYPE_SDHC:
		ret = mmc_send_cmd(SD_SEND_RELATIVE_ADDR, 0x0000 << 16, resp, REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_CRC);
		if(!ret)
			return FALSE;
		info->rca = (resp[0] >> 16) & 0xffff;
		break;

	default:
		info->rca = 0;
		return FALSE;
	}

	/*
	 * get the card specific data
	 */
	ret = mmc_send_cmd(MMC_SEND_CSD, (u32_t)(info->rca) << 16, resp, REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_136BIT | REALVIEW_MCI_RSP_CRC);
	if(!ret)
		return FALSE;

	info->raw_csd[0] = resp[0];
	info->raw_csd[1] = resp[1];
	info->raw_csd[2] = resp[2];
	info->raw_csd[3] = resp[3];

	/*
	 * select the card, and put it into transfer mode
	 */
	ret = mmc_send_cmd(MMC_SELECT_CARD, (u32_t)(info->rca) << 16, resp, REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_CRC);
	if(!ret)
		return FALSE;

	/*
	 * deselect the card, and put it into standby mode
	 */
	ret = mmc_send_cmd(MMC_SELECT_CARD, 0, resp, REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_CRC);
	if(!ret)
		return FALSE;

	return TRUE;
}

bool_t realview_mmc_read_one_sector(struct mmc_card_t * card, u8_t * buf, u32_t sector)
{
	u32_t resp[4];
	u32_t blk_bits = card->info->csd.read_blkbits;
	u32_t blk_len = 1 << blk_bits;
	s32_t i, len, remain = blk_len;
	u8_t * p = buf;
	u32_t status;
	u32_t v;
	bool_t ret;

	/*
	 * select the card, and put it into transfer mode
	 */
	ret = mmc_send_cmd(MMC_SELECT_CARD, (u32_t)(card->info->rca) << 16, resp, REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_CRC);
	if(!ret)
		return FALSE;

	/*
	 * always do full block reads from the card
	 */
	ret = mmc_send_cmd(MMC_SET_BLOCKLEN, blk_len, resp, REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_CRC);
	if(!ret)
		return FALSE;

	write32(REALVIEW_MCI_DATA_TIMER, 0xffff);
	write32(REALVIEW_MCI_DATA_LENGTH, blk_len);
	write32(REALVIEW_MCI_DATA_CTRL, (0x1<<0) | (0x1<<1) | (blk_bits<<4));

	if(card->info->type == MMC_CARD_TYPE_SDHC)
	{
		ret = mmc_send_cmd(MMC_READ_SINGLE_BLOCK, sector, resp, REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_CRC);
		if(!ret)
			return FALSE;
	}
	else
	{
		ret = mmc_send_cmd(MMC_READ_SINGLE_BLOCK, sector * blk_len, resp, REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_CRC);
		if(!ret)
			return FALSE;
	}

	do {
		len = remain - (read32(REALVIEW_MCI_FIFO_CNT) << 2);
		kprintf("len: %d\n", len);

		if(len <= 0)
			break;

		for(i = 0; i < (len >> 2); i++)
		{
			v = read32(REALVIEW_MCI_FIFO);
			kprintf("v: %x\n", v);
			*(p++) = (v >> 0) & 0xff;
			*(p++) = (v >> 8) & 0xff;
			*(p++) = (v >> 16) & 0xff;
			*(p++) = (v >> 24) & 0xff;
		}

		remain -= len;

		if(remain <= 0)
			break;

		status = read32(REALVIEW_MCI_STATUS);
		if((status & REALVIEW_MCI_STAT_DAT_TIME_OUT) || (status & REALVIEW_MCI_STAT_DAT_CRC_FAIL))
		{
			write32(REALVIEW_MCI_CLEAR, (REALVIEW_MCI_CLR_DAT_TIMEOUT | REALVIEW_MCI_CLR_DAT_CRC_FAIL));

			mmc_send_cmd(MMC_SELECT_CARD, 0, resp, REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_CRC);
			return FALSE;
		}

	} while(1);

	/*
	 * deselect the card, and put it into standby mode
	 */
	ret = mmc_send_cmd(MMC_SELECT_CARD, 0, resp, REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_CRC);
	if(!ret)
		return FALSE;

	return TRUE;
}

bool_t realview_mmc_write_one_sector(struct mmc_card_t * card, const u8_t * buf, u32_t sector)
{
	u32_t resp[4];
	u32_t blk_bits = card->info->csd.write_blkbits;
	u32_t blk_len = 1 << blk_bits;
	s32_t i, remain = blk_len;
	u8_t * p = (u8_t *)buf;
	u32_t status;
	bool_t ret;

	/*
	 * select the card, and put it into transfer mode
	 */
	ret = mmc_send_cmd(MMC_SELECT_CARD, (u32_t)(card->info->rca) << 16, resp, REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_CRC);
	if(!ret)
		return FALSE;

	/*
	 * always do full block writes to the card
	 */
	ret = mmc_send_cmd(MMC_SET_BLOCKLEN, blk_len, resp, REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_CRC);
	if(!ret)
		return FALSE;

	write32(REALVIEW_MCI_DATA_TIMER, 0xffff);
	write32(REALVIEW_MCI_DATA_LENGTH, blk_len);
	write32(REALVIEW_MCI_DATA_CTRL, (0x1<<0) | (0x0<<1) | (blk_bits<<4));

	if(card->info->type == MMC_CARD_TYPE_SDHC)
	{
		ret = mmc_send_cmd(MMC_WRITE_SINGLE_BLOCK, sector, resp, REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_CRC);
		if(!ret)
			return FALSE;
	}
	else
	{
		ret = mmc_send_cmd(MMC_WRITE_SINGLE_BLOCK, sector * blk_len, resp, REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_CRC);
		if(!ret)
			return FALSE;
	}

	do {
		for(i=0; i<64; i++)
		{
			write8(REALVIEW_MCI_FIFO, *(p++));
		}

		remain -= 64;

		if(remain <= 0)
			break;

		status = read32(REALVIEW_MCI_STATUS);
		if((status & REALVIEW_MCI_STAT_DAT_TIME_OUT) || (status & REALVIEW_MCI_STAT_DAT_CRC_FAIL))
		{
			write32(REALVIEW_MCI_CLEAR, (REALVIEW_MCI_CLR_DAT_TIMEOUT | REALVIEW_MCI_CLR_DAT_CRC_FAIL));

			mmc_send_cmd(MMC_SELECT_CARD, 0, resp, REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_CRC);
			return FALSE;
		}
	} while(1);

	/*
	 * deselect the card, and put it into standby mode
	 */
	ret = mmc_send_cmd(MMC_SELECT_CARD, 0, resp, REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_CRC);
	if(!ret)
		return FALSE;

	return TRUE;
}

bool_t realview_mmc_read_sectors(struct mmc_card_t * card, u8_t * buf, u32_t sector, u32_t count)
{
	u32_t blk_bits = card->info->csd.write_blkbits;
	u32_t blk_len = 1 << blk_bits;
	bool_t ret;

	while(count)
	{
		ret = realview_mmc_read_one_sector(card, buf, sector);
		if(ret != TRUE)
			return FALSE;

		count--;
		sector++;
		buf += blk_len;
	}

	return TRUE;
}

bool_t realview_mmc_write_sectors(struct mmc_card_t * card, const u8_t * buf, u32_t sector, u32_t count)
{
	u32_t blk_bits = card->info->csd.write_blkbits;
	u32_t blk_len = 1 << blk_bits;
	bool_t ret;

	while(count)
	{
		ret = realview_mmc_write_one_sector(card, buf, sector);
		if(ret != TRUE)
			return FALSE;

		count--;
		sector++;
		buf += blk_len;
	}

	return TRUE;
}

