/*****************************************************************************
* Copyright 2004 - 2009 Broadcom Corporation.  All rights reserved.
*
* Unless you and Broadcom execute a separate written software license
* agreement governing use of this software, this software is licensed to you
* under the terms of the GNU General Public License version 2, available at
* http://www.broadcom.com/licenses/GPLv2.php (the "GPL").
*
* Notwithstanding the above, under no circumstances may you combine this
* software in any way with any other Broadcom software provided under a
* license other than the GPL, without Broadcom's express prior written
* consent.
*****************************************************************************/

/* ---- Include Files ---------------------------------------------------- */
//#include <linux/types.h>
//#include <linux/err.h>
//#include <linux/mtd/compat.h>
#include <asm/errno.h>

//#include <linux/string.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
//#include <linux/mtd/nand_ecc.h>
//#include <linux/mtd/partitions.h>

#include <common.h>

#include <asm/io.h>
#include <asm/mach-types.h>
#include <asm/system.h>

#include <asm/arch/reg_nand.h>
#include <asm/arch/reg_umi.h>
#include <asm/arch/nand_bcm_umi.h>

#define USE_HWECC 1

/* ---- External Variable Declarations ----------------------------------- */
/* ---- External Function Prototypes ------------------------------------- */
/* ---- Public Variables ------------------------------------------------- */
/* ---- Private Constants and Types -------------------------------------- */
static const char gBanner[] = "BCM UMI MTD NAND Driver: 1.00\n";

/*
** Preallocate a buffer to avoid having to do this every dma operation.
** This is the size of the preallocated coherent DMA buffer.
*/
#define DMA_MIN_BUFLEN          0
#define DMA_MAX_BUFLEN          0
#define USE_DIRECT_IO(len)      1

/* ---- Private Function Prototypes -------------------------------------- */
static void bcm_umi_nand_read_buf(struct mtd_info *mtd, u_char * buf, int len);
static void bcm_umi_nand_write_buf(struct mtd_info *mtd, const u_char * buf,
				   int len);

/* ---- Private Variables ------------------------------------------------ */
static void __iomem *bcm_umi_io_base;

/* ---- Private Functions ------------------------------------------------ */
#if NAND_ECC_BCH
#include "bcm_umi_bch.c"
#else
#include "bcm_umi_hamming.c"
#endif

/****************************************************************************
*
*  nand_dev_ready
*
*   Routine to check if nand is ready.
*
***************************************************************************/
static int nand_dev_ready(struct mtd_info *mtd)
{
	return nand_bcm_umi_dev_ready();
}

/****************************************************************************
*
*  bcm_umi_nand_inithw
*
*   This routine does the necessary hardware (board-specific)
*   initializations.  This includes setting up the timings, etc.
*
***************************************************************************/

int bcm_umi_nand_inithw(void)
{
	/* Configure nand timing parameters */

	REG_UMI_NAND_TCR &= ~0x7ffff;
	REG_UMI_NAND_TCR |= HW_CFG_NAND_TCR;

#if !defined(CONFIG_MTD_NAND_BCM_UMI_HWCS)
	/* enable software control of CS */
	REG_UMI_NAND_TCR |= REG_UMI_NAND_TCR_CS_SWCTRL;
#endif

	/* keep NAND chip select asserted */
	REG_UMI_NAND_RCSR |= REG_UMI_NAND_RCSR_CS_ASSERTED;

	REG_UMI_NAND_TCR &= ~REG_UMI_NAND_TCR_WORD16;
	/* enable writes to flash */
	REG_UMI_MMD_ICR |= REG_UMI_MMD_ICR_FLASH_WP;

	writel(NAND_CMD_RESET, bcm_umi_io_base + REG_NAND_CMD_OFFSET);
	nand_bcm_umi_wait_till_ready();

#if NAND_ECC_BCH
	nand_bcm_umi_bch_config_ecc(NAND_ECC_NUM_BYTES);
#endif

	return 0;
}

/****************************************************************************
*
*  bcm_umi_nand_hwcontrol
*
*   Used to turn latch the proper register for access.
*
***************************************************************************/

static void bcm_umi_nand_hwcontrol(struct mtd_info *mtd, int cmd,
				   unsigned int ctrl)
{
	/* send command to hardware */
	struct nand_chip *chip = mtd->priv;
	if (ctrl & NAND_CTRL_CHANGE) {
		if (ctrl & NAND_CLE) {
			chip->IO_ADDR_W = bcm_umi_io_base + REG_NAND_CMD_OFFSET;
			goto CMD;
		}
		if (ctrl & NAND_ALE) {
			chip->IO_ADDR_W =
			    bcm_umi_io_base + REG_NAND_ADDR_OFFSET;
			goto CMD;
		}
		chip->IO_ADDR_W = bcm_umi_io_base + REG_NAND_DATA8_OFFSET;
	}

CMD:
	/* Send command to chip directly */
	if (cmd != NAND_CMD_NONE)
		writeb(cmd, chip->IO_ADDR_W);
}

