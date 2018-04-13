/*
 * (C) Copyright 2009
 * Duc Dang, AppliedMicro, dhdang@amcc.com
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
 * beech.h - configuration for Beech (APM821x1)
 ***********************************************************************/
#ifndef __CONFIG_H
#define __CONFIG_H

/*-----------------------------------------------------------------------
 * High Level Configuration Options
 *----------------------------------------------------------------------*/
/*
 * This config file is used for Beech (APM821x1)
 *
 */
#define CONFIG_APM82181		1	/* Specific APM821x1		*/
#define CONFIG_HOSTNAME		beech

#define CONFIG_440		1
#define CONFIG_464FP		1	/* New 464 core with FP */
#define CONFIG_4xx		1	/* ... PPC4xx family */

/*
 * Include common defines/options for all AMCC eval boards
 */
#include "amcc-common.h"
#define CONFIG_SYS_CLK_FREQ	50000000

#define CONFIG_BOARD_EARLY_INIT_F	1	/* Call board_early_init_f */
#define CONFIG_BOARD_EARLY_INIT_R	1	/* Call board_early_init_r */
#define CONFIG_MISC_INIT_R		1	/* Call misc_init_r */
#define CONFIG_BOARD_TYPES		1	/* support board types */

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
        (*(u32*)CONFIG_PLL_RECONFIG_ADDR == 3 ? 3 : \
        (*(u32*)CONFIG_PLL_RECONFIG_ADDR == 4 ? 4 : \
        (*(u32*)CONFIG_PLL_RECONFIG_ADDR == 5 ? 5 : \
        (*(u32*)CONFIG_PLL_RECONFIG_ADDR == 6 ? 6 : 2)))))
#else
#define CONFIG_SYS_PLL_RECONFIG CONFIG_CLK_1000_166_250_83
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
#define CONFIG_SYS_PCIE_BASE		0xc4000000	/* PCIe UTL regs */

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
#undef CONFIG_UART1_CONSOLE	/* define this if you want console on UART1 */
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
#define CONFIG_ENV_IS_EMBEDDED	1	/* use embedded environment */
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
#define CONFIG_SYS_NAND_U_BOOT_SIZE	(1 << 20)	/* Size of RAM U-Boot image   */

/* When booting from NOR/SRAM, we can access 1 NAND only */
#if !defined(CONFIG_NAND_U_BOOT) && !defined(CONFIG_NAND_SPL) 
#define CONFIG_SYS_NAND_ADDR    CONFIG_SYS_NAND0_ADDR
#if defined(CONFIG_SRAM_U_BOOT)
#define CONFIG_SYS_NAND_CS	0
#else
#define CONFIG_SYS_NAND_CS      1
#endif
#endif

/* Maui support NAND boot with/without ECC */
//#define CONFIG_BOOT_NAND_WO_ECC

/*
 * Now the NAND chip has to be defined (no autodetection used!)
 * For NAND boot used only 
 */
#if defined(CONFIG_NAND_U_BOOT)
#if defined(CONFIG_NAND_4CYCLE_U_BOOT)
#define CONFIG_SYS_NAND_ADDR	CONFIG_SYS_NAND0_ADDR

#define CONFIG_SYS_NAND_PAGE_SIZE	(2 << 10)	/* NAND chip page size	      */
#define CONFIG_SYS_NAND_BLOCK_SIZE	(128 << 10)	/* NAND chip block size	      */
#define CONFIG_SYS_NAND_PAGE_COUNT	(CONFIG_SYS_NAND_BLOCK_SIZE / CONFIG_SYS_NAND_PAGE_SIZE)
						/* NAND chip page count	      */
#define CONFIG_SYS_NAND_BAD_BLOCK_POS	0		/* Location of bad block marker*/
#define CONFIG_SYS_NAND_4_ADDR_CYCLE			/* Fifth addr used (<=128MB)  */

#define CONFIG_SYS_NAND_ECCSIZE	256
#define CONFIG_SYS_NAND_ECCBYTES	3
#define CONFIG_SYS_NAND_ECCSTEPS	(CONFIG_SYS_NAND_PAGE_SIZE / CONFIG_SYS_NAND_ECCSIZE)
#define CONFIG_SYS_NAND_OOBSIZE	64
#define CONFIG_SYS_NAND_ECCTOTAL	(CONFIG_SYS_NAND_ECCBYTES * CONFIG_SYS_NAND_ECCSTEPS)
#define CONFIG_SYS_NAND_ECCPOS		{40, 41, 42, 43, 44, 45, 46, 47, \
				 48, 49, 50, 51, 52, 53, 54, 55, \
				 56, 57, 58, 59, 60, 61, 62, 63}

