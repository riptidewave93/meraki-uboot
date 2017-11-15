/*
 * (C) Copyright 2010 Applied Micro Circuit Corp
 * Feng Kan <fkan@apm.com>

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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */


#include <common.h>
#include <command.h>
#include <linux/stddef.h>
#include <asm/byteorder.h>
#include <flash.h>

#ifdef CONFIG_SAVE_FLASH_INFO
DECLARE_GLOBAL_DATA_PTR;


static ulong finfo_addr = CONFIG_FINFO_ADDR;
static ulong finfo_end_addr = CONFIG_FINFO_ADDR + CONFIG_FINFO_SECT_SIZE - 1;

extern flash_info_t flash_info[];	/* FLASH chips info */


int getfi(void)
{
	int crc;

	crc = crc32(0, (void *)finfo_addr, sizeof(flash_info_t));

	if (crc != *(int *)(finfo_end_addr - 3)) {
		debug("Unmatch CRC for flash information %x:%x\n", crc, *(int *)(finfo_end_addr - 3));
		return flash_init();
	}
	memcpy(&flash_info, (char *)(finfo_addr), sizeof(flash_info_t));
	debug("flash size is %x\n", flash_info[0].size);
	printf("*");
	return flash_info[0].size;
}

int safi(void)
{
	int rc = 1;
	int crc;

	printf("Saving flash information to persistent storage\n");
	crc = crc32(0, (unsigned char *)&flash_info, sizeof(flash_info_t));

	puts ("Unprotect Flash...");
	if (flash_sect_protect (0, finfo_addr, finfo_end_addr)) {
		goto Done;
	}

	puts ("Erasing Flash...");
	if (flash_sect_erase (finfo_addr, finfo_end_addr)) {
		goto Done;
	}

	puts ("Writing to Flash... ");
	if ((rc = flash_write((char *)flash_info,
			(ulong)finfo_addr,
			sizeof(flash_info_t))) ||
	    (rc = flash_write((char *)&crc,
			(ulong)(finfo_end_addr - 3),
			sizeof(int))))
	{
		flash_perror (rc);
		goto Done;
	}

	puts ("done\n");
	(void) flash_sect_protect (1, (ulong)finfo_addr, finfo_end_addr);

Done:
	return 1;
}

int do_safi (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

	printf("Saving Flash Info...\n");

	return(safi() ? 1 : 0);
}

U_BOOT_CMD(
	safi, 1, 0,	do_safi,
	"save flash info to persistent storage",
	""
);

#endif
