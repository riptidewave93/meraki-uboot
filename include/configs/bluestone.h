/*
 * (C) Copyright 2009
 * Hoan Tran, AppliedMicro, hotran@amcc.com
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

/************************************************************************
 * bluestone.h - configuration for Bluestone (APM821x1)
 ***********************************************************************/
#ifndef __CONFIG_H
#define __CONFIG_H

/*-----------------------------------------------------------------------
 * High Level Configuration Options
 *----------------------------------------------------------------------*/
/*
 * This config file is used for Bluestone (APM821x1)
 *
 */
#define CONFIG_APM82181		1	/* Specific APM821x1		*/
#define CONFIG_HOSTNAME		bluestone

#define CONFIG_440		1
#define CONFIG_464FP	1	/* New 464 core with FP */
#define CONFIG_4xx		1	/* ... PPC4xx family */

#define CONFIG_PCA9670
#define CONFIG_SYS_I2C_PCA9670_ADDR	0x27
#define CONFIG_CMD_PCA9670
#define CONFIG_MERAKI_BUCKMINISTER  1   //Alex:20110315
#define NOT_USE_PCIE
/*
 * Include common defines/options for all AMCC eval boards
 */
#define	CONFIG_USE_TTY	ttyS1	//Alex:20101125
#include "amcc-common.h"
#define CONFIG_SYS_CLK_FREQ	50000000

#define CONFIG_BOARD_EARLY_INIT_F	1	/* Call board_early_init_f */
#define CONFIG_BOARD_EARLY_INIT_R	1	/* Call board_early_init_r */
#define CONFIG_MISC_INIT_R		1	/* Call misc_init_r */
#define CONFIG_BOARD_TYPES		1	/* support board types */
#define	CONFIG_LAST_STAGE_INIT		1

/* PLL stuff */
#define CONFIG_CLK_800_200_200_100      1
#define CONFIG_CLK_1000_166_250_83      2
#define CONFIG_CLK_1000_250_250_83      3
#define CONFIG_CLK_1200_200_300_100     4
#define CONFIG_CLK_1200_200_200_100     5
#define CONFIG_CLK_667_166_166_83		6

#if !defined(CONFIG_NAND_U_BOOT)
#define CONFIG_PLL_RECONFIG_ADDR   0xFFF60000
#define CONFIG_PLL_RECONFIG_SIZE   CONFIG_ENV_SECT_SIZE          /* size of one complete sector */

#define CONFIG_SYS_PLL_RECONFIG \
        (*(u32*)CONFIG_PLL_RECONFIG_ADDR == 1 ? 1 : \
        (*(u32*)CONFIG_PLL_RECONFIG_ADDR == 2 ? 2 : \
        (*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 3 : \
        (*(u32*)CONFIG_PLL_RECONFIG_ADDR == 4 ? 4 : \
        (*(u32*)CONFIG_PLL_RECONFIG_ADDR == 5 ? 5 : \
	(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 6 : 1))))))
#else
// 0xE3004000 = CONFIG_SYS_NAND_BOOT_SPL_DST + 0x1000 -> SPL with DDR Calibration
// 0x2FFC  = ((12 << 10) - 4 ) -> is the end of SPL image
#define CONFIG_PLL_RECONFIG_OFF		0x2FFC
#define CONFIG_PLL_RECONFIG_ADDR   	(0xE3004000 + CONFIG_PLL_RECONFIG_OFF)
#define CONFIG_PLL_RECONFIG_RAM_R	0x200000
#define NAND_ERASE_BLOCK_SIZE		0x20000

#define CONFIG_SYS_PLL_RECONFIG \
        (*(u32*)CONFIG_PLL_RECONFIG_ADDR == 1 ? 1 : \
        (*(u32*)CONFIG_PLL_RECONFIG_ADDR == 2 ? 2 : \
        (*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 3 : \
        (*(u32*)CONFIG_PLL_RECONFIG_ADDR == 4 ? 4 : \
        (*(u32*)CONFIG_PLL_RECONFIG_ADDR == 5 ? 5 : \
        (*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 6 : 1))))))
#endif

/* 
 * Undef to test different PLL confguration using I2C EEPROM 
 * or during SRAM boot
 */
#if defined(CONFIG_SRAM_U_BOOT)
#undef CONFIG_SYS_PLL_RECONFIG
#endif
/*-----------------------------------------------------------------------
 * Base addresses -- Note these are effective addresses where the
 * actual resources get mapped (not physical addresses)
 *----------------------------------------------------------------------*/
#define CONFIG_SYS_PCIE_MEMBASE	0xb0000000	/* mapped PCIe memory	*/
#define CONFIG_SYS_PCIE_MEMSIZE	0x08000000	/* smallest incr for PCIe port */
#define CONFIG_SYS_PCIE_BASE	0xc4000000	/* PCIe UTL regs */

#define CONFIG_SYS_PCIE0_CFGBASE	0xc0000000
#define CONFIG_SYS_PCIE0_XCFGBASE	0xc3000000

#define	CONFIG_SYS_PCIE0_UTLBASE	0xc08010000ULL	/* 36bit physical addr	*/

/* base address of inbound PCIe window */
#define CONFIG_SYS_PCIE_INBOUND_BASE	0x000000000ULL	/* 36bit physical addr	*/

/* EBC stuff */
#define CONFIG_SYS_EXTSRAM_BASE		0xE1000000	/* 2MB */
#define CONFIG_SYS_FLASH_BASE		0xFFF00000	/* later mapped to this addr */
#define CONFIG_SYS_FLASH_SIZE		(1 << 20)	/* 1MB usable */

#define CONFIG_SYS_NAND0_ADDR		0xE4000000
#define CONFIG_SYS_NAND1_ADDR		0xE5000000
#define CONFIG_SYS_BOOT_BASE_ADDR	0xFF000000	/* EBC Boot Space: 0xFF000000 */

#define CONFIG_SYS_OCM_BASE		0xE3000000	/* OCM: 32k		*/
#define CONFIG_SYS_SRAM_BASE		0xE8000000	/* SRAM: 256k		*/
#define CONFIG_SYS_LOCAL_CONF_REGS	0xEF000000

#define CONFIG_SYS_PERIPHERAL_BASE	0xEF600000	/* internal peripherals */

#define CONFIG_SYS_AHB_BASE		0xE2000000	/* internal AHB peripherals	*/

/*-----------------------------------------------------------------------
 * Initial RAM & stack pointer (placed in OCM)
 *----------------------------------------------------------------------*/
