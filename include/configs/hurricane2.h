#ifndef __HURRICANE2_H
#define __HURRICANE2_H

#include <asm/sizes.h>

/* PCIE */
#define CONFIG_CMD_PCI
#define CONFIG_CMD_PCI_ENUM
#define CONFIG_PCI
#define CONFIG_PCI_SCAN_SHOW
#define CONFIG_IPROC_PCIE

#define CONFIG_BOARD_LATE_INIT
#define CONFIG_BOARD_EARLY_INIT_F (1)
#define DEERHOUND_SVK_SDRAM_SIZE	(0x10000000) /* 256 MB */
#define CONFIG_PHYS_SDRAM_1_SIZE	(0x20000000) /* 512 MB */
#define IPROC_ETH_MALLOC_BASE 0xD00000

/* Architecture, CPU, etc */
#define CONFIG_ARMV7
#define CONFIG_IPROC (1)
#define CONFIG_HURRICANE2 (1)

/*
Fix me

When DEBUG is enabled, need to disable both CACHE to make u-boot running
#define CONFIG_SYS_NO_ICACHE
#define CONFIG_SYS_NO_DCACHE
#define DEBUG

*/
#define CONFIG_IPROC_MMU	(1)
#define CONFIG_SYS_ARM_CACHE_WRITETHROUGH (1)
#define CONFIG_SYS_L2_PL310
#define CONFIG_SYS_PL310_BASE		(0x19022000)

#define CONFIG_MISC_INIT_R			/* Call board's misc_init_r function */

/* Interrupt configuration */
#define CONFIG_USE_IRQ          1	/* we need IRQ stuff for timer	*/

/* Memory Info */
#if (defined(CONFIG_L2C_AS_RAM ) && defined(CONFIG_NO_CODE_RELOC))
#define CONFIG_SYS_MALLOC_LEN 			0x8000  	/* see armv7/start.S. */
#else
#define CONFIG_SYS_MALLOC_LEN 			SZ_256K  	/* see armv7/start.S. */
#endif

#define CONFIG_STACKSIZE_IRQ			(4096)
#define CONFIG_STACKSIZE_FIQ			(4096)

#ifdef CONFIG_L2C_AS_RAM
#define CONFIG_PHYS_SDRAM_0				0x50000000
#define CONFIG_PHYS_SDRAM_1				0x60000000
#define CONFIG_LOADADDR					0x90000000 /* default destination location for tftp file (tftpboot cmd) */
#define CONFIG_PHYS_SDRAM_RSVD_SIZE		0x0 /* bytes reserved from CONFIG_PHYS_SDRAM_1 for custom use */
#else
#define CONFIG_PHYS_SDRAM_0				0x1b000000
#define CONFIG_PHYS_SDRAM_1				0x60000000
#define CONFIG_LOADADDR					0x70000000 /* default destination location for tftp file (tftpboot cmd) */
#define CONFIG_PHYS_SDRAM_RSVD_SIZE		0x00000000 /* bytes reserved from CONFIG_PHYS_SDRAM_1 for custom use */
#endif

/* Where kernel is loaded to in memory */
#define CONFIG_SYS_LOAD_ADDR				0x70000000
#define LINUX_BOOT_PARAM_ADDR				0x60200000

#define CONFIG_SYS_MEMTEST_START			CONFIG_PHYS_SDRAM_1
#define CONFIG_SYS_MEMTEST_END			(CONFIG_PHYS_SDRAM_1+CONFIG_PHYS_SDRAM_1_SIZE)
#define CONFIG_NR_DRAM_BANKS				1

#define CONFIG_SYS_SDRAM_BASE		(CONFIG_PHYS_SDRAM_1 + CONFIG_PHYS_SDRAM_RSVD_SIZE)
/* CONFIG_SYS_TEXT_BASE is where u-boot is loaded by boot1 */
#ifdef CONFIG_L2C_AS_RAM
#define CONFIG_SYS_STACK_SIZE		(0x0001f000)      
#else
#define CONFIG_SYS_STACK_SIZE		(0x00003000)       
#endif
#define CONFIG_SYS_INIT_SP_ADDR		(CONFIG_PHYS_SDRAM_0 + CONFIG_SYS_STACK_SIZE - 16)

