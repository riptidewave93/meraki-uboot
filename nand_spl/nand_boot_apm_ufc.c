/*
 * NAND boot for Applied Micro Universal Flash Controller (UFC)
 *
 * Copyright (c) 2011 Applied Micro 
 * Author: Tushar Jagad, Applied Micro, tjagad@apm.com.
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
 */

#include <common.h>
#include <nand.h>
#include <asm/io.h>
#include <asm/fsl_lbc.h>
#include <asm/processor.h>
#include <linux/mtd/nand.h>

#if defined(CONFIG_NAND_SPL_S2)
#if defined(CONFIG_SYS_NAND_ECCPOS)
unsigned int nand_ecc_pos[] = CONFIG_SYS_NAND_ECCPOS;
#else /* !defined(CONFIG_SYS_NAND_ECCPOS) */
unsigned int *nand_ecc_pos;
#endif /* !defined(CONFIG_SYS_NAND_ECCPOS) */
#endif /* defined(CONFIG_NAND_SPL_S2) */

extern void ufc_nand_command(struct mtd_info *mtd, unsigned int command,
			     int column, int page_addr);
extern void ufc_read_buf(struct mtd_info *mtdinfo, uint8_t *buf, int len);

#define udelay(x) { \
	volatile u32 delay; \
	for (delay = 0; delay < 100; delay++); \
	}

#if defined(CONFIG_NAND_SPL_S1)
void nand_read_page(struct mtd_info *mtd, int block, int page,
		    uint8_t *buf)
{
	uint8_t *p = buf;
	page += (block * CONFIG_SYS_NAND_PAGE_COUNT);
	ufc_nand_command(mtd, NAND_CMD_READ0, 0x0, page);
	ufc_read_buf(mtd, p, CONFIG_SYS_NAND_PAGE_SIZE);
}

void nand_load(struct mtd_info *mtd, unsigned int offs,
	       unsigned int uboot_size, uchar *dst)
{
	unsigned int block, lastblock;
	unsigned int page, lastpage;

	/*
	 * offs has to be aligned to a page address!
	 */
	block = offs / CONFIG_SYS_NAND_BLOCK_SIZE;
	lastblock = (offs + uboot_size - 1) / CONFIG_SYS_NAND_BLOCK_SIZE;
	page = (offs % CONFIG_SYS_NAND_BLOCK_SIZE) / CONFIG_SYS_NAND_PAGE_SIZE;
	lastpage = page + uboot_size / CONFIG_SYS_NAND_PAGE_SIZE;

	while (block <= lastblock) {
		while (page < lastpage) {
			nand_read_page(mtd, block, page, dst);
			dst += CONFIG_SYS_NAND_PAGE_SIZE;
			page++;
		}
		page = 0;
		block++;
		udelay(1);
	}
}
#elif defined(CONFIG_NAND_SPL_S2) /* !defined(CONFIG_NAND_SPL_S1) */

static int nand_command(struct mtd_info *mtd, int block, int page, int offs, u8 cmd)
{
	int page_addr = page + block * CONFIG_SYS_NAND_PAGE_COUNT;

	ufc_nand_command(mtd, cmd, offs, page_addr);

	return 0;
}

#if defined(CONFIG_ECCBCH)
extern int ufc_sw_correct_data(struct mtd_info *mtd,
			       unsigned char *buf,
			       unsigned char *read_ecc,
			       unsigned char *calc_ecc);

extern int ufc_calculate_swecc(struct mtd_info *mtd,
			       const unsigned char *buf,
			       unsigned char *code);

void copy_to_ecc_code(uchar *ecc_code, uchar *oob_data)
{
	int i;
	int eccbytes;
	int eccsteps;

	eccbytes = CONFIG_SYS_NAND_ECCBYTES;
	eccsteps = CONFIG_SYS_NAND_ECCSTEPS;

	for (i = 0; i < eccbytes * eccsteps; i++)
		ecc_code[i] = oob_data[nand_ecc_pos[i]];
}
#endif /* defined(CONFIG_ECCBCH) */

void nand_read_page(struct mtd_info *mtd, int block, int page,
		    uint8_t *buf)
{
#if defined(CONFIG_ECCBCH)
	u_char *ecc_calc;
	u_char *ecc_code;
	u_char *oob_data;
	int i;
	int eccsize = 0;
	int eccbytes = 0;
	int eccsteps = 0;
	uint8_t *p = buf;
	int stat;

	eccsize = CONFIG_SYS_NAND_ECCSIZE;
	eccbytes = CONFIG_SYS_NAND_ECCBYTES;
	eccsteps = CONFIG_SYS_NAND_ECCSTEPS;

	nand_command(mtd, block, page, 0, NAND_CMD_READ0);

	/* No malloc available for now, just use some temporary locations
	 * in SDRAM
	 */
	ecc_calc = (u_char *)(CONFIG_SYS_SDRAM_BASE + 0x80000);
	ecc_code = ecc_calc + 0x100;
	oob_data = ecc_calc + 0x200;

	memset(ecc_calc, 0, 0x100);
	memset(ecc_code, 0, 0x100);
	memset(oob_data, 0, 0x100);

	for (i = 0; eccsteps; eccsteps--, i += eccbytes, p += eccsize) {
		ufc_read_buf(mtd, p, eccsize);
		ufc_calculate_swecc(mtd, p, &ecc_calc[i]);
	}

	nand_command(mtd, block, page, 0, NAND_CMD_READOOB);
	ufc_read_buf(mtd, oob_data, CONFIG_SYS_NAND_OOBSIZE);

	/* Pick the ECC bytes out of the oob data */
	copy_to_ecc_code(ecc_code, oob_data);

	eccsteps = CONFIG_SYS_NAND_ECCSTEPS;
	p = buf;

	for (i = 0 ; eccsteps; eccsteps--, i += eccbytes, p += eccsize) {
		/* No chance to do something with the possible error message
		 * from correct_data(). We just hope that all possible errors
		 * are corrected by this routine.
		 */
		stat = ufc_sw_correct_data(mtd, p, &ecc_code[i], &ecc_calc[i]);
	}
#else /* !defined(CONFIG_ECCBCH) */
	uint8_t *p = buf;
	page += (block * CONFIG_SYS_NAND_PAGE_COUNT);
	ufc_nand_command(mtd, NAND_CMD_READ0, 0x0, page);
	ufc_read_buf(mtd, p, CONFIG_SYS_NAND_PAGE_SIZE);
#endif /* !defined(CONFIG_ECCBCH) */
}

