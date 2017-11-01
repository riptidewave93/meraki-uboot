/*
 * Copyright 2012 Freescale Semiconductor, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/fsl_pamu.h>

void construct_pamu_addr_table(struct pamu_addr_tbl *tbl, int *num_entries)
{
	int i  = 0;

	tbl->start_addr[i] = (uint64_t)CONFIG_SYS_SDRAM_BASE ;
	tbl->size[i] = (phys_size_t)(CONFIG_SYS_SDRAM_SIZE / 2) * 1024 * 1024;
								/* 2GB DDR */
	tbl->end_addr[i] = tbl->start_addr[i] +  tbl->size[i] - 1;

	i++;
	tbl->start_addr[i] = (uint64_t)CONFIG_SYS_FLASH_BASE_PHYS;
	tbl->size[i] = 256 * 1024 * 1024; /* 256MB flash */
	tbl->end_addr[i] = tbl->start_addr[i] +  tbl->size[i] - 1;

	i++;
#ifdef DEBUG
	int j;
	printf("address\t\t\tsize\n");
	for (j = 0; j < i ; j++)
		printf("%llx \t\t\t%llx\n",  tbl->start_addr[j],  tbl->size[j]);
#endif

	*num_entries = i;
}

int sec_config_pamu_table(uint32_t liodn)
{
	struct pamu_addr_tbl tbl;
	int num_entries = 0;
	int ret = 0;

	construct_pamu_addr_table(&tbl, &num_entries);
	ret = config_pamu(&tbl, num_entries, liodn);

	return ret;
}
