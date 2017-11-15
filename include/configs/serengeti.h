/*
 * Configuration for APM APM86xxx Ref board
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

#ifndef CONFIG_SYS_MASTER_CPUID
#define CONFIG_SYS_MASTER_CPUID		0
#endif

/**
 * Include common defines/options for all AMCC boards
 */
#define CONFIG_HOSTNAME			serengeti

#define CONFIG_SYS_CLK_FREQ		25000000   /* external freq to pll	*/
#define CONFIG_SYS_DDR_FREQ		1333333333 /* 1600 MHz */	
#undef CONFIG_SYS_EXT_SERIAL_CLOCK 	/* 50000000 */

#include "amcc-common.h"
#undef CONFIG_PPC4xx_EMAC		/* No IBM EMAC on Serengeti */
#undef CONFIG_CMD_REGINFO		/* No 4xx register dump support */

/*****************************************************************************
 * VHP Configuration
 *****************************************************************************/  
#ifdef CONFIG_ISS
#undef CONFIG_CMD_MII
#undef CONFIG_CMD_EEPROM
#undef CONFIG_NETCONSOLE 
#undef CONFIG_CMD_IMLS
#define CONFIG_NET_MULTI
#define CONFIG_SYS_NO_FLASH
#define CONFIG_PCI 			/* include pci support		*/
#define CONFIG_CMD_QM
#define CONFIG_SYS_TEMP_STACK_OCM	1
#define CONFIG_SYS_INIT_RAM_END		0x2000		/* End of used area in RAM */
#define CONFIG_APM86xxx_ENET
#endif /* CONFIG_ISS */

/*****************************************************************************
 * Serengeti HW Configuration
 *****************************************************************************/  
#if defined(CONFIG_HW_BRINGUP) 
#define CONFIG_SYS_INIT_RAM_END		0x1000		/* End of used area in RAM */
#undef  CONFIG_SYS_HUSH_PARSER
#undef  CONFIG_BAUDRATE
#define CONFIG_BAUDRATE         	57600
#define CONFIG_HW_DDR
#define CONFIG_BOARD_RESET		/* Using custom reset function */
#undef  CONFIG_SYS_NO_FLASH
#define CONFIG_CMD_FLASH
#undef  CONFIG_CMD_SDRAM
#define CONFIG_CMD_EEPROM
#define CONFIG_NAND_UFC
#define CONFIG_CMD_MII
#define CONFIG_DW_SPI
#define CONFIG_CMD_IPP
#define CONFIG_CMD_I2C
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
#endif

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
#define CONFIG_ENV_IS_NOWHERE
#undef CONFIG_CMD_FLASH
#endif

/*****************************************************************************
 * Reduce malloc size when no DDR RAM (due to reduce memory available)
 *****************************************************************************/  
#if !defined(CONFIG_HW_DDR) && !defined(CONFIG_ISS)
#undef CONFIG_SYS_MALLOC_LEN
#define CONFIG_SYS_INIT_RAM_DCACHE
#define CONFIG_SYS_MALLOC_LEN 		0x4000
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
#define CONFIG_SYS_DDRC_BASE		(CONFIG_SYS_MB_DDRC_BASE + 0x3800)
#define CONFIG_SYS_PRI_FABRIC_BASE	0xdf000000	/* Primary Fabric */	
#define CONFIG_SYS_CSR_BASE		0xdd800000	/* CSR */
#define CONFIG_SYS_SDU_BASE		(CONFIG_SYS_CSR_BASE + 0x10000)
#define CONFIG_SYS_SCU_BASE		(CONFIG_SYS_CSR_BASE + 0xB0000)
#define CONFIG_SYS_MPIC_BASE		0xdf400000	/* MPIC base */
#define CONFIG_SYS_ETH_CSR_BASE		(CONFIG_SYS_CSR_BASE + 0xA0000) /* ETH CSR */
#define CONFIG_SYS_CLE_CSR_BASE		(CONFIG_SYS_CSR_BASE + 0xE0000) /* CLE CSR */
#define CONFIG_SYS_SHAREDMEM_BASE	0x80000000