/****************************************************************************
*
*  bcm_umi_nand_write_buf - write buffer to chip
*  @mtd:	MTD device structure
*  @buf:	data buffer
*  @len:	number of bytes to write
*
***************************************************************************/
static void bcm_umi_nand_write_buf(struct mtd_info *mtd, const u_char * buf,
				   int len)
{
	if (USE_DIRECT_IO(len)) {
		/* Do it the old way if the buffer is small or too large.
		 * Probably quicker than starting and checking dma. */
		int i;
		struct nand_chip *this = mtd->priv;

		for (i = 0; i < len; i++)
			writeb(buf[i], this->IO_ADDR_W);
	}
}

/****************************************************************************
*
*  nand_read_buf - read chip data into buffer
*  @mtd:	MTD device structure
*  @buf:	buffer to store data
*  @len:	number of bytes to read
*
***************************************************************************/

static void bcm_umi_nand_read_buf(struct mtd_info *mtd, u_char * buf, int len)
{
	if (USE_DIRECT_IO(len)) {
		int i;
		struct nand_chip *this = mtd->priv;

		for (i = 0; i < len; i++)
			buf[i] = readb(this->IO_ADDR_R);
	}
}

/****************************************************************************
*
*  bcm_umi_nand_verify_buf - Verify chip data against buffer
*  @mtd:	MTD device structure
*  @buf:	buffer containing the data to compare
*  @len:	number of bytes to compare
*
***************************************************************************/
static uint8_t readbackbuf[NAND_MAX_PAGESIZE];
static int bcm_umi_nand_verify_buf(struct mtd_info *mtd, const u_char * buf,
				   int len)
{
	/*
	 * Try to readback page with ECC correction. This is necessary
	 * for MLC parts which may have permanently stuck bits.
	 */
	struct nand_chip *chip = mtd->priv;
	int ret = chip->ecc.read_page(mtd, chip, readbackbuf, 0);
	if (ret < 0)
		return -EFAULT;
	else {
		if (memcmp(readbackbuf, buf, len) == 0)
			return 0;

		return -EFAULT;
	}
	return 0;
}

int board_nand_init(struct nand_chip *nand)
{
	struct nand_chip *this;

	debug(gBanner);

	bcm_umi_io_base = (void __iomem *)MM_ADDR_IO_NAND;

	if (!bcm_umi_io_base) {
		printk(KERN_ERR "ioremap to access BCM UMI NAND chip failed\n");
		return -EIO;
	}

	/* Get pointer to private data */
	this = (struct nand_chip *) nand;

	/* Initialize the NAND hardware.  */
	if (bcm_umi_nand_inithw() < 0) {
		printk(KERN_ERR "BCM UMI NAND chip could not be initialized\n");
		return -EIO;
	}

	/* Set address of NAND IO lines */
	this->IO_ADDR_W = bcm_umi_io_base + REG_NAND_DATA8_OFFSET;
	this->IO_ADDR_R = bcm_umi_io_base + REG_NAND_DATA8_OFFSET;

	/* Set command delay time, see datasheet for correct value */
	this->chip_delay = 0;
	/* Assign the device ready function, if available */
	this->dev_ready = nand_dev_ready;
	this->options = 0;

	this->write_buf = bcm_umi_nand_write_buf;
	this->read_buf = bcm_umi_nand_read_buf;
	this->verify_buf = bcm_umi_nand_verify_buf;

	this->cmd_ctrl = bcm_umi_nand_hwcontrol;
#if USE_HWECC
	this->ecc.mode = NAND_ECC_HW;
	this->ecc.size = 512;
	this->ecc.bytes = NAND_ECC_NUM_BYTES;

#if NAND_ECC_BCH
	this->ecc.read_page = bcm_umi_bch_read_page_hwecc;
	this->ecc.write_page = bcm_umi_bch_write_page_hwecc;
#else
	this->ecc.correct = nand_correct_data512;
	this->ecc.calculate = bcm_umi_hamming_get_hw_ecc;
	this->ecc.hwctl = bcm_umi_hamming_enable_hwecc;
#endif
#else
	this->ecc.mode = NAND_ECC_SOFT;
#endif

	/* Return happy */
	return 0;
}