#define CONFIG_SYS_INIT_RAM_ADDR	CONFIG_SYS_OCM_BASE	/* OCM			*/
#define CONFIG_SYS_INIT_RAM_END		(4 << 10)
#define CONFIG_SYS_GBL_DATA_SIZE	256		/* num bytes initial data */
#define CONFIG_SYS_GBL_DATA_OFFSET	(CONFIG_SYS_INIT_RAM_END - CONFIG_SYS_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_OFFSET	CONFIG_SYS_GBL_DATA_OFFSET

/*-----------------------------------------------------------------------
 * Serial Port
 *----------------------------------------------------------------------*/
#define CONFIG_SERIAL_MULTI
#define CONFIG_UART1_CONSOLE	/* define this if you want console on UART1 */
/*-----------------------------------------------------------------------
 * Environment
 *----------------------------------------------------------------------*/
/*
 * Define here the location of the environment variables (FLASH).
 */
#if !defined(CONFIG_NAND_U_BOOT) && !defined(CONFIG_NAND_SPL) \
	&& !defined(CONFIG_SRAM_U_BOOT)
#define	CONFIG_ENV_IS_IN_FLASH	1	/* use FLASH for environment vars */
#define CONFIG_SYS_NOR_CS		0	/* NOR chip connected to CSx */
#define CONFIG_SYS_NAND_CS		1	/* NAND chip connected to CSx */
//#define	CONFIG_ENV_IS_IN_NAND	1	/* use NAND for environment vars  */
#else
#define	CONFIG_ENV_IS_IN_NAND	1	/* use NAND for environment vars  */
#define CONFIG_SYS_NOR_CS		2	/* NOR chip connected to CSx */
#define CONFIG_SYS_NAND_CS		0	/* NAND chip connected to CSx */
#endif

/*
 * IPL (Initial Program Loader, integrated inside CPU)
 * Will load first 4k from NAND (SPL) into cache and execute it from there.
 *
 * SPL (Secondary Program Loader)
 * Will load special U-Boot version (NUB) from NAND and execute it. This SPL
 * has to fit into 4kByte. It sets up the CPU and configures the SDRAM
 * controller and the NAND controller so that the special U-Boot image can be
 * loaded from NAND to SDRAM.
 *
 * NUB (NAND U-Boot)
 * This NAND U-Boot (NUB) is a special U-Boot version which can be started
 * from RAM. Therefore it mustn't (re-)configure the SDRAM controller.
 *
 * On 440EPx the SPL is copied to SDRAM before the NAND controller is
 * set up. While still running from cache, I experienced problems accessing
 * the NAND controller.	sr - 2006-08-25
 *
 * This is the first official implementation of booting from 2k page sized
 * NAND devices (e.g. Micron 29F2G08AA 256Mbit * 8)
 */
#define CONFIG_SYS_NAND_BOOT_SPL_SRC	0xfffff000	/* SPL location		      */
#define CONFIG_SYS_NAND_BOOT_SPL_SIZE	(4 << 10)	/* SPL size		      */
#define CONFIG_SYS_NAND_BOOT_SPL_DST	(CONFIG_SYS_OCM_BASE + (12 << 10)) /* Copy SPL here  */
#define CONFIG_SYS_NAND_U_BOOT_DST	0x01000000	/* Load NUB to this addr      */
#define CONFIG_SYS_NAND_U_BOOT_START	CONFIG_SYS_NAND_U_BOOT_DST	/* Start NUB from     */
							/*   this addr	      */
#define CONFIG_SYS_NAND_BOOT_SPL_DELTA	(CONFIG_SYS_NAND_BOOT_SPL_SRC - CONFIG_SYS_NAND_BOOT_SPL_DST)

/*
 * Define the partitioning of the NAND chip (only RAM U-Boot is needed here)
 */
#define CONFIG_SYS_NAND_U_BOOT_OFFS	(128 << 10)	/* Offset to RAM U-Boot image */
#define CONFIG_SYS_NAND_U_BOOT_SIZE	0x00100000	/* Size of RAM U-Boot image   */
#define CONFIG_SYS_NAND_U_BOOT_PARTITION 0x00200000	/* NAND reserved for U-Boot + environment + extra blocks */
/* When booting from NOR/SRAM, we can access 1 NAND only */
#if !defined(CONFIG_NAND_U_BOOT) && !defined(CONFIG_NAND_SPL)
#define CONFIG_SYS_NAND_ADDR    CONFIG_SYS_NAND0_ADDR
#if defined(CONFIG_SRAM_U_BOOT)
#define CONFIG_SYS_NAND_CS	0
#else
#define CONFIG_SYS_NAND_CS	1
#endif
#endif

/* Maui support NAND boot with/without ECC */
//#define CONFIG_BOOT_NAND_WO_ECC

/*
 * Now the NAND chip has to be defined (no autodetection used!)
 * For NAND boot used only 
 */
#if defined(CONFIG_NAND_U_BOOT)
#if !defined(CONFIG_MERAKI_BUCKMINISTER)
#if defined(CONFIG_NAND2K_U_BOOT)
/* MT29F2G08AA */
#define CONFIG_SYS_NAND_ADDR	CONFIG_SYS_NAND0_ADDR

#define CONFIG_SYS_NAND_PAGE_SIZE	(2 << 8)	/* NAND chip page size	      */
#define CONFIG_SYS_NAND_BLOCK_SIZE	(16 << 10)	/* NAND chip block size	      */
#define CONFIG_SYS_NAND_PAGE_COUNT	(CONFIG_SYS_NAND_BLOCK_SIZE / CONFIG_SYS_NAND_PAGE_SIZE)
						/* NAND chip page count	      */
#define CONFIG_SYS_NAND_BAD_BLOCK_POS	5		/* Location of bad block marker*/
#undef CONFIG_SYS_NAND_5_ADDR_CYCLE			/* Fifth addr used (<=128MB)  */ 

#define CONFIG_SYS_NAND_ECCSIZE	256
#define CONFIG_SYS_NAND_ECCBYTES	3
#define CONFIG_SYS_NAND_ECCSTEPS	(CONFIG_SYS_NAND_PAGE_SIZE / CONFIG_SYS_NAND_ECCSIZE)
#define CONFIG_SYS_NAND_OOBSIZE	16
#define CONFIG_SYS_NAND_ECCTOTAL	(CONFIG_SYS_NAND_ECCBYTES * CONFIG_SYS_NAND_ECCSTEPS)
#define CONFIG_SYS_NAND_ECCPOS	{0, 1, 2, 3, 6, 7}	

#define CONFIG_SYS_EBC_PB0AP            0x018003c0
#define CONFIG_SYS_EBC_PB0CR            (CONFIG_SYS_NAND_ADDR | 0x1E000) /* BAS=NAND,BS=1MB,BU=R/W,BW=32bit*/