#define CONFIG_SYS_EBC_PB0AP            0x018003c0
#define CONFIG_SYS_EBC_PB0CR            (CONFIG_SYS_NAND_ADDR | 0x1E000) /* BAS=NAND,BS=1MB,BU=R/W,BW=32bit*/

#else
#if defined(CONFIG_NAND2K_U_BOOT)
/* MT29F2G08AA */
#define CONFIG_SYS_NAND_ADDR	CONFIG_SYS_NAND0_ADDR

#define CONFIG_SYS_NAND_PAGE_SIZE	(2 << 10)	/* NAND chip page size	      */
#define CONFIG_SYS_NAND_BLOCK_SIZE	(128 << 10)	/* NAND chip block size	      */
#define CONFIG_SYS_NAND_PAGE_COUNT	(CONFIG_SYS_NAND_BLOCK_SIZE / CONFIG_SYS_NAND_PAGE_SIZE)
						/* NAND chip page count	      */
#define CONFIG_SYS_NAND_BAD_BLOCK_POS	0		/* Location of bad block marker*/
#define CONFIG_SYS_NAND_5_ADDR_CYCLE			/* Fifth addr used (<=128MB)  */ 

#define CONFIG_SYS_NAND_ECCSIZE	256
#define CONFIG_SYS_NAND_ECCBYTES	3
#define CONFIG_SYS_NAND_ECCSTEPS	(CONFIG_SYS_NAND_PAGE_SIZE / CONFIG_SYS_NAND_ECCSIZE)
#define CONFIG_SYS_NAND_OOBSIZE	64
#define CONFIG_SYS_NAND_ECCTOTAL	(CONFIG_SYS_NAND_ECCBYTES * CONFIG_SYS_NAND_ECCSTEPS)
#define CONFIG_SYS_NAND_ECCPOS		{40, 41, 42, 43, 44, 45, 46, 47, \
				 48, 49, 50, 51, 52, 53, 54, 55, \
				 56, 57, 58, 59, 60, 61, 62, 63}

#define CONFIG_SYS_EBC_PB0AP            0x018003c0
#define CONFIG_SYS_EBC_PB0CR            (CONFIG_SYS_NAND_ADDR | 0x1E000) /* BAS=NAND,BS=1MB,BU=R/W,BW=32bit*/

#else	
/* NAND Samsung K9GAG08U0M - 4K page size */
#define CONFIG_SYS_NAND_ADDR    CONFIG_SYS_NAND0_ADDR

#define CONFIG_SYS_NAND_PAGE_SIZE       (4 << 10)       /* NAND chip page size        */
#define CONFIG_SYS_NAND_BLOCK_SIZE      (512 << 10)     /* NAND chip block size       */
#define CONFIG_SYS_NAND_PAGE_COUNT      (CONFIG_SYS_NAND_BLOCK_SIZE / CONFIG_SYS_NAND_PAGE_SIZE)
                                                /* NAND chip page count       */
#define CONFIG_SYS_NAND_BAD_BLOCK_POS   0               /* Location of bad block marker*/
#define CONFIG_SYS_NAND_5_ADDR_CYCLE                    /* Fifth addr used (<=128MB)  */

#define CONFIG_SYS_NAND_ECCSIZE 256
#define CONFIG_SYS_NAND_ECCBYTES        3	/* drivers/mtd/nand_base.c */
#define CONFIG_SYS_NAND_ECCSTEPS        (CONFIG_SYS_NAND_PAGE_SIZE / CONFIG_SYS_NAND_ECCSIZE)
#define CONFIG_SYS_NAND_OOBSIZE 128
#define CONFIG_SYS_NAND_ECCTOTAL        (CONFIG_SYS_NAND_ECCBYTES * CONFIG_SYS_NAND_ECCSTEPS)
#define CONFIG_SYS_NAND_ECCPOS          {80,  81,  82,  83,  84,  85,  86,  87, \
				88,  89,  90,  91,  92,  93,  94,  95, \
		                96,  97,  98,  99, 100, 101, 102, 103, \
				104, 105, 106, 107, 108, 109, 110, 111, \
                                112, 113, 114, 115, 116, 117, 118, 119, \
                                120, 121, 122, 123, 124, 125, 126, 127}

