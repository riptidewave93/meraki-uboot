#ifndef __HURRICANE2_EMUL_H
#define __HURRICANE2_EMUL_H

#include <asm/sizes.h>

#define BOARD_LATE_INIT
#define CONFIG_PHYS_SDRAM_1_SIZE			0x00020000 /* 128KB */

/* Backup uboot uses same environment area as primary uboot */
#define CONFIG_ENV_OFFSET	0xa0000


/* Architecture, CPU, etc */
#define CONFIG_ARMV7
#define CONFIG_IPROC (0)
#define CONFIG_HURRICANE2 (1)
#define CONFIG_HURRICANE2_EMULATION (1)
#define CONFIG_HURRICANE2_ROMCODE (1)

/*
Fix me

When DEBUG is enabled, need to disable both CACHE to make u-boot running
#define CONFIG_SYS_NO_ICACHE
#define CONFIG_SYS_NO_DCACHE
#define DEBUG

*/
#define CONFIG_IPROC_MMU	(1)
#define CONFIG_L2_OFF				/* Disable L2 cache */
#define CONFIG_SYS_ARM_CACHE_WRITETHROUGH (1)
#define CONFIG_MACH_TYPE			0xa8d

//#define CONFIG_MISC_INIT_R			/* Call board's misc_init_r function */

/* Interrupt configuration */
#define CONFIG_USE_IRQ          1	/* we need IRQ stuff for timer	*/

/* Memory Info */
#ifdef CONFIG_L2C_AS_RAM
#define CONFIG_SYS_MALLOC_LEN 			8192  	/* see armv7/start.S. */
#else
#define CONFIG_SYS_MALLOC_LEN 			SZ_256K  	/* see armv7/start.S. */
#endif

#define CONFIG_STACKSIZE_IRQ			(4096)
#define CONFIG_STACKSIZE_FIQ			(4096)

#ifdef CONFIG_L2C_AS_RAM
#define CONFIG_PHYS_SDRAM_0				0xc0000000
#define CONFIG_PHYS_SDRAM_1				0xc0000000
//#define CONFIG_LOADADDR					0x00010000 /* default destination location for tftp file (tftpboot cmd) */
#define CONFIG_PHYS_SDRAM_RSVD_SIZE		0x0 /* bytes reserved from CONFIG_PHYS_SDRAM_1 for custom use */
#else
#define CONFIG_PHYS_SDRAM_0				0x00000000
#define CONFIG_PHYS_SDRAM_1				0x00000000
//#define CONFIG_LOADADDR					0x00010000 /* default destination location for tftp file (tftpboot cmd) */
#define CONFIG_PHYS_SDRAM_RSVD_SIZE		0x0 /* bytes reserved from CONFIG_PHYS_SDRAM_1 for custom use */
#endif

/* Where kernel is loaded to in memory */
//#define CONFIG_SYS_LOAD_ADDR				0x08000000
//#define LINUX_BOOT_PARAM_ADDR				(CONFIG_PHYS_SDRAM_1+CONFIG_PHYS_SDRAM_RSVD_SIZE)

#define CONFIG_SYS_MEMTEST_START			CONFIG_PHYS_SDRAM_1
#define CONFIG_SYS_MEMTEST_END			(CONFIG_PHYS_SDRAM_1+CONFIG_PHYS_SDRAM_1_SIZE)
#define CONFIG_NR_DRAM_BANKS				1

#define CONFIG_SYS_SDRAM_BASE		(CONFIG_PHYS_SDRAM_1 + CONFIG_PHYS_SDRAM_RSVD_SIZE)
/* CONFIG_SYS_TEXT_BASE is where u-boot is loaded by boot1 */
#ifdef CONFIG_L2C_AS_RAM
#define CONFIG_SYS_STACK_SIZE		(0x00002000) /* 8K */       
#else
#define CONFIG_SYS_STACK_SIZE		(0x00010000) /* 64K */       
#endif

#ifdef CONFIG_NO_CODE_RELOC
#define CONFIG_SYS_INIT_SP_ADDR		(CONFIG_PHYS_SDRAM_0 + 2*CONFIG_SYS_STACK_SIZE - 16)
#else
#define CONFIG_SYS_INIT_SP_ADDR		(CONFIG_PHYS_SDRAM_0 + CONFIG_SYS_STACK_SIZE - 16)
#endif

/* Clocks */
#define CONFIG_SYS_REF_CLK			(25000000) /*Reference clock = 25MHz */
#define CONFIG_SYS_REF2_CLK			(200000000) /*Reference clock = 25MHz */
#ifdef CONFIG_HURRICANE2_EMULATION
#define IPROC_ARM_CLK		(173231)	/* Fix me: Modify these values as per emulation clocks */
#define IPROC_AXI_CLK		(346462)
#define IPROC_APB_CLK		(86615)
#endif
/* Serial Info */
#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_CONS_INDEX					1
#if 0 //CCB
#define CONFIG_SYS_NS16550_COM1			(0x18037000)
#define CONFIG_SYS_NS16550_REG_SIZE		(-4)	/* Post pad 3 bytes after each reg addr */
#define CONFIG_SYS_NS16550_CLK			(268800)
#define CONFIG_SYS_NS16550_CLK_DIV		(7)
#define CONFIG_BAUDRATE					2400
#endif

