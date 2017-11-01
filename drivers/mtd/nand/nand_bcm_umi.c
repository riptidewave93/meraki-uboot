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
#include <asm/arch/reg_umi.h>
#include <asm/arch/nand_bcm_umi.h>

/* ---- External Variable Declarations ----------------------------------- */
/* ---- External Function Prototypes ------------------------------------- */
/* ---- Public Variables ------------------------------------------------- */
/* ---- Private Constants and Types -------------------------------------- */
/* ---- Private Function Prototypes -------------------------------------- */
/* ---- Private Variables ------------------------------------------------ */
/* ---- Private Functions ------------------------------------------------ */

#if NAND_ECC_BCH
/****************************************************************************
*  nand_bch_ecc_flip_bit - Routine to flip an errored bit
*
*  PURPOSE:
*     This is a helper routine that flips the bit (0 -> 1 or 1 -> 0) of the
*     errored bit specified
*
*  PARAMETERS:
*     datap - Container that holds the 512 byte data
*     errorLocation - Location of the bit that needs to be flipped
*
*  RETURNS:
*     None
****************************************************************************/
static void nand_bcm_umi_bch_ecc_flip_bit(uint8_t *datap, int errorLocation)
{
	int locWithinAByte = (errorLocation & REG_UMI_BCH_ERR_LOC_BYTE) >> 0;
	int locWithinAWord = (errorLocation & REG_UMI_BCH_ERR_LOC_WORD) >> 3;
	int locWithinAPage = (errorLocation & REG_UMI_BCH_ERR_LOC_PAGE) >> 5;

	uint8_t errorByte = 0;
	uint8_t byteMask = 1 << locWithinAByte;

	/* BCH uses big endian, need to change the location
	 * bits to little endian */
	locWithinAWord = 3 - locWithinAWord;

	errorByte = datap[locWithinAPage * sizeof(uint32_t) + locWithinAWord];

	if (errorByte & byteMask) {
		/* bit needs to be cleared */
		errorByte &= ~byteMask;
	} else {
		/* bit needs to be set */
		errorByte |= byteMask;
	}

	/* write back the value with the fixed bit */
	datap[locWithinAPage * sizeof(uint32_t) + locWithinAWord] = errorByte;
}

/****************************************************************************
*  nand_correct_page_bch - Routine to correct bit errors when reading NAND
*
*  PURPOSE:
*     This routine reads the BCH registers to determine if there are any bit
*     errors during the read of the last 512 bytes of data + ECC bytes.  If
*     errors exists, the routine fixes it.
*
*  PARAMETERS:
*     datap - Container that holds the 512 byte data
*     len - data ecc is over length
*     readEccData - ecc data read into a buffer
*     numEccBytes - ecc length
*
*  RETURNS:
*     0 or greater = Number of errors corrected
*                    (No errors are found or errors have been fixed)
*    -1 = Error(s) cannot be fixed
****************************************************************************/
int nand_bcm_umi_bch_correct_page(uint8_t *datap, int datalen, uint8_t *readEccData,
				  int numEccBytes)
{
	int numErrors;
	int errorLocation;
	int idx;
	uint32_t regValue;
	uint32_t *longwordp;

	/* wait for read ECC to be valid */
	regValue = nand_bcm_umi_bch_poll_read_ecc_calc();

	/*
	 * read the control status register to determine if there
	 * are error'ed bits
	 * see if errors are correctible
	 */
	if ((regValue & REG_UMI_BCH_CTRL_STATUS_UNCORR_ERR) > 0) {
		int i;

		for (i = 0; i < numEccBytes; i++) {
			if (readEccData[i] != 0xff) {
				/* errors cannot be fixed, return -1 */
				return -1;
			}
		}
		/* 
		 * If bits in an erased page are stuck low and 
		 * the ECC is all FF, then we should report
		 * the stuck low bits as uncorrectible errors
		 * as the ECC is unprogrammed. Later, if the
		 * page is still in use (block not marked bad
		 * by filesystem code) and programmed, the ECC 
		 * can then recover stuck low bits. For stuck 
		 * high bits, this check won't help, but that
		 * will be detected upon initial programming 
		 * and again handled by the file system.
		 */
		numErrors = 0;
		longwordp = (uint32_t *)datap;
		for (i = 0; i < (int)(datalen/sizeof(uint32_t)); i++, longwordp++) {
			if (*longwordp != 0xffffffff) {
			  int j;
			  for (j = 0; j < 32; j++) {
				  if ((*longwordp & (1<<j)) == 0) {
						  numErrors++;
				  }
			  }
			  *longwordp = 0xffffffff; /* Fake correction */
			}
		}
		return numErrors;
	}

	if ((regValue & REG_UMI_BCH_CTRL_STATUS_CORR_ERR) == 0) {
		/* no errors */
		return 0;
	}

	/*
	 * Fix errored bits by doing the following:
	 * 1. Read the number of errors in the control and status register
	 * 2. Read the error location registers that corresponds to the number
	 *    of errors reported
	 * 3. Invert the bit in the data
	 */
	numErrors = (regValue & REG_UMI_BCH_CTRL_STATUS_NB_CORR_ERROR) >> 20;

	for (idx = 0; idx < numErrors; idx++) {
		errorLocation =
		    REG_UMI_BCH_ERR_LOC_ADDR(idx) & REG_UMI_BCH_ERR_LOC_MASK;

		/* Flip bit */
		nand_bcm_umi_bch_ecc_flip_bit(datap, errorLocation);
	}
	/* Errors corrected */
	return numErrors;
}
#endif
