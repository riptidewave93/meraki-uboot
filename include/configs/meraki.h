/*
 * Senao APM86xxx Cloud Managed Router board
 * Based on configuration for APM APM86xxx Ref board
 *
 * Copyright (c) 2010 Applied Micro Circuits Corporation.
 * All rights reserved. Feng Kan <fkan@apm.com>.
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
#ifndef __CONFIG_H
#define __CONFIG_H

/**
 * High Level Configuration Options
 */
#define CONFIG_4xx			1	/* ... PPC4xx family	*/
#define CONFIG_440			1	/* ... PPC440 family	*/
#define CONFIG_460			1	/* ... PPC460 family	*/
#define CONFIG_APM86XXX			1	/* APM SoC APM86xxx     */
#define CONFIG_BOARD_EARLY_INIT_F	1
#define CONFIG_MISC_INIT_F		1
#define CONFIG_BOARD_EARLY_INIT_R	1
#define CONFIG_PHYS_64BIT
/* Odd APM86XXX define needed */
#define CONFIG_HW_BRINGUP

#ifndef CONFIG_SYS_MASTER_CPUID
#define CONFIG_SYS_MASTER_CPUID		0
#define CPUID						0
#endif

#define CONFIG_MERAKI
/**
 * Include common defines/options for all AMCC boards
 */
#define CONFIG_HOSTNAME			meraki
#define CONFIG_IMGSUFFIX		""

#define CONFIG_SYS_CLK_FREQ		25000000   /* external freq to pll	*/

//#define CONFIG_SYS_DDR_FREQ		1333333333 /* 1333 MHz */
//#define CONFIG_SYS_DDR_FREQ		1066666666
#define CONFIG_SYS_DDR_FREQ		800000000
#undef CONFIG_SYS_EXT_SERIAL_CLOCK 	/* 50000000 */

#include "amcc-common.h"
#undef CONFIG_PPC4xx_EMAC		/* No IBM EMAC on APM86xxx */
#undef CONFIG_CMD_REGINFO		/* No 4xx register dump support */

#define CONFIG_MISC_INIT_R
#define CONFIG_APM86XXX_GPIO
/*****************************************************************************
 * Meraki HW Configuration
 *****************************************************************************/  
#define CONFIG_SYS_INIT_RAM_END		0x1000		/* End of used area in RAM */
#undef  CONFIG_SYS_HUSH_PARSER
#undef  CONFIG_BAUDRATE
#define CONFIG_BAUDRATE         	57600
#define CONFIG_HW_DDR
#define CONFIG_BOARD_RESET		/* Using custom reset function */
#undef  CONFIG_SYS_NO_FLASH
#define CONFIG_CMD_FLASH
#undef  CONFIG_CMD_SDRAM
#undef CONFIG_CMD_EEPROM		/* Meraki/Fullerene has no I2C? */
#define CONFIG_NAND_UFC
#define CONFIG_CMD_MII
#undef CONFIG_DW_SPI
#define CONFIG_CMD_IPP
#define CONFIG_PCI
#undef CONFIG_VIDEO			/* Enable video here if you need this */
#undef  CONFIG_CMD_IMLS
#define CONFIG_DBG_LINUX_PM
#define CONFIG_IPP_MESSAGING
#define CONFIG_NET_MULTI
#define CONFIG_CMD_PING
#define CONFIG_CMD_NET
#define CONFIG_APM86xxx_ENET
#define CONFIG_CMD_APM86xxx_ENET
#define CONFIG_CMD_QM
#define CONFIG_UDP_CHECKSUM
#undef CONFIG_CMD_NFS
#undef CONFIG_NETCONSOLE
#undef  CONFIG_SYS_RX_ETH_BUFFER
#define CONFIG_SYS_RX_ETH_BUFFER	1 /* We only need one buffer on TX */
#define CONFIG_NET_RETRY_COUNT 		5
#define CONFIG_NET_TIMEOUT		1000UL

#define CONFIG_CMD_SAVEENV 

/*-----------------------------------------------------------------------
* Ethernet
*----------------------------------------------------------------------*/
#define CONFIG_PHY_ADDR			0xff	/* PHY address, See schematics	*/
#define CONFIG_PHY1_ADDR		0xff	/* PHY address, See schematics	*/
#ifndef CONFIG_OCM_U_BOOT
#define CONFIG_ATHRS16_PHY
#endif
#define CONFIG_ETH_PHY_TX_RX_DELAY
/*#define RECONFIG_PLL1_500*/			//test set PLL1 (SOC 500MHz) when init 
/*#define RECONFIG_PLL1_1000*/
/*#define AXI_RECONFIGURE_125*/

#define CONFIG_HAS_ETH0
#define CONFIG_HAS_ETH1
//#define CONFIG_PHY_RESET	1	/* reset phy upon startup	*/
#define CONFIG_PHY_GIGE		1	/* Include GbE speed/duplex detection */
//#define CONFIG_PHY_DYNAMIC_ANEG			1
/***************************************************************************/
#if 0
/*	 port_number     port_name		  mode1		      speed           phy address       */
#define CONFIG_UDIAG_TEST_ETH_TABLE { \
	{    0      , "ppc_4xx_eth0" , 1, {10,100,0} , CONFIG_PHY_ADDR }, \
}

