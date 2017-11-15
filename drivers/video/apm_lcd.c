/**
 * AppliedMicro LCD Controller
 *
 * Copyright (c) 2010 Applied Micro Circuits Corporation.
 * All rights reserved. Loc Ho <lho@amcc.com>
 *                      Ravi Patel <rapatel@apm.com>
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
 * @file apmdb9xxx.c
 *
 */
#include <common.h>
#include <command.h>
#include <asm/io.h>
#include <asm/mp.h>
#include <video_fb.h>
#include <asm/apm_ipp_csr.h>
#include <asm/apm_scu.h>
#include "videomodes.h"
#include "apm_lcd_csr.h"

/* #define DEBUG */

#ifdef DEBUG
#define DPRINT(x...) printf(x)
#else
#define DPRINT(x...) do{}while(0)
#endif

DECLARE_GLOBAL_DATA_PTR;

#if !defined(VIDEO_COLS_MAX) && !defined(VIDEO_COLS) && \
    !defined(VIDEO_PIXEL_SIZE)
#	define VIDEO_COLS_MAX		1280
#	define VIDEO_COLS_MAX		1024
#	define VIDEO_PIXEL_SIZE_MAX	4
#	define VIDEO_SIZE         	(VIDEO_COLS_MAX * VIDEO_ROWS_MAX * \
					VIDEO_PIXEL_SIZE_MAX) 
#endif

GraphicDevice lcdfb;

phys_addr_t LCD2PADDR(u32 vaddr)
{
	return (LCD_CSR_BASE_ADDR & 0xFF00000000ULL) | vaddr;
}

u32 LCD2VADDR(phys_addr_t paddr)
{
	return (u32) paddr;
}

static phys_addr_t PADDR(u32 vaddr)
{
	return vaddr;
}

static inline int INREG(u32 addr, int reg)
{
	int val;
	
	val = in_le32((void *) (addr + reg));
	DPRINT("LCD CSR RD 0x%08X val: 0x%08X\n", addr + reg, val);
	return val;
}

static inline void OUTREG(u32 addr, int reg, u32 val)	
{
	out_le32((void *) (addr + reg), val);
	DPRINT("LCD CSR WR 0x%08X val: 0x%08X\n", addr + reg, val);
}

static inline int INREG16(u32 addr, int reg)
{
	u32 addr1 = addr + reg;
        u32 addr32 = addr1 & ~0x3;
        u32 val32  = in_le32((void *) addr32);
        u32 val16;

        switch (addr1 & 0x3) {
        case 2:
                val16 = val32 >> 16;
                break;
        case 0:
        default:
                val16 = val32 & 0xFFFF;
                break;
        }
        DPRINT("LCD CSR RD16: 0x%08X value: 0x%04X (0x%08X 0x%08X)\n",
		addr1, val16, addr32, val32);
        return 0;
}

static inline void OUTREG16(u32 addr, int reg, u16 val)	
{
	u32 addr1  = addr + reg;
        u32 addr32 = addr1 & ~0x3;
        u32 val32  = in_le32((void *) addr32);

        switch ((u32) addr1 & 0x3) {
        case 2:
                val32 &= ~0xFFFF0000;
                val32 |= (u32) val << 16;
                break;
        case 0:
        default:
                val32 &= ~0xFFFF;
                val32 |= val;
                break;
        }
        DPRINT("LCD CSR WR16: 0x%08X value: 0x%04X (0x%08X 0x%08X)\n",
        	addr1, val, addr32, val32);
        out_le32((void *) addr32, val32);
}

int apm_lcd_map_init(GraphicDevice *pGD)
{
	int i;
	
	/* Clear all LCD address translate as we will use default which
	   map them directly to lower 32 address space */
	for (i = 0; i <= 16; i++) {
		OUTREG(pGD->isaBase, START_ADDR0_ADDR + i * 8, 0x00000000);
		OUTREG(pGD->isaBase, END_ADDR0_ADDR + i * 8, 0x00000000);
	}
		
	return 0;
}

