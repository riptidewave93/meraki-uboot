#ifndef __NORTHSTAR_PLUS_H
#define __NORTHSTAR_PLUS_H

#include <asm/sizes.h>

#define CONFIG_BOARD_EARLY_INIT_F (1)
#define CONFIG_BOARD_LATE_INIT
#define CONFIG_PHYS_SDRAM_1_SIZE			0x01000000 /* 16 MB */
#define IPROC_ETH_MALLOC_BASE 0xD00000

#define STDK_BUILD (1)

/* Architecture, CPU, etc */
#define CONFIG_ARMV7
#define CONFIG_IPROC (1)
#define CONFIG_NS_PLUS (1)

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

#define CONFIG_MISC_INIT_R			/* Call board's misc_init_r function */

/* Interrupt configuration */
#define CONFIG_USE_IRQ          1	/* we need IRQ stuff for timer	*/

/* Memory Info */
#if (defined(CONFIG_L2C_AS_RAM ) && defined(CONFIG_NO_CODE_RELOC))
#define CONFIG_SYS_MALLOC_LEN 			0x8000  	/* see armv7/start.S. */
#else
#define CONFIG_SYS_MALLOC_LEN 			SZ_256K  	/* see armv7/start.S. */
#endif /* (defined(CONFIG_L2C_AS_RAM ) && defined(CONFIG_NO_CODE_RELOC)) */
#define CONFIG_STACKSIZE_IRQ			(4096)
#define CONFIG_STACKSIZE_FIQ			(4096)
#ifdef CONFIG_L2C_AS_RAM
//#define CONFIG_PHYS_SDRAM_0				0xc0000000
#define CONFIG_PHYS_SDRAM_0				0x50000000  /* SRAM */
#define CONFIG_L2_CACHE_SIZE			0x40000  
#define CONFIG_PHYS_SDRAM_1				0x00000000
#define CONFIG_LOADADDR					0x90000000 /* default destination location for tftp file (tftpboot cmd) */
#define CONFIG_PHYS_SDRAM_RSVD_SIZE		0x0 /* bytes reserved from CONFIG_PHYS_SDRAM_1 for custom use */
#else
#define CONFIG_PHYS_SDRAM_0				0x00000000
#define CONFIG_PHYS_SDRAM_1				0x00000000
#define CONFIG_LOADADDR					0x90000000 /* default destination location for tftp file (tftpboot cmd) */
#define CONFIG_PHYS_SDRAM_RSVD_SIZE		0x00100000 /* bytes reserved from CONFIG_PHYS_SDRAM_1 for custom use */
#endif /* CONFIG_L2C_AS_RAM */

/* Where kernel is loaded to in memory */
#define CONFIG_SYS_LOAD_ADDR				0x70000000
#define LINUX_BOOT_PARAM_ADDR				0x60200000 /* default mapped location to store the atags pointer */

#define CONFIG_SYS_MEMTEST_START			CONFIG_PHYS_SDRAM_1
#define CONFIG_SYS_MEMTEST_END			(CONFIG_PHYS_SDRAM_1+CONFIG_PHYS_SDRAM_1_SIZE)
#define CONFIG_NR_DRAM_BANKS				1

#define CONFIG_SYS_SDRAM_BASE		(CONFIG_PHYS_SDRAM_1 + CONFIG_PHYS_SDRAM_RSVD_SIZE)
/* CONFIG_SYS_TEXT_BASE is where u-boot is loaded by boot1 */
#ifdef CONFIG_L2C_AS_RAM
#define CONFIG_SYS_STACK_SIZE		(0x0001f000)
#else
#define CONFIG_SYS_STACK_SIZE		(0x00010000) /* 64K */       
#endif /* CONFIG_L2C_AS_RAM */

#define CONFIG_SYS_INIT_SP_ADDR		(CONFIG_PHYS_SDRAM_0 + CONFIG_SYS_STACK_SIZE - 16)
//#define CONFIG_SYS_INIT_SP_ADDR		(CONFIG_PHYS_SDRAM_1 + CONFIG_SYS_STACK_SIZE - 16)