__attribute__((noinline)) int nand_is_bad_block(struct mtd_info *mtd, int block)
{
	int val = 0;
	uint8_t *c_ptr = (uint8_t *)&val;

	nand_command(mtd, block, 0, CONFIG_SYS_NAND_BAD_BLOCK_POS, NAND_CMD_READOOB);

	/*
	 * Read one byte
	 */
#if 0
	if (this->read_byte)
		val = this->read_byte(mtd);
	else
		val = readb(this->IO_ADDR_R);
	if (val != 0xff)
		return 1;

	return 0;
#else
	ufc_read_buf(mtd, c_ptr, 4);
	if (*c_ptr == 0xFF)
		return 0;

	return 1;
#endif
}

void nand_load(struct mtd_info *mtd, unsigned int offs,
	       unsigned int uboot_size, uchar *dst)
{
	unsigned int block, lastblock;
	unsigned int page, lastpage;

	/*
	 * offs has to be aligned to a page address!
	 */
	block = offs / CONFIG_SYS_NAND_BLOCK_SIZE;
	lastblock = (offs + uboot_size - 1) / CONFIG_SYS_NAND_BLOCK_SIZE;
	page = (offs % CONFIG_SYS_NAND_BLOCK_SIZE) / CONFIG_SYS_NAND_PAGE_SIZE;
	lastpage = page + uboot_size / CONFIG_SYS_NAND_PAGE_SIZE;

	while (block <= lastblock) {
		if (!nand_is_bad_block(mtd, block)) {
			while (page < CONFIG_SYS_NAND_PAGE_COUNT) {
				nand_read_page(mtd, block, page, dst);
				dst += CONFIG_SYS_NAND_PAGE_SIZE;
				page++;
			}
			page = 0;
		} else {
			lastblock++;
		}

		block++;

		udelay(1);
	}
}
#endif /* defined(CONFIG_NAND_SPL_S2) */

void nand_boot(void)
{
	struct nand_chip nand_chip;
	nand_info_t nand_info;
	__attribute__((noreturn)) void (*uboot)(void);

	/*
	 * Init board specific nand support
	 */
	nand_info.priv = &nand_chip;
	nand_chip.IO_ADDR_R = nand_chip.IO_ADDR_W = (void  __iomem *)CONFIG_SYS_NAND_BASE;

	/*
	 * Load U-Boot image from NAND into RAM
	 */
#ifdef CONFIG_NAND_SPL_S1
        int count = 0;
        do {
            if (count > 1) {
                while(1) {
                    udelay(100);
                    mtspr(SPRN_DBCR0, mfspr(SPRN_DBCR0) | 0x30000000); //do reset
                }
            }

            (*((uint32_t*)CONFIG_SYS_NAND_U_BOOT_DST)) = 0xdeadbeef;
#endif
            board_nand_init(&nand_chip);
            nand_load(&nand_info, CONFIG_SYS_NAND_U_BOOT_OFFS,
                      CONFIG_SYS_NAND_U_BOOT_SIZE,
                      (uchar *)CONFIG_SYS_NAND_U_BOOT_DST);
#ifdef CONFIG_NAND_SPL_S1
#define U_BOOT_MAGIC 0x38000000
            count++;
        } while (*((uint32_t*)CONFIG_SYS_NAND_U_BOOT_DST) != U_BOOT_MAGIC);
#endif

#if !defined(CONFIG_NAND_SPL_S1) && defined(CONFIG_NAND_ENV_DST)
	nand_load(&nand_info, CONFIG_ENV_OFFSET, CONFIG_ENV_SIZE,
		  (uchar *)CONFIG_NAND_ENV_DST);
# ifdef CONFIG_ENV_OFFSET_REDUND
	nand_load(&nand_info, CONFIG_ENV_OFFSET_REDUND, CONFIG_ENV_SIZE,
		  (uchar *)CONFIG_NAND_ENV_DST + CONFIG_ENV_SIZE);
# endif /* CONFIG_ENV_OFFSET_REDUND */
#endif /* !defined(CONFIG_NAND_SPL_S1) && defined(CONFIG_NAND_ENV_DST) */

	/*
	 * Jump to U-Boot image
	 */
#ifdef CONFIG_NAND_SPL_S2
	uboot = (void *)CONFIG_SYS_NAND_U_BOOT_START + _START_OFFSET;
	(*uboot)();
#endif
}
