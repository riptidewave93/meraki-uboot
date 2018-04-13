/*
 * (C) Copyright 2009 AMCC Applied Micro Circuits Corporation
 * Tai Tri Nguyen, ttnguyen@amcc.com
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

#if 0
#define DEBUG
#endif

#include <common.h>
#include <ppc4xx.h>
#include <asm/io.h>
#include <asm/processor.h>
/*
 * This code can configure DIMM MT9HTF12872AZ-80EH1 0942 1G
 *
 */
#if defined(CONFIG_SYS_PLL_RECONFIG)
#if (CONFIG_SYS_PLL_RECONFIG == 800)
#define CONFIG_SDRAM_400Mbps
#endif

#if (CONFIG_SYS_PLL_RECONFIG == 1000)
#define CONFIG_SDRAM_500Mbps
#endif

#else
#define CONFIG_SDRAM_400Mbps
#endif
/* SDRAM Controller */
#if defined(CONFIG_SDRAM_500Mbps)
#define CFG_SDRAM0_MB0CF                0x00007701
#define CFG_SDRAM0_MB1CF                0x00000000
#define CFG_SDRAM0_MB2CF                0x00000000
#define CFG_SDRAM0_MB3CF                0x00000000
#define CFG_SDRAM0_MCOPT1               0x04322000 /* no ecc */
#define CFG_SDRAM0_MCOPT2               0x08000000
#define CFG_SDRAM0_MODT0                0x01000000
#define CFG_SDRAM0_MODT1                0x00000000
#define CFG_SDRAM0_MODT2                0x00000000
#define CFG_SDRAM0_MODT3                0x00000000
#define CFG_SDRAM0_CODT                 0x00800021
#define CFG_SDRAM0_RTR                  0x07a00000
#define CFG_SDRAM0_INITPLR0             0x99380000
#define CFG_SDRAM0_INITPLR1             0x82100000
#define CFG_SDRAM0_INITPLR2             0x81820000
#define CFG_SDRAM0_INITPLR3             0x81830000
#define CFG_SDRAM0_INITPLR4             0x81810040
#define CFG_SDRAM0_INITPLR5             0xe4000542
#define CFG_SDRAM0_INITPLR6             0x82100400
#define CFG_SDRAM0_INITPLR7             0x90080000
#define CFG_SDRAM0_INITPLR8             0x90080000
#define CFG_SDRAM0_INITPLR9             0x90080000
#define CFG_SDRAM0_INITPLR10            0x90080000
#define CFG_SDRAM0_INITPLR11            0x81800442
#define CFG_SDRAM0_INITPLR12            0x818103c0
#define CFG_SDRAM0_INITPLR13            0x81810040
#define CFG_SDRAM0_INITPLR14            0x00000000
#define CFG_SDRAM0_INITPLR15            0x00000000
#define CFG_SDRAM0_RQDC                 0x8000003f
#define CFG_SDRAM0_RFDC                 0x003f02bc
#define CFG_SDRAM0_RDCC                 0x80000000
#define CFG_SDRAM0_DLCR                 0x0300007b
#define CFG_SDRAM0_CLKTR                0x00000000
#define CFG_SDRAM0_WRDTR                0x86000800
#define CFG_SDRAM0_SDTR1                0x80201000
#define CFG_SDRAM0_SDTR2                0x42104242
#define CFG_SDRAM0_SDTR3                0x0c0f0d20
#define CFG_SDRAM0_MMODE                0x00000642
#define CFG_SDRAM0_MEMODE		0x00000040
#else
#define CFG_SDRAM0_MB0CF                0x00007701
#define CFG_SDRAM0_MB1CF                0x00000000
#define CFG_SDRAM0_MB2CF                0x00000000
#define CFG_SDRAM0_MB3CF                0x00000000
#define CFG_SDRAM0_MCOPT1               0x04322000 /* no ecc */
#define CFG_SDRAM0_MCOPT2               0x08000000
#define CFG_SDRAM0_MODT0                0x01000000
#define CFG_SDRAM0_MODT1                0x00000000
#define CFG_SDRAM0_MODT2                0x00000000
#define CFG_SDRAM0_MODT3                0x00000000
#define CFG_SDRAM0_CODT                 0x00800021
#define CFG_SDRAM0_RTR                  0x06180000
#define CFG_SDRAM0_INITPLR0             0x94380000
#define CFG_SDRAM0_INITPLR1             0x82100000
#define CFG_SDRAM0_INITPLR2             0x81820000
#define CFG_SDRAM0_INITPLR3             0x81830000
#define CFG_SDRAM0_INITPLR4             0x81810040
#define CFG_SDRAM0_INITPLR5             0xe4000532
#define CFG_SDRAM0_INITPLR6             0x82100400
#define CFG_SDRAM0_INITPLR7             0x8d080000
#define CFG_SDRAM0_INITPLR8             0x8d080000
#define CFG_SDRAM0_INITPLR9             0x8d080000
#define CFG_SDRAM0_INITPLR10            0x8d080000
#define CFG_SDRAM0_INITPLR11            0x81800432
#define CFG_SDRAM0_INITPLR12            0x818103c0
#define CFG_SDRAM0_INITPLR13            0x81810040
#define CFG_SDRAM0_INITPLR14            0x00000000
#define CFG_SDRAM0_INITPLR15            0x00000000
#define CFG_SDRAM0_RQDC                 0x8000003f
#define CFG_SDRAM0_RFDC                 0x003f02a2
#define CFG_SDRAM0_RDCC                 0x40000000
#define CFG_SDRAM0_DLCR                 0x0300009b
#define CFG_SDRAM0_CLKTR                0x00000000
#define CFG_SDRAM0_WRDTR                0x86000800
#define CFG_SDRAM0_SDTR1                0x80201000
#define CFG_SDRAM0_SDTR2                0x42104242
#define CFG_SDRAM0_SDTR3                0x0a0d0d1a
#define CFG_SDRAM0_MMODE                0x00000632
#define CFG_SDRAM0_MEMODE		0x00000040
#endif

