/*
 * Copyright 2010 Freescale Semiconductor, Inc.
 * Authors: Srikanth Srinivasan <srikanth.srinivasan@freescale.com>
 *          Timur Tabi <timur@freescale.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 */

#include <common.h>
#include <asm/mmu.h>
#include <asm/immap_85xx.h>
#include <asm/processor.h>
#include <asm/fsl_ddr_sdram.h>
#include <asm/fsl_ddr_dimm_params.h>
#include <asm/io.h>
#include <asm/fsl_law.h>

struct board_specific_parameters {
	u32 n_ranks;
	u32 datarate_mhz_high;
	u32 clk_adjust;		/* Range: 0-8 */
	u32 cpo;		/* Range: 2-31 */
	u32 write_data_delay;	/* Range: 0-6 */
	u32 force_2T;
};

/*
 * This table contains all valid speeds we want to override with board
 * specific parameters. datarate_mhz_high values need to be in ascending order
 * for each n_ranks group.
 */
static const struct board_specific_parameters dimm0[] = {
	/*
	 * memory controller 0
	 *   num|  hi|  clk| cpo|wrdata|2T
	 * ranks| mhz|adjst|    | delay|
	 */
	{1,  549,    5,  31,     3, 0},
	{1,  850,    5,  31,     5, 0},
	{2,  549,    5,  31,     3, 0},
	{2,  850,    5,  31,     5, 0},
	{}
};

/* Fixed sdram init -- doesn't use serial presence detect. */
phys_size_t fixed_sdram(void)
{
	sys_info_t sysinfo;
	char buf[32];
	size_t ddr_size;
	fsl_ddr_cfg_regs_t ddr_cfg_regs = {
		.cs[0].bnds = CONFIG_SYS_DDR_CS0_BNDS,
		.cs[0].config = CONFIG_SYS_DDR_CS0_CONFIG,
		.cs[0].config_2 = CONFIG_SYS_DDR_CS0_CONFIG_2,
#if CONFIG_CHIP_SELECTS_PER_CTRL > 1
		.cs[1].bnds = CONFIG_SYS_DDR_CS1_BNDS,
		.cs[1].config = CONFIG_SYS_DDR_CS1_CONFIG,
		.cs[1].config_2 = CONFIG_SYS_DDR_CS1_CONFIG_2,
#endif
		.timing_cfg_3 = CONFIG_SYS_DDR_TIMING_3,
		.timing_cfg_0 = CONFIG_SYS_DDR_TIMING_0,
		.timing_cfg_1 = CONFIG_SYS_DDR_TIMING_1,
		.timing_cfg_2 = CONFIG_SYS_DDR_TIMING_2,
		.ddr_sdram_cfg = CONFIG_SYS_DDR_CONTROL,
		.ddr_sdram_cfg_2 = CONFIG_SYS_DDR_CONTROL_2,
		.ddr_sdram_mode = CONFIG_SYS_DDR_MODE_1,
		.ddr_sdram_mode_2 = CONFIG_SYS_DDR_MODE_2,
		.ddr_sdram_md_cntl = CONFIG_SYS_DDR_MODE_CONTROL,
		.ddr_sdram_interval = CONFIG_SYS_DDR_INTERVAL,
		.ddr_data_init = CONFIG_SYS_DDR_DATA_INIT,
		.ddr_sdram_clk_cntl = CONFIG_SYS_DDR_CLK_CTRL,
		.ddr_init_addr = CONFIG_SYS_DDR_INIT_ADDR,
		.ddr_init_ext_addr = CONFIG_SYS_DDR_INIT_EXT_ADDR,
		.timing_cfg_4 = CONFIG_SYS_DDR_TIMING_4,
		.timing_cfg_5 = CONFIG_SYS_DDR_TIMING_5,
		.ddr_zq_cntl = CONFIG_SYS_DDR_ZQ_CONTROL,
		.ddr_wrlvl_cntl = CONFIG_SYS_DDR_WRLVL_CONTROL,
		.ddr_sr_cntr = CONFIG_SYS_DDR_SR_CNTR,
		.ddr_sdram_rcw_1 = CONFIG_SYS_DDR_RCW_1,
		.ddr_sdram_rcw_2 = CONFIG_SYS_DDR_RCW_2
	};

	get_sys_info(&sysinfo);
	printf("Configuring DDR for %s MT/s data rate\n",
			strmhz(buf, sysinfo.freqDDRBus));

	ddr_size = CONFIG_SYS_SDRAM_SIZE * 1024 * 1024;

	fsl_ddr_set_memctl_regs(&ddr_cfg_regs, 0);

	if (set_ddr_laws(CONFIG_SYS_DDR_SDRAM_BASE,
				ddr_size, LAW_TRGT_IF_DDR_1) < 0) {
		printf("ERROR setting Local Access Windows for DDR\n");
		return 0;
	};

	return ddr_size;
}

void fsl_ddr_board_options(memctl_options_t *popts,
				dimm_params_t *pdimm,
				unsigned int ctrl_num)
{
	int i;
	popts->clk_adjust = 6;
	popts->cpo_override = 0x1f;
	popts->write_data_delay = 2;
	popts->half_strength_driver_enable = 1;
	/* Write leveling override */
	popts->wrlvl_en = 1;
	popts->wrlvl_override = 1;
	popts->wrlvl_sample = 0xf;
	popts->wrlvl_start = 0x8;
	popts->trwt_override = 1;
	popts->trwt = 0;

	if (pdimm->primary_sdram_width == 64)
		popts->data_bus_width = 0;
	else if (pdimm->primary_sdram_width == 32)
		popts->data_bus_width = 1;
	else
		printf("Error in DDR bus width configuration!\n");

	for (i = 0; i < CONFIG_CHIP_SELECTS_PER_CTRL; i++) {
		popts->cs_local_opts[i].odt_rd_cfg = FSL_DDR_ODT_NEVER;
		popts->cs_local_opts[i].odt_wr_cfg = FSL_DDR_ODT_CS;
	}
}
