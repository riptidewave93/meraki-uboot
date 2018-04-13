/*
 * cpu/ppc4xx/4xx_ibm_ddr2_autocalib.c
 * This SPD SDRAM detection code supports AMCC PPC44x cpu's with a
 * DDR2 controller (non Denali Core). Those currently are:
 *
 * 405:		405EX
 * 440/460:	440SP/440SPe/460EX/460GT/460SX
 *
 * (C) Copyright 2008 Applied Micro Circuits Corporation
 * Adam Graham  <agraham@amcc.com>
 *
 * (C) Copyright 2007-2008
 * Stefan Roese, DENX Software Engineering, sr@denx.de.
 *
 * COPYRIGHT   AMCC   CORPORATION 2004
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 */

/* 
 * Define DEBUG for debugging output (obviously ;-)) 
 * Should undefine DEBUG when building SRAM U-Boot
 * otherwise, the build may fail.
 */
#undef DEBUG

#include <common.h>
#include <ppc4xx.h>
#include <asm/io.h>
#include <asm/processor.h>

#if defined(CONFIG_PPC4xx_DDR_AUTOCALIBRATION)

/*
 * Only compile the DDR auto-calibration code for NOR boot or
 * NAND SPL with auto-calibration enabled but not for NAND U-Boot - NUB
 */
#if ( (!defined(CONFIG_NAND_U_BOOT) && !defined(CONFIG_NAND_SPL)) || \
	(defined(CONFIG_NAND_SPL) && \
          defined(CONFIG_NAND_SPL_WITH_DDR_CALIBRATION)) )
        

#define MAXBXCF			4
#define SDRAM_RXBAS_SHIFT_1M	20

#if defined(CONFIG_SYS_DECREMENT_PATTERNS)
#define NUMMEMTESTS		24
#else
#define NUMMEMTESTS		8
#endif /* CONFIG_SYS_DECREMENT_PATTERNS */


#define NUMLOOPS		1	
#define NUMMEMWORDS		16
#define NUMTESTPATTERNS		8
#define MEMTESTRANGE		16

#define SDRAM_RDCC_RDSS_VAL(n)	SDRAM_RDCC_RDSS_DECODE(ddr_rdss_opt(n))

/*
 * Newer PPC's like 440SPe, 460EX/GT can be equipped with more than 2GB of SDRAM.
 * To support such configurations, we "only" map the first 2GB via the TLB's. We
 * need some free virtual address space for the remaining peripherals like, SoC
 * devices, FLASH etc.
 *
 * Note that ECC is currently not supported on configurations with more than 2GB
 * SDRAM. This is because we only map the first 2GB on such systems, and therefore
 * the ECC parity byte of the remaining area can't be written.
 */
#ifndef CONFIG_MAX_MEM_MAPPED
#define CONFIG_MAX_MEM_MAPPED   ((phys_size_t)2 << 30)
#endif

#if defined(CFG_DDR_EXTRA_MEMTEST) || defined(CFG_DDR_EXTRA_MEMTEST2)
#if !defined(CFG_BEGIN_BANK_TEST) && !defined(CFG_MIDLLE_BANK_TEST) && !defined(CFG_END_BANK_TEST)
#error "Must define CFG_BEGIN_BANK_TEST or CFG_MIDLLE_BANK_TEST or CFG_END_BANK_TEST for extra memory testing"
#endif
#endif
/* Private Structure Definitions */
struct ddrautocal {
	u32 rffd_size;
	u32 rqfd_size;
	u32 rffd_mid;
	u32 rqfd_mid;
	u32 hos_ct;
	
	u32 wdtr;
	u32 clkp;
	u32 rdcc;
};

struct ddr_rffd {
	u32 size;
	u32 start;
	u32 end;
	u32 rdcc;
	u32 hos_ct;
};

struct sdram_timing {
        u32 wrdtr;
        u32 clktr;
};

/*--------------------------------------------------------------------------+
 * Prototypes
 *--------------------------------------------------------------------------*/
extern u32 mfdcr_any(u32);
extern void mtdcr_any(u32, u32);
extern u32 ddr_wrdtr(u32);
extern inline void ppc4xx_ibm_ddr2_register_dump(void);
extern void inline blank_string(int size);
extern void spd_ddr_init_hang(void);

static u32 program_DQS_calibration(struct ddrautocal *);
#if defined(CFG_DDR_EXTRA_MEMTEST) || defined(CFG_DDR_EXTRA_MEMTEST2)
static u32 get_bank_mode(int bxcr_num);
#endif
static int short_mem_test(volatile u32 *, u32 *);

/*
 * To provide an interface for board specific config values in this common
 * DDR setup code, we implement he "weak" default functions here. They return
 * the default value back to the caller.
 *
 * Please see include/configs/yucca.h for an example fora board specific
 * implementation.
 */

#if !defined(CONFIG_SPD_EEPROM)
u32 __ddr_wrdtr(u32 default_val)
{
	return default_val;
}
u32 ddr_wrdtr(u32) __attribute__((weak, alias("__ddr_wrdtr")));

u32 __ddr_clktr(u32 default_val)
{
	return default_val;
}
u32 ddr_clktr(u32) __attribute__((weak, alias("__ddr_clktr")));

/*
 * Board-specific Platform code can reimplement spd_ddr_init_hang () if needed
 */
void __spd_ddr_init_hang(void)
{
	hang();
}
void
spd_ddr_init_hang(void) __attribute__((weak, alias("__spd_ddr_init_hang")));
#endif /* defined(CONFIG_SPD_EEPROM) */

u32 __ddr_rdss_opt(u32 default_val)
{
	return default_val;
}
u32 ddr_rdss_opt(ulong) __attribute__((weak, alias("__ddr_rdss_opt")));


/* 
 * Autocalibration 
 * The following 5 function scan be overridden via platform specific versions
 * These should be used to reduce scan ranges once data has been collected 
 * determining valid ranges across multiple platforms.
 * 
 * THe larger the dataset of boards used during the collection, the 
 * more restricted the scan range can be made. The RFFD scan range should be 
 * padded to account for temperature variations.
 */
ulong __ddr_scan_option(ulong default_val)
{
	return default_val;
}
ulong ddr_scan_option(ulong) __attribute__((weak, alias("__ddr_scan_option")));

u32 __ddr_rqfd_start_opt(u32 default_val)
{
	return default_val;
}
u32 ddr_rqfd_start_opt(u32) __attribute__((weak, alias("__ddr_rqfd_start_opt")));

u32 __ddr_rqfd_end_opt(u32 default_val)
{
	return default_val;
}
u32 ddr_rqfd_end_opt(u32) __attribute__((weak, alias("__ddr_rqfd_end_opt")));

u32 __ddr_rffd_start_opt(u32 default_val)
{
	return default_val;
}
u32 ddr_rffd_start_opt(u32) __attribute__((weak, alias("__ddr_rffd_start_opt")));

u32 __ddr_rffd_end_opt(u32 default_val)
{
	return default_val;
}
u32 ddr_rffd_end_opt(u32) __attribute__((weak, alias("__ddr_rffd_end_opt")));


/* Decode the memory start address from register settings */
static u32 *get_membase(int bxcr_num)
{
	ulong bxcf;
	u32 *membase;

#if defined(SDRAM_R0BAS)
	/* BAS from Memory Queue rank reg. */
	membase =
	    (u32 *)(SDRAM_RXBAS_SDBA_DECODE(mfdcr_any(SDRAM_R0BAS+bxcr_num)));
	bxcf = 0;	/* just to satisfy the compiler */
#else
	/* BAS from SDRAM_MBxCF mem rank reg. */
	mfsdram(SDRAM_MB0CF + (bxcr_num<<2), bxcf);
	membase = (u32 *)((bxcf & 0xfff80000) << 3);
#endif

	return membase;
}

static int is_able_to_test(u32* membase)
{
	if ((u32)membase >= ((CONFIG_MAX_MEM_MAPPED) - 
		(((sizeof(u32) * NUMTESTPATTERNS) *  MEMTESTRANGE * NUMLOOPS) + 2))) 
		return 0;
	return 1;
}

