#ifndef __BCM95301X_SVK_H
#define __BCM95301X_SVK_H

#include "iproc_board.h" 

#define CONFIG_BOARD_EARLY_INIT_F (1)
#define CONFIG_BOARD_LATE_INIT
#define CONFIG_PHYS_SDRAM_1_SIZE			0x01000000 /* 16 MB */
#define IPROC_ETH_MALLOC_BASE 0xD00000

#undef CONFIG_ENV_IS_NOWHERE

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
#define CONFIG_IPROC_BSPI_DATA_LANES            1
#define CONFIG_IPROC_BSPI_ADDR_LANES            1
#define CONFIG_IPROC_BSPI_READ_CMD              0x0b
#define CONFIG_IPROC_BSPI_READ_DUMMY_CYCLES     8
#define CONFIG_SF_DEFAULT_SPEED                 50000000
#define CONFIG_SF_DEFAULT_MODE                  SPI_MODE_3

/* Environment variables */
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
#endif /* CONFIG_NAND_BOOT */

#ifdef IPROC_BOARD_DIAGS
#define CONFIG_SYS_HZ       1000*1000
#else
#define CONFIG_SYS_HZ       1000
#endif

#if 1 /* Enable NVRAM subsystem for MWG support*/
#define CONFIG_NVRAM_WL
#define CONFIG_NAND_FLASH_BASE_ADDR         0x1c000000
#define CONFIG_NVRAM_OFFSET                 0x180000
#define CONFIG_NVRAM_PARTITION_SIZE         0x80000
#if WNC_DEL // Steve 2013/07/10
#define CONFIG_NVRAM_SIZE                   0x4000
#endif // WNC_DEL
// WNC Steve 2013/07/10
#define CONFIG_NVRAM_SIZE                   0x10000
// WNC
#if WNC_DEL // Steve 2013/07/24
#define CONFIG_ENV_SIZE                     0x4000
#endif // WNC_DEL
#endif

/* Environment variables for NAND flash */
#define CONFIG_CMD_NAND 
#define CONFIG_IPROC_NAND 
#define CONFIG_SYS_MAX_NAND_DEVICE			1
#define CONFIG_SYS_NAND_BASE		        0xdeadbeef
#define CONFIG_SYS_NAND_ONFI_DETECTION

#include "iproc_common_configs.h"

#undef CONFIG_STANDALONE_LOAD_ADDR
#define CONFIG_STANDALONE_LOAD_ADDR 0x81000000


#define CONFIG_BAUDRATE 115200
//#define CONFIG_BOOTARGS     	"console=ttyS0,115200n8 maxcpus=2 mem=512M"

//#define CONFIG_BOOTDELAY		5	/* User can hit a key to abort kernel boot and stay in uboot cmdline */
//#define CONFIG_BOOTCOMMAND 		"dhcp; run nfsargs; bootm;"	/* UBoot command issued on power up */

//#define CONFIG_DEFAULT_L2_CACHE_LATENCY (1) /* To use default value */ // this has not been proven to be working for all boards

#define CONFIG_EXTRA_ENV_SETTINGS \
	"brcmtag=0\0"               \
	"console=ttyS0\0"           \
    "loglevel=7\0"              \
    "tftpblocksize=512\0"       \
    "hostname=northstar\0"      \
    "vlan1ports=0 1 2 3 8u\0"

#endif /* __BCM95301X_SVK_H */



	
