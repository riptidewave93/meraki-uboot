/**
 * APM SoC 86xxx SCU/iPP Interface 
 *
 * Copyright (c) 2010 Applied Micro Circuits Corporation.
 * All rights reserved. Prodyut Hazarika <phazarika@apm.com>.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * This module implements all Common SCU/iPP Functions for APM 86xxx SoC.
 *
 */
#include <config.h>
#include <common.h>
#include <linux/err.h>
#include <asm/byteorder.h>
#include <asm/io.h>
#include <asm/mmu.h>
#include <asm/processor.h>
#include <asm/mp.h>
#include <asm/atomic.h>
#include <asm/errno.h>
#include <asm/apm_scu.h>
#include <asm/apm_ipp_csr.h>
#include <asm/apm_ipp_interface.h>
#include <asm/apm_ipp_imageinfo.h>

DECLARE_GLOBAL_DATA_PTR;

#undef SCU_DEBUG
#if !defined(CONFIG_MEDIA_SPL) && !defined(CONFIG_NAND_SPL)
#ifdef SCU_DEBUG
#define SCU_DBG_PRINT(x, ...)		printf((x), ##__VA_ARGS__)
#else
#define SCU_DBG_PRINT(x, ...)      
#endif
#else
#define SCU_DBG_PRINT(x, ...)      
#endif

#define MP_PROCESSOR_ID() (unsigned int)mfspr(SPRN_PIR)

#if defined(CONFIG_MEDIA_SPL) || defined(CONFIG_NAND_SPL)
#define udelay(x)               { \
        volatile u32 delay; \
        for (delay = 0; delay < 1000; delay++); \
        }
#undef BUG_ON
#define BUG_ON(x)
#endif

/* Module Reset Sequence */
/* 1) Enable Module clock (set appropriate bit in SCU_CLKEN or SCU_CLKEN1)   */
/* 2) Release the module's reset (clear the appropriate bit in               */
/*      SCU_SRST or SCU_SRST1 register)                                      */
/* 3) Clear Module CSR reset (clear appropriate bit in SCU_CSR_SRST          */
/*      or SCU_CSR_SRST1 register)                                           */
/* 4) Disable Module's RAM shutdown (write 0 to CFG_MEM_RAM_SHUTDOWN         */
/*      register in module's Global Diag section)                            */
/* 5) Read back CFG_MEM_RAM_SHUTDOWN register to ensure AXI write was posted */
/* 6) Wait till the module's MEM_RDY is set by polling for appropriate bit   */
/*      in SCU_MRDY register to be set                                       */
int reset_apm86xxx_block(struct apm86xxx_reset_ctrl *reset_ctrl)
{
	u32 scu_base;
	atomic_t *lock_addr;
	volatile u32 data;
	int mem_rdy_timeout_cnt;
	
	scu_base = (unsigned int) CONFIG_SYS_SCU_BASE;

#if defined(CONFIG_MEDIA_SPL) || defined(CONFIG_NAND_SPL) || !defined(CONFIG_MP)
	lock_addr = NULL;
#else
	/* Locking is required when DDR is ready */
	if (gd->shmem == NULL)
		lock_addr = NULL;
	else
		lock_addr  = (void *) ((u8 *) gd->shmem);
#endif
 	
	/* Reset the IP */
	if (reset_ctrl->reg_group == REG_GROUP_SRST_CLKEN) {
		/* Assert reset first */
		if (reset_ctrl->csr_reset_mask)
			atomic_csr_setbits(lock_addr, 
				reset_ctrl->csr_reset_mask, 
				(void *) (scu_base + SCU_CSR_SRST_ADDR));
		if (reset_ctrl->reset_mask)
			atomic_csr_setbits(lock_addr, reset_ctrl->reset_mask, 
				(void *) (scu_base + SCU_SRST_ADDR));
		if (reset_ctrl->clken_mask) 
			atomic_csr_clrbits(lock_addr, reset_ctrl->clken_mask, 
				(void *) (scu_base + SCU_CLKEN_ADDR));

		/* Don't clear reset if reset_type is ASSERT_RESET_FLAG */
		if (reset_ctrl->reset_type == ASSERT_RESET_FLAG) 
			return 0;

		/* Read back to make sure AXI writes are posted */
		if (reset_ctrl->clken_mask) {
			atomic_csr_setbits(lock_addr, reset_ctrl->clken_mask, 
				(void *) (scu_base + SCU_CLKEN_ADDR));
			data = in_be32((u32 *)(scu_base + SCU_CLKEN_ADDR));
		}
		if (reset_ctrl->reset_mask) {
			atomic_csr_clrbits(lock_addr, reset_ctrl->reset_mask, 
				(void *) (scu_base + SCU_SRST_ADDR));
			data = in_be32((u32 *)(scu_base + SCU_SRST_ADDR));
		}
		if (reset_ctrl->csr_reset_mask) {
			atomic_csr_clrbits(lock_addr, 
				reset_ctrl->csr_reset_mask, 
				(void *) (scu_base + SCU_CSR_SRST_ADDR));
			data = in_be32((u32 *)(scu_base + SCU_CSR_SRST_ADDR));
		}
#if !defined(CONFIG_NAND_SPL) 
	} else if (reset_ctrl->reg_group == REG_GROUP_SRST1_CLKEN1) {
		if (reset_ctrl->csr_reset_mask)
			atomic_csr_setbits(lock_addr, 
				reset_ctrl->csr_reset_mask, 
				(void *) (scu_base + SCU_CSR_SRST1_ADDR));
		if (reset_ctrl->reset_mask)
			atomic_csr_setbits(lock_addr, reset_ctrl->reset_mask, 
				(void *) (scu_base + SCU_SRST1_ADDR));
		if (reset_ctrl->clken_mask) 
			atomic_csr_clrbits(lock_addr, reset_ctrl->clken_mask, 
				(void *) (scu_base + SCU_CLKEN1_ADDR));

		/* Don't clear reset if reset_type is ASSERT_RESET_FLAG */
		if (reset_ctrl->reset_type == ASSERT_RESET_FLAG) 
			return 0;

		/* Read back to make sure AXI writes are posted */
		if (reset_ctrl->clken_mask) {
			atomic_csr_setbits(lock_addr, reset_ctrl->clken_mask, 
				(void *) (scu_base + SCU_CLKEN1_ADDR));
			data = in_be32((u32 *)(scu_base + SCU_CLKEN1_ADDR));
		}
		if (reset_ctrl->reset_mask) {
			atomic_csr_clrbits(lock_addr, reset_ctrl->reset_mask, 
				(void *) (scu_base + SCU_SRST1_ADDR));
			data = in_be32((u32 *)(scu_base + SCU_SRST1_ADDR));
		}
		if (reset_ctrl->csr_reset_mask) {
			atomic_csr_clrbits(lock_addr, 
				reset_ctrl->csr_reset_mask, 
				(void *) (scu_base + SCU_CSR_SRST1_ADDR));
			data = in_be32((u32 *)(scu_base + SCU_CSR_SRST1_ADDR));
		}
#endif
	} else {
		return -EINVAL;
	}
	sync();

#ifdef SCU_DEBUG
	if (reset_ctrl->reg_group == REG_GROUP_SRST_CLKEN) {
		SCU_DBG_PRINT("CSR_SRST: %08x SRST: %08x CLKEN %08x\n", 
				*((u32*)(scu_base + SCU_CSR_SRST_ADDR)),
				*((u32*)(scu_base + SCU_SRST_ADDR)),
				*((u32*)(scu_base + SCU_CLKEN_ADDR)));
	} else if (reset_ctrl->reg_group == REG_GROUP_SRST1_CLKEN1) {
		SCU_DBG_PRINT("CSR_SRST1: %08x SRST1: %08x CLKEN1 %08x\n", 
				*((u32*)(scu_base + SCU_CSR_SRST1_ADDR)),
				*((u32*)(scu_base + SCU_SRST1_ADDR)),
				*((u32*)(scu_base + SCU_CLKEN1_ADDR)));
	}
#endif
	
	/* Disable Module Shutdown ONLY if in shutdown - common across 
	   register groups
	   FIXME: change to Atomic operations 
	 */

	if (reset_ctrl->ram_shutdown_addr) {
		if ((data = in_be32(reset_ctrl->ram_shutdown_addr))) {
			out_be32(reset_ctrl->ram_shutdown_addr, 0);
			data = in_be32(reset_ctrl->ram_shutdown_addr);
			iobarrier_rw();
			BUG_ON(!reset_ctrl->mem_rdy_mask);
			mem_rdy_timeout_cnt = MEM_RDY_TIMEOUT_COUNT;
			while (mem_rdy_timeout_cnt-- > 0) {
				if (((data = in_be32((u32 *)(scu_base+SCU_MRDY_ADDR))) &
					reset_ctrl->mem_rdy_mask) == 
					reset_ctrl->mem_rdy_mask)
					return 0;
			}
			if (mem_rdy_timeout_cnt <= 0) 
				return -EIO;
		}
	}

	return 0;
}