#endif /* defined(CONFIG_NAND2K_U_BOOT */
#else /*!defined(CONFIG_MERAKI_BUCKMINISTER)*/
#if defined(CONFIG_NAND2K_U_BOOT)
/* SAMSUNG K9K8G08U0D */
#define CONFIG_SYS_NAND_ADDR	CONFIG_SYS_NAND0_ADDR

#define CONFIG_SYS_NAND_PAGE_SIZE	(2 << 10)	/* NAND chip page size	      */
#define CONFIG_SYS_NAND_BLOCK_SIZE	(128 << 10)	/* NAND chip block size	      */
#define CONFIG_SYS_NAND_PAGE_COUNT	(CONFIG_SYS_NAND_BLOCK_SIZE / CONFIG_SYS_NAND_PAGE_SIZE)
						/* NAND chip page count	      */
#define CONFIG_SYS_NAND_BAD_BLOCK_POS	0		/* Location of bad block marker*/
#define CONFIG_SYS_NAND_5_ADDR_CYCLE			/* Fifth addr used (<=128MB)  */ 

#define CONFIG_SYS_NAND_ECCSIZE		256
#define CONFIG_SYS_NAND_ECCBYTES	3
#define CONFIG_SYS_NAND_ECCSTEPS	(CONFIG_SYS_NAND_PAGE_SIZE / CONFIG_SYS_NAND_ECCSIZE)
#define CONFIG_SYS_NAND_OOBSIZE		64
#define CONFIG_SYS_NAND_ECCTOTAL	(CONFIG_SYS_NAND_ECCBYTES * CONFIG_SYS_NAND_ECCSTEPS)
#define CONFIG_SYS_NAND_ECCPOS		{40, 41, 42, 43, 44, 45, 46, 47, \
				 	48, 49, 50, 51, 52, 53, 54, 55, \
					56, 57, 58, 59, 60, 61, 62, 63}
#define CONFIG_SYS_EBC_PB0AP            0x018003c0
#define CONFIG_SYS_EBC_PB0CR            (CONFIG_SYS_NAND_ADDR | 0x1E000) /* BAS=NAND,BS=1MB,BU=R/W,BW=32bit*/
#endif /* defined(CONFIG_NAND2K_U_BOOT */
#endif /*!defined(CONFIG_MERAKI_BUCKMINISTER)*/
#endif /* defined(CONFIG_NAND_U_BOOT */

#if !defined(CONFIG_NAND_U_BOOT)
/* Boot from NOR, store environment to NAND */
#ifdef CONFIG_ENV_IS_IN_NAND
/* Too big environment size (512KB) may cause system hang due to out of malloc space */
#define CONFIG_ENV_SIZE		(64 << 10)
#define CONFIG_ENV_OFFSET	(2 << 20 )
#endif

#else /* NAND boot */

#ifdef CONFIG_ENV_IS_IN_NAND
/*
 * The environment size is one NAND block (128K on Buckminster). We allocate four
 * NAND blocks to deal with bad blocks which may exist in the saved
 * environment.
 */
#define CONFIG_ENV_SIZE		CONFIG_SYS_NAND_BLOCK_SIZE
#define CONFIG_ENV_RANGE	(CONFIG_ENV_SIZE * 4)

#define CONFIG_ENV_OFFSET 0x100000
#define CONFIG_ENV_OFFSET_REDUND (CONFIG_ENV_OFFSET + CONFIG_ENV_RANGE)
#endif
#endif /* !defined(CONFIG_NAND_U_BOOT) */

/*-----------------------------------------------------------------------
 * FLASH related
 *----------------------------------------------------------------------*/
/* M29W320DB flash - 4Mbx8 but only maximum of 2MB usable */
#define CONFIG_SYS_FLASH_CFI		/* The flash is CFI compatible	*/
#define CONFIG_FLASH_CFI_DRIVER		/* Use common CFI driver	*/

#define CONFIG_SYS_FLASH_CFI_WIDTH      FLASH_CFI_8BIT
#define CONFIG_SIZE_REDUCE		(3 << 20)	/* Reduce the actual size of the flash */
#define CONFIG_SECTOR_REDUCE		48 		/* Reduce the actual sectors of the flash */

#define CONFIG_SYS_FLASH_BANKS_LIST    {CONFIG_SYS_FLASH_BASE}
#define CONFIG_SYS_MAX_FLASH_BANKS	1	/* max number of memory banks		*/
#define CONFIG_SYS_MAX_FLASH_SECT	80	/* max number of sectors on one chip	*/

#define CONFIG_SYS_FLASH_ERASE_TOUT	120000	/* Timeout for Flash Erase (in ms)	*/
#define CONFIG_SYS_FLASH_WRITE_TOUT	500	/* Timeout for Flash Write (in ms)	*/

#define CONFIG_SYS_FLASH_USE_BUFFER_WRITE 1	/* use buffered writes (20x faster)	*/
#define CONFIG_SYS_FLASH_EMPTY_INFO		/* print 'E' for empty sector on flinfo */

#ifdef CONFIG_ENV_IS_IN_FLASH
#define CONFIG_ENV_SECT_SIZE	0x10000		/* size of one complete sector	*/
#define CONFIG_ENV_ADDR		(CONFIG_SYS_MONITOR_BASE - CONFIG_ENV_SECT_SIZE)
#define	CONFIG_ENV_SIZE		0x4000	/* Total Size of Environment Sector	*/

/* Address and size of Redundant Environment Sector	*/
#define CONFIG_ENV_ADDR_REDUND	(CONFIG_ENV_ADDR - CONFIG_ENV_SECT_SIZE)
#define CONFIG_ENV_SIZE_REDUND	(CONFIG_ENV_SIZE)
#endif /* CONFIG_ENV_IS_IN_FLASH */

/*-----------------------------------------------------------------------
 * NAND-FLASH related
 *----------------------------------------------------------------------*/
#if !defined(CONFIG_NAND_U_BOOT)
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_SYS_NAND_BASE		(CONFIG_SYS_NAND_ADDR + CONFIG_SYS_NAND_CS)
#define CONFIG_SYS_NAND_MAX_CHIPS       1
#else /* NAND boot */
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_SYS_NAND_BASE		{CONFIG_SYS_NAND0_ADDR + 0/*, CONFIG_SYS_NAND1_ADDR + 1*/}
#define CONFIG_SYS_NAND_MAX_CHIPS       1
#define NOT_USE_FLASH
#endif
#define CONFIG_SYS_NAND_SELECT_DEVICE  1	/* nand driver supports mutipl. chips	*/