#define CONFIG_SYS_PHY_CSR_BASE_HI	0x0D
#define CONFIG_SYS_QM_CSR_BASE		0xdd800000	/* QM CSR	  */
#define CONFIG_SYS_QM_FABRIC_BASE	0xdf000000	/* QM FABRIC	  */
#define CONFIG_SYS_DMA_CSR_BASE		(CONFIG_SYS_CSR_BASE + 0x20000)

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
#if !defined(CONFIG_NAND_SPL) && !defined(CONFIG_MEDIA_SPL) && !defined(CONFIG_MAMBA_WAP)
#if defined(CONFIG_NAND_U_BOOT)
# define CONFIG_FIXED_DDR_INIT 		1
# define CONFIG_NOT_COHERENT_CACHE  	1
#else
#ifndef CONFIG_HW_REV_14
//# define CONFIG_SYS_SIMULATE_SPD_EEPROM		/* Utilize a fake SPD  */
# define CONFIG_DDR_SPD			1	/* Enable SPD reading */
# undef CONFIG_DDR_ECC				/* Don't Enable ECC support until the bug is fixed */
# define CONFIG_APM86XXX_DDR_AUTOCALIBRATION	/* Enable DDR calibration */
//#define CONFIG_FIXED_DDR_INIT
#endif
#endif
#endif
#define CONFIG_SYS_SPD_MAX_DIMMS	1
#ifndef CONFIG_SYS_MBYTES_SDRAM
#define CONFIG_SYS_MBYTES_SDRAM		1024	/* MB of DDR */
#endif

#ifndef CONFIG_NOT_COHERENT_CACHE
#define CONFIG_4xx_DCACHE
#endif

/* SPD i2c spd addresses */
#define IIC0_DIMM0_ADDR		0x57
#define SPD_EEPROM_ADDRESS	{IIC0_DIMM0_ADDR}


#define MEMC_DQS2DQ_OFFSET 	0x40
#define PHY_RD_DQS_SLAVE_VAL 	0x40
#if defined(CONFIG_MAMBA_WAP)
#define DDRC1_tREFI		0x28
#define DDRC5_tRFCMIN		0x26
#define DDRC5_tRC		0x12
#define DDRC6_tRASMIN		0x00
#define DDRC6_tRASMAX		0x20
#define DDRC6_tFAWVAL		0x3b
#define DDRC6_WR2PRE		0x12
#define DDRC7_WRLAT		0x04
#define DDRC7_tRCD		0x05
#define DDRC7_tRTP		0x06
#define DDRC7_WR2RD		0x0e
#define DDRC7_RD2WR		0x09
#define DDRC8_tRPMIN		0x04
#define DDRC8_tRRDMIN		0x04
#define DDRC11_EMR		0x00000010
#define DDRC10_PRECKE		0x186
#define DDRC12_MR		0x00000050
#define DDRC19_RDDATA		0x00304009
#define PHY1_RDLODT		0x03
#define PHY1_WE2REDLY		0x05
#define PHY2_WRRL		0x0f

#else

#endif

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

/**
 * Environment
 */
#undef	CONFIG_ENV_IS_IN_NVRAM		/* ... not in NVRAM		*/
#undef	CONFIG_ENV_IS_IN_EEPROM		/* ... not in EEPROM		*/

#define CONFIG_PREBOOT	"echo;"	\
	"echo Type \"run flash_nfs\" to mount root filesystem over NFS;" \
	"echo"

#undef	CONFIG_BOOTARGS

/**
 * For AMP support, do the following:
 * 1. Add "AMP=0" to ENV variable
 * 2. Add "cpu 1 reset" to CONFIG_BOOTCOMMAND or 
 *    Add "bootcmd=cpu 1 reset; run mtd_self" to ENV variable
 */

/**
 * BOOTCOMMAND 
 */
#undef CONFIG_BOOTCOMMAND

#if defined(CONFIG_MEDIA_U_BOOT)
#define CONFIG_BOOTCOMMAND	"ipp eread ${bootfile} ${kernel_addr_r}; " \
				"ipp eread ${fdt_file} ${fdt_addr_r}; " \
				"ipp eread ${ramdisk_file} ${ramdisk_addr_r}; " \
				"run ramargs addip addtty; " \
				"bootm ${kernel_addr_r} ${ramdisk_addr_r} ${fdt_addr_r}"