#define NUM_OF_EMACS		1
#define NUM_ACTIVE_CH		1
#define NUM_USED_CH			1
#define NUM_RX_PKT			64
#define NUM_TX_PKT			64
#define TRANSMIT_TIME		1
#define PKT_SIZE			1500
#define eth_port_enable		{1}
#endif
/****************************************************************************/


/*****************************************************************************
 * OCM U-Boot override 
 * OCM U-Boot means to be run from on-chip up to 256KB RAM 
 *****************************************************************************/  
#if defined(CONFIG_OCM_U_BOOT)
#undef CONFIG_CMD_MII
#undef CONFIG_CMD_IMLS
#undef CONFIG_PCI
#undef CONFIG_DBG_LINUX_PM
#undef CONFIG_IPP_MESSAGING
#undef CONFIG_DW_SPI
#undef CONFIG_CMD_IPP
#undef CONFIG_NET_MULTI
#undef CONFIG_CMD_PING
#undef CONFIG_CMD_NET
#undef CONFIG_APM86xxx_ENET
#undef CONFIG_CMD_APM86xxx_ENET
#undef CONFIG_CMD_QM
#undef CONFIG_UDP_CHECKSUM
#undef CONFIG_CMD_NFS
#undef CONFIG_NETCONSOLE 
#undef CONFIG_MISC_INIT_F
#undef CONFIG_SYS_MONITOR_LEN
#define CONFIG_SYS_MONITOR_LEN 		0x90000
#define CONFIG_SYS_NO_FLASH
#define CONFIG_ENV_IS_NOWHERE
#undef CONFIG_CMD_FLASH
#endif

/*****************************************************************************
 * Media SPL override - reduce feature to fit in available space
 *****************************************************************************/  
#if defined(CONFIG_MEDIA_SPL)
#undef CONFIG_MISC_INIT_F
#undef CONFIG_CMD_IPP
#endif

/*****************************************************************************
 * Media U-Boot override - reduce features to fit in available space
 *****************************************************************************/  
#if defined(CONFIG_MEDIA_U_BOOT)
#undef CONFIG_MISC_INIT_F
#undef CONFIG_SYS_MONITOR_LEN
#define CONFIG_SYS_MONITOR_LEN 		0x60000
#define CONFIG_SYS_NO_FLASH
#define CONFIG_ENV_IS_NOWHERE
#undef CONFIG_CMD_FLASH
#define CONFIG_APM_EXTRA_ENV_MEDIA	"pcie_mode=RC:RC:RC"
#else
#define CONFIG_APM_EXTRA_ENV_MEDIA	""
#endif

/*****************************************************************************
 * NAND SPL override - reduce features to fit in available space
 *****************************************************************************/  
#if defined(CONFIG_NAND_SPL)
#undef CONFIG_MISC_INIT_F
#undef CONFIG_CMD_IPP
#undef CONFIG_IPP_MESSAGING
#endif

/*****************************************************************************
 * NAND U-Boot override - reduce features to fit in available space 
 *****************************************************************************/  
#if defined(CONFIG_NAND_U_BOOT) 
#undef CONFIG_MISC_INIT_F
#undef CONFIG_SYS_MONITOR_LEN
#define CONFIG_SYS_MONITOR_LEN 		0x90000
#define CONFIG_SYS_NO_FLASH
//#define CONFIG_ENV_IS_NOWHERE
#undef CONFIG_CMD_FLASH
#endif

/*****************************************************************************
 * Power Management Debuging
 *****************************************************************************/  
#if defined(CONFIG_DBG_LINUX_PM)
#undef CONFIG_ADDMISC
#define CONFIG_ADDMISC	"addmisc=setenv bootargs ${bootargs} "	\
			"no_console_suspend loop.max_part=7\0"
#endif

/**
 * Base addresses -- Note these are effective addresses where the
 * actual resources get mapped (not physical addresses)
 */
#define CONFIG_SYS_PERIPHERAL_BASE	0xE0000000	/* peripherals	  */
#define CONFIG_SYS_EBC_BASE		(CONFIG_SYS_PERIPHERAL_BASE + 0x140000)	/* EBC base */
#define CONFIG_SYS_ISRAM_BASE		0x90000000	/* internal SRAM  */
#define CONFIG_SYS_MB_DDRC_BASE		0xefffc000	/* MQ & MEMC BASE */	
#define CONFIG_SYS_MB_BASE		(CONFIG_SYS_MB_DDRC_BASE + 0x1000)
#define CONFIG_SYS_DDRC_BASE		(CONFIG_SYS_MB_DDRC_BASE + 0x3800)
#define CONFIG_SYS_PRI_FABRIC_BASE	0xdf000000	/* Primary Fabric */	
#define CONFIG_SYS_CSR_BASE		0xdd800000	/* CSR */
#define CONFIG_SYS_SDU_BASE		(CONFIG_SYS_CSR_BASE + 0x10000)
#define CONFIG_SYS_SCU_BASE		(CONFIG_SYS_CSR_BASE + 0xB0000)
#define CONFIG_SYS_MPIC_BASE		0xdf400000	/* MPIC base */
#define CONFIG_SYS_ETH_CSR_BASE		(CONFIG_SYS_CSR_BASE + 0xA0000) /* ETH CSR */
#define CONFIG_SYS_CLE_CSR_BASE		(CONFIG_SYS_CSR_BASE + 0xE0000) /* CLE CSR */
#define CONFIG_SYS_SHAREDMEM_BASE	0x80000000

