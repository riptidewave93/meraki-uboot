/*
 * (C) Copyright 2000-2007
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
 */

#include <common.h>
#include <watchdog.h>
#include <ppc4xx_enet.h>
#include <asm/processor.h>
#include <asm/gpio.h>
#include <ppc4xx.h>

#include <asm/apm_ipp_csr.h>
#include <asm/io.h>
#include <asm/4xx_pcie.h>

#if defined(CONFIG_405GP)  || defined(CONFIG_405EP)
DECLARE_GLOBAL_DATA_PTR;
#endif

#ifndef CONFIG_SYS_PLL_RECONFIG
#define CONFIG_SYS_PLL_RECONFIG	0
#endif

extern int apm86xxx_write_scu_reg(u32 offset, u32 val);
extern int apm86xxx_read_scu_reg(u32 offset, u32 *val);

void reconfigure_pll(u32 new_cpu_freq)
{
#if defined(RECONFIG_PLL1_500) || defined(RECONFIG_PLL1_1000)
	volatile u32 *pll1_addr;
	volatile u32 pll1_val, data;
	volatile u32 *addr;
	u32 scu_base = (unsigned int) CONFIG_SYS_SCU_BASE;

	pll1_addr = (volatile u32 *) (scu_base + SCU_SOCPLL1_ADDR);
	pll1_val = in_be32(pll1_addr);

	//puts("reconfig PLL1\n");
	/* Return Success if PLL already configured */
// 	if (!(pll1_val & PWRDN1_MASK)) 
// 		return 0;

	/* Clear SOCPLL1 PWRDWN */
	pll1_val = (pll1_val | RESET1_MASK) & ~(PWRDN1_MASK | BYPASS1_MASK);
	out_be32(pll1_addr, pll1_val);

	/* Write SOCPLL1 CLKF/CLKOD/CLKR values */
#if defined(RECONFIG_PLL1_500)	//SOC 500
	pll1_val = (pll1_val & ~(CLKR1_MASK | CLKOD1_MASK | CLKF1_MASK)) | 
		(CLKR1_WR(0) | 
		CLKOD1_WR(0) | 
		CLKF1_WR(0x13));
	out_be32(pll1_addr, pll1_val);
#else	//SOC 1000
	pll1_val = (pll1_val & ~(CLKR1_MASK | CLKOD1_MASK | CLKF1_MASK)) | 
		(CLKR1_WR(0) | 
		CLKOD1_WR(0) | 
		CLKF1_WR(0x27));
	out_be32(pll1_addr, pll1_val);
#endif

	/* Write SOCPLLADJ1 BWADJ values - half of CLKF's divider val */
	addr = (volatile u32 *) (scu_base + SCU_SOCPLLADJ1_ADDR);
	data = in_be32(addr);
	data &= ~BWADJ1_MASK;
#if defined(RECONFIG_PLL1_500)	//SOC 500
	data |= 0x09;
#else	//SOC 1000
	data |= 0x13;
#endif
	out_be32(addr, data);

	/* Wait for SCU_PLLDLY.PLLSTARTDELAY time */
	addr = (volatile u32 *) (scu_base + SCU_PLLDLY_ADDR);
	/* Wait for SCU_PLLDLY.PLLSTARTDELAY time */
	/* FIXME: Calculate delay from SCU_PLLDLY.PLLSTARTDELAY */
	data = in_be32(addr);
	data >>= PLLSTARTDELAY_SHIFT;

	/* Calculate delay in us - SYSCLK is 25MHz : Minimum is 5 us*/
	mdelay(1000);

	/* Clear SOCPLL1 RESET */
	out_be32(pll1_addr, pll1_val & ~RESET1_MASK);

	/* Wait for SOCPLLSTAT.LOCK1 or PLLDLY.PLLLOCKDELAY */
	addr = (volatile u32 *) (scu_base + SCU_PLLDLY_ADDR);
	/* Wait for SCU_PLLDLY.PLLSTARTDELAY time */
	/* FIXME: Calculate delay from SCU_PLLDLY.PLLLOCKDELAY */
	data = in_be32(addr);
	data &= PLLLOCKDELAY_MASK;

	/* Calculate delay in us - SYSCLK is 25MHz : Minimum is 5 us*/
	mdelay(1000);

#if defined(RECONFIG_PLL1_500)	//SOC 500 

	apm86xxx_write_scu_reg(SCU_SOCDIV4_ADDR, 0x050A0000);
	mdelay(1000);
#endif	//SOC 500
#endif

#if defined(AXI_RECONFIGURE_125)	//SOC 1000 
	apm86xxx_write_scu_reg(SCU_SOCDIV_ADDR, 0x08060C00);
	mdelay(100);
	//apm86xxx_write_scu_reg(SCU_SOCDIV_ADDR, 0x08080C00);
	//mdelay(700);
 	apm86xxx_write_scu_reg(SCU_SOCDIV_ADDR, 0x08081000);
 	mdelay(100);
#endif	//SOC 1000
}

