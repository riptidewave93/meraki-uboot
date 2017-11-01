/*
 * Copyright (C) 2013, Broadcom Corporation. All Rights Reserved.
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */


#ifndef __SOCREGS_H
#define __SOCREGS_H


#define  CCA_CHIPID											0x18000000
#define  CCA_CHIPID_BASE                                    0x000
#define  CCB_GP_DATA_IN										0x18001000
#define  USB30_BASE											0x18023000	
#define  SDIO_EMMC_SDXC_SYSADDR								0x18020000
#define  CCB_PWM_CTL										0x18002000
#define  CCB_TIM0_TIM_TMR1_LOAD								0x18005000
#define  CCB_TIM1_TIM_TMR1_LOAD								0x18006000
#define  CCB_SMBUS_START									0x18009000
#define  CCB_GP_AUX_SEL_BASE								0x028
#define  QSPI_MSPI_SPCR0_LSB								0x18029200
#define  QSPI_MSPI_DISABLE_FLUSH_GEN						0x18029384
#define  QSPI_BSPI_REGS_REV_ID								0x18029000
#define  QSPI_BSPI_REGS_BSPI_PIO_DATA						0x1802904c
#define  QSPI_RAF_START_ADDR								0x18029100
#define  QSPI_RAF_CURR_ADDR									0x18029120
#define  QSPI_RAF_INTERRUPT_LR_FULLNESS_REACHED				0x180293a0
#define  QSPI_MSPI_INTERRUPT_MSPI_HALT_SET_TRANSACTION_DONE	0x180293b8
#define  GMAC0_DEVCTL										0x18024000
#define  GMAC1_DEVCTL										0x18025000
#define  GMAC2_DEVCTL										0x18026000
#define  GMAC3_DEVCTL										0x18027000
#define  CCA_GPIO_EVT_BASE									0x078
#define  CCA_GPIO_INPUT										0x18000060
#define  CCA_GPIO_INPUT_BASE								0x060
#define  CCB_GP_INT_CLR_BASE								0x024
#define  CCA_GPIO_EVTINT_MASK_BASE							0x07c
#define  CCB_GP_INT_MSK_BASE								0x018
#define  CCA_GPIOINT_MASK_BASE								0x074
#define  CCA_GPIO_EVT_INT_POLARITY_BASE						0x084
#define  CCA_GPIO_INT_POLARITY_BASE							0x070
#define  CCA_INT_MASK_BASE									0x024
#define  CCB_GP_INT_TYPE_BASE								0x00c
#define  CCB_GP_INT_DE_BASE									0x010
#define  CCB_GP_INT_EDGE_BASE								0x014
#define  CCA_INT_STS_BASE									0x020
#define  CCB_GP_INT_MSTAT_BASE								0x020
#define  CCB_GP_PAD_RES_BASE								0x034
#define  CCB_GP_RES_EN_BASE									0x038
#define  CCB_MII_MGMT_CTL									0x18003000
#define  NAND_NAND_FLASH_REV								0x18028000
#define  NAND_DIRECT_READ_RD_MISS							0x18028f00
#define  CCB_PWM_PRESCALE_BASE								0x024
#define  CCB_PWM_PERIOD_COUNT0_BASE							0x004
#define  CCB_PWM_PERIOD_COUNT1_BASE							0x00c
#define  CCB_PWM_DUTY_HI_COUNT2_BASE						0x018
#define  CCB_PWM_PERIOD_COUNT3_BASE							0x01c
#define  CCB_PWM_DUTY_HI_COUNT0_BASE						0x008
#define  CCB_PWM_DUTY_HI_COUNT1_BASE						0x010
#define  CCB_PWM_DUTY_HI_COUNT2_BASE						0x018
#define  CCB_PWM_DUTY_HI_COUNT3_BASE						0x020
#define  CCB_PWM_CTL_BASE									0x000
#define  CCB_PWM_PERIOD_COUNT2_BASE							0x014
#define  CCB_RNG_CTRL										0x18004000
#define IHOST_PROC_CLK_PLLARMA								0x19000c00
#define IHOST_PROC_CLK_PLLARMB								0x19000c04
#define IHOST_PROC_CLK_PLLARMA__pllarm_pdiv_R				24
#define IHOST_PROC_CLK_POLICY_FREQ							0x19000008
#define CRU_RESET_OFFSET									0x1800c184
#define ChipcommonA_CoreCtrl								0x18000008
#define ChipcommonA_CoreCtrl__UARTClkOvr					0
#define APBX_IDM_IDM_IO_CONTROL_DIRECT						0x18121408
#define APBX_IDM_IDM_IO_CONTROL_DIRECT__UARTClkSel			17
#define ChipcommonA_ClkDiv									0x180000a4
#define ChipcommonA_ClkDiv__UartClkDiv_R					0
#define ChipcommonA_ClkDiv__UartClkDiv_WIDTH				8
#define ChipcommonA_ChipID									0x18000000
#define IHOST_PROC_CLK_PLLARMA__pllarm_ndiv_int_R			8
#define IHOST_PROC_CLK_PLLARMA__pllarm_soft_post_resetb		1
#define IHOST_PROC_CLK_PLLARMA__pllarm_soft_resetb			0
#define IHOST_PROC_CLK_CORE0_CLKGATE						0x19000200
#define IHOST_PROC_CLK_CORE1_CLKGATE						0x19000204
#define IHOST_PROC_CLK_ARM_SWITCH_CLKGATE					0x19000210
#define IHOST_PROC_CLK_ARM_PERIPH_CLKGATE					0x19000300
#define IHOST_PROC_CLK_APB0_CLKGATE							0x19000400
#define IHOST_PROC_CLK_WR_ACCESS							0x19000000
#define IHOST_PROC_CLK_POLICY_FREQ							0x19000008
#define IHOST_PROC_CLK_POLICY_FREQ__priv_access_mode		31
#define IHOST_PROC_CLK_POLICY_FREQ__policy3_freq_R			24
#define IHOST_PROC_CLK_POLICY_FREQ__policy2_freq_R			16
#define IHOST_PROC_CLK_POLICY_FREQ__policy1_freq_R			8
#define IHOST_PROC_CLK_POLICY_CTL							0x1900000c
#define IHOST_PROC_CLK_POLICY_CTL__GO						0
#define IHOST_PROC_CLK_POLICY_CTL__GO_AC					1
#define IHOST_PROC_CLK_PLLARMB__pllarm_ndiv_frac_R			0
#define IHOST_PROC_CLK_PLLARMB__pllarm_ndiv_frac_WIDTH		20
#define IHOST_PROC_CLK_PLLARMA__pllarm_lock					28
#define IHOST_PROC_CLK_POLICY_FREQ__policy0_freq_R			0
#define CRU_CLKSET_KEY_OFFSET								0x1800c180
#define CRU_LCPLL_CONTROL0_OFFSET							0x1800c100
#define CRU_LCPLL_CONTROL1_OFFSET							0x1800c104
#define CRU_LCPLL_CONTROL2_OFFSET							0x1800c108
#define DDR_DENALI_CTL_00									0x18010000
#define DDR_PHY_CONTROL_REGS_REVISION						0x18010800
#define DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE_RD_EN			0x18010a00
#define DDR_DENALI_CTL_89									0x18010164
#define DDR_DENALI_CTL_43									0x180100ac
#define DDR_S1_IDM_RESET_CONTROL							0x18108800
#define DDR_S2_IDM_RESET_CONTROL							0x18109800
#define ROM_S0_IDM_IO_STATUS								0x1810d500
#define PCU_MDIO_MGT_OFFSET									0x1800c000
#define PCU_MDIO_CMD_OFFSET									0x1800c004
#define DDR_S1_IDM_IO_CONTROL_DIRECT						0x18108408
#define DDR_S1_IDM_IO_STATUS								0x18108500
#define PCU_1V8_1V5_VREGCNTL_OFFSET							0x1800c02c
#define DDR_PHY_CONTROL_REGS_PLL_DIVIDERS					0x1801081c
#define DDR_PHY_WORD_LANE_0_READ_DATA_DLY					0x18010b60
#define NAND_nand_flash_UNCORR_ERROR_COUNT					0x180280fc
#define NAND_nand_flash_INTFC_STATUS						0x18028014
#define NAND_ro_ctlr_ready									0x18028f10
#define NAND_nand_flash_CMD_ADDRESS							0x1802800c
#define NAND_nand_flash_CMD_EXT_ADDRESS						0x18028008
#define NAND_nand_flash_INIT_STATUS							0x18028144
#define NAND_nand_flash_FLASH_DEVICE_ID						0x18028194
#define NAND_nand_flash_ONFI_STATUS							0x18028148
#define NAND_nand_flash_CONFIG_CS1							0x18028064
#define NAND_nand_flash_CONFIG_CS0							0x18028054
#define CRU_STRAPS_CONTROL_OFFSET							0x1800c2a0
#define NAND_nand_flash_ACC_CONTROL_CS1						0x18028060
#define NAND_nand_flash_ACC_CONTROL_CS0						0x18028050
#define NAND_IDM_IDM_IO_CONTROL_DIRECT						0x1811a408
#define NAND_nand_flash_FLASH_CACHE0						0x18028400
#define NAND_nand_flash_ECC_UNC_ADDR						0x18028118
#define NAND_nand_flash_CS_NAND_SELECT						0x18028018
#define NAND_nand_flash_SPARE_AREA_READ_OFS_0				0x18028200
#define NAND_nand_flash_SPARE_AREA_WRITE_OFS_0				0x18028280
#define NAND_nand_flash_FLASH_DEVICE_ID_EXT					0x18028198
#define NAND_nand_flash_CMD_START							0x18028004
#define NAND_IDM_IDM_RESET_CONTROL							0x1811a800
#define CRU_OTP_CPU_CONTROL0_OFFSET							0x1800c240
#define CRU_OTP_CPU_CONTROL1_OFFSET							0x1800c244
#define CRU_OTP_CPU_ADDRESS_OFFSET							0x1800c24c
#define CRU_OTP_CPU_BITSEL_OFFSET							0x1800c250
#define CRU_OTP_CPU_RDATA_OFFSET							0x1800c25c
#define CRU_OTP_CPU_STATUS_OFFSET							0x1800c260
#define CRU_OTP_CPU_CONFIG_OFFSET							0x1800c258
#define AMAC_IDM0_IDM_RESET_CONTROL							0x18110800
#define AMAC_IDM1_IDM_RESET_CONTROL							0x18111800
#define AMAC_IDM2_IDM_RESET_CONTROL							0x18112800
#define AMAC_IDM3_IDM_RESET_CONTROL							0x18113800
#define ChipcommonB_MII_Management_Control					0x18003000
#define ChipcommonB_MII_Management_Command_Data				0x18003004
#define ChipcommonB_MII_Management_Control__BSY				8
#define ChipcommonB_MII_Management_Control__PRE				7
#define ChipcommonB_MII_Management_Control__EXT				9
#define ChipcommonB_MII_Management_Command_Data__SB_R		30
#define ChipcommonB_MII_Management_Command_Data__PA_R		23
#define ChipcommonB_MII_Management_Command_Data__RA_R		18
#define ChipcommonB_MII_Management_Command_Data__TA_R		16
#define ChipcommonB_MII_Management_Command_Data__OP_R		28
#define GMAC0_DEVCONTROL									0x18024000
#define GMAC1_DEVCONTROL									0x18025000
#define GMAC2_DEVCONTROL									0x18026000
#define QSPI_bspi_registers_REVISION_ID						0x18029000
#define CRU_control											0x1800b000
#define IHOST_SCU_INVALIDATE_ALL							0x1902000c
#define IHOST_SCU_CONTROL									0x19020000
#define IHOST_L2C_CACHE_ID									0x19022000
#define AXIIC_REMAP											0x1a000000
#define SDROM_REMAP_SELECT_ENABLE							0x18130e00
#define SDROM_REMAP_SELECT									0x18130e04

#endif /* __SOCREGS_H */