#if !defined(CONFIG_NAND_SPL)
static void wait_for_pllstart(void)
{
	u32 scu_base = (unsigned int) CONFIG_SYS_SCU_BASE;
	volatile u32 *addr;
	volatile u32 data;

	addr = (volatile u32 *) (scu_base + SCU_PLLDLY_ADDR);
	/* Wait for SCU_PLLDLY.PLLSTARTDELAY time */
	/* FIXME: Calculate delay from SCU_PLLDLY.PLLSTARTDELAY */
	data = in_be32(addr);
	data >>= PLLSTARTDELAY_SHIFT;

	/* Calculate delay in us - SYSCLK is 25MHz : Minimum is 5 us*/
	udelay(10);
}

static void wait_for_plllock(u32 lock_mask)
{
	u32 scu_base = (unsigned int) CONFIG_SYS_SCU_BASE;
	volatile u32 *addr;
	volatile u32 data;

	addr = (volatile u32 *) (scu_base + SCU_PLLDLY_ADDR);

	/* Wait for SCU_PLLDLY.PLLSTARTDELAY time */
	/* FIXME: Calculate delay from SCU_PLLDLY.PLLLOCKDELAY */
	data = in_be32(addr);
	data &= PLLLOCKDELAY_MASK;

	/* Calculate delay in us - SYSCLK is 25MHz : Minimum is 5 us*/
	udelay(10);
	
	/* Sometimes PLLLOCK bit might not to be set - print a warning */
#if 0 
	addr = (volatile u32 *) (scu_base + SCU_SOCPLLSTAT_ADDR);
	data = in_be32(addr);
	if (!(data & lock_mask))
		SCU_DBG_PRINT("PLL Lock did not happen\n");
#endif
}
#endif

