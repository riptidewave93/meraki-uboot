#ifndef __CP_TABLET_H
#define __CP_TABLET_H

#include "bcmhana_board.h"

#define BOARD_LATE_INIT

// FPGA builds should comment out this define to keep the clock speed low and to prevent
// switching the card to high speed mode. FPGA builds should uncomment this define.
#define BCMHANA_SETUP_CLOCK_FREQS_ON_CHIP 

#define CONFIG_PHYS_SDRAM_1_SIZE			0x20000000

#define CONFIG_BCMHANA_GPIO_CP_TABLET

#define CONFIG_ENV_OFFSET	0xa0000		/* 30000-b0000 - use last 10000 for env */

#define CONFIG_BCMHANA_TIMER_FREQUENCY_HZ PERIPHERAL_TIMER_FREQUENCY_HZ

#define UART_BOOTMODE	"uart_bootmode=mw 35004100 80000160\0"
#define RELOAD				"reload=run uart_bootmode; reset\0"

#define BASEINFO \
"loadaddr=80000000\0"\
"emmcdev=0\0"\
"sddev=1\0"\
"knladdr=580\0"\
"knlsize=1800\0"\
"fsaddr=22000\0"\
"ubootaddr=180\0"\
"ptaddr=0\0"\
"boot1addr=80\0"

#define START_KNL "start_knl=setenv bootargs ${bootargs} ${extraargs}; mmc read ${emmcdev} ${loadaddr} ${knladdr} ${knlsize}; bootm ${loadaddr}\0"
#define BURN_KNL		"burn_knl=mmcinfo 1; fatload mmc ${sddev} ${loadaddr} uImage; mmc write ${emmcdev} ${loadaddr} ${knladdr} ${fileblocks}\0"
#define BURN_FS		"burn_fs=mmcinfo 1; fatload mmc ${sddev} ${loadaddr} ext4; mmc write ${emmcdev} ${loadaddr} ${fsaddr} ${fileblocks}\0"
#define BURN_UBOOT	"burn_uboot=mmcinfo 1; fatload mmc ${sddev} ${loadaddr} u-boot.img; mmc write ${emmcdev} ${loadaddr} ${ubootaddr} ${fileblocks}\0"
#define BURN_PT		"burn_pt=mmcinfo 1; fatload mmc ${sddev} ${loadaddr} gpt.img; mmc write ${emmcdev} ${loadaddr} ${ptaddr} ${fileblocks}\0"
#define BURN_BOOT1	"burn_boot1=mmcinfo 1; fatload mmc ${sddev} ${loadaddr} bcmhana/binaries/hana/boot1.img_with_header.bin; mmc write ${emmcdev} ${loadaddr} ${boot1addr} ${fileblocks}\0"
#define BURN_ALL		"burn_all=run burn_knl; run burn_fs; run burn_uboot; run burn_pt; run burn_boot1; reset\0"

#define BCM_BOOTCOMMAND	"bcm_bootcmd="\
"mmcinfo ${sddev}; "\
"if run loadbootscript; then "\
	"run bootscript; "\
"else "\
	"echo Booting from mmc ... ; run start_knl; "\
"fi; \0"

#define LOADBOOTSCRIPT 	"loadbootscript=fatload mmc ${sddev} ${loadaddr} boot.scr\0" 
#define BOOTSCRIPT		"bootscript=echo Running boot.scr ...; source ${loadaddr}\0"

#define CONFIG_EXTRA_ENV_SETTINGS 		BASEINFO LOADBOOTSCRIPT BOOTSCRIPT BCM_BOOTCOMMAND START_KNL UART_BOOTMODE RELOAD BURN_KNL BURN_FS BURN_UBOOT BURN_PT BURN_BOOT1 BURN_ALL

#endif /* __CP_TABLET_H */




