/**
 * APM SoC APM86xxx UFC  
 *
 * Copyright (c) 2011 Applied Micro Circuits Corporation.
 * All rights reserved. Feng Kan <fkan@apm.com>.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 * Overview:
 *   Platform independend driver for UFC (Universal Flash Controller)
 *   integrated into Lanai SoC from Naksha.
 *
 * Based on original work by
 *	Thomas Gleixner
 *	Copyright 2006 IBM
 *
 */
#include <common.h>
#include <malloc.h>
#include <nand.h>

#include <linux/mtd/ufc.h>
#include <linux/mtd/nand_ecc.h>

#include <asm/io.h>
#include <asm/errno.h>
#include <asm/apm_ipp_csr.h>
#include <asm/apm_scu.h>
#include <asm/bch_port.h>
#include <bch.h>

#undef DEBUG
#undef DEBUG_CSR

#ifdef DEBUG 
#define UFC_DBG(format, arg...) printf(format, ##arg)
#else
#define UFC_DBG(format, arg...) do {} while (0)
#endif

#ifdef DEBUG_CSR 
#define UFC_CSRDBG(format, arg...) printf(format, ##arg)
#else
#define UFC_CSRDBG(format, arg...) do {} while (0)
#endif

#define	NAND_PKT_SIZE	256
#define	NAND_PKT_CNT	4
#define	NAND_UFC_READ	0
#define	NAND_UFC_WR	1

#if !defined(CONFIG_NAND_SPL_S1) && defined(CONFIG_ECCBCH)
static struct nand_ecclayout nand_swecc_4 = {
        .eccbytes = 28,
        .eccpos = { 	32, 33, 34, 35, 36, 37, 38, 
			40, 41, 42, 43, 44, 45, 46, 
			48, 49, 50, 51, 52, 53, 54, 
			56, 57, 58, 59, 60, 61, 62 },
        .oobfree = {
                {.offset = 2,
                 .length = 30},
        }
};
#endif

struct ufc_state {
	/* UFC interface is 32-bit access. Need shadow to support 8-bit access */
	int shadow_rd_offset;
	unsigned int shadow_rd_val;
};

/**
 * struct nand_bch_control - private NAND BCH control structure
 * @bch:       BCH control structure
 * @ecclayout: private ecc layout for this BCH configuration
 * @errloc:    error location array
 * @eccmask:   XOR ecc mask, allows erased pages to be decoded as valid
 */
struct nand_bch_control {
	struct bch_control   *bch;
	struct nand_ecclayout ecclayout;
	unsigned int         *errloc;
	unsigned char        *eccmask;
};

#if !defined(CONFIG_NAND_SPL)
/* NAND SPL can not have global data as it is running direct from device */
static struct ufc_state ufc_chip_state[CONFIG_SYS_MAX_NAND_DEVICE];
static u32 ufc_cs;
#endif

static inline void OUTLE32(void *addr, unsigned int val)
{
	out_le32(addr, val);
	UFC_CSRDBG("UFC CSR WR %p val 0x%08X\n", addr, val);
}

static inline unsigned int INLE32(void *addr)
{
	unsigned int val = in_le32(addr);	
	UFC_CSRDBG("UFC CSR RD %p val 0x%08X\n", addr, val);
	return val;
}

static inline void OUTBE32(void *addr, unsigned int val)
{
	out_be32((volatile unsigned int *)addr, val);
	UFC_CSRDBG("SCU CSR WR %p val 0x%08X\n", addr, val);
}

static inline unsigned int INBE32(void *addr)
{
	unsigned int val = in_be32(addr);	
	UFC_CSRDBG("SCU CSR RD %p val 0x%08X\n", addr, val);
	return val;
}

static inline void ufc_cs_nand(void)
{
#if !defined(CONFIG_NAND_SPL)
	/* No need to CS change for NAND SPL */
	ufc_cs = INLE32((u32 *)(UFC_BASE_ADDR + UFC_FLAG));
	OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_FLAG),
			UFC_FLAG_EN(CONFIG_SYS_NAND_CS));
#endif
}

static inline void ufc_cs_restore(void)
{
#if !defined(CONFIG_NAND_SPL)
	/* No need to CS change for NAND SPL */
	OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_FLAG), ufc_cs);
#endif
}

#if !defined(CONFIG_NAND_SPL)
static void ufc_hwcontrol(struct mtd_info *mtd, int cmd, unsigned int ctrl)
{
}
#endif

static int ufc_dev_ready(struct mtd_info *mtdinfo)
{
	volatile u32 timeout = 0x10000;
	u32 reg_val;

	do {
		reg_val = INLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL));
		udelay(1);
	} while ((reg_val & UFC_CTRL_BUSY) && (timeout--));

	return timeout;
}

void ufc_dev_done(void)
{
	volatile u32 timeout = 0x20;
	u32 reg_val;
	
	do {
		reg_val = INLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL));
/*                udelay(1);*/
	} while ((reg_val & UFC_CTRL_START) && timeout--);
}

#if 0
/* UFC_AUTO_TX = 1 UFC_AUTO_RX = 0 */
static int ufc_chk_rdy(unsigned int tx)
{

        volatile u32 timeout = 0x10000;
        u32 reg_val = 0, tmp;

        reg_val = INLE32((u32 *)(UFC_BASE_ADDR + UFC_INT_STAT));
	if (tx)
		tmp = UFC_TX_BUF_WRRDY_INT;
	else
		tmp = UFC_RX_BUF_RDRDY_INT;

       	while( (!(reg_val & tmp)) && (timeout--) ) {
               	reg_val = in_le32((u32 *)(UFC_BASE_ADDR + UFC_INT_STAT));
	}
	UFC_DBG("ufc_chk_rdy : INT status = 0x%x timeout = 0x%x\n", 
		reg_val, timeout);
	OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_INT_STAT), reg_val);
        return timeout;
}
#endif

