/*
 * Copyright (C) 2013, Broadcom Corporation. All Rights Reserved. 
 *  
 * Permission to use, copy, modify, and/or distribute this software for any 
 * purpose with or without fee is hereby granted, provided that the above 
 * copyright notice and this permission notice appear in all copies. 
 *  
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES 
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY 
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES 
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION 
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. 
 */
#include <common.h>
#include <asm/io.h>
#include <asm/mach-types.h>
#include <configs/iproc_board.h>
#include <asm/arch/iproc.h>
#include <asm/arch/socregs.h>
#include <asm/arch/reg_utils.h>
#include <asm/system.h>
#include <iproc_gpio.h>
#include <iproc_gpio_cfg.h>

DECLARE_GLOBAL_DATA_PTR;

extern int bcmiproc_eth_register(u8 dev_num);

#ifdef CONFIG_BOARD_VENOM
extern void venom_setup(void);
#endif

/*****************************************
 * board_init -early hardware init
 *****************************************/
int board_init (void)
{
    gd->bd->bi_arch_number = CONFIG_MACH_TYPE;      /* board id for linux */
    gd->bd->bi_boot_params = LINUX_BOOT_PARAM_ADDR; /* adress of boot parameters */

    return 0;
}

/*****************************************************************
 * misc_init_r - miscellaneous platform dependent initializations
 ******************************************************************/
int misc_init_r (void)
{
    return(0);
}

/**********************************************
 * dram_init - sets uboots idea of sdram size
 **********************************************/
int dram_init (void)
{
#ifdef CONFIG_L2C_AS_RAM
	ddr_init();
#endif
    gd->ram_size = CONFIG_PHYS_SDRAM_1_SIZE - CONFIG_PHYS_SDRAM_RSVD_SIZE;

    return 0;
}

#ifdef CONFIG_PWM0_FOR_FAN
void bcm95301x_config_pwm0_4_fan(void)
{
	reg32_set_bits(ChipcommonB_GP_AUX_SEL, 0x01); // Select PWM 0 
	reg32_clear_bits(CRU_GPIO_CONTROL0_OFFSET, 1 << 8); // Selct PWM
	reg32_write_masked(ChipcommonB_PWM_PRESCALE, 0x3F, 24); // prescale = 24 + 1
	reg32_write(ChipcommonB_PWM_PERIOD_COUNT0, 5 * 180); 
	reg32_write(ChipcommonB_PWM_DUTYHI_COUNT0, 5 * 135);
	reg32_set_bits(ChipcommonB_PWMCTL, 0x101); // Enable PWM0
}
#endif

int board_early_init_f (void)
{
	int sku_id;
	sku_id = (reg32_read((volatile uint32_t *)ROM_S0_IDM_IO_STATUS) >> 2) & 0x03;
	if(sku_id==0)
	iproc_config_armpll(1000);
	else
	iproc_config_armpll(800);
	return 0; 
}

static void pwm_clock_word(int cki, int sdi, uint16_t word)
{
	int bit;

	for (bit = 0x8000; bit; bit >>= 1) {
		setGPIOasOutput(sdi, ((word & bit) != 0));
		setGPIOasOutput(cki, 1);
		udelay(1);
		setGPIOasOutput(cki, 0);
		udelay(1);
	}
}

static void led_set_rgb(int red, int green, int blue, int cki, int lei, int sdi)
{
	pwm_clock_word(cki, sdi, (blue << 8) | blue);
	pwm_clock_word(cki, sdi, (green << 8) | green);
	pwm_clock_word(cki, sdi, (red << 8) | red);
	setGPIOasOutput(lei, 1);
	udelay(600);
	setGPIOasOutput(lei, 0);
}