/* Clocks */
#ifdef CONFIG_HURRICANE2_EMULATION
#define CONFIG_SYS_REF_CLK			(19224) /*Reference clock = 25MHz */
#define CONFIG_SYS_REF2_CLK			(19224) /*Reference clock = 25MHz */
#define IPROC_ARM_CLK		(CONFIG_SYS_REF_CLK * 4) /* 100 MHz */
#define IPROC_AXI_CLK		(CONFIG_SYS_REF_CLK * 16)  /* 400 MHz */
#define IPROC_APB_CLK		(IPROC_AXI_CLK / 4)  /* 100 MHz */
#else
#define CONFIG_SYS_REF_CLK			(25000000) /*Reference clock = 25MHz */
#define CONFIG_SYS_REF2_CLK			(200000000) /*Reference clock = 25MHz */
#define IPROC_ARM_CLK		(1000000000) /* 1GHz */
#define IPROC_AXI_CLK		(400000000)  /* 400 MHz */
#define IPROC_APB_CLK		(100000000)  /* 100 MHz */
#endif

#define CONFIG_ENV_OVERWRITE	/* Allow serial# and ethernet mac address to be overwritten in nv storage */
#ifdef CONFIG_L2C_AS_RAM
#define CONFIG_ENV_SIZE			0x1000 /* 4K */
#else
#define CONFIG_ENV_SIZE			0x10000 /* 64K */
#endif

/* NO flash */
#define CONFIG_SYS_NO_FLASH		/* Not using NAND/NOR unmanaged flash */

/* Ethernet configuration */
#define CONFIG_BCMIPROC_ETH
#define CONFIG_NET_MULTI
#define CONFIG_CMD_MII

/* DMA configuration */
//#define CONFIG_BCM5301X_DMA

/* General U-Boot configuration */
#define CONFIG_SYS_CBSIZE			1024	/* Console buffer size */
#define CONFIG_SYS_PBSIZE			(CONFIG_SYS_CBSIZE +	sizeof(CONFIG_SYS_PROMPT) + 16) /* Printbuffer size */
#define CONFIG_SYS_MAXARGS			64
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE

#define CONFIG_VERSION_VARIABLE	/* Enabled UBOOT build date/time id string */
#define CONFIG_AUTO_COMPLETE
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT_HUSH_PS2	"> "
#define CONFIG_CMDLINE_EDITING
#define CONFIG_SYS_LONGHELP

#define CONFIG_CRC32_VERIFY		/* Add crc32 to memory verify commands */
#define CONFIG_MX_CYCLIC			/* Memory display cyclic */

#define CONFIG_CMDLINE_TAG				/* ATAG_CMDLINE setup */
#define CONFIG_SETUP_MEMORY_TAGS		/* ATAG_MEM setup */


//#include <config_cmd_default.h>
#define CONFIG_ENV_IS_NOWHERE
#undef CONFIG_CMD_NFS
#undef CONFIG_GENERIC_MMC
//#define CONFIG_CMD_MISC
#include "iproc_common_configs.h"

#define CONFIG_ETHADDR		00:90:4c:06:a5:72
#define CONFIG_NETMASK		255.255.255.0
#define CONFIG_IPADDR		192.168.0.1
#define CONFIG_GATEWAYIP	192.168.0.10
#define CONFIG_SERVERIP		192.168.0.10

#ifdef UBOOT_MDK
#define CONFIG_GMAC_NUM		2 
#else
#define CONFIG_GMAC_NUM		0 
#endif

/* Enable generic u-boot SPI flash drivers and commands */
#define CONFIG_CMD_SF
#define CONFIG_SPI_FLASH
#define CONFIG_SPI_FLASH_STMICRO_NS
#define CONFIG_SPI_FLASH_MACRONIX_NS
#define CONFIG_SPI_FLASH_SPANSION
#define CONFIG_SPI_FLASH_SST
#define CONFIG_SPI_FLASH_WINBOND
#define CONFIG_SPI_FLASH_ATMEL