#define CONFIG_BOOTCOMMAND1	"bootcmd1=run sd_mtdself1"
#elif defined(CONFIG_NAND_U_BOOT)
#define CONFIG_BOOTCOMMAND	"nand read ${kernel_addr_r} ${nkernel_up_addr} 0x300000; " \
				"nand read ${fdt_addr_r} ${nfdt_addrup} 0x20000; " \
				"nand read ${ramdisk_addr_r} ${nramdisk_addr_up} 0xF00000; " \
				"run ramargs addip addtty; " \
				"bootm ${kernel_addr_r} ${ramdisk_addr_r} ${fdt_addr_r}"
#define CONFIG_BOOTCOMMAND1	"bootcmd1=run sd_mtdself1"
#else
#define CONFIG_BOOTCOMMAND	"run flash_self"
#define CONFIG_BOOTCOMMAND1	"bootcmd1=run flash_amp1\0"
#undef CONFIG_CMD_IPP_PWRMGMT
#endif /* #if defined(CONFIG_MEDIA_U_BOOT) */

#define CONFIG_BOOTCOMMAND0	"bootcmd0=cpu 1 release 0;run flash_amp0\0"

#ifdef CONFIG_CMD_IPP
#define CONFIG_AMCC_DEF_ENV_IPP "ipp_addr_r=0x900000\0" \
		"ippfile=ipp_run.bin\0" \
		"net_ipp_load=tftp ${ipp_addr_r} ${user_dir}/${ippfile}; " \
		"ipp fwload ${ipp_addr_r}\0"
#else
#define CONFIG_AMCC_DEF_ENV_IPP ""
#endif

#ifdef CONFIG_VIDEO
#define CONFIG_AMCC_DEF_ENV_VIDEO	"videomode=x:640,y:480,depth:24," \
					"pclk:25000,le:40,ri:40,up:11,lo:11," \
					"hs:79,vs:21,sync:0,vmode:0\0"
#else
#define CONFIG_AMCC_DEF_ENV_VIDEO ""
#endif

#define CONFIG_AMCC_DEF_ENV_NOR_KERNEL "updkern=tftp 100000 ${user_dir}/${bootfile}; " \
		 "protect off ${kernel_addr} +${filesize}; " \
		 "era ${kernel_addr} +${filesize}; " \
		 "cp.b ${fileaddr} ${kernel_addr} ${filesize}\0"

#define CONFIG_AMCC_DEF_ENV_NOR_KERNEL_AMP "updkernamp=tftp 100000 ${user_dir}/${bootfileamp}; " \
		 "protect off ${kernel_amp_addr} +${filesize}; " \
		 "era ${kernel_amp_addr} +${filesize}; " \
		 "cp.b ${fileaddr} ${kernel_amp_addr} ${filesize}\0"

#define CONFIG_AMCC_DEF_ENV_NOR_KERNEL_UP "updkernup=tftp 100000 ${user_dir}/${bootfileup}; " \
		 "protect off ${kernel_up_addr} +${filesize}; " \
		 "era ${kernel_up_addr} +${filesize}; " \
		 "cp.b ${fileaddr} ${kernel_up_addr} ${filesize}\0"

#define CONFIG_AMCC_DEF_ENV_NOR_DTB "upddtb=tftp 100000 ${user_dir}/${fdt_file}; " \
		"protect off ${fdt_addr} +${filesize}; " \
		"era ${fdt_addr} +${filesize}; " \
		"cp.b ${fileaddr} ${fdt_addr} ${filesize}\0"

#define CONFIG_AMCC_DEF_ENV_NOR_DTB0 "upddtb0=tftp 100000 ${user_dir}/${fdt_file0}; " \
		"protect off ${fdt_addr0} +${filesize}; " \
		"era ${fdt_addr0} +${filesize}; " \
		"cp.b ${fileaddr} ${fdt_addr0} ${filesize}\0"

#define CONFIG_AMCC_DEF_ENV_NOR_DTB1 "upddtb1=tftp 100000 ${user_dir}/${fdt_file1}; " \
		"protect off ${fdt_addr1} +${filesize}; " \
		"era ${fdt_addr1} +${filesize}; " \
		"cp.b ${fileaddr} ${fdt_addr1} ${filesize}\0"

#define CONFIG_AMCC_DEF_ENV_NOR_DTBUP "upddtbup=tftp 100000 ${user_dir}/${fdt_fileup}; " \
		"protect off ${fdt_addrup} +${filesize}; " \
		"era ${fdt_addrup} +${filesize}; " \
		"cp.b ${fileaddr} ${fdt_addrup} ${filesize}\0"

