/*
 * (C) Copyright 2008
 * Stefan Roese, DENX Software Engineering, sr@denx.de.
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
 */

#include <common.h>
#include <ppc440.h>
#include <libfdt.h>
#include <fdt_support.h>
#include <i2c.h>
#include <asm/processor.h>
#include <asm/io.h>
#include <pca9670.h>
#include <asm/mmu.h>
#include <asm/4xx_pcie.h>
#include <asm/gpio.h>

#include <net.h>

#if !defined(CONFIG_SYS_NO_FLASH)
extern flash_info_t flash_info[CONFIG_SYS_MAX_FLASH_BANKS]; /* info for FLASH chips */
#endif

DECLARE_GLOBAL_DATA_PTR;

#define SDR_AHB_CFG	0x370

/* Board type */
#define BOARD_BLUESTONE_SATA0_SATA1	1	/* APM82181 2 SATA */
#define BOARD_BLUESTONE_PCIE0_SATA1	2	/* APM82181 1 PCIE and 1 SATA*/
#define BOARD_BLUESTONE_x_SATA1		3	/* APM82161 1 SATA */

/* Define Boot devices */
#define BOOT_FROM_8BIT_SRAM	0x00
#define BOOT_FROM_8BIT_NAND	0x01
#define BOOT_FROM_8BIT_NOR	0x02
#define BOOT_DEVICE_UNKNOWN	0xff

/* Peripheral Bank Access Parameters - EBC_BxAP */
/* 8-bit SRAM */
#define EBC_BXAP_SRAM              EBC_BXAP_BME_DISABLED   | \
                                        EBC_BXAP_TWT_ENCODE(8)  | \
                                        EBC_BXAP_BCE_DISABLE    | \
                                        EBC_BXAP_BCT_2TRANS     | \
                                        EBC_BXAP_CSN_ENCODE(0)  | \
                                        EBC_BXAP_OEN_ENCODE(1)  | \
                                        EBC_BXAP_WBN_ENCODE(1)  | \
                                        EBC_BXAP_WBF_ENCODE(3)  | \
                                        EBC_BXAP_TH_ENCODE(1)   | \
                                        EBC_BXAP_RE_DISABLED    | \
                                        EBC_BXAP_SOR_NONDELAYED | \
                                        EBC_BXAP_BEM_RW         | \
                                        EBC_BXAP_PEN_DISABLED

/* NAND Flash */
#define EBC_BXAP_NAND	0x018003c0

/* NOR Flash */
#define EBC_BXAP_NOR                    EBC_BXAP_BME_DISABLED   | \
                                        EBC_BXAP_TWT_ENCODE(64)  | \
                                        EBC_BXAP_BCE_DISABLE    | \
                                        EBC_BXAP_BCT_2TRANS     | \
                                        EBC_BXAP_CSN_ENCODE(1)  | \
                                        EBC_BXAP_OEN_ENCODE(2)  | \
                                        EBC_BXAP_WBN_ENCODE(2)  | \
                                        EBC_BXAP_WBF_ENCODE(2)  | \
                                        EBC_BXAP_TH_ENCODE(7)   | \
                                        EBC_BXAP_SOR_DELAYED    | \
                                        EBC_BXAP_BEM_WRITEONLY  | \
                                        EBC_BXAP_PEN_DISABLED
/* This value seems good now */
#undef EBC_BXAP_NOR
#define EBC_BXAP_NOR			0x10055e40
/* Peripheral Bank Configuration Register - EBC_BxCR */
#define EBC_BXCR_8BIT_NOR	EBC_BXCR_BAS_ENCODE(CONFIG_SYS_FLASH_BASE) | \
                                EBC_BXCR_BS_1MB                | \
                                EBC_BXCR_BU_RW                  | \
                                EBC_BXCR_BW_8BIT

#define EBC_BXCR_8BIT_SRAM	EBC_BXCR_BAS_ENCODE(CONFIG_SYS_EXTSRAM_BASE) | \
                                EBC_BXCR_BS_1MB                 | \
                                EBC_BXCR_BU_RW                  | \
                                EBC_BXCR_BW_8BIT

#define EBC_BXCR_8BIT_NAND0	EBC_BXCR_BAS_ENCODE(CONFIG_SYS_NAND0_ADDR) | \
                                EBC_BXCR_BS_2MB                | \
                                EBC_BXCR_BU_RW                  | \
                                EBC_BXCR_BW_32BIT

#define EBC_BXCR_8BIT_NAND1     EBC_BXCR_BAS_ENCODE(CONFIG_SYS_NAND1_ADDR) | \
                                EBC_BXCR_BS_2MB                | \
                                EBC_BXCR_BU_RW                  | \
                                EBC_BXCR_BW_32BIT

/* Bus Switch Pins*/
//#define MERAKI_IKAREM   	1               //Alex:20101125
#define MERAKI_BUCKMINISTER   	1               //Alex:20110315

#if !(defined(MERAKI_IKAREM) || defined(MERAKI_BUCKMINISTER))
#define BLUESTONE_USBOTGDrvVBus	5   /* USBOTGDrvVBus 	*/
#define BLUESTONE_RTC_INT_L 	7
#define BLUESTONE_UART_EN		8
#define BLUESTONE_SW_GPIO09		9
#define BLUESTONE_SW_GPIO10		10
#define BLUESTONE_SW_GPIO11		11
#define BLUESTONE_SW_GPIO12		12
#define BLUESTONE_SW_GPIO13		13
#define BLUESTONE_SW_EN_UART1	14
#define BLUESTONE_SW_SPI_CS_L	15
#define BLUESTONE_SW_SPI_I2C1	16
#define BLUESTONE_USB_OTG_ID	17
#define BLUESTONE_CPU_ALERT_L 	18
#define BLUESTONE_CPU_THERM_L 	19
#define BLUESTONE_USB_OC_L		20
#define BLUESTONE_PWR_SW_L		21
#define BLUESTONE_PCIE0_WAKE_L 	23

#else


#if defined(MERAKI_IKAREM) 
#define IKAREM_WD_CLK_INPUT     3
#define IKAREM_AT_AF_IND        5
#define IKAREM_WD_EN            7
#define IKAREM_POE_DET          8
#define IKAREM_BOARD_ID0        9
#define IKAREM_BOARD_ID1        10
#define IKAREM_PCIE1_WL_DIS     11
#define IKAREM_UART1_RX         12
#define IKAREM_UART1_TX         13
#define IKAREM_PCIE0_WL_DIS     14
#define IKAREM_ADAPTER_DET      15
#define IKAREM_RST_DEFAULT      16
#define IKAREM_LAN_GLED         17
#define IKAREM_PWR_GLED         18
#define IKAREM_PWR_YLED         19
#define IKAREM_LED1_GLED        20
#define IKAREM_LED2_GLED        21
#define IKAREM_LED3_GLED        22
#define IKAREM_LED4_GLED        23
#endif

