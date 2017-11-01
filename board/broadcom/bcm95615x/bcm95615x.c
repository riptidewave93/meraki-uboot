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
#include <common.h>
#include <asm/io.h>
#include <asm/mach-types.h>
#include <config.h>
#include <asm/arch/iproc.h>
#include <asm/system.h>
#include "asm/iproc/iproc_common.h"
#include <asm/arch/iproc_regs.h>
#include "asm/arch/socregs.h"
#include "asm/arch/reg_utils.h"

DECLARE_GLOBAL_DATA_PTR;

extern int bcmiproc_eth_register(u8 dev_num);
extern void iproc_save_shmoo_values(void);

void init_timeouts(void) {

#define DDR_IDM_ERROR_LOG_CONTROL_VAL                       0x33a //0x330

#define W_REG(o, a, v) *a=(unsigned int)v
#define uint32 unsigned int

    W_REG(osh, (uint32 *)IHOST_S0_IDM_ERROR_LOG_CONTROL, DDR_IDM_ERROR_LOG_CONTROL_VAL);
    W_REG(osh, (uint32 *)IHOST_S1_IDM_ERROR_LOG_CONTROL, DDR_IDM_ERROR_LOG_CONTROL_VAL);

    W_REG(osh, (uint32 *)DDR_S1_IDM_ERROR_LOG_CONTROL, DDR_IDM_ERROR_LOG_CONTROL_VAL);
    W_REG(osh, (uint32 *)DDR_S2_IDM_ERROR_LOG_CONTROL, DDR_IDM_ERROR_LOG_CONTROL_VAL);

	W_REG(osh, (uint32 *)AXI_PCIE_S0_IDM_IDM_ERROR_LOG_CONTROL, DDR_IDM_ERROR_LOG_CONTROL_VAL);

	W_REG(osh, (uint32 *)CMICD_S0_IDM_IDM_ERROR_LOG_CONTROL, DDR_IDM_ERROR_LOG_CONTROL_VAL);
	W_REG(osh, (uint32 *)APBY_S0_IDM_IDM_ERROR_LOG_CONTROL, DDR_IDM_ERROR_LOG_CONTROL_VAL);
	W_REG(osh, (uint32 *)ROM_S0_IDM_ERROR_LOG_CONTROL, DDR_IDM_ERROR_LOG_CONTROL_VAL);
	W_REG(osh, (uint32 *)NAND_IDM_IDM_ERROR_LOG_CONTROL, DDR_IDM_ERROR_LOG_CONTROL_VAL);
	W_REG(osh, (uint32 *)QSPI_IDM_IDM_ERROR_LOG_CONTROL, DDR_IDM_ERROR_LOG_CONTROL_VAL);
	W_REG(osh, (uint32 *)A9JTAG_S0_IDM_IDM_ERROR_LOG_CONTROL, DDR_IDM_ERROR_LOG_CONTROL_VAL);
	W_REG(osh, (uint32 *)SRAM_S0_IDM_ERROR_LOG_CONTROL, DDR_IDM_ERROR_LOG_CONTROL_VAL);
	W_REG(osh, (uint32 *)APBZ_S0_IDM_IDM_ERROR_LOG_CONTROL, DDR_IDM_ERROR_LOG_CONTROL_VAL);
	W_REG(osh, (uint32 *)AXIIC_DS_3_IDM_ERROR_LOG_CONTROL, DDR_IDM_ERROR_LOG_CONTROL_VAL);
	W_REG(osh, (uint32 *)APBW_IDM_IDM_ERROR_LOG_CONTROL, DDR_IDM_ERROR_LOG_CONTROL_VAL);
	W_REG(osh, (uint32 *)APBX_IDM_IDM_ERROR_LOG_CONTROL, DDR_IDM_ERROR_LOG_CONTROL_VAL);
	W_REG(osh, (uint32 *)AXIIC_DS_0_IDM_ERROR_LOG_CONTROL, DDR_IDM_ERROR_LOG_CONTROL_VAL);

}

void init_axitrace(void)
{

	reg32_write((volatile uint32_t *)IHOST_AXITRACE_M1_ATM_CONFIG, 0x5551);
	reg32_write((volatile uint32_t *)IHOST_AXITRACE_M1_ATM_CMD, 2);
	reg32_write((volatile uint32_t *)IHOST_AXITRACE_M0_ATM_CONFIG, 0x5551);
	reg32_write((volatile uint32_t *)IHOST_AXITRACE_M0_ATM_CMD, 2);
	reg32_write((volatile uint32_t *)IHOST_AXITRACE_ACP_ATM_CONFIG, 0x5551);
	reg32_write((volatile uint32_t *)IHOST_AXITRACE_ACP_ATM_CMD, 2);
}
/*****************************************
 * board_init -early hardware init
 *****************************************/