#define CONFIG_SYS_PHY_MB_BASE		0x3ffffd000ULL
#define CONFIG_SYS_PHY_CSR_BASE_HI	0x0D
#define CONFIG_SYS_QM_CSR_BASE		0xdd800000	/* QM CSR	  */
#define CONFIG_SYS_QM_FABRIC_BASE	0xdf000000	/* QM FABRIC	  */
#define CONFIG_SYS_DMA_CSR_BASE		(CONFIG_SYS_CSR_BASE + 0x20000)

/**
 * Deep Sleep Resume Register offsets
 */
#define CONFIG_PPC0_RESUME_DS_ADDR	0x68
#define CONFIG_PPC1_RESUME_DS_ADDR	0x6c

/**
 * Initial RAM & stack pointer (placed in internal SRAM)
 */
#define CONFIG_SYS_OCM_DATA_ADDR	CONFIG_SYS_ISRAM_BASE
#ifdef CONFIG_SYS_INIT_RAM_DCACHE
#define CONFIG_SYS_INIT_RAM_ADDR	0x70000000	/* Initial RAM address	*/
#else
#define CONFIG_SYS_INIT_RAM_ADDR	CONFIG_SYS_ISRAM_BASE	/* Initial RAM address	*/
#endif
#define CONFIG_SYS_GBL_DATA_SIZE	128		/* num bytes initial data */

#define CONFIG_SYS_GBL_DATA_OFFSET	(CONFIG_SYS_INIT_RAM_END - CONFIG_SYS_GBL_DATA_SIZE)
#define CONFIG_SYS_POST_WORD_ADDR	(CONFIG_SYS_GBL_DATA_OFFSET - 0x4)
#define CONFIG_SYS_INIT_SP_OFFSET	CONFIG_SYS_POST_WORD_ADDR

/*****************************************************************************
 * DDR SDRAM
 *****************************************************************************/  
#define CONFIG_DDR_32BIT		1	/* APM86391 uses 32BIT DDR3 */
#define CONFIG_DDR_DISCRETE
#define	CONFIG_VERY_BIG_RAM
#define CONFIG_MAX_MEM_MAPPED		0x80000000
#if !defined(CONFIG_NAND_SPL) && !defined(CONFIG_MEDIA_SPL)
#if defined(CONFIG_NAND_U_BOOT) || defined(CONFIG_DDR_U_BOOT)
# define CONFIG_FIXED_DDR_INIT 		1
# define CONFIG_NOT_COHERENT_CACHE  	1
#else
#if !defined(CONFIG_HW_REV_14) && !defined(CONFIG_HW_REV_15) 
# undef CONFIG_DDR_ECC				/* Don't Enable ECC support until the bug is fixed */
#ifndef CONFIG_FIXED_DDR_INIT
# define CONFIG_DDR_SPD			1	/* Enable SPD reading */
# define CONFIG_FIXED_DDR_INIT
# define REWORK_FIXED_INIT
# define HW_WRITE_LEVELING
# define HW_READ_GATE_LEVELING
# define HW_READ_EYE_TRAINING
# define CONFIG_APM86XXX_DDR_AUTOCALIBRATION	/* Enable DDR calibration */
#endif
#endif
#endif
#endif

#define CONFIG_SYS_MBYTES_2GB
#ifdef CONFIG_SYS_MBYTES_2GB
#define CONFIG_SYS_MBYTES_SDRAM		2048	/* MB of DDR */
#else
#define CONFIG_SYS_MBYTES_SDRAM		1024	/* MB of DDR */
#endif

#ifndef CONFIG_NOT_COHERENT_CACHE
#define CONFIG_4xx_DCACHE
#endif

#define MEMC_DQS2DQ_OFFSET 	0x40
#define PHY_RD_DQS_SLAVE_VAL	0x40

#define DDR_IOCTRL_ODTM	0x05

#ifdef CONFIG_SYS_MBYTES_2GB
#define DDRC0_ENRANKS	0x0F	/* 4 Ranks */
#else
#define DDRC0_ENRANKS	0x01    /* 1 Ranks */
#endif

#define DDRC1_VAL	0x44000030
#define DDRC5_VAL	0x2010200a
#define DDRC6_VAL	0x070e24ce
#define DDRC7_VAL	0x3203b4e4
#define DDRC8_VAL	0x0488324a
#define DDRC10_VAL	0x00986007
#define DDRC11_VAL	0x000000d0
#define DDRC12_VAL	0x02060040
#define DDRC18_VAL	0x02000100
#define DDRC18_1_VAL	0x08000400
#define DDRC19_VAL	0x00304007
#define DDRC23_VAL	0x00006023
#define PHY1_VAL	0x9500900f