/* SPI flash configurations */
#define CONFIG_IPROC_QSPI
#define CONFIG_IPROC_QSPI_BUS                   0
#define CONFIG_IPROC_QSPI_CS                    0

/* SPI flash configuration - flash specific */
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
#ifdef CONFIG_NOR_IPROC_BOOT

#undef CONFIG_SYS_NO_FLASH
#define CONFIG_ENV_IS_IN_FLASH
#define CONFIG_ENV_OFFSET                       0xc0000
#define CONFIG_ENV_ADDR  (CONFIG_SYS_FLASH_BASE + 0xc0000)
#define CONFIG_ENV_SIZE  0x4000
#define CONFIG_ENV_SECT_SIZE 0x20000 /* size of one complete sector */

#else /* CONFIG_NOR_IPROC_BOOT */
#ifdef CONFIG_SPI_FLASH
#define CONFIG_ENV_IS_IN_SPI_FLASH              1
#define CONFIG_ENV_OFFSET                       0xc0000   
#define CONFIG_ENV_SPI_MAX_HZ                   10000000
#define CONFIG_ENV_SPI_MODE                     SPI_MODE_3
#define CONFIG_ENV_SPI_BUS                      CONFIG_IPROC_QSPI_BUS
#define CONFIG_ENV_SPI_CS                       CONFIG_IPROC_QSPI_CS
#define CONFIG_ENV_SECT_SIZE                    0x10000     /* 64KB */
#endif /* CONFIG_NOR_IPROC_BOOT */
#endif /* CONFIG_SPI_FLASH */
#endif /* CONFIG_NAND_BOOT */

#define CONFIG_CMD_NAND 
#define CONFIG_IPROC_NAND 
#define CONFIG_SYS_MAX_NAND_DEVICE			1
#define CONFIG_SYS_NAND_BASE		        0xdeadbeef
#define CONFIG_SYS_NAND_ONFI_DETECTION

#ifndef CONFIG_NAND_IPROC_BOOT
/* On hurricane2, NAND and NOR interface share the same chip pins - so they can't be on the same time */

/* NOR flash support */
#undef CONFIG_SYS_NO_FLASH
#define CONFIG_SYS_FLASH_QUIET_TEST

/* Board independent required settings */
#define CONFIG_SYS_FLASH_CFI                 /* Flash is CFI compliant */
#define CONFIG_FLASH_CFI_DRIVER              /* Use the common CFI flash driver */
#define CONFIG_SYS_FLASH_EMPTY_INFO  /* print 'E' for empty sector on flinfo */
#define CONFIG_CMD_FLASH

/* Board dependent required settings */
#define CONFIG_SYS_FLASH_BASE       0x20000000   /* Address flash is mapped */

/* Max number of banks -- can be greater than available, but cannot be smaller */
#define CONFIG_SYS_MAX_FLASH_BANKS   1
/* Max number of sectors -- can be greater than available, but cannot be smaller */
#define CONFIG_SYS_MAX_FLASH_SECT    512

/* Optional settings */
#define CONFIG_SYS_FLASH_PROTECTION   0           /* Use hardware flash protection */


#define CONFIG_SYS_FLASH_BANKS_LIST    {CONFIG_SYS_FLASH_BASE}
#define CONFIG_FLASH_SHOW_PROGRESS  1

#define CONFIG_SYS_FLASH_ERASE_TOUT 22000 /* Timeout for Flash Erase (in ms) */
#define CONFIG_SYS_FLASH_WRITE_TOUT 100 /* Timeout for Flash Write (in ms) */

#endif /* ifndef NAND_BOOT */

#ifdef CONFIG_RUN_DDR_SHMOO2
/* Shmoo reuse: skip Shmoo process by reusing values saved in flash. */
#define CONFIG_SHMOO_REUSE
/* Define it for 32bit DDR */
#define CONFIG_SHMOO_REUSE_DDR_32BIT            0
/* Offset of spi flash to save Shmoo values */
#define CONFIG_SHMOO_REUSE_QSPI_OFFSET          0x000E0000
/* Offset of NOR flash to save Shmoo values */
#define CONFIG_SHMOO_REUSE_NOR_OFFSET           0x000E0000
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

#endif /* __HURRICANE2_H */