#if 0
static int ufc_chk_wrrd_done()
{
        volatile u32 timeout = 0x1000;
        u32 reg_val = 0;

        reg_val = INLE32((u32 *)(UFC_BASE_ADDR + UFC_INT_STAT));
	while( (!(reg_val & UFC_WRRD_DONE_INT)) && (timeout--) )
                reg_val = INLE32((u32 *)(UFC_BASE_ADDR + UFC_INT_STAT));

	OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_INT_STAT), reg_val);
	if (timeout) {
		reg_val = INLE32((u32 *)(UFC_BASE_ADDR + UFC_STAT1)) & 
							UFC_RX_FIFO_CNT;
		UFC_DBG("ufc_chk_wrrd_done:data left = %x\n",reg_val);
	}

        return timeout;
}
#endif

#if 0
static void ufc_enable_hwecc(struct mtd_info *mtdinfo, int mode)
{
#if defined(CONFIG_UFC_HWECC)
	u32 ccr;
	/* Enable or disable the UFC ECC feature */
	ccr = INLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CMD_REG));
	if (mode == NAND_ECC_READ) {
		ccr &= ~UFC_ECC_ON_OFF;
	} else {
		ccr |= UFC_ECC_ON_OFF;
	}
	OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CMD_REG), ccr);
#else
	/* Enable or disable the NAND device ECC feature */
#endif
}

static int ufc_calculate_ecc(struct mtd_info *mtdinfo,
			      const u_char *dat, u_char *ecc_code)
{
#if defined(CONFIG_UFC_HWECC)
	/* Calculate ECC from UFC */
#else
	/* Calculate ECC from NAND device */	
#endif	
	return 0;
}
#endif

#if !defined(CONFIG_NAND_SPL)
/**
 * ufc_correct_data - [NAND Interface] Detect and correct bit error(s)
 * @mtd:        MTD block structure
 * @dat:        raw data read from the chip
 * @read_ecc:   ECC from the chip
 * @calc_ecc:   the ECC calculated from raw data
 *
 * Detect and correct a 1 bit error for 256 byte block for UFC.
 * Detect and correct a 4 bit error for 512 byte block for NAND device.
 */
int ufc_correct_data(struct mtd_info *mtd, u_char *dat,
                      u_char *read_ecc, u_char *calc_ecc)
{
#if defined(CONFIG_UFC_HWECC)
	/* Calculate ECC from UFC */
	u32 reg_val;
	reg_val = INLE32((u32 *) (UFC_BASE_ADDR + UFC_INT_STAT));
	return reg_val & UFC_MUL_BIT_ERR_INT;
#else
	/* Calculate ECC from NAND device */	
	return 0;
#endif
}

/**
 * ufc_read_page_hwecc - [REPLACABLE] hardware ecc based page read function
 * @mtd:        mtd info structure
 * @chip:       nand chip info structure
 * @buf:        buffer to store read data
 *
 * Not for syndrome calculating ecc controllers which need a special oob layout
 */
int ufc_read_page_hwecc(struct mtd_info *mtd, struct nand_chip *chip,
                                uint8_t *buf, int page)
{
        uint8_t *p = buf;

#if defined(CONFIG_UFC_HWECC)
        for (i = 0; eccsteps; eccsteps--, i += eccbytes, p += eccsize) {
                chip->read_buf(mtd, p, eccsize);
                if (chip->ecc.correct(mtd, p, NULL, NULL))
                        mtd->ecc_stats.failed++;
        }
#else
	chip->read_buf(mtd, p, mtd->writesize); 
#endif
        return 0;
}

/**
 * ufc_write_page_hwecc - UFC hardware ecc based page write function
 * @mtd:        mtd info structure
 * @chip:       nand chip info structure
 * @buf:        data buffer
 */
void ufc_write_page_hwecc(struct mtd_info *mtd, struct nand_chip *chip,
                                  const uint8_t *buf)
{
        const uint8_t *p = buf;

#if defined(CONFIG_UFC_HWECC)
       for (i = 0; eccsteps; eccsteps--, i += eccbytes, p += eccsize) {
                chip->ecc.hwctl(mtd, NAND_ECC_WRITE);
                chip->write_buf(mtd, p, eccsize);
                chip->ecc.calculate(mtd, p, NULL);
        }
#else
	chip->ecc.hwctl(mtd, NAND_ECC_WRITE);
	chip->write_buf(mtd, p, mtd->writesize); 
#endif
}
#endif

#if !defined(CONFIG_NAND_SPL_S1)
#if defined(CONFIG_ECCBCH)
static int ufc_sw_bch_init(struct nand_chip *nand)
{
	unsigned char *erased_page;
	struct nand_bch_control *nbc;
	int m = fls(1+8*nand->ecc.size);
	int t = (nand->ecc.bytes*8)/m;
	int i;
	
	UFC_DBG("UFC BCH initalize m %d t %d eccsize %d eccbyte %d\n", 
		m, t, nand->ecc.size, nand->ecc.bytes);
	nbc = KMALLOC(sizeof(*nbc), GFP_KERNEL);
	if (nbc == NULL)
		return -1;
	nbc->bch = init_bch(m, t, 0);
	if (nbc->bch == NULL) {
		KFREE(nbc);
		return -1;
	}
	nbc->eccmask = KMALLOC(nand->ecc.bytes, GFP_KERNEL);
	nbc->errloc = KMALLOC(t*sizeof(*nbc->errloc), GFP_KERNEL);
	/*
	 * compute and store the inverted ecc of an erased ecc block
	 */
	erased_page = KMALLOC(nand->ecc.size, GFP_KERNEL);
	if (!erased_page || !nbc->eccmask || !nbc->errloc)
		goto fail;

	memset(erased_page, 0xff, nand->ecc.size);
	memset(nbc->eccmask, 0, nand->ecc.bytes);
	encode_bch(nbc->bch, erased_page, nand->ecc.size, nbc->eccmask);
	KFREE(erased_page);

	for (i = 0; i < nand->ecc.bytes; i++)
		nbc->eccmask[i] ^= 0xff;
	
	nand->ecc.priv = nbc;
	return 0;
	
fail:
	if (nbc->eccmask)
		KFREE(nbc->eccmask);
	if (nbc->eccmask)
		KFREE(nbc->errloc);
	if (erased_page)
		KFREE(erased_page);
	return -1;
}
#endif /* defined(CONFIG_ECCBCH) */