/*------------------------------------------------------------------------------
 * DDR SDRAM
 *----------------------------------------------------------------------------*/
/* Memory testing options */
/* Only use during bring-up process */
//#define	CONFIG_MEMTEST	

#define CONFIG_SYS_ALT_MEMTEST

#if defined(CONFIG_MEMTEST)

#define CONFIG_SYS_MEMTEST_SCRATCH	(0 + sizeof(vu_long))

#if defined(CONFIG_SYS_MEMTEST_START)
#undef CONFIG_SYS_MEMTEST_START
#define CONFIG_SYS_MEMTEST_START (CONFIG_SYS_MEMTEST_SCRATCH + sizeof(vu_long))
#endif

#if defined(CONFIG_SYS_MEMTEST_END)
#undef CONFIG_SYS_MEMTEST_END
#define CONFIG_SYS_MEMTEST_END ((512 << 20) - sizeof(vu_long)) /* Full usable size of DIMM module */

#endif

#endif

/*
 * Use 1 byte of I2C EEPROM as a flag to force SDRAM controller
 * to be configured as 16-bit
 */
#define CONFIG_SDRAM_INFO_EEPROM_ADDR   0x54 /* Board specific */
#define CONFIG_SDRAM16BIT_OFFSET        0x20 /* Board specific */
#define CONFIG_FORCE_SDRAM16BIT         0x16

#if !defined(CONFIG_NAND_U_BOOT)&& !defined(CONFIG_SRAM_U_BOOT)
/*
 * NAND booting U-Boot version uses a fixed initialization, since the whole
 * I2C SPD DIMM autodetection/calibration doesn't fit into the 4k of boot
 * code.
 */
#define CONFIG_SPD_EEPROM	1	/* Use SPD EEPROM for setup	*/
#define SPD_EEPROM_ADDRESS	{0x53, 0x51}	/* SPD i2c spd addresses*/

//#define CONFIG_44x_SDRAM_DEBUG
#define CONFIG_PPC4xx_DDR_AUTOCALIBRATION       /* IBM DDR autocalibration */
//#define DEBUG_PPC4xx_DDR_AUTOCALIBRATION
#define CONFIG_AUTOCALIB      "silent\0"     	/* default is non-verbose    */
//#define CONFIG_AUTOCALIB      "display\0"     	/* default is non-verbose    */
#define CFG_DDR_EXTRA_MEMTEST2
#define CFG_DDR_EXTRA_MEMTEST
#define CFG_BEGIN_BANK_TEST
#define CFG_MIDLLE_BANK_TEST
#define CFG_END_BANK_TEST

#define CFG_USE_HOS_COUNT
#define CONFIG_DDR_ECC		1	/* with ECC support		*/
#define CONFIG_DDR_RQDC_FIXED	0x3f /* fixed value for RQDC	*/
#define CONFIG_RFFD_GOOD	220  /* Early exit calibration process if RFFD window is wide enough */
//#define	CFG_DDRAUTOCALIB_REDUCE

#else
#if defined(CONFIG_MERAKI_BUCKMINISTER)
/* Fixed DDR initialization */
#define CONFIG_SYS_MBYTES_SDRAM	512	/* 512MB			*/
/* NAND U-Boot with DDR calibration support */
#define CONFIG_NAND_SPL_WITH_DDR_CALIBRATION
#define CONFIG_PPC4xx_DDR_AUTOCALIBRATION       /* IBM DDR autocalibration */
//#define DEBUG_PPC4xx_DDR_AUTOCALIBRATION
//#define CONFIG_AUTOCALIB      "display\0"     /* default is non-verbose    */
#define CONFIG_AUTOCALIB      "silent\0"     	/* default is non-verbose    */
#define CFG_USE_HOS_COUNT
#define CONFIG_DDR_RQDC_FIXED   0x3f
#define CONFIG_RFFD_GOOD        220
/*#define CFG_DDR_EXTRA_MEMTEST2
#define CFG_DDR_EXTRA_MEMTEST
#define CFG_BEGIN_BANK_TEST
#define CFG_MIDLLE_BANK_TEST
#define CFG_END_BANK_TEST*/
/* DDR1/2 SDRAM Device Control Register Data Values */
#define CONFIG_SYS_SDRAM0_MB0CF		0x00007701
#define CONFIG_SYS_SDRAM0_MB1CF		0x00000000
#define CONFIG_SYS_SDRAM0_MB2CF		0x00000000
#define CONFIG_SYS_SDRAM0_MB3CF		0x00000000
#define CONFIG_SYS_SDRAM0_MCOPT1	0x04320000
#define CONFIG_SYS_SDRAM0_MCOPT2	0x00000000
#define CONFIG_SYS_SDRAM0_MODT0		0x01000000
#define CONFIG_SYS_SDRAM0_MODT1		0x00000000
#define CONFIG_SYS_SDRAM0_MODT2		0x00000000
#define CONFIG_SYS_SDRAM0_MODT3		0x00000000
#define CONFIG_SYS_SDRAM0_CODT		0x00800021
#define CONFIG_SYS_SDRAM0_RTR		(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 2 ? 0x06180000 : \
									(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 0x05100000 : \
        							(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 0x07A00000 : 0x06180000))) // default 400M
#define CONFIG_SYS_SDRAM0_INITPLR0	(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 2 ? 0xA8380000 : \
									(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 0x90B80000 : \
        							(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 0x99380000 : 0x99380000))) // default 400M
#define CONFIG_SYS_SDRAM0_INITPLR1	(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 2 ? 0x81900400 : \
									(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 0x81900000 : \
        							(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 0x82100000 : 0x82100000))) // default 400M
#define CONFIG_SYS_SDRAM0_INITPLR2	(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 2 ? 0x81020000 : \
									(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 0x81020000 : \
        							(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 0x81820000 : 0x81820000))) // default 400M
#define CONFIG_SYS_SDRAM0_INITPLR3	(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 2 ? 0x81030000 : \
									(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 0x81030000 : \
        							(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 0x81830000 : 0x81830000))) // default 400M
#define CONFIG_SYS_SDRAM0_INITPLR4	(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 2 ? 0x81010444 : \
									(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 0x81010040 : \
        							(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 0x81810040 : 0x81810040))) // default 400M
#define CONFIG_SYS_SDRAM0_INITPLR5	(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 2 ? 0xE4000532 : \
									(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 0xE4000532 : \
        							(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 0xe4000542 : 0xe4000532))) // default 400M
#define CONFIG_SYS_SDRAM0_INITPLR6	(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 2 ? 0x81900400 : \
									(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 0x81900400 : \
        							(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 0x82100400 : 0x82100400))) // default 400M