#define CONFIG_SYS_EBC_PB0AP            0x018003c0
#define CONFIG_SYS_EBC_PB0CR            (CONFIG_SYS_NAND_ADDR | 0x1E000) /* BAS=NAND,BS=1MB,BU=R/W,BW=32bit*/

#endif /* defined(CONFIG_NAND2K_U_BOOT */
#endif /* CONFIG_NAND_4CYCLE_U_BOOT */
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
 * For NAND booting the environment is embedded in the U-Boot image. Please take
 * look at the file board/amcc/canyonlands/u-boot-nand.lds for details.
 */
#define CONFIG_ENV_SIZE			CONFIG_SYS_NAND_BLOCK_SIZE
#define CONFIG_ENV_OFFSET		(CONFIG_SYS_NAND_U_BOOT_OFFS + CONFIG_ENV_SIZE)
#define CONFIG_ENV_OFFSET_REDUND	(CONFIG_ENV_OFFSET + CONFIG_ENV_SIZE)
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
#define CONFIG_SDRAM_INFO_EEPROM_ADDR	0x54 /* Board specific */
#define CONFIG_SDRAM16BIT_OFFSET	0x20 /* Board specific */
#define CONFIG_FORCE_SDRAM16BIT		0x16

#if !defined(CONFIG_NAND_U_BOOT)
/*
 * NAND booting U-Boot version uses a fixed initialization, since the whole
 * I2C SPD DIMM autodetection/calibration doesn't fit into the 4k of boot
 * code.
 */
#define CONFIG_SPD_EEPROM	1	/* Use SPD EEPROM for setup	*/
#define SPD_EEPROM_ADDRESS	{0x50, 0x51}	/* SPD i2c spd addresses*/

//#define CONFIG_44x_SDRAM_DEBUG
#define CONFIG_PPC4xx_DDR_AUTOCALIBRATION       /* IBM DDR autocalibration */
//#define DEBUG_PPC4xx_DDR_AUTOCALIBRATION        /* dynamic DDR autocal debug */
#define CONFIG_AUTOCALIB      "silent\0"     /* default is non-verbose    */

#define CFG_DDR_EXTRA_MEMTEST2
#define CFG_DDR_EXTRA_MEMTEST
#define CFG_BEGIN_BANK_TEST
#define CFG_MIDLLE_BANK_TEST
#define CFG_END_BANK_TEST

#define CFG_USE_HOS_COUNT
#define CONFIG_DDR_ECC		1	/* with ECC support		*/
#define CONFIG_DDR_RQDC_FIXED	0x3f /* fixed value for RQDC	*/
#define CONFIG_RFFD_GOOD	220  /* Early exit calibration procss if RFFD window is wide enough */
//#define	CFG_DDRAUTOCALIB_REDUCE

#else
/* Fixed DDR initialization */
#define CONFIG_SYS_MBYTES_SDRAM	512	/* 512MB			*/

/* DDR1/2 SDRAM Device Control Register Data Values */
/* Maui does not have Memory Queue */