#if !defined(CONFIG_NAND_SPL) && defined(CONFIG_ECCBCH)
static void ufc_enable_swecc(struct mtd_info *mtd, int mode)
{
}
#endif /* !defined(CONFIG_NAND_SPL) && defined(CONFIG_ECCBCH) */

#if defined(CONFIG_ECCBCH)
/**
 * ufc_sw_correct_data - [NAND Interface] Detect and correct bit error(s)
 * @mtd:        MTD block structure
 * @dat:        raw data read from the chip
 * @read_ecc:   ECC from the chip
 * @calc_ecc:   the ECC calculated from raw data
 *
 * Detect and correct a 4 bit error for 256 byte block
 */
int ufc_sw_correct_data(struct mtd_info *mtd, unsigned char *buf,
                      unsigned char *read_ecc, unsigned char *calc_ecc)
{
	struct nand_chip *chip = mtd->priv;
	struct nand_bch_control *nbc = chip->ecc.priv;
	unsigned int *errloc = nbc->errloc;
	int i, count;
	
	if (nbc == NULL) {
		if (ufc_sw_bch_init(chip) < 0)
			return -1; 
		nbc = chip->ecc.priv;
	}
	
	count = decode_bch(nbc->bch, NULL, chip->ecc.size, read_ecc, calc_ecc,
			   NULL, errloc);
	if (count > 0) {
		for (i = 0; i < count; i++) {
			if (errloc[i] < (chip->ecc.size*8))
				/* error is located in data, correct it */
				buf[errloc[i] >> 3] ^= (1 << (errloc[i] & 7));
			/* else error in ecc, no action needed */

			UFC_DBG("corrected bitflip %u\n", errloc[i]);
		}
	} else if (count < 0) {
		UFC_DBG("ecc unrecoverable error\n");
		count = -1;
	}
	return count;
}

int ufc_calculate_swecc(struct mtd_info *mtd, const unsigned char *buf,
			   unsigned char *code)
{	struct nand_chip *chip = mtd->priv;
	struct nand_bch_control *nbc = chip->ecc.priv;
	unsigned int i;

	if (nbc == NULL) {
		if (ufc_sw_bch_init(chip) < 0)
			return -1; 
		nbc = chip->ecc.priv;
	}
	
	memset(code, 0, chip->ecc.bytes);
	encode_bch(nbc->bch, buf, chip->ecc.size, code);
	
	/* apply mask so that an erased page is a valid codeword */
	for (i = 0; i < chip->ecc.bytes; i++)
		code[i] ^= nbc->eccmask[i];

	return 0;
}
#else /* !defined(CONFIG_ECCBCH) */
#define ufc_sw_correct_data NULL
#define ufc_calculate_swecc NULL
#endif /* !defined(CONFIG_ECCBCH) */

#endif /* !defined(CONFIG_NAND_SPL_S1) */

/*
 * UFC allows 32bit read/write of data. So we can speed up the buffer
 */
void ufc_read_buf(struct mtd_info *mtdinfo, uint8_t *buf, int len)
{
	
	uint32_t *p = (uint32_t *) buf;
#if 0
	/* FIXME */
	uint32_t frag_len;
	uint32_t reg_val = 0;
	uint32_t pkt_cnt;

	reg_val = in_le32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_COL_ADDR));
	
	if (reg_val >= mtdinfo->writesize) {
		/* Reading OOB - indirect READ */
		out_le32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), 
			UFC_CTRL_FIFO_RST);
	        out_le32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), 0x00000000);
		for (;len > 0; len -= 4) {
			/* Set the START bit */
                        out_le32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), 
                        	(UFC_CTRL_START 
                        	| UFC_CTRL_READ 
                        	| UFC_CTRL_DATACNT));
			ufc_dev_done();
			*p++ = in_le32((u32 *)(UFC_BASE_ADDR + UFC_RX_CMDDATA));
		}
	} else {
		/* calculate the spare area offset */
		out_le32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_SPARE_OFF), 0x28);
		
		pkt_cnt = len / NAND_PKT_SIZE;
                if(len % NAND_PKT_SIZE)
                        pkt_cnt++;
		reg_val = ((NAND_PKT_SIZE) | ((pkt_cnt) << UFC_PKTCNT_SHIFT));
		out_le32((u32 *)(UFC_BASE_ADDR + UFC_PKTCNT_PKTSIZE), reg_val);
		
		reg_val = in_le32((u32 *)(UFC_BASE_ADDR + UFC_INT_STAT));
		out_le32((u32 *)(UFC_BASE_ADDR + UFC_INT_STAT), reg_val);	 
       		
		reg_val = in_le32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL));	
		out_le32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), 
				(reg_val | UFC_CTRL_FIFO_RST));
		out_le32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), 
				(reg_val | UFC_CTRL_DATACNT));
		
		reg_val = (UFC_ECC_ON_OFF | UFC_PAGE_2K | UFC_FLASH_READ | 
				UFC_FLASH_AUTO);
		out_le32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CMD_REG), reg_val);
		ufc_dev_ready(NULL);	
		
		while(pkt_cnt) {
			if (ufc_chk_rdy(NAND_UFC_READ)) {
				frag_len = NAND_PKT_SIZE;
 				for (; frag_len > 0; frag_len -= 4) {
					*p++ = in_le32((u32 *)(UFC_BASE_ADDR + 
							UFC_RX_CMDDATA));
				}
				len -= frag_len;
				pkt_cnt--;
				reg_val = in_le32((u32 *)(UFC_BASE_ADDR + 
							UFC_STAT1));
			} else  {
				UFC_DBG("CHK RDY failure :Error in read\n");
				break;
			}
		}
