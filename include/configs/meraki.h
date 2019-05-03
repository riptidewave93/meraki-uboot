/*
 * Copyright (c) 2009, Atheros Communications Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 */
/*
 * This file contains the configuration parameters for the dbau1x00 board.
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <configs/ar7100.h>

#undef CONFIG_BOOTDELAY
#define CONFIG_BOOTDELAY 1

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */

#define CFG_MAX_FLASH_BANKS     1       /* max number of memory banks */

/* define for sector size = 256K  */
//#define FLASH_SPANSION
/*-------*/

#if (FLASH_SIZE == 16)
  #ifdef FLASH_SPANSION
    #define CFG_MAX_FLASH_SECT	64
  #else
    #define CFG_MAX_FLASH_SECT	256 
  #endif /* ifdef FLASH_SPANSION  */
#else
  #if (FLASH_SIZE == 8)
    #define CFG_MAX_FLASH_SECT	128    /* max number of sectors on one chip */
  #else
    #define CFG_MAX_FLASH_SECT	64
  #endif  /* FLASH_SIZE == 8  */
#endif    /* FLASH_SIZE == 16 */

#if (FLASH_SIZE == 16)
  #ifdef FLASH_SPANSION
    #define CFG_FLASH_SECTOR_SIZE   (256*1024)
  #else
    #define CFG_FLASH_SECTOR_SIZE   (64*1024)
  #endif /* ifdef FLASH_SPANSION */
  #define CFG_FLASH_SIZE          0x1000000 /* Total flash size */
#else
  #define CFG_FLASH_SECTOR_SIZE	  (64*1024)
  #if (FLASH_SIZE == 8)
    #define CFG_FLASH_SIZE	  0x00800000
  #else
    #define CFG_FLASH_SIZE	  0x00400000
  #endif /* FLASH_SIZE == 8  */
#endif	 /* FLASH_SIZE == 16 */

#define CFG_FLASH_WORD_SIZE     unsigned short 
#define CFG_FLASH_ADDR0         (0x5555)        
#define CFG_FLASH_ADDR1         (0x2AAA)


/* 
 * We boot from this flash
 */
#define CFG_FLASH_BASE          0xbf000000

#undef CONFIG_ROOTFS_RD
#undef CONFIG_ROOTFS_FLASH
#undef CONFIG_BOOTARGS_FL
#undef CONFIG_BOOTARGS_RD
#undef CONFIG_BOOTARGS
#undef  MTDPARTS_DEFAULT
#undef  MTDIDS_DEFAULT

#define CONFIG_ROOTFS_FLASH
#define CONFIG_BOOTARGS CONFIG_BOOTARGS_FL

//#define CONFIG_BOOTARGS_RD     "console=ttyS0,115200 root=01:00 rd_start=0x80600000 rd_size=5242880 init=/sbin/init mtdparts=ar9100-nor0:256k(u-boot),64k(u-boot-env),4096k(rootfs),2048k(uImage)"

/* XXX - putting rootfs in last partition results in jffs errors */
#if (FLASH_SIZE == 8)
  #define CONFIG_BOOTARGS_FL     "console=ttyS0,115200 root=31:02 rootfstype=jffs2 init=/sbin/init mtdparts=ar7100-nor0:192k(uboot),64k(env),6144k(rootfs),1728k(uImage),64k(caldata) mem=64M"
#else
  #if (FLASH_SIZE == 16)
    #define CONFIG_BOOTARGS_FL	 "console=ttyS0,115200 root=31:02 rootfstype=jffs2 init=/sbin/init mtdparts=ar7100-nor0:256k(uboot),256k(env),6144k(rootfs),2048k(uImage),7680k(caldata) mem=64M"
  #endif 
#endif

#if (FLASH_SIZE ==16)
  #define MTDPARTS_DEFAULT    "mtdparts=ar7100-nor0:256k(u-boot),256k(env),6144k(rootfs),2048k(uImage),7680k(caldata) mem=64M"
#else 
  #define MTDPARTS_DEFAULT    "mtdparts=ar7100-nor0:192k(u-boot),64k(env),6144k(rootfs),1728k(uImage),64k(caldata) mem=64M"
#endif

#define MTDIDS_DEFAULT      "nor0=ar7100-nor0"

/* 
 * The following #defines are needed to get flash environment right 
 */
#define CFG_MONITOR_BASE    TEXT_BASE
#define CFG_MONITOR_LEN     (192 << 10)

#undef CFG_HZ
/*
 * MIPS32 24K Processor Core Family Software User's Manual
 *
 * 6.2.9 Count Register (CP0 Register 9, Select 0)
 * The Count register acts as a timer, incrementing at a constant
 * rate, whether or not an instruction is executed, retired, or
 * any forward progress is made through the pipeline.  The counter
 * increments every other clock, if the DC bit in the Cause register
 * is 0.
 */

#define CFG_HZ	(680000000/2)

/* 
 * timeout values are in ticks 
 */
#define CFG_FLASH_ERASE_TOUT    (2 * CFG_HZ) /* Timeout for Flash Erase */
#define CFG_FLASH_WRITE_TOUT    (2 * CFG_HZ) /* Timeout for Flash Write */

/*
 * Cache lock for stack
 */
#define CFG_INIT_SP_OFFSET  0x1000

#define CFG_ENV_IS_IN_FLASH    1  
#undef CFG_ENV_IS_NOWHERE  

/* Address and size of Primary Environment Sector   */
#if (FLASH_SIZE == 16)
  #define CFG_ENV_ADDR        0xbf040000