/* SDRAM Controller */
#define CONFIG_SYS_SDRAM0_MB0CF		0x00007701
#define CONFIG_SYS_SDRAM0_MB1CF		0x00000000
#define CONFIG_SYS_SDRAM0_MB2CF		0x00000000
#define CONFIG_SYS_SDRAM0_MB3CF		0x00000000
#define CONFIG_SYS_SDRAM0_MCOPT1	0x34322000
#define CONFIG_SYS_SDRAM0_MCOPT2	0x08000000
#define CONFIG_SYS_SDRAM0_MODT0		0x01000000
#define CONFIG_SYS_SDRAM0_MODT1		0x00000000
#define CONFIG_SYS_SDRAM0_MODT2		0x00000000
#define CONFIG_SYS_SDRAM0_MODT3		0x00000000
#define CONFIG_SYS_SDRAM0_CODT		0x00800021
#define CONFIG_SYS_SDRAM0_RTR		0x07a00000
#define CONFIG_SYS_SDRAM0_INITPLR0	0x99380000
#define CONFIG_SYS_SDRAM0_INITPLR1	0x82100000
#define CONFIG_SYS_SDRAM0_INITPLR2	0x81820000
#define CONFIG_SYS_SDRAM0_INITPLR3	0x81830000
#define CONFIG_SYS_SDRAM0_INITPLR4	0x81810040
#define CONFIG_SYS_SDRAM0_INITPLR5	0xe4000542
#define CONFIG_SYS_SDRAM0_INITPLR6	0x82100400
#define CONFIG_SYS_SDRAM0_INITPLR7	0x90080000
#define CONFIG_SYS_SDRAM0_INITPLR8	0x90080000
#define CONFIG_SYS_SDRAM0_INITPLR9	0x90080000
#define CONFIG_SYS_SDRAM0_INITPLR10	0x90080000
#define CONFIG_SYS_SDRAM0_INITPLR11	0x81800442
#define CONFIG_SYS_SDRAM0_INITPLR12	0x818103c0
#define CONFIG_SYS_SDRAM0_INITPLR13	0x81810040
#define CONFIG_SYS_SDRAM0_INITPLR14	0x00000000
#define CONFIG_SYS_SDRAM0_INITPLR15	0x00000000
#define CONFIG_SYS_SDRAM0_RQDC		0x8000003e
#define CONFIG_SYS_SDRAM0_RFDC		0x0000022f
#define CONFIG_SYS_SDRAM0_RDCC		0x40000000
#define CONFIG_SYS_SDRAM0_DLCR		0x0300007a
#define CONFIG_SYS_SDRAM0_CLKTR		0x80000000
#define CONFIG_SYS_SDRAM0_WRDTR		0x82000800
#define CONFIG_SYS_SDRAM0_SDTR1		0x80201000
#define CONFIG_SYS_SDRAM0_SDTR2		0x42104242
#define CONFIG_SYS_SDRAM0_SDTR3		0x0c100d20
#define CONFIG_SYS_SDRAM0_MMODE		0x00000642
#define CONFIG_SYS_SDRAM0_MEMODE	0x00000040

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

/* I2C DTT */
/* 
 * Beech has Temperature sensor but Maui cannot access it via I2C 
 * The sensor is connected with external I2C bus
 */
#if 0
#define CONFIG_DTT_ADM1021      1       /* ADM1021 temp sensor support  */
#define CONFIG_SYS_DTT_BUS_NUM          0       /* The I2C bus for DTT          */
#define CONFIG_DTT_SENSORS      { 0 }

/*
 * ADM1021 temp sensor configuration (see dtt/adm1021.c for details).
 * there will be one entry in this array for each two (dummy) sensors in
 * CONFIG_DTT_SENSORS.
 *
 * For Beech board:
 * - only one ADM1021
 * - i2c addr 0x18
 * - conversion rate 0x02 = 0.25 conversions/second
 * - ALERT ouput disabled
 * - local temp sensor enabled, min set to 0 deg, max set to 85 deg
 * - remote temp sensor enabled, min set to 0 deg, max set to 85 deg
 */
#define CONFIG_SYS_DTT_ADM1021          { { 0x4C, 0x02, 0, 1, 0, 85, 1, 0, 58} }
#endif

/* RTC configuration */
#define CONFIG_RTC_MC146818     1
#define CFG_DCR_RTC_ADDR        0x300

/*-----------------------------------------------------------------------
 * Ethernet
 *----------------------------------------------------------------------*/
#define CONFIG_IBM_EMAC4_V4	1

#define CONFIG_EMAC_PHY_MODE  EMAC_PHY_MODE_NONE_RGMII

#define CONFIG_HAS_ETH0
#define CONFIG_M88E1111_PHY
#define CONFIG_PHY_ADDR		0x1f	/* PHY address, See schematics	*/
#define CONFIG_PHY_RESET	1	/* reset phy upon startup	*/
#define CONFIG_PHY_RESET_R
#define CONFIG_PHY_GIGE		1	/* Include GbE speed/duplex detection */
#define CONFIG_PHY_DYNAMIC_ANEG	1
/*
 * FIXME: Workarround for wrong autonegotiation value in Beech board
 *        this should be remove later
 */
#define CONFIG_PHY_ANEG_BEECH_WORKARROUND       1

/*-----------------------------------------------------------------------
 * USB-OHCI
 *----------------------------------------------------------------------*/
/* Beech only has USB OTG */

/*
 * Default environment variables
 */

/* Can overwrite Ethernet MAC address */
#define CONFIG_ENV_OVERWRITE

