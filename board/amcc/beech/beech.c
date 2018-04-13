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
#include <asm/mmu.h>
#include <asm/4xx_pcie.h>
#include <asm/gpio.h>
#if !defined(CONFIG_SYS_NO_FLASH)
extern flash_info_t flash_info[CONFIG_SYS_MAX_FLASH_BANKS]; /* info for FLASH chips */
#endif

DECLARE_GLOBAL_DATA_PTR;


#define SDR_AHB_CFG	0x370

/* Board type */
#define BOARD_BEECH_SATA0_SATA1	1	/* APM82181 2 SATA */
#define BOARD_BEECH_PCIE0_SATA1	2	/* APM82181 1 PCIE and 1 SATA*/
#define BOARD_BEECH_x_SATA1	3	/* APM82161 1 SATA */

/* Define Boot devices */
#define BOOT_FROM_8BIT_SRAM                     0x00
#define BOOT_FROM_8BIT_SRAM_FULL_ADDR           0x01
#define BOOT_FROM_8BIT_NAND                     0x02
#define BOOT_FROM_8BIT_NOR                     	0x03
#define BOOT_FROM_8BIT_NOR_FULL_ADDR          	0x04
#define BOOT_DEVICE_UNKNOWN                     0xff


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

/*
 * CPLD read/write utilities 
 */

/*
 * Early initialize CPLD 
 * Do not need to call afer I2C is initialized 
 */
void cpld_early_init(void)
{
	/* Init I2C for CPLD access */
        I2C_SET_BUS(CONFIG_CPLD_BUS_NUM);
        i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
}

/*
 * Read 8-bit data from CPLD
 * return: positive number: data read
 *	   -1: error
 */
unsigned char cpld_read(uchar addr)
{
	unsigned char data;
	int old_bus;

	old_bus = I2C_GET_BUS();
        I2C_SET_BUS(CONFIG_CPLD_BUS_NUM);
	i2c_read(CONFIG_CPLD_I2C_ADDR, addr, 1, &data, 1);
	
	I2C_SET_BUS(old_bus);
	
	return data;
}

/*
 * Write 8-bit data to CPLD
 * return: 0  success
 *	   -1 fail
 */
int cpld_write(uchar addr, uchar data)
{
	unsigned char data2wr = data;
	int old_bus;
	int ret = 0;
	old_bus = I2C_GET_BUS();
        I2C_SET_BUS(CONFIG_CPLD_BUS_NUM);

	if (i2c_write(CONFIG_CPLD_I2C_ADDR, addr, 1, &data2wr, 1) != 0) {
		printf("WARN: Failed to write to CPLD offset 0x%x\n", addr);
                ret = -1;
	}

	I2C_SET_BUS(old_bus);
        return ret;
}

static void cpld_reg_dump(void)
{
	printf("CONFIG_CPLD_ID = 0x%02x\n", cpld_read(CONFIG_CPLD_ID));
	printf("CONFIG_CPLD_VER = 0x%02x\n", cpld_read(CONFIG_CPLD_VER));
	printf("CONFIG_CPLD_BSR = 0x%02x\n", cpld_read(CONFIG_CPLD_BSR));
	printf("CONFIG_CPLD_RSTCR = 0x%02x\n", cpld_read(CONFIG_CPLD_RSTCR));
	printf("CONFIG_CPLD_BCR = 0x%02x\n", cpld_read(CONFIG_CPLD_BCR));
	printf("CONFIG_CPLD_IMR = 0x%02x\n", cpld_read(CONFIG_CPLD_IMR));
	printf("CONFIG_CPLD_UTFR = 0x%02x\n", cpld_read(CONFIG_CPLD_UTFR));
	
}
/*
 * Get bootdevice information and 
 * setup EBC configuration 
 * NOTE: Assume that I2C had been initialized 
 */