#define CONFIG_AMCC_DEF_ENV_NOR_RAMDISK "updramdisk=tftp 100000 ${user_dir}/${ramdisk_file}; " \
		"protect off ${ramdisk_addr} +${filesize}; " \
		"era ${ramdisk_addr} +${filesize}; " \
		"cp.b ${fileaddr} ${ramdisk_addr} ${filesize}\0"

#define CONFIG_AMCC_DEF_ENV_NOR_UPDALL "updall=" \
		"run updkern; run upddtb; run updramdisk;" \
		"run updkernamp; run upddtb0; run upddtb1;" \
		"run updkernup; run upddtbup\0" 

#define CONFIG_AMCC_DEF_ENV_NAND_DTB_UP "nupddtbup=tftp 100000 ${user_dir}/${fdt_file}; " \
		"nand erase  ${nfdt_addrup} 0x20000; " \
		"nand write ${fileaddr} ${nfdt_addrup} 0x20000\0"

#define CONFIG_AMCC_DEF_ENV_NAND_KERNEL_UP "nupdkernup=tftp 100000 ${user_dir}/${bootfileup}; " \
		 "nand erase ${nkernel_up_addr} 0x300000; " \
		 "nand write ${fileaddr} ${nkernel_up_addr} 0x300000\0"

#define CONFIG_AMCC_DEF_ENV_NAND_RAMDISK_UP "nupdramdiskup=tftp 100000 ${user_dir}/${ramdisk_file}; " \
		"nand erase ${nramdisk_addr_up} 0xF00000; " \
		"nand write ${fileaddr} ${nramdisk_addr_up} 0xF00000\0"

#ifdef CONFIG_ISS
#define	CONFIG_EXTRA_ENV_SETTINGS					\
	CONFIG_AMCC_DEF_ENV						\
	CONFIG_AMCC_DEF_ENV_POWERPC					\
	CONFIG_AMCC_DEF_ENV_NOR_UPD					\
	CONFIG_AMCC_DEF_ENV_NAND_UPD					\
        "ram_self=run ramargs addip addtty addmisc;"                    \
                "bootm ${kernel_addr_r} ${ramdisk_addr_r} ${fdt_addr_r}\0"    \
	"kernel_addr=fc000000\0"					\
	"fdt_addr=fc600000\0"						\
	"kernel_amp_addr=fc8000000\0"					\
	"fdt_addr0=fce00000\0"						\
	"fdt_addr1=fce40000\0"						\
	"ramdisk_addr=fd000000\0"					\
	"verify=no\0"							\
	"hostname1=" xstr(CONFIG_HOSTNAME) "1\0"			\
	"addip1=setenv bootargs ${bootargs} "				\
		"ip=${ipaddr1}:${serverip}:${gatewayip}:${netmask}"	\
		":${hostname1}:${netdev}:off panic=1\0"			\
	"mtdargs=setenv bootargs root=/dev/mtdblock0 rw "		\
		"rootfstype=ext2\0"					\
	"mtdargs1=setenv bootargs root=/dev/mtdblock1 rw "		\
		"rootfstype=ext2\0"					\
	"sd_mtdself=run mtdargs addip addtty addmisc;"			\
		"load_e uImage ${kernel_addr_r} serengeti.dtb ${fdt_addr_r};"	\
		"bootm ${kernel_addr_r} - ${fdt_addr_r}\0"				\
	"mtd_self=run mtdargs addip addtty addmisc;"			\
		"bootm ${kernel_addr} - ${fdt_addr}\0"			\
	"mtd_self1=run mtdargs1 addip1 addtty addmisc;"			\
		"bootm ${kernel_amp_addr} - ${fdt_addr1}\0"		\
	"flash_amp0=run ramargs addip addtty addmisc;"			\
		"bootm ${kernel_amp_addr} ${ramdisk_addr} ${fdt_addr0}\0"	\
	"flash_amp1=run ramargs addip1 addtty addmisc;"			\
		"bootm ${kernel_amp_addr} ${ramdisk_addr} ${fdt_addr1}\0"	\
	"pciconfighost=1\0" 						\
	"pcie_mode=EP:EP:EP\0"						\
	"sharememory=16777216\0"					\
	"iocmemory=16777216\0"						\
	CONFIG_BOOTCOMMAND1 						\
	"ipaddr=10.66.12.197\0"						\
	"ipaddr1=10.66.12.235\0"					\
	"ethaddr=00:11:3a:8a:5a:6c\0"					\
	"eth1addr=1a:2b:3b:8b:5b:7d\0"					\
	"netmask=255.255.252.0\0"					\
	"serverip=10.66.15.50\0"					\
	"gatewayip=10.66.12.1\0"					\
	""