#ifndef CONFIG_NAND_SPL
		if(ufc_chk_wrrd_done()) {
		}
#endif
	}
	out_le32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CMD_REG), 0x0);
#else
	int tmp = len;
	
	ufc_cs_nand();

	OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), UFC_CTRL_FIFO_RST);
	OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), 0x00000000);
	for (; tmp > 0; tmp -= 4) {
		OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), 
			UFC_CTRL_START | UFC_CTRL_READ | UFC_CTRL_DATACNT);
		ufc_dev_done();
		*p++ = INLE32((u32 *)(UFC_BASE_ADDR + UFC_RX_CMDDATA));
	}

	ufc_cs_restore();
#endif
}

unsigned char ufc_read_byte(struct mtd_info *mtd)
{
        register struct nand_chip *chip = mtd->priv;
	struct ufc_state *state = (struct ufc_state *) chip->priv;
	u32 val;
	int off;
	
	if (state == NULL || state->shadow_rd_offset == -1) {
		ufc_cs_nand();
			
		OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), 
				UFC_CTRL_FIFO_RST);
		OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), 0x00000000);
		OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), 
			UFC_CTRL_START | UFC_CTRL_READ | UFC_CTRL_DATACNT);
		ufc_dev_done();
		off = 0;
		val = INLE32((u32 *)(UFC_BASE_ADDR + UFC_RX_CMDDATA));
		if (state) {
			state->shadow_rd_offset = 0;
			state->shadow_rd_val = val;
		}	
		ufc_cs_restore();
	} else {
		off = state->shadow_rd_offset;
		val = state->shadow_rd_val;
	}

	switch (off % 4) {
	case 0:
	default:
		if (state)
			state->shadow_rd_offset++;
		return val & 0xFF;
	case 1:
		if (state)
			state->shadow_rd_offset++;
		return (val >> 8) & 0xFF;
	case 2:
		if (state)
			state->shadow_rd_offset++;
		return (val >> 16) & 0xFF;
	case 3:
		if (state)
			state->shadow_rd_offset = -1;
		return (val >> 24) & 0xFF;
	}
}

#if !defined(CONFIG_NAND_SPL)
unsigned short ufc_read_word(struct mtd_info *mtd)
{
        register struct nand_chip *chip = mtd->priv;
	struct ufc_state *state = (struct ufc_state *) chip->priv;
	
	if (state->shadow_rd_offset == -1) {
		ufc_cs_nand();
		
		OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), 
				UFC_CTRL_FIFO_RST);
		OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), 0x00000000);
		OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), 
			UFC_CTRL_START | UFC_CTRL_READ | UFC_CTRL_DATACNT);
		ufc_dev_done();
		state->shadow_rd_val = INLE32((u32 *)(UFC_BASE_ADDR + 
							UFC_RX_CMDDATA));
		state->shadow_rd_offset = 0;
		
		ufc_cs_restore();
	}

	switch (state->shadow_rd_offset % 2) {
	case 0:
	default:
		state->shadow_rd_offset += 2;
		return state->shadow_rd_val & 0xFFFF;
	case 2:
		state->shadow_rd_offset = -1;
		return (state->shadow_rd_val >> 16) & 0xFFFF;
	}
}

unsigned int ufc_read_long(void)
{
	ufc_cs_nand();

	OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), UFC_CTRL_FIFO_RST);
	OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), 0x00000000);
	OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), 
			UFC_CTRL_START | UFC_CTRL_READ | UFC_CTRL_DATACNT);
	ufc_dev_done();
	
	ufc_cs_restore();

	return INLE32((u32 *)(UFC_BASE_ADDR + UFC_RX_CMDDATA));
}

void ufc_write_byte(unsigned char val)
{
	ufc_cs_nand();

	OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), UFC_CTRL_FIFO_RST);
	OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), 0x00000000);
	OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_TX_CMDDATA), val);
	OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), 
			UFC_CTRL_START | UFC_CTRL_WRITE);
        ufc_dev_done();

	ufc_cs_restore();
}

void ufc_write_word(unsigned short val)
{
	ufc_cs_nand();
	
	OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), UFC_CTRL_FIFO_RST);
	OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), 0x00000000);	
	OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_TX_CMDDATA), val);
	OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), 
			UFC_CTRL_START | UFC_CTRL_WRITE | UFC_CTRL_DATAHALF);
	ufc_dev_done();
	
	ufc_cs_restore();
}

void ufc_write_long(unsigned int val)
{
	ufc_cs_nand();

        OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), UFC_CTRL_FIFO_RST);
	OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), 0x00000000);
        OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_TX_CMDDATA), val);
	OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), 
			UFC_CTRL_START | UFC_CTRL_WRITE | UFC_CTRL_DATACNT);
	ufc_dev_done();

	ufc_cs_restore();
}