#if defined(MERAKI_BUCKMINISTER) 
#define BUCKMINISTER_PCS_1             3
#define BUCKMINISTER_USB_PSW           5
#define BUCKMINISTER_WD_EN             7
#define BUCKMINISTER_USB_OC            8
#define BUCKMINISTER_BOARD_ID0         9
#define BUCKMINISTER_BOARD_ID1        10
#define BUCKMINISTER_WD_CLK_INPUT     11
#define BUCKMINISTER_UART1_RX         12
#define BUCKMINISTER_UART1_TX         13
#define BUCKMINISTER_GPHY_INT         14
#define BUCKMINISTER_WAN_YLED         15
#define BUCKMINISTER_RST_DEFAULT      16
#define BUCKMINISTER_WAN_GLED         17
#define BUCKMINISTER_PWR_GLED         18
#define BUCKMINISTER_PWR_YLED         19
#define BUCKMINISTER_LED1_GLED        20
#define BUCKMINISTER_LED2_GLED        21
#define BUCKMINISTER_LED3_GLED        22
#define BUCKMINISTER_LED4_GLED        23
#endif

#endif

/* IO Expander PCA9670 pins*/
#define PCA9670_SHUTDOWN_PIN	0
#define PCA9670_PCIE0_PERST_L	1
#define PCA9670_HD01_PWR_EN		2
#define PCA9670_HDD_ACTIVITY	3
#define PCA9670_USER_LED1		4
#define PCA9670_USER_LED2		5
#define PCA9670_USER_LED3		6
#define PCA9670_GPIO_SEL		7

void gpio_exp_config(int pin, int in_out, int gpio_alt, int out_val) {
	/* not implement */
}

void gpio_exp_write_bit(int pin, int val) {
	int pos_bit;

	pos_bit = (0x01 << pin);

	pca9670_set_val(CONFIG_SYS_I2C_PCA9670_ADDR, pos_bit, (val ? pos_bit : 0));
}

int gpio_exp_read_out_bit(int pin) {
	/* not implement*/
	return 0;
}

int gpio_exp_read_in_bit(int pin) {
	int val;
	int pos_bit;

	val = pca9670_get_val(CONFIG_SYS_I2C_PCA9670_ADDR);

	pos_bit = (0x01 << pin);

	return ((pos_bit & val) ? pos_bit : 0);
}

void gpio_exp_set_chip_configuration(void) {
	/* not implement*/
}

/*
 * Get bootdevice information and 
 * setup EBC configuration 
 * NOTE: Assume that I2C had been initialized 
 */
int bootdevice_selected(void) {
	unsigned long rl;
	unsigned long bootstrap_settings;
	int computed_boot_device = BOOT_DEVICE_UNKNOWN;

	/* 
	 * UserInput Jumper is used by CPLD to configure the bootdevice 
	 * for the board
	 */
	mfsdr(sdr_sdstp1, bootstrap_settings);
	rl = SDR_SDSTP1_RL_DECODE(bootstrap_settings);
	if (rl == SDR_SDSTP1_RL_EBC) { /* Boot device is NOR/SRAM */
		computed_boot_device = BOOT_FROM_8BIT_NOR;
	} else if (rl == SDR_SDSTP1_RL_NDFC)
		computed_boot_device = BOOT_FROM_8BIT_NAND;

	return computed_boot_device;
}

void reconfigure_EBC(int computed_boot_device) {

	switch (computed_boot_device) {
	case BOOT_FROM_8BIT_NOR:
		/*
		 * CS0: NOR
		 * CS1: x
		 * CS2: x
		 */
		/* Configure EBC CS0 - Nor Flash 1MBx8 */
		mtebc(pb0ap, EBC_BXAP_NOR);
		mtebc(pb0ap, EBC_BXAP_NOR);

		mtebc(pb0cr, EBC_BXCR_8BIT_NOR);

		/* NAND and SRAM cannot be accessed in this boot option */
		mtebc(pb1ap, EBC_BXAP_NAND);
		mtebc(pb1cr, EBC_BXCR_8BIT_NAND0);
		mtebc(pb2ap, EBC_BXAP_SRAM);
		mtebc(pb2cr, EBC_BXCR_8BIT_SRAM);

		break;
	case BOOT_FROM_8BIT_NAND:
		/*
		 * CS0: NAND0
		 * CS1: NAND1
		 * CS2: NOR
		 */
		mtebc(pb0ap, EBC_BXAP_NAND);
		mtebc(pb1ap, EBC_BXAP_NAND);

		mtebc(pb0cr, EBC_BXCR_8BIT_NAND0);
		mtebc(pb1cr, EBC_BXCR_8BIT_NAND1);

		mtebc(pb2ap, EBC_BXAP_NOR);
		mtebc(pb2cr, EBC_BXCR_8BIT_NOR);

		break;
	case BOOT_FROM_8BIT_SRAM:
        /*
         * CS0: SRAM
         * CS1: NAND
         * CS2: NOR
         */
        mtebc(pb0ap, EBC_BXAP_SRAM);
        mtebc(pb0cr, EBC_BXCR_8BIT_SRAM);

        /* NAND will be auto-detected */
        mtebc(pb1ap, EBC_BXAP_NAND);
        mtebc(pb1cr, EBC_BXCR_8BIT_NAND0);

        mtebc(pb2ap, EBC_BXAP_NOR);
        mtebc(pb2cr, EBC_BXCR_8BIT_NOR);

        break;
	default:
		break;
	}
}

#define APM82181		(u32)( 0x0 << 21)
#define APM82161		(u32)( 0x1 << 21)
#define APM_SECURITY		(u32)( 0x0 << 20)
#define APM_NON_SECURITY	(u32)( 0x1 << 20)

/*
 * Distinguish APM82181 and APM82161
 * Return 1: APM82181
 * Return 0: APM82161
 */
static int is_apm82181(void) {
	u32 ecid3 = 0;
	mfsdr(SDR0_ECID3, ecid3);
	if (ecid3 & APM82161)
		return 0;

	return 1;
}

/*
 * Distinguish APM82181/APM82161 with Security and Non-Security
 * Return 1: APM82181/APM82161 with Security
 * Return 0: APM82181/APM82161 without Security
 */
static int is_apm_security(void) {
	u32 ecid3 = 0;
	mfsdr(SDR0_ECID3, ecid3);
	if (ecid3 & APM_NON_SECURITY)
		return 0;

	return 1;
}