/* Clocks */
#define CONFIG_SYS_REF_CLK			(25000000) /*Reference clock = 25MHz */
#define CONFIG_SYS_REF2_CLK			(200000000) /*Reference clock = 25MHz */
#define IPROC_ARM_CLK		(1000000000) /* 1GHz */
#define IPROC_AXI_CLK		(500000000)  /* 500 MHz */
#define IPROC_APB_CLK		(125000000)  /* 125 MHz */

#define CONFIG_ENV_OVERWRITE	/* Allow serial# and ethernet mac address to be overwritten in nv storage */
#ifdef CONFIG_L2C_AS_RAM
#define CONFIG_ENV_SIZE			0x1000 /* 4K */
#else
#define CONFIG_ENV_SIZE			0x10000 /* 64K */
#endif /* CONFIG_L2C_AS_RAM */
/* NO flash */
#define CONFIG_SYS_NO_FLASH		/* Not using NAND/NOR unmanaged flash */

/* Ethernet configuration */
#ifndef CONFIG_NO_CODE_RELOC
#define CONFIG_BCMIPROC_ETH
#endif /* CONFIG_NO_CODE_RELOC */
//#define CONFIG_BCMHANA_ETH
#define CONFIG_NET_MULTI

/* DMA configuration */
//#define CONFIG_BCM5301X_DMA

/* General U-Boot configuration */
#define CONFIG_SYS_CBSIZE			1024	/* Console buffer size */
#define CONFIG_SYS_PBSIZE			(CONFIG_SYS_CBSIZE +	sizeof(CONFIG_SYS_PROMPT) + 16) /* Printbuffer size */
#define CONFIG_SYS_MAXARGS			64
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE

//#define CONFIG_VERSION_VARIABLE	/* Enabled UBOOT build date/time id string */
#define CONFIG_AUTO_COMPLETE
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT_HUSH_PS2	"> "
#define CONFIG_CMDLINE_EDITING
#define CONFIG_SYS_LONGHELP

#define CONFIG_CRC32_VERIFY		/* Add crc32 to memory verify commands */
#define CONFIG_MX_CYCLIC			/* Memory display cyclic */

#define CONFIG_CMDLINE_TAG				/* ATAG_CMDLINE setup */
#define CONFIG_SETUP_MEMORY_TAGS		/* ATAG_MEM setup */

#define CONFIG_SYS_PROMPT					"u-boot> "  

//#include <config_cmd_default.h>
#define CONFIG_CMD_CONSOLE
#define CONFIG_ENV_IS_NOWHERE
#define CONFIG_CMD_NFS
#undef CONFIG_GENERIC_MMC
#define CONFIG_CMD_NET
#define CONFIG_CMD_PING
#define CONFIG_CMD_MEMORY
//#define CONFIG_CMD_MISC
#define CONFIG_CMD_LICENSE

#if 1
//dgb for bringup
#define CONFIG_CMD_NAND 
#define CONFIG_IPROC_NAND 
#endif /* 0 */

#define CONFIG_ETHADDR		d4:ae:52:bc:a5:08
//#define CONFIG_NETMASK		255.255.255.0
#define CONFIG_IPADDR		192.168.0.1
//#define CONFIG_GATEWAYIP	192.168.0.100
#define CONFIG_SERVERIP		192.168.0.10

#ifdef UBOOT_MDK
#define CONFIG_GMAC_NUM		2 
#else
#define CONFIG_GMAC_NUM		1 
#endif /* UBOOT_MDK */

/* QSPI */
#define CONFIG_CMD_SPI
#define CONFIG_IPROC_QSPI

/* Enable generic u-boot SPI flash drivers and commands */
#define CONFIG_CMD_SF
#define CONFIG_SPI_FLASH
#define CONFIG_SPI_FLASH_STMICRO_NS
#define CONFIG_SPI_FLASH_MACRONIX_NS
#define CONFIG_SPI_FLASH_SPANSION
#define CONFIG_SPI_FLASH_SST
#define CONFIG_SPI_FLASH_WINBOND
#define CONFIG_SPI_FLASH_ATMEL

/* SPI flash configurations for Northstar */
#define CONFIG_IPROC_QSPI
#define CONFIG_IPROC_QSPI_BUS                   0
#define CONFIG_IPROC_QSPI_CS                    0