int apm_lcd_map_bpp(int bits_per_pixel)
{
	switch (bits_per_pixel) {
	case 1:
		return 0;
	case 2:
		return 1;
	case 4: 
		return 2;
	default:
	case 8:
		return 3;
	case 15:
	case 16:
		return 4;
	case 18:
		return 5;
	case 24:
		return 6;  
	}
}

int apm_lcd_clk_init(GraphicDevice *pGD, struct ctfb_res_modes *res_mode)
{
	struct apm86xxx_reset_ctrl reset_ctrl;
	struct apm86xxx_pll_ctrl pll_ctr;
	int lcd_ser_freq_sel;
	int lcd_clk_freq_sel;
	u32 val;

	/* LCD Clock Formula:
	 * Input:       X = ?
	 * Input:       Y = ?
	 * Input:       REFRESH_RATE (Hz) = ?
	 * LCD_PCLK_IN Required = PCLK = X * 1.31 * Y * 1.04 * REFRESH_RATE

	 * Input:       LCD_PCLK_IN = 25000000
	 * Input:       OD = 2  NOTE: Various this value to find proper
	 * output clock
	 * Input Fixed: NR = 1
	 * VCO = LCD_PCLK_IN * 7 * OD
	 * NF = (VCO * NR) / LCD_PCLK_IN
	 * LCD_PLL_OUT = VCO / OD
	 * Input:       LCD_SER_FREQ_SEL = 1
	 * Input:       LCD_CLK_FREQ_SEL = 7
	 * LCD_PCLK_OUT = LCD_PLL_OUT / LCD_CLK_FREQ_SEL
	 * LCD_CLK = LCD_PLL_OUT / LCD_SER_FREQ_SEL
	 */

	if (res_mode->xres == 320 && res_mode->yres == 240) {
		pll_ctr.clkf_val = 105 - 1;
		pll_ctr.clkod_val = 15 - 1;
		pll_ctr.clkr_val = 1 - 1;
		pll_ctr.bwadj_val = 105 / 2 - 1;
		lcd_ser_freq_sel = 1;
		lcd_clk_freq_sel = 7;
	} else if (res_mode->xres == 480 && res_mode->yres == 272) {
		pll_ctr.clkf_val = 56 - 1;
		pll_ctr.clkod_val = 8 - 1;
		pll_ctr.clkr_val = 1 - 1;
		pll_ctr.bwadj_val = 56 / 2 - 1;
		lcd_ser_freq_sel = 1;
		lcd_clk_freq_sel = 7;
	} else if ((res_mode->xres == 640 && res_mode->yres == 480) ||
			(res_mode->xres == 800 && res_mode->yres == 600)) {
		pll_ctr.clkf_val = 28 - 1;
		pll_ctr.clkod_val = 4 - 1;
		pll_ctr.clkr_val = 1 - 1;
		pll_ctr.bwadj_val = 28 / 2 - 1;
		lcd_ser_freq_sel = 1;
		lcd_clk_freq_sel = 7;
	} else if ((res_mode->xres == 1024 && res_mode->yres == 768) ||
			(res_mode->xres == 1280 && res_mode->yres == 768) ||
			(res_mode->xres == 1280 && res_mode->yres == 1024) ||
			(res_mode->xres == 1920 && res_mode->yres == 1080)) {
		pll_ctr.clkf_val = 14 - 1;
		pll_ctr.clkod_val = 2 - 1;
		pll_ctr.clkr_val = 1 - 1;
		pll_ctr.bwadj_val = 14 / 2 - 1;
		lcd_ser_freq_sel = 1;
		lcd_clk_freq_sel = 7;
	} else {
		/* Need to compute all values - FIXME */
		return -1;
	}

	DPRINT("VIDEO: APMLCD: "
		"CLKF %d CLKOD %d CLKR %d BWAdj %d "
		"Ser Freq Sel %d CLK Freq Sel %d\n",
		pll_ctr.clkf_val, pll_ctr.clkod_val, pll_ctr.clkr_val,
		pll_ctr.bwadj_val, lcd_ser_freq_sel, lcd_clk_freq_sel);

	/* Select external LCD input clock */
	apm86xxx_read_scu_reg(SCU_SOCDIV3_ADDR, &val);
	val |= LCD_PLL_MUX_SEL3_MASK;
	apm86xxx_write_scu_reg(SCU_SOCDIV3_ADDR, val);
	apm86xxx_read_scu_reg(SCU_SOCDIV3_ADDR, &val);
	DPRINT("VIDEO: APMLCD: SOCDIV3 0x%08X\n", val);
	/* Program the LCD_SER and LCD_PCLK divisor */
	apm86xxx_read_scu_reg(SCU_SOCDIV1_ADDR, &val);
	val &= ~(LCD_SER_FREQ_SEL1_MASK | LCD_CLK_FREQ_SEL1_MASK);
	val |= LCD_SER_FREQ_SEL1_WR(lcd_ser_freq_sel)
		| LCD_CLK_FREQ_SEL1_WR(lcd_clk_freq_sel);
	apm86xxx_write_scu_reg(SCU_SOCDIV1_ADDR, val);
	apm86xxx_read_scu_reg(SCU_SOCDIV1_ADDR, &val);
	DPRINT("VIDEO: APMLCD: SOCDIV1 0x%08X\n", val);
	/* Program the PLL and wait for lock */
	enable_lcd_pll(&pll_ctr);
	apm86xxx_read_scu_reg(SCU_SOCPLL3_ADDR, &val);
	DPRINT("VIDEO: APMLCD: SOCPLL3 0x%08X\n", val);
	apm86xxx_read_scu_reg(SCU_SOCPLLADJ3_ADDR, &val);
	DPRINT("VIDEO: APMLCD: SOCPLLADJ3 0x%08X\n", val);

	/* Enable LCD clock and reset IP */
	memset(&reset_ctrl, 0, sizeof(reset_ctrl));
	reset_ctrl.reg_group  = REG_GROUP_SRST_CLKEN;
	reset_ctrl.clken_mask = LCD_F1_MASK;
	reset_ctrl.csr_reset_mask = LCD_F2_MASK;
	reset_ctrl.reset_mask = LCD_MASK;
	/* LCD RAMShutdown is mapped by MPIC_F1_MASK in SCU_MRDY */
	reset_ctrl.ram_shutdown_addr = (u32 __iomem *)
			(CONFIG_SYS_SDU_BASE + APM_GLBL_DIAG_OFFSET +
			LCD_CFG_MEM_RAM_SHUTDOWN_ADDR);
	reset_ctrl.mem_rdy_mask = MPIC_F1_MASK;
	if (reset_apm86xxx_block(&reset_ctrl))
		return -1;

	return 0;
}