#define CONFIG_SYS_SDRAM0_INITPLR7	(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 2 ? 0x8A880000 : \
									(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 0x8B080000 : \
        							(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 0x90080000 : 0x8d080000))) // default 400M
#define CONFIG_SYS_SDRAM0_INITPLR8	(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 2 ? 0x8A880000 : \
									(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 0x8B080000 : \
        							(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 0x90080000 : 0x8d080000))) // default 400M
#define CONFIG_SYS_SDRAM0_INITPLR9	(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 2 ? 0x8A880000 : \
									(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 0x8B080000 : \
        							(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 0x90080000 : 0x8d080000))) // default 400M
#define CONFIG_SYS_SDRAM0_INITPLR10	(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 2 ? 0x8A880000 : \
									(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 0x8B080000 : \
        							(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 0x90080000 : 0x8d080000))) // default 400M
#define CONFIG_SYS_SDRAM0_INITPLR11	(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 2 ? 0x81000432 : \
									(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 0x81000432 : \
        							(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 0x81800442 : 0x81800432))) // default 400M
#define CONFIG_SYS_SDRAM0_INITPLR12	0x810107C4
#define CONFIG_SYS_SDRAM0_INITPLR13	0x81010444
#define CONFIG_SYS_SDRAM0_INITPLR14	0x00000000
#define CONFIG_SYS_SDRAM0_INITPLR15	0x00000000
#define CONFIG_SYS_SDRAM0_RQDC		0x80000038
#define CONFIG_SYS_SDRAM0_RFDC		(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 2 ? 0x003F0000 : \
									(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 0x003F07FF : \
        							(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 0x003F0285 : 0x003f02a2))) // default 400M
#define CONFIG_SYS_SDRAM0_RDCC		0x00000000
#define CONFIG_SYS_SDRAM0_DLCR		(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 2 ? 0x00000000 : \
									(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 0x030000BB : \
        							(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 0x03000078 : 0x0300009b))) // default 400M
#define CONFIG_SYS_SDRAM0_CLKTR		0x40000000
#define CONFIG_SYS_SDRAM0_WRDTR		0x84000800
#define CONFIG_SYS_SDRAM0_SDTR1		(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 2 ? 0x00201000 : \
									(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 0x00201000 : \
        							(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 0x00201000 : 0x80201000))) // default 400M
#define CONFIG_SYS_SDRAM0_SDTR2		(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 2 ? 0x32204232 : \
									(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 0x32103232 : \
        							(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 0x42104242 : 0x42104242))) // default 400M
#define CONFIG_SYS_SDRAM0_SDTR3		(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 2 ? 0x080B0D15 : \
									(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 0x080B0D16 : \
        							(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 0x0C0E0D20 : 0x0a0d0d1a))) // default 400M
#define CONFIG_SYS_SDRAM0_MMODE		(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 2 ? 0x00000432 : \
									(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 0x00000432 : \
        							(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 0x00000642 : 0x00000632))) // default 400M
#define CONFIG_SYS_SDRAM0_MEMODE	0x00000444

#else /*!defined(CONFIG_MERAKI_BUCKMINISTER)*/

/* Fixed DDR initialization */
#define CONFIG_SYS_MBYTES_SDRAM	128	/* 512MB			*/
/* NAND U-Boot with DDR calibration support */
#define CONFIG_NAND_SPL_WITH_DDR_CALIBRATION
#define CONFIG_PPC4xx_DDR_AUTOCALIBRATION       /* IBM DDR autocalibration */
//#define DEBUG_PPC4xx_DDR_AUTOCALIBRATION
//#define CONFIG_AUTOCALIB      "display\0"     /* default is non-verbose    */
#define CONFIG_AUTOCALIB      "silent\0"     	/* default is non-verbose    */
#define CFG_USE_HOS_COUNT
#define CONFIG_DDR_RQDC_FIXED   0x3f
#define CONFIG_RFFD_GOOD        220
/*#define CFG_DDR_EXTRA_MEMTEST2
#define CFG_DDR_EXTRA_MEMTEST
#define CFG_BEGIN_BANK_TEST
#define CFG_MIDLLE_BANK_TEST
#define CFG_END_BANK_TEST*/
/* DDR1/2 SDRAM Device Control Register Data Values */
#define CONFIG_SYS_SDRAM0_MB0CF		0x00005201
#define CONFIG_SYS_SDRAM0_MB1CF		0x00000000
#define CONFIG_SYS_SDRAM0_MB2CF		0x00000000
#define CONFIG_SYS_SDRAM0_MB3CF		0x00000000
#define CONFIG_SYS_SDRAM0_MCOPT1	0x04120000
#define CONFIG_SYS_SDRAM0_MCOPT2	0x00000000
#define CONFIG_SYS_SDRAM0_MODT0		0x01000000
#define CONFIG_SYS_SDRAM0_MODT1		0x00000000
#define CONFIG_SYS_SDRAM0_MODT2		0x00000000
#define CONFIG_SYS_SDRAM0_MODT3		0x00000000
#define CONFIG_SYS_SDRAM0_CODT		0x00800021
#define CONFIG_SYS_SDRAM0_RTR		(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 2 ? 0x06180000 : \
									(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 0x05100000 : \
        							(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 0x07A00000 : 0x06180000))) // default 400M
#define CONFIG_SYS_SDRAM0_INITPLR0	(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 2 ? 0xA8380000 : \
									(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 0x90B80000 : \
        							(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 0x99380000 : 0x99380000))) // default 400M
#define CONFIG_SYS_SDRAM0_INITPLR1	(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 2 ? 0x81900400 : \
									(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 0x81900000 : \
        							(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 0x82100000 : 0x82100000))) // default 400M
#define CONFIG_SYS_SDRAM0_INITPLR2	(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 2 ? 0x81020000 : \
									(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 0x81020000 : \
        							(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 0x81820000 : 0x81820000))) // default 400M
#define CONFIG_SYS_SDRAM0_INITPLR3	(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 2 ? 0x81030000 : \
									(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 0x81030000 : \
        							(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 0x81830000 : 0x81830000))) // default 400M
#define CONFIG_SYS_SDRAM0_INITPLR4	(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 2 ? 0x81010444 : \
									(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 0x81010040 : \
        							(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 0x81810040 : 0x81810040))) // default 400M
#define CONFIG_SYS_SDRAM0_INITPLR5	(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 2 ? 0xE4000532 : \
									(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 0xE4000532 : \
        							(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 0xe4000542 : 0xe4000532))) // default 400M