/*
 * Breath some life into the CPU...
 *
 * Reconfigure PLL if necessary,
 * set up the memory map,
 * initialize a bunch of registers
 */
void
cpu_init_f (void)
{
#if defined(CONFIG_WATCHDOG) || defined(CONFIG_440GX) || defined(CONFIG_460EX)
	u32 val;
#endif

	reconfigure_pll(CONFIG_SYS_PLL_RECONFIG);

#if (defined(CONFIG_405EP) || defined (CONFIG_405EX)) && !defined(CONFIG_SYS_4xx_GPIO_TABLE)
	/*
	 * GPIO0 setup (select GPIO or alternate function)
	 */
#if defined(CONFIG_SYS_GPIO0_OR)
	out32(GPIO0_OR, CONFIG_SYS_GPIO0_OR);		/* set initial state of output pins	*/
#endif
#if defined(CONFIG_SYS_GPIO0_ODR)
	out32(GPIO0_ODR, CONFIG_SYS_GPIO0_ODR);	/* open-drain select			*/
#endif
	out32(GPIO0_OSRH, CONFIG_SYS_GPIO0_OSRH);	/* output select			*/
	out32(GPIO0_OSRL, CONFIG_SYS_GPIO0_OSRL);
	out32(GPIO0_ISR1H, CONFIG_SYS_GPIO0_ISR1H);	/* input select				*/
	out32(GPIO0_ISR1L, CONFIG_SYS_GPIO0_ISR1L);
	out32(GPIO0_TSRH, CONFIG_SYS_GPIO0_TSRH);	/* three-state select			*/
	out32(GPIO0_TSRL, CONFIG_SYS_GPIO0_TSRL);
#if defined(CONFIG_SYS_GPIO0_ISR2H)
	out32(GPIO0_ISR2H, CONFIG_SYS_GPIO0_ISR2H);
	out32(GPIO0_ISR2L, CONFIG_SYS_GPIO0_ISR2L);
#endif
#if defined (CONFIG_SYS_GPIO0_TCR)
	out32(GPIO0_TCR, CONFIG_SYS_GPIO0_TCR);	/* enable output driver for outputs	*/
#endif
#endif /* CONFIG_405EP ... && !CONFIG_SYS_4xx_GPIO_TABLE */

#if defined (CONFIG_405EP)
	/*
	 * Set EMAC noise filter bits
	 */
	mtdcr(CPC0_EPCTL, CPC0_EPRCSR_E0NFE | CPC0_EPRCSR_E1NFE);
#endif /* CONFIG_405EP */

#if defined(CONFIG_SYS_4xx_GPIO_TABLE)
#ifdef CONFIG_MP
	if (CPUID == CONFIG_SYS_MASTER_CPUID)
#endif
	gpio_set_chip_configuration();
#endif /* CONFIG_SYS_4xx_GPIO_TABLE */

	/*
	 * External Bus Controller (EBC) Setup
	 */
#if (defined(CONFIG_SYS_EBC_PB0AP) && defined(CONFIG_SYS_EBC_PB0CR))
#if (defined(CONFIG_405GP) || defined(CONFIG_405CR) || \
     defined(CONFIG_405EP) || defined(CONFIG_405EZ) || \
     defined(CONFIG_405EX) || defined(CONFIG_405))
	/*
	 * Move the next instructions into icache, since these modify the flash
	 * we are running from!
	 */
	asm volatile("	bl	0f"		::: "lr");
	asm volatile("0:	mflr	3"		::: "r3");
	asm volatile("	addi	4, 0, 14"	::: "r4");
	asm volatile("	mtctr	4"		::: "ctr");
	asm volatile("1:	icbt	0, 3");
	asm volatile("	addi	3, 3, 32"	::: "r3");
	asm volatile("	bdnz	1b"		::: "ctr", "cr0");
	asm volatile("	addis	3, 0, 0x0"	::: "r3");
	asm volatile("	ori	3, 3, 0xA000"	::: "r3");
	asm volatile("	mtctr	3"		::: "ctr");
	asm volatile("2:	bdnz	2b"		::: "ctr", "cr0");
