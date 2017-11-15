/*
 * Copyright (C) 2011
 * Anup Patel, Applied Micro <apatel@apm.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
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
#include <malloc.h>
#include <spi.h>
#include <spi_flash.h>

#include "spi_flash_internal.h"

#define MICRON_MAX_PAGE_SIZE			0x1000
#define MICRON_MAX_TRY_COUNT			1000

#define MICRON_CMD_BLOCK_ERASE			0xD8
#define MICRON_CMD_GET_FEATURE			0x0F /* Read Feature */
#define MICRON_CMD_PAGE_READ			0x13 /* Page Read */
#define MICRON_CMD_PROGRAM_EXECUTE		0x10
#define MICRON_CMD_PROGRAM_LOAD			0x02
#define MICRON_CMD_PROGRAM_LOAD_RANDOM_DATA	0x84
#define MICRON_CMD_READ_FROM_CACHE		0x03 /* Output cache data */
#define MICRON_CMD_READ_ID			0x9F /* Read ID */
#define MICRON_CMD_RESET			0xFF /* Reset Device */
#define MICRON_CMD_SET_FEATURE			0x1F /* Write Feature */
#define MICRON_CMD_WRITE_DISABLE		0x04 /* Write Disable */
#define MICRON_CMD_WRITE_ENABLE			0x06 /* Write Enable */

#define MICRON_FEATURE_BLOCKLOCK		0xA0
#define MICRON_FEATURE_OTP			0xB0
#define MICRON_FEATURE_STATUS			0xC0

#define MICRON_ID_MT29F1G01_1			0x11
#define MICRON_ID_MT29F1G01_2			0x12
#define MICRON_ID_MT29F1G01_3			0x13

struct micron_spi_flash_params {
	u8 idcode1;
	u16 page_data_size;
	u16 page_ecc_size;
	u16 pages_per_block;
	u16 nr_blocks;
	const char *name;
};

static const struct micron_spi_flash_params micron_spi_flash_table[] = {
	{
		.idcode1 = MICRON_ID_MT29F1G01_1,
		.page_data_size = 2048,
		.page_ecc_size = 64,
		.pages_per_block = 64,
		.nr_blocks = 1024,
		.name = "MT29F1G01",
	},
	{
		.idcode1 = MICRON_ID_MT29F1G01_2,
		.page_data_size = 2048,
		.page_ecc_size = 64,
		.pages_per_block = 64,
		.nr_blocks = 1024,
		.name = "MT29F1G01",
	},
	{
		.idcode1 = MICRON_ID_MT29F1G01_3,
		.page_data_size = 2048,
		.page_ecc_size = 64,
		.pages_per_block = 64,
		.nr_blocks = 1024,
		.name = "MT29F1G01",
	},
};

struct micron_spi_flash {
	struct spi_flash flash;
	const struct micron_spi_flash_params* params;
};

static inline struct micron_spi_flash *to_micron_spi_flash(struct spi_flash
							     *flash)
{
	return container_of(flash, struct micron_spi_flash, flash);
}

int micron_reset(struct spi_flash *flash)
{
	int ret;
	ret = spi_flash_cmd(flash->spi, MICRON_CMD_RESET, NULL, 0);
	udelay(1000);
	return ret;
}

int micron_get_features(struct spi_flash *flash, u8 reg, u8 *val)
{
	u8 cmd[2];
	cmd[0] = MICRON_CMD_GET_FEATURE;
	cmd[1] = reg;
	return spi_flash_cmd_read(flash->spi, cmd, 2, val, 1);
}

int micron_set_features(struct spi_flash *flash, u8 reg, u8 val)
{
	u8 cmd[3];
	cmd[0] = MICRON_CMD_SET_FEATURE;
	cmd[1] = reg;
	cmd[2] = val;
	return spi_flash_cmd_read(flash->spi, cmd, 3, NULL, 0);
}

int micron_unlock_all(struct spi_flash *flash)
{
	int ret;
	u8 stat;

	ret = micron_get_features(flash, MICRON_FEATURE_BLOCKLOCK, &stat);
	if (ret)
		return ret;

	ret = micron_set_features(flash, MICRON_FEATURE_BLOCKLOCK, 0);
	if (ret) 
		return ret;

	return 0;
}

int micron_wait_for_completion(struct spi_flash *flash)
{
	int ret;
	u8 stat;
	u32 try;

	udelay(100);
	ret = micron_get_features(flash, MICRON_FEATURE_STATUS, &stat);
	if (ret)
		return ret;
	try = 0;
	while ((stat & 0x01) && (try < MICRON_MAX_TRY_COUNT)) {
		ret = micron_get_features(flash, MICRON_FEATURE_STATUS, &stat);
		if (ret)
			return ret;
		if (stat & 0x2c) 
			return -1;
		udelay(100);
		try++;
	}
	if (try == MICRON_MAX_TRY_COUNT)
		return -1;

	return 0;
}