#define DDRC0_RMODE	0x02
#define DDRC0_ODTBLK	0x00 
#define DDRC1_tREFI	0x51
#define DDRC5_tRFCMIN	0x28
#define DDRC5_tRC	0x14
#define DDRC6_tRASMIN	0x10
#define DDRC6_tRASMAX	0x1a
#define DDRC6_tFAWVAL	0x0e
#define DDRC6_WR2PRE	0x15
#define DDRC7_WRLAT	0x04
#define DDRC7_tRCD	0x08
#define DDRC7_tRTP	0x05
#define DDRC7_WR2RD	0x15
#define DDRC7_RD2WR	0x08
#define DDRC8_tRPMIN	0x09
#define DDRC8_tRRDMIN	0x06
#define DDRC10_PRECKE	0x03
#define DDRC11_EMR	0x00000010
#define DDRC12_MR	0x00000050
#define DDRC13_L2SIZZ	0x0c
#define DDRC19_RDDATA	0x00304008
#define DDRC33_RSTN	0x0f
#define PHY1_WRLODT	0x01
#define PHY1_RDLODT	0x00
#define PHY1_WE2REDLY	0x05
#define PHY1_USE_F_RE	0x01
#define PHY2_WRRL	0x05
#define PHY2_DLYSEL	0x01
/* 64/32bit related */
#define DDRC0_DQWDTH	0x01
#define PHY1_DSLICE	0x0f
#define DDRC15_RADDR0	0x11
#define DDRC15_RADDR1	0x11
#define DDRC15_BADDR0	0x06
#define DDRC15_BADDR1	0x06
#define DDRC15_BADDR2	0x06
#define DDRC16_COLADDRS 0xffff0000
#define DDRC17_ROWADDRS 0x0f222222


/* Datasheet memory values */
#define DDR_DSVAL_CWL		5*100	/* CAS Write Latency */
#define DDR_DSVAL_AL		0*100	/* Additive Latency (disabled) */
#define DDR_DSVAL_CL		5*100	/* CAS Latency */
#define DDR_DSVAL_BURSTLEN	4*100	/* Burst Length */
#define DDR_DSVAL_tREFI		7.8*100	/* Nominal Refresh Cycle Time */
#define DDR_DSVAL_tRFC_MIN	160*100	/* Minimum Refresh Cycle Time */
#define DDR_DSVAL_tRC_MIN	50*100	/* Row Cycle Time */
#define DDR_DSVAL_tRAS_MIN	37.5*100	/* Min Bank Active Time */
#define DDR_DSVAL_tFAW		40*100	/* Four Active Window */
#define DDR_DSVAL_tWR		15*100	/* */
#define DDR_DSVAL_tRCD		12.5*100	/* RAS-to-CAS Delay */
#define DDR_DSVAL_tRTP		4*100	/* Read-to-Precharge Time */
#define DDR_DSVAL_tWTR		4*100	/* */
#define DDR_DSVAL_tRP		12.5*100	/* Row Prechare Command Period */
#define DDR_DSVAL_tRRD		10*100	/* RAS-to-RAS Delay */


/*****************************************************************************
 * I2C 
 *****************************************************************************/  
#ifdef CONFIG_CMD_I2C
#define CONFIG_DW_I2C
#define CONFIG_DW_I2C_TX_BUFFER_DEPTH	4
#define CONFIG_DW_I2C_RX_BUFFER_DEPTH	4
#define CONFIG_I2C_MULTI_BUS    1
#define CONFIG_SYS_I2C_EEPROM_ADDR	0x51
#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN	1	/* Bytes of address	*/
#define CONFIG_SYS_I2C_EEPROM_PAGE_WRITE_LEN	8
#define CONFIG_SYS_I2C_SPEED	100000	/* I2C speed */
#undef CONFIG_SYS_I2C_SLAVE
#define CONFIG_SYS_I2C_SLAVE	CONFIG_SYS_I2C_EEPROM_ADDR
#define CONFIG_SYS_I2C1_EEPROM_ADDR	0x52
#define CONFIG_SYS_I2C1_EEPROM_ADDR_LEN	2	/* Bytes of address	*/
#define CONFIG_SYS_I2C1_EEPROM_PAGE_WRITE_LEN	64
#define CONFIG_SYS_I2C1_SPEED	400000	/* I2C speed */
#define IIC0_BOOTPROM_ADDR	0x50
#define IIC0_ALT_BOOTPROM_ADDR	0x54
/* Don't probe these addrs */
#define CONFIG_SYS_I2C_NOPROBES	{ {0, 0x50}, {0, 0x52}, {0, 0x53}, {0, 0x54}}
#endif /* CONFIG_CMD_I2C */

#undef CONFIG_HOSTNAME
#define CONFIG_SYS_HOSTNAME    fuller
#define CONFIG_SYS_PLATFORM    fuller

#undef CONFIG_BOOTCOMMAND
#undef CONFIG_AMCC_DEF_ENV
#undef CONFIG_AMCC_DEF_ENV_POWERPC
#undef CONFIG_AMCC_DEF_ENV_NOR_UPD
#undef CONFIG_AMCC_DEF_ENV_NAND_UPD

#define MERAKI_PANIC_PART 0x00200000
#define MERAKI_PANIC_SIZE 0x00040000