#endif

	mtebc(PB0AP, CONFIG_SYS_EBC_PB0AP);
	mtebc(PB0CR, CONFIG_SYS_EBC_PB0CR);
#endif

#if (defined(CONFIG_SYS_EBC_PB1AP) && defined(CONFIG_SYS_EBC_PB1CR) && !(CONFIG_SYS_INIT_DCACHE_CS == 1))
	mtebc(PB1AP, CONFIG_SYS_EBC_PB1AP);
	mtebc(PB1CR, CONFIG_SYS_EBC_PB1CR);
#endif

#if (defined(CONFIG_SYS_EBC_PB2AP) && defined(CONFIG_SYS_EBC_PB2CR) && !(CONFIG_SYS_INIT_DCACHE_CS == 2))
	mtebc(PB2AP, CONFIG_SYS_EBC_PB2AP);
	mtebc(PB2CR, CONFIG_SYS_EBC_PB2CR);
#endif

#if (defined(CONFIG_SYS_EBC_PB3AP) && defined(CONFIG_SYS_EBC_PB3CR) && !(CONFIG_SYS_INIT_DCACHE_CS == 3))
	mtebc(PB3AP, CONFIG_SYS_EBC_PB3AP);
	mtebc(PB3CR, CONFIG_SYS_EBC_PB3CR);
#endif

#if (defined(CONFIG_SYS_EBC_PB4AP) && defined(CONFIG_SYS_EBC_PB4CR) && !(CONFIG_SYS_INIT_DCACHE_CS == 4))
	mtebc(PB4AP, CONFIG_SYS_EBC_PB4AP);
	mtebc(PB4CR, CONFIG_SYS_EBC_PB4CR);
#endif

#if (defined(CONFIG_SYS_EBC_PB5AP) && defined(CONFIG_SYS_EBC_PB5CR) && !(CONFIG_SYS_INIT_DCACHE_CS == 5))
	mtebc(PB5AP, CONFIG_SYS_EBC_PB5AP);
	mtebc(PB5CR, CONFIG_SYS_EBC_PB5CR);
#endif

#if (defined(CONFIG_SYS_EBC_PB6AP) && defined(CONFIG_SYS_EBC_PB6CR) && !(CONFIG_SYS_INIT_DCACHE_CS == 6))
	mtebc(PB6AP, CONFIG_SYS_EBC_PB6AP);
	mtebc(PB6CR, CONFIG_SYS_EBC_PB6CR);
#endif

#if (defined(CONFIG_SYS_EBC_PB7AP) && defined(CONFIG_SYS_EBC_PB7CR) && !(CONFIG_SYS_INIT_DCACHE_CS == 7))
	mtebc(PB7AP, CONFIG_SYS_EBC_PB7AP);
	mtebc(PB7CR, CONFIG_SYS_EBC_PB7CR);
#endif

#if defined (CONFIG_SYS_EBC_CFG)
	mtebc(EBC0_CFG, CONFIG_SYS_EBC_CFG);
#endif

#if defined(CONFIG_WATCHDOG)
	val = mfspr(tcr);
#if defined(CONFIG_440EP) || defined(CONFIG_440GR)
	val |= 0xb8000000;      /* generate system reset after 1.34 seconds */
#elif defined(CONFIG_440EPX)
	val |= 0xb0000000;      /* generate system reset after 1.34 seconds */
#else
	val |= 0xf0000000;      /* generate system reset after 2.684 seconds */
#endif
#if defined(CONFIG_SYS_4xx_RESET_TYPE)
	val &= ~0x30000000;			/* clear WRC bits */
	val |= CONFIG_SYS_4xx_RESET_TYPE << 28;	/* set board specific WRC type */
#endif
	mtspr(tcr, val);

	val = mfspr(tsr);
	val |= 0x80000000;      /* enable watchdog timer */
	mtspr(tsr, val);

	reset_4xx_watchdog();
#endif /* CONFIG_WATCHDOG */

#if defined(CONFIG_440GX)
	/* Take the GX out of compatibility mode
	 * Travis Sawyer, 9 Mar 2004
	 * NOTE: 440gx user manual inconsistency here
	 *       Compatibility mode and Ethernet Clock select are not
	 *       correct in the manual
	 */
	mfsdr(SDR0_MFR, val);
	val &= ~0x10000000;
	mtsdr(SDR0_MFR,val);
#endif /* CONFIG_440GX */