/*
 * Don't use these speedup functions in NAND boot image, since the image
 * has to fit into 4kByte.
 */
static void ufc_write_buf(struct mtd_info *mtdinfo, const uint8_t *buf, int len)
{
	uint32_t *p = (uint32_t *) buf;
#if 0
	/* FIXME */
	uint32_t frag_len;
	uint32_t reg_val;
	uint32_t pkt_cnt;

	reg_val = in_le32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_COL_ADDR));

        if (reg_val > mtdinfo->writesize) {
		return;
	} else {
		/* calculate the spare area offset */
		out_le32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_SPARE_OFF), 0x28);
	
		pkt_cnt = len / NAND_PKT_SIZE;
		if (len % NAND_PKT_SIZE)
                        pkt_cnt++;

		reg_val = (NAND_PKT_SIZE | ((pkt_cnt) << UFC_PKTCNT_SHIFT));
		out_le32((u32 *)(UFC_BASE_ADDR + UFC_PKTCNT_PKTSIZE), reg_val);
      
		reg_val = in_le32((u32 *)(UFC_BASE_ADDR + UFC_INT_STAT));
		out_le32((u32 *)(UFC_BASE_ADDR + UFC_INT_STAT), reg_val);
			
		reg_val = in_le32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL));
		out_le32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL),
			 (reg_val | UFC_CTRL_FIFO_RST));
		out_le32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), 
			(reg_val | UFC_CTRL_DATACNT));
				
		reg_val = (UFC_ECC_ON_OFF 
				| UFC_PAGE_2K 
				| UFC_FLASH_WRITE 
				| UFC_FLASH_AUTO);
		out_le32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CMD_REG), reg_val);
	
        	while(pkt_cnt) {
                	if (ufc_chk_rdy(NAND_UFC_WR)) {
				frag_len = NAND_PKT_SIZE;
                        	for (; frag_len > 0; frag_len -= 4) {
					out_le32((u32 *)(UFC_BASE_ADDR + 
							UFC_TX_CMDDATA),*p++);
                        	}
				len -= frag_len;
                        	pkt_cnt--;
				reg_val = in_le32((u32 *)(UFC_BASE_ADDR + 
							UFC_STAT1));
                	} else 
			{
				UFC_DBG("RDY timed out WR \n");
                        	break;
                	}
        	}
		if(ufc_chk_wrrd_done()) {
			UFC_DBG("WR RD Done Int timed out in WRITE\n");
		}
	}
	out_le32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CMD_REG), 0x0);

#else
	unsigned int tmp;

	ufc_cs_nand();

	OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), UFC_CTRL_FIFO_RST);
	OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), 0x00000000);
	tmp = len;
	ufc_dev_done();
	for (tmp = len; tmp > 0; tmp -= 4) {
		OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_TX_CMDDATA), *p++);
        	OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), 
        		UFC_CTRL_START | UFC_CTRL_WRITE |  UFC_CTRL_DATACNT);
		ufc_dev_done();
	}
		
	ufc_cs_restore();
#endif
}

static int ufc_verify_buf(struct mtd_info *mtdinfo, const uint8_t *buf, int len)
{
	u32 *p = (u32 *) buf;
	
	ufc_cs_nand();

	OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), UFC_CTRL_FIFO_RST);
	OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), 0x00000000);
	for (; len > 0; len -= 4) {
		OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), 
			UFC_CTRL_START | UFC_CTRL_READ | UFC_CTRL_DATACNT);
		ufc_dev_done();
		if (*p++ != INLE32((u32 *)(UFC_BASE_ADDR + UFC_RX_CMDDATA))) {
			ufc_cs_restore();			
			return -1;
		}
	}
	
	ufc_cs_restore();			
	return 0;
}
#endif

void ufc_reset(void)
{
#if defined(CONFIG_NAND_SPL)
	u32 val;

#if defined(CONFIG_NAND_SPL_S1)
	val = INBE32((void *)CONFIG_SYS_SCU_BASE + SCU_SRST_ADDR);
	OUTBE32((void *)CONFIG_SYS_SCU_BASE + SCU_SRST_ADDR, val & ~NDFC_MASK);
	OUTBE32((void *)CONFIG_SYS_SCU_BASE + SCU_SRST_ADDR, val);
#else
	apm86xxx_read_scu_reg(SCU_SRST_ADDR, &val);
	apm86xxx_write_scu_reg(SCU_SRST_ADDR, val & ~NDFC_MASK);
	apm86xxx_write_scu_reg(SCU_SRST_ADDR, val);
#endif
#else	
	struct apm86xxx_reset_ctrl reset_ctrl;

	/* Reset UFC */
	memset(&reset_ctrl, 0, sizeof(reset_ctrl));
	reset_ctrl.reg_group = REG_GROUP_SRST_CLKEN;
	reset_ctrl.csr_reset_mask = 0;
	reset_ctrl.reset_mask = NDFC_MASK;
	reset_apm86xxx_block(&reset_ctrl);
#endif
}

void board_nand_select_device(struct nand_chip *nand, int chip)
{
	u32 reg_val;
	u32 count = 0x100;
	
	ufc_reset();

	/* Wait until NAND controller ready */	
	reg_val = INLE32((u32 *) (UFC_BASE_ADDR + UFC_NFLASH_CTRL));
	while ((reg_val & UFC_CTRL_BUSY) && count > 0) {
		reg_val = INLE32((u32 *) (UFC_BASE_ADDR + UFC_NFLASH_CTRL));
		if (count-- == 0)
			return;
	}

	/* Program NAND device type on CSx */	
	reg_val  = INLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL3));
	reg_val &= ~(0x3 << (CONFIG_SYS_NAND_CS * 2));
	reg_val |= UFC_FLASH_TYPE_NAND << (CONFIG_SYS_NAND_CS * 2);
	OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL3), reg_val);
		
	/* Relocate CS0-CS3 to 0xFF00.0000 to 0xFC00.0000 */
	OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_SRAM_CS), 0x0000cdef);	
		
	/* Disable all interrupt and clear all interrupt status */		
	OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_INT_EN), 0x00000000);
	OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_INT_STAT), 0xFFFFFFFF);
}