int bootdevice_selected(void)
{
        unsigned long rl ;
        unsigned long bootstrap_settings;
        int computed_boot_device = BOOT_DEVICE_UNKNOWN;
	unsigned char user_input;
	
	/* 
	 * UserInput Jumper is used by CPLD to configure the bootdevice 
	 * for the board
 	 */
	mfsdr(sdr_sdstp1, bootstrap_settings);
	rl = SDR_SDSTP1_RL_DECODE(bootstrap_settings);	
	if (rl == SDR_SDSTP1_RL_EBC) { /* Boot device is NOR/SRAM */
		user_input = cpld_read(CONFIG_CPLD_UTFR);
		user_input = CONFIG_CPLD_UTRF_USRINPUT(user_input);
		if (CONFIG_CPLD_UTRF_USRINPUT_SRAM(user_input))
			/* Boot from SRAM */
			if (CONFIG_CPLD_UTRF_USRINPUT_FULL_ADDR(user_input)) 
				/* Full address space */
				computed_boot_device = BOOT_FROM_8BIT_SRAM_FULL_ADDR;
			else
				computed_boot_device = BOOT_FROM_8BIT_SRAM;
		else
			/* Boot from NOR */
			if (CONFIG_CPLD_UTRF_USRINPUT_FULL_ADDR(user_input))
				/* Full address space */
				computed_boot_device = BOOT_FROM_8BIT_NOR_FULL_ADDR;
			else
				computed_boot_device = BOOT_FROM_8BIT_NOR;
	} else if (rl == SDR_SDSTP1_RL_NDFC)
		computed_boot_device = BOOT_FROM_8BIT_NAND;
	
	return computed_boot_device;
}

void reconfigure_EBC(int computed_boot_device)
{
	u32 val32;
	switch(computed_boot_device) {
		case BOOT_FROM_8BIT_NOR:
			/*
			 * CS0: NOR
			 * CS1: NAND
			 * CS2: SRAM
			 */
			/* Configure EBC CS0 - Nor Flash 1MBx8 */
			mtebc(pb0ap, EBC_BXAP_NOR);
			mtebc(pb0cr, EBC_BXCR_8BIT_NOR);
        		
			/* NAND will be auto-detected */
			mtebc(pb1ap, EBC_BXAP_NAND);
			mtebc(pb1cr, EBC_BXCR_8BIT_NAND0); 


			mtebc(pb2ap, EBC_BXAP_SRAM);
			mtebc(pb2cr, EBC_BXCR_8BIT_SRAM);
			break;
		case BOOT_FROM_8BIT_NOR_FULL_ADDR:
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
		case BOOT_FROM_8BIT_SRAM_FULL_ADDR:
                        /*
                         * CS0: SRAM
                         * CS1: x
                         * CS2: x
                         */
                        mtebc(pb0ap, EBC_BXAP_SRAM);
                        mtebc(pb0cr, EBC_BXCR_8BIT_SRAM);

                        /* NAND and NOR cannot be accessed in this boot option */
                        mtebc(pb1ap, EBC_BXAP_NAND);
                        mtebc(pb1cr, EBC_BXCR_8BIT_NAND0);
                        mtebc(pb2ap, EBC_BXAP_NOR);
                        mtebc(pb2cr, EBC_BXCR_8BIT_NOR);

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
		default:
			break;
	}
}

#define APM82181               	(u32)( 0x0 << 21)
#define APM82161                (u32)( 0x1 << 21)
#define APM_SECURITY            (u32)( 0x0 << 20)
#define APM_NON_SECURITY        (u32)( 0x1 << 20)

/*
 * Distinguish APM82181 and APM82161
 * Return 1: APM82181
 * Return 0: APM82161
 */