/* SPI flash configuration - flash specific */
/*
Different parts require different opcode (CMD) and dummy cycles.

For N25Q256, the following settings should provide max performance (for read):

#define CONFIG_IPROC_BSPI_DATA_LANES            4
#define CONFIG_IPROC_BSPI_ADDR_LANES            4
#define CONFIG_IPROC_BSPI_READ_CMD              0xeb
#define CONFIG_IPROC_BSPI_READ_DUMMY_CYCLES     10
#define CONFIG_SF_DEFAULT_SPEED                 62500000
*/
/* using 1 lane mode to work with most parts */

#define CONFIG_IPROC_BSPI_DATA_LANES            1
#define CONFIG_IPROC_BSPI_ADDR_LANES            1
#define CONFIG_IPROC_BSPI_READ_CMD              0x0b
#define CONFIG_IPROC_BSPI_READ_DUMMY_CYCLES     8
#define CONFIG_SF_DEFAULT_SPEED                 50000000
#define CONFIG_SF_DEFAULT_MODE                  SPI_MODE_3

/* Environment variables */
#undef CONFIG_ENV_IS_NOWHERE
#ifdef CONFIG_NAND_IPROC_BOOT
#define CONFIG_ENV_IS_IN_NAND                   1
#define CONFIG_ENV_OFFSET                       0x200000
#define CONFIG_ENV_RANGE                        0x200000
#else
#ifdef CONFIG_SPI_FLASH
#define CONFIG_ENV_IS_IN_SPI_FLASH              1
#define CONFIG_ENV_OFFSET                       0xa0000
#define CONFIG_ENV_SPI_MAX_HZ                   10000000
#define CONFIG_ENV_SPI_MODE                     SPI_MODE_3
#define CONFIG_ENV_SPI_BUS                      CONFIG_IPROC_QSPI_BUS
#define CONFIG_ENV_SPI_CS                       CONFIG_IPROC_QSPI_CS
#define CONFIG_ENV_SECT_SIZE                    0x10000     /* 64KB */
#endif /* CONFIG_SPI_FLASH */
#endif /* CONFIG_NAND_IPROC_BOOT */

/* Environment variables for NAND flash */
#define CONFIG_CMD_NAND 
#define CONFIG_SYS_MAX_NAND_DEVICE			1
#define CONFIG_SYS_NAND_BASE		        0xdeadbeef
#define CONFIG_SYS_NAND_ONFI_DETECTION

#ifdef CONFIG_IPROC_BOARD_DIAGS
#define IPROC_BOARD_DIAGS 	/* Enabling this fails booting on HR board */
#endif /* CONFIG_IPROC_BOARD_DIAGS */

#ifdef IPROC_BOARD_DIAGS
//POST related defines
#define CONFIG_HAS_POST
//define post to support diags available, this is a ORed list
//See bitmask definition in post.h
#ifndef CONFIG_NO_SLIC
#define CONFIG_POST      (CONFIG_SYS_POST_PWM | CONFIG_SYS_POST_QSPI|CONFIG_SYS_POST_NAND|\
                          CONFIG_SYS_POST_UART |CONFIG_SYS_POST_MEMORY|CONFIG_SYS_POST_GPIO|\
                          CONFIG_SYS_POST_GSIO_SPI | CONFIG_SYS_POST_USB20 | CONFIG_SYS_POST_PCIE |\
                          CONFIG_SYS_POST_I2C | CONFIG_SYS_POST_MMC | CONFIG_SYS_POST_I2S | CONFIG_SYS_POST_USB30|\
                          CONFIG_SYS_POST_I2S_W8955 | CONFIG_SYS_POST_SPDIF | CONFIG_SYS_POST_RGMII |\
                          CONFIG_SYS_POST_SATA | CONFIG_SYS_POST_EMMC | CONFIG_SYS_POST_VOIP | CONFIG_SYS_POST_SGMII |\
                          CONFIG_SYS_POST_PVTMON )