int apm_lcd_core_init(GraphicDevice *pGD, struct ctfb_res_modes *res_mode,
			int bits_per_pixel)
{
	phys_addr_t paddr;
	u32 cr1;
	u32 val;
	u32 ops1;

	if (apm_lcd_clk_init(pGD, res_mode)) {
		printf("VIDEO: APMLCD: "
			"Error initializing LCD PLLs");
		pGD = NULL;
		return -1;
	}

	val = INREG(pGD->isaBase, LCDC_CIR_ADDR);
	DPRINT("Model: %s %d-bits Bus version 1.%02d\n",
		CIR_MN_RD(val) == 0x90 ? "BD9000" : "Unknown",
		CIR_BW_RD(val) == 5 ? 128 : (CIR_BW_RD(val) == 4 ? 64 : 32),
		CIR_REV_RD(val));

	apm_lcd_map_init(pGD);
	cr1 = INREG(pGD->isaBase, LCDC_CR1_ADDR);

	cr1 &= ~FBP1_MASK;	/* No 24 bpp packing */
	cr1 &= ~DEE1_MASK;	/* DMA End Address disable */
	cr1 &= ~LPS1_MASK;	/* Output to first port */
	cr1 &= ~FDW1_MASK;	/* Clear FIFO DMA request word */
	cr1 &= ~PSS1_MASK;	/* Load palette from slave bus instead frame 
				   buffer memeory */
	cr1 &= ~EBO1_MASK;	/* Little endian byte ordering */
	cr1 &= ~EPO1_MASK;	/* Little endian pixel ordering with byte */
	cr1 &= ~RGB1_MASK;	/* RGB format */
	cr1 |= LPE1_MASK;	/* Enable LCD power */
	cr1 &= ~LCE1_MASK;	/* Disable LCD controller until timing 
				   and etc is programmed */
	ops1 = 0;

	switch (bits_per_pixel) {
	case 8:
		ops1 |= 0x1;
		cr1 |= RGB1_MASK;       /* RGB format */
		break;
	/* For 16bpp, set out to 5:6:5 or 5:5:5 and for 18-bit/24-bit LCD,
	   output to LCD panel MSB */
	case 15:
	case 16:
		ops1 |= 0x1;
		cr1 |= FDW1_WR(0x01);
		break;
	case 24:
		cr1 |= FBP1_MASK;       /* 24 bpp packed pixel */
	default:
		ops1 |= 0x1;
		cr1 |= DEE1_MASK;       /* Enable DEAR */
		cr1 |= FDW1_WR(0x01);
		break;
	}

	/* ops1 |= 0x02; */	
	cr1 &= ~OPS1_MASK;
	cr1 |= OPS1_WR(ops1);	

	/* Set LCD_VSYNC Polarity high */
	if (res_mode->sync & FB_SYNC_VERT_HIGH_ACT)
		cr1 &= ~VSP1_MASK;
	else 
		cr1 |= VSP1_MASK;
	/* Set LCD_HSYNC Polarity high */
	if (res_mode->sync & FB_SYNC_HOR_HIGH_ACT)
		cr1 &= ~HSP1_MASK;
	else		 
		cr1 |= HSP1_MASK;
	/* Set Pixel clock Polarity rising */
	cr1 &= ~PCP1_MASK; 
	/* Set LCD enable polarity low */
	cr1 &= ~PCP1_MASK;
	/* High data polarity */
	cr1 |= DEP1_MASK;
	/* Set bpp */
	cr1 &= ~BPP1_MASK;
	cr1 |= BPP1_WR(apm_lcd_map_bpp(bits_per_pixel));
	cr1 &= ~LCE1_MASK;
/*	OUTREG(pGD->isaBase, LCDC_CR1_ADDR, cr1);	*/

	/* Set LVDS Invert and Intel 24.1 mode */
	val = INREG(pGD->isaBase, DEBUG_TEST_REG_ADDR);
	val |= 0x10;	/* Do not invert lvds */
	val |= 0x04;	/* intel24.1 mode */
	OUTREG(pGD->isaBase, DEBUG_TEST_REG_ADDR, val);

	/* Program Horizontal Timing */
	val = (u32) res_mode->hsync_len << 24 |
	      (u32) res_mode->left_margin << 16 |
	      ((u32) res_mode->xres/16) << 8 |
	      (u32) res_mode->right_margin;
	OUTREG(pGD->isaBase, LCDC_HTR_ADDR, val);
	
	/* Program Vertical Timing */	
	val = (u32) res_mode->upper_margin << 16 |
	      (u32) res_mode->lower_margin << 8 |
	      (u32) res_mode->vsync_len;
	OUTREG(pGD->isaBase, LCDC_VTR1_ADDR, val);
	OUTREG(pGD->isaBase, LCDC_VTR2_ADDR, res_mode->yres);
	
	/* Program Pixel clock timing */
	OUTREG(pGD->isaBase, LCDC_PCTR_ADDR, 0x00000201);
	
	/* Disable all interrupts */
	OUTREG(pGD->isaBase, LCDC_IMR_ADDR, 0x00000000); 
	
	/* Program Frame Buffer address */
	paddr = PADDR(pGD->frameAdrs);	
	OUTREG(pGD->isaBase, LCDC_DBAR_ADDR, (u32) paddr);
	paddr += (res_mode->xres * res_mode->yres * pGD->gdfBytesPP);
	OUTREG(pGD->isaBase, LCDC_DEAR_ADDR, (u32) paddr);
	 
	/* Program Map to map accordingly */
	if (((u32) (paddr >> 32)) != 0) {
		u64 paddr_end = paddr + pGD->memSize;
		OUTREG(pGD->isaBase, START_ADDR0_ADDR, 
				((u32) paddr & 0xFFFFFFF0) | 0x01);
		OUTREG(pGD->isaBase, END_ADDR0_ADDR, 
			((u32) paddr_end & 0xFFFFFFF0) |
				((u32) (paddr_end >> 32) & 0xF));
	}

	OUTREG(pGD->isaBase, LCDC_PWMFR_ADDR, 0x0030003e);
	OUTREG(pGD->isaBase, LCDC_PWMDCR_ADDR, 0x80);

	/* Enable the controller */
	cr1 |= LCE1_MASK;
	OUTREG(pGD->isaBase, LCDC_CR1_ADDR, cr1);
	return 0;
}

