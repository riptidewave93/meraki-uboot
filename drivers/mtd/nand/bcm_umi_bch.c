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
#include <asm/arch/nand_bcm_umi.h>

/* ---- External Variable Declarations ----------------------------------- */
/* ---- External Function Prototypes ------------------------------------- */
/* ---- Public Variables ------------------------------------------------- */
/* ---- Private Constants and Types -------------------------------------- */

/* ---- Private Function Prototypes -------------------------------------- */
static int bcm_umi_bch_read_page_hwecc(struct mtd_info *mtd,
	struct nand_chip *chip, uint8_t *buf, int page);
static void bcm_umi_bch_write_page_hwecc(struct mtd_info *mtd,
	struct nand_chip *chip, const uint8_t *buf);

/* ---- Private Variables ------------------------------------------------ */

/* ---- Private Functions ------------------------------------------------ */
/* ==== Public Functions ================================================= */

/****************************************************************************
*
*  bcm_umi_bch_read_page_hwecc - hardware ecc based page read function
*  @mtd:	mtd info structure
*  @chip:	nand chip info structure
*  @buf:	buffer to store read data
*
***************************************************************************/
static int bcm_umi_bch_read_page_hwecc(struct mtd_info *mtd,
				       struct nand_chip *chip, uint8_t * buf, int page)
{
	int sectorIdx = 0;
	int eccsize = chip->ecc.size;
	int eccsteps = chip->ecc.steps;
	uint8_t *datap = buf;
	uint8_t eccCalc[NAND_ECC_NUM_BYTES];
	int sectorOobSize = mtd->oobsize / eccsteps;
	int stat;

   /* Page is needed in API but not used */
   (void) page;

	for (sectorIdx = 0; sectorIdx < eccsteps;
			sectorIdx++, datap += eccsize) {
		if (sectorIdx > 0) {
			/* Seek to page location within sector */
			chip->cmdfunc(mtd, NAND_CMD_RNDOUT, sectorIdx * eccsize,
				      -1);
		}

		/* Enable hardware ECC before reading the buf */
		nand_bcm_umi_bch_enable_read_hwecc();

		/* Read in data */
		bcm_umi_nand_read_buf(mtd, datap, eccsize);

		/* Pause hardware ECC after reading the buf */
		nand_bcm_umi_bch_pause_read_ecc_calc();

		/* Read the OOB ECC */
		if (mtd->writesize > 512) {
			/* 
			 * For 512 byte page devices, we are already at the oob 
			 * location and don't need a READOOB command. Also  
			 * RNDOUT isn't available in general on 512 byte devices.
			 */
			chip->cmdfunc(mtd, NAND_CMD_RNDOUT,
						mtd->writesize + sectorIdx * sectorOobSize, -1);
		}
		nand_bcm_umi_bch_read_oobEcc(mtd->writesize, eccCalc,
					     NAND_ECC_NUM_BYTES,
					     chip->oob_poi +
					     sectorIdx * sectorOobSize);

		/* Correct any ECC detected errors */
		stat =
		    nand_bcm_umi_bch_correct_page(datap, eccsize, eccCalc,
						  NAND_ECC_NUM_BYTES);

		/* Update Stats */
		if (stat < 0) {
#if defined(NAND_BCM_UMI_DEBUG)
			printk(KERN_WARNING "%s uncorr_err sectorIdx=%d\n",
			       __func__, sectorIdx);
			printk(KERN_WARNING
			       "%s data %02x %02x %02x %02x"
					 "%02x %02x %02x %02x\n",
			       __func__, datap[0], datap[1], datap[2], datap[3],
			       datap[4], datap[5], datap[6], datap[7]);
			printk(KERN_WARNING
			       "%s ecc  %02x %02x %02x %02x"
					 "%02x %02x %02x %02x %02x %02x"
					 "%02x %02x %02x\n",
			       __func__, eccCalc[0], eccCalc[1], eccCalc[2],
			       eccCalc[3], eccCalc[4], eccCalc[5], eccCalc[6],
			       eccCalc[7], eccCalc[8], eccCalc[9], eccCalc[10],
			       eccCalc[11], eccCalc[12]);
			BUG();
#endif
			mtd->ecc_stats.failed++;
		} else {
#if defined(NAND_BCM_UMI_DEBUG)
			if (stat > 0) {
				printk(KERN_INFO
				       "%s %d correctable_errors detected\n",
				       __func__, stat);
			}
#endif
			mtd->ecc_stats.corrected += stat;
		}
	}
	return 0;
}

/****************************************************************************
*
*  bcm_umi_bch_write_page_hwecc - hardware ecc based page write function
*  @mtd:	mtd info structure
*  @chip:	nand chip info structure
*  @buf:	data buffer
*
***************************************************************************/
static void bcm_umi_bch_write_page_hwecc(struct mtd_info *mtd,
	struct nand_chip *chip, const uint8_t *buf)
{
	int sectorIdx = 0;
	int eccsize = chip->ecc.size;
	int eccsteps = chip->ecc.steps;
	const uint8_t *datap = buf;
	uint8_t *oobp = chip->oob_poi;
	int sectorOobSize = mtd->oobsize / eccsteps;

	for (sectorIdx = 0; sectorIdx < eccsteps;
	     sectorIdx++, datap += eccsize, oobp += sectorOobSize) {
		/* Enable hardware ECC before writing the buf */
		nand_bcm_umi_bch_enable_write_hwecc();
		bcm_umi_nand_write_buf(mtd, datap, eccsize);
		nand_bcm_umi_bch_write_oobEcc(mtd->writesize, oobp,
					      NAND_ECC_NUM_BYTES);
	}

	bcm_umi_nand_write_buf(mtd, chip->oob_poi, mtd->oobsize);
}