#if !defined(CONFIG_NAND_SPL)
int enable_ddr_pll(struct apm86xxx_pll_ctrl *ddr_pll)
{
	volatile u32 *pll2_addr;
	volatile u32 pll2_val, data;
	volatile u32 *addr;
	u32 scu_base = (unsigned int) CONFIG_SYS_SCU_BASE;

	pll2_addr = (volatile u32 *) (scu_base + SCU_SOCPLL2_ADDR);
	pll2_val = in_be32(pll2_addr);

	/* Return Success if PLL already configured */
	if (!(pll2_val & PWRDN2_MASK)) 
		return 0;
	
	/* Clear SOCPLL2 PWRDWN */
	pll2_val = (pll2_val | RESET2_MASK) & ~(PWRDN2_MASK | BYPASS2_MASK);
	out_be32(pll2_addr, pll2_val);

#ifndef CONFIG_PAL
	/* Write SOCPLL2 CLKF/CLKOD/CLKR values */
	pll2_val = (pll2_val & ~(CLKR2_MASK | CLKOD2_MASK | CLKF2_MASK)) | 
			(CLKR2_WR(ddr_pll->clkr_val) | 
			CLKOD2_WR(ddr_pll->clkod_val) | 
			CLKF2_WR(ddr_pll->clkf_val));
	out_be32(pll2_addr, pll2_val);

	/* Write SOCPLLADJ2 BWADJ values - half of CLKF's divider val */
	addr = (volatile u32 *) (scu_base + SCU_SOCPLLADJ2_ADDR);
	data = in_be32(addr);
	data &= ~BWADJ2_MASK;
	data |= ddr_pll->bwadj_val;
	out_be32(addr, data);
#endif

	/* Wait for SCU_PLLDLY.PLLSTARTDELAY time */
	wait_for_pllstart();

	/* Clear SOCPLL2 RESET */
	out_be32(pll2_addr, pll2_val & ~RESET2_MASK);

#ifndef CONFIG_PAL
	/* Wait for SOCPLLSTAT.LOCK2 or PLLDLY.PLLLOCKDELAY */
	wait_for_plllock(LOCK2_MASK);
#endif

	return 0;
}
#else
int enable_ddr_pll(struct apm86xxx_pll_ctrl *ddr_pll)
{
	volatile u32 *pll2_addr;
	volatile u32 pll2_val;
	volatile u32 *addr;
	u32 scu_base = (unsigned int) CONFIG_SYS_SCU_BASE;

	pll2_addr = (volatile u32 *) (scu_base + SCU_SOCPLL2_ADDR);

	pll2_val = (in_be32(pll2_addr) | RESET2_MASK) & ~(PWRDN2_MASK | BYPASS2_MASK);
	out_be32(pll2_addr, pll2_val);

	pll2_val = (pll2_val & ~(CLKR2_MASK | CLKOD2_MASK | CLKF2_MASK)) | 
			CLKF2_WR(ddr_pll->clkf_val);
	out_be32(pll2_addr, pll2_val);

	addr = (volatile u32 *) (scu_base + SCU_SOCPLLADJ2_ADDR);
	out_be32(addr, (in_be32(addr) & ~BWADJ2_MASK) | ddr_pll->bwadj_val);

	in_be32((volatile u32 *) (scu_base + SCU_PLLDLY_ADDR));
	udelay(10);

	out_be32(pll2_addr, pll2_val & ~RESET2_MASK);

	in_be32((volatile u32 *) (scu_base + SCU_PLLDLY_ADDR));
	udelay(10);
	return 0;
}
#endif

void disable_ddr_pll(void)
{
	volatile u32 *pll2_addr;
	u32 scu_base = (unsigned int) CONFIG_SYS_SCU_BASE;

	pll2_addr = (volatile u32 *) (scu_base + SCU_SOCPLL2_ADDR);

	/* Assert SOCPLL4 RESET */
	setbits_be32(pll2_addr, RESET2_MASK);

	/* Assert SOCPLL4 PWRDWN */
	setbits_be32(pll2_addr, PWRDN2_MASK);
}

#if !defined(CONFIG_NAND_SPL)
int enable_lcd_pll(struct apm86xxx_pll_ctrl *lcd_pll)
{
	volatile u32 *pll3_addr;
	volatile u32 pll3_val, data;
	volatile u32 *addr;
	u32 scu_base = (unsigned int) CONFIG_SYS_SCU_BASE;

	pll3_addr = (volatile u32 *) (scu_base + SCU_SOCPLL3_ADDR);
	pll3_val = in_be32(pll3_addr);

	/* Return Success if PLL already configured */
	if (!(pll3_val & PWRDN3_MASK)) 
		return 0;
	
	/* Clear SOCPLL3 PWRDWN */
	pll3_val = (pll3_val | RESET3_MASK) & ~(PWRDN3_MASK | BYPASS3_MASK);
	out_be32(pll3_addr, pll3_val);

	/* Write SOCPLL3 CLKF/CLKOD/CLKR values */
	pll3_val = (pll3_val & ~(CLKR3_MASK | CLKOD3_MASK | CLKF3_MASK)) | 
			(CLKR3_WR(lcd_pll->clkr_val) | 
			CLKOD3_WR(lcd_pll->clkod_val) | 
			CLKF3_WR(lcd_pll->clkf_val));
	out_be32(pll3_addr, pll3_val);

	/* Write SOCPLLADJ3 BWADJ values - half of CLKF's divider val */
	addr = (volatile u32 *) (scu_base + SCU_SOCPLLADJ3_ADDR);
	data = in_be32(addr);
	data &= ~BWADJ3_MASK;
	data |= lcd_pll->bwadj_val;
	out_be32(addr, data);

	/* Wait for SCU_PLLDLY.PLLSTARTDELAY time */
	wait_for_pllstart();

	/* Clear SOCPLL3 RESET */
	out_be32(pll3_addr, pll3_val & ~RESET3_MASK);

	/* Wait for SOCPLLSTAT.LOCK2 or PLLDLY.PLLLOCKDELAY */
	wait_for_plllock(LOCK3_MASK);

	return 0;
}

int enable_eth_pll(struct apm86xxx_pll_ctrl *eth_pll)
{
	volatile u32 *pll4_addr;
	volatile u32 pll4_val, data;
	volatile u32 *addr;
	u32 scu_base = (unsigned int) CONFIG_SYS_SCU_BASE;

	pll4_addr = (volatile u32 *) (scu_base + SCU_SOCPLL4_ADDR);
	pll4_val = in_be32(pll4_addr);
#if 0
	/* Return Success if PLL already configured */
	if (!(pll4_val & PWRDN4_MASK)) {
		printf("PLL4 in normal operate, not reconfigure\n");
		return -1;
	}
#endif
	//printf("configure ETH_PLL (PLL4)\n");
	/* Clear SOCPLL4 PWRDWN */
	pll4_val = (pll4_val | RESET4_MASK) & ~(PWRDN4_MASK | BYPASS4_MASK);
	out_be32(pll4_addr, pll4_val);

	/* Write SOCPLL4 CLKF/CLKOD/CLKR values */
	pll4_val = (pll4_val & ~(CLKR4_MASK | CLKOD4_MASK | CLKF4_MASK)) |
			(CLKR4_WR(eth_pll->clkr_val) | 
			CLKOD4_WR(eth_pll->clkod_val) |
			CLKF4_WR(eth_pll->clkf_val));
	out_be32(pll4_addr, pll4_val);

	/* Write SOCPLLADJ4 BWADJ values - half of CLKF's divider val */
	addr = (volatile u32 *) (scu_base + SCU_SOCPLLADJ4_ADDR);
	data = in_be32(addr);
	data &= ~BWADJ4_MASK;
	data |= eth_pll->bwadj_val;
	out_be32(addr, data);

	/* Wait for SCU_PLLDLY.PLLSTARTDELAY time */
	wait_for_pllstart();

	/* Clear SOCPLL4 RESET */
	out_be32(pll4_addr, pll4_val & ~RESET4_MASK);

	/* Wait for SOCPLLSTAT.LOCK4 or PLLDLY.PLLLOCKDELAY */
	wait_for_plllock(LOCK4_MASK);

	return 0;
}