static u32 get_memsize(int bxcr_num)
{
	ulong bxcf;
	u32 memsize;

#if defined(SDRAM_R0BAS)
	/* BAS from Memory Queue rank reg. */
	bxcf = mfdcr_any(SDRAM_R0BAS+bxcr_num);
#else
	/* BAS from SDRAM_MBxCF mem rank reg. */
	mfsdram(SDRAM_MB0CF + (bxcr_num<<2), bxcf);
#endif
	memsize = (bxcf & SDRAM_RXBAS_SDSZ_MASK);
	switch(memsize) {
		case SDRAM_RXBAS_SDSZ_8:
			memsize = 8;
			break;
		case SDRAM_RXBAS_SDSZ_16:
			memsize = 16;
			break;
		case SDRAM_RXBAS_SDSZ_32:
			memsize = 32;
			break;
		case SDRAM_RXBAS_SDSZ_64:
			memsize = 64;
			break;
		case SDRAM_RXBAS_SDSZ_128:
			memsize = 128;
			break;
		case SDRAM_RXBAS_SDSZ_256:
			memsize = 256;
			break;
		case SDRAM_RXBAS_SDSZ_512:
			memsize = 512;
			break;
		case SDRAM_RXBAS_SDSZ_1024:
			memsize = 1024;
			break;
		case SDRAM_RXBAS_SDSZ_2048:
			memsize = 2048;
			break;
		case SDRAM_RXBAS_SDSZ_4096:
			memsize = 4096;
			break;
	};

	return memsize;
}

static inline void ecc_clear_status_reg(void)
{
	mtsdram(SDRAM_ECCCR, 0xffffffff);
#if defined(SDRAM_R0BAS)
	mtdcr(SDRAM_ERRSTATLL, 0xffffffff);
#endif
}

/*
 * Reset and relock memory DLL after SDRAM_CLKTR change
 */
static inline void relock_memory_DLL(void)
{
	u32 reg;

	mtsdram(SDRAM_MCOPT2, SDRAM_MCOPT2_IPTR_EXECUTE);

	do {
		mfsdram(SDRAM_MCSTAT, reg);
	} while (!(reg & SDRAM_MCSTAT_MIC_COMP));

	mfsdram(SDRAM_MCOPT2, reg);
	mtsdram(SDRAM_MCOPT2, reg | SDRAM_MCOPT2_DCEN_ENABLE);

	/* make sure delay-line calibration is ready */
	do {
		mfsdram(SDRAM_DLCR, reg);
	} while ((reg & SDRAM_DLCR_DLCS_CONT_DONE) != 
		SDRAM_DLCR_DLCS_CONT_DONE);

}

static int ecc_check_status_reg(void)
{
	u32 ecc_status;

	/*
	 * Compare suceeded, now check
	 * if got ecc error. If got an
	 * ecc error, then don't count
	 * this as a passing value
	 */
	mfsdram(SDRAM_ECCCR, ecc_status);
	if (ecc_status != 0x00000000) {
		/* clear on error */
		ecc_clear_status_reg();
		/* ecc check failure */
		return 0;
	}
	ecc_clear_status_reg();
	sync();

	return 1;
}

#if defined(CFG_DDR_EXTRA_MEMTEST) || defined(CFG_DDR_EXTRA_MEMTEST2)
/* TODO: Move this definition to .h file */
#define SDRAM_RXBAS_SDAM_MODE_MASK	0x00000F00

static u32 get_bank_mode(int bxcr_num)
{
        ulong bxcf;
        u32 mode,bank;

        /* BAS from SDRAM_MBxCF mem rank reg. */
        mfsdram(SDRAM_MB0CF + (bxcr_num << 2), bxcf);
        mode = (bxcf & SDRAM_RXBAS_SDAM_MODE_MASK) >> 8;

	bank = (mode < 5) ? 4 : 8;	

	/* Debug */
	debug("Mode: 0x%x\tNum of banks: %d\n", mode, bank);

        return bank;
}
#endif

/*
 * Returns Hit Oversample Count from FCSR & RTSR
 * Status Feedback registers.
 *
 * If no HOS are found, returns 0 and the test should be 
 * considered failed. 
 */