#else
#define CONFIG_POST      (CONFIG_SYS_POST_PWM | CONFIG_SYS_POST_QSPI|CONFIG_SYS_POST_NAND|\
                          CONFIG_SYS_POST_UART |CONFIG_SYS_POST_MEMORY|CONFIG_SYS_POST_GPIO|\
                          CONFIG_SYS_POST_GSIO_SPI | CONFIG_SYS_POST_USB20 | CONFIG_SYS_POST_PCIE |\
                          CONFIG_SYS_POST_I2C | CONFIG_SYS_POST_MMC | CONFIG_SYS_POST_I2S | CONFIG_SYS_POST_USB30|\
                          CONFIG_SYS_POST_I2S_W8955 | CONFIG_SYS_POST_SPDIF | CONFIG_SYS_POST_RGMII |\
                          CONFIG_SYS_POST_SATA | CONFIG_SYS_POST_EMMC | CONFIG_SYS_POST_SGMII |\ 
                          CONFIG_SYS_POST_PVTMON )
#endif

#define CONFIG_CMD_DIAG
//#define CONFIG_SWANG_DEBUG_BUILD
#ifdef CONFIG_SWANG_DEBUG_BUILD
#define CONFIG_SKIP_LOWLEVEL_INIT
#define CONFIG_SYS_INIT_SP_ADDR         (0x10000- 16)
#endif
/*
#define CONFIG_DIAGS_MEMTEST_START                (CONFIG_PHYS_SDRAM_1 + 0x1000000 )//offset by 16 MB if not running from L2
#define CONFIG_DIAGS_MEMTEST_END                  0x8000000
*/

#if (defined(CONFIG_NS_PLUS))
#define CONFIG_DIAGS_MEMTEST_START                0x61000000//offset by 16 MB if not running from L2
#define CONFIG_DIAGS_MEMTEST_END                  0xA0000000
#else
#define CONFIG_DIAGS_MEMTEST_START                0x81000000//offset by 16 MB if not running from L2
#define CONFIG_DIAGS_MEMTEST_END                  0xC0000000
#endif

//Chip Common A UART 0
//#define CONFIG_SYS_NS16550_COM1                       (0x18000300)
#ifndef CONFIG_NO_COM2
//Chip Common A UART 1
#define CONFIG_SYS_NS16550_COM2                 (0x18000400)
#endif
#ifndef CONFIG_NO_COM3
//Chip Common B UART 0
#define CONFIG_SYS_NS16550_COM3                 (0x18037000)
#endif

#define  CONFIG_UART0_INDEX              1
#define  CONFIG_UART1_INDEX              2
#define  CONFIG_UART2_INDEX              3


#define CONFIG_HAS_PWM
#define CONFIG_IPROC_GPIO
#define CONFIG_IPROC_GSIOSPI


/* USB */
#define CONFIG_CMD_USB
#ifdef CONFIG_CMD_USB
#define CONFIG_USB_EHCI
#define CONFIG_USB_EHCI_IPROC
#define CONFIG_USB_STORAGE
#define CONFIG_CMD_FAT
#define CONFIG_DOS_PARTITION
#define CONFIG_LEGACY_USB_INIT_SEQ
#endif /* CONFIG_CMD_USB */

/* PCIE */
#define CONFIG_CMD_PCI
#define CONFIG_CMD_PCI_ENUM
#define CONFIG_PCI
#define CONFIG_PCI_SCAN_SHOW
#define CONFIG_IPROC_PCIE

/* I2C */
#define CONFIG_CMD_I2C
#define CONFIG_IPROC_I2C
#define CONFIG_SYS_I2C_SPEED    0    	/* Default on 100KHz */
#define CONFIG_SYS_I2C_SLAVE	0xff	/* No slave address */

/* SDIO */
#define CONFIG_CMD_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_IPROC_MMC

/* Sound */
#define CONFIG_IPROC_I2S
#define CONFIG_IPROC_PCM

/* USB3.0 */
#define CONFIG_USB_XHCI_IPROC

/* MDIO */
#define CONFIG_IPROC_MDIO

/* SATA */
#define CONFIG_IPROC_AHCI

#define CONFIG_NS_DMA
#ifndef CONFIG_NO_SLIC
#define CONFIG_IPROC_SLIC
#endif
#endif /* IPROC_BOARD_DIAGS */

#include "iproc_common_configs.h"

#define CONFIG_BOOTARGS     				"console=ttyS0,115200n8 maxcpus=2 mem=256M"
#define CONFIG_BAUDRATE 115200
//#define CONFIG_BOOTARGS     	"console=ttyS0,115200n8 maxcpus=2 mem=512M"