int micron_write_enable(struct spi_flash *flash)
{
	return spi_flash_cmd(flash->spi, MICRON_CMD_WRITE_ENABLE, NULL, 0);
}

int micron_write_disable(struct spi_flash *flash)
{
	return spi_flash_cmd(flash->spi, MICRON_CMD_WRITE_DISABLE, NULL, 0);
}

int micron_read_page(struct spi_flash *flash, u32 pnum, u8 *pbuf)
{
	int ret;
	u8 cmd[4];
	u32 psz;
	struct micron_spi_flash *msf = to_micron_spi_flash(flash);

	psz = msf->params->page_data_size;

	cmd[0] = MICRON_CMD_PAGE_READ;
	cmd[1] = 0x00;
	cmd[2] = pnum >> 8;
	cmd[3] = pnum & 0xFF;

	ret = spi_flash_cmd_read(flash->spi, cmd, 4, NULL, 0);
	if (ret)
		return ret;

	ret = micron_wait_for_completion(flash);
	if (ret)
		return ret;

	cmd[0] = MICRON_CMD_READ_FROM_CACHE;
	cmd[1] = 0x0 >> 8; /* Cache Offset MSB */
	cmd[2] = 0x0 & 0xff; /* Cache Offset LSB */
	cmd[3] = 0x00;

	ret = spi_flash_cmd_read(flash->spi, cmd, 4, pbuf, psz);
	if (ret)
		return ret;
	
	return 0;
}

int micron_write_page(struct spi_flash *flash, u32 pnum, u8 *pbuf)
{
	int ret;
	u8 cmd[4];
	u32 psz;
	struct micron_spi_flash *msf = to_micron_spi_flash(flash);

	psz = msf->params->page_data_size;

	ret = micron_write_enable(flash);
	if (ret) 
		return ret;

	cmd[0] = MICRON_CMD_PROGRAM_LOAD;
	cmd[1] = 0x0;
	cmd[2] = 0x0;

	ret = spi_flash_cmd_write(flash->spi, cmd, 3, pbuf, psz);
	if (ret)
		return ret;

	cmd[0] = MICRON_CMD_PROGRAM_EXECUTE;
	cmd[1] = 0x00;
	cmd[2] = (pnum >> 8) & 0xff;
	cmd[3] = pnum & 0xff;
	ret = spi_flash_cmd_write(flash->spi, cmd, 4, NULL, 0);
	if (ret)
		return ret;

	ret = micron_wait_for_completion(flash);
	if (ret)
		return ret;

	return 0;
}

int micron_erase_block(struct spi_flash *flash, u32 blknum)
{
	int ret;
	u8 cmd[4];
	u32 psz, blksz;
	struct micron_spi_flash *msf = to_micron_spi_flash(flash);

	psz = msf->params->page_data_size;
	blksz = psz * msf->params->pages_per_block;

	ret = micron_write_enable(flash);
	if (ret) 
		return ret;

	cmd[0] = MICRON_CMD_BLOCK_ERASE;
	cmd[1] = 0x00;
	cmd[2] = (((blknum * blksz) / psz) >> 8) & 0xFF;
	cmd[3] = ((blknum * blksz) / psz) & 0xFF;

	ret = spi_flash_cmd_write(flash->spi, cmd, 4, NULL, 0);
	if (ret) 
		return ret;

	ret = micron_wait_for_completion(flash);
	if (ret)
		return ret;

	return 0;
}

int micron_read(struct spi_flash *flash, u32 offset, size_t len, void *buf)
{
	struct micron_spi_flash *msf = to_micron_spi_flash(flash);
	int ret;
	u8 *dest = NULL;
	u32 ite=0, page_sz, buf_bytes, buf_off, dest_off=0;

	ret = spi_claim_bus(flash->spi);
	if (ret) {
		debug("SF: Unable to claim SPI bus\n");
		return ret;
	}

	page_sz = msf->params->page_data_size;
	dest = buf;
	while(len) {
		if (ite == 0 && 0 < (offset % page_sz)) {
			buf_off = (offset % page_sz);
			buf_bytes = (len > (page_sz - (offset % page_sz))) ? 
				    page_sz - (offset % page_sz) : len;
		} else {
			buf_off = 0;
			buf_bytes = (len < page_sz) ? 
				    len : page_sz;
		}
		if(buf_bytes % page_sz == 0) {
			if ((ret = micron_read_page(flash, 
						    (offset / page_sz) + ite,
						    &dest[dest_off])))
				break;
		} else {
			u8 tbuf[MICRON_MAX_PAGE_SIZE];
			if ((ret = micron_read_page(flash, 
						    (offset / page_sz) + ite,
						    tbuf)))
				break;
			memcpy(&dest[dest_off], &tbuf[buf_off], buf_bytes);
		}
		len -= buf_bytes;
		dest_off += buf_bytes;
		ite += ((buf_off + buf_bytes) / page_sz);
	}

	spi_release_bus(flash->spi);

	return ret;
}