void disable_lcd_pll(void)
{
	volatile u32 *pll3_addr;
	u32 scu_base = (unsigned int) CONFIG_SYS_SCU_BASE;

	pll3_addr = (volatile u32 *) (scu_base + SCU_SOCPLL3_ADDR);

	/* Assert SOCPLL4 RESET */
	setbits_be32(pll3_addr, RESET3_MASK);

	/* Assert SOCPLL4 PWRDWN */
	setbits_be32(pll3_addr, PWRDN3_MASK);
}

void disable_eth_pll(void)
{
	volatile u32 *pll4_addr;
	u32 scu_base = (unsigned int) CONFIG_SYS_SCU_BASE;

	pll4_addr = (volatile u32 *) (scu_base + SCU_SOCPLL4_ADDR);

	/* Assert SOCPLL4 RESET */
	setbits_be32(pll4_addr, RESET4_MASK);

	/* Assert SOCPLL4 PWRDWN */
	setbits_be32(pll4_addr, PWRDN4_MASK);
}
#endif

int apm86xxx_write_scu_reg(u32 offset, u32 val)
{
	volatile u32 *scu_addr;
	u32 scu_base = (u32) CONFIG_SYS_SCU_BASE;

	if (offset < 0x1FF) {
		scu_addr = (volatile u32 *) (scu_base + offset);
		out_be32(scu_addr, val);
		val = in_be32(scu_addr);
		return 0;
	} 
	return -EINVAL;
}

int apm86xxx_read_scu_reg(u32 offset, u32 *val)
{
	volatile u32 *scu_addr;
	u32 scu_base = (u32) CONFIG_SYS_SCU_BASE;

	if (offset < 0x1FF) {
		scu_addr = (volatile u32 *) (scu_base + offset);
		*val = in_be32(scu_addr);
		return 0;
	} 
	return -EINVAL;
}

#if !defined(CONFIG_NAND_SPL)
static inline u32 _get_cpu_pll_output(volatile u32 *cpu_pll)
{
	volatile u32 data;
	u32 pll_nf, pll_nr, pll_od;
	volatile u32 *soc_pll0_addr = (volatile u32 *)
					(CONFIG_SYS_SCU_BASE + SCU_SOCPLL0_ADDR);

	data = in_be32(soc_pll0_addr);
	pll_nf = (data & CLKF0_MASK) + 1;
	pll_nr = ((data & CLKR0_MASK) >> CLKR0_SHIFT) + 1;
	pll_od = ((data & CLKOD0_MASK) >> CLKOD0_SHIFT) + 1;
	*cpu_pll = (CONFIG_SYS_CLK_FREQ * pll_nf)/ (pll_nr * pll_od); 

	return *cpu_pll ? 0: -EINVAL;
}

static inline u32 _get_soc_pll_output(volatile u32 *soc_pll)
{
	volatile u32 data;
	u32 pll_nf, pll_nr, pll_od;
	volatile u32 *soc_pll1_addr = (volatile u32 *)
					(CONFIG_SYS_SCU_BASE + SCU_SOCPLL1_ADDR);
	data = in_be32(soc_pll1_addr);
	pll_nf = (data & CLKF1_MASK) + 1;
	pll_nr = ((data & CLKR1_MASK) >> CLKR1_SHIFT) + 1;
	pll_od = ((data & CLKOD1_MASK) >> CLKOD1_SHIFT) + 1;
	*soc_pll = (CONFIG_SYS_CLK_FREQ * pll_nf)/ (pll_nr * pll_od); 

	return *soc_pll ? 0: -EINVAL;
}
#endif /* !defined(CONFIG_NAND_SPL) */

static inline u32 _get_ddr_pll_output(volatile u32 *ddr_pll)
{
	volatile u32 data;
	u32 pll_nf, pll_nr, pll_od;
	volatile u32 *soc_pll2_addr = (volatile u32 *)
					(CONFIG_SYS_SCU_BASE + SCU_SOCPLL2_ADDR);

	data = in_be32(soc_pll2_addr);
	pll_nf = (data & CLKF2_MASK) + 1;
	pll_nr = ((data & CLKR2_MASK) >> CLKR2_SHIFT) + 1;
	pll_od = ((data & CLKOD2_MASK) >> CLKOD2_SHIFT) + 1;
	*ddr_pll = (CONFIG_SYS_CLK_FREQ * pll_nf)/ (pll_nr * pll_od); 

	return *ddr_pll ? 0: -EINVAL;
}

#if !defined(CONFIG_NAND_SPL)
#define CPM_CMD_CTL		(CONFIG_SYS_SCU_BASE + 0xa00)
#define CPM_CMD_STAT		(CONFIG_SYS_SCU_BASE + 0xa04)
#define CPM_RDDATA		(CONFIG_SYS_SCU_BASE + 0xa0c)
#define CPM_STAT		(CONFIG_SYS_SCU_BASE + 0xa10)
#define CPM_STATMASK		(CONFIG_SYS_SCU_BASE + 0xa14)

#define CPM_REG_PLB_DFS		0x01
#define CPM_REG_CPU(id)		((id)*0x20)
#define CPM_REG_CPU_DFS(id)	(CPM_REG_CPU(id) + 0x10)
#define CPM_REG_CPU_PWR_CTL	(CPM_REG_CPU(Id) + 0x12)