/**
 * nand_cmd - [DEFAULT] Send command to NAND large page device
 * @mtd:        MTD device structure
 * @command:    the command to be sent
 * @column:     the column address for this command, -1 if none
 * @page_addr:  the page address for this command, -1 if none
 *
 * Send command to NAND device. This is the version for the new large page
 * devices We dont have the separate regions as we have in the small page
 * devices.  We must emulate NAND_CMD_READOOB to keep the code compatible.
 */
void ufc_nand_command(struct mtd_info *mtd, unsigned int command,
                            int column, int page_addr)
{
        register struct nand_chip *chip = mtd->priv;
	unsigned int ctrl = 0;
	struct ufc_state *state;

	ufc_cs_nand();

        /* Emulate NAND_CMD_READOOB */
        if (command == NAND_CMD_READOOB) {
#if defined(CONFIG_NAND_SPL_S2)
                column += CONFIG_SYS_NAND_PAGE_SIZE;
#else /* !defined(CONFIG_NAND_SPL_S2) */
                column += mtd->writesize;
#endif /* !defined(CONFIG_NAND_SPL_S2) */
        }

	/* Reset the FIFO */
	OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), UFC_CTRL_FIFO_RST);
	OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), 0x00000000);
	ctrl |= UFC_CTRL_START;
	ctrl |= (command & 0xFF) | UFC_CTRL_CMD0_PRE;
		
	if ((column != -1 || page_addr != -1) && command != NAND_CMD_READID) {
		/* Serially input address */
		if (column != -1) {
			OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_COL_ADDR), 
				column);
			ctrl |= UFC_CTRL_COL_ADDR_PRE;
		}
		if (page_addr != -1) {
			OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_ROW_ADDR), 
				page_addr);
			ctrl |= UFC_CTRL_ROW_ADDR_PRE;
		}
	}

	state = (struct ufc_state *) chip->priv; 	
	if (state)
		state->shadow_rd_offset = -1;	/* Reset offset state */ 
	
        /*
         * program and erase have their own busy handlers
         * status, sequential in, and deplete1 need no delay
         */
        switch (command) {
        case NAND_CMD_CACHEDPROG:
        case NAND_CMD_PAGEPROG:
        case NAND_CMD_ERASE1:
        case NAND_CMD_SEQIN:
        case NAND_CMD_RNDIN:
        case NAND_CMD_STATUS:
        case NAND_CMD_DEPLETE1:
        case NAND_CMD_RESET:
		break;
	case NAND_CMD_READID:
		ctrl |= UFC_CTRL_CMD1_PRE | UFC_CTRL_ADDR_SPL;			
		break;					
        case NAND_CMD_ERASE2:
		ctrl |= ((command & 0xFF) << UFC_CTRL_CMD1_SHIFT) 
			| UFC_CTRL_CMD1_PRE;
		break;
		
                /*
                 * read error status commands require only a short delay
                 */
        case NAND_CMD_STATUS_ERROR:
        case NAND_CMD_STATUS_ERROR0:
		OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), ctrl);
		ufc_dev_done();
		break;

        case NAND_CMD_RNDOUT:
		ctrl &= ~UFC_CTRL_START;
		ctrl |= ((NAND_CMD_RNDOUTSTART & 0xFF) << UFC_CTRL_CMD1_SHIFT) 
			| UFC_CTRL_CMD1_PRE;
		break;

	case NAND_CMD_READOOB:
		ctrl &= ~0xFF;
		ctrl |= (NAND_CMD_READ0 | UFC_CTRL_CMD0_PRE | UFC_CTRL_START);
		ctrl |= ((NAND_CMD_READSTART & 0xFF) << UFC_CTRL_CMD1_SHIFT) 
			| UFC_CTRL_CMD1_PRE;
		break;

        case NAND_CMD_READ0:
		ctrl |= ((NAND_CMD_READSTART & 0xFF) << UFC_CTRL_CMD1_SHIFT) 
			| UFC_CTRL_CMD1_PRE;
		break;
                /* This applies to read commands */
        default:
                /*
                 * If we don't have access to the busy pin, we apply the given
                 * command delay
                 */
                if (!chip->dev_ready) {
                        udelay(chip->chip_delay);
			ufc_cs_restore(); 
			return;
                }
        }
	OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), ctrl);
	ufc_dev_done();

	if (command == NAND_CMD_STATUS) {
		OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), 
			(UFC_CTRL_START | UFC_CTRL_READ));
		ufc_dev_done();
	}
	
	ufc_cs_restore();
	 
        /* Apply this short delay always to ensure that we do wait tWB in
         * any case on any machine. 
         * FIXME - why 100us 
         */
        udelay(1);
}

