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
#include "asm/iproc/reg_utils.h"
#include "asm/iproc/iproc_common.h"

DECLARE_GLOBAL_DATA_PTR;

extern int bcmiproc_eth_register(u8 dev_num);
extern void iproc_save_shmoo_values(void);
extern int linux_usbh_init(void);

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
#if !defined(CONFIG_SRAM_RELOC) && !defined(CONFIG_KATANA2_EMULATION)
	ddr_init2();
#endif
	sku_id = (reg32_read((volatile uint32_t *)CMIC_DEV_REV_ID)) & 0x0000ffff;
	if(sku_id == 0xb040) {
		/* Ranger SVK board */
		gd->ram_size = ENDURO3_SVK_SDRAM_SIZE - CONFIG_PHYS_SDRAM_RSVD_SIZE;
	}
	else {
		/* default Katana2 */
		gd->ram_size = CONFIG_PHYS_SDRAM_1_SIZE - CONFIG_PHYS_SDRAM_RSVD_SIZE;
	}

    return 0;
}

int board_early_init_f (void)
{
	int status = 0;
	uint32_t val;
#ifdef CONFIG_KATANA2_EMULATION
        /* Set the UART clock to APB clock so we get baudrate 115200. 50000000/16/27 =  115740 */
		val = reg32_read((volatile uint32_t *)ChipcommonA_CoreCtrl);
		val &= 0xFFFFFFF7;
		reg32_write((volatile uint32_t *)ChipcommonA_CoreCtrl, val); /* Disable UART clock */
		val |= 0x9;
		reg32_write((volatile uint32_t *)ChipcommonA_CoreCtrl, val); /* Enable and set it to APB clock(bit 0) */
#else
	iproc_config_armpll(1000);
#endif
	return(status);
}

int board_late_init (void) 
{
	int status = 0;

	iproc_clk_enum();

	disable_interrupts();

#if defined(CONFIG_RUN_DDR_SHMOO2) && defined(CONFIG_SHMOO_REUSE)
	/* Save DDR PHY parameters into flash if Shmoo was performed */
	iproc_save_shmoo_values();
#endif

#ifndef CONFIG_KATANA2_EMULATION
	linux_usbh_init();
#endif
	return status;
}

int board_eth_init(bd_t *bis)
{
	int rc = -1;
#ifdef CONFIG_BCMIPROC_ETH
#ifndef CONFIG_SRAM_RELOC
#ifndef CONFIG_KATANA2_EMULATION
	printf("Registering eth\n");
	rc = bcmiproc_eth_register(0);
#endif
#endif
#endif
	return rc;
}
