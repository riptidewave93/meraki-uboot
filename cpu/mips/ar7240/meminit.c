/* 
 * Memory controller config:
 * Assumes that the caches are initialized.
 *
 * 0) Figah out the Tap controller settings.
 * 1) Figure out whether the interface is 16bit or 32bit.
 * 2) Size the DRAM
 *
 *  0) Tap controller settings
 *  --------------------------
 * The Table below provides all possible values of TAP controllers. We need to
 * find the extreme left and extreme right of the spectrum (of max_udelay and
 * min_udelay). We then program the TAP to be in the middle.
 * Note for this we would need to be able to read and write memory. So, 
 * initially we assume that a 16bit interface, which will always work unless
 * there is exactly _1_ 32 bit part...for now we assume this is not the case.
 * 
 * The algo:
 * 0) Program the controller in 16bit mode.
 * 1) Start with the extreme left of the table
 * 2) Write 0xa4, 0xb5, 0xc6, 0xd7 to 0, 2, 4, 6
 * 3) Read 0 - this will fetch the entire cacheline.
 * 4) If the value at address 4 is good, record this table entry, goto 6
 * 5) Increment to get the next table entry. Goto 2.
 * 6) Start with extreme right. Do the same as above.
 *
 * 1) 16bit or 32bit
 * -----------------
 *  31st bit of reg 0x1800_0000 will  determine the mode. By default, 
 *  controller is set to 32-bit mode. In 32 bit mode, full data bus DQ [31:0] 
 *  will be used to write 32 bit data. Suppose you have 16bit DDR memory
 *  (it will have 16bit wide data bus). If you try to write 16 bit DDR in 32 
 *  bit mode, you are going to miss upper 16 bits of data. Reading to that 
 *  location will give you only lower 16 bits correctly, upper 16 bits will 
 *  have some junk value. E.g.,
 *
 *  write to 0x0000_0000 0x12345678
 *  write to 0x0000_1000 0x00000000 (just to discharge DQ[31:16] )
 *  read from 0x0000_0000
 *  if u see something like 0x0000_5678 (or XXXX_5678 but not equal to 
 *  0x12345678) - its a 16 bit interface
 *
 *  2) Size the DRAM
 *  -------------------
 *  DDR wraps around. Write a pattern to 0x0000_0000. Write an address 
 *  pattern at 4M, 8M, 16M etc. and check when 0x0000_0000 gets overwritten.
 *
 *
 *  We can use #define's for all these addresses and patterns but its easier
 *  to see what's going on without :)
 */
#include <common.h>
#include <asm/addrspace.h>
#include "ar7240_soc.h"


uint8_t     tap_settings[] = 
            {0x40, 0x41, 0x10, 0x12, 0x13, 0x15, 0x1a, 0x1c, 0x1f, 0x2f, 0x3f};

uint16_t    tap_pattern[] = {0xa5, 0xb6, 0xc7, 0xd8};

void
ar7240_ddr_tap_set(uint8_t set)
{
    ar7240_reg_wr_nf(AR7240_DDR_TAP_CONTROL0, set);
    ar7240_reg_wr_nf(AR7240_DDR_TAP_CONTROL1, set);
    ar7240_reg_wr_nf(AR7240_DDR_TAP_CONTROL2, set);
    ar7240_reg_wr_nf(AR7240_DDR_TAP_CONTROL3, set);
}

/*
 * We check for size in 4M increments
 */
#define AR7240_DDR_SIZE_INCR    (4*1024*1024)
int
ar7240_ddr_find_size(void)
{
    uint8_t  *p = (uint8_t *)KSEG1, pat = 0x77;
    int i;

    *p = pat;

    for(i = 1; ; i++) {
        *(p + i * AR7240_DDR_SIZE_INCR) = (uint8_t)(i);
        if (*p != pat) {
            break;
        }
    }

    return (i*AR7240_DDR_SIZE_INCR);
}