#if !defined(CONFIG_NAND_SPL)
static int ufc_wait(struct mtd_info *mtd, struct nand_chip *this)
{
	volatile unsigned long   timeo;
	int state = this->state;
	
	if (state == FL_ERASING)
		timeo = (CONFIG_SYS_HZ * 400) / 1000;
	else
		timeo = (CONFIG_SYS_HZ * 20) / 1000;

	if ((state == FL_ERASING) && (this->options & NAND_IS_AND)) {
		this->cmdfunc(mtd, NAND_CMD_STATUS_MULTI, -1, -1);
	} else {
		this->cmdfunc(mtd, NAND_CMD_STATUS, -1, -1);
	}

#if !defined(CONFIG_NAND_SPL)
	reset_timer();

	while (1) {
		if (get_timer(0) > timeo) {
			printf("Timeout!");
			return 0x01;
		}

		if (this->dev_ready) {
			if (this->dev_ready(mtd))
				break;
		}
	}
#else
	timeo = 0x200000;
	while (timeo--) {
		if (this->dev_ready) {
			if (this->dev_ready(mtd))
				break;
		}
	}	
#endif
	
	ufc_cs_nand();
	
	OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), 
		UFC_CTRL_START | UFC_CTRL_READ);
	/* wait until command done */
	ufc_dev_done();

	timeo = INLE32((u32 *)(UFC_BASE_ADDR + UFC_STAT_REG));
	
	ufc_cs_restore();
		
	return timeo & 0xFF;
}

static int ufc_read_status(struct mtd_info *mtd, unsigned int *val)
{
	*val = INLE32((u32 *)(UFC_BASE_ADDR + UFC_STAT_REG));
	return 0;
}
#endif /* !defined(CONFIG_NAND_SPL) */

int board_nand_init(struct nand_chip *nand)
{
#if !defined(CONFIG_NAND_SPL)
	static int chip = 0;
#else
	int chip = 0;
#endif
	int pagesize;
	u32 val;
	
	/* Select required NAND chip in UFC */
	board_nand_select_device(nand, chip);
	nand->IO_ADDR_R = (void __iomem *)(UFC_BASE_ADDR + UFC_RX_CMDDATA);
	nand->IO_ADDR_W = (void __iomem *)(UFC_BASE_ADDR + UFC_TX_CMDDATA);
	nand->chip_delay = 50;
	nand->read_byte = ufc_read_byte;
	nand->read_buf = ufc_read_buf;
	nand->dev_ready = ufc_dev_ready;
#if !defined(CONFIG_NAND_SPL)
	nand->cmdfunc = ufc_nand_command;
	nand->cmd_ctrl = ufc_hwcontrol;
	nand->read_word = ufc_read_word;
	nand->write_buf  = ufc_write_buf;
	nand->verify_buf = ufc_verify_buf;
	nand->waitfunc = ufc_wait;
	nand->read_status = ufc_read_status;	/* Need for write protect */
	nand->priv = &ufc_chip_state[chip];
#else
	nand->priv = NULL;
	nand->ecc.layout  = NULL;
	nand->select_chip = NULL;
#endif	

	/* Initialize UFC */
#if defined(CONFIG_NAND_SPL_S1)
	INBE32((u32 *)(CONFIG_SYS_SCU_BASE + SCU_NFBOOT0_ADDR));
	val  = 0x00200000; 		/* Enable Auto-Read */
	/* val |= 0x00040000; */	/* Enable UFC HW ECC */
	OUTBE32((u32 *)(CONFIG_SYS_SCU_BASE + SCU_NFBOOT0_ADDR), val);
#else
	apm86xxx_read_scu_reg(SCU_NFBOOT0_ADDR, &val);
	val  = 0x00200000; 		/* Enable Auto-Read */
	/* val |= 0x00040000; */	/* Enable UFC HW ECC */
	apm86xxx_write_scu_reg(SCU_NFBOOT0_ADDR, val);
#endif

	/* Program 2-byte column and 3-byte row adress size for CSx */		
	val  = 0x1 << 2;		/* 2-bytes column */
#if defined(CONFIG_SYS_NAND_5_ADDR_CYCLE)
	val |= 0x2 << 4;		/* 3-bytes row */
#else
	val |= 0x1 << 4;		/* 2-bytes row */
#endif					
	switch (CONFIG_SYS_NAND_CS) {
	case 0:
	default:
		/* FIXME - Need to tune the timing interval */
		OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_TIM_INTER_0), 
			0x00000002);
		OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_TIMING1_INTER_0),
			0x00000004);
		OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_RDYBSYLOW_CNT_0),
			0x00000014);
		OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL2_0), val);
		break;
	case 1:
		/* FIXME - Need to tune the timing interval */
		OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_TIM_INTER_1), 
			0x00000002);
		OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_TIMING1_INTER_1),
			0x00000004);
		OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_RDYBSYLOW_CNT_1),
			0x00000014);
		OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL2_1), val);
		break;
	case 2:
		/* FIXME - Need to tune the timing interval */
		OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_TIM_INTER_2), 
			0x00000002);
		OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_TIMING1_INTER_2),
			0x00000004);
		OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_RDYBSYLOW_CNT_2),
			0x00000014);
		OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL2_2), val);
		break;
	case 3:								
		/* FIXME - Need to tune the timing interval */
		OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_TIM_INTER_3), 
			0x00000002);
		OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_TIMING1_INTER_3),
			0x00000004);
		OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_RDYBSYLOW_CNT_3),
			0x00000014);
		OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL2_3), val);
		break;
	}

	ufc_cs_nand();

	/* Reset the NAND device */
	OUTLE32((u32*)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), 
			UFC_CTRL_START | UFC_CTRL_CMD0_PRE | NAND_CMD_RESET);	
	ufc_dev_ready(NULL);
	
	/* Read the NAND ID to extract page size */
	OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), 
			UFC_CTRL_START | UFC_CTRL_CMD0_PRE | NAND_CMD_READID
			| UFC_CTRL_CMD1_PRE | UFC_CTRL_ADDR_SPL);
	ufc_dev_ready(NULL);
	OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), 
			UFC_CTRL_DATACNT | UFC_CTRL_START| UFC_CTRL_READ);
	ufc_dev_ready(NULL);
	val = INLE32((u32 *)(UFC_BASE_ADDR + UFC_RX_CMDDATA));
	pagesize = 1024 << ((val >> 16) & 0x3);
	OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL), 
			UFC_CTRL_DATACNT | UFC_CTRL_START| UFC_CTRL_READ);
	ufc_dev_ready(NULL);
	val = INLE32((u32 *)(UFC_BASE_ADDR + UFC_RX_CMDDATA));

	/* APM862xxx does not support 4-bit ECC. Therefore, either use the
	   NAND device 4-bit ECC or SW based */	