int board_early_init_f(void) {
	u32 perclk;
	u32 sdr0_pfc0;
	/*
	 * Setup the interrupt controller polarities, triggers, etc.
	 */
	mtdcr(uic0sr, 0xffffffff);
	/* clear all */
	mtdcr(uic0er, 0x00000000);
	/* disable all */
	mtdcr(uic0cr, 0x00000005);
	/* ATI & UIC1 crit are critical */
	mtdcr(uic0pr, 0xffffffff);
	/* per ref-board manual */
	mtdcr(uic0tr, 0x00000000);
	/* per ref-board manual */
	mtdcr(uic0vr, 0x00000000);
	/* int31 highest, base=0x000 */
	mtdcr(uic0sr, 0xffffffff);
	/* clear all */

	mtdcr(uic1sr, 0xffffffff);
	/* clear all */
	mtdcr(uic1er, 0x00000000);
	/* disable all */
	mtdcr(uic1cr, 0x00000000);
	/* all non-critical */
	mtdcr(uic1pr, 0xffffffff);
	/* per ref-board manual */
	mtdcr(uic1tr, 0x00000000);
	/* per ref-board manual */
	mtdcr(uic1vr, 0x00000000);
	/* int31 highest, base=0x000 */
	mtdcr(uic1sr, 0xffffffff);
	/* clear all */

	mtdcr(uic2sr, 0xffffffff);
	/* clear all */
	mtdcr(uic2er, 0x00000000);
	/* disable all */
	mtdcr(uic2cr, 0x00000000);
	/* all non-critical */
	mtdcr(uic2pr, 0xffffffff);
	/* per ref-board manual */
	mtdcr(uic2tr, 0x00000000);
	/* per ref-board manual */
	mtdcr(uic2vr, 0x00000000);
	/* int31 highest, base=0x000 */
	mtdcr(uic2sr, 0xffffffff);
	/* clear all */

	mtdcr(uic3sr, 0xffffffff);
	/* clear all */
	mtdcr(uic3er, 0x00000000);
	/* disable all */
	mtdcr(uic3cr, 0x00000000);
	/* all non-critical */
	mtdcr(uic3pr, 0xffffffff);
	/* per ref-board manual */
	mtdcr(uic3tr, 0x00000000);
	/* per ref-board manual */
	mtdcr(uic3vr, 0x00000000);
	/* int31 highest, base=0x000 */
	mtdcr(uic3sr, 0xffffffff);
	/* clear all */

	/*
	 * Configure PFC (Pin Function Control) registers
	 * UART0: 2 pins
	 */
	mtsdr(SDR0_PFC1, 0x0000000);

	/*
	 * The AHB Bridge core is held in reset after power-on or reset
	 * so enable it now
	 */
	mtsdr(SDR0_SRST1, 0);
	/* Setup PLB4-AHB bridge based on the system address map */
	mtdcr(AHB_TOP, 0x8000004B);
	mtdcr(AHB_BOT, 0x8000004B);

	/* Enable PCIE ref clock */
	mfsdr(0x4201, perclk);
	perclk |= 0x40000000;
	perclk &= ~0x2; //Alex:20101125
	mtsdr(0x4201, perclk);

        /* Init I2C for IO expander access */
        I2C_SET_BUS(CONFIG_SYS_SPD_BUS_NUM);
        i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);

#if defined(CONFIG_NAND_U_BOOT) || defined(CONFIG_SRAM_U_BOOT)

//#if !defined(MERAKI_IKAREM)	//Alex:20101125
#if !(defined(MERAKI_IKAREM) || defined(MERAKI_BUCKMINISTER))
	gpio_exp_write_bit(PCA9670_GPIO_SEL, 0);
	//GPIO_05
	gpio_config(BLUESTONE_USBOTGDrvVBus, GPIO_OUT, GPIO_SEL, GPIO_OUT_0); /* USB OTG Power */

	//GPIO_07 as IRQ0
//	gpio_config(BLUESTONE_RTC_INT_L, GPIO_IN, GPIO_ALT1, GPIO_OUT_1); /* RTC_INT# */
	gpio_config(BLUESTONE_RTC_INT_L, GPIO_OUT, GPIO_SEL, GPIO_OUT_0); /* RTC_INT# */

	//GPIO_08
	gpio_config(BLUESTONE_UART_EN, GPIO_OUT, GPIO_SEL, GPIO_OUT_1); /* UART Enable */

	//GPIO_09
	gpio_config(BLUESTONE_SW_GPIO09, GPIO_OUT, GPIO_SEL, GPIO_OUT_1); /* SW GPIO09 */

	//GPIO_10
	gpio_config(BLUESTONE_SW_GPIO10, GPIO_OUT, GPIO_SEL, GPIO_OUT_1); /* SW GPIO10 */

	//GPIO_11
	gpio_config(BLUESTONE_SW_GPIO11, GPIO_OUT, GPIO_SEL, GPIO_OUT_1); /* SW GPIO11 */

	//GPIO_12
	gpio_config(BLUESTONE_SW_GPIO12, GPIO_IN, GPIO_ALT1, GPIO_OUT_1); /* SW GPIO12 */

	//GPIO_13
	gpio_config(BLUESTONE_SW_GPIO13, GPIO_OUT, GPIO_ALT1, GPIO_OUT_1); /* SW GPIO13 */

	//GPIO_14
	gpio_config(BLUESTONE_SW_EN_UART1, GPIO_OUT, GPIO_SEL, GPIO_OUT_1); /* SW EN UART1 */

	//GPIO_15
	gpio_config(BLUESTONE_USB_OTG_ID, GPIO_OUT, GPIO_SEL, GPIO_OUT_1); /* USB OTG ID */

	//GPIO_18 as IRQ6
	gpio_config(BLUESTONE_CPU_ALERT_L, GPIO_IN, GPIO_ALT1, GPIO_OUT_1); /* ALERT# */

	//GPIO_19 as IRQ7
	gpio_config(BLUESTONE_CPU_THERM_L, GPIO_IN, GPIO_ALT1, GPIO_OUT_1); /* THERM# */

    //GPIO_21 as IRQ9
   	gpio_config(BLUESTONE_PWR_SW_L, GPIO_IN, GPIO_ALT1, GPIO_OUT_1); /* PWR_SW# */

	//GPIO_23
	gpio_config(BLUESTONE_PCIE0_WAKE_L, GPIO_IN, GPIO_SEL, GPIO_OUT_1); /* PCIE0_WAKE_L */

    /* Enable output GPIO pins for GPIO05 and GPIO08 */
	mfsdr(SDR0_PFC0, sdr0_pfc0);
	sdr0_pfc0 |= (1 << (31 - BLUESTONE_USBOTGDrvVBus))
					| (1 << (31 - BLUESTONE_RTC_INT_L))
					| (1 << (31 - BLUESTONE_UART_EN))
					| (1 << (31 - BLUESTONE_SW_GPIO09))
					| (1 << (31 - BLUESTONE_SW_GPIO10))
					| (1 << (31 - BLUESTONE_SW_GPIO11))
					| (1 << (31 - BLUESTONE_SW_GPIO12))
					| (1 << (31 - BLUESTONE_SW_GPIO13))
					| (1 << (31 - BLUESTONE_SW_EN_UART1))
					| (1 << (31 - BLUESTONE_SW_SPI_CS_L))
					| (1 << (31 - BLUESTONE_SW_SPI_I2C1))
					| (1 << (31 - BLUESTONE_USB_OTG_ID))
					| (1 << (31 - BLUESTONE_CPU_ALERT_L))
					| (1 << (31 - BLUESTONE_CPU_THERM_L))
					| (1 << (31 - BLUESTONE_PWR_SW_L))
					| (1 << (31 - BLUESTONE_PCIE0_WAKE_L));
	mtsdr(SDR0_PFC0, sdr0_pfc0);

	gpio_write_bit( BLUESTONE_USBOTGDrvVBus , 0 ); /* Enable USB OTG Power */
	gpio_write_bit( BLUESTONE_UART_EN , 1 ); /* Enable UART */