#define CONFIG_SYS_SDRAM0_INITPLR6	(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 2 ? 0x81900400 : \
									(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 0x81900400 : \
        							(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 0x82100400 : 0x82100400))) // default 400M
#define CONFIG_SYS_SDRAM0_INITPLR7	(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 2 ? 0x8A880000 : \
									(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 0x8B080000 : \
        							(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 0x90080000 : 0x8d080000))) // default 400M
#define CONFIG_SYS_SDRAM0_INITPLR8	(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 2 ? 0x8A880000 : \
									(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 0x8B080000 : \
        							(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 0x90080000 : 0x8d080000))) // default 400M
#define CONFIG_SYS_SDRAM0_INITPLR9	(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 2 ? 0x8A880000 : \
									(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 0x8B080000 : \
        							(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 0x90080000 : 0x8d080000))) // default 400M
#define CONFIG_SYS_SDRAM0_INITPLR10	(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 2 ? 0x8A880000 : \
									(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 0x8B080000 : \
        							(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 0x90080000 : 0x8d080000))) // default 400M
#define CONFIG_SYS_SDRAM0_INITPLR11	(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 2 ? 0x81000432 : \
									(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 0x81000432 : \
        							(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 0x81800442 : 0x81800432))) // default 400M
#define CONFIG_SYS_SDRAM0_INITPLR12	0x810107C4
#define CONFIG_SYS_SDRAM0_INITPLR13	0x81010444
#define CONFIG_SYS_SDRAM0_INITPLR14	0x00000000
#define CONFIG_SYS_SDRAM0_INITPLR15	0x00000000
#define CONFIG_SYS_SDRAM0_RQDC		0x80000038
#define CONFIG_SYS_SDRAM0_RFDC		(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 2 ? 0x003F0000 : \
									(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 0x003F07FF : \
        							(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 0x003F0285 : 0x003f02a2))) // default 400M
#define CONFIG_SYS_SDRAM0_RDCC		0x00000000
#define CONFIG_SYS_SDRAM0_DLCR		(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 2 ? 0x00000000 : \
									(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 0x030000BB : \
        							(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 0x03000078 : 0x0300009b))) // default 400M
#define CONFIG_SYS_SDRAM0_CLKTR		0x40000000
#define CONFIG_SYS_SDRAM0_WRDTR		0x84000800
#define CONFIG_SYS_SDRAM0_SDTR1		(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 2 ? 0x00201000 : \
									(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 0x00201000 : \
        							(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 0x00201000 : 0x80201000))) // default 400M
#define CONFIG_SYS_SDRAM0_SDTR2		(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 2 ? 0x32204232 : \
									(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 0x32103232 : \
        							(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 0x42104242 : 0x42104242))) // default 400M
#define CONFIG_SYS_SDRAM0_SDTR3		(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 2 ? 0x080B0D15 : \
									(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 0x080B0D16 : \
        							(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 0x0C0E0D20 : 0x0a0d0d1a))) // default 400M
#define CONFIG_SYS_SDRAM0_MMODE		(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 2 ? 0x00000432 : \
									(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 0x00000432 : \
        							(*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 0x00000642 : 0x00000632))) // default 400M
#define CONFIG_SYS_SDRAM0_MEMODE	0x00000444
#endif /*!defined(CONFIG_MERAKI_BUCKMINISTER)*/
#endif	/* !defined(CONFIG_NAND_U_BOOT) */


/*-----------------------------------------------------------------------
 * I2C
 *----------------------------------------------------------------------*/
#define CONFIG_SYS_I2C_SPEED		400000	/* I2C speed			*/

#define CONFIG_SYS_I2C_MULTI_EEPROMS
#define CONFIG_SYS_I2C_EEPROM_ADDR		(0xa8>>1)
#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN		1
#define CONFIG_SYS_EEPROM_PAGE_WRITE_BITS	3
#define CONFIG_SYS_EEPROM_PAGE_WRITE_DELAY_MS	5	/* Data sheet */

/* I2C bootstrap EEPROM */
#define CONFIG_4xx_CONFIG_I2C_EEPROM_ADDR	0x52
#define CONFIG_4xx_CONFIG_I2C_EEPROM_OFFSET	0
#define CONFIG_4xx_CONFIG_BLOCKSIZE		16

#define CONFIG_RTC_M41T62
#define CONFIG_SYS_I2C_RTC_ADDR	0x68
/*-----------------------------------------------------------------------
 * Ethernet
 *----------------------------------------------------------------------*/
#define CONFIG_IBM_EMAC4_V4	1

#define CONFIG_EMAC_PHY_MODE  EMAC_PHY_MODE_NONE_RGMII

#define CONFIG_HAS_ETH0
//#define CONFIG_RTL8211CL_PHY
#if !defined(CONFIG_MERAKI_BUCKMINISTER)
#define CONFIG_AR8035_PHY
#define CONFIG_PHY_ADDR		1	/* PHY address, See schematics	*/
#else /*!defined(CONFIG_MERAKI_BUCKMINISTER)*/
#define CONFIG_AR8327_SWITCH
#define CONFIG_FIXED_PHY   0xFFFFFFFF
#define CONFIG_PHY_ADDR    CONFIG_FIXED_PHY
#define CONFIG_SYS_FIXED_PHY_PORT(devnum,speed,duplex) \
			{devnum, speed, duplex},
#define CONFIG_SYS_FIXED_PHY_PORTS CONFIG_SYS_FIXED_PHY_PORT(0,1000,FULL)
#endif /*!defined(CONFIG_MERAKI_BUCKMINISTER)*/

#define CONFIG_PHY_RESET	1	/* reset phy upon startup	*/
#define CONFIG_PHY_GIGE		1	/* Include GbE speed/duplex detection */
#define CONFIG_PHY_DYNAMIC_ANEG	1
#define INFO_4XX_ENET		1

/*
 * Default environment variables
 */

/* Customize AMCC default environment for Bluestone */
#undef CONFIG_HOSTNAME
#define CONFIG_SYS_HOSTNAME    buck
#define CONFIG_SYS_PLATFORM    buck

#undef CONFIG_BOOTCOMMAND
#undef CONFIG_AMCC_DEF_ENV
#undef CONFIG_AMCC_DEF_ENV_POWERPC
#undef CONFIG_AMCC_DEF_ENV_NOR_UPD
#undef CONFIG_AMCC_DEF_ENV_NAND_UPD

#define MERAKI_PANIC_PART 0x00200000
#define MERAKI_PANIC_SIZE 0x00040000

#define MERAKI_UBI_PART 0x00240000
#define MERAKI_UBI_SIZE 0x3fdc0000

/* Can overwrite Ethernet MAC address */
#define CONFIG_ENV_OVERWRITE