#define MERAKI_UBI_PART 0x00240000
#define MERAKI_UBI_SIZE 0x3fdc0000
/**
 * Environment
 */
#undef	CONFIG_ENV_IS_IN_NVRAM		/* ... not in NVRAM		*/
#undef	CONFIG_ENV_IS_IN_EEPROM		/* ... not in EEPROM		*/

#undef CONFIG_PREBOOT
#define CONFIG_PREBOOT "echo;echo Set serverpath and run meraki_netboot to netboot;echo"

#undef CONFIG_BOOTDELAY
#define CONFIG_BOOTDELAY 1

#define CONFIG_SYS_NAND_U_BOOT_PARTITION 0x200000

#define CONFIG_AMCC_DEF_ENV_NAND_UPD                                    \
        "nload=${netloadmethod} 200000 ${serverpath}u-boot-nand.bin\0" \
	"nupdate=nand erase 0 " xstr(CONFIG_SYS_NAND_U_BOOT_SIZE)" ;nand write 200000 0 " xstr(CONFIG_SYS_NAND_U_BOOT_SIZE) "\0" \
        "nupd=run nload nupdate\0"

#define CONFIG_MERAKI_ENV_SETTINGS					\
	"bootcmd=run meraki_fix_leds meraki_boot\0"					\
	"meraki_bootargs=setenv bootargs root=/dev/ram console=ttyS0,${baudrate} ubi.mtd=ubi MERAKI_BOARD=fullerene mtdoops.mtddev=oops mem=0x7f000000 ${extra_bootargs}\0" \
	"meraki_bootfile=fuller.bin\0"					\
	"meraki_bootlinux=bootm ${meraki_loadaddr_kernel} ${meraki_loadaddr_ramdisk} ${meraki_loadaddr_fdt}\0"	\
	"meraki_boot=run meraki_ubi meraki_bootargs ; run meraki_load1 meraki_checkpart meraki_bootlinux; " \
	"run meraki_load2 meraki_checkpart2 meraki_bootlinux\0"				\
	"meraki_checkpart=meraki checkpart ${meraki_loadaddr}\0"	\
	"meraki_checkpart2=meraki checkpart ${meraki_loadaddr} skiphash\0"	\
	"meraki_netboot=run meraki_load_net meraki_checkpart2 meraki_bootargs meraki_bootlinux\0" \
        "meraki_loadaddr=800000\0"					\
	"meraki_part_fdt_index=1\0"					\
	"meraki_load1=ubi read ${meraki_loadaddr} part1\0"		\
	"meraki_load2=ubi read ${meraki_loadaddr} part2\0"		\
	"meraki_load_net=${netloadmethod} ${meraki_loadaddr} ${serverpath}${meraki_bootfile}\0" \
	"meraki_ubi=ubi part ubi\0"					\
	"meraki_ubifile=fuller-ubi.bin\0"					\
	"meraki_ubi_loadfile=${netloadmethod} 200000 ${serverpath}${meraki_ubifile}\0" \
	"meraki_ubi_update_nand=nand erase " xstr(MERAKI_UBI_PART) " " xstr(MERAKI_UBI_SIZE) " ; " \
        "nand write 200000 " xstr(MERAKI_UBI_PART) " ${filesize}\0"	\
	"meraki_ubi_update=run meraki_ubi_loadfile meraki_ubi_update_nand\0" \
	"meraki_update_part1=run meraki_ubi meraki_load_net meraki_write1\0"	\
	"meraki_update_part2=run meraki_ubi meraki_load_net meraki_write2\0"	\
	"meraki_write1=ubi write ${meraki_loadaddr} part1 ${filesize}\0" \
	"meraki_write2=ubi write ${meraki_loadaddr} part2 ${filesize}\0" \
	"meraki_fix_leds=i2c mw 22 c ff 3\0" \
	"mtdids=nand0=nand0\0"						\
	"mtdparts=mtdparts=nand0:" xstr(CONFIG_SYS_NAND_U_BOOT_PARTITION) "@0(firmware)," \
         xstr(MERAKI_PANIC_SIZE) "@" xstr(MERAKI_PANIC_PART)"(panic)," \
	 xstr(MERAKI_UBI_SIZE) "@" xstr(MERAKI_UBI_PART)"(ubi)\0"	\

#define CONFIG_OVERWRITE_ETHADDR_ANYTIME
#define	CONFIG_EXTRA_ENV_SETTINGS					\
	CONFIG_AMCC_DEF_ENV_NAND_UPD					\
	"kernel_addr=fc000000\0"					\
	"fdt_addr=fc1e0000\0"						\
	"ramdisk_addr=fc200000\0"					\
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
	"factory_load=setenv ipaddr 192.168.1.1; setenv serverip 192.168.1.101; tftp 0x100000 uDiag-fullerene-ddr-nand.bin\0"	\
	"factory_go=go 0x102100\0"						\
	"factory_boot=run factory_load factory_go\0"				\
	"console="  xstr(CONFIG_USE_TTY) ",${baudrate}\0"

/**
 * Commands in addition to amcc-common.h
 */
#undef CONFIG_WATCHDOG			/* watchdog disabled		*/

/*****************************************************************************
 * PPCx upper 4-bit physical address mapping 
 *****************************************************************************/  