static void gpio_setup(void)
{
	iproc_gpio_chip *gpiodev;
	int i;

	iprocGPIOInit();

	gpiodev = getGPIODev();

	for (i = 0; i <= 2; i++) {
		iproc_gpio_set_config(gpiodev, i, IPROC_GPIO_GENERAL);
		setGPIOasOutput(i, 0);
	}

	for (i = 12; i <= 13; i++) {
		iproc_gpio_set_config(gpiodev, i, IPROC_GPIO_GENERAL);
		setGPIOasOutput(i, 0);
	}



	led_set_rgb(255, 0, 255, 0, 1, 2);

	iproc_gpio_set_config(gpiodev, 11, IPROC_GPIO_GENERAL);
	setGPIOasInput(11);
	if (!getGPIOpinValue(11)) {
		setenv("bootcmd", "run meraki_install");
	}
}

int board_late_init (void) 
{
	int status = 0;
#ifdef CONFIG_L2C_AS_RAM
	int i;
	extern ulong *mmu_table_addr;    /* MMU on flash fix: 16KB aligned */
    
#if 0
	for (i = 0; i < 4096; i++) {
		printf("TLB entry %x(@%p) = %lx\n", i, &mmu_table_addr[i],
		       mmu_table_addr[i]);
	}
#endif
	for (i = 0; i < 128; i++) {
//		mmu_table_addr[i] = (i << 20) | 0x5c06;
		mmu_table_addr[i] = (i << 20) | (3 << 10) | 0x06;
	}
#endif /* CONFIG_L2C_AS_RAM */
	/* unlock the L2 Cache */
#ifndef CONFIG_NO_CODE_RELOC
	printf("Unlocking L2 Cache ...");
	l2cc_unlock();
	printf("Done\n");
#endif

#ifndef CONFIG_SKIP_LOWLEVEL_INIT
#ifdef CONFIG_L2C_AS_RAM
	/* MMU on flash fix: 16KB aligned */
	asm volatile ("mcr p15, 0, r1, c8, c7, 0");  /*invalidate TLB*/
	asm volatile ("mcr p15, 0, r1, c7, c10, 4"); /* DSB */
	asm volatile ("mcr p15, 0, r0, c7, c5, 4"); /* ISB */		
	printf("About to set TTBR0 to %p\n", mmu_table_addr);
	asm volatile ("mcr p15, 0, %0, c2, c0, 0"::"r"(mmu_table_addr)); /*update TTBR0 */
	asm volatile ("mcr p15, 0, r1, c8, c7, 0");  /*invalidate TLB*/
	asm volatile ("mcr p15, 0, r1, c7, c10, 4"); /* DSB */
	asm volatile ("mcr p15, 0, r0, c7, c5, 4"); /* ISB */		
	printf("Set TTBR0 to %p\n", mmu_table_addr);
#endif
#endif

	/* Systick initialization(private timer)*/
	iproc_clk_enum();
	//glb_tim_init();
#ifdef CONFIG_BOARD_VENOM
	venom_setup();
#endif
#ifndef STDK_BUILD
	disable_interrupts();
#ifdef CONFIG_PWM0_FOR_FAN
	bcm95301x_config_pwm0_4_fan();
#endif
#else
	gic_disable_interrupt(29);
	irq_install_handler(29, systick_isr, NULL);
	gic_config_interrupt(29, 1, IPROC_INTR_LEVEL_SENSITIVE, 0, IPROC_GIC_DIST_IPTR_CPU0);
	iproc_systick_init(10000);

	/* MMU and cache setup */
	disable_interrupts();
	//printf("Enabling SCU\n");
	scu_enable();

	printf("Enabling icache and dcache\n");
	dcache_enable();
	icache_enable();

	printf("Enabling l2cache\n");
	status = l2cc_enable();
	//printf("Enabling done, status = %d\n", status);

	enable_interrupts();
#endif
	return status;
}

int board_eth_init(bd_t *bis)
{
	int rc = -1;
#ifdef CONFIG_BCMIPROC_ETH
	printf("Registering eth\n");
	rc = bcmiproc_eth_register(0);
#endif
	return rc;
}