#define CPM_CMD_SEND		0x80000000
#define CPM_CMD_WR		0x40000000
#define CPM_CMD_RD		0x00000000
#define CPM_CMD_ADDR(reg)	((0x000000ff & (reg)) << 8)
#define CPM_CMD_WRDATA(data)	(0x000000ff & (data))
#define CPM_CMD_INPROGRESS	0x00000001
#define CPM_CMD_RDDATA		0x000000ff

u32 cpm_read(u32 reg)
{
	u32 retry;
	u32 timeout;
	u32 stat;
	u32 valA = 0;
	u32 valB = 0;
	u32 valC = 0;
	u32 cmp  = 0;
	u32 cmd  = CPM_CMD_SEND | CPM_CMD_RD | CPM_CMD_ADDR(reg);

	for (retry = 0; retry < 10; retry++) {
		/* Send read command to CPM */
		out_be32((void *)CPM_CMD_CTL, cmd);

		/* Wait for CPM to complete */
		for (timeout = 0; timeout < 100; timeout++) {
			stat = in_be32((void *)CPM_CMD_CTL);
			if ((stat & CPM_CMD_SEND) == 0) {
				break;
			}
		}

		for (timeout = 0; timeout < 100; timeout++) {
			stat = in_be32((void *)CPM_CMD_STAT);
			if ((stat & CPM_CMD_INPROGRESS) == 0) {
				break;
			}
		}

		if ( timeout >= 100)
			continue;

		/* Verify data was read correctly */
		if (cmp == 0) {
			cmp = 1;
			valA = in_be32((void *)CPM_RDDATA);
			valA &= CPM_CMD_RDDATA;
		} else if (cmp == 1) {
			cmp = 2;
			valB = in_be32((void *)CPM_RDDATA);
			valB &= CPM_CMD_RDDATA;
			if (valA != valB)
				cmp = 0;
		} else {
			valC = in_be32((void *)CPM_RDDATA);
			valC &= CPM_CMD_RDDATA;
			if (valA != valC)
				cmp = 0;
			else
				return valA;
		}
	}

	return 0;
}

void cpm_write(u32 reg, u32 val)
{
	u32 retry;
	u32 timeout;
	u32 stat;
	u32 retval;
	u32 cmd = CPM_CMD_SEND | CPM_CMD_WR |
		  CPM_CMD_ADDR(reg) | CPM_CMD_WRDATA(val);

	for (retry = 0; retry < 10; retry++) {
		/* Send write command to CPM */
		out_be32((void *)CPM_CMD_CTL, cmd);

		/* Wait for CPM to complete */
		for (timeout = 0; timeout < 100; timeout++) {
			stat = in_be32((void *)CPM_CMD_CTL);
			if ((stat & CPM_CMD_SEND) == 0) {
				break;
			}
		}

		for (timeout = 0; timeout < 100; timeout++) {
			stat = in_be32((void *)CPM_CMD_STAT);
			if ((stat & CPM_CMD_INPROGRESS) == 0) {
				break;
			}
		}

		/* Verify data was written correctly */
		retval = cpm_read(reg);
		if ( val == retval)
			break;
	}

	return;
}

int apm86xxx_get_cpu_divisor2(int id, u32 *divisor)
{
	u32 i;
	for (i = 0; i < 10; i++) {
		*divisor = cpm_read(CPM_REG_CPU_DFS(id));
		if ((*divisor >= 2) && (*divisor <=32))
			return 0;
	}

	return -EINVAL;
}

int apm86xxx_get_plb2x_divisor2(u32 *divisor)
{
	u32 i;
	for (i = 0; i < 10; i++) {
		*divisor = cpm_read(CPM_REG_PLB_DFS);
		if ((*divisor >= 2) && (*divisor <=16))
			return 0;
	}

	return -EINVAL;
}

int apm86xxx_set_cpu_divisor2(int id, u32 divisor)
{
	cpm_write(CPM_REG_CPU_DFS(id), divisor);
	return 0;
}

int apm86xxx_set_plb2x_divisor2(u32 divisor)
{
	cpm_write(CPM_REG_PLB_DFS, divisor);
	return 0;
}