#if 0
static int short_mem_test(volatile u32 *base_address, u32 *hos_count)
{
	int i, j, l;
	u32 ecc_mode = 0,
		fcsr_reg = 0,
		rtsr_reg = 0,
		hos_ct = 0;
	
	ulong test[NUMMEMTESTS][NUMMEMWORDS] = {
	/* 0 */	{0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF,
		 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF,
		 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF,
		 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF},
	/* 1 */	{0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000,
		 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000,
		 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000,
		 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000},
	/* 2 */	{0xAAAAAAAA, 0xAAAAAAAA, 0x55555555, 0x55555555,
		 0xAAAAAAAA, 0xAAAAAAAA, 0x55555555, 0x55555555,
		 0xAAAAAAAA, 0xAAAAAAAA, 0x55555555, 0x55555555,
		 0xAAAAAAAA, 0xAAAAAAAA, 0x55555555, 0x55555555},
	/* 3 */	{0x55555555, 0x55555555, 0xAAAAAAAA, 0xAAAAAAAA,
		 0x55555555, 0x55555555, 0xAAAAAAAA, 0xAAAAAAAA,
		 0x55555555, 0x55555555, 0xAAAAAAAA, 0xAAAAAAAA,
		 0x55555555, 0x55555555, 0xAAAAAAAA, 0xAAAAAAAA},
	/* 4 */	{0xA5A5A5A5, 0xA5A5A5A5, 0x5A5A5A5A, 0x5A5A5A5A,
		 0xA5A5A5A5, 0xA5A5A5A5, 0x5A5A5A5A, 0x5A5A5A5A,
		 0xA5A5A5A5, 0xA5A5A5A5, 0x5A5A5A5A, 0x5A5A5A5A,
		 0xA5A5A5A5, 0xA5A5A5A5, 0x5A5A5A5A, 0x5A5A5A5A},
	/* 5 */	{0x5A5A5A5A, 0x5A5A5A5A, 0xA5A5A5A5, 0xA5A5A5A5,
		 0x5A5A5A5A, 0x5A5A5A5A, 0xA5A5A5A5, 0xA5A5A5A5,
		 0x5A5A5A5A, 0x5A5A5A5A, 0xA5A5A5A5, 0xA5A5A5A5,
		 0x5A5A5A5A, 0x5A5A5A5A, 0xA5A5A5A5, 0xA5A5A5A5},
	/* 6 */	{0xAA55AA55, 0xAA55AA55, 0x55AA55AA, 0x55AA55AA,
		 0xAA55AA55, 0xAA55AA55, 0x55AA55AA, 0x55AA55AA,
		 0xAA55AA55, 0xAA55AA55, 0x55AA55AA, 0x55AA55AA,
		 0xAA55AA55, 0xAA55AA55, 0x55AA55AA, 0x55AA55AA},
	/* 7 */	{0x55AA55AA, 0x55AA55AA, 0xAA55AA55, 0xAA55AA55,
		 0x55AA55AA, 0x55AA55AA, 0xAA55AA55, 0xAA55AA55,
		 0x55AA55AA, 0x55AA55AA, 0xAA55AA55, 0xAA55AA55,
		 0x55AA55AA, 0x55AA55AA, 0xAA55AA55, 0xAA55AA55},

#if defined(CONFIG_SYS_DECREMENT_PATTERNS)
	/* 8 */	{0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
		 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
		 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
		 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff},
	/* 9 */	{0xfffefffe, 0xfffefffe, 0xfffefffe, 0xfffefffe,
		 0xfffefffe, 0xfffefffe, 0xfffefffe, 0xfffefffe,
		 0xfffefffe, 0xfffefffe, 0xfffefffe, 0xfffefffe,
		 0xfffefffe, 0xfffefffe, 0xfffefffe, 0xfffefffe},
	/* 10 */{0xfffdfffd, 0xfffdfffd, 0xfffdffff, 0xfffdfffd,
		 0xfffdfffd, 0xfffdfffd, 0xfffdffff, 0xfffdfffd,
		 0xfffdfffd, 0xfffdfffd, 0xfffdffff, 0xfffdfffd,
		 0xfffdfffd, 0xfffdfffd, 0xfffdffff, 0xfffdfffd},
	/* 11 */{0xfffcfffc, 0xfffcfffc, 0xfffcfffc, 0xfffcfffc,
		 0xfffcfffc, 0xfffcfffc, 0xfffcfffc, 0xfffcfffc,
		 0xfffcfffc, 0xfffcfffc, 0xfffcfffc, 0xfffcfffc,
		 0xfffcfffc, 0xfffcfffc, 0xfffcfffc, 0xfffcfffc},
	/* 12 */{0xfffbfffb, 0xfffffffb, 0xfffffffb, 0xfffffffb,
		 0xfffbfffb, 0xfffffffb, 0xfffffffb, 0xfffffffb,
		 0xfffbfffb, 0xfffffffb, 0xfffffffb, 0xfffffffb,
		 0xfffbfffb, 0xfffffffb, 0xfffffffb, 0xfffffffb},
	/* 13 */{0xfffafffa, 0xfffafffa, 0xfffffffa, 0xfffafffa,
		 0xfffafffa, 0xfffafffa, 0xfffafffa, 0xfffafffa,
		 0xfffafffa, 0xfffafffa, 0xfffafffa, 0xfffafffa,
		 0xfffafffa, 0xfffafffa, 0xfffafffa, 0xfffafffa},
	/* 14 */{0xfff9fff9, 0xfff9fff9, 0xfff9fff9, 0xfff9fff9,
		 0xfff9fff9, 0xfff9fff9, 0xfff9fff9, 0xfff9fff9,
		 0xfff9fff9, 0xfff9fff9, 0xfff9fff9, 0xfff9fff9,
		 0xfff9fff9, 0xfff9fff9, 0xfff9fff9, 0xfff9fff9},
	/* 15 */{0xfff8fff8, 0xfff8fff8, 0xfff8fff8, 0xfff8fff8,
		 0xfff8fff8, 0xfff8fff8, 0xfff8fff8, 0xfff8fff8,
		 0xfff8fff8, 0xfff8fff8, 0xfff8fff8, 0xfff8fff8,
		 0xfff8fff8, 0xfff8fff8, 0xfff8fff8, 0xfff8fff8},
	/* 16 */{0xfff7fff7, 0xfff7ffff, 0xfff7fff7, 0xfff7fff7,
		 0xfff7fff7, 0xfff7ffff, 0xfff7fff7, 0xfff7fff7,
		 0xfff7fff7, 0xfff7ffff, 0xfff7fff7, 0xfff7fff7,
		 0xfff7ffff, 0xfff7ffff, 0xfff7fff7, 0xfff7fff7},
	/* 17 */{0xfff6fff5, 0xfff6ffff, 0xfff6fff6, 0xfff6fff7,
		 0xfff6fff5, 0xfff6ffff, 0xfff6fff6, 0xfff6fff7,
		 0xfff6fff5, 0xfff6ffff, 0xfff6fff6, 0xfff6fff7,
		 0xfff6fff5, 0xfff6ffff, 0xfff6fff6, 0xfff6fff7},
	/* 18 */{0xfff5fff4, 0xfff5ffff, 0xfff5fff5, 0xfff5fff5,
		 0xfff5fff4, 0xfff5ffff, 0xfff5fff5, 0xfff5fff5,
		 0xfff5fff4, 0xfff5ffff, 0xfff5fff5, 0xfff5fff5,
		 0xfff5fff4, 0xfff5ffff, 0xfff5fff5, 0xfff5fff5},
	/* 19 */{0xfff4fff3, 0xfff4ffff, 0xfff4fff4, 0xfff4fff4,
		 0xfff4fff3, 0xfff4ffff, 0xfff4fff4, 0xfff4fff4,
		 0xfff4fff3, 0xfff4ffff, 0xfff4fff4, 0xfff4fff4,
		 0xfff4fff3, 0xfff4ffff, 0xfff4fff4, 0xfff4fff4},
	/* 20 */{0xfff3fff2, 0xfff3ffff, 0xfff3fff3, 0xfff3fff3,
		 0xfff3fff2, 0xfff3ffff, 0xfff3fff3, 0xfff3fff3,
		 0xfff3fff2, 0xfff3ffff, 0xfff3fff3, 0xfff3fff3,
		 0xfff3fff2, 0xfff3ffff, 0xfff3fff3, 0xfff3fff3},
	/* 21 */{0xfff2ffff, 0xfff2ffff, 0xfff2fff2, 0xfff2fff2,
		 0xfff2ffff, 0xfff2ffff, 0xfff2fff2, 0xfff2fff2,
		 0xfff2ffff, 0xfff2ffff, 0xfff2fff2, 0xfff2fff2,
		 0xfff2ffff, 0xfff2ffff, 0xfff2fff2, 0xfff2fff2},
	/* 22 */{0xfff1ffff, 0xfff1ffff, 0xfff1fff1, 0xfff1fff1,
		 0xfff1ffff, 0xfff1ffff, 0xfff1fff1, 0xfff1fff1,
		 0xfff1ffff, 0xfff1ffff, 0xfff1fff1, 0xfff1fff1,
		 0xfff1ffff, 0xfff1ffff, 0xfff1fff1, 0xfff1fff1},
	/* 23 */{0xfff0fff0, 0xfff0fff0, 0xfff0fff0, 0xfff0fff0,
		 0xfff0fff0, 0xfff0fff0, 0xfff0fff0, 0xfff0fff0,
		 0xfff0fff0, 0xfff0fff0, 0xfff0fff0, 0xfff0fff0,
		 0xfff0fff0, 0xfff0fffe, 0xfff0fff0, 0xfff0fff0},
#endif /* CONFIG_SYS_DECREMENT_PATTERNS */
								 };
	mfsdram(SDRAM_MCOPT1, ecc_mode);
	if ((ecc_mode & SDRAM_MCOPT1_MCHK_CHK_REP) ==
						SDRAM_MCOPT1_MCHK_CHK_REP) {
		ecc_clear_status_reg();
		sync();
		ecc_mode = 1;
	} else {
		ecc_mode = 0;
	}

	/*
	 * Run the short memory test.
	 * Write patterns then read and verify.
	 */
	
	for (i = 0; i < NUMMEMTESTS; i++) {
		for (j = 0; j < NUMMEMWORDS; j++) {
			base_address[j] = test[i][j];
			//ppcDcbf((ulong)&(base_address[j]));

			/* capture for oversample tracking */
			mfsdram(SDRAM_FCSR, fcsr_reg);
			/* capture RTR for oversample tracking */
			mfsdram(SDRAM_RTSR, rtsr_reg);
			/* this register is sticky, so clear it */
			mtsdram(SDRAM_RTSR, 0);
			/* track hit oversamples - all must match */
			if ((rtsr_reg & SDRAM_RTSR_RHOS) && 
				!(fcsr_reg & SDRAM_FCSR_CMOS_MISSED))
					hos_ct++;
			//else return 0; /* no reason to continue test */
		}
		sync();
		iobarrier_rw();
		for (l = 0; l < NUMLOOPS; l++) {
			for (j = 0; j < NUMMEMWORDS; j++) {
				if (base_address[j] != test[i][j]) {
					//ppcDcbf((u32)&(base_address[j]));
					*hos_count += hos_ct;
					return 0;
				} else {

					/* capture for oversample tracking */
					mfsdram(SDRAM_FCSR, fcsr_reg);
					/* capture RTR for oversample tracking */
					mfsdram(SDRAM_RTSR, rtsr_reg);
					/* this register is sticky, so clear it */
					mtsdram(SDRAM_RTSR, 0);
					/* track hit oversamples - all must match */
					if ((rtsr_reg & SDRAM_RTSR_RHOS) && 
						!(fcsr_reg & SDRAM_FCSR_CMOS_MISSED))
							hos_ct++;

					if (ecc_mode) {
						if (!ecc_check_status_reg())
							return 0;
					}
				}
				//ppcDcbf((u32)&(base_address[j]));
			} /* for (j = 0; j < NUMMEMWORDS; j++) */
			sync();
			iobarrier_rw();
		} /* for (l=0; l<NUMLOOPS; l++) */
	}
	
	*hos_count += hos_ct;
	return 1;
}
#else /* adjusted memory test routine */
static int short_mem_test(volatile u32 *base_address, u32 *hos_count)
{
	int i, j, l;
	u32 ecc_mode = 0,
		fcsr_reg = 0,
		rtsr_reg = 0,
		hos_ct = 0;
	volatile u32 *address = base_address;
	
	u32 test[NUMTESTPATTERNS] = {
		0x00000000, 0xffffffff, 0xaaaaaaaa, 0x55555555, 
		0xcccccccc, 0x66666666, 0x33333333, 0x00000000
	};

	mfsdram(SDRAM_MCOPT1, ecc_mode);
	if ((ecc_mode & SDRAM_MCOPT1_MCHK_CHK_REP) ==
						SDRAM_MCOPT1_MCHK_CHK_REP) {
		ecc_clear_status_reg();
		sync();
		ecc_mode = 1;
	} else {
		ecc_mode = 0;
	}

	/*
	 * Run the short memory test.
	 * Write patterns then read and verify.
	 */
	
	for (l = 0; l < NUMLOOPS; l++) {
		for (i = 0; i < NUMTESTPATTERNS; i++) {
			for (j = 0; j < MEMTESTRANGE; j++) {
				address[j] = test[i];
				ppcDcbf((ulong)&(address[j]));
#if defined(CFG_USE_HOS_COUNT)
				/* capture for oversample tracking */
				mfsdram(SDRAM_FCSR, fcsr_reg);
				/* capture RTR for oversample tracking */
				mfsdram(SDRAM_RTSR, rtsr_reg);
				/* this register is sticky, so clear it */
				mtsdram(SDRAM_RTSR, 0);
				/* track hit oversamples - all must match */
				if ((rtsr_reg & SDRAM_RTSR_RHOS) && 
					!(fcsr_reg & SDRAM_FCSR_CMOS_MISSED))
						hos_ct++;
#endif
			}

			sync();
			iobarrier_rw();

			for (j = 0; j < MEMTESTRANGE; j++) {
				if (address[j] != test[i]) {
					ppcDcbf((u32)&(address[j]));
					*hos_count += hos_ct;
					return 0;
				} else {
#if defined(CFG_USE_HOS_COUNT)
					/* capture for oversample tracking */
					mfsdram(SDRAM_FCSR, fcsr_reg);
					/* capture RTR for oversample tracking */
					mfsdram(SDRAM_RTSR, rtsr_reg);
					/* this register is sticky, so clear it */
					mtsdram(SDRAM_RTSR, 0);
					/* track hit oversamples - all must match */
					if ((rtsr_reg & SDRAM_RTSR_RHOS) && 
						!(fcsr_reg & SDRAM_FCSR_CMOS_MISSED))
							hos_ct++;
#endif
					if (ecc_mode) 
						if (!ecc_check_status_reg()) {
							*hos_count += hos_ct;
							return 0;
						}
					
				}
				ppcDcbf((u32)&(address[j]));
			} /* for (j = 0; j < MEMTESTRANGE; j++) */
			sync();
			iobarrier_rw();

			/* advance the memory address */
			address = &address[j+1];			
		} /* for (i=0; i<NUMTESTPATTERNS; i++) */
	} /* for (l=0; l<NUMLOOPS; l++) */
	
	*hos_count += hos_ct;
	return 1;
}
#endif 
/*
 * program_DQS_calibration
 *
 * Autocalibration
 *
 * "Method A" is defined as a full scan of ranges.
 * The "Method B" calibration is defined a restricted scan based on 
 * emphirical data collected from "Method A" scans. The more data 
 * collected from full scans, the more restricted the MethodB scan 
 * can become (smaller scan ranges equate to a faster calibration time).
 *
 *  ARRAY [Entire DQS Range] DQS_Valid_Window ;    initialized to all zeros
 *
 *  for (i = 0; i < Entire/Restricted DQS Range; i++) {       RQDC.RQF

 *  ARRAY [Entire FDBK Range] FDBK_Valid_Window;   initialized to all zeros
 *  MEMWRITE(addr, expected_data);
 *  for (i = 0; i < Entire/Restricted DQS Range; i++) {       RQDC.RQFD
 *      for (j = 0; j < Entire/Restricted FDBK Range; j++) {  RFDC.RFFD
 *         MEMREAD(addr, actual_data);
 *		if !(FCSR missed oversample) && !(RTSR missed oversample)
 *			hit_oversample_counter++;
 *         MEMWRITE(addr, actual_data);
 *		if !(FCSR missed oversample) && !(RTSR missed oversample)
 *			hit_oversample_counter++;
 *         if (actual_data == expected_data) {
 *             	DQS_Valid_Window[i] = 1;            RQDC.RQFD
 *	   }

 *      }
 *  }
 *
 * Stack space can be a problem this early in the init sequence. Be wary of 
 * nested function calls.
 *
 */
