/*
 * Copyright (c) 2015-2017, The Linux Foundation. All rights reserved.

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef ___QCA_COMMON_H_
#define ___QCA_COMMON_H_
#include <asm/u-boot.h>
#include <asm/arch-qca-common/smem.h>

#ifdef CONFIG_ARCH_IPQ807x
#include <asm/arch-ipq807x/clk.h>
#endif

#ifdef CONFIG_ARCH_IPQ40xx
#include <asm/arch-ipq40xx/clk.h>
#endif


#ifdef CONFIG_ARCH_IPQ806x
#include <asm/arch-ipq806x/clock.h>
#endif

struct ipq_i2c_platdata {
	int type;
};

typedef struct {
	uint base;
	uint clk_mode;
	struct mmc *mmc;
	int dev_num;
} qca_mmc;

/* Reserved-memory node names and register value*/
typedef struct {
	const char *nodename;
	u32 val[2];
} add_node_t;

int qca_mmc_init(bd_t *, qca_mmc *);
void board_mmc_deinit(void);

void set_flash_secondary_type(qca_smem_flash_info_t *);

struct dumpinfo_t{
	char name[16]; /* use only file name in 8.3 format */
	uint32_t start;
	uint32_t size;
	int is_aligned_access; /* non zero represent 4 byte access */
};
extern struct dumpinfo_t dumpinfo[];
extern int dump_entries;

#define MSM_SDC1_BASE		0x7824000
#define MMC_IDENTIFY_MODE	0
#define MMC_DATA_TRANSFER_MODE	1
#define MMC_MODE_HC		0x800

#endif  /*  __QCA_COMMON_H_ */