static int is_apm82181(void)
{
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

int board_early_init_f(void)
{
	u32 perclk;

	/*
	 * Setup the interrupt controller polarities, triggers, etc.
	 */
	mtdcr(uic0sr, 0xffffffff);	/* clear all */
	mtdcr(uic0er, 0x00000000);	/* disable all */
	mtdcr(uic0cr, 0x00000005);	/* ATI & UIC1 crit are critical */
	mtdcr(uic0pr, 0xffffffff);	/* per ref-board manual */
	mtdcr(uic0tr, 0x00000000);	/* per ref-board manual */
	mtdcr(uic0vr, 0x00000000);	/* int31 highest, base=0x000 */
	mtdcr(uic0sr, 0xffffffff);	/* clear all */

	mtdcr(uic1sr, 0xffffffff);	/* clear all */
	mtdcr(uic1er, 0x00000000);	/* disable all */
	mtdcr(uic1cr, 0x00000000);	/* all non-critical */
	mtdcr(uic1pr, 0xffffffff);	/* per ref-board manual */
	mtdcr(uic1tr, 0x00000000);	/* per ref-board manual */
	mtdcr(uic1vr, 0x00000000);	/* int31 highest, base=0x000 */
	mtdcr(uic1sr, 0xffffffff);	/* clear all */

	mtdcr(uic2sr, 0xffffffff);	/* clear all */
	mtdcr(uic2er, 0x00000000);	/* disable all */
	mtdcr(uic2cr, 0x00000000);	/* all non-critical */
	mtdcr(uic2pr, 0xffffffff);	/* per ref-board manual */
	mtdcr(uic2tr, 0x00000000);	/* per ref-board manual */
	mtdcr(uic2vr, 0x00000000);	/* int31 highest, base=0x000 */
	mtdcr(uic2sr, 0xffffffff);	/* clear all */

	mtdcr(uic3sr, 0xffffffff);	/* clear all */
	mtdcr(uic3er, 0x00000000);	/* disable all */
	mtdcr(uic3cr, 0x00000000);	/* all non-critical */
	mtdcr(uic3pr, 0xffffffff);	/* per ref-board manual */
	mtdcr(uic3tr, 0x00000000);	/* per ref-board manual */
	mtdcr(uic3vr, 0x00000000);	/* int31 highest, base=0x000 */
	mtdcr(uic3sr, 0xffffffff);	/* clear all */

	/* Init CPLD for later access */
	cpld_early_init();
	
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
	mtsdr(0x4201, perclk);

	return 0;
}

void beech_sata_init(int board_type)
{
	u32 reg;
	u32 control;

	reg = SDR_READ(PESDR0_PHY_CTL_RST);
	SDR_WRITE(PESDR0_PHY_CTL_RST,(reg & 0xcfffffc) | 0x00000001);

	if ( board_type == BOARD_BEECH_SATA0_SATA1 ) {
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

static void beech_usb_init(void)
{
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

	/* Fixed disconnect interrupt happened when connect some USB Hard disk */
	mfsdr(SDR_USBOTG_PHY_BASE + SDR_USBOTG_PHY_TUNE_OFFSET, control);
	control &= 0xf88fffff;
	control |= 0x04600000;
	mtsdr(SDR_USBOTG_PHY_BASE + SDR_USBOTG_PHY_TUNE_OFFSET, control);

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

void beech_pcie_init(int board_type)
{
	unsigned char rcr;

	if (board_type != BOARD_BEECH_PCIE0_SATA1)
		return;

	/* Configure CPLD for PCIE0 enpoint or rootport */
	rcr = cpld_read(CONFIG_CPLD_RSTCR);
	if (is_end_point(0)) {
		rcr = (rcr | CONFIG_CPLD_RSTCR_MPRI0_EN) &
			(~CONFIG_CPLD_RSTCR_MPRO0_EN);
		cpld_write(CONFIG_CPLD_RSTCR, rcr);	
	} else {
		rcr = (rcr & (~CONFIG_CPLD_RSTCR_MPRI0_EN)) |
                        (CONFIG_CPLD_RSTCR_MPRO0_EN);

		cpld_write(CONFIG_CPLD_RSTCR, rcr);	
	}
#if defined(CONFIG_CPLD_DEBUG)
	cpld_reg_dump();
#endif
}

#if defined(CONFIG_PHY_RESET_R)
void reset_phy(void)
{
	unsigned char rcr;
	rcr = cpld_read(CONFIG_CPLD_RSTCR);
	rcr = rcr & CONFIG_CPLD_RSTCR_GE0_RST;
	cpld_write(CONFIG_CPLD_RSTCR, rcr);
	udelay(1000);
	rcr = rcr | (~CONFIG_CPLD_RSTCR_GE0_RST);
	cpld_write(CONFIG_CPLD_RSTCR, rcr);
}
#endif

int get_cpu_num(void)
{
	int cpu = NA_OR_UNKNOWN_CPU;

	return cpu;
}

int checkboard(void)
{
	char *s = getenv("serial#");
	char* boardtype = getenv("boardtype");
	char* usbotg	= getenv("usbotg");
	unsigned char bcr;
	unsigned char sdram_16bit = 0;
 
	if (is_apm82181()) {
		printf("Board: Beech - AMCC APM82181 Validation Board");
		if ((boardtype != NULL) && 
			!strcmp(boardtype, "pcie")) {
			gd->board_type = BOARD_BEECH_PCIE0_SATA1;
			/* Configure CPLD */
			bcr = cpld_read(CONFIG_CPLD_BCR);
			bcr &= (~CONFIG_CPLD_BCR_SATA0_EN);
			cpld_write(CONFIG_CPLD_BCR, bcr);
			beech_pcie_init(gd->board_type);
			puts(", PCIE0/SATA1");
		} else {
			gd->board_type = BOARD_BEECH_SATA0_SATA1;
			/* Configure CPLD */
			bcr = cpld_read(CONFIG_CPLD_BCR);
                        bcr |= CONFIG_CPLD_BCR_SATA0_EN;
                        cpld_write(CONFIG_CPLD_BCR, bcr);
			puts(", 2*SATA");
		}
		bcr = cpld_read(CONFIG_CPLD_BCR);
		if((usbotg !=NULL)&& !strcmp(usbotg,"yes")){
			puts(", 1*USB OTG");
			bcr |= CONFIG_CPLD_BCR_USBOTG_EN;
		}
		else{
			puts(", 1*USB");
			bcr  &= (~CONFIG_CPLD_BCR_USBOTG_EN);
		}
	
		cpld_write(CONFIG_CPLD_BCR, bcr);
	} else {
		printf("Board: Beech - AMCC APM82161 Validation Board");
		gd->board_type = BOARD_BEECH_x_SATA1;
		puts(", 1*SATA");
	}
	
	if (s != NULL) {
		puts(", serial# ");
		puts(s);
	}
	putc('\n');

	/*
	 * Check to see whether we need to force SDRAM controller 
	 * to be configured as 16-bit width
 	 */
	if (i2c_read(CONFIG_SDRAM_INFO_EEPROM_ADDR, 
		CONFIG_SDRAM16BIT_OFFSET, 1, &sdram_16bit, 1)) {
		printf("Failed to read I2C EEPROM to determine \n");
		printf("SDRAM controller bit width, assume 32-bit\n");
		gd->sdram_16bit = 0;
	} else {
		if (sdram_16bit == CONFIG_FORCE_SDRAM16BIT)
			gd->sdram_16bit = 1;
	}

	beech_sata_init(gd->board_type);
	
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
        {3, 0}, {3, 1}, {3, 2}, /*{3, 3},*/
        {2, 0}, {2, 1}, {2, 2}, /*{2, 3},*/
        {4, 0}, {4, 1}, /*{4, 2}, {4, 3},*/
        /*{1, 0},*/ {1, 1}, {1, 2}, /*{1, 3},*/
        {5, 0}, /*{5, 1}, {5, 2}, {5, 3},
        {0, 0}, {0, 1},*/ {0, 2}, /*{0, 3},
        {6, 0}, {6, 1}, {6, 2}, {6, 3},*/
        {-1, -1}
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

static struct pci_controller pcie_hose[1] = {{0}};

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
	if (!is_apm82181())
		return;
	if ( (gd->board_type == BOARD_BEECH_SATA0_SATA1) ||	
		(gd->board_type == BOARD_BEECH_x_SATA1) )	
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

int board_early_init_r (void)
{
	u32 bootdevice;
	u32 sdr0_cust0;
	u32 pfc1;
	u32  *p_ndfc_b1cr = (u32*)(CONFIG_SYS_NAND0_ADDR + 0x34);
	u32 *p_ndfc_cr = (u32*)(CONFIG_SYS_NAND0_ADDR + 0x40);

	unsigned char rcr;
	
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
	
	/* Disable NOR reset */
	rcr = cpld_read(CONFIG_CPLD_RSTCR);
	rcr |= (~CONFIG_CPLD_RSTCR_NORF_RST);
	cpld_write(CONFIG_CPLD_RSTCR, rcr);	
	rcr = cpld_read(CONFIG_CPLD_RSTCR);
	

	/* init PHY USB OTG */
	beech_usb_init();
	
	return 0;
}

int misc_init_r(void)
{
	u32 sdr0_srst1 = 0;
	u32 eth_cfg;
	
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
	/*
	 * The AHB Bridge core is held in reset after power-on or reset
	 * so enable it now
	 */
	mfsdr(SDR0_SRST1, sdr0_srst1);
	sdr0_srst1 &= ~SDR0_SRST1_AHB;
	mtsdr(SDR0_SRST1, sdr0_srst1);
	
	return 0;
}

#if defined(CONFIG_OF_LIBFDT) && defined(CONFIG_OF_BOARD_SETUP)
extern void __ft_board_setup(void *blob, bd_t *bd);

void ft_board_setup(void *blob, bd_t *bd)
{
	u32 ebc_ranges_prop[12];
	u32 nor_reg_prop[3];
	u32 ndfc_reg_prop[3];
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
	int l2c_size;

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
		if (gd->board_type == BOARD_BEECH_SATA0_SATA1) {
			/*
			 * When SATA0 is selected we need to disable the PCIe0
			 * node in the device tree, so that Linux doesn't initialize
			 * it.
			 */
			fdt_find_and_setprop(blob, "/plb/pciex@d00000000", "status",
				     "disabled", sizeof("disabled"), 1);
		}

		if (gd->board_type == BOARD_BEECH_PCIE0_SATA1) {
			/*
			 * When PCIe0 is selected we need to disable the SATA0
			 * node in the device tree, so that Linux doesn't initialize
			 * it.
			 */
			fdt_find_and_setprop(blob, "/plb/sata@bffd1000", "status",
				     "disabled", sizeof("disabled"), 1);
		}

	} else  { /* APM82161 */
                /* L2C is 128Kb */
                l2c_size = 131072;
                fdt_find_and_setprop(blob, "/l2c", "cache-size",
                                &l2c_size, sizeof(l2c_size), 1);

                /* XOR Engine is disabled */
		xor_node = fdt_node_offset_by_compatible(blob, -1, "amcc,xor");
		if (xor_node >= 0)
                        fdt_del_node(blob, xor_node);
	
                /* PCIe is disabled */
                fdt_find_and_setprop(blob, "/plb/pciex@d00000000", "status",
                                "disabled", sizeof("disabled"), 1);

                /* SATA0 is disabled */
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
	ebc_ranges_prop[0] = 0;                         /* bank 0 */
	ebc_ranges_prop[1] = 0;                         /* always 0 */
	ebc_ranges_prop[2] = CONFIG_SYS_FLASH_BASE;     /* re-mapped NOR flash to this address */
	ebc_ranges_prop[3] = gd->bd->bi_flashsize;      /* NOR Flash size */
	ebc_ranges_prop[4] = 1;                         /* bank 1 */
	ebc_ranges_prop[5] = 0;                         /* always 0 */
	ebc_ranges_prop[6] = CONFIG_SYS_NAND0_ADDR;             /* NAND NDFC base */
	ebc_ranges_prop[7] = 0x2000;                    /* NAND NDFC size */
	/* NOR flash fixup */
	nor_reg_prop[0] = 0;                            /* NOR chip select = 0 */
	nor_reg_prop[1] = 0;                            /* always 0 */
	nor_reg_prop[2] = CONFIG_SYS_FLASH_SIZE;               /* NOR Flash size */
        /* Fixup NAND NDFC mapping */
	ndfc_reg_prop[0] = 1;                           /* NAND NDFC chip select = 1 */
	ndfc_reg_prop[1] = 0;                           /* always 0 */
	ndfc_reg_prop[2] = 0x2000;                      /* NAND NDFC size */
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
	rc = fdt_find_and_setprop(blob, "/plb/opb/ebc/ndfc@1,0", "reg",
		ndfc_reg_prop, sizeof(ndfc_reg_prop), 1);
	if (rc)
		printf("Unable to update property NAND NDFC mapping, err=%s\n",
			fdt_strerror(rc));

#else

	/* EBC ranges fixup */
	ebc_ranges_prop[0] = 0;                         /* bank 0 */
	ebc_ranges_prop[1] = 0;                         /* always 0 */
	ebc_ranges_prop[2] = CONFIG_SYS_NAND0_ADDR;     /* NAND0 NDFC base */
	ebc_ranges_prop[3] = 0x2000;      /* NOR Flash size */
	ebc_ranges_prop[4] = 1;                         /* bank 1 */
	ebc_ranges_prop[5] = 0;                         /* always 0 */
	ebc_ranges_prop[6] = CONFIG_SYS_NAND1_ADDR;             /* NAND1 NDFC base */
	ebc_ranges_prop[7] = 0x2000;                    /* NAND NDFC size */
	ebc_ranges_prop[8] = 0;                         /* bank 0 */
	ebc_ranges_prop[9] = 0;                         /* always 0 */
	ebc_ranges_prop[10] = CONFIG_SYS_FLASH_BASE;     /* re-mapped NOR flash to this address */
	ebc_ranges_prop[11] = gd->bd->bi_flashsize;      /* NOR Flash size */

	 /* Fixup NAND NDFC mapping */
	ndfc_reg_prop[0] = 0;                           /* NAND0 NDFC chip select = 0 */
	ndfc_reg_prop[1] = 0;                           /* always 0 */
	ndfc_reg_prop[2] = 0x2000;                      /* NAND NDFC size */

	/* Fixup NAND NDFC mapping */
	ndfc1_reg_prop[0] = 1;                           /* NAND NDFC chip select = 1 */
	ndfc1_reg_prop[1] = 0;                           /* always 0 */
	ndfc1_reg_prop[2] = 0x2000;                      /* NAND NDFC size */
	 /* NOR flash fixup */
        nor_reg_prop[0] = 2;                            /* NOR chip select = 2 */
        nor_reg_prop[1] = 0;                            /* always 0 */
        nor_reg_prop[2] = CONFIG_SYS_FLASH_SIZE;               /* NOR Flash size */

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
	rc = fdt_find_and_setprop(blob, "/plb/opb/ebc/ndfc@1,0", "reg",
		ndfc_reg_prop, sizeof(ndfc_reg_prop), 1);
	if (rc)
		printf("Unable to update property NAND0 NDFC mapping, err=%s\n",
		fdt_strerror(rc));
	rc = fdt_find_and_setprop(blob, "/plb/opb/ebc/ndfc@2,0", "reg",
		ndfc_reg_prop, sizeof(ndfc1_reg_prop), 1);
	if (rc)
		printf("Unable to update property NAND1 NDFC mapping\n");
#endif
}
#endif /* defined(CONFIG_OF_LIBFDT) && defined(CONFIG_OF_BOARD_SETUP) */