#ifdef FLASH_SPANSION
  #define CFG_ENV_SIZE        0x40000
#else
  #define CFG_ENV_SIZE        0x10000
#endif /* endif FLASH_SPANSION */

  #define CONFIG_BOOTCOMMAND "cpckgo.b 0xbf2c0000 0xbf940000 0x80060000"
#else 
  #define CFG_ENV_ADDR        0xbf030000
  #define CFG_ENV_SIZE        0x10000

  #define CONFIG_BOOTCOMMAND "bootm 0xbf640000"
#endif

/*
** Extra environmental variables useful for loading the board
*/
#if (FLASH_SIZE == 16)
   #define CONFIG_EXTRA_ENV_SETTINGS \
"loadUboot=tftpboot 0x80010000 u-boot.bin;erase 0xbf000000 +0x40000;cp.b 0x80010000 0xbf000000 0x40000\0" \
"loadLinux=tftpboot 0x80010000 vmlinux.gz.uImage;erase 0xbf680000 +0x200000;cp.b 0x80010000 0xbf680000 0x200000\0" \
"loadFiles=tftpboot 0x80010000 ap96-jffs2;erase 0xbf080000 +0x600000;cp.b 0x80010000 0xbf080000 0x600000\0" \
"loadAll=run loadUboot;run loadLinux;run loadFiles\0" \
"factory_boot=tftpboot 0x3000000 vmlinux.gz.uImage;bootm 0x3000000\0"
#else
  #define CONFIG_EXTRA_ENV_SETTINGS \
"loadUboot=tftpboot 0x80010000 u-boot.bin;erase 0xbf000000 +0x30000;cp.b 0x80010000 0xbf000000 0x30000\0" \
"loadLinux=tftpboot 0x80010000 vmlinux.gz.uImage;erase 0xbf640000 +0x120000;cp.b 0x80010000 0xbf640000 0x120000\0" \
"loadFiles=tftpboot 0x80010000 ap96-jffs2;erase 0xbf040000 +0x600000;cp.b 0x80010000 0xbf040000 0x600000\0" \
"loadAll=run loadUboot;run loadLinux;run loadFiles\0" \
"factory_boot=tftpboot 0x3000000 vmlinux.gz.uImage ; bootm 0x3000000\0"
#endif

#define CONFIG_NR_DRAM_BANKS    		2

#define CFG_BOARD_AP96 1

/*
** Parameters defining the location of the calibration/initialization
** information for the two Merlin devices.
** NOTE: **This will change with different flash configurations**
*/

#define MERLIN24CAL                     0xbfff1000
#define MERLIN50CAL                     0xbfff5000
#define BOARDCAL                        0xbfff0000
#define ATHEROS_PRODUCT_ID              145
//#define CAL_SECTOR                      127
#define CAL_SECTOR			(CFG_MAX_FLASH_SECT - 1)

/*
** Configure the CPU clock settings
*/

#define CFG_PLL_FREQ	CFG_PLL_680_340_170
/*
** These values for DDR configuration came from the
** BDM configuration script
*/

#define CFG_DDR_REFRESH_VAL     		0x461b
#define CFG_DDR_CONFIG_VAL      		0x77b8884e
#define CFG_DDR_CONFIG2_VAL			0x812cd6a8
#define CFG_DDR_MODE_VAL_INIT   		0x131
#define CFG_DDR_EXT_MODE_VAL    		0x0
#define CFG_DDR_MODE_VAL        		0x33
#define CFG_DDR_TRTW_VAL        		0x1f
#define CFG_DDR_TWTR_VAL        		0x1e
#define CFG_DDR_RD_DATA_THIS_CYCLE_VAL  	0xff


#define CONFIG_NET_MULTI
#define CONFIG_MEMSIZE_IN_BYTES
#define CONFIG_PCI


/*-----------------------------------------------------------------------
 * Cache Configuration
 */
#define CONFIG_COMMANDS (( CONFIG_CMD_DFL        | CFG_CMD_MII   | CFG_CMD_PING  \
   | CFG_CMD_NET | CFG_CMD_PCI     | CFG_CMD_ENV | CFG_CMD_FLASH | CFG_CMD_LOADS \
   | CFG_CMD_RUN | CFG_CMD_LOADB   | CFG_CMD_ELF | CFG_CMD_BSP ))

/*
** Ethernet Configuration
** Set default values, and select the PHY to use
*/

#define CONFIG_IPADDR       		192.168.1.2
#define CONFIG_SERVERIP     		192.168.1.101
//#define CONFIG_ETHADDR      		00:03:7f:ff:ff:fe
//#define CFG_FAULT_ECHO_LINK_DOWN    1
#define CONFIG_PHY_GIGE       		1              /* GbE speed/duplex detect */

#define CFG_ATHRF1_PHY				1
//#define	CFG_ATHRS16_PHY  			1
#define CFG_AG7100_NMACS 			2

#define CFG_PHY_ADDR				0  /* Port 4 */
#define CFG_GMII 1
#define CFG_MII0_RGMII 0
#define CFG_AG7100_GE0_RGMII 1
#define CFG_AG7100_GE1_RGMII 1

/*
** Configure Parser
*/

#define CFG_BOOTM_LEN   (16 << 20) /* 16 MB */
#define DEBUG
#define CFG_HUSH_PARSER
#define CFG_PROMPT_HUSH_PS2 "hush>"

#include <cmd_confdefs.h>

#endif  /* __CONFIG_H */