#define DEBUG

#ifdef DEBUG
#define PPC4xx_IBM_DDR2_DUMP_REGISTER(mnemonic)				\
	do {								\
		u32 data;						\
		mfsdram(SDRAM_##mnemonic, data);			\
		printf("%20s[%02x] = 0x%08X\n",				\
		       "SDRAM_" #mnemonic, SDRAM_##mnemonic, data);	\
	} while (0)
#define PPC4xx_IBM_DDR2_DUMP_DCR(dcr)					\
	do {                                                            \
                printf("%20s[%02x] = 0x%08X\n",                         \
                       "SDRAM_" #dcr, SDRAM_##dcr, mfdcr(SDRAM_##dcr)); \
        } while (0)
#endif

static void wait_init_complete(void)
{
	u32 val;

	do {
		mfsdram(SDRAM_MCSTAT, val);
	} while (!(val & 0x80000000));
}
phys_size_t initdram(int board_type)
{
 /* Set Memory Bank Configuration Registers */
	unsigned long val;
	mtsdram(SDRAM_MB0CF, CFG_SDRAM0_MB0CF);
        /* Set Memory Clock Timing Register */
        mtsdram(SDRAM_CLKTR, CFG_SDRAM0_CLKTR);

        /* Set Refresh Time Register */

        mtsdram(SDRAM_RTR, CFG_SDRAM0_RTR);

        /* Set SDRAM Timing Registers */

        mtsdram(SDRAM_SDTR1, CFG_SDRAM0_SDTR1);
        mtsdram(SDRAM_SDTR2, CFG_SDRAM0_SDTR2);
        mtsdram(SDRAM_SDTR3, CFG_SDRAM0_SDTR3);

        /* Set Mode and Extended Mode Registers */

        mtsdram(SDRAM_MMODE, CFG_SDRAM0_MMODE);
        mtsdram(SDRAM_MEMODE, CFG_SDRAM0_MEMODE);

        /* Set Memory Controller Options 1 Register */

        mtsdram(SDRAM_MCOPT1, CFG_SDRAM0_MCOPT1);

        /* Set Manual Initialization Control Registers */

        mtsdram(SDRAM_INITPLR0, CFG_SDRAM0_INITPLR0);
        mtsdram(SDRAM_INITPLR1, CFG_SDRAM0_INITPLR1);
        mtsdram(SDRAM_INITPLR2, CFG_SDRAM0_INITPLR2);
        mtsdram(SDRAM_INITPLR3, CFG_SDRAM0_INITPLR3);
        mtsdram(SDRAM_INITPLR4, CFG_SDRAM0_INITPLR4);
        mtsdram(SDRAM_INITPLR5, CFG_SDRAM0_INITPLR5);
        mtsdram(SDRAM_INITPLR6, CFG_SDRAM0_INITPLR6);
        mtsdram(SDRAM_INITPLR7, CFG_SDRAM0_INITPLR7);
        mtsdram(SDRAM_INITPLR8, CFG_SDRAM0_INITPLR8);
        mtsdram(SDRAM_INITPLR9, CFG_SDRAM0_INITPLR9);
        mtsdram(SDRAM_INITPLR10, CFG_SDRAM0_INITPLR10);
        mtsdram(SDRAM_INITPLR11, CFG_SDRAM0_INITPLR11);
        mtsdram(SDRAM_INITPLR12, CFG_SDRAM0_INITPLR12);
        mtsdram(SDRAM_INITPLR13, CFG_SDRAM0_INITPLR13);
	
	for(val=0;val<100000;val++);

        /* Set On-Die Termination Registers */

        mtsdram(SDRAM_CODT, CFG_SDRAM0_CODT);
        mtsdram(SDRAM_MODT0, CFG_SDRAM0_MODT0);
        mtsdram(SDRAM_MODT1, CFG_SDRAM0_MODT1);
        /* Set Write Timing Register */

        mtsdram(SDRAM_WRDTR, CFG_SDRAM0_WRDTR);

        /*
         * Start Initialization by SDRAM0_MCOPT2[SREN] = 0 and
         * SDRAM0_MCOPT2[IPTR] = 1
         */

        mtsdram(SDRAM_MCOPT2, (SDRAM_MCOPT2_SREN_EXIT |
                               SDRAM_MCOPT2_IPTR_EXECUTE));
	  /*
         * Poll SDRAM0_MCSTAT[MIC] for assertion to indicate the
         * completion of initialization.
         */

        do {
                mfsdram(SDRAM_MCSTAT, val);
        } while ((val & SDRAM_MCSTAT_MIC_MASK) != SDRAM_MCSTAT_MIC_COMP);

        /* Set Delay Control Registers */
        mtsdram(SDRAM_DLCR, CFG_SDRAM0_DLCR);
        mtsdram(SDRAM_RDCC, CFG_SDRAM0_RDCC);
        mtsdram(SDRAM_RQDC, CFG_SDRAM0_RQDC);
        mtsdram(SDRAM_RFDC, CFG_SDRAM0_RFDC);

        /*
         * Enable Controller by SDRAM0_MCOPT2[DCEN] = 1:
         */

        mfsdram(SDRAM_MCOPT2, val);
        mtsdram(SDRAM_MCOPT2, val | SDRAM_MCOPT2_DCEN_ENABLE);
        return (CONFIG_SYS_MBYTES_SDRAM << 20);
}
