#ifndef __VENOM_H
#define __VENOM_H

#include "iproc_board.h" 

#include "iproc_common_configs.h"

#define CONFIG_BOARD_EARLY_INIT_F (1)
#define CONFIG_BOARD_LATE_INIT
//#define CONFIG_PHYS_SDRAM_1_SIZE			0x08000000 /* 128 MB */
#define CONFIG_PHYS_SDRAM_1_SIZE			0x01000000 /* 16 MB */
#define IPROC_ETH_MALLOC_BASE 0xD00000


#define CONFIG_BOARD_VENOM

/* Environment variables */

#define CONFIG_ENV_IS_NOWHERE
#define CONFIG_ENV_OFFSET                       0x200000
#define CONFIG_ENV_RANGE                        0x200000

#ifdef IPROC_BOARD_DIAGS
#define CONFIG_SYS_HZ       1000*1000
#else
#define CONFIG_SYS_HZ       1000
#endif

#undef CONFIG_SYS_MALLOC_LEN
#define CONFIG_SYS_MALLOC_LEN			SZ_512K

#define CONFIG_SYS_MEM_TOP_HIDE			SZ_64K

#undef CONFIG_IPROC_QSPI
#undef CONFIG_CMD_SPI

#undef CONFIG_ETHADDR
#define CONFIG_ETHADDR			00:18:0a:00:01:02

/* Environment variables for NAND flash */
#define CONFIG_CMD_NAND 
#define CONFIG_IPROC_NAND 
#define CONFIG_SYS_MAX_NAND_DEVICE			1
#define CONFIG_SYS_NAND_BASE		        0xdeadbeef
#define CONFIG_SYS_NAND_ONFI_DETECTION

#define CONFIG_CMD_UBI
#define CONFIG_CMD_UBIFS
#define CONFIG_RBTREE
#define CONFIG_MTD_DEVICE               /* needed for mtdparts commands */
#define CONFIG_MTD_PARTITIONS
#define CONFIG_CMD_MTDPARTS
#define CONFIG_LZO

#define CONFIG_CMD_CACHE
#define CONFIG_CMD_ELF
#define CONFIG_CMD_IMI

#define CONFIG_FIT
#define CONFIG_OF_LIBFDT

#define CONFIG_BOOTP_MAY_FAIL
#undef CONFIG_SERVERIP

#undef CONFIG_CMD_USB
#undef CONFIG_USB_EHCI
#undef CONFIG_USB_EHCI_IPROC
#undef CONFIG_USB_STORAGE
#undef CONFIG_CMD_FAT
#undef CONFIG_DOS_PARTITION
#undef CONFIG_LEGACY_USB_INIT_SEQ
#undef CONFIG_CMD_MMC
#undef CONFIG_GENERIC_MMC
#undef CONFIG_IPROC_MMC

#undef MTDPARTS_DEFAULT
#define MTDPARTS_DEFAULT    "mtdparts=nand_iproc.0:2048k(U-boot),2048k(U-boot-env),2048k(U-boot-backup),2048K(U-boot-env-backup),0x7780000(ubi)"

#define CONFIG_AUTOBOOT_KEYED
#define CONFIG_AUTOBOOT_STOP_STR "xyzzy"

#define CONFIG_BOOT_RETRY_TIME 15
#define CONFIG_RESET_TO_RETRY

#undef CONFIG_STANDALONE_LOAD_ADDR
#define CONFIG_STANDALONE_LOAD_ADDR 0x81000000

#undef CONFIG_MACH_TYPE
#define CONFIG_MACH_TYPE 4804

#define CONFIG_BAUDRATE 115200
#undef CONFIG_BOOTARGS
#define CONFIG_BOOTARGS     	"console=ttyS0,115200n8 earlyprintk"

//#define CONFIG_BOOTDELAY		5	/* User can hit a key to abort kernel boot and stay in uboot cmdline */
//#define CONFIG_BOOTCOMMAND 		"dhcp; run nfsargs; bootm;"	/* UBoot command issued on power up */

//#define CONFIG_DEFAULT_L2_CACHE_LATENCY (1) /* To use default value */ // this has not been proven to be working for all boards