void *video_hw_init(void)
{
	GraphicDevice *pGD = &lcdfb;
	struct ctfb_res_modes *res_mode;
	struct ctfb_res_modes var_mode;
	char *penv;
	unsigned int *vm;
	unsigned long t1;
	unsigned long hsynch;
	unsigned long vsynch;
	int bits_per_pixel;
	int vesa_idx;
	int videomode;
	int tmp;
	int i;
		
	memset(pGD, 0, sizeof(GraphicDevice));
	pGD->isaBase = LCD2VADDR(LCD_CSR_BASE_ADDR);

#ifdef CONFIG_MP
	/* Check which CPU video is assigned to:
	 *  1. If VIDEOID is set, use that.
	 *  2. If VIDEOID is not set, check with MASTER CPU ID
	 */
	if ((penv = getenv ("VIDEOID")) != NULL) {
		if (simple_strtoul(penv, NULL, 16) != CPUID)
			return NULL;
	} else {
		if (CPUID != CONFIG_SYS_MASTER_CPUID)
			return NULL;
	}	
#endif

	tmp = 0;
	videomode = CONFIG_SYS_DEFAULT_VIDEO_MODE;
	/* get video mode via environment */
	if ((penv = getenv ("videomode")) != NULL) {
		/* deceide if it is a string */
		if (penv[0] <= '9') {
			videomode = (int) simple_strtoul (penv, NULL, 16);
			tmp = 1;
		}
	} else {
		tmp = 1;
	}
	if (tmp) {
		/* parameter are vesa modes */
		/* search params */
		for (i = 0; i < VESA_MODES_COUNT; i++) {
			if (vesa_modes[i].vesanr == videomode)
				break;
		}
		if (i == VESA_MODES_COUNT) {
			printf ("no VESA Mode found, switching to mode 0x%x ", 
				CONFIG_SYS_DEFAULT_VIDEO_MODE);
			i = 0;
		}
		res_mode = (struct ctfb_res_modes *) 
				&res_mode_init[vesa_modes[i].resindex];
		bits_per_pixel = vesa_modes[i].bits_per_pixel;
		vesa_idx = vesa_modes[i].resindex;
	} else {
		res_mode = (struct ctfb_res_modes *) &var_mode;
		bits_per_pixel = video_get_params (res_mode, penv);
	}

	/* calculate hsynch and vsynch freq (info only) */
	t1 = (res_mode->left_margin + res_mode->xres +
	      res_mode->right_margin + res_mode->hsync_len) / 8;
	t1 *= 8;
	t1 *= res_mode->pixclock;
	t1 /= 1000;
	hsynch = 1000000000L / t1;
	t1 *= (res_mode->upper_margin + res_mode->yres +
	       res_mode->lower_margin + res_mode->vsync_len);
	t1 /= 1000;
	vsynch = 1000000000L / t1;
	
	/* fill in Graphic device struct */
	sprintf(pGD->modeIdent, "%dx%dx%d %ldkHz %ldkHz", res_mode->xres,
		 res_mode->yres, bits_per_pixel, (hsynch / 1000),
		 (vsynch / 1000));
	pGD->winSizeX = res_mode->xres;
	pGD->winSizeY = res_mode->yres;
	pGD->plnSizeX = res_mode->xres;
	pGD->plnSizeY = res_mode->yres;

	switch (bits_per_pixel) {
	default:
        case 8:
                pGD->gdfBytesPP = 1;
                pGD->gdfIndex = GDF__8BIT_INDEX;
                break;
        case 15:
                pGD->gdfBytesPP = 2;
                pGD->gdfIndex = GDF_15BIT_555RGB;
                break;
        case 16:
                pGD->gdfBytesPP = 2;
                pGD->gdfIndex = GDF_16BIT_565RGB;
                break;
        case 24:
                pGD->gdfBytesPP = 3;
                pGD->gdfIndex = GDF_24BIT_888RGB;
                break;
        }

        if ((pGD->frameAdrs = gd->fb_base) == 0) {
                return NULL;
        }
        pGD->frameAdrs = (pGD->frameAdrs + 0xF) & ~0xF; 
	
	DPRINT("Video frame buffer @ %08x\n", pGD->frameAdrs);

	pGD->pciBase = pGD->frameAdrs;
	pGD->memSize = pGD->winSizeX * pGD->winSizeY * pGD->gdfBytesPP;

	/* Cursor Start Address */
	pGD->dprBase = pGD->frameAdrs;	/* Dummy */
	pGD->vprBase = pGD->frameAdrs;	/* Dummy */
	pGD->cprBase = pGD->frameAdrs;	/* Dummy */
	
	/* Clear video memory */
	i = pGD->memSize / 4;
	vm = (unsigned int *) pGD->frameAdrs;
	while (i--)
		*vm++ = 0;

	/* Initialize HW */
	apm_lcd_core_init(pGD, res_mode, bits_per_pixel);

	printf("Video: APM86xxx LCD Controller ID 0x%04X %s\n", 
		INREG(pGD->isaBase, CORE_ID_REG_ADDR), pGD->modeIdent);

	DPRINT("Video CSR @ 0x%02X.%08X (0x%08X)\n", 
		(u32) (LCD2PADDR(pGD->isaBase) >> 32), 
		(u32) (LCD2PADDR(pGD->isaBase)),
		pGD->isaBase);
        
	return pGD;
}