#if defined(CONFIG_MEDIA_SPL)
# define CONFIG_SYS_BOOT_BASE_ERPN	0xE	/* OCM */
#else
# define CONFIG_SYS_BOOT_BASE_ERPN	0xF	/* AHBC */
#endif

/*****************************************************************************
 * FLASH related
 *****************************************************************************/  
#define CONFIG_SYS_BOOT_BASE_ADDR	0xFF000000
#define CONFIG_SYS_FLASH_BASE           0xFC000000
#define CONFIG_SYS_FLASH_SIZE           (64 << 20)

/*****************************************************************************
 * NOR Configuration 
 *****************************************************************************/  
#if defined(CONFIG_CMD_FLASH)
#define CONFIG_SAVE_FLASH_INFO		/* Save flash no double probing */
#define CONFIG_SYS_FLASH_CFI		/* The flash is CFI compatible	*/
#define CONFIG_FLASH_CFI_DRIVER		/* Use common CFI driver	*/
#if !defined(CONFIG_HW_REV_11) && !defined(CONFIG_HW_REV_14) && !defined(CONFIG_HW_REV_15)
# define CONFIG_SYS_WRITE_SWAPPED_DATA
#endif
#define CONFIG_SYS_FLASH_CFI_WIDTH FLASH_CFI_16BIT
#ifndef CONFIG_ISS
# define CONFIG_FLASH_SPANSION_S29WS_N
# define CONFIG_SYS_FLASH_USE_BUFFER_WRITE 1    /* use buffered writes (20x faster)     */
#endif
#define CONFIG_SYS_FLASH_BANKS_LIST    {CONFIG_SYS_FLASH_BASE}
#define CONFIG_SYS_MAX_FLASH_BANKS     1       /* max number of memory banks           */
#define CONFIG_SYS_MAX_FLASH_SECT      518     /* max number of sectors on one chip    */
#define CONFIG_SYS_FLASH_PROTECTION    1       /* use hardware flash protection        */
#undef	CONFIG_SYS_FLASH_CHECKSUM
#endif /* CONFIG_CMD_FLASH */

/*****************************************************************************
 * NAND Configuration
 *****************************************************************************/  
#if defined(CONFIG_NAND_UFC) || \
    defined(CONFIG_NAND_U_BOOT) || \
    defined(CONFIG_NAND_SPL)

#define CONFIG_ECCBCH			/* Include SW ECC algorithms */
#define CONFIG_ECCBCH_RAM		CONFIG_SYS_ISRAM_BASE 					
#define CONFIG_CMD_NAND
#define CONFIG_CMD_MTDPARTS
#define CONFIG_MTD_DEVICE
#define CONFIG_CMD_UBI
#define CONFIG_RBTREE		// Needed for UBI support

#undef CONFIG_MAC_PARTITION
#undef CONFIG_DOS_PARTITION
#undef CONFIG_ISO_PARTITION
#undef CONFIG_EFI_PARTITION
#define CONFIG_MTD_PARTITIONS

#define CONFIG_SYS_MAX_NAND_DEVICE      1
#define CONFIG_SYS_NAND_BASE            CONFIG_SYS_EBC_BASE
#define CONFIG_SYS_NAND_BAD_BLOCK_POS   0	/* Location of bad block marker */
#define CONFIG_SYS_NAND_5_ADDR_CYCLE    1	/* Fourth addr used (>32MB)     */
#define CONFIG_NAND_COL_ADDR_CYCLE	2
#define CONFIG_NAND_ROW_ADDR_CYCLE	3
#if !defined(CONFIG_NAND_U_BOOT) && !defined(CONFIG_NAND_SPL) && \
    !defined(CONFIG_OCM_U_BOOT)
/* Non-NAND Booting with NAND support */
#if !defined(CONFIG_ISS) && !defined(CONFIG_ENV_IS_NOWHERE)
# define CONFIG_ENV_IS_IN_FLASH  	1	/* use FLASH for environment vars */
#endif
#define CONFIG_SYS_NAND_CS              0       /* NAND chip connected to CSx */

#else
/* NAND Booting */
#if !defined(CONFIG_ISS) && !defined(CONFIG_ENV_IS_NOWHERE)
#define CONFIG_ENV_IS_IN_NAND   	1       /* use NAND for environment vars  */
#endif
#if !defined(CONFIG_NAND_U_BOOT) && !defined(CONFIG_OCM_U_BOOT)
#define CONFIG_ENV_IS_EMBEDDED  	1       /* use embedded environment */
#endif
#define CONFIG_SYS_NAND_CS              0       /* NAND chip connected to CSx */

#if defined(CONFIG_NAND_SPL)
# if defined(CONFIG_NAND_SPL_S1)
#  define CONFIG_SYS_NAND_BOOT_SPL_SRC   0xFFFFF000      /* 4k SPL location */
#  define CONFIG_SYS_NAND_BOOT_SPL_SIZE  (4 << 10)       /* SPL size */
#  define CONFIG_SYS_NAND_BOOT_SPL_DST   0x90001000     /* Copy 4k SPL here */
# elif defined(CONFIG_NAND_SPL_S2) /* !defined(CONFIG_NAND_SPL_S1) */
#  define CONFIG_SYS_NAND_BOOT_SPL_SRC   0x90002000 /* 20k Stage-2 SPL location */
#  define CONFIG_SYS_NAND_BOOT_SPL_SIZE  (20 << 10) /* 20k Stage-2 SPL size */
#  define CONFIG_SYS_NAND_BOOT_SPL_DST   (0x20000000) /* Copy 20k SPL here */