//#define MTDPARTS_DEFAULT    "mtdparts=nand_iproc.0:1024k(U-boot),3072k(bootkernel-1),1024k(unused),3072K(bootkernel 2),0x7780000(ubi)"
#define MERAKI_AUTOBOOT_CMD   "nand read 0x8000 0x500000 0x300000; bootbk 0x8000 bootkernel2; nand read 0x8000 0x100000 0x300000; bootbk 0x8000 bootkernel1"
#define CONFIG_IDENT_STRING   " Meraki bootkernel loader"

#define CONFIG_EXTRA_ENV_SETTINGS \
	"brcmtag=0\0"               \
	"console=ttyS0\0"           \
	"loglevel=7\0"              \
	"tftpblocksize=512\0"       \
	"hostname=venom\0"	    \
	"vlan1ports=0 1 2 3 8u\0"   \
	"netloadmethod=dhcp\0" \
	"ubootfile=u-boot-venom\0" \
	"ubootsize=0x00200000\0" \
	"loaduboot=${netloadmethod} ${meraki_loadaddr} ${serverpath}${ubootfile}\0" \
	"eraseboot=nand erase 0x0 ${ubootsize}\0" \
	"writeboot=nand write ${meraki_loadaddr} 0x0 ${filesize}\0" \
	"updateuboot=run loaduboot eraseboot writeboot\0" \
	"meraki_loadaddr=0x04000000\0"		  \
	"mtdparts=" MTDPARTS_DEFAULT "\0" \
	"meraki_bootfile=venom.itb\0" \
	"meraki_bootlinux=bootm ${meraki_loadaddr}\0" \
	"meraki_boot=run meraki_ubi meraki_load_script meraki_run_script; " \
	"run meraki_load_part_safe meraki_bootlinux; " \
	"run meraki_load_part_new meraki_bootlinux; " \
	"run meraki_load_part_old meraki_bootlinux; " \
	"run meraki_load_part1 meraki_bootlinux; " \
	"run meraki_load_part2 meraki_bootlinux; " \
	"run meraki_netboot;" \
	"run meraki_netboot_static\0" \
	"meraki_ubi=ubi part ubi\0" \
	"meraki_load_part_safe=ubi read ${meraki_loadaddr} part.safe\0" \
	"meraki_load_part_new=ubi read ${meraki_loadaddr} part.new\0" \
	"meraki_load_part_old=ubi read ${meraki_loadaddr} part.old\0" \
	"meraki_load_part1=ubi read ${meraki_loadaddr} part1\0" \
	"meraki_load_part2=ubi read ${meraki_loadaddr} part2\0" \
	"meraki_load_net=${netloadmethod} ${meraki_loadaddr} ${serverpath}${meraki_bootfile}\0" \
	"meraki_load_script=ubi read ${meraki_loadaddr} bootscr\0" \
	"meraki_netboot=run meraki_load_net meraki_bootlinux\0" \
	"meraki_netboot_static=run meraki_set_static_ip meraki_tftp_load_net meraki_bootlinux\0" \
	"meraki_run_script=source ${meraki_loadaddr}\0" \
	"meraki_set_static_ip=setenv ipaddr 10.128.128.128; " \
	"setenv netmask 255.0.0.0\0" \
	"meraki_tftp_load_net=tftpboot ${meraki_loadaddr} ${serverpath}${meraki_bootfile}\0" \
	"factory_boot=run meraki_ubi factory_load meraki_bootlinux\0" \
	"factory_load=ubi read ${meraki_loadaddr} mr26-wnc-mfg\0" \
	"bootcmd=run meraki_boot\0"

#undef CONFIG_GZIP
#undef CONFIG_GZIP_COMPRESSED
#undef CONFIG_LZO
#undef CONFIG_CMD_LICENSE
#undef CONFIG_AUTO_COMPLETE
#undef CONFIG_CMD_IMPORTENV
#undef CONFIG_CMD_EXPORTENV
#undef CONFIG_ENV_IS_IN_NAND
#undef CONFIG_CMD_MEMORY
#undef CONFIG_SYS_VSNPRINTF
#undef CONFIG_CMD_UBI
#undef CONFIG_CMD_NET
#undef CONFIG_CMD_PING
#undef CONFIG_CMD_MTDPARTS
#undef CONFIG_MTD_DEVICE
#undef CONFIG_BOOTM_NETBSD
#undef CONFIG_HAS_POST
#undef CONFIG_POST_STD_LIST
#undef CONFIG_POST