#else

#if defined(MERAKI_IKAREM)
        gpio_exp_write_bit(PCA9670_GPIO_SEL, 0);

	//GPIO_03: GPO
	gpio_config(IKAREM_WD_CLK_INPUT, GPIO_OUT, GPIO_SEL, GPIO_OUT_0);

        //GPIO_05: INT
        gpio_config(IKAREM_AT_AF_IND, GPIO_IN, GPIO_ALT1, GPIO_OUT_1);

        //GPIO_07: GPO
        gpio_config(IKAREM_WD_EN, GPIO_OUT, GPIO_SEL, GPIO_OUT_0);

        //GPIO_08: GPI
        gpio_config(IKAREM_POE_DET, GPIO_IN, GPIO_ALT1, GPIO_OUT_1);

        //GPIO_09: GPI
        gpio_config(IKAREM_BOARD_ID0, GPIO_IN, GPIO_ALT1, GPIO_OUT_1);

        //GPIO_10: GPI
        gpio_config(IKAREM_BOARD_ID1, GPIO_IN, GPIO_ALT1, GPIO_OUT_1);

        //GPIO_11: GPO
        gpio_config(IKAREM_PCIE1_WL_DIS, GPIO_OUT, GPIO_SEL, GPIO_OUT_1);

        //GPIO_12: UART-RX
        gpio_config(IKAREM_UART1_RX, GPIO_IN, GPIO_ALT1, GPIO_OUT_1);

        //GPIO_13: UART-TX
        gpio_config(IKAREM_UART1_TX, GPIO_OUT, GPIO_ALT1, GPIO_OUT_1);

        //GPIO_14: GPO
        gpio_config(IKAREM_PCIE0_WL_DIS, GPIO_OUT, GPIO_SEL, GPIO_OUT_1);

        //GPIO_15: GPI
        gpio_config(IKAREM_ADAPTER_DET, GPIO_IN, GPIO_ALT1, GPIO_OUT_1);

        //GPIO_16: GPI
        gpio_config(IKAREM_RST_DEFAULT, GPIO_IN, GPIO_ALT1, GPIO_OUT_1);

        //GPIO_17: GPO
        gpio_config(IKAREM_LAN_GLED, GPIO_OUT, GPIO_SEL, GPIO_OUT_1);

        //GPIO_18: GPO
        gpio_config(IKAREM_PWR_GLED, GPIO_OUT, GPIO_SEL, GPIO_OUT_1);

        //GPIO_19: GPO	Low
        gpio_config(IKAREM_PWR_YLED, GPIO_OUT, GPIO_SEL, GPIO_OUT_0);

        //GPIO_20: GPO
        gpio_config(IKAREM_LED1_GLED, GPIO_OUT, GPIO_SEL, GPIO_OUT_1);

        //GPIO_21: GPO
        gpio_config(IKAREM_LED2_GLED, GPIO_OUT, GPIO_SEL, GPIO_OUT_1);

        //GPIO_22: GPO
        gpio_config(IKAREM_LED3_GLED, GPIO_OUT, GPIO_SEL, GPIO_OUT_1);

        //GPIO_23: GPO
        gpio_config(IKAREM_LED4_GLED, GPIO_OUT, GPIO_SEL, GPIO_OUT_1);

        mfsdr(SDR0_PFC0, sdr0_pfc0);
        sdr0_pfc0 |= (1 << (31 - IKAREM_WD_CLK_INPUT))
                                        | (1 << (31 - IKAREM_AT_AF_IND))
                                        | (1 << (31 - IKAREM_WD_EN))
                                        | (1 << (31 - IKAREM_POE_DET))
                                        | (1 << (31 - IKAREM_BOARD_ID0))
                                        | (1 << (31 - IKAREM_BOARD_ID1))
                                        | (1 << (31 - IKAREM_PCIE1_WL_DIS))
                                        | (1 << (31 - IKAREM_UART1_RX))
                                        | (1 << (31 - IKAREM_UART1_TX))
                                        | (1 << (31 - IKAREM_PCIE0_WL_DIS))
                                        | (1 << (31 - IKAREM_ADAPTER_DET))
                                        | (1 << (31 - IKAREM_RST_DEFAULT))
                                        | (1 << (31 - IKAREM_LAN_GLED))
                                        | (1 << (31 - IKAREM_PWR_GLED))
                                        | (1 << (31 - IKAREM_PWR_YLED))
                                        | (1 << (31 - IKAREM_LED1_GLED))
                                        | (1 << (31 - IKAREM_LED2_GLED))
                                        | (1 << (31 - IKAREM_LED3_GLED))
                                        | (1 << (31 - IKAREM_LED4_GLED));
        mtsdr(SDR0_PFC0, sdr0_pfc0);
#endif