#undef CONFIG_PREBOOT
#define CONFIG_PREBOOT "echo;echo Set serverpath and run meraki_netboot to netboot;echo"

#undef CONFIG_BOOTDELAY
#define CONFIG_BOOTDELAY 1

#define CONFIG_AMCC_DEF_ENV_NAND_UPD                                    \
        "nload=${netloadmethod} 200000 ${serverpath}u-boot-nand.bin\0" \
	"nupdate=nand erase 0 " xstr(CONFIG_SYS_NAND_U_BOOT_PARTITION)" ;nand write 200000 0 " xstr(CONFIG_SYS_NAND_U_BOOT_SIZE) "\0" \
        "nupd=run nload nupdate\0"

//Default values for older partitions.
//meraki_checkpart will overwrite these values
//for newer partitions.
#define CONFIG_MERAKI_DEFAULT_PART_OFFSETS				\
	"meraki_loadaddr_fdt=800400\0"					\
	"meraki_loadaddr_kernel=810000\0"				\
	"meraki_loadaddr_ramdisk=a00000\0"				\


#define CONFIG_MERAKI_ENV_SETTINGS					\
	"bootcmd=run meraki_boot\0"					\
	"meraki_bootargs=setenv bootargs root=/dev/ram console=ttyS0,${baudrate} ubi.mtd=ubi MERAKI_BOARD=buck mtdoops.mtddev=oops ${extra_bootargs}\0" \
	"meraki_bootfile=buck.bin\0"					\
	"meraki_bootlinux=bootm ${meraki_loadaddr_kernel} ${meraki_loadaddr_ramdisk} ${meraki_loadaddr_fdt}\0"	\
	"meraki_boot=run meraki_ubi meraki_bootargs ; run meraki_load1 meraki_checkpart meraki_bootlinux; " \
	"run meraki_load2 meraki_checkpart2 meraki_bootlinux\0"				\
	"meraki_checkpart=meraki checkpart ${meraki_loadaddr}\0"	\
	"meraki_checkpart2=meraki checkpart ${meraki_loadaddr} skiphash\0"	\
	"meraki_netboot=run meraki_load_net meraki_bootargs meraki_checkpart2 meraki_bootlinux\0" \
        "meraki_loadaddr=800000\0"					\
	"meraki_part_fdt_index=0\0"					\
	CONFIG_MERAKI_DEFAULT_PART_OFFSETS				\
	"meraki_load1=ubi read ${meraki_loadaddr} part1\0"		\
	"meraki_load2=ubi read ${meraki_loadaddr} part2\0"		\
	"meraki_load_net=${netloadmethod} ${meraki_loadaddr} ${serverpath}${meraki_bootfile}\0" \
	"meraki_ubi=ubi part ubi\0"					\
	"meraki_ubifile=buck-ubi.bin\0"					\
	"meraki_ubi_loadfile=${netloadmethod} 200000 ${serverpath}${meraki_ubifile}\0" \
	"meraki_ubi_update_nand=nand erase " xstr(MERAKI_UBI_PART) " " xstr(MERAKI_UBI_SIZE) " ; " \
        "nand write 200000 " xstr(MERAKI_UBI_PART) " ${filesize}\0"	\
	"meraki_ubi_update=run meraki_ubi_loadfile meraki_ubi_update_nand\0" \
	"meraki_update_part1=run meraki_ubi meraki_load_net meraki_write1\0"	\
	"meraki_update_part2=run meraki_ubi meraki_load_net meraki_write2\0"	\
	"meraki_write1=ubi write ${meraki_loadaddr} part1 ${filesize}\0" \
	"meraki_write2=ubi write ${meraki_loadaddr} part2 ${filesize}\0" \
	"mtdids=nand0=nand0\0"						\
	"mtdparts=mtdparts=nand0:" xstr(CONFIG_SYS_NAND_U_BOOT_PARTITION) "@0(firmware)," \
         xstr(MERAKI_PANIC_SIZE) "@" xstr(MERAKI_PANIC_PART)"(panic)," \
	 xstr(MERAKI_UBI_SIZE) "@" xstr(MERAKI_UBI_PART)"(ubi)\0"	\

#define	CONFIG_EXTRA_ENV_SETTINGS					\
	CONFIG_AMCC_DEF_ENV_NAND_UPD					\
	"kernel_addr=fc000000\0"					\
	"fdt_addr=fc1e0000\0"						\
	"ramdisk_addr=fc200000\0"					\
	"pciconfighost=1\0"						\
	"pcie_mode=RP:RP\0"						\
	"pcie=enabled\0"						\
        "netdev=eth0\0"                                                 \
	"netloadmethod=dhcp\0"						\
	"ethact=ppc_4xx_eth0\0"						\
	"ethaddr=00:01:73:01:23:41\0"					\
	"boardtype=pcie\0"						\
	"mtd_addr_r=0x4000000\0"					\
	"kernel_size=0x400000\0"	\
	"fdt_size=0x25000\0"		\
	CONFIG_MERAKI_ENV_SETTINGS						\
	"serverpath=10.1.30.240:tftpboot/\0" \
	"ipaddr=192.168.1.1\0"							\
	"serverip=192.168.1.101\0"						\
	"gatewayip=192.168.1.1\0"						\
	"netmask=255.255.255.0\0"						\
	"factory_cmdline=setenv bootargs root=/dev/ram ramdisk_size=${factory_ramdisk_size} rw " \
	"ip=${ipaddr}:${serverip}:${gatewayip}:${netmask}:${hostname}:${netdev}:off panic=1 " \
	"console="  xstr(CONFIG_USE_TTY) ",${baudrate}\0"		\
	"factory_ramdisk_file=uRamdisk\0"				\
	"factory_bootfile=uImage\0"					\
	"factory_fdt_file=bluestone.dtb\0"				\
	"factory_kernel_addr_r=0x400000\0"				\
	"factory_fdt_addr_r=0x800000\0"					\
	"factory_mtd_addr_r=0x4000000\0"				\
	"factory_ramdisk_size=0x200000\0"				\
	"factory_boot=tftp ${factory_kernel_addr_r} ${factory_bootfile};" \
			"tftp ${factory_fdt_addr_r} ${factory_fdt_file};" \
			"tftp ${factory_mtd_addr_r} ${factory_ramdisk_file};" \
			"run factory_cmdline;"				\
			"bootm ${factory_kernel_addr_r} ${factory_mtd_addr_r} ${factory_fdt_addr_r}\0"

/*
 * Commands additional to the ones defined in amcc-common.h
 */