static u32 program_DQS_calibration(struct ddrautocal *cal)
{
	u32 rqdc_reg;
	u32 rqfd_index,
		rqfd_end,
		range_index;
	u32 rqfd_range[SDRAM_RQDC_RQFD_MAX];
	u32 rffd_highest = 0;
	u32 min_filter;
	u32 window_count,
		max_window_count,
		win_start,
		win_end,
		best_mid;
	u32 temp; 
	u8 loopi = 0;
	struct ddr_rffd rffd_set;
#if !defined(DEBUG)
	char slash[] = "\\|/-\\|/-";
#else
	char slash[] = "..........";
#endif
	/* RFFD vars */
	u32 first_pass,
		first_fail,
		mid_point,
		index;
	u32 bxcr_num, bxcf;
	u32 rfdc_reg;
	u32 best_start,
		best_end,
		best_hos,
		best_mos;
	int bPass, 
		bNewBestFound;
	int test_result;
	u32 hos_ct;
	u32* membase;
#if defined(CFG_DDR_EXTRA_MEMTEST) || defined(CFG_DDR_EXTRA_MEMTEST2)
	u32 rank_size;
	u32 bank;
	u32 bank_size;
	u32 i;
#endif

	/* init */
	memset(&rffd_set, 0, sizeof(struct ddr_rffd));
	win_start = win_end = best_mid = 0;
	
	/*
	 * Program RDCC register
	 * Read sample cycle auto-update enable
	 */
	mtsdram(SDRAM_RDCC,
		ddr_rdss_opt(SDRAM_RDCC_RDSS_T1) | SDRAM_RDCC_RSAE_ENABLE);

#ifdef DEBUG
	mfsdram(SDRAM_RDCC, temp);
	debug("SDRAM_RDCC=0x%x\n", temp);
	mfsdram(SDRAM_RTSR, temp);
	debug("SDRAM_RTSR=0x%x\n", temp);
	mfsdram(SDRAM_FCSR, temp);
	debug("SDRAM_FCSR=0x%x\n", temp);
#endif

	/*
	 * Program RQDC register
	 * Internal DQS delay mechanism enable
	 */
	mtsdram(SDRAM_RQDC,
		SDRAM_RQDC_RQDE_ENABLE | SDRAM_RQDC_RQFD_ENCODE(0x38));

#ifdef DEBUG
	mfsdram(SDRAM_RQDC, temp);
	debug("SDRAM_RQDC=0x%x\n", temp);
#endif

	/*
	 * Program RFDC register
	 * Set Feedback Fractional Oversample
	 * Auto-detect read sample cycle enable
	 */
	#if defined(CONFIG_460SX)
		mtsdram(SDRAM_RFDC, SDRAM_RFDC_ARSE_ENABLE |
                        SDRAM_RFDC_RFOS_ENCODE(0) | SDRAM_RFDC_RFFD_ENCODE(0x38));
	#else
		mtsdram(SDRAM_RFDC, SDRAM_RFDC_ARSE_ENABLE |
			SDRAM_RFDC_RFOS_ENCODE(0x3f) | SDRAM_RFDC_RFFD_ENCODE(0x38));
	#endif
#ifdef DEBUG
	mfsdram(SDRAM_RFDC, temp);
	debug("SDRAM_RFDC=0x%x\n", temp);
#endif

	putc(' ');

	/* initialize for loop */
	rqdc_reg = 0;
	memset(rqfd_range, 0, sizeof(u32) * SDRAM_RQDC_RQFD_MAX);
	/* Get platform or default ranges start and end */
	rqfd_index = ddr_rqfd_start_opt(0);
#if defined(CONFIG_DDR_RQDC_FIXED)
	rqfd_end = ddr_rqfd_end_opt(0);
#else
	rqfd_end = ddr_rqfd_end_opt(SDRAM_RQDC_RQFD_MAX);
#endif
	rffd_set.start = ddr_rffd_start_opt(0);
	rffd_set.end = ddr_rffd_end_opt(SDRAM_RFDC_RFFD_MAX);
	range_index = 0;

	/* 
	 * Identify the RQDC[RQFD] windows. 
	 * Scan the RQFD range provided running an RFFD scan for each
	 * setting. Once the best RQFD[RFFD] window is found, find the mid-point
	 * and save it to the RQFD array.
	 */
#if !defined(CONFIG_DDR_RQDC_FIXED)
	for (; rqfd_index <= rqfd_end; rqfd_index++) {
		mfsdram(SDRAM_RQDC, rqdc_reg);
		rqdc_reg &= ~(SDRAM_RQDC_RQFD_MASK);
		mtsdram(SDRAM_RQDC, 
			rqdc_reg | SDRAM_RQDC_RQFD_ENCODE(rqfd_index));
#if !defined(DEBUG)
		putc('\b');
#endif
		putc(slash[loopi++ % 8]);

		/* check for best RFFD window to mark RQFD range */
		first_pass = 0;
		first_fail = 0;
		hos_ct = 0;
		mid_point = 0;
		best_start = 0;
		best_hos = 0;
		best_mos = 0;
		best_end = 0;
		bPass = 0;
		bNewBestFound = 0;
		test_result = 0;

		for (index = rffd_set.start;index <= rffd_set.end; index++) {
			mfsdram(SDRAM_RFDC, rfdc_reg);
			rfdc_reg &= ~(SDRAM_RFDC_RFFD_MASK);
			mtsdram(SDRAM_RFDC, 
				rfdc_reg | SDRAM_RFDC_RFFD_ENCODE(index));

			for (bxcr_num = 0; bxcr_num < MAXBXCF; bxcr_num++) {
				mfsdram(SDRAM_MB0CF + (bxcr_num<<2), bxcf);
	
				/* Banks enabled */
				if (bxcf & SDRAM_BXCF_M_BE_MASK) {
					/* Bank is enabled */
				#if defined(CFG_DDR_EXTRA_MEMTEST)
					membase = get_membase(bxcr_num);
					/* FIXME: Only can do memory test in maximum size of CONFIG_MAX_MEM_MAPPED */
					if (!is_able_to_test(membase)) {
						debug("Out of loop, rank %d with membase = 0x%08x\n", bxcr_num, membase);
						break;
					}
                        		rank_size = get_memsize(bxcr_num);
		                        bank = get_bank_mode(bxcr_num);
                		        bank_size = rank_size/bank;
					debug("Rank%d size 0x%08x\n",bxcr_num,  rank_size << 20);
					debug("Bank size: 0x%08x\n", bank_size << 20);
		                        for(i=0 ; i < bank ; i++){
						debug("Iteration %d \n", i);
						/* Do test in the beginning, middle and the end of each bank */
						/* Beginning */
						#if defined(CFG_BEGIN_BANK_TEST)
						if (!is_able_to_test(membase))
							break;
						debug("Memtest at the beginning of the bank: membase = 0x%08x\n", membase);
                        	        	if (!short_mem_test(membase, &hos_ct)){
                                	       		debug("Short test at 0x%08x, rank(%d), bank(%d) %d \n",membase,rank_size,bank,i);
	                                        	debug("  => Failed.\n");
        	                                	test_result += 1;
							break;
                                		}
						#endif
						/* Middle */
						membase += (bank_size << 17); 
						#if defined(CFG_MIDLLE_BANK_TEST)
						if (!is_able_to_test(membase))
                                                        break;
						debug("Memtest at the middle of the bank: membase = 0x%08x\n", membase);
                        	        	if (!short_mem_test(membase, &hos_ct)){
                                	       		debug("Short test at 0x%08x, rank(%d), bank(%d) %d \n",membase,rank_size,bank,i);
	                                        	debug("  => Failed.\n");
        	                                	test_result += 1;
							break;
                                		}
						#endif
						/* End */
						membase += (bank_size << 17);
	                                        membase -= (((sizeof(u32) * NUMTESTPATTERNS) *
                                                MEMTESTRANGE * NUMLOOPS) + 2);
						#if defined(CFG_END_BANK_TEST)
						debug("Memtest at the end of the bank: membase = 0x%08x\n", membase);
						if (!is_able_to_test(membase))
                                                        break;
                        	        	if (!short_mem_test(membase, &hos_ct)){
                                	       		debug("Short test at 0x%08x, rank(%d), bank(%d) %d \n",membase,rank_size,bank,i);
	                                        	debug("  => Failed.\n");
        	                                	test_result += 1;
							break;
                                		}
						#endif
	                                	membase += (((sizeof(u32) * NUMTESTPATTERNS) *
                                                MEMTESTRANGE * NUMLOOPS) + 2);
                        		}	

				#else
					membase = get_membase(bxcr_num);
					if (!is_able_to_test(membase))
                                        	break;
					temp = get_memsize(bxcr_num);
					/* memory test at the start of the bank */
					if (!short_mem_test(membase, &hos_ct)) 
						test_result += 1;
					
#if 0 /* extra memory testing */
					membase += (temp << 17);
					/* memory test near the mid of the bank */
					if (!is_able_to_test(membase))
						break;
					if (!short_mem_test(membase, &hos_ct))
						test_result += 1;
					membase += (temp << 17);
					membase -= (((sizeof(u32) * NUMTESTPATTERNS) *
						MEMTESTRANGE * NUMLOOPS) + 2);
					/* memory test near the end of the bank */
					if (!is_able_to_test(membase))
						break;
					if (!short_mem_test(membase, &hos_ct))
						test_result += 1;
#endif
				#endif
				} /* if bank enabled */
			} /* for bxcr_num */

			/* If this value passed, check for RFFD window start */
			if (test_result == 0) {
				if (!bPass) {
					/* first pass found, mark start */
					bPass = 1;
					first_pass = index;
					/* record rdcc */
					mfsdram(SDRAM_RDCC, rffd_set.rdcc);
				}
			} else { /* test_result shows failure */
				if (bPass) {
					/* first fail, marks window end */
					first_fail = index;
					mid_point = first_fail - first_pass; 
	
					/* avoid a div-by-0 */
					if (mid_point >= 2)
						mid_point = mid_point / 2;
				}
			}
	
			/* Did we find the end of window? */
			if (first_fail) {
				/* Compare to saved "best" */
				if (hos_ct > best_hos)
					bNewBestFound = 1;
				if ((hos_ct == best_hos) &&
					((first_fail - first_pass) > 
						(best_end - best_start)))
						bNewBestFound = 1;
	
				if (bNewBestFound) {
					best_start = first_pass;
					best_end = first_fail;
					best_hos = hos_ct;	
					bNewBestFound = 0;
				}
	
				/* still looping, reset for next window */
				first_pass = 0;
				first_fail = 0;
				hos_ct = 0;
				bPass = 0;
			} /* first_fail != 0 */
			
			test_result = 0;
		} /* RFDC[RFFD] for loop */	
	
		if (bPass && !first_fail) {
	
			/* the window hit the RFDC_end */
			debug("RFFD end range hit on window\n");
			first_fail = index;
			
			/* Compare to saved "best" */
			if (hos_ct > best_hos)
				bNewBestFound = 1;
			if ((hos_ct == best_hos) && 
				(first_fail - first_pass) > (best_end - best_start))
				bNewBestFound = 1;
	
			if (bNewBestFound) {
				best_start = first_pass;
				best_end = first_fail;
				best_hos = hos_ct;
			}
		} /* if bPass && !first_fail */
	
		if (best_start && best_end) {
			if ((best_end - best_start) >= 2)
				mid_point = (best_end - best_start) / 2;
			else mid_point = (best_end - best_start);
			mid_point = best_start + mid_point;
			rffd_set.size = (best_end - best_start);
			rffd_set.hos_ct = best_hos;
		}
		
		rqfd_range[range_index] = mid_point;
		/* update highest found, if necessary */
		if (rqfd_range[range_index] > rffd_highest)
			rffd_highest = rqfd_range[range_index];
		range_index++;
	} /* rqfd scan loop */
	
	/* make certain our scanning found at least one */
	if (!rffd_highest)
		return 0;

	/* use highest to determine our filter */
	min_filter = rffd_highest / 4;	

	/* 
	 * Find center of the largest collection of values that meet
	 * the minimum requirements
	 * 
	 * Plotting all of the hits (where x-axis is RQDC mids and 
	 * y-axis is RFFD mids) should create a bell-type curve. The 
	 * optimal range is near the top of that curve so this attempts 
	 * to filter out the edge/lower values in that curve.
	 *	
	 * The remaining ranges are searched for the largest consecutive 
	 * collection and the mid-point thereof is the optimal setting.
	 */
	rqfd_index = ddr_rqfd_start_opt(0);
	rqfd_end = ddr_rqfd_end_opt(SDRAM_RQDC_RQFD_MAX);
	range_index = window_count = max_window_count = 0;
	for (; rqfd_index <= rqfd_end; rqfd_index++, range_index++) {
		/* test if value meets filter requirement */
		if (rqfd_range[range_index] >= min_filter) {
			/* value is in range */
			if (window_count++) 
				win_end = rqfd_index;
			else /* start of a window */
				win_end = win_start = rqfd_index;
				
			/* track the largest in our filtered area */
			if (window_count > max_window_count)
				max_window_count = window_count;

		} else { /* out of filter range */

			/* check for best and reset for next window */
			if (window_count) {
				if (window_count >= max_window_count) {
					temp = win_end - win_start;
					if (temp >= 2)
						best_mid = win_start + 
							(temp / 2);
					else best_mid = win_start + temp;
				}
				window_count = 0;
			} 
			
		}
	} /* rqfd filter loop */
	
	/* failed to find a window for these settings */
	if (!max_window_count) {
		debug("RQFD scan did not find a window\n");
		return 0;
	}

	if (!(best_mid) || (window_count > max_window_count)) {
		temp = win_end - win_start;
		if (temp >= 2)
			best_mid = win_start + (temp / 2);
		else
			best_mid = win_start + temp;
		max_window_count = window_count;
	}

	putc('\b');

	debug("\n");

#else /* CONFIG_DDR_RQDC_FIXED */
	best_mid = CONFIG_DDR_RQDC_FIXED;
	max_window_count = 1; /* Fake value */
#endif /* CONFIG_DDR_RQDC_FIXED */

	/* Program RQDC[RQFD] mid-point based on results */
	debug("Selected RQFD mid-point: 0x%08x\n", best_mid);
	mfsdram(SDRAM_RQDC, rqdc_reg);
        rqdc_reg &= ~(SDRAM_RQDC_RQFD_MASK);
	mtsdram(SDRAM_RQDC, (rqdc_reg & ~SDRAM_RQDC_RQFD_MASK) |
				SDRAM_RQDC_RQFD_ENCODE(best_mid));
	
	/* Find best RFDC[RFFD] window for selected RQFD mid-point */
	first_pass = 0;
	first_fail = 0;
	hos_ct = 0;
	mid_point = 0;
	best_start = 0;
	best_hos = 0;
	best_mos = 0;
	best_end = 0;
	bPass = 0;
	bNewBestFound = 0;
	test_result = 0;
	for (index = rffd_set.start;index <= rffd_set.end; index++) {
		mfsdram(SDRAM_RFDC, rfdc_reg);
		rfdc_reg &= ~(SDRAM_RFDC_RFFD_MASK);
		mtsdram(SDRAM_RFDC, rfdc_reg | SDRAM_RFDC_RFFD_ENCODE(index));

		for (bxcr_num = 0; bxcr_num < MAXBXCF; bxcr_num++) {
			mfsdram(SDRAM_MB0CF + (bxcr_num<<2), bxcf);

			/* Banks enabled */
			if (bxcf & SDRAM_BXCF_M_BE_MASK) {
				#if defined(CFG_DDR_EXTRA_MEMTEST2)
					membase = get_membase(bxcr_num);
					/* FIXME: Only can do memory test in maximum size of CONFIG_MAX_MEM_MAPPED */
					if (!is_able_to_test(membase)) {
						debug("Out of loop, rank %d with membase = 0x%08x\n", bxcr_num, membase);
						break;
					}
                        		rank_size = get_memsize(bxcr_num);
		                        bank = get_bank_mode(bxcr_num);
                		        bank_size = rank_size/bank;
					debug("Rank%d size 0x%08x\n",bxcr_num,  rank_size << 20);
					debug("Bank size: 0x%08x\n", bank_size << 20);
		                        for(i=0 ; i < bank ; i++){
						debug("Iteration %d \n", i);
						/* Do test in the beginning, middle and the end of each bank */
						/* Beginning */
						#if defined(CFG_BEGIN_BANK_TEST)
						if (!is_able_to_test(membase))
							break;
						debug("Memtest at the beginning of the bank: membase = 0x%08x\n", membase);
                        	        	if (!short_mem_test(membase, &hos_ct)){
                                	       		debug("Short test at 0x%08x, rank(%d), bank(%d) %d \n",membase,rank_size,bank,i);
	                                        	debug("  => Failed.\n");
        	                                	test_result += 1;
							break;
                                		}
						#endif
						/* Middle */
						membase += (bank_size << 17); 
						#if defined(CFG_MIDLLE_BANK_TEST)
						if (!is_able_to_test(membase))
                                                        break;
						debug("Memtest at the middle of the bank: membase = 0x%08x\n", membase);
                        	        	if (!short_mem_test(membase, &hos_ct)){
                                	       		debug("Short test at 0x%08x, rank(%d), bank(%d) %d \n",membase,rank_size,bank,i);
	                                        	debug("  => Failed.\n");
        	                                	test_result += 1;
							break;
                                		}
						#endif
						/* End */
						membase += (bank_size << 17);
	                                        membase -= (((sizeof(u32) * NUMTESTPATTERNS) *
                                                MEMTESTRANGE * NUMLOOPS) + 2);
						#if defined(CFG_END_BANK_TEST)
						debug("Memtest at the end of the bank: membase = 0x%08x\n", membase);
						if (!is_able_to_test(membase))
                                                        break;
                        	        	if (!short_mem_test(membase, &hos_ct)){
                                	       		debug("Short test at 0x%08x, rank(%d), bank(%d) %d \n",membase,rank_size,bank,i);
	                                        	debug("  => Failed.\n");
        	                                	test_result += 1;
							break;
                                		}
						#endif
	                                	membase += (((sizeof(u32) * NUMTESTPATTERNS) *
                                                MEMTESTRANGE * NUMLOOPS) + 2);
                        		}	

				#else/* Bank is enabled */
				membase = get_membase(bxcr_num);
				if (!is_able_to_test(membase))
					break;
				temp = get_memsize(bxcr_num);
				if (!short_mem_test(membase, &hos_ct)) 
					test_result += 1;
#if 0 /* extra memory testing */
					membase += (temp << 17);
					if (!is_able_to_test(membase))
						break;
					if (!short_mem_test(membase, &hos_ct))
						test_result += 1;
					membase += (temp << 17);
					membase -= (((sizeof(u32) * NUMTESTPATTERNS) *
						MEMTESTRANGE * NUMLOOPS) + 2);
					if (!is_able_to_test(membase))
						break;
					if (!short_mem_test(membase, &hos_ct))
						test_result += 1;
#endif
				#endif
			} /* if bank enabled */
		} /* for bxcr_num */

		/* If this value passed, check for RFFD window start */
		if (test_result == 0) {
			if (!bPass) {
				/* first pass found, mark start */
				bPass = 1;
				first_pass = index;
				/* record rdcc */
				mfsdram(SDRAM_RDCC, rffd_set.rdcc);
			}
		} else { /* test_result shows one or more failures */
			if (bPass) {
				/* first fail, marks window end */
				first_fail = index;
				mid_point = first_fail - first_pass; 

				/* avoid a div-by-0 */
				if (mid_point >= 2)
					mid_point = mid_point / 2;
			}
		}

		/* Did we find the end of window? */
		if (first_fail) {
			/* Compare to saved "best" */
			if (hos_ct > best_hos)
				bNewBestFound = 1;
			if ((hos_ct == best_hos) &&
				((first_fail - first_pass) > 
					(best_end - best_start)))
					bNewBestFound = 1;

			if (bNewBestFound) {
				best_start = first_pass;
				best_end = first_fail;
				best_hos = hos_ct;
				bNewBestFound = 0;
			}

			/* still looping, reset for next window */
			first_pass = 0;
			first_fail = 0;
			hos_ct = 0;
			bPass = 0;
		} /* first_fail != 0 */
		test_result = 0;
	} /* RFDC for loop */	

	if (bPass && !first_fail) {

		/* the window hit the RFDC_end */
		debug("Best scan RFDC hit end of range\n");
		first_fail = index;

		/* Compare to saved "best" */
			if (hos_ct > best_hos)
				bNewBestFound = 1;
			if ((hos_ct == best_hos) &&
				((first_fail - first_pass) > 
					(best_end - best_start)))
					bNewBestFound = 1;

			if (bNewBestFound) {
				best_start = first_pass;
				best_end = first_fail;
				best_hos = hos_ct;
				bNewBestFound = 0;
			}
	} /* if bPass && !first_fail */

	if (best_start || best_end) {
		if ((best_end - best_start) >= 2)
			mid_point = (best_end - best_start) / 2;
		else mid_point = (best_end - best_start);
		mid_point = best_start + mid_point;
		rffd_set.size = (best_end - best_start);
		rffd_set.hos_ct = best_hos;
	}
	
	cal->rffd_mid = mid_point;

	if (!(cal->rffd_mid)) {
		/* did not find a valid RFFD window */
		debug("No RFDC[RFFD] window found for RQDC[RQFD] = 0x%04x\n",
			best_mid);
		return 0;
	}

	/* Save out the found mid points and windows */
	cal->rffd_size = rffd_set.size;
	cal->rqfd_size = max_window_count;
	cal->rqfd_mid = best_mid;
	cal->rdcc = rffd_set.rdcc;
	cal->hos_ct = rffd_set.hos_ct;

	/* notify top level calibration of returned values */
	return 1;
}