int apm86xxx_get_cpu_divisor(int id, u32 *divisor)
{
	u32 mode;
	int major, minor;

#if 0
	*divisor = DEFAULT_APM86xxx_CPU_DIVISOR;
#else
	apm86xxx_get_cpu_divisor2(id, divisor);
#endif

	mode = apm86xxx_ipp_getmode(&major, &minor);
	switch (mode) {
	case IPP_ROM_MODE_MASK:
		break;
	case IPP_RUNTIME_MODE_MASK:
	case IPP_EXTBOOT_MODE_MASK:
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

int apm86xxx_get_plb_divisor(u32 *divisor)
{
	u32 mode;
	int major, minor;

#if 0
	*divisor = DEFAULT_APM86xxx_PLB_DIVISOR;
#else
	apm86xxx_get_plb2x_divisor2(divisor);
	*divisor *= 2;
#endif

	mode = apm86xxx_ipp_getmode(&major, &minor);

	switch (mode) {
	case IPP_ROM_MODE_MASK:
		break;
	case IPP_RUNTIME_MODE_MASK:
	case IPP_EXTBOOT_MODE_MASK:
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/* Get BUS Divisors */
u32 apm86xxx_get_divisor(int id)
{
	u32 div;

	switch (id) {
	case APM86xxx_AXI:
		div = in_be32((volatile u32 *)(CONFIG_SYS_SCU_BASE +
					SCU_SOCDIV_ADDR));
		return IO_AXI_CLK_FREQ_SEL_RD(div);
		break;
	case APM86xxx_AHB:
		div = in_be32((volatile u32 *)(CONFIG_SYS_SCU_BASE +
					SCU_SOCDIV_ADDR));
		return AHB_CLK_FREQ_SEL_RD(div);
	case APM86xxx_IAHB:
		div = in_be32((volatile u32 *)(CONFIG_SYS_SCU_BASE +
				SCU_SOCDIV2_ADDR));
		return MPA_AHB_CLK_FREQ_SEL2_RD(div);
	case APM86xxx_APB:
		div = in_be32((volatile u32 *)(CONFIG_SYS_SCU_BASE +
					SCU_SOCDIV_ADDR));
		return APB_CLK_FREQ_SEL_RD(div);
	case APM86xxx_PCIE_REFCLK:
		div = in_be32((volatile u32 *)(CONFIG_SYS_SCU_BASE +
					SCU_SOCDIV1_ADDR));
		return PCIE_CLK_FREQ_SEL1_RD(div);
	case APM86xxx_CRYPTO:
		div = in_be32((volatile u32 *)(CONFIG_SYS_SCU_BASE +
					SCU_SOCDIV2_ADDR));
		return SEC_CRYPTO_FREQ_SEL2_RD(div);
	case APM86xxx_EBUS:
		div = in_be32((volatile u32 *)(CONFIG_SYS_SCU_BASE +
					SCU_SOCDIV5_ADDR));
		return EBUS_CLK_FREQ_SEL5_RD(div);
	case APM86xxx_SDIO:
		div = in_be32((volatile u32 *)(CONFIG_SYS_SCU_BASE +
					SCU_SOCDIV5_ADDR));
		return SDIO_CLK_FREQ_SEL5_RD(div);
	default:
		/* Return 1 to just avoid divide by zero exception */
		SCU_DBG_PRINT("Unsupported APM86xxx Divisor ID!!!\n");
		return 1;
	}
}
#endif /* !defined(CONFIG_NAND_SPL) */

int apm86xxx_get_freq(int id, u32 *freq)
{
#if !defined(CONFIG_NAND_SPL)
	u32 data, div;
#endif
	int ret = -EINVAL;

	switch (id) {
#if !defined(CONFIG_NAND_SPL)
	case APM86xxx_CPU_PLL:
	case APM86xxx_CPU0:
	case APM86xxx_CPU1:
	case APM86xxx_PLB:
		ret = _get_cpu_pll_output(freq);
		if (id == APM86xxx_CPU_PLL) break;
		if (!ret) {
			switch (id) {
			case APM86xxx_CPU0:
				ret = apm86xxx_get_cpu_divisor(CORE0_ID, &div);
			case APM86xxx_CPU1:
				ret = apm86xxx_get_cpu_divisor(CORE1_ID, &div);
			case APM86xxx_PLB:
				ret = apm86xxx_get_plb_divisor(&div);
			default: 
				ret = -EINVAL;
			}
			if (!ret)
				*freq /= div; 
		}
		break;
	case APM86xxx_SOC_PLL:
		ret = _get_soc_pll_output(freq);
		break;
#endif /* !defined(CONFIG_NAND_SPL) */
	case APM86xxx_DDR_PLL:
	case APM86xxx_DDR:
		ret = _get_ddr_pll_output(freq);
		if (!ret && (id == APM86xxx_DDR))
			*freq /= 2;
		break;
#if !defined(CONFIG_NAND_SPL)
	case APM86xxx_AXI:
		ret = _get_soc_pll_output(&data);
		if (!ret) {
			*freq = data/apm86xxx_get_divisor(id);
		}
		break;
	case APM86xxx_AHB:
		ret = _get_soc_pll_output(&data);
		if (!ret) {
			*freq = data/apm86xxx_get_divisor(id);
		}
		break;
	case APM86xxx_IAHB:
		ret = _get_soc_pll_output(&data);
		if (!ret) {
			*freq = data/apm86xxx_get_divisor(id);
		}
		break;
	case APM86xxx_APB:
		ret = _get_soc_pll_output(&data);
		if (!ret) {
			*freq = data/apm86xxx_get_divisor(id);
		}
		break;
	case APM86xxx_PCIE_REFCLK:
		ret = _get_soc_pll_output(&data);
		if (!ret) {
			*freq = data/apm86xxx_get_divisor(id);
		}
		break;
	case APM86xxx_CRYPTO:
		ret = _get_soc_pll_output(&data);
		if (!ret) {
			*freq = data/apm86xxx_get_divisor(id);
		}
		break;
	case APM86xxx_EBUS:
		ret = _get_soc_pll_output(&data);
		if (!ret) {
			*freq = data/apm86xxx_get_divisor(id);
		}
		break;
	case APM86xxx_SDIO:
		ret = _get_soc_pll_output(&data);
		if (!ret) {
			*freq = data/apm86xxx_get_divisor(id);
		}
		break;
#endif /* !defined(CONFIG_NAND_SPL) */
	default:
		return -EINVAL;
	}

	return 0;
}

#if !defined(CONFIG_NAND_SPL)
u32 apm86xxx_ipp_getmode(int *major, int *minor)
{
	volatile u32 val;

	u32 mpa_reg_base = (unsigned int) (CONFIG_SYS_SCU_BASE +
				APM_MPA_REG_OFFSET);

	val = in_be32((volatile u32 *)(mpa_reg_base + MPA_SCRATCH_ADDR));
	*major = (val & IPP_MAJOR_VER_MASK) >> IPP_MAJOR_VER_SHIFT;
	*minor = val & IPP_MINOR_VER_MASK;
	
	return val & IPP_MODE_MASK;
}
#endif

#if defined(CONFIG_IPP_MESSAGING)

/* Add delay loop */
#define IPP_DELAY		{ \
	volatile u32 delay; \
	for (delay = 0; delay < 10000; delay++); \
	}

int apm86xxx_read_ipp_msg(u32 *data)
{
	int timeout_cnt = 100;
	u32 mpa_reg_base = (unsigned int) (CONFIG_SYS_SCU_BASE + APM_MPA_REG_OFFSET);
	
	/* Wait for iPP message since we don't operate in interrupt mode */
	while (!(in_be32((volatile u32 *)(mpa_reg_base + MPA_P0_INTSTAT_ADDR))
				& P0_ACODE_REQ_MASK)) {
		IPP_DELAY;
		if (--timeout_cnt == 0)
			return -ENAVAIL;
	}

	/* Read iPP message */
	*data = in_be32((volatile u32 *)(mpa_reg_base + MPA_P0_ACODE_ADDR));
	/* Send acknowledgment to iPP */
	out_be32((volatile u32 *)(mpa_reg_base + MPA_P0_INTSTAT_ADDR),
			P0_ACODE_REQ_MASK);

	/* Wait for iPP ack */
	timeout_cnt = 100;
	while (in_be32((volatile u32 *)(mpa_reg_base + MPA_P0_INTSTAT_ADDR))
				& P0_ACODE_REQ_MASK) {
		IPP_DELAY;
		if (--timeout_cnt == 0)
			return -ETIMEDOUT;
	}
	return 0;
}

int apm86xxx_write_ipp_msg(u32 data, u32 param) 
{
	int timeout_cnt = 100;
	u32 mpa_reg_base = (unsigned int) (CONFIG_SYS_SCU_BASE + APM_MPA_REG_OFFSET);

	/* Clear previous pending ack if any */
	if (in_be32((volatile u32 *)(mpa_reg_base + MPA_P0_INTSTAT_ADDR))
				& P0_PCODE_ACK_MASK) {
		out_be32((volatile u32 *)(mpa_reg_base + MPA_P0_INTSTAT_ADDR),
				P0_PCODE_ACK_MASK);
	}
	/* Send message to iPP */
	out_be32((volatile u32 *)(mpa_reg_base + MPA_SCRATCH1_ADDR), param);
	out_be32((volatile u32 *)(mpa_reg_base + MPA_P0_PCODE_ADDR), data);

	/* Wait for ack */
	while (!(in_be32((volatile u32 *)(mpa_reg_base + MPA_P0_INTSTAT_ADDR))
				& P0_PCODE_ACK_MASK)) {
		IPP_DELAY;
		if (--timeout_cnt == 0)
			return -ETIMEDOUT;
	}

	/* Clear iPP ack */
	out_be32((volatile u32 *)(mpa_reg_base + MPA_P0_INTSTAT_ADDR),
				P0_PCODE_ACK_MASK);
	return 0;
}

#endif /* #if defined(CONFIG_IPP_MESSAGING) */

#if defined(CONFIG_CMD_IPP)
int apm86xxx_ipp_fwload(u32 fwload_addr) 
{
	int ret;
	int timeout_cnt = 500;
	volatile u32 val;
	volatile u32 *data_addr;
	u32 size, load_addr;
	ipp_imageinfo_t *ipp_fw_hdr;
	u16 *pfw_type;
	u32 *pfw_load_addr, *pfw_magic, *pfw_sz;
	u32 mpa_reg_base = (unsigned int) (CONFIG_SYS_SCU_BASE + APM_MPA_REG_OFFSET);

	val = in_be32((volatile u32 *)(mpa_reg_base + MPA_SCRATCH_ADDR));
	if ((val & IPP_MODE_MASK) != IPP_ROM_MODE_MASK) {
		printf("iPP not in ROM mode ... skipping firmware load\n");
		return -EPERM;
	}

	ipp_fw_hdr = (ipp_imageinfo_t *) (fwload_addr + IPP_IMAGEINFO_OFFSET);
	pfw_magic = (u32 *)((u32)ipp_fw_hdr + offsetof(ipp_imageinfo_t, image_signature));
	pfw_type = (u16 *)((u32)ipp_fw_hdr + offsetof(ipp_imageinfo_t, image_type));
	pfw_load_addr = (u32 *)((u32)ipp_fw_hdr + offsetof(ipp_imageinfo_t, image_loadaddr));
	pfw_sz = (u32 *)((u32)ipp_fw_hdr + offsetof(ipp_imageinfo_t, image_size));

	if (IPP_IMAGE_SIGNATURE != in_le32(pfw_magic)) {
		printf("invalid iPP firmware magic 0x%08x ... skipping firmware load\n",
				in_le32(pfw_magic));
		return -EINVAL;
	}

	if (IPP_RUNTIME_TYPE != in_le16(pfw_type)) {
		printf("invalid iPP firmware type (%d) ... skipping firmware load\n",
				in_le16(pfw_type));
		return -EINVAL;
	}

	/* Word align size */
	load_addr = in_le32(pfw_load_addr);
	size = in_le32(pfw_sz) & ~0x3;
	printf("Loading iPP Firmware (%d bytes) from 0x%x to iPP IRAM 0x%x ... ",
		size, fwload_addr, load_addr);
	size >>= 2;
	data_addr = (volatile u32 *)(fwload_addr);
	while(size-- > 0) {
		out_be32((volatile u32 *)(mpa_reg_base + MPA_IRAM_ADDR_ADDR), load_addr);
		val = in_be32((volatile u32 *)(mpa_reg_base + MPA_IRAM_ADDR_ADDR));
		iobarrier_rw();
		out_le32((volatile u32 *)(mpa_reg_base + MPA_IRAM_DATA_ADDR), *data_addr); 
		val = in_be32((volatile u32 *)(mpa_reg_base + MPA_IRAM_DATA_ADDR));
		load_addr += 4;
		data_addr++;
	}
	printf(" completed\n");

	/* Send FWLoad command to IRAM */
	val = IPP_ENCODE_FWLOAD_MSG(IPP_FWLOAD_FROM_PPC);
	if ((ret = apm86xxx_write_ipp_msg(val,IPP_MSG_PARAM_UNUSED)) != 0) {
		printf("Failed to write iPP FWLOAD Msg ... \n");
		return ret;
	}

	/* Wait for iPP Firmware to be Loaded */
	while (timeout_cnt-- > 0) {
		val = in_be32((volatile u32 *)(mpa_reg_base + MPA_SCRATCH_ADDR));
		if ((val & IPP_MODE_MASK) == IPP_RUNTIME_MODE_MASK) {
			printf("iPP Runtime Firmware Version %d.%d loaded ... successful\n",
					(val&IPP_MAJOR_VER_MASK)>>IPP_MAJOR_VER_SHIFT,
					val&IPP_MINOR_VER_MASK);
			return 0;
		} else {
			IPP_DELAY;
		}
	}
	return -ETIMEDOUT;
}

int apm86xxx_switch_cpm_mode(int cpu, int mode_flag) 
{
	u32 msg_data;

	msg_data = IPP_ENCODE_USER_MSG(IPP_CONFIG_SET_HDLR,IPP_MSG_CONTROL_URG_BIT,
			IPP_SLEEP_CTRL_MODE_VAR,cpu);
	return apm86xxx_write_ipp_msg(msg_data, mode_flag);
}

#define PWRMODE_DOZE		0
#define PWRMODE_LIGHTNAP	1
#define PWRMODE_NAP		2	
#define PWRMODE_SLEEP		3	
#define PWRMODE_DSLEEP		4	
#define PWRMODE_MAX		5	

#define PWRMODE_PWR_ON  IPP_PWRMGMT_CMD_CPU_SET_NORMAL
#define PWRMODE_CLK_OFF IPP_PWRMGMT_CMD_CPU_CLKGATE
#define PWRMODE_PWR_OFF IPP_PWRMGMT_CMD_CPU_POWEROFF

struct apm86xxx_pm_modes
{
        int cpu[MAX_PPC_CORES];
        const char *name;
};

int apm86xxx_set_powerstate(char *mode) 
{
	int i, id, ret;
	u32 msg_data;
	int cpu_id = MP_PROCESSOR_ID();

	static struct apm86xxx_pm_modes apm86xxx_pm_modes[PWRMODE_MAX] = {
	        [PWRMODE_DOZE]  = {{PWRMODE_CLK_OFF,  PWRMODE_CLK_OFF}, "doze" },
	        [PWRMODE_NAP]   = {{PWRMODE_CLK_OFF, PWRMODE_PWR_OFF}, "nap" },
	        [PWRMODE_LIGHTNAP]   = {{PWRMODE_PWR_ON, PWRMODE_PWR_OFF}, "lightnap" },
	        [PWRMODE_SLEEP] = {{PWRMODE_CLK_OFF, PWRMODE_CLK_OFF}, "sleep" },
	        [PWRMODE_DSLEEP]= {{PWRMODE_PWR_OFF, PWRMODE_PWR_OFF}, "deepsleep" },
	};

	printf("Setting (%s) mode ... \n", mode);
	for (i=0; i<PWRMODE_MAX; i++) {
		if (strcmp(apm86xxx_pm_modes[i].name, mode) == 0) {
			printf("Entering (%s) state ... \n", mode);
			for (id=PPC_CORE0; id<MAX_PPC_CORES; id++) {
				/* Send Message to Configure GPIO 13 as Wakeup interrupt */
				msg_data = IPP_ENCODE_USER_MSG(IPP_CONFIG_SET_HDLR,IPP_MSG_CONTROL_URG_BIT,
						IPP_GPIO_CONFIG_VAR,GPIO_TYPE_WAKEUP_IPP_INPUT);
				ret = apm86xxx_write_ipp_msg(msg_data, 0x1<<IPP_GPIO_13);
				if (ret) {
					printf("Failed to write GPIO wakeup message for CPU%d\n", id);
					return ret;
				}

				/* Send iPP Message to Powerdown PPCx */
				msg_data = IPP_ENCODE_USER_MSG(IPP_PWRMGMT_HDLR,IPP_MSG_CONTROL_URG_BIT,
		                               	apm86xxx_pm_modes[i].cpu[id],id);
				ret = apm86xxx_write_ipp_msg(msg_data, IPP_MSG_PARAM_UNUSED);
				if (ret) {
					printf("Failed to write powercmd for CPU%d\n", id);
					return ret;
				}
				printf("Sent iPP PowerCMD (%d) for CPU%d... \n",
						apm86xxx_pm_modes[i].cpu[id], id);

				ret =  apm86xxx_switch_cpm_mode(id, IPP_GLOBAL_SLEEP_MODE); 
				if (ret) {
					printf("Failed CPM mode switch for CPU%d\n", id);
					return ret;
				}
				printf("Switch CPU%d to global mode ... \n", id);
			}

			printf("successful ... \n");
		        if (apm86xxx_pm_modes[i].cpu[cpu_id] != PWRMODE_PWR_ON) {
				unsigned long msr_save;

				printf("CPU%d entering wait state... \n", cpu_id);
				/* set wait state MSR */
				msr_save = mfmsr();
				isync();
				__asm__ __volatile__ ("sync" : : : "memory");
				mtmsr(msr_save|MSR_WE|MSR_EE|MSR_CE|MSR_DE);
				sync();
			}
			return ret;
		}
	}
	printf("Invalid state (%s) ... \n", mode);
	return -EINVAL;
}

int apm86xxx_write_cpmreg(u8 offset, u8 value) 
{
	u32 msg_data;

	msg_data = IPP_ENCODE_DEBUG_MSG(IPP_DBG_SUBTYPE_CPMWRITE,IPP_MSG_CONTROL_URG_BIT,
			offset,value);
	return apm86xxx_write_ipp_msg(msg_data, IPP_MSG_PARAM_UNUSED);
}

int apm86xxx_read_cpmreg(u8 offset, u8 *value) 
{
	int ret;
	u32 msg_data;
	u8 type, subtype, param0, param1;

	msg_data = IPP_ENCODE_DEBUG_MSG(IPP_DBG_SUBTYPE_CPMREAD,IPP_MSG_CONTROL_URG_BIT,
			offset,IPP_MSG_PARAM_UNUSED);
	ret = apm86xxx_write_ipp_msg(msg_data, IPP_MSG_PARAM_UNUSED);
	if (ret) return ret;

	ret = apm86xxx_read_ipp_msg(&msg_data);
	if (ret) return ret;

	type = IPP_DECODE_MSG_TYPE(msg_data);
	subtype = IPP_DECODE_DBGMSG_TYPE(msg_data);
	param0 = IPP_DECODE_DBGMSG_P0(msg_data);
	param1 = IPP_DECODE_DBGMSG_P1(msg_data);
	if ((type == IPP_DEBUG_MSG) && (subtype == IPP_DBG_SUBTYPE_CPMREAD_RESP) &&
			(param0 == offset)) {
		*value = param1;
		return 0;
	} else {
		return -EINVAL;
	}
}
#endif /* CONFIG_CMD_IPP */