#if defined(CONFIG_460EX)
	/*
	 * Set SDR0_AHB_CFG[A2P_INCR4] (bit 24) and
	 * clear SDR0_AHB_CFG[A2P_PROT2] (bit 25) for a new 460EX errata
	 * regarding concurrent use of AHB USB OTG, USB 2.0 host and SATA
	 */
	mfsdr(SDR0_AHB_CFG, val);
	val |= 0x80;
	val &= ~0x40;
	mtsdr(SDR0_AHB_CFG, val);
	mfsdr(SDR0_USB2HOST_CFG, val);
	val &= ~0xf00;
	val |= 0x400;
	mtsdr(SDR0_USB2HOST_CFG, val);
#endif /* CONFIG_460EX */

#if defined(CONFIG_405EX) || \
    defined(CONFIG_440SP) || defined(CONFIG_440SPE) || \
    defined(CONFIG_460EX) || defined(CONFIG_460GT)  || \
    defined(CONFIG_460SX)
	/*
	 * Set PLB4 arbiter (Segment 0 and 1) to 4 deep pipeline read
	 */
	mtdcr(PLB0_ACR, (mfdcr(PLB0_ACR) & ~PLB0_ACR_RDP_MASK) |
	      PLB0_ACR_RDP_4DEEP);
	mtdcr(PLB1_ACR, (mfdcr(PLB1_ACR) & ~PLB1_ACR_RDP_MASK) |
	      PLB1_ACR_RDP_4DEEP);
#endif /* CONFIG_440SP/SPE || CONFIG_460EX/GT || CONFIG_405EX */
}

/*
 * initialize higher level parts of CPU like time base and timers
 */
int cpu_init_r (void)
{
#if defined(CONFIG_405GP)
	uint pvr = get_pvr();

	/*
	 * Set edge conditioning circuitry on PPC405GPr
	 * for compatibility to existing PPC405GP designs.
	 */
	if ((pvr & 0xfffffff0) == (PVR_405GPR_RB & 0xfffffff0)) {
		mtdcr(CPC0_ECR, 0x60606000);
	}
#endif  /* defined(CONFIG_405GP) */

	return 0;
}

#if defined(CONFIG_PCI) && \
	(defined(CONFIG_440EP) || defined(CONFIG_440EPX) || \
	 defined(CONFIG_440GR) || defined(CONFIG_440GRX))
/*
 * 440EP(x)/GR(x) PCI async/sync clocking restriction:
 *
 * In asynchronous PCI mode, the synchronous PCI clock must meet
 * certain requirements. The following equation describes the
 * relationship that must be maintained between the asynchronous PCI
 * clock and synchronous PCI clock. Select an appropriate PCI:PLB
 * ratio to maintain the relationship:
 *
 * AsyncPCIClk - 1MHz <= SyncPCIclock <= (2 * AsyncPCIClk) - 1MHz
 */
static int ppc4xx_pci_sync_clock_ok(u32 sync, u32 async)
{
	if (((async - 1000000) > sync) || (sync > ((2 * async) - 1000000)))
		return 0;
	else
		return 1;
}

int ppc4xx_pci_sync_clock_config(u32 async)
{
	sys_info_t sys_info;
	u32 sync;
	int div;
	u32 reg;
	u32 spcid_val[] = {
		CPR0_SPCID_SPCIDV0_DIV1, CPR0_SPCID_SPCIDV0_DIV2,
		CPR0_SPCID_SPCIDV0_DIV3, CPR0_SPCID_SPCIDV0_DIV4 };

	get_sys_info(&sys_info);
	sync = sys_info.freqPCI;

	/*
	 * First check if the equation above is met
	 */
	if (!ppc4xx_pci_sync_clock_ok(sync, async)) {
		/*
		 * Reconfigure PCI sync clock to meet the equation.
		 * Start with highest possible PCI sync frequency
		 * (divider 1).
		 */
		for (div = 1; div <= 4; div++) {
			sync = sys_info.freqPLB / div;
			if (ppc4xx_pci_sync_clock_ok(sync, async))
			    break;
		}

		if (div <= 4) {
			mtcpr(CPR0_SPCID, spcid_val[div]);

			mfcpr(CPR0_ICFG, reg);
			reg |= CPR0_ICFG_RLI_MASK;
			mtcpr(CPR0_ICFG, reg);

			/* do chip reset */
			mtspr(SPRN_DBCR0, 0x20000000);
		} else {
			/* Impossible to configure the PCI sync clock */
			return -1;
		}
	}

	return 0;
}
#endif