#else
#define CONFIG_OVERWRITE_ETHADDR_ANYTIME
#define	CONFIG_EXTRA_ENV_SETTINGS					\
	CONFIG_AMCC_DEF_ENV						\
	CONFIG_AMCC_DEF_ENV_POWERPC					\
	CONFIG_AMCC_DEF_ENV_NOR_UPD					\
	CONFIG_AMCC_DEF_ENV_NAND_UPD					\
	CONFIG_AMCC_DEF_ENV_IPP						\
	CONFIG_AMCC_DEF_ENV_VIDEO					\
	CONFIG_AMCC_DEF_ENV_NOR_KERNEL					\
	CONFIG_AMCC_DEF_ENV_NOR_KERNEL_AMP				\
	CONFIG_AMCC_DEF_ENV_NOR_KERNEL_UP				\
	CONFIG_AMCC_DEF_ENV_NOR_DTB					\
	CONFIG_AMCC_DEF_ENV_NOR_DTB0					\
	CONFIG_AMCC_DEF_ENV_NOR_DTB1					\
	CONFIG_AMCC_DEF_ENV_NOR_DTBUP					\
	CONFIG_AMCC_DEF_ENV_NOR_RAMDISK					\
	CONFIG_AMCC_DEF_ENV_NOR_UPDALL					\
	CONFIG_AMCC_DEF_ENV_NAND_DTB_UP                                 \
	CONFIG_AMCC_DEF_ENV_NAND_KERNEL_UP                              \
	CONFIG_AMCC_DEF_ENV_NAND_RAMDISK_UP                             \
	CONFIG_BOOTCOMMAND0 						\
	CONFIG_BOOTCOMMAND1 						\
	"hostname1=" xstr(CONFIG_HOSTNAME) "1\0"			\
	"addip1=setenv bootargs ${bootargs} "				\
		"ip=${ipaddr1}:${serverip}:${gatewayip}:${netmask}"	\
		":${hostname1}:${netdev}:off panic=1\0"			\
	"flash_amp0=run ramargs addip addtty addmisc;"			\
		"bootm ${kernel_amp_addr} ${ramdisk_addr} ${fdt_addr0}\0"	\
	"flash_amp1=run ramargs addip1 addtty addmisc;"			\
		"bootm ${kernel_amp_addr} ${ramdisk_addr} ${fdt_addr1}\0"	\
	"flash_up=run ramargs addip addtty addmisc;"			\
		"bootm ${kernel_up_addr} ${ramdisk_addr} ${fdt_addrup}\0"	\
	"kernel_addr=fc000000\0"					\
	"fdt_addr=fc600000\0"						\
	"kernel_amp_addr=fc800000\0"					\
	"fdt_addr0=fce00000\0"						\
	"fdt_addr1=fce40000\0"						\
	"fdt_addrup=fce80000\0"						\
	"nfdt_addrup=0x100000\0"						\
	"kernel_up_addr=fd000000\0"					\
	"nkernel_up_addr=0x120000\0"					\
	"ramdisk_addr=fd600000\0"					\
	"nramdisk_addr_up=0x400000\0"					\
	"bootfileamp=uImage_amp\0"					\
	"bootfileup=uImage_up\0"					\
	"fdt_file0=serengeti_amp0.dtb\0"				\
	"fdt_file1=serengeti_amp1.dtb\0"				\
	"fdt_fileup=serengeti_up.dtb\0"					\
	"ipaddr=192.168.1.2\0"						\
	"ipaddr1=192.168.1.3\0"						\
	"netmask=255.255.252.0\0"					\
	"serverip=192.168.1.254\0"					\
	"gatewayip=192.168.1.1\0"					\
	"iocmemory=16777216\0"						\
	CONFIG_APM_EXTRA_ENV_MEDIA					\
	""
#endif

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
#if !defined(CONFIG_HW_REV_11) && !defined(CONFIG_HW_REV_14)
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
#define CONFIG_SYS_MAX_NAND_DEVICE      1
#define CONFIG_SYS_NAND_BASE            CONFIG_SYS_EBC_BASE
#define CONFIG_SYS_NAND_BAD_BLOCK_POS   0	/* Location of bad block marker */
#define CONFIG_SYS_NAND_5_ADDR_CYCLE    1	/* Fourth addr used (>32MB)     */