/*
 * Default table for DDR auto-calibration of all
 * possible WRDTR and CLKTR values.
 * Table format is:
 *	 {SDRAM_WRDTR.[WDTR], SDRAM_CLKTR.[CKTR]}
 *
 * Table is terminated with {-1, -1} value pair.
 *
 * Board vendors can specify their own board specific subset of
 * known working {SDRAM_WRDTR.[WDTR], SDRAM_CLKTR.[CKTR]} value
 * pairs via a board defined ddr_scan_option() function.
 */
#if defined(CONFIG_460SX)
	/*
         * 460SX has different layouts of SDRAM_WRDTR and
         * SDRAM_WRDTR if compared with 460EX
         */
struct sdram_timing full_scan_options[] = {
        {16, 16}, {16, 24}, {16, 32}, {16, 40}, {16, 48},
        {24, 16}, {24, 24}, {24, 32}, {24, 40}, {24, 48},
        {32, 16}, {32, 24}, {32, 32}, {32, 40}, {32, 48},
        {40, 16}, {40, 24}, {40, 32}, {40, 40}, {40, 48},
        {48, 16}, {48, 24}, {48, 32}, {48, 40}, {48, 48},
        {-1, -1}
};
#else
struct sdram_timing full_scan_options[] = {
	{3, 0},	{3, 1}, {3, 2}, {3, 3},
	{2, 0}, {2, 1}, {2, 2}, {2, 3},
	{4, 0}, {4, 1}, {4, 2}, {4, 3},
	{1, 0}, {1, 1}, {1, 2}, {1, 3},
	{5, 0}, {5, 1}, {5, 2}, {5, 3},
	{0, 0}, {0, 1}, {0, 2}, {0, 3},
	{6, 0}, {6, 1}, {6, 2}, {6, 3},
	{-1, -1}
};
#endif