#if defined(MERAKI_BUCKMINISTER)
        gpio_exp_write_bit(PCA9670_GPIO_SEL, 0);

       //GPIO_03: GPO
       gpio_config(BUCKMINISTER_PCS_1, GPIO_OUT, GPIO_SEL, GPIO_OUT_0);

        //GPIO_05: GPO Low
        gpio_config(BUCKMINISTER_USB_PSW, GPIO_OUT, GPIO_SEL, GPIO_OUT_0);

        //GPIO_07: GPO
        //gpio_config(BUCKMINISTER_WD_EN, GPIO_OUT, GPIO_SEL, GPIO_OUT_0);

        //GPIO_08: INT
        gpio_config(BUCKMINISTER_USB_OC, GPIO_IN, GPIO_ALT1, GPIO_OUT_1);

        //GPIO_09: GPI
        gpio_config(BUCKMINISTER_BOARD_ID0, GPIO_IN, GPIO_ALT1, GPIO_OUT_1);

        //GPIO_10: GPI
        gpio_config(BUCKMINISTER_BOARD_ID1, GPIO_IN, GPIO_ALT1, GPIO_OUT_1);

        //GPIO_11: GPI
        gpio_config(BUCKMINISTER_WD_CLK_INPUT, GPIO_OUT, GPIO_SEL, GPIO_OUT_1);

        //GPIO_12: UART-RX
        gpio_config(BUCKMINISTER_UART1_RX, GPIO_IN, GPIO_ALT1, GPIO_OUT_1);

        //GPIO_13: UART-TX
        gpio_config(BUCKMINISTER_UART1_TX, GPIO_OUT, GPIO_ALT1, GPIO_OUT_1);

        //GPIO_14: INT
        gpio_config(BUCKMINISTER_GPHY_INT, GPIO_OUT, GPIO_SEL, GPIO_OUT_1);

        //GPIO_15: GPO
        gpio_config(BUCKMINISTER_WAN_YLED, GPIO_OUT, GPIO_SEL, GPIO_OUT_1);

        //GPIO_16: GPI
        gpio_config(BUCKMINISTER_RST_DEFAULT, GPIO_IN, GPIO_ALT1, GPIO_OUT_1);

        //GPIO_17: GPO
        gpio_config(BUCKMINISTER_WAN_GLED, GPIO_OUT, GPIO_SEL, GPIO_OUT_1);

        //GPIO_18: GPO
        gpio_config(BUCKMINISTER_PWR_GLED, GPIO_OUT, GPIO_SEL, GPIO_OUT_1);

        //GPIO_19: GPO	Low
        gpio_config(BUCKMINISTER_PWR_YLED, GPIO_OUT, GPIO_SEL, GPIO_OUT_0);

        //GPIO_20: GPO
        gpio_config(BUCKMINISTER_LED1_GLED, GPIO_OUT, GPIO_SEL, GPIO_OUT_1);

        //GPIO_21: GPO
        gpio_config(BUCKMINISTER_LED2_GLED, GPIO_OUT, GPIO_SEL, GPIO_OUT_1);

        //GPIO_22: GPO
        gpio_config(BUCKMINISTER_LED3_GLED, GPIO_OUT, GPIO_SEL, GPIO_OUT_1);

        //GPIO_23: GPO
        gpio_config(BUCKMINISTER_LED4_GLED, GPIO_OUT, GPIO_SEL, GPIO_OUT_1);

        mfsdr(SDR0_PFC0, sdr0_pfc0);
        sdr0_pfc0 |= (1 << (31 - BUCKMINISTER_PCS_1))
                                        | (1 << (31 - BUCKMINISTER_USB_PSW))
                                       /* | (1 << (31 - BUCKMINISTER_WD_EN))*/
                                        | (1 << (31 - BUCKMINISTER_USB_OC))
                                        | (1 << (31 - BUCKMINISTER_BOARD_ID0))
                                        | (1 << (31 - BUCKMINISTER_BOARD_ID1 ))
                                        | (1 << (31 - BUCKMINISTER_WD_CLK_INPUT))
                                        | (1 << (31 - BUCKMINISTER_UART1_RX))
                                        | (1 << (31 - BUCKMINISTER_UART1_TX))
                                        | (1 << (31 - BUCKMINISTER_GPHY_INT))
                                        | (1 << (31 - BUCKMINISTER_WAN_YLED))
                                        | (1 << (31 - BUCKMINISTER_RST_DEFAULT))
                                        | (1 << (31 - BUCKMINISTER_WAN_GLED))
                                        | (1 << (31 - BUCKMINISTER_PWR_GLED))
                                        | (1 << (31 - BUCKMINISTER_PWR_YLED))
                                        | (1 << (31 - BUCKMINISTER_LED1_GLED))
                                        | (1 << (31 - BUCKMINISTER_LED2_GLED))
                                        | (1 << (31 - BUCKMINISTER_LED3_GLED))
                                        | (1 << (31 - BUCKMINISTER_LED4_GLED));
        mtsdr(SDR0_PFC0, sdr0_pfc0);
#endif
#endif

#endif

	return 0;
}

void bluestone_sata_init(int board_type) {
	u32 reg;

	reg = SDR_READ(PESDR0_PHY_CTL_RST);
	SDR_WRITE(PESDR0_PHY_CTL_RST,(reg & 0xcfffffc) | 0x00000001);

	if (board_type == BOARD_BLUESTONE_SATA0_SATA1) {
		// Put both SATA0 and SATA1 in reset
		SDR_WRITE(SDR0_SRST1, 0x00030187);
		/*
		 * Configure for SATA0
		 */
		reg = SDR_READ(SDR_SATA0_CLK);
		SDR_WRITE(SDR_SATA0_CLK, (reg & 0xfffffff8) | 0x00000007); // Configure Reference clock to 100MHz
		SDR_WRITE(SDR_SATA0_CDRCTL, 0x00000111);
		SDR_WRITE(SDR_SATA0_DRV, 0x000000a4); // DRV_LVL = 250mV, EMP_POST=50mV
	} else {
		// Put only SATA1 in reset
		SDR_WRITE(SDR0_SRST1, 0x00010186);
	}

	/*
	 * Configure for SATA1
	 */
	reg = SDR_READ(SDR_SATA1_CLK);
	SDR_WRITE(SDR_SATA1_CLK, (reg & 0xfffffff8) | 0x00000007); // Configure Reference clock to 100MHz
	SDR_WRITE(SDR_SATA1_CDRCTL, 0x00000111);
	SDR_WRITE(SDR_SATA1_DRV, 0x000000a4); // DRV_LVL = 250mV, EMP_POST=50mV

	// Bring SATA out of reset
	SDR_WRITE(SDR0_SRST1, 0x00000000);
	mdelay(10);
}

static void bluestone_usb_init(void) {
#if defined(CONFIG_APM82181)
	u32 control;

	mfsdr(SDR_AHB_CFG, control);
	control |= 0x800;
	mtsdr(SDR_AHB_CFG, control);

	mfsdr(SDR_USBOTG_PHY_BASE + SDR_USBOTG_PHY_CLKRST_OFFSET, control);

	control &= 0xffffffcc;
	control |= 0x00000002;

	/* Powerdown XO,Bias and PLL when suspended */
	control |= 0x20000000;

	/* Poweron PHY block */
	control &= (~0x40000000);

	mtsdr(SDR_USBOTG_PHY_BASE + SDR_USBOTG_PHY_CLKRST_OFFSET, control);

	/* reset PHY */
	mfsdr(SDR_USBOTG_PHY_BASE + SDR_USBOTG_PHY_CLKRST_OFFSET, control);
	control |= 0x80000000;
	mtsdr(SDR_USBOTG_PHY_BASE + SDR_USBOTG_PHY_CLKRST_OFFSET, control);
	udelay(100);

	mfsdr(SDR_USBOTG_PHY_BASE + SDR_USBOTG_PHY_CLKRST_OFFSET, control);
	control &= 0x7fffffff;
	mtsdr(SDR_USBOTG_PHY_BASE + SDR_USBOTG_PHY_CLKRST_OFFSET, control);
#endif
}

