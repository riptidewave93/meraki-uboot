/*
 * (C) Copyright 2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
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
 *
 * Be sure to mark tests to be run before relocation as such with the
 * CONFIG_SYS_POST_PREREL flag so that logging is done correctly if the
 * logbuffer support is enabled.
 */

#include <common.h>

#include <post.h>

extern int ocm_post_test (int flags);
extern int cache_post_test (int flags);
extern int watchdog_post_test (int flags);
extern int i2c_post_test (int flags);
extern int rtc_post_test (int flags);
extern int memory_post_test (int flags);
extern int cpu_post_test (int flags);
extern int fpu_post_test (int flags);
extern int uart_post_test (int flags);
extern int ether_post_test (int flags);
extern int spi_post_test (int flags);
extern int usb_post_test (int flags);
extern int spr_post_test (int flags);
extern int sysmon_post_test (int flags);
extern int dsp_post_test (int flags);
extern int codec_post_test (int flags);
extern int ecc_post_test (int flags);
extern int flash_post_test(int flags);

extern int dspic_init_post_test (int flags);
extern int dspic_post_test (int flags);
extern int gdc_post_test (int flags);
extern int fpga_post_test (int flags);
extern int lwmon5_watchdog_post_test(int flags);
extern int sysmon1_post_test(int flags);
extern int coprocessor_post_test(int flags);

#ifdef IPROC_BOARD_DIAGS
extern int qspi_post_test (int flags);
extern int NAND_post_test (int flags);
extern int UART_post_test (int flags);
extern int PWM_post_test (int flags);
extern int GPIO_post_test (int flags);
extern int gsiospi_post_test(int flags);
extern int USB20_post_test(int flags);
extern int USB30_post_test(int flags);
extern int PCIE_post_test(int flags);
extern int MMC_post_test(int flags);
extern int I2S_post_test(int flags);
extern int rgmii_post_test (int flags);
extern int sata_post_test (int flags);
extern int I2S__w8955_post_test(int flags);
extern int spdif_post_test (int flags);
extern int voip_post_test( int flags );
extern int eMMC_post_test( int flags );
extern int sgmii_post_test ( int flags );
extern int pvtmon_post_test( int flags );
#endif
extern int led_post_test(int flags);
extern int button_post_test(int flags);
extern int memory_regions_post_test(int flags);

extern int sysmon_init_f (void);

extern void sysmon_reloc (void);