#if 0
	if ((val & 0x3) == 0x02) {
		/* NAND device support HW 4-bit ECC */
		UFC_DBG("Device support HW ECC\n");
		ufc_enable_device_4bit_ecc(nand, chip);
		nand->ecc.correct = ufc_device_correct_data;
		nand->ecc.hwctl = ufc_device_enable_hwecc;
		nand->ecc.calculate = ufc_device_calculate_ecc;
		nand->ecc.read_page = ufc_read_page_hwecc;
		nand->ecc.write_page = ufc_write_page_hwecc;
		nand->ecc.mode = NAND_ECC_HW_SYNDROME;		
	} else 
#endif
#if defined(CONFIG_NAND_SPL)
		nand->ecc.hwctl = NULL;
		nand->ecc.correct = NULL;
		nand->ecc.calculate = NULL;
# if defined(CONFIG_NAND_SPL_S2)
#  if defined(CONFIG_ECCBCH)
		nand->ecc.mode = NAND_ECC_SOFT;
		nand->ecc.size = CONFIG_SW_BCH_ECC_SIZE;
		nand->ecc.steps = pagesize / nand->ecc.size;
		nand->ecc.bytes = CONFIG_SW_BCH_ECC_BYTES;
		nand->ecc.layout = &nand_swecc_4;
#  else /* !defined(CONFIG_ECCBCH) */
		nand->ecc.mode = NAND_ECC_NONE;
#  endif /* !defined(CONFIG_ECCBCH) */
# else /* !defined(CONFIG_NAND_SPL_S2) */
		nand->ecc.mode = NAND_ECC_NONE;
# endif /* !defined(CONFIG_NAND_SPL_S2) */
#else /* !defined(CONFIG_NAND_SPL) */
# if defined(CONFIG_ECCBCH)
		nand->ecc.hwctl = ufc_enable_swecc;
		nand->ecc.correct = ufc_sw_correct_data;
		nand->ecc.calculate = ufc_calculate_swecc;
		nand->ecc.mode = NAND_ECC_SOFT;
		nand->ecc.size = 512;
		nand->ecc.steps = pagesize / nand->ecc.size;
		nand->ecc.bytes = 7;
		nand->ecc.layout = &nand_swecc_4;
# else /* !defined(CONFIG_ECCBCH) */
		nand->ecc.mode = NAND_ECC_NONE;
# endif /* !defined(CONFIG_ECCBCH) */
#endif /* !defined(CONFIG_NAND_SPL) */
	
	if (nand->ecc.mode == NAND_ECC_SOFT) {
#if !defined(CONFIG_NAND_SPL_S1)
# if defined(CONFIG_ECCBCH)
		/* For non-NAND SPL S1, initialize BCH now */
		ufc_sw_bch_init(nand);
# endif /* defined(CONFIG_ECCBCH) */
#endif /* !defined(CONFIG_NAND_SPL_S1) */
	}
	
	ufc_cs_restore();

	chip++;
	return 0;
}

#if !defined(CONFIG_NAND_SPL)
void ufc_nand_enable(void)
{
	u32 val;
	
	board_nand_select_device(NULL, 0);
	/* Initialize UFC */
	val  = 0x00200000; 		/* Enable Auto-Read */
	/* val |= 0x00040000; */	/* Enable UFC HW ECC */
#if defined(CONFIG_NAND_SPL_S1)
	OUTBE32((u32 *)(CONFIG_SYS_SCU_BASE + SCU_NFBOOT0_ADDR), val);
#else
	apm86xxx_write_scu_reg(SCU_NFBOOT0_ADDR, val);
#endif
	
	/* Relocate CS0-CS3 to 0xFF00.0000 to 0xFC00.0000 */
	OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_SRAM_CS), 0x0000cdef);	
}

void ufc_nand_disable(void)
{
	OUTLE32((void *)(UFC_BASE_ADDR + UFC_NFLASH_CTRL3), UFC_FLASH_TYPE_NOR);
#if defined(CONFIG_NAND_SPL_S1)
	OUTBE32((u32 *)(CONFIG_SYS_SCU_BASE + SCU_NFBOOT0_ADDR), 0xa0e00000);
#else
	apm86xxx_write_scu_reg(SCU_NFBOOT0_ADDR, 0xa0e00000);
#endif
	OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_SRAM_CS), 0xdcba0000);
	OUTLE32((u32 *)(UFC_BASE_ADDR + UFC_FLAG), UFC_FLAG_EN(0));

	OUTLE32((void *)CONFIG_SYS_EBC_BASE + UFC_SRAM_BOOT_CFG, 0x0000FFFF);
	OUTLE32((void *)CONFIG_SYS_EBC_BASE + UFC_SRAM_BANKCFG_0, 0x00000007);
	OUTLE32((void *)CONFIG_SYS_EBC_BASE + UFC_SRAM_CTRL2_0, 0x00725010);
	OUTLE32((void *)CONFIG_SYS_EBC_BASE + UFC_SRAM_CFG_0, 0x0000037e);
	OUTLE32((void *)CONFIG_SYS_EBC_BASE + UFC_SRAM_CTRL1_0, 0x00000040);
}
#endif