void
ar7240_ddr_initial_config(uint32_t refresh)
{
#ifndef COMPRESSED_UBOOT

	int ddr2 = 0,ddr_config;
	int ddr_config2,ext_mod,ddr2_ext_mod;
	int mod_val,mod_val_init;

	printf("\nsri\n");
#else
	int mod_val = CFG_DDR_MODE_VAL;
#endif /* #ifndef COMPRESSED_UBOOT */
#if 0
	ar7240_reg_wr(AR7240_RESET, AR7240_RESET_DDR);
	udelay(10);
#endif
#ifndef COMPRESSED_UBOOT
	ddr2 = ((ar7240_reg_rd(0xb8050020) & 0x1) == 0);
#ifdef ENABLE_DYNAMIC_CONF
	if(*(volatile int *)CFG_DDR_MAGIC_F == CFG_DDR_MAGIC){
		ddr_config = CFG_DDR_CONFIG_VAL_F;
		ddr_config2 = CFG_DDR_CONFIG2_VAL_F;
		ext_mod = CFG_DDR_EXT_MODE_VAL_F;
		ddr2_ext_mod = ext_mod;
	}
	else
#endif
	{
#ifdef CONFIG_SUPPORT_AR7241
		if (is_ar7241() || is_ar7242()) {
			if (ddr2) {
				printf("%s(%d): virian ddr2 init\n", __func__, __LINE__);
				ddr_config	= CFG_7241_DDR2_CONFIG_VAL;
				ddr_config2	= CFG_7241_DDR2_CONFIG2_VAL;
				ext_mod		= CFG_7241_DDR2_EXT_MODE_VAL;
				ddr2_ext_mod	= CFG_DDR2_EXT_MODE_VAL;
				mod_val_init	= CFG_7241_DDR2_MODE_VAL_INIT;
				mod_val		= CFG_7241_DDR2_MODE_VAL;
			} else {
				printf("%s(%d): virian ddr1 init\n", __func__, __LINE__);
				ddr_config	= CFG_7241_DDR1_CONFIG_VAL;
				ddr_config2	= CFG_7241_DDR1_CONFIG2_VAL;
				ext_mod		= CFG_7241_DDR1_EXT_MODE_VAL;
				ddr2_ext_mod	= CFG_DDR2_EXT_MODE_VAL;
				mod_val_init	= CFG_7241_DDR1_MODE_VAL_INIT;
				mod_val		= CFG_7241_DDR1_MODE_VAL;
			}
		}
		else
#endif
		{
			printf("%s(%d): python ddr init\n", __func__, __LINE__);
			ddr_config = CFG_DDR_CONFIG_VAL;
			ddr_config2 = CFG_DDR_CONFIG2_VAL;
			ext_mod = CFG_DDR_EXT_MODE_VAL;
			ddr2_ext_mod = CFG_DDR2_EXT_MODE_VAL;
			mod_val_init = CFG_DDR_MODE_VAL_INIT;
			mod_val = CFG_DDR_MODE_VAL;
		}
	}

	if (ddr2) {
		ar7240_reg_wr_nf(0xb800008c, 0xA59);
		udelay(100);
		ar7240_reg_wr_nf(AR7240_DDR_CONTROL, 0x10);
		udelay(10);
		ar7240_reg_wr_nf(AR7240_DDR_CONTROL, 0x20);
		udelay(10);
	}

	ar7240_reg_wr_nf(AR7240_DDR_CONFIG, ddr_config);
	udelay(100);
	ar7240_reg_wr_nf(AR7240_DDR_CONFIG2, ddr_config2 | 0x80);
	udelay(100);
	ar7240_reg_wr_nf(AR7240_DDR_CONTROL, 0x8);
	udelay(10);

	ar7240_reg_wr_nf(AR7240_DDR_MODE, mod_val_init);
	udelay(1000);

	ar7240_reg_wr_nf(AR7240_DDR_CONTROL, 0x1);
	udelay(10);

	if (ddr2) {
		ar7240_reg_wr_nf(AR7240_DDR_EXT_MODE, ddr2_ext_mod);
	} else {
		ar7240_reg_wr_nf(AR7240_DDR_EXT_MODE, ext_mod);
	}

#endif /* #ifndef COMPRESSED_UBOOT */
	udelay(100);
	ar7240_reg_wr_nf(AR7240_DDR_CONTROL, 0x2);
	udelay(10);
	ar7240_reg_wr_nf(AR7240_DDR_CONTROL, 0x8);
	udelay(10);
	ar7240_reg_wr_nf(AR7240_DDR_MODE, mod_val);
	udelay(100);
	ar7240_reg_wr_nf(AR7240_DDR_CONTROL, 0x1);
	udelay(10);
	ar7240_reg_wr_nf(AR7240_DDR_REFRESH, refresh);
	udelay(100);
	ar7240_reg_wr_nf(AR7240_DDR_RD_DATA_THIS_CYCLE,
				CFG_DDR_RD_DATA_THIS_CYCLE_VAL);
	udelay(100);
}