/* SW BCH ECC Related Settings */
#  define CONFIG_SW_BCH_ECC_BYTES 7
#  define CONFIG_SW_BCH_ECC_SIZE 512
#  define CONFIG_SW_BCH_ECC_STEPS 4

#  define CONFIG_SYS_NAND_ECCBYTES CONFIG_SW_BCH_ECC_BYTES
#  define CONFIG_SYS_NAND_ECCSIZE CONFIG_SW_BCH_ECC_SIZE
#  define CONFIG_SYS_NAND_ECCSTEPS CONFIG_SW_BCH_ECC_STEPS
#  define CONFIG_SYS_NAND_ECCPOS { \
	32, 33, 34, 35, 36, 37, 38, \
	40, 41, 42, 43, 44, 45, 46, \
	48, 49, 50, 51, 52, 53, 54, \
	56, 57, 58, 59, 60, 61, 62 }

# endif /* !defined(CONFIG_NAND_SPL_S2) */
#endif /* defined(CONFIG_NAND_SPL) */

#if defined(CONFIG_NAND_SPL_S1)
# define CONFIG_SYS_NAND_U_BOOT_DST      (CONFIG_SYS_ISRAM_BASE + 0x2000)      /* Load Stage-2 to this addr */
# define CONFIG_SYS_NAND_U_BOOT_START    (CONFIG_SYS_ISRAM_BASE + 0x6FFC) /* Start Stage-2 spl from this addr     */
# define CONFIG_SYS_NAND_U_BOOT_OFFS     (4 << 10)
# define CONFIG_SYS_NAND_U_BOOT_SIZE     (20 << 10)
#else /* !defined(CONFIG_NAND_SPL_S1) */
# define CONFIG_SYS_NAND_U_BOOT_DST      0x01001000      /* Load NUB to this addr */
# define CONFIG_SYS_NAND_U_BOOT_START    (CONFIG_SYS_NAND_U_BOOT_DST)/* Start NUB from this addr     */
# define CONFIG_SYS_NAND_U_BOOT_OFFS     ((4 + 20) << 10)
# define CONFIG_SYS_NAND_U_BOOT_SIZE     0x00100000
#endif /* !defined(CONFIG_NAND_SPL_S1) */

/*
 * Now the NAND chip has to be defined (no autodetection used for NAND SPL)
 */
#define CONFIG_SYS_NAND_PAGE_SIZE       (2 << 10)       /* NAND chip page size          */
#define CONFIG_SYS_NAND_BLOCK_SIZE      (128 << 10)     /* NAND chip block size         */
#define CONFIG_SYS_NAND_PAGE_COUNT      64              /* NAND chip page count         */
#define CONFIG_SYS_NAND_BAD_BLOCK_POS   0               /* Location of bad block marker */
#define CONFIG_SYS_NAND_OOBSIZE			64

#endif /* !defined(CONFIG_NAND_U_BOOT) && !defined(CONFIG_NAND_SPL) */
#endif /* CONFIG_NAND_UFC */

/*****************************************************************************
 * ENV in NOR Flash 
 *****************************************************************************/  
#ifdef CONFIG_ENV_IS_IN_FLASH
#define CONFIG_ENV_SECT_SIZE	0x20000	/* size of one complete sector	*/
#define CONFIG_ENV_ADDR		(CONFIG_SYS_MONITOR_BASE - CONFIG_ENV_SIZE)
#endif /* CONFIG_ENV_IS_IN_FLASH */
#define CONFIG_ENV_SIZE		0x20000	/* Size of Environment vars	*/

#ifdef CONFIG_SAVE_FLASH_INFO
#define CONFIG_ENV_SECT_SIZE	0x20000	/* Size of Environment vars	*/
#define CONFIG_FINFO_SECT_SIZE	CONFIG_ENV_SECT_SIZE /* size of one complete sector	*/
#define CONFIG_FINFO_ADDR 	(CONFIG_ENV_ADDR - CONFIG_ENV_SECT_SIZE)	
#endif

/*****************************************************************************
 * ENV in NAND Flash
 *****************************************************************************/  
#if defined(CONFIG_ENV_IS_IN_NAND)
#undef CONFIG_ENV_SIZE
/*
 * The environment size is one NAND block (128K on Fullerene). We allocate four
 * NAND blocks to deal with bad blocks which may exist in the saved
 * environment.
 */
#define CONFIG_ENV_SIZE		CONFIG_SYS_NAND_BLOCK_SIZE
#define CONFIG_ENV_RANGE	(CONFIG_ENV_SIZE * 4)

#define CONFIG_ENV_OFFSET 0x100000
#define CONFIG_ENV_OFFSET_REDUND (CONFIG_ENV_OFFSET + CONFIG_ENV_RANGE)
#endif

/*****************************************************************************
 * SPI Related
 *****************************************************************************/  
