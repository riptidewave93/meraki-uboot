/**
 * AppliedMicro SoC 86xxx SCU Interface Header file
 *
 * Copyright (c) 2010 Applied Micro Circuits Corporation.
 * All rights reserved. Prodyut Hazarika <phazarika@apm.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 *
 */
#ifndef __ASM_APM_SCU_H_
#define __ASM_APM_SCU_H_

#include <asm/io.h>

#define APM_MPA_REG_OFFSET		0x800
#define APM_GLBL_DIAG_OFFSET		0x7000

/* Reset Control Structure for APM86xxx IP block                    */
/* reg_group: indicate whether to use SCU_SRST or SCU_SRST1 reg    */
/* clken_mask: Mask to enable clock in SCU_CLKEN/CLKEN1 reg: 0 skip*/
/* csr_reset_mask: Mask for SCU_CSR_SRST/SRST1 reg: 0 means NO CSR */
/* reset_mask: Mask for SCU_SRST/SRST1 reg: 0 skip                 */
/* mem_rdy_mask: Mask for SCU_MRDY reg: 0 means not MEM_RDY bit    */
/* ram_shutdown_addr: Module's RAM shutdown addr: NULL to skip     */
#define REG_GROUP_SRST_CLKEN		0
#define REG_GROUP_SRST1_CLKEN1		1
#define TOGGLE_RESET_FLAG		0
#define ASSERT_RESET_FLAG		1
#define MEM_RDY_TIMEOUT_COUNT		10000

#ifndef __ASSEMBLY__

struct apm86xxx_reset_ctrl {
	int	reset_type;
	int	reg_group;
	u32	clken_mask;
	u32	csr_reset_mask;
	u32	reset_mask;
	u32	mem_rdy_mask;
	u32	__iomem *ram_shutdown_addr;
};

/* Reset APM 86xxx block - User must pass proper values in reset_ctrl */
/* To disable APM 86xxx block, set reset_type == ASSERT_RESET_FLAG    */
/* Example: to reset Ethernet Port1 and Port 2, the values should be */
/* reg_group = REG_GROUP_SRST_CLKEN; */
/* clken_mask = ENET1_F1_MASK | ENET0_F1_MASK; in SCU_CLKEN reg: apm_ipp_csr.h */
/* csr_reset_mask = ENET0_F3_MASK | ENET1_F2_MASK; in SCU_CSR_SRST reg         */
/* reset_mask = ENET0_MASK | ENET1_MASK; in SCU_SRST reg: apm_ipp_csr.h        */
/* mem_rdy_mask = ENET0_F2_MASK; in SCU_MRDY reg: apm_ipp_csr.h                */
/* ram_shutdown_addr = Virt addr of ENET_CFG_MEM_RAM_SHUTDOWN reg:mb_enet_csr.h*/
int reset_apm86xxx_block(struct apm86xxx_reset_ctrl *reset_ctrl);

struct apm86xxx_pll_ctrl {
	u16	clkf_val;
	u16	clkod_val;
	u16	clkr_val;
	u16	bwadj_val;
};

/* To reconfigure PLL, user must shutdown PLL and pass new values */
/* If PLL already configured, enable_xxx_pll returns safely       */
/* Safe to call enable_xxx_pll multiple times                     */

/* Enable APM 86xxx DDR PLL - User must pass proper PLL values in eth_pll */
int enable_ddr_pll(struct apm86xxx_pll_ctrl *ddr_pll);
/* Enable APM 86xxx Ethernet PLL - User must pass proper PLL values in eth_pll */
int enable_eth_pll(struct apm86xxx_pll_ctrl *eth_pll);
/* Enable APM 86xxx LCD PLL - User must pass proper PLL values in eth_pll */
int enable_lcd_pll(struct apm86xxx_pll_ctrl *lcd_pll);

/* Shutdown DDR PLL */
void disable_ddr_pll(void);
/* Shutdown Ethernet PLL */
void disable_eth_pll(void);
/* Shutdown LCD PLL */
void disable_lcd_pll(void);

/* WRITE SCU register - returns 0 if successful      */
/* offfset: SCU reg offset defined in apm_ipp_csr.h */
/* Use with caution: meant for debugging only       */
int apm86xxx_write_scu_reg(u32 offset, u32 val);

/* Read SCU register - returns 0 if successful      */
/* offfset: SCU reg offset defined in apm_ipp_csr.h */
/* val is valid only if return value is 0           */
int apm86xxx_read_scu_reg(u32 offset, u32 *val);

#define APM86xxx_REFCLK_MHZ		25
enum
{
	APM86xxx_CPU_PLL = 0x0,
	APM86xxx_CPU0,
	APM86xxx_CPU1,
	APM86xxx_PLB,
	APM86xxx_TRACE_PPC,
	APM86xxx_SOC_PLL,
	APM86xxx_AXI,
	APM86xxx_AHB,
	APM86xxx_IAHB,
	APM86xxx_APB,
	APM86xxx_PCIE_REFCLK,
	APM86xxx_CRYPTO,
	APM86xxx_EBUS,
	APM86xxx_SDIO,
	APM86xxx_DDR_PLL,
	APM86xxx_DDR,
	APM86xxx_MAX
};

/* Get APM SoC BUS Frequency in Hz         */
/* freq is valid only if return value is 0 */
int apm86xxx_get_freq(int id, u32 *freq);

/* Get APM Bus Divisor */
u32 apm86xxx_get_divisor(int id);
/* Read iPP message                            */
/* ipp data is valid only if return value is 0 */
int apm86xxx_read_ipp_msg(u32 *data);

/* Write iPP message */
int apm86xxx_write_ipp_msg(u32 data, u32 param);

/* Load iPP Firmware from fwload_addr */
int apm86xxx_ipp_fwload(u32 fwload_addr);

/* Switch CPM to local/global mode for cpuX */
int apm86xxx_switch_cpm_mode(int cpu, int mode_flag);

/* Set apm86xxx to low powermodes eg. doze, wait, nap ... */
int apm86xxx_set_powerstate(char *mode);

/* Write to CPM Register offset X with value */
int apm86xxx_write_cpmreg(u8 offset, u8 value); 

/* Read from CPM Register offset X          */
/* Value is valid only if return value is 0 */
int apm86xxx_read_cpmreg(u8 offset, u8 *value);

/* Get iPP Mode (rom, runtime or external) along with versionId         */
/* Return IPP_ROM_MODE_MASK (rom mode), IPP_RUNTIME_MODE_MASK (runtime) */
/* or IPP_EXTBOOT_MODE_MASK (external boot) mode                        */
u32 apm86xxx_ipp_getmode(int *major, int *minor);

/* Default CPU divisor after coldboot */
#define DEFAULT_APM86xxx_CPU_DIVISOR	2

/* Get CPU divisor - valid only if return value is 0 */
int apm86xxx_get_cpu_divisor(int id, u32 *divisor);
int apm86xxx_get_cpu_divisor2(int id, u32 *divisor);

/* Default PLB divisor after coldboot */
#define DEFAULT_APM86xxx_PLB_DIVISOR	4

/* Get PLB divisor - valid only if return value is 0 */
int apm86xxx_get_plb_divisor(u32 *divisor);
int apm86xxx_get_plb2x_divisor2(u32 *divisor);

/* Set CPU and PLB divisor */
int apm86xxx_set_cpu_divisor2(int id, u32 divisor);
int apm86xxx_set_plb2x_divisor2(u32 divisor);

#endif /*__ASSEMBLY__ */

#endif /* __ASM_APM_SCU_H_ */ 