#define CONFIG_CMD_CHIP_CONFIG
#define CONFIG_CMD_I2C
#define CONFIG_CMD_DATE

/* 
 * Beech has Temperature sensor but Maui cannot access it via I2C 
 * The sensor is connected with external I2C bus
 */
/* #define CONFIG_CMD_DTT */
#undef CONFIG_CMD_EXT2
#undef CONFIG_CMD_FAT
#define CONFIG_CMD_JFFS2
#define CONFIG_CMD_MTDPARTS
#define CONFIG_CMD_NAND
#define CONFIG_CMD_PCI
#undef CONFIG_CMD_SATA
#define CONFIG_CMD_SDRAM
#define CONFIG_CMD_UBI

#define CONFIG_MTD_DEVICE

/* Partitions */
#undef CONFIG_MAC_PARTITION
#undef CONFIG_DOS_PARTITION
#undef CONFIG_ISO_PARTITION
#undef CONFIG_EFI_PARTITION
#define CONFIG_MTD_PARTITIONS

#define CONFIG_RBTREE		// Needed for UBI support

#if defined(CONFIG_SRAM_U_BOOT)
#undef CONFIG_CMD_CHIP_CONFIG
#undef CONFIG_CMD_DATE
#undef CONFIG_CMD_EXT2
#undef CONFIG_CMD_FAT
#undef CONFIG_CMD_PCI
#undef CONFIG_CMD_SATA
#undef CONFIG_CMD_SDRAM
#undef CONFIG_MAC_PARTITION
#undef CONFIG_DOS_PARTITION
#undef CONFIG_ISO_PARTITION
#undef CONFIG_EFI_PARTITION

#undef CONFIG_CMD_BDI        
#undef CONFIG_CMD_BOOTD      
#undef CONFIG_CMD_CONSOLE    
#undef CONFIG_CMD_FPGA      
#undef CONFIG_CMD_IMI       
#undef CONFIG_CMD_ITEST     
#undef CONFIG_CMD_LOADB     
#undef CONFIG_CMD_LOADS     
#undef CONFIG_CMD_MISC         
#undef CONFIG_CMD_SOURCE       

#undef CONFIG_CMD_ASKENV
#if defined(CONFIG_440)
#undef CONFIG_CMD_CACHE
#endif
#undef CONFIG_CMD_DHCP
#undef CONFIG_CMD_DIAG
#undef CONFIG_CMD_EEPROM
#undef CONFIG_CMD_ELF
#undef CONFIG_CMD_IRQ
//#undef CONFIG_CMD_MII
#undef CONFIG_CMD_NFS
#undef CONFIG_CMD_REGINFO

/* Misc. options */
#undef CONFIG_CMDLINE_EDITING         
#undef CONFIG_AUTO_COMPLETE         
#undef CONFIG_LOOPW                 
#undef CONFIG_MX_CYCLIC             

/* Discard LIBFDT in U-Boot SRAM */
#undef CONFIG_OF_LIBFDT
#undef CONFIG_OF_BOARD_SETUP

#endif

/*-----------------------------------------------------------------------
 * PCI stuff
 *----------------------------------------------------------------------*/
/* 
 * There is no  PCI in Maui but we need to add definition here to make
 * the PCIE setup code take effect
 */
#define CONFIG_PCI                      /* include pci support          */
#define CONFIG_PCI_PNP          1       /* do pci plug-and-play         */
#define CONFIG_PCI_SCAN_SHOW    1       /* show pci devices on startup  */
#define CONFIG_PCI_CONFIG_HOST_BRIDGE
	
/*
 * SATA driver setup
 */
#ifdef CONFIG_CMD_SATA
#define CONFIG_LBA48
#define CONFIG_SATA_DWC
#define CONFIG_LIBATA
#define SATA_BASE_ADDR		(CONFIG_SYS_AHB_BASE + 0xD1000)	/* SATA0  */
#define SATA1_BASE_ADDR		(CONFIG_SYS_AHB_BASE + 0xD1800)	/* SATA1  */
#define SATA_DMA_REG_ADDR	(CONFIG_SYS_AHB_BASE + 0xD0800)	/* AHB DMA */
#define CONFIG_SYS_SATA_MAX_DEVICE	1	/* SATA MAX DEVICE */
/* Convert sectorsize to wordsize */
#define ATA_SECTOR_WORDS (ATA_SECT_SIZE/2)
#endif

/*-----------------------------------------------------------------------
 * External Bus Controller (EBC) Setup
 *----------------------------------------------------------------------*/
#define CONFIG_SYS_EBC_CFG		0xB8400000		/*  EBC0_CFG */

/*
 * PPC4xx GPIO Configuration
 */
/* APM821x1 has only 32 GPIO pins */

/* Misc. configuration */
#define CONFIG_BRINGUP_DEBUG
/*
 *#define NOT_SAVE_ENV: Not save environtment
 *#define NOT_USE_FLASH: Not use NOT FLASH
 *#define NOT_USE_NAND: Not use NAND FLASH
 *#define NOT_USE_PCIE: Not use PCIE
 */
#if defined(CONFIG_SRAM_U_BOOT)
#define NOT_SAVE_ENV
#define NOT_USE_FLASH
//Alex #define NOT_USE_PCIE
#endif

#if defined(CONFIG_BRINGUP_DEBUG)

#if defined(NOT_SAVE_ENV)
#undef CONFIG_ENV_IS_IN_FLASH         /* not use FLASH for environment vars */
#undef CONFIG_ENV_IS_IN_NAND
#define CONFIG_ENV_IS_NOWHERE
#endif

#if defined(NOT_USE_FLASH)
#define CONFIG_SYS_NO_FLASH
#undef CONFIG_CMD_FLASH
#undef CONFIG_CMD_IMLS
#undef CONFIG_FLASH_CFI
#undef CONFIG_FLASH_CFI_DRIVER
#undef CONFIG_ENV_ADDR_REDUND
#if defined(CONFIG_ENV_IS_IN_FLASH)
#undef CONFIG_ENV_IS_IN_FLASH
#define CONFIG_ENV_IS_NOWHERE
#endif
#endif

#if defined(NOT_USE_NAND)
#undef CONFIG_CMD_NAND	
#if defined(CONFIG_ENV_IS_IN_NAND)
#undef CONFIG_ENV_IS_IN_NAND
#define CONFIG_ENV_IS_NOWHERE      
#undef CONFIG_ENV_OFFSET_REDUND
#endif
#endif

#if defined(NOT_USE_PCIE)
#undef CONFIG_CMD_PCI
#undef CONFIG_PCI
#endif

#endif /* CONFIG_BRINGUP_DEBUG  */

#endif	/* __CONFIG_H */