#undef CONFIG_HAS_PWM
#undef CONFIG_IPROC_GSIOSPI


/* USB */
#undef CONFIG_CMD_USB

/* PCIE */
#undef CONFIG_CMD_PCI
#undef CONFIG_CMD_PCI_ENUM
#undef CONFIG_PCI
#undef CONFIG_PCI_SCAN_SHOW
#undef CONFIG_IPROC_PCIE


/* SDIO */
#undef CONFIG_CMD_MMC
#undef CONFIG_GENERIC_MMC
#undef CONFIG_IPROC_MMC

/* USB3.0 */
#undef CONFIG_USB_XHCI_IPROC

/* QSPI */
#undef CONFIG_CMD_SPI
#undef CONFIG_IPROC_QSPI

/* MDIO */
#undef CONFIG_IPROC_MDIO

#undef CONFIG_CMD_UBIFS
#undef CONFIG_RBTREE
#undef CONFIG_MTD_DEVICE               /* needed for mtdparts commands */
#undef CONFIG_MTD_PARTITIONS
#undef CONFIG_CMD_MTDPARTS
#undef CONFIG_LZO
#undef CONFIG_CMD_ELF
#undef CONFIG_FIT
#undef CONFIG_OF_LIBFDT

/* libnet */
#define CONFIG_BCMIPROC_ETH
#undef CONFIG_NET_MULTI
#undef CONFIG_CMDLINE_EDITING
#undef CONFIG_SYS_LONGHELP
#undef CONFIG_CMD_NFS

/* lib fs */
#undef CONFIG_CMD_CRAMFS
#undef CONFIG_CMD_EXT4
#undef CONFIG_CMD_EXT2
#undef CONFIG_CMD_FAT
#undef CONFIG_CMD_FDOS
#undef CONFIG_CMD_JFFS2
#undef CONFIG_CMD_REISER
#undef CONFIG_YAFFS2
#undef CONFIG_CMD_UBIFS
#undef CONFIG_CMD_ZFS

/* libdisk */
#undef CONFIG_PARTITIONS
#undef CONFIG_MAC_PARTITION
#undef CONFIG_DOS_PARTITION
#undef CONFIG_ISO_PARTITION
#undef CONFIG_AMIGA_PARTITION
#undef CONFIG_EFI_PARTITION

/* libblock */
#undef CONFIG_SCSI_AHCI
#undef CONFIG_ATA_PIIX
#undef CONFIG_DWC_AHSATA
#undef CONFIG_FSL_SATA
#undef CONFIG_IDE_FTIDE020
#undef CONFIG_LIBATA
#undef CONFIG_MVSATA_IDE
#undef CONFIG_MX51_PATA
#undef CONFIG_PATA_BFIN
#undef CONFIG_SATA_DWC
#undef CONFIG_SATA_SIL3114
#undef CONFIG_SATA_SIL
#undef CONFIG_IDE_SIL680
#undef CONFIG_SCSI_SYM53C8XX
#undef CONFIG_SYSTEMACE
#undef CONFIG_IPROC_AHCI
#undef CONFIG_IPROC_AHCI

/* libfpga */
#undef CONFIG_FPGA

/* libgpio */
#undef CONFIG_IPROC_GPIO

/* libi2c */
#undef CONFIG_CMD_I2C
#undef CONFIG_IPROC_I2C

/* libsound */
#undef CONFIG_IPROC_I2S
#undef CONFIG_IPROC_PCM
#undef CONFIG_NS_DMA

#undef CONFIG_CMD_TFTPSRV
#undef CONFIG_CMD_WNC

#undef CONFIG_AUTOBOOT_KEYED
#undef CONFIG_AUTOBOOT_STOP_STR

#undef CONFIG_ETHADDR
#undef CONFIG_CMD_ITEST

#undef CONFIG_SYS_HUSH_PARSER
#undef CONFIG_ZLIB
#undef CONFIG_CMD_BOOTM
#undef CONFIG_EXTRA_ENV_SETTINGS
#undef MTDPARTS_DEFAULT

#endif /* __VENOM_H */



	