void bluestone_pcie_init(int board_type) {

	if (board_type != BOARD_BLUESTONE_PCIE0_SATA1)
		return;

	if (is_end_point(0)) {
		//TODO: reconfigure to Endpoint
	} else {
		/* Assert PERST# signal */
		gpio_exp_write_bit(PCA9670_PCIE0_PERST_L, 0);
		mdelay(200);
		gpio_exp_write_bit(PCA9670_PCIE0_PERST_L, 1);
		mdelay(50);
	}
}

#if defined(CONFIG_PHY_RESET_R)
void reset_phy(void)
{
	// Not available
}
#endif

int get_cpu_num(void) {
	int cpu = NA_OR_UNKNOWN_CPU;

	return cpu;
}

//Alex:20101221 - {
int check_reset_button(void)
{
#if defined(MERAKI_IKAREM)
	if (gpio_read_in_bit(IKAREM_RST_DEFAULT) ? 1 : 0)	
	{
	printf("RESET is un-pushed\n");
	gpio_write_bit(IKAREM_LED1_GLED, 1 );
	}
	else	
	{
	printf("RESET is pushed\n");
	gpio_write_bit(IKAREM_LED1_GLED, 0 );
	run_command("setenv netretry no ; run factory_boot bootcmd", 1);
	}
#endif /*defined(MERAKI_IKAREM)*/

#if defined(MERAKI_BUCKMINISTER)
	if (gpio_read_in_bit(BUCKMINISTER_RST_DEFAULT) ? 1 : 0)	
	{
	printf("RESET is un-pushed\n");
	gpio_write_bit(BUCKMINISTER_LED1_GLED, 1 );
	}
	else	
	{
	printf("RESET is pushed\n");
	gpio_write_bit(BUCKMINISTER_LED1_GLED, 0 );
	run_command("setenv netretry no ; run factory_boot bootcmd", 1);
	}
#endif /*defined(MERAKI_BUCKMINISTER)*/

	
	return 0;
}
//Alex:20101221 - }

int checkboard(void) {
	char *s = getenv("serial#");
	char* boardtype = getenv("boardtype");

	printf("Board: Buckminster - Meraki Buckminster Cloud Managed Router");

	putc('\n');
//Alex:20101125 - {
#if defined(MERAKI_IKAREM)
	printf("============================\n");
        printf("BoardID: %d %d\n",gpio_read_in_bit(IKAREM_BOARD_ID1),gpio_read_in_bit(IKAREM_BOARD_ID0));
        printf("POE-PWR_DET Status: %x\n", gpio_read_in_bit(IKAREM_POE_DET));
	if (gpio_read_in_bit(IKAREM_POE_DET))
        printf("IEEE 802.3AT#/AF Status: %x\n", gpio_read_in_bit(IKAREM_AT_AF_IND));
        printf("ADAPTER_DET Status: %x\n", gpio_read_in_bit(IKAREM_ADAPTER_DET));
        printf("Reset Button Status: %x\n", gpio_read_in_bit(IKAREM_RST_DEFAULT));
	printf("============================\n");

	u32 perclk;
        mfsdr(0x4201, perclk);
	printf("SDR0_PERCLK=0x%x\n",perclk);

	int i=0;
	for (i=0;i<3;i++)	
	{
	gpio_write_bit(IKAREM_LAN_GLED, 0 );
	gpio_write_bit(IKAREM_PWR_GLED, 0 );
	gpio_write_bit(IKAREM_PWR_YLED, 0 );
	gpio_write_bit(IKAREM_LED1_GLED, 0 );
	gpio_write_bit(IKAREM_LED2_GLED, 0 );
	gpio_write_bit(IKAREM_LED3_GLED, 0 );
	gpio_write_bit(IKAREM_LED4_GLED, 0 );
	
	udelay(100000);

	gpio_write_bit(IKAREM_LAN_GLED, 1 );
	gpio_write_bit(IKAREM_PWR_GLED, 1 );
	gpio_write_bit(IKAREM_PWR_YLED, 1 );
	gpio_write_bit(IKAREM_LED1_GLED, 1 );
	gpio_write_bit(IKAREM_LED2_GLED, 1 );
	gpio_write_bit(IKAREM_LED3_GLED, 1 );
	gpio_write_bit(IKAREM_LED4_GLED, 1 );
	
	udelay(100000);
	}
	

	gpio_write_bit(IKAREM_PWR_YLED, 0 );
#endif

//Alex:20101125 - {
#if defined(MERAKI_BUCKMINISTER)
	printf("============================\n");
        printf("BoardID: %d %d\n",gpio_read_in_bit(BUCKMINISTER_BOARD_ID1),gpio_read_in_bit(BUCKMINISTER_BOARD_ID0));
        printf("Reset Button Status: %x\n", gpio_read_in_bit(BUCKMINISTER_RST_DEFAULT));
	printf("============================\n");

	u32 perclk;
        mfsdr(0x4201, perclk);
	printf("SDR0_PERCLK=0x%x\n",perclk);

	gpio_write_bit(BUCKMINISTER_PWR_YLED, 0 );
#endif
//Alex:20101125 - }

	
	return (0);
}

#if defined(CONFIG_NAND_U_BOOT)
/*
 * NAND booting U-Boot version uses a fixed initialization, since the whole
 * I2C SPD DIMM autodetection/calibration doesn't fit into the 4k of boot
 * code.
 */
phys_size_t initdram(int board_type)
{
	return CONFIG_SYS_MBYTES_SDRAM << 20;
}
#endif

#if defined(CFG_DDRAUTOCALIB_REDUCE)
/* SDRAM tunning */
/* These values are get after testing full calibration in some boards */
struct sdram_timing {
	u32 wrdtr;
	u32 clktr;
};

struct sdram_timing rewood_scan_options[] = {
	{	3, 0}, {3, 1}, {3, 2}, /*{3, 3},*/
	{	2, 0}, {2, 1}, {2, 2}, /*{2, 3},*/
	{	4, 0}, {4, 1}, /*{4, 2}, {4, 3},*/
	/*{1, 0},*/{1, 1}, {1, 2}, /*{1, 3},*/
	{	5, 0}, /*{5, 1}, {5, 2}, {5, 3},
	 {0, 0}, {0, 1},*/{0, 2}, /*{0, 3},
	 {6, 0}, {6, 1}, {6, 2}, {6, 3},*/
	{	-1, -1}
};

ulong ddr_scan_option(ulong default_val)
{
	return (ulong)rewood_scan_options;
}

u32 ddr_rqfd_start_opt(u32 default_val)
{
	/* This value is get after testing full calibration in some boards */
	return 0x0;
}

u32 ddr_rqfd_end_opt(u32 default_val)
{
	/* This value is get after testing full calibration in some boards */
	return 0x80;
}