void video_set_lut(unsigned int index, unsigned char r, unsigned char g,              /* green */
			unsigned char b)
{
	GraphicDevice *pGD = &lcdfb;
	
	/* Update pallete */
	u32 cr1 = INREG(pGD->isaBase, LCDC_CR1_ADDR);
	int cr1_ops = OPS1_RD(cr1);
	int cr1_rgb = RGB1_RD(cr1);
	
	switch (cr1_rgb) {
	default:
	case 0:
		if (cr1_ops & 0x1) {
			/* RGB with 5:5:5 Format */
			OUTREG16(pGD->isaBase, LCDC_PALT_ADDR + index * 2, 
				(u16) r << 10 | (u16) g << 5 | (u16) b);  
		} else {
			/* RGB with 5:6:5 Format */
			OUTREG16(pGD->isaBase, LCDC_PALT_ADDR + index * 2, 
				(u16) r << 11 | (u16) g << 5 | (u16) b);  
		}
		break;
	case 1:
		if (cr1_ops & 0x1) {
			/* BGR with 5:5:5 Format */
			OUTREG16(pGD->isaBase, LCDC_PALT_ADDR + index * 2, 
				(u16) b << 10 | (u16) g << 5 | (u16) r);  
		} else {
			/* BGR with 5:6:5 Format */
			OUTREG16(pGD->isaBase, LCDC_PALT_ADDR + index * 2, 
				(u16) b << 11 | (u16) g << 5 | (u16) r);  
		}
		break;	
	}	
}