#if defined(CONFIG_DW_SPI)
#define	CONFIG_HARD_SPI			1
#define	CONFIG_CMD_SF
#define	CONFIG_SPI_FLASH		
#define	CONFIG_SPI_FLASH_MICRON		1
#define CONFIG_SF_DEFAULT_SPEED		4000000
#define CONFIG_ENV_SPI_MAX_HZ		4000000
#define CONFIG_DW_SPI
#define CONFIG_DW_SPI_BASE		(CONFIG_SYS_PERIPHERAL_BASE + 0x5000)
#define CONFIG_DW_SPI_MAX_CS		4
#define CONFIG_DW_SPI_FIFO_DEPTH	8
#define CONFIG_DW_SPI_SYSCLK		100000000
#define CONFIG_DW_SPI_VER_ID		0x3331352a
#endif

/*****************************************************************************
 * Video 
 *****************************************************************************/  
#ifdef CONFIG_VIDEO
#define CONFIG_VIDEO_APMDB9000
#define CONFIG_VIDEO_SW_CURSOR
#define CONFIG_VIDEO_LOGO
#define CONFIG_CFB_CONSOLE
#define CONFIG_VGA_AS_SINGLE_DEVICE
#define CONFIG_SPLASH_SCREEN
#define CONFIG_CMD_BMP
#define VIDEO_COLS_MAX		1280
#define VIDEO_ROWS_MAX		1024
#define VIDEO_PIXEL_SIZE_MAX	4
#endif

/*****************************************************************************
 * PCIE 
 *****************************************************************************/  
#ifdef CONFIG_PCI

#define CONFIG_CMD_PCI
#define CONFIG_PCIE_MAX_PORTS		3
#define CONFIG_PCI_PNP			/* do pci plug-and-play		*/
#define CONFIG_PCI_SCAN_SHOW		/* show pci devices on startup	*/
#define CONFIG_PCI_CONFIG_HOST_BRIDGE
#define CONFIG_SYS_PCI_64BIT      1     /* enable 64-bit PCI resources */

/* Board-specific PCI */
#define CONFIG_SYS_PCI_TARGET_INIT	/* let board init pci target    */
#undef	CONFIG_SYS_PCI_MASTER_INIT

#define CONFIG_SYS_PCI_SUBSYS_VENDORID 0x1014	/* IBM			*/
#define CONFIG_SYS_PCI_SUBSYS_DEVICEID 0xcafe	/* Whatever		*/

/**
 * PCIE Base addresses -- Note these are effective addresses where the
 * actual resources get mapped (not physical addresses). If you changes
 * these, update init.S tlb entry as well. Current, each TLB mapping of
 * 256MB for memory and 16MB for configuration space (TLB size). For 
 * the PCIE2, we map on 128 as there isn't enough remain.
 *
 * NOTE: As PCIE1 is shared with SATA, if you do not want to use PCIE1,
 * set CONFIG_SYS_PCIEX_CFGBASE to 0.
 */
#define CONFIG_SYS_PCIE0_MEMBASE 0xA0000000 /* mapped PCIe memory */
#define CONFIG_SYS_PCIE0_MEMSIZE 0x20000000
#define CONFIG_SYS_PCIE1_MEMBASE 0xB0000000 /* mapped PCIe memory */
#define CONFIG_SYS_PCIE1_MEMSIZE 0x20000000
#define CONFIG_SYS_PCIE2_MEMBASE 0xC0000000 /* mapped PCIe memory */
#define CONFIG_SYS_PCIE2_MEMSIZE 0x10000000

#define CONFIG_SYS_PCIEX_CFGSIZE 0x01000000 /* 16 MB configuration space */
#define CONFIG_SYS_PCIE0_CFGBASE (0xD0000000 + 0x00000000)
#define CONFIG_SYS_PCIE1_CFGBASE (0xD0000000 + 0x01000000)
#define CONFIG_SYS_PCIE2_CFGBASE (0xD0000000 + 0x02000000)

/* base address of inbound PCIe window */
#define CONFIG_SYS_PCIE_INBOUND_BASE 0x000000000ULL /* 36bit physical addr */

/* Physical Mapping Address */
#define CONFIG_SYS_PCIE0_PHYMEMBASE_HI	0x06
#define CONFIG_SYS_PCIE0_PHYMEMBASE_LO	0x00000000
#define CONFIG_SYS_PCIE1_PHYMEMBASE_HI	0x07
#define CONFIG_SYS_PCIE1_PHYMEMBASE_LO	0x00000000
#define CONFIG_SYS_PCIE2_PHYMEMBASE_HI	0x0A
#define CONFIG_SYS_PCIE2_PHYMEMBASE_LO	0x00000000

#endif /* CONFIG_PCI */

/*****************************************************************************
 * Register offsets and masks for controlling PPC1 reset release (for AMP) 
 *****************************************************************************/
#ifdef CONFIG_MP
#define CONFIG_SYS_PPC_RESET_CTRL_ADDR		(CONFIG_SYS_SCU_BASE+0x6c)
#define CONFIG_SYS_PPC1_RESET_RELEASE_MASK	(0x20000000)
#endif

#endif	/* __CONFIG_H */