u32 ddr_rffd_start_opt(u32 default_val)
{
	/* This value is get after testing full calibration in some boards */
	return 0x1b0;
}
u32 ddr_rffd_end_opt(u32 default_val)
{
	/* This value is get after testing full calibration in some boards */
	return 0x2e0;
}
#endif

int pci_available()
{
	static int r = -1;
	if (r == -1) {
		char *pcie = getenv("pcie");
		r = pcie && strcmp(pcie, "enabled") == 0;
	}
	return r;
}

#if defined(CONFIG_PCI)
/*
 * is_pci_host
 *
 * This routine is called to determine if a pci scan should be
 * performed. With various hardware environments (especially cPCI and
 * PPMC) it's insufficient to depend on the state of the arbiter enable
 * bit in the strap register, or generic host/adapter assumptions.
 *
 * Rather than hard-code a bad assumption in the general 440 code, the
 * 440 pci code requires the board to decide at runtime.
 *
 * Return 0 for adapter mode, non-zero for host (monarch) mode.
 */
int is_pci_host(struct pci_controller *hose)
{
	/* Board is always configured as host. */
	return (1);
}

static struct pci_controller pcie_hose[1] = { {0}};

void pcie_setup_hoses(int busno)
{
	struct pci_controller *hose;
	int bus;
	int ret = 0;
	char *env;
	unsigned int delay;
	
	/*
	 * assume we're called after the PCIX hose is initialized, which takes
	 * bus ID 0 and therefore start numbering PCIe's from 1.
	 */
	bus = busno;

	/*
	 * Only APM82181 has PCIE0 (that can be shared with SATA0)
	 */
	if (!is_apm82181() || !pci_available())
	return;
	if ( (gd->board_type == BOARD_BLUESTONE_SATA0_SATA1) ||
			(gd->board_type == BOARD_BLUESTONE_x_SATA1) )
	return;
	/* PCIE0 init for APM82181 */
	if (is_end_point(0))
	ret = ppc4xx_init_pcie_endport(0);
	else
	ret = ppc4xx_init_pcie_rootport(0);
	if (ret) {
		printf("PCIE%d: initialization as %s failed\n", 0,
				is_end_point(0) ? "endpoint" : "root-complex");
		return;
	}

	hose = &pcie_hose[0];
	hose->first_busno = bus;
	hose->last_busno = bus;
	hose->current_busno = bus;

	/* setup mem resource */
	pci_set_region(hose->regions + 0,
			CONFIG_SYS_PCIE_MEMBASE + 0 * CONFIG_SYS_PCIE_MEMSIZE,
			CONFIG_SYS_PCIE_MEMBASE + 0 * CONFIG_SYS_PCIE_MEMSIZE,
			CONFIG_SYS_PCIE_MEMSIZE,PCI_REGION_MEM);
	hose->region_count = 1;
	pci_register_hose(hose);


	if (is_end_point(0)) {
		ppc4xx_setup_pcie_endpoint(hose, 0);
		/*
		 * Reson for no scanning is endpoint can not generate
		 * upstream configuration accesses.
		 */
	} else {
		ppc4xx_setup_pcie_rootpoint(hose, 0);
		env = getenv ("pciscandelay");
		if (env != NULL) {
			delay = simple_strtoul(env, NULL, 10);
			if (delay > 5)
			printf("Warning, expect noticable delay before "
					"PCIe scan due to 'pciscandelay' value!\n");
			mdelay(delay * 1000);
		}

		/*
		 * Config access can only go down stream
		 */
		hose->last_busno = pci_hose_scan(hose);
		bus = hose->last_busno + 1;
	}
}
#endif /* CONFIG_PCI */


int board_early_init_r(void) {
	u32 bootdevice;
	u32 sdr0_cust0;

	/*
	 * Clear potential errors resulting from auto-calibration.
	 * If not done, then we could get an interrupt later on when
	 * exceptions are enabled.
	 */
	set_mcsr(get_mcsr());

	/* Reconfigure EBC here */
	bootdevice = bootdevice_selected();
	reconfigure_EBC(bootdevice);

	mdelay(100);
	
	/* Initialize SATA port(s)*/
	bluestone_sata_init(gd->board_type);
	gpio_exp_write_bit(PCA9670_HD01_PWR_EN, 0); // Enable HD Power
	mdelay(10);

	/* Initialzied PCIE0 */
	bluestone_pcie_init(gd->board_type);

	/* init PHY USB OTG */
	bluestone_usb_init();

	return 0;
}

int misc_init_r(void) {
	u32 sdr0_srst1 = 0;
	u32 sdr0_pfc0;
	u32 eth_cfg;
	u8 val;

	/*
	 * Set EMAC mode/configuration (GMII, SGMII, RGMII...).
	 * This is board specific, so let's do it here.
	 */
	mfsdr(SDR0_ETH_CFG, eth_cfg);
	/* Set the for  RGMII mode */
	eth_cfg &= ~SDR0_ETH_CFG_GMC0_BRIDGE_SEL;
	/* EMAC0 as MDIO source */
	eth_cfg &= ~SDR0_ETH_CFG_MDIO_SEL;
	/* GMCTXCLK is PHY REF clock output */
	eth_cfg |= SDR0_ETH_CFG_PHY_REFCLK_EN;
	mtsdr(SDR0_ETH_CFG, eth_cfg);

	/* External RTC */
	val = i2c_reg_read(CONFIG_SYS_I2C_RTC_ADDR, 0xa);
	val &= ~0x40;
	i2c_reg_write(CONFIG_SYS_I2C_RTC_ADDR, 0xa, val);

	/* clear RTC ALARM interrupt */
	val = i2c_reg_read(CONFIG_SYS_I2C_RTC_ADDR, 0xF);

	/* Config ALARM mode is daily-alarm */
	val = i2c_reg_read(CONFIG_SYS_I2C_RTC_ADDR, 0xB);
	i2c_reg_write(CONFIG_SYS_I2C_RTC_ADDR, 0xB, val | 0xC0);

	return 0;
}

#ifdef CONFIG_LAST_STAGE_INIT
int last_stage_init(void)
{
	printf("Initializing Bluestone Ethernet Port ... \n");
	NetLoop(PING);
	
	/* Flush UART0/1 input */
        while (tstc()) {   /* we got a key press */
                (void) getc();  /* consume input */
        }

	return 0;
}
#endif

#if defined(CONFIG_OF_LIBFDT) && defined(CONFIG_OF_BOARD_SETUP)
extern void __ft_board_setup(void *blob, bd_t *bd);