#if !defined(CONFIG_NAND_U_BOOT) && !defined(CONFIG_NAND_SPL) && \
    !defined(CONFIG_OCM_U_BOOT)
/* Non-NAND Booting with NAND support */
#if !defined(CONFIG_ISS) && !defined(CONFIG_ENV_IS_NOWHERE)
# define CONFIG_ENV_IS_IN_FLASH  	1	/* use FLASH for environment vars */
#endif
#define CONFIG_SYS_NOR_CS               0       /* NOR chip connected to CSx */
#define CONFIG_SYS_NAND_CS              2       /* NAND chip connected to CSx */

#else
/* NAND Booting */
#if !defined(CONFIG_ISS) && !defined(CONFIG_ENV_IS_NOWHERE)
#define CONFIG_ENV_IS_IN_NAND   	1       /* use NAND for environment vars  */
#endif
#if !defined(CONFIG_NAND_U_BOOT) && !defined(CONFIG_OCM_U_BOOT)
#define CONFIG_ENV_IS_EMBEDDED  	1       /* use embedded environment */
#endif
#define CONFIG_SYS_NOR_CS               1       /* NOR chip connected to CSx */
#define CONFIG_SYS_NAND_CS              0       /* NAND chip connected to CSx */

#if defined(CONFIG_NAND_SPL)
# if defined(CONFIG_NAND_SPL_S1)
#  define CONFIG_SYS_NAND_BOOT_SPL_SRC   0xFFFFF000      /* 4k SPL location */
#  define CONFIG_SYS_NAND_BOOT_SPL_SIZE  (4 << 10)       /* SPL size */
#  define CONFIG_SYS_NAND_BOOT_SPL_DST   (CONFIG_SYS_ISRAM_BASE) /* Copy 4k SPL here */
# elif defined(CONFIG_NAND_SPL_S2) /* !defined(CONFIG_NAND_SPL_S1) */
#  define CONFIG_SYS_NAND_BOOT_SPL_SRC   0x90003000 /* 20k Stage-2 SPL location */
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
# define CONFIG_SYS_NAND_U_BOOT_DST      0x90003000      /* Load Stage-2 to this addr */
# define CONFIG_SYS_NAND_U_BOOT_START    0x90007FFC /* Start Stage-2 spl from this addr     */
# define CONFIG_SYS_NAND_U_BOOT_OFFS     (4 << 10)
# define CONFIG_SYS_NAND_U_BOOT_SIZE     (20 << 10)
#else /* !defined(CONFIG_NAND_SPL_S1) */
# define CONFIG_SYS_NAND_U_BOOT_DST      0x01000000      /* Load NUB to this addr */
# define CONFIG_SYS_NAND_U_BOOT_START    (CONFIG_SYS_NAND_U_BOOT_DST)/* Start NUB from this addr     */
# define CONFIG_SYS_NAND_U_BOOT_OFFS     ((4 + 20) << 10)
# define CONFIG_SYS_NAND_U_BOOT_SIZE     (384 << 10)
#endif /* !defined(CONFIG_NAND_SPL_S1) */

/*
 * Now the NAND chip has to be defined (no autodetection used for NAND SPL)
 */
#define CONFIG_SYS_NAND_PAGE_SIZE       (2 << 10)       /* NAND chip page size          */
#define CONFIG_SYS_NAND_BLOCK_SIZE      (128 << 10)     /* NAND chip block size         */
#define CONFIG_SYS_NAND_PAGE_COUNT      64              /* NAND chip page count         */
#define CONFIG_SYS_NAND_BAD_BLOCK_POS   0               /* Location of bad block marker */
#define CONFIG_SYS_NAND_OOBSIZE		64

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
#define CONFIG_ENV_SIZE         (CONFIG_SYS_NAND_BLOCK_SIZE)
#define CONFIG_ENV_OFFSET	(0xC0000)
#define CONFIG_NAND_ENV_DST	(CONFIG_SYS_NAND_U_BOOT_DST + CONFIG_SYS_NAND_U_BOOT_SIZE + CONFIG_SYS_NAND_BOOT_SPL_SIZE)
#define CONFIG_ENV_ADDR CONFIG_NAND_ENV_DST
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