#ifdef IPROC_BOARD_DIAGS
struct post_test post_list[] =
{
#if CONFIG_POST & CONFIG_SYS_POST_PWM
    {
        "PWM test",
        "pwm",
        "This test checks on four PWM channels.",
        POST_RAM | POST_MANUAL | POST_SEMI_AUTO,
        &PWM_post_test,
        NULL,
        NULL,
        CONFIG_SYS_POST_PWM
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_GPIO
    {
        "GPIO test",
        "gpio",
        "This test verifies the GPIO operation.",
        POST_RAM | POST_MANUAL | POST_AUTO,
        &GPIO_post_test,
        NULL,
        NULL,
        CONFIG_SYS_POST_GPIO
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_GSIO_SPI
    {
        "GSIO SPI test",
        "gsio",
        "This test checks if the the WM8955 register can be written through the GSIO-SPI interface.",
        POST_RAM | POST_MANUAL,
        &gsiospi_post_test,
        NULL,
        NULL,
        CONFIG_SYS_POST_GSIO_SPI
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_NAND
    {
        "NAND test",
        "nand",
        "This test verifies the NAND operation.",
        POST_RAM | POST_MANUAL | POST_AUTO,
        &NAND_post_test,
        NULL,
        NULL,
        CONFIG_SYS_POST_NAND
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_QSPI
    {
        "QSPI test",
        "qspi",
        "This test verifies the qspi operation.",
        POST_RAM | POST_MANUAL| POST_AUTO,
        &qspi_post_test,
        NULL,
        NULL,
        CONFIG_SYS_POST_QSPI
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_MEMORY
    {
        "Memory test",
        "memory",
        "This test checks RAM between 0x8100-0000 to 0xC000-0000.",
        POST_RAM | POST_MANUAL | POST_AUTO,
        &memory_post_test,
        NULL,
        NULL,
        CONFIG_SYS_POST_MEMORY
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_I2C
    {
        "I2C test",
        "i2c",
        "This test verifies the connectivity of I2C interface",
        POST_RAM | POST_MANUAL | POST_AUTO,
        &i2c_post_test,
        NULL,
        NULL,
        CONFIG_SYS_POST_I2C
    },
#endif
#if defined(CONFIG_CMD_USB) && (CONFIG_POST & CONFIG_SYS_POST_USB2)
    {
        "USB 2.0 test",
        "usb2",
        "This test verifies the connectivity of the USB 2.0 link and USB 2.0 functions",
        POST_RAM | POST_MANUAL | POST_AUTO,
        &USB20_post_test,
        NULL,
        NULL,
        CONFIG_SYS_POST_USB20
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_PCIE
    {
        "PCIE test",
        "pcie",
        "This test verifies the connectivity of PCI-Express functions",
        POST_RAM | POST_MANUAL | POST_AUTO,
        &PCIE_post_test,
        NULL,
        NULL,
        CONFIG_SYS_POST_PCIE
    },
#endif
#if defined(CONFIG_CMD_MMC) && (CONFIG_POST & CONFIG_SYS_POST_MMC)
    {
        "MMC/SD test",
        "sd",
        "This test verifies the connectivity of MMC/SD functions",
        POST_RAM | POST_MANUAL | POST_AUTO,
        &MMC_post_test,
        NULL,
        NULL,
        CONFIG_SYS_POST_MMC
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_I2S
    {
        "I2S through wm8750 test",
        "i2s",
        "This test verifies the connectivity of the i2s interface and functions",
        POST_RAM | POST_MANUAL | POST_SEMI_AUTO,
        &I2S_post_test,
        NULL,
        NULL,
        CONFIG_SYS_POST_I2S
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_I2S_W8955
    {
        "I2S through Wolfson 8955 test",
        "I2S8955",
        "This test verifies the connectivity of the i2s interface and functions through wm8955",
        POST_RAM | POST_MANUAL |POST_SEMI_AUTO,
        &I2S__w8955_post_test,
        NULL,
        NULL,
        CONFIG_SYS_POST_I2S_W8955
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_UART
#if defined(CONFIG_POST_UART)
        CONFIG_POST_UART,
#else
    {
        "UART test",
        "uart",
        "This test verifies the UART operation for console 2 and 3.",
        POST_RAM | POST_MANUAL,
        &UART_post_test,
        NULL,
        NULL,
        CONFIG_SYS_POST_UART
    },
#endif /* CONFIG_POST_UART */
#endif
#if CONFIG_POST & CONFIG_SYS_POST_RGMII
    {
        "RGMII test",
        "rgmii",
        "This test verifies the connectivity of the RGMII interface and functions",
        POST_RAM | POST_MANUAL,// | POST_AUTO,
        &rgmii_post_test,
        NULL,
        NULL,
        CONFIG_SYS_POST_RGMII
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_SATA
    {
        "SATA test",
        "sata",
        "This test verifies the connectivity of the SATA interface and functions",
        POST_RAM | POST_MANUAL,
        &sata_post_test,
        NULL,
        NULL,
        CONFIG_SYS_POST_SATA
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_USB30
    {
        "USB 3.0 test",
        "usb3",
        "This test verifies the USB 3.0 link and operation.",
        POST_RAM | POST_MANUAL|POST_SEMI_AUTO,
        &USB30_post_test,
        NULL,
        NULL,
        CONFIG_SYS_POST_USB30
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_SPDIF
    {
        "SPDIF test",
        "spdif",
        "This test verifies the SPDIF operation.",
        POST_RAM | POST_MANUAL | POST_SEMI_AUTO,
        &spdif_post_test,
        NULL,
        NULL,
        CONFIG_SYS_POST_SPDIF
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_VOIP
    {
        "voip test",
        "voip",
        "This test checks the PCM/VOIP functions.",
        POST_RAM | POST_MANUAL | POST_SEMI_AUTO,
        &voip_post_test,
        NULL,
        NULL,
        CONFIG_SYS_POST_VOIP
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_EMMC
    {
        "eMMC test",
        "emmc",
        "This test verifies the connectivity of eMMC functions",
        POST_RAM | POST_MANUAL,//| POST_AUTO,
        &eMMC_post_test,
        NULL,
        NULL,
        CONFIG_SYS_POST_EMMC
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_SGMII
    {
        "SGMII test",
        "sgmii",
        "This test verifies the connectivity of SGMII ports",
        POST_RAM | POST_MANUAL,//| POST_AUTO,
        &sgmii_post_test,
        NULL,
        NULL,
        CONFIG_SYS_POST_SGMII
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_PVTMON 
    {
        "PVT monitor test",
        "pvtmon",
        "This test reads the temperature and voltage from the onchip analog IP",
        POST_RAM | POST_MANUAL,//| POST_AUTO,
        &pvtmon_post_test,
        NULL,
        NULL,
        CONFIG_SYS_POST_PVTMON 
    },
#endif
};
//
#else
struct post_test post_list[] =
{
#if CONFIG_POST & CONFIG_SYS_POST_OCM
    {
	"OCM test",
	"ocm",
	"This test checks on chip memory (OCM).",
	POST_ROM | POST_ALWAYS | POST_PREREL | POST_CRITICAL | POST_STOP,
	&ocm_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_OCM
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_CACHE
    {
	"Cache test",
	"cache",
	"This test verifies the CPU cache operation.",
	POST_RAM | POST_ALWAYS,
	&cache_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_CACHE
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_WATCHDOG
#if defined(CONFIG_POST_WATCHDOG)
	CONFIG_POST_WATCHDOG,
#else
    {
	"Watchdog timer test",
	"watchdog",
	"This test checks the watchdog timer.",
	POST_RAM | POST_POWERON | POST_SLOWTEST | POST_MANUAL | POST_REBOOT,
	&watchdog_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_WATCHDOG
    },
#endif
#endif
#if CONFIG_POST & CONFIG_SYS_POST_NAND
    {
	"NAND test",
	"NAND",
	"This test verifies the NAND operation.",
	POST_RAM | POST_MANUAL,
	&NAND_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_NAND
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_QSPI
    {
	"QSPI test",
	"qspi",
	"This test verifies the qspi operation.",
	POST_RAM | POST_MANUAL,
	&qspi_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_QSPI
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_MEMORY
    {
	"Memory test",
	"memory",
	"This test checks RAM.",
	POST_RAM | POST_MANUAL,
	&memory_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_MEMORY
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_CPU
    {
	"CPU test",
	"cpu",
	"This test verifies the arithmetic logic unit of"
	" CPU.",
	POST_RAM | POST_ALWAYS,
	&cpu_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_CPU
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_FPU
    {
	"FPU test",
	"fpu",
	"This test verifies the arithmetic logic unit of"
	" FPU.",
	POST_RAM | POST_ALWAYS,
	&fpu_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_FPU
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_UART
#if defined(CONFIG_POST_UART)
	CONFIG_POST_UART,
#else
    {
	"UART test",
	"uart",
	"This test verifies the UART operation.",
	POST_RAM | POST_SLOWTEST | POST_MANUAL,
	&UART_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_UART
    },
#endif /* CONFIG_POST_UART */
#endif
#if CONFIG_POST & CONFIG_SYS_POST_ETHER
    {
	"ETHERNET test",
	"ethernet",
	"This test verifies the ETHERNET operation.",
	POST_RAM | POST_ALWAYS | POST_MANUAL,
	&ether_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_ETHER
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_SPI
    {
	"SPI test",
	"spi",
	"This test verifies the SPI operation.",
	POST_RAM | POST_ALWAYS | POST_MANUAL,
	&spi_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_SPI
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_USB
    {
	"USB test",
	"usb",
	"This test verifies the USB operation.",
	POST_RAM | POST_ALWAYS | POST_MANUAL,
	&usb_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_USB
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_SPR
    {
	"SPR test",
	"spr",
	"This test checks SPR contents.",
	POST_RAM | POST_ALWAYS,
	&spr_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_SPR
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_SYSMON
    {
	"SYSMON test",
	"sysmon",
	"This test monitors system hardware.",
	POST_RAM | POST_ALWAYS,
	&sysmon_post_test,
	&sysmon_init_f,
	&sysmon_reloc,
	CONFIG_SYS_POST_SYSMON
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_DSP
    {
	"DSP test",
	"dsp",
	"This test checks any connected DSP(s).",
	POST_RAM | POST_ALWAYS | POST_MANUAL,
	&dsp_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_DSP
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_CODEC
    {
	"CODEC test",
	"codec",
	"This test checks any connected codec(s).",
	POST_RAM | POST_MANUAL,
	&codec_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_CODEC
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_ECC
    {
	"ECC test",
	"ecc",
	"This test checks the ECC facility of memory.",
	POST_ROM | POST_ALWAYS | POST_PREREL,
	&ecc_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_ECC
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_BSPEC1
	CONFIG_POST_BSPEC1,
#endif
#if CONFIG_POST & CONFIG_SYS_POST_BSPEC2
	CONFIG_POST_BSPEC2,
#endif
#if CONFIG_POST & CONFIG_SYS_POST_BSPEC3
	CONFIG_POST_BSPEC3,
#endif
#if CONFIG_POST & CONFIG_SYS_POST_BSPEC4
	CONFIG_POST_BSPEC4,
#endif
#if CONFIG_POST & CONFIG_SYS_POST_BSPEC5
	CONFIG_POST_BSPEC5,
#endif
#if CONFIG_POST & CONFIG_SYS_POST_COPROC
    {
	"Coprocessors communication test",
	"coproc_com",
	"This test checks communication with coprocessors.",
	POST_RAM | POST_ALWAYS | POST_CRITICAL,
	&coprocessor_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_COPROC
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_FLASH
    {
	"Parallel NOR flash test",
	"flash",
	"This test verifies parallel flash operations.",
	POST_RAM | POST_SLOWTEST | POST_MANUAL,
	&flash_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_FLASH
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_MEM_REGIONS
    {
	"Memory regions test",
	"mem_regions",
	"This test checks regularly placed regions of the RAM.",
	POST_ROM | POST_SLOWTEST | POST_PREREL,
	&memory_regions_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_MEM_REGIONS
    },
#endif
};
#endif

unsigned int post_list_size = ARRAY_SIZE(post_list);