void ft_board_setup(void *blob, bd_t *bd)
{
	u32 ebc_ranges_prop[12];
	u32 nor_reg_prop[3];
	u32 ndfc_reg_prop[3];
	u32 l2c_size;
#if defined(CONFIG_NAND_U_BOOT)
	u32 ndfc1_reg_prop[1];
#endif
	int rc;

	char* tah_enable;
        u32 eth_cfg;
        int tahoe_node;
        int emac_node;

	int xor_node;
        int crypto_node;
        int pka_node;
        int trng_node;

	__ft_board_setup(blob, bd);
	
	/* Check to enable/disable tahoe */
        tah_enable = getenv("tahoe");
        if (!strcmp(tah_enable, "disabled")) {
                /* By-pass TAHOE core */
                mfsdr(SDR0_ETH_CFG, eth_cfg);
                eth_cfg |= SDR0_ETH_CFG_TAHOE0_BYPASS;
                mtsdr(SDR0_ETH_CFG, eth_cfg);

                /* Delete TAHOE node */
                tahoe_node = fdt_node_offset_by_compatible(blob, -1, "ibm,tah");
                if (tahoe_node >= 0)
                        fdt_del_node(blob, tahoe_node);

                /* Delete TAH device in EMAC node */
                emac_node = fdt_node_offset_by_compatible(blob, -1, "ibm,emac-405ex");
                if (emac_node >= 0) {
                        fdt_delprop(blob, emac_node, "tah-device");
                        fdt_delprop(blob, emac_node, "tah-channel");
                }
        } else {
                /* Do not by-pass TAHOE core */
                mfsdr(SDR0_ETH_CFG, eth_cfg);
                eth_cfg &= ~SDR0_ETH_CFG_TAHOE0_BYPASS;
                mtsdr(SDR0_ETH_CFG, eth_cfg);
        }

	if (is_apm82181()) { /* APM82181 */
#ifdef MERAKI_BUCKMINISTER
                if (!pci_available()) {
			fdt_find_and_setprop(blob, "/plb/pciex@d00000000", "status",
				"disabled", sizeof("disabled"), 1);
                }
#else
		if ( (gd->board_type == BOARD_BLUESTONE_SATA0_SATA1)) { 
			/*
			 * When SATA is selected we need to disable the PCIe
			 * node in the device tree, so that Linux doesn't initialize
			 * it.
			 */
			fdt_find_and_setprop(blob, "/plb/pciex@d00000000", "status",
				"disabled", sizeof("disabled"), 1);
		}

		if (gd->board_type == BOARD_BLUESTONE_PCIE0_SATA1) { 
			/*
			 * When PCIe0 is selected we need to disable the SATA0
			 * node in the device tree, so that Linux doesn't initialize
			 * it.
			 */
			fdt_find_and_setprop(blob, "/plb/sata@bffd1000", "status",
				"disabled", sizeof("disabled"), 1);
		}
#endif
	} else { /* APM82161 */
		/* L2C is 128Kb */
		l2c_size = 131072;
		fdt_find_and_setprop(blob, "/l2c", "cache-size",
				&l2c_size, sizeof(l2c_size), 1);
		
		/* XOR Engine is disabled */
                xor_node = fdt_node_offset_by_compatible(blob, -1, "amcc,xor");
                if (xor_node >= 0)
                        fdt_del_node(blob, xor_node);

		/* PCIe is disable */
		fdt_find_and_setprop(blob, "/plb/pciex@d00000000", "status",
				"disabled", sizeof("disabled"), 1);
		/* SATA0 is disable */
		fdt_find_and_setprop(blob, "/plb/sata@bffd1000", "status",
				"disabled", sizeof("disabled"), 1);
	}

	if (!is_apm_security()) { /* APM82181/APM82161 without Security Option */
		 /* CRYPTO Engine is disabled */
                crypto_node = fdt_node_offset_by_compatible(blob, -1, "amcc,ppc4xx-crypto");
                if (crypto_node >= 0)
                        fdt_del_node(blob, crypto_node);

                /* PKA and TRNG are disabled */
                pka_node = fdt_node_offset_by_compatible(blob, -1, "amcc, ppc4xx-pka");
                if (pka_node >= 0)
                        fdt_del_node(blob, pka_node);

                trng_node = fdt_node_offset_by_compatible(blob, -1, "amcc, ppc4xx-trng");
                if (trng_node >= 0)
                        fdt_del_node(blob, trng_node);
	}

#if !defined(CONFIG_NAND_U_BOOT) && !defined(CONFIG_NAND_SPL)
	/* EBC ranges fixup */
	ebc_ranges_prop[0] = 0; /* bank 0 */
	ebc_ranges_prop[1] = 0; /* always 0 */
	ebc_ranges_prop[2] = CONFIG_SYS_FLASH_BASE; /* re-mapped NOR flash to this address */
	ebc_ranges_prop[3] = gd->bd->bi_flashsize; /* NOR Flash size */
	
	/* NOR flash fixup */
	nor_reg_prop[0] = 0; /* NOR chip select = 0 */
	nor_reg_prop[1] = 0; /* always 0 */
	nor_reg_prop[2] = CONFIG_SYS_FLASH_SIZE; /* NOR Flash size */
	
	rc = fdt_find_and_setprop(blob, "/plb/opb/ebc", "ranges",
			ebc_ranges_prop, sizeof(ebc_ranges_prop), 1);
	if (rc)
	printf("Unable to update EBC ranges property, err=%s\n",
			fdt_strerror(rc));
	rc = fdt_find_and_setprop(blob, "/plb/opb/ebc/nor_flash@0,0", "reg",
			nor_reg_prop, sizeof(nor_reg_prop), 1);
	if (rc)
	printf("Unable to update property NOR mapping, err=%s\n",
			fdt_strerror(rc));

#else

	/* EBC ranges fixup */
	ebc_ranges_prop[0] = 0; /* bank 0 */
	ebc_ranges_prop[1] = 0; /* always 0 */
	ebc_ranges_prop[2] = CONFIG_SYS_NAND0_ADDR; /* NAND0 NDFC base */
	ebc_ranges_prop[3] = 0x2000; /* NOR Flash size */

	/* Fixup NAND NDFC mapping */
	ndfc_reg_prop[0] = 0; /* NAND0 NDFC chip select = 0 */
	ndfc_reg_prop[1] = 0; /* always 0 */
	ndfc_reg_prop[2] = 0x2000; /* NAND NDFC size */

	rc = fdt_find_and_setprop(blob, "/plb/opb/ebc", "ranges",
			ebc_ranges_prop, sizeof(ebc_ranges_prop), 1);
	if (rc)
	printf("Unable to update EBC ranges property, err=%s\n",
			fdt_strerror(rc));
	
	rc = fdt_find_and_setprop(blob, "/plb/opb/ebc/ndfc@1,0", "reg",
			ndfc_reg_prop, sizeof(ndfc_reg_prop), 1);
	if (rc)
	printf("Unable to update property NAND0 NDFC mapping, err=%s\n",
			fdt_strerror(rc));
	
#endif
}
#endif /* defined(CONFIG_OF_LIBFDT) && defined(CONFIG_OF_BOARD_SETUP) */