//#define CONFIG_BOOTDELAY		5	/* User can hit a key to abort kernel boot and stay in uboot cmdline */
//#define CONFIG_BOOTCOMMAND 		"dhcp; run nfsargs; bootm;"	/* UBoot command issued on power up */

#ifdef IPROC_BOARD_DIAGS
#define CONFIG_EXTRA_ENV_SETTINGS \
    "brcmtag=1\0"               \
    "console=ttyS0\0"           \
    "loglevel=7\0"              \
    "tftpblocksize=512\0"       \
    "hostname=northstar\0"      \
    "nfsroot=/dev/nfs rw nfsroot=/nfs/rootfs\0"        \
    "nfsargs=setenv bootargs "  \
        "loglevel=${loglevel} " \
        "console=${console},${baudrate}n8 "   \
        "ip=${ipaddr}:${serverip}:${gatewayip}:${netmask}:northstar:: " \
        "maxcpus=2 "            \
        "mem=256M "             \
        "root=${nfsroot} "      \
        "\0"                    \
    "reflash="                  \
        "dhcp ${loadaddr} u-boot.bin; " \
        "sf probe 0; "          \
        "sf erase 0 +${filesize}; " \
        "sf write ${fileaddr} 0 ${filesize};" \
        "\0"                    \
    "clr_env="                  \
        "sf probe 0;"           \
        "sf erase 0xa0000 +1;"  \
        "\0"			 
#else
#define CONFIG_EXTRA_ENV_SETTINGS \
    "brcmtag=1\0"               \
    "console=ttyS0\0"           \
    "loglevel=7\0"              \
    "tftpblocksize=512\0"       \
    "hostname=northstar\0"      \
    "nfsroot=/dev/nfs rw nfsroot=/nfs/rootfs\0"        \
    "nfsargs=setenv bootargs "  \
        "loglevel=${loglevel} " \
        "console=${console},${baudrate}n8 "   \
        "ip=${ipaddr}:${serverip}:${gatewayip}:${netmask}:northstar:: " \
        "maxcpus=2 "            \
        "mem=256M "             \
        "root=${nfsroot} "      \
        "\0"                    \
    "reflash="                  \
        "dhcp ${loadaddr} u-boot.bin; " \
        "sf probe 0; "          \
        "sf erase 0 +${filesize}; " \
        "sf write ${fileaddr} 0 ${filesize};" \
        "\0"                    \
    "clr_env="                  \
        "sf probe 0;"           \
        "sf erase 0xa0000 +1;"  \
        "\0"
#endif


//#define CONFIG_BOOTCOMMAND \
//	"if mmc rescan ${mmcdev}; then " \
//		"if run loadbootscript; then " \
//			"run bootscript; " \
//		"else " \
//			"if run loaduimage; then " \
//				"run mmcboot; " \
//			"fi; " \
//		"fi; " \
//	"fi"

#ifdef CONFIG_RUN_DDR_SHMOO2
/* Shmoo reuse: skip Shmoo process by reusing values saved in flash. */
#define CONFIG_SHMOO_REUSE
/* Define it for 32bit DDR */
#define CONFIG_SHMOO_REUSE_DDR_32BIT            0
/* Offset of spi flash to save Shmoo values */
#ifdef CONFIG_NSPLUS_SV_STDK_BUILD
#define CONFIG_SHMOO_REUSE_QSPI_OFFSET          0x00F00000
#else
#define CONFIG_SHMOO_REUSE_QSPI_OFFSET          0x000E0000
#endif
/* Offset of NAND flash to save Shmoo values */
#define CONFIG_SHMOO_REUSE_NAND_OFFSET          0x00400000
/* Range for the partition to support NAND bad blocks replacement */
#define CONFIG_SHMOO_REUSE_NAND_RANGE           0x00200000
/* Delay to wait for the magic character to force Shmoo; 0 to disable delay */
#define CONFIG_SHMOO_REUSE_DELAY_MSECS          50
/* Length of memory test after restored; 0 to disable memory test */
#define CONFIG_SHMOO_REUSE_MEMTEST_LENGTH       (0x200000)
/* Starting address of memory test after restored */
#define CONFIG_SHMOO_REUSE_MEMTEST_START        IPROC_DDR_MEM_BASE2
#endif /* CONFIG_RUN_DDR_SHMOO2 */

#endif /* __NORTHSTAR_PLUS_H */