int board_init (void)
{
    gd->bd->bi_arch_number = CONFIG_MACH_TYPE;      /* board id for linux */
    gd->bd->bi_boot_params = LINUX_BOOT_PARAM_ADDR; /* adress of boot parameters */

    return 0;
}

/*****************************************************************
 * misc_init_r - miscellaneous platform dependent initializations
 ******************************************************************/
int misc_init_r (void)
{
    return(0);
}

/**********************************************
 * dram_init - sets uboots idea of sdram size
 **********************************************/
int dram_init (void)
{
	uint32_t sku_id;
#ifndef CONFIG_HURRICANE2_EMULATION
	bench_screen_test1();

	ddr_init2();
#endif
	sku_id = (reg32_read((volatile uint32_t *)CMIC_DEV_REV_ID)) & 0x0000ffff;
	if(sku_id == 0xb151  || sku_id == 0x8393 || sku_id == 0x8394) {
		/* DeerHound SVK */
		gd->ram_size = DEERHOUND_SVK_SDRAM_SIZE - CONFIG_PHYS_SDRAM_RSVD_SIZE;
	}
	else {
		gd->ram_size = CONFIG_PHYS_SDRAM_1_SIZE - CONFIG_PHYS_SDRAM_RSVD_SIZE;
	}

    return 0;
}

int board_early_init_f (void)
{
	int status = 0;
	uint32_t sku_id, val;

#ifdef CONFIG_HURRICANE2_EMULATION
		/* Set the UART clock to APB clock so we get baudrate 115200. 50000000/16/27 =  115740 */
		val = reg32_read((volatile uint32_t *)ChipcommonA_CoreCtrl);
		val &= 0xFFFFFFF7;
		reg32_write((volatile uint32_t *)ChipcommonA_CoreCtrl, val); /* Disable UART clock */
		val |= 0x9;  
		reg32_write((volatile uint32_t *)ChipcommonA_CoreCtrl, val); /* Enable and set it to APB clock(bit 0) */
#else

	sku_id = (reg32_read((volatile uint32_t *)CMIC_DEV_REV_ID)) & 0x0000ffff;
	if(sku_id == 0x8342 || sku_id == 0x8343 || sku_id == 0x8344 || 
	   sku_id == 0x8346 || sku_id == 0x8347 || sku_id == 0x8393 || 
	   sku_id == 0x8394) {
		   /* Wolfhound/Deerhound */
		/* Configure ARM PLL to 400MHz */
		iproc_config_armpll(400);

		status = iproc_config_genpll(4); /* change AXI clock to 200MHz */
	}
	else {
		/* default SKU = b15x, ARM Clock is 1GHz, AXI clock is 400MHz, APB clock is 100MHz */
		/* Configure ARM PLL to 1GHz */
		iproc_config_armpll(1000);

		//status = iproc_config_genpll(3); /* 0: 400, 2: 285, 3: 250, 4: 200, 5: 100 */
	}
		/* Set the UART clock to APB clock */
		/* APB clock is axiclk/4 */
		val = reg32_read((volatile uint32_t *)ChipcommonA_CoreCtrl);
		val &= 0xFFFFFFF7;
		reg32_write((volatile uint32_t *)ChipcommonA_CoreCtrl, val); /* Disable UART clock */
		val |= 0x9;  
		reg32_write((volatile uint32_t *)ChipcommonA_CoreCtrl, val); /* Enable and set it to APB clock(bit 0) */

#endif
	return(status);
}
int board_late_init (void) 
{
	extern ulong mmu_table_addr;    /* MMU on flash fix: 16KB aligned */
	int status = 0;

	printf("Unlocking L2 Cache ...");
	l2cc_unlock();
	printf("Done\n");
    
	/* MMU on flash fix: 16KB aligned */
	asm volatile ("mcr p15, 0, %0, c2, c0, 0"::"r"(mmu_table_addr)); /*update TTBR0 */
	asm volatile ("mcr p15, 0, r1, c8, c7, 0");  /*invalidate TLB*/
	asm volatile ("mcr p15, 0, r1, c7, c10, 4"); /* DSB */
	asm volatile ("mcr p15, 0, r0, c7, c5, 4"); /* ISB */		

	iproc_clk_enum();

	disable_interrupts();
#ifndef CONFIG_HURRICANE2_EMULATION
	init_timeouts();
	init_axitrace();
#endif

#if defined(CONFIG_RUN_DDR_SHMOO2) && defined(CONFIG_SHMOO_REUSE)
	/* Save DDR PHY parameters into flash if Shmoo was performed */
	iproc_save_shmoo_values();
#endif
	return status;
}

int board_eth_init(bd_t *bis)
{
	int rc = -1;
#ifdef CONFIG_BCMIPROC_ETH
#ifndef CONFIG_HURRICANE2_EMULATION
	printf("Registering eth\n");
	rc = bcmiproc_eth_register(0);
#endif
#endif
	return rc;
}