int micron_write(struct spi_flash *flash,
			 u32 offset, size_t len, const void *buf)
{
	struct micron_spi_flash *msf = to_micron_spi_flash(flash);
	u8 *src = NULL;
	int ret = 0;
	u32 ite = 0, page_sz, buf_bytes, buf_off, src_off = 0;

	ret = spi_claim_bus(flash->spi);
	if (ret) {
		debug("SF: Unable to claim SPI bus\n");
		return ret;
	}

	page_sz = msf->params->page_data_size;
	src = (u8 *)buf;
	while(len) {
		if (ite == 0 && 0 < (offset % page_sz)) {
			buf_off = (offset % page_sz);
			buf_bytes = (len > (page_sz - (offset % page_sz))) ? 
				    page_sz - (offset % page_sz) : len;
		} else {
			buf_off = 0;
			buf_bytes = (len < page_sz) ? len : page_sz;
		}
		if(buf_bytes % page_sz == 0) {
			if ((ret = micron_write_page(flash, 
						     (offset / page_sz) + ite, 
						     &src[src_off]))) 
				break;
		} else {
			u8 tbuf[MICRON_MAX_PAGE_SIZE];
			if ((ret = micron_read_page(flash, 
						    (offset / page_sz) + ite, 
						    tbuf))) 
				break;
			memcpy(&tbuf[buf_off], &src[src_off], buf_bytes);
			if ((ret = micron_write_page(flash, 
						     (offset / page_sz) + ite, 
						     tbuf))) 
				break;
		}
		len -= buf_bytes;
		src_off += buf_bytes;
		ite += ((buf_off + buf_bytes) / page_sz);
	}

	spi_release_bus(flash->spi);

	return ret;
}

int micron_erase(struct spi_flash *flash, u32 offset, size_t len)
{
	struct micron_spi_flash *msf = to_micron_spi_flash(flash);
	u32 blksz;
	size_t actual;
	int ret;

	/*
	 * This function currently uses block erase only.
	 * probably speed things up by using bulk erase
	 * when possible.
	 */
	blksz = msf->params->page_data_size * msf->params->pages_per_block;

	if (offset % blksz || len % blksz) {
		debug("SF: Erase offset/length not multiple of sector size\n");
		return -1;
	}

	len /= blksz;

	ret = spi_claim_bus(flash->spi);
	if (ret) {
		debug("SF: Unable to claim SPI bus\n");
		return ret;
	}

	ret = micron_unlock_all(flash);
	if (ret) {
		debug("SF: Fail to unlock chip\n");
		spi_release_bus(flash->spi);
		return ret;
	}

	ret = 0;
	for (actual = 0; actual < len; actual++) {
		ret = micron_erase_block(flash, 
					 (offset / blksz) + actual);
		if (ret) {
			debug("SF: Erasing %d block failed\n", 
					(offset / blksz) + actual);
			break;
		}
	}

	spi_release_bus(flash->spi);

	return ret;
}

struct spi_flash *spi_flash_probe_micron(struct spi_slave *spi, u8 * idcode)
{
	const struct micron_spi_flash_params *params;
	struct micron_spi_flash *msf;
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(micron_spi_flash_table); i++) {
		params = &micron_spi_flash_table[i];
		if (params->idcode1 == idcode[1]) {
			break;
		}
	}

	if (i == ARRAY_SIZE(micron_spi_flash_table)) {
		debug("SF: Unsupported Micron ID %02x\n", idcode[1]);
		return NULL;
	}

	msf = malloc(sizeof(struct micron_spi_flash));
	if (!msf) {
		debug("SF: Failed to allocate memory\n");
		return NULL;
	}

	msf->params = params;
	msf->flash.spi = spi;
	msf->flash.name = params->name;

	msf->flash.write = micron_write;
	msf->flash.erase = micron_erase;
	msf->flash.read = micron_read;
	msf->flash.size = params->page_data_size
			* params->pages_per_block
	    		* params->nr_blocks;

	micron_reset(&msf->flash);

	debug("SF: Detected %s with page size %u, total %u bytes\n",
	      params->name, params->page_size, stm->flash.size);

	return &msf->flash;
}