/*---------------------------------------------------------------------------+
| DQS_calibration.
+----------------------------------------------------------------------------*/
u32 DQS_autocalibration(void)
{
	u32 wdtr;
	u32 clkp;
	u32 result = 0;
	struct ddrautocal ddrcal;
	struct ddrautocal best_ddrcal;
	ulong rfdc_reg;
	ulong rqdc_reg;
	u32 val;
	int verbose_lvl = 0;
	int bUpdateBest = 0;
	char *str;
#if !defined(DEBUG)
	char slash[] = "\\|/-\\|/-";
#else	
	char slash[] = ".........."; /* debug is likely being logged */
#endif
	int loopi = 0;
	struct sdram_timing *scan_list;

#if defined(DEBUG_PPC4xx_DDR_AUTOCALIBRATION)
	int i;
	char tmp[64];	/* long enough for environment variables */
#endif

	/* init structures */
	memset(&ddrcal, 0, sizeof(ddrcal));
	memset(&best_ddrcal, 0, sizeof(best_ddrcal));

	ddr_scan_option((ulong)full_scan_options);

	scan_list =
	      (struct sdram_timing *)ddr_scan_option((ulong)full_scan_options);

	mfsdram(SDRAM_MCOPT1, val);
	if ((val & SDRAM_MCOPT1_MCHK_CHK_REP) == SDRAM_MCOPT1_MCHK_CHK_REP)
		str = "ECC Auto calibration -";
	else
		str = "Auto calibration -";

	puts(str);

#if defined(DEBUG_PPC4xx_DDR_AUTOCALIBRATION)
	i = getenv_r("autocalib", tmp, sizeof(tmp));
	if (i < 0)
		strcpy(tmp, CONFIG_AUTOCALIB);

	if (strcmp(tmp, "final") == 0) {
		/* display the final autocalibration results only */
		verbose_lvl = 1;
	} else if (strcmp(tmp, "loop") == 0) {
		/* display summary autocalibration info per iteration */
		verbose_lvl = 2;
	} else if (strcmp(tmp, "display") == 0) {
		/* display full debug autocalibration window info. */
		verbose_lvl = 3;
	}
#endif /* (DEBUG_PPC4xx_DDR_AUTOCALIBRATION) */

	while ((scan_list->wrdtr != -1) && (scan_list->clktr != -1)) {
		wdtr = scan_list->wrdtr;
		clkp = scan_list->clktr;

		/* disable controller before update */
		mfsdram(SDRAM_MCOPT2, val);
		mtsdram(SDRAM_MCOPT2,
		(val &
		 ~(SDRAM_MCOPT2_SREN_MASK | SDRAM_MCOPT2_PMEN_MASK |
		   SDRAM_MCOPT2_IPTR_MASK | SDRAM_MCOPT2_XSRP_MASK |
		   SDRAM_MCOPT2_ISIE_MASK))
		| (SDRAM_MCOPT2_SREN_ENTER | SDRAM_MCOPT2_PMEN_DISABLE |
		   SDRAM_MCOPT2_IPTR_IDLE | SDRAM_MCOPT2_XSRP_ALLOW |
		   SDRAM_MCOPT2_ISIE_ENABLE));
		udelay(400);
#if defined(CONFIG_460SX)
		/* 
		 * 460SX has different layouts of SDRAM_WRDTR and 
		 * SDRAM_WRDTR if compared with 460EX
		 */
		__asm__ __volatile__ ("isync"); /* commented because SDRAM is disable now */
		mfsdram(SDRAM_WRDTR, val);
                mtsdram(SDRAM_WRDTR,  (val & (~0x0000003F)) | wdtr);
                __asm__ __volatile__ ("isync"); /* commented because SDRAM is disable now */

                mtsdram(SDRAM_CLKTR, clkp);
#else
		mtsdram(SDRAM_CLKTR, clkp << 30);
		mfsdram(SDRAM_WRDTR, val);
		val &= ~(SDRAM_WRDTR_LLWP_MASK | SDRAM_WRDTR_WTR_MASK);
		mtsdram(SDRAM_WRDTR, (val |
			ddr_wrdtr(SDRAM_WRDTR_LLWP_1_CYC | (wdtr << 25))));
#endif		
		relock_memory_DLL();
#if !defined(DEBUG)
		putc('\b');
#endif
		putc(slash[loopi++ % 8]);

		if (verbose_lvl > 2) {
			printf("\n*** Start WDTR/CLKP loop--------------\n");
			mfsdram(SDRAM_WRDTR, val);
			printf("*** SDRAM_WRDTR set to 0x%08x\n", val);
			mfsdram(SDRAM_CLKTR, val);
			printf("*** SDRAM_CLKTR set to 0x%08x\n", val);

			printf("\n");

			//printf("*** SDRAM_WRDTR[WDTR] = %d\n", wdtr);
			//printf("*** SDRAM_CLKTR[clkp] = %d\n", clkp);
		}

		memset(&ddrcal, 0, sizeof(ddrcal));
		ddrcal.wdtr = wdtr;
		ddrcal.clkp = clkp;

		/*
		 * DQS calibration.
		 */
		/*
		 * program_DQS_calibration_method[A|B]() returns 0 if no
		 * passing RFDC.[RFFD] window is found. Otherwise a non-zero 
		 * value is returned.
		 * The ddrcal structure contains the settings found.
		 */

		/*
		 * Call PPC4xx SDRAM DDR autocalibration (methodA).
		 *
		 * For faster calibration, full scan statistics should be 
		 * gathered during the prototype or manufacturing stage to 
		 * determine typical RQFD & RFFD ranges and a subset of valid 
		 * WDTR & CLKP combinations for the platform. Thermal testing
		 * and varying configurations should be used during this data 
		 * collection stage.
		 * 
		 * These ranges and subsets are *implementation* specific. 
		 * Without the use of external measuring equipment, there 
		 * is no shortcut to this process.
		 *	
		 * These identified typical ranges can then be used to 
		 * improve boot time by overriding the default full scan 
		 * ranges. Override functions are defined at the top of this 
		 * file.
		 *
		 * Please see include/configs/kilauea.h and 
		 * board/amcc/kilauea/kilauea.c for an example for a board 
		 * specific implementation.
		 */
		result = program_DQS_calibration(&ddrcal);
	
		putc('\b');
		
		sync();

		/*
		 * Clear potential errors resulting from auto-calibration.
		 * If not done, then we could get an interrupt later on when
		 * exceptions are enabled.
		 */
		set_mcsr(get_mcsr());

		val = ddrcal.rdcc;	/* RDCC from the best passing window */

		udelay(100);

		if ((verbose_lvl > 1) && result) {
			char *tstr;
			switch ((val >> 30)) {
			case 0:
				if (result != 0)
					tstr = "T1";
				else
					tstr = "N/A";
				break;
			case 1:
				tstr = "T2";
				break;
			case 2:
				tstr = "T3";
				break;
			case 3:
				tstr = "T4";
				break;
			default:
				tstr = "unknown";
				break;
			}
			printf("*************************************\n");
			printf("** WRDTR[WDTR] = 0x%04x\n", wdtr);
			printf("** CLKTR[CLKP] = 0x%04x\n", clkp);
			printf("** RFDC[RFFD] = 0x%04x\n", ddrcal.rffd_mid);
			printf("** RQDC[RQFD] = 0x%04x\n", ddrcal.rqfd_mid);
			if ((ddrcal.rffd_size / 2) > ddrcal.rffd_mid) 
				val = 0;
			else 
				val = ddrcal.rffd_mid - (ddrcal.rffd_size / 2);
			printf("** RFFD window: 0x%04x <-> ", val);
				
			printf(" 0x%04x (%d)\n",
				ddrcal.rffd_mid + (ddrcal.rffd_size / 2),
				ddrcal.rffd_size);

			if ((ddrcal.rqfd_size / 2) > ddrcal.rqfd_mid)
				val = 0;
			else
				val = ddrcal.rqfd_mid - (ddrcal.rqfd_size / 2);
			printf("** RQFD window: 0x%04x <-> ", val);
			printf("0x%04x (%d)\n",
				ddrcal.rqfd_mid + (ddrcal.rqfd_size / 2),
				ddrcal.rqfd_size);
			printf("** HOS count = %d\n", ddrcal.hos_ct);
			printf("** RDCC[RDSS] = %s\n", tstr);
			printf("*************************************\n");
		}


		/* 
		 * Save off the best combination.
		 * "Best" is determined by the following priority:
		 *	1) Highest HOS count
		 *	2) largest RFFD window
		 * 	3) largest RQFD window
		 *	4) preference to RDCC[RDSS] = Tx+ for most layouts
		 */

		if (ddrcal.hos_ct > best_ddrcal.hos_ct)
			bUpdateBest = 1;

		if ((ddrcal.hos_ct == best_ddrcal.hos_ct) &&
			(ddrcal.rffd_size > best_ddrcal.rffd_size))
			bUpdateBest = 1;
		else if ((ddrcal.hos_ct == best_ddrcal.hos_ct) && 
			(ddrcal.rffd_size == best_ddrcal.rffd_size)) {

			/* secondary and tertiary "best" checks */
			if (ddrcal.rqfd_size > best_ddrcal.rqfd_size)
				bUpdateBest = 1;

			if (ddrcal.rqfd_size == best_ddrcal.rqfd_size)
				if (ddrcal.rdcc > best_ddrcal.rdcc)
					bUpdateBest = 1;
		}


		if (bUpdateBest) {
			best_ddrcal.rffd_size = ddrcal.rffd_size;
			best_ddrcal.rqfd_size = ddrcal.rqfd_size;
			best_ddrcal.rffd_mid = ddrcal.rffd_mid;
			best_ddrcal.rqfd_mid = ddrcal.rqfd_mid;
			best_ddrcal.rdcc = ddrcal.rdcc;
			best_ddrcal.wdtr = ddrcal.wdtr;
			best_ddrcal.clkp = ddrcal.clkp;
			best_ddrcal.hos_ct = ddrcal.hos_ct;
			bUpdateBest = 0;
		}

		if (verbose_lvl > 2) 
			printf("*** End WDTR/CLKP loop--------------\n\n");

		/* Experimental code */
	#if defined(CONFIG_RFFD_GOOD)
		/* FIXME: Early exit when we found a wide RFFD window */
		if (best_ddrcal.rffd_size >= CONFIG_RFFD_GOOD)
			break;
	#endif
		scan_list++;
	} /* while ((scan_list->wrdtr != -1) && (scan_list->clktr != -1)) */

	if (best_ddrcal.rffd_size && best_ddrcal.rqfd_size) {
		if (verbose_lvl > 0) {
			printf("\n*** -- Final Calibration Results --\n");
			printf("** WRDTR[WDTR] = 0x%04x\n", best_ddrcal.wdtr);
			printf("** CLKTR[CLKP] = 0x%04x\n", best_ddrcal.clkp);
			printf("** RFDC[RFFD] = 0x%04x\n", 
				best_ddrcal.rffd_mid);
			printf("** RQDC[RQFD] = 0x%04x\n", 
				best_ddrcal.rqfd_mid);
			if ((best_ddrcal.rffd_size / 2) > best_ddrcal.rffd_mid)
				val = 0;
			else 
				val = best_ddrcal.rffd_mid - 
					(best_ddrcal.rffd_size / 2);
			printf("** RFFD window: 0x%04x <-> ", val);
				
			printf(" 0x%04x (%d)\n",
				best_ddrcal.rffd_mid + 
					(best_ddrcal.rffd_size / 2),
					best_ddrcal.rffd_size);

	if ((best_ddrcal.rqfd_size / 2) > best_ddrcal.rqfd_mid)
				val = 0;
			else
				val = best_ddrcal.rqfd_mid - 
					(best_ddrcal.rqfd_size / 2);
			printf("** RQFD window: 0x%04x <-> ", val);
			printf("0x%04x (%d)\n",
				best_ddrcal.rqfd_mid + 
					(best_ddrcal.rqfd_size / 2),
					best_ddrcal.rqfd_size);

			printf("** Hit Oversample count = %d\n", 
				best_ddrcal.hos_ct);
			printf("** RDCC = 0x%08x\n", best_ddrcal.rdcc);
			printf("*** -------------------------------\n");
			printf("\n");
		}

		/*
		 * if got best passing result window, then lock in the
		 * best CLKTR, WRDTR, RQFD, and RFFD values
		 */
		#if defined(CONFIG_460SX)
		/*
         	 * 460SX has different layouts of SDRAM_WRDTR and
	         * SDRAM_WRDTR if compared with 460EX
        	 */
			mfsdram(SDRAM_WRDTR, val);
			/* TODO: define DQSFD mask for SDRAM_WRDTR */
			mtsdram(SDRAM_WRDTR, (val & (~0x0000003F)) | best_ddrcal.wdtr); 
			mtsdram(SDRAM_CLKTR, best_ddrcal.clkp);
		#else
			mfsdram(SDRAM_WRDTR, val);
			mtsdram(SDRAM_WRDTR, (val &
			    ~(SDRAM_WRDTR_LLWP_MASK | SDRAM_WRDTR_WTR_MASK)) |
			    ddr_wrdtr(SDRAM_WRDTR_LLWP_1_CYC |
					(best_ddrcal.wdtr << 25)));

			mtsdram(SDRAM_CLKTR, best_ddrcal.clkp << 30);
		#endif
		relock_memory_DLL();

		mfsdram(SDRAM_RQDC, rqdc_reg);
		rqdc_reg &= ~(SDRAM_RQDC_RQFD_MASK);
		mtsdram(SDRAM_RQDC, rqdc_reg |
				SDRAM_RQDC_RQFD_ENCODE(best_ddrcal.rqfd_mid));

		mfsdram(SDRAM_RQDC, rqdc_reg);
		debug("*** set best result: read value SDRAM_RQDC 0x%08x\n",
				rqdc_reg);

		mfsdram(SDRAM_RFDC, rfdc_reg);
		rfdc_reg &= ~(SDRAM_RFDC_RFFD_MASK);
		mtsdram(SDRAM_RFDC, rfdc_reg |
				SDRAM_RFDC_RFFD_ENCODE(best_ddrcal.rffd_mid));

		mfsdram(SDRAM_RFDC, rfdc_reg);
		debug("*** set best result: read value SDRAM_RFDC 0x%08x\n",
				rfdc_reg);
		mfsdram(SDRAM_RDCC, val);
		debug("*** set best result: read value SDRAM_RDCC 0x%08x\n", 
				val);
	} else {
		/*
		 * no valid windows were found
		 */
		printf("DQS memory calibration windows can not be determined.\n ");
		printf("U-Boot cannot continue. Please consider defining\n");
		printf("fallback values and utilizing them to avoid this\n");
		printf("situation.\n");
		ppc4xx_ibm_ddr2_register_dump();
		spd_ddr_init_hang();
	}

	sync();

	/* Clear potential errors before continuing. */
	set_mcsr(get_mcsr());
	udelay(100);

	blank_string(strlen(str));

	return 0;
}
#else /* defined(CONFIG_NAND_U_BOOT) || defined(CONFIG_NAND_SPL) */
u32 DQS_autocalibration(void)
{
	return 0;
}
#endif /* !defined(CONFIG_NAND_U_BOOT) && !defined(CONFIG_NAND_SPL) */
#endif /* defined(CONFIG_PPC4xx_DDR_AUTOCALIBRATION) */