#if 1 //CCA 
#define CONFIG_SYS_NS16550_COM1			(0x18000300)
#define CONFIG_SYS_NS16550_REG_SIZE		(1)	/* no padding */
#define CONFIG_SYS_NS16550_CLK			(43308) /* Set it to right clock, i.e APB_clk/2 */
#define CONFIG_SYS_NS16550_CLK_DIV		(9) /* Set it to right divider, i.e APB_clk/2/16/baudrate */
#define CONFIG_BAUDRATE					300
#endif

#define CONFIG_SYS_BAUDRATE_TABLE	{9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600} 

#define CONFIG_ENV_OVERWRITE	/* Allow serial# and ethernet mac address to be overwritten in nv storage */
#ifdef CONFIG_L2C_AS_RAM
#define CONFIG_ENV_SIZE			0x1000 /* 4K */
#else
#define CONFIG_ENV_SIZE			0x10000 /* 64K */
#endif

/* NO DDR */
#define CONFIG_SYS_NO_DDR		/* Not using DDR in unmanaged mode */

/* NO flash */
#define CONFIG_SYS_NO_FLASH		/* Not using NAND/NOR unmanaged flash */

/* NO bootm command */
#define CONFIG_SYS_NO_BOOTM	    /* Not using any boot mechanism */

/* Ethernet configuration */
//#define CONFIG_BCM5301X_ETH
//#define CONFIG_NET_MULTI

/* DMA configuration */
//#define CONFIG_BCM5301X_DMA

/* General U-Boot configuration */
#define CONFIG_SYS_CBSIZE			1024	/* Console buffer size */
#define CONFIG_SYS_PBSIZE			(CONFIG_SYS_CBSIZE +	sizeof(CONFIG_SYS_PROMPT) + 16) /* Printbuffer size */
#define CONFIG_SYS_MAXARGS			16
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE

//#define CONFIG_VERSION_VARIABLE	/* Enabled UBOOT build date/time id string */
//#define CONFIG_AUTO_COMPLETE
//#define CONFIG_SYS_HUSH_PARSER
//#define CONFIG_SYS_PROMPT_HUSH_PS2	"> "
//#define CONFIG_CMDLINE_EDITING
//#define CONFIG_SYS_LONGHELP

//#define CONFIG_CRC32_VERIFY		/* Add crc32 to memory verify commands */
//#define CONFIG_MX_CYCLIC			/* Memory display cyclic */

//#define CONFIG_CMDLINE_TAG				/* ATAG_CMDLINE setup */
//#define CONFIG_SETUP_MEMORY_TAGS		/* ATAG_MEM setup */

//#define CONFIG_BOOTARGS     				"console=ttyS0,115200n8 androidboot.console=ttyS0 root=/dev/mmcblk0p4 rootfstype=ext4 gpt bootmemheap rootwait"

//#define CONFIG_BOOTDELAY			1	/* User can hit a key to abort kernel boot and stay in uboot cmdline */
//#define CONFIG_BOOTCOMMAND 				""	/* UBoot command issued on power up */
#define CONFIG_SYS_PROMPT					"u-boot> "  

//#include <config_cmd_default.h>
#define CONFIG_CMD_CONSOLE
#define CONFIG_ENV_IS_NOWHERE
#undef CONFIG_CMD_NFS
#undef CONFIG_GENERIC_MMC
//#define CONFIG_CMD_NET
//#define CONFIG_CMD_PING
//#define CONFIG_CMD_MEMORY
//#define CONFIG_CMD_MISC

//#define CONFIG_ETHADDR		00:90:4c:06:a5:72
//#define CONFIG_NETMASK		255.255.255.0
//#define CONFIG_IPADDR		192.168.0.1
//#define CONFIG_GATEWAYIP	192.168.0.100
//#define CONFIG_SERVERIP		192.168.0.10

#ifdef UBOOT_MDK
#define CONFIG_GMAC_NUM		2 
#else
#define CONFIG_GMAC_NUM		0 
#endif

/* Undefine config_defaults.h */
#undef CONFIG_BOOTM_LINUX
#undef CONFIG_BOOTM_NETBSD
#undef CONFIG_BOOTM_OSE
#undef CONFIG_BOOTM_RTEMS

#undef CONFIG_GZIP
#undef CONFIG_ZLIB

#endif /* __HURRICANE2_EMUL_H */



	