#define CONFIG_EXTRA_ENV_SETTINGS					\
	CONFIG_AMCC_DEF_ENV						\
	CONFIG_AMCC_DEF_ENV_POWERPC					\
	CONFIG_AMCC_DEF_ENV_NOR_UPD					\
	CONFIG_AMCC_DEF_ENV_NAND_UPD					\
	"kernel_addr=fc000000\0"					\
	"fdt_addr=fc1e0000\0"						\
	"ramdisk_addr=fc200000\0"					\
	"pciconfighost=1\0"						\
	"pcie_mode=RP:RP\0"						\
	"ethact=ppc_4xx_eth0\0"						\
	"ethaddr=00:01:73:01:23:41\0"					\
	"boardtype=pcie\0"						\
	""

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
#define CONFIG_CMD_EXT2
#define CONFIG_CMD_FAT
#define CONFIG_CMD_NAND
#define CONFIG_CMD_PCI
#define CONFIG_CMD_SATA
#define CONFIG_CMD_SDRAM

/* Partitions */
#define CONFIG_MAC_PARTITION
#define CONFIG_DOS_PARTITION
#define CONFIG_ISO_PARTITION
#define CONFIG_EFI_PARTITION

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
#undef CONFIG_CMD_MII
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


/*-----------------------------------------------------------------------*/
/* CPLD register */
#define CONFIG_CPLD_I2C_ADDR	0x48
#define CONIFG_CPLD_BUS_NUM	0

//#define CONFIG_CPLD_DEBUG

#define CONFIG_CPLD_ID	0x00	/* CPLD ID */
#define CONFIG_CPLD_VER	0x01	/* CPLD version */
#define CONFIG_CPLD_BSR	0x02	/* Board Status Register */
#define CONFIG_CPLD_RSTCR	0x03	/* Reset Control Register */
#define CONFIG_CPLD_BCR	0x04	/* Board Control Register */
#define CONFIG_CPLD_IMR	0x05	/* Interrupt Mask Register */
#define CONFIG_CPLD_UTFR	0x06	/* User Test Function Register */
/* GPIO control */
#define CONFIG_CPLD_GPIO0_DIR	0x10
#define CONFIG_CPLD_GPIO1_DIR	0x11
#define CONFIG_CPLD_GPIO2_DIR	0x12
#define CONFIG_CPLD_GPIO3_DIR	0x13
#define CONFIG_CPLD_GPIO0_INP   0x14
#define CONFIG_CPLD_GPIO1_INP   0x15
#define CONFIG_CPLD_GPIO2_INP   0x16
#define CONFIG_CPLD_GPIO3_INP   0x17
#define CONFIG_CPLD_GPIO0_OUTP  0x18
#define CONFIG_CPLD_GPIO1_OUTP  0x19
#define CONFIG_CPLD_GPIO2_OUTP  0x1a
#define CONFIG_CPLD_GPIO3_OUTP	0x1b
/* Data register */
#define CONFIG_CPLD_DATA0	0x20
#define CONFIG_CPLD_DATA1	0x21
#define CONFIG_CPLD_DATA2	0x22
#define CONFIG_CPLD_DATA3	0x23

/* Register Field */
#define CONFIG_CPLD_BSR_BSTP	0xE0
#define CONFIG_CPLD_BCR_SATA0_EN	0x10
#define CONFIG_CPLD_RSTCR_GE0_RST	0xFE
#define CONFIG_CPLD_RSTCR_NORF_RST	0xF7
#define CONFIG_CPLD_RSTCR_MPRI0_EN	0x40
#define CONFIG_CPLD_RSTCR_MPRO0_EN	0x80
#define CONFIG_CPLD_RSTCR_PCIE0_CLR	0x10
#define CONFIG_CPLD_BCR_USBOTG_EN		0x08
#define CONFIG_CPLD_UTRF_USRINPUT(x)	(((x) & 0x38) >> 3)
#define CONFIG_CPLD_UTRF_USRINPUT_SRAM(x)	((x) & 0x1)
#define CONFIG_CPLD_UTRF_USRINPUT_SRAM(x)	((x) & 0x1)
#define CONFIG_CPLD_UTRF_USRINPUT_FULL_ADDR(x)	((x) & 0x2)


/* Misc. configuration */
#define CONFIG_BRINGUP_DEBUG
/*
 *#define NOT_SAVE_ENV: Not save environtment
 *#define NOT_USE_FLASH: Not use NOR FLASH
 *#define NOT_USE_NAND: Not use NAND FLASH
 *#define NOT_USE_PCIE: Not use PCIE
 */
#if defined(CONFIG_SRAM_U_BOOT)
#define NOT_SAVE_ENV
#define NOT_USE_FLASH
#define NOT_USE_PCIE
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
