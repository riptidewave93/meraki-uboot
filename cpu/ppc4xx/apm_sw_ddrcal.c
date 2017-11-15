/*
 * APM APM86xxx DDR3 controller calibration code
 *
 * Copyright (c) 2010 Applied Micro Circuits Corporation.
 * All rights reserved. Feng Kan <fkan@apm.com>.
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
 */
#define DEBUG
#undef DEBUG_REGMOD
#undef DEBUG_CALIB

#include <common.h>
#include <command.h>
#include <ppc4xx.h>
#include <i2c.h>
#include <asm/io.h>
#include <asm/processor.h>
#include <asm/mmu.h>
#include <asm/cache.h>
#include <asm/mp.h>
#include <asm/apm_ipp_csr.h>
#include <asm/apm_scu.h>
#include "config.h"
#include "apm_spd_ddr.h"


#if defined(CONFIG_APM86XXX)

#ifdef CONFIG_APM86XXX_DDR_AUTOCALIBRATION


/*
 * How many iteration calibration will run
 */
#define MULTI_PASS_NO		1

/*
 * Use SW read gate leveling
 */
#ifdef CONFIG_HW_REV_14
#define HW_WRITE_LEVELING
#endif
#define HW_READ_GATE_LEVELING	/* Enable hw read gate leveling */
#undef HW_READ_EYE_TRAINING	/* Enable hw read eye training */
#undef SW_FINE_WRLVLING		/* Enable second round sw fine write leveling */

#define SW_CALIB_SZ		0x20  /* specify in no of words */
#define SW_CALIB_GRAIN		0x2
#endif


#ifdef DEBUG
#define DUMP_SPD_INFO		print_spd_info
#else
#define DUMP_SDRAM_REG(...)
#define DUMP_SPD_INFO(...)
#endif

#ifdef CONFIG_APM86XXX_DDR_AUTOCALIBRATION

extern void memc_dimm_mr_wr(u8, u16);
extern int memc_fifo_we_dly_rd(u8, u8);
extern int memc_fifo_we_dly(u8, u8, u16);
extern int memc_rd_dqs_dly_rd(u8, u8);
extern int memc_rd_dqs_dly(u8, u8, u16);
extern int memc_wr_dqs_dly_rd(u8, u8);
extern int memc_wr_dqs_dly(u8, u8, u16);
extern int memc_wr_data_dly(u8, u8, u16);
extern int memc_dq_dqs_offset(u8, u8);
extern int memc_wrlvl(u8, u8, u16); 
extern int memc_gatelvl(u8, u8, u16);

extern void reset_ddr_controller(void);

int mslicetest(u8 rank, u32 size, u8 slice, u8 alt, u8 no_patt) {
        volatile u32 *ptr, *test;
	u32 addr;
        u32 mask=0, offset, rd_data, exp_data;
        int i,err;
        u32 pattern[8];
	u32 *start_addr = (u32 *)(rank * 2 << 28);
	u32 *end_addr = start_addr + size;

	if (alt) {
		pattern[0] = 0xFFFFFFFF;
		pattern[1] = 0xAAAAAAAA;
		pattern[2] = 0xDDDDDDDD;
		pattern[3] = 0xCCCCCCCC;
		pattern[4] = 0xBBBBBBBB;
		pattern[5] = 0xEEEEEEEE;
		pattern[6] = 0x99999999;
		pattern[7] = 0x88888888;
	} else {
		pattern[7] = 0xFFFFFFFF;
		pattern[0] = 0xAAAAAAAA;
		pattern[1] = 0xDDDDDDDD;
		pattern[2] = 0xCCCCCCCC;
		pattern[3] = 0xBBBBBBBB;
		pattern[4] = 0xEEEEEEEE;
		pattern[5] = 0x99999999;
		pattern[6] = 0x88888888;
	}

        err = 0;
        if(slice==0 || slice==4) {
                mask = 0xff000000;
        } else if(slice==1 || slice==5) {
                mask = 0x00ff0000;
        } else if(slice==2 || slice==6) {
                mask = 0x0000ff00;
        } else if(slice==3 || slice==7) {
                mask = 0x000000ff;
        }
        if(slice<4) {
                offset = 0;
        } else {
                offset = 4;
        }
        addr = (u32)start_addr + offset;

#ifdef DEBUG_CALIB
        debug("\nSLICE:%d\tADDR:0x%08x\tMASK:0x%08x", slice, addr, mask);
#endif
	/* For each pattern */
        for(i = 0; i < no_patt; i++) {
                for(ptr = (u32 *)start_addr; ptr < end_addr; ptr += 1) {
			if ((u32)ptr & 0x20)
				*ptr = ~pattern[i];
			else
				*ptr = pattern[i];
                }

		flush_dcache_range(addr, addr + (size << 2) - 1);
                for(ptr = (u32 *)addr; ptr < end_addr; ptr += 2) {
			//test = ptr + 6 + (offset >> 2);
			test = ptr;
                        rd_data = *test;
                        rd_data &= mask;
			if ((u32)ptr & 0x20)
				exp_data = ~pattern[i] & mask;
			else
				exp_data = pattern[i] & mask;

                        if(rd_data != exp_data) {
#ifdef DEBUG_CALIB
                                debug("\nERR @ 0x%08x : EXP: 0x%08x; GOT: 0x%08x", (u32)ptr, exp_data, rd_data);
#endif
                                return -1;
                        }
                }
        }
        return 1;
}


int rdslicetest(u8 rank, u32 size, u8 slice) 
{
        u32 *ptr;
        u32 addr;
        u32 pattern, mask=0, offset;
        u32 rd_data, exp_data;
	u32 *start_addr = (u32 *)(rank * 2 << 28);
	u32 *end_addr = start_addr + size - 1;
        int i,err;
        err = 0;

        if(slice==0 || slice==4) {
                mask = 0xff000000;
        } else if(slice==1 || slice==5) {
                mask = 0x00ff0000;
        } else if(slice==2 || slice==6) {
                mask = 0x0000ff00;
        } else if(slice==3 || slice==7) {
                mask = 0x000000ff;
        }
        if(slice<4) {
                offset = 0;
        } else {
                offset = 4;
        }
        addr = (u32)start_addr + offset;
        pattern = 0x0;

#ifdef DEBUG_CALIB
        debug("\n%s: SLICE:%d\tADDR:0x%08x\tMASK:0x%08x\n",__func__, slice,addr,mask);
#endif
        for(ptr = (u32 *)addr,i=0; ptr<end_addr; ptr+=2,i++) {
		rd_data = *ptr;
#ifdef DEBUG_CALIB
		debug("SLICE:%d\tADDR:0x%08x\tMASK:0x%08x\tPATTERN:0x%08x\tRD_DATA:0x%08x\n",
			slice, (u32)ptr, mask, pattern, rd_data);
#endif
		exp_data = pattern & mask;
		rd_data &= mask;
		if(rd_data != exp_data)
			return -1;
		pattern = ~pattern;
        }
        return 1;
}

void precal_setup(unsigned int speed)
{
	int i;

	/* Set Ratio value for address/command timing */
	memc_regmod(PHY_CTRL_SLAVE, 0x80, 22, 31);
	
	/*
	 * Write leveling initialization
	 */
	for (i = 0; i < 9; i++) {
		memc_wrlvl(0, i, 0x0);
		memc_wrlvl(1, i, 0x0);
		memc_wrlvl(2, i, 0x0);
		memc_wrlvl(3, i, 0x0);
	}

        memc_dq_dqs_offset(0, MEMC_DQS2DQ_OFFSET);
        memc_dq_dqs_offset(1, MEMC_DQS2DQ_OFFSET);
        memc_dq_dqs_offset(2, MEMC_DQS2DQ_OFFSET);
        memc_dq_dqs_offset(3, MEMC_DQS2DQ_OFFSET);
        memc_dq_dqs_offset(4, MEMC_DQS2DQ_OFFSET);
        memc_dq_dqs_offset(5, MEMC_DQS2DQ_OFFSET);
        memc_dq_dqs_offset(6, MEMC_DQS2DQ_OFFSET);
        memc_dq_dqs_offset(7, MEMC_DQS2DQ_OFFSET);
        memc_dq_dqs_offset(8, MEMC_DQS2DQ_OFFSET);


	for(i=0; i<4; i++) {
		if (speed <= 800) {
			memc_gatelvl(i, 0, 0x0);
			memc_gatelvl(i, 1, 0x0);
			memc_gatelvl(i, 2, 0x1);
			memc_gatelvl(i, 3, 0xf);
			memc_gatelvl(i, 4, 0x29);
			memc_gatelvl(i, 5, 0x37);
			memc_gatelvl(i, 6, 0x44);
			memc_gatelvl(i, 7, 0x51);
			memc_gatelvl(i, 8, 0x1c);
		}
		if ((speed > 800) && (speed <= 1333)) {
#if defined(CONFIG_DDR_DISCRETE)
			/* These are values for discrete dimms */
			memc_gatelvl(i, 0, 0xa8);
			memc_gatelvl(i, 1, 0x91);
			memc_gatelvl(i, 2, 0x85);
			memc_gatelvl(i, 3, 0x75);
			memc_gatelvl(i, 4, 0x69);
			memc_gatelvl(i, 5, 0x5e);
			memc_gatelvl(i, 6, 0x51);
			memc_gatelvl(i, 7, 0x49);
			memc_gatelvl(i, 8, 0x34);
#else
			memc_gatelvl(i, 0, 0x79);
			memc_gatelvl(i, 1, 0x8e);
			memc_gatelvl(i, 2, 0x9f);
			memc_gatelvl(i, 3, 0xbf);
			memc_gatelvl(i, 4, 0xe6);
			memc_gatelvl(i, 5, 0xf9);
			memc_gatelvl(i, 6, 0x118);
			memc_gatelvl(i, 7, 0x135);
			memc_gatelvl(i, 8, 0xd3);
#endif
		}
		if (speed > 1333) {
			memc_gatelvl(i, 0, 0xb5);
			memc_gatelvl(i, 1, 0xce);
			memc_gatelvl(i, 2, 0xe2);
			memc_gatelvl(i, 3, 0x108);
			memc_gatelvl(i, 4, 0x136);
			memc_gatelvl(i, 5, 0x14e);
			memc_gatelvl(i, 6, 0x173);
			memc_gatelvl(i, 7, 0x195);
			memc_gatelvl(i, 8, 0x121);

		}
	}
	

	/*
	 * Red eye training with fixed value
	 */
	for(i=0; i<4; i++) {
		memc_rd_dqs_dly(i, 0, PHY_RD_DQS_SLAVE_VAL);
		memc_rd_dqs_dly(i, 1, PHY_RD_DQS_SLAVE_VAL);
		memc_rd_dqs_dly(i, 2, PHY_RD_DQS_SLAVE_VAL);
		memc_rd_dqs_dly(i, 3, PHY_RD_DQS_SLAVE_VAL);
		memc_rd_dqs_dly(i, 4, PHY_RD_DQS_SLAVE_VAL);
		memc_rd_dqs_dly(i, 5, PHY_RD_DQS_SLAVE_VAL);
		memc_rd_dqs_dly(i, 6, PHY_RD_DQS_SLAVE_VAL);
		memc_rd_dqs_dly(i, 7, PHY_RD_DQS_SLAVE_VAL);
		memc_rd_dqs_dly(i, 8, PHY_RD_DQS_SLAVE_VAL);
	}

	
	/* Set to use preprogrammed wr leveling values */
#ifdef HW_WRITE_LEVELING
	memc_regmod(PHY_1, 1, 9, 9);
#endif

#ifdef HW_READ_GATE_LEVELING
	/* Set to use preprogrammed rd DQS values */
	memc_regmod(PHY_1, 1, 20, 20);
#endif

#ifdef HW_READ_EYE_TRAINING
	/* Set to use preprogrammed read data eye values */
	memc_regmod(PHY_1, 1, 21, 21);
#endif

#ifdef HW_WRITE_LEVELING
	/* Use user initial values */
	memc_regmod(PHY_2, 1, 0, 0);
#else
	/* Set start wr leveling to use previous data slice */
	memc_regmod(PHY_2, 0, 0, 0);
#endif
	
#ifdef HW_WRITE_LEVELING
	/* Use write leveling value for init value */
	memc_regmod(PHY_2, 0, 1, 1);
#else
	/* Set start gate leveling to use init value */
	memc_regmod(PHY_2, 1, 1, 1);
#endif

#ifdef HW_READ_EYE_TRAINING
	/* Enable rd data eye training mode */
	memc_regmod(DDRC0_35, 1, 5, 5);
#endif

#ifdef HW_READ_GATE_LEVELING
	/* Enable rd DQS data leveling training mode */
	memc_regmod(DDRC0_35, 1, 6, 6);
#endif

#ifdef HW_WRITE_LEVELING
	/* Enable write leveling training mode */
	memc_regmod(DDRC0_35, 1, 7, 7); 
#endif

	/* Set timeout value for read leveling */
	memc_regmod(DDRC0_35, 0xfff, 8, 19); 

	/* Set timeout value for write leveling */
	memc_regmod(DDRC0_35, 0xfff, 20, 31); 

	/* Write leveling strobe gap */
	memc_regmod(DDRC0_37, 0x3f, 0, 7); 

	/* Read level to level delay */
	memc_regmod(DDRC0_37, 0x30, 8, 15);

	/* Read level to level delay */
	memc_regmod(DDRC0_37, 0x40, 18, 27);

}

void software_read_gate_seek_start(u8 rank) 
{
        int slice, err, offset;
	u32 step, round;
        int ratio_save[9];

        err = 1;
        debug("\nSeek software read gate start position:\n");
        memc_dimm_mr_wr(0x3, 0x0004);
        for(slice = 0; slice < 8; slice++) {
                ratio_save[slice] =  memc_fifo_we_dly_rd(rank, slice);
                offset = round = 0;
		step = SW_CALIB_GRAIN;

read_gate_refine:
                /* Increment Loop */
                while(err!=1 && offset < 0x3ff) {
                        offset += step;
                        memc_fifo_we_dly(rank, slice, offset);
                        err = rdslicetest(rank, SW_CALIB_SZ, slice);
                }
                err = 0;

		/* Check if the next one is OK too */
		if ((round == 0) && (offset < 0x3ff)) {
                        offset += step;
                        memc_fifo_we_dly(rank, slice, offset);
                        err = rdslicetest(rank, SW_CALIB_SZ, slice);
			if (err == 1)
				round ++;
			else
				goto read_gate_refine;
		}
		
		if ((round == 1) && (offset < 0x3ff)) {
			offset -=20;
			step = 1;
			round ++;
			goto read_gate_refine;
		}
			
                debug("Slice %d : Orig Off: %08x Optimal offset: %08x\n",
				slice, ratio_save[slice], offset);
        }
        memc_dimm_mr_wr(0x3,0x0);
}

void software_read_gate_training(u8 rank) 
{
        u32 slice, err, offset, low_val, high_val, opt_val;
	u32 max_high = 0, max_low = 0, max_width = 0, pass = 1;
	u32 avg_delta = 0x3ff;
        int ratio_save[9];

	/*
	 * First seek a good starting position
	 */
	
read_gate_pass:
        debug("\nStart software read gate calibration: round %d\n", pass);
        memc_dimm_mr_wr(0x3, 0x0004);
        for(slice = 0; slice < 8; slice++) {
                ratio_save[slice] =  memc_fifo_we_dly_rd(rank, slice);
                offset = 0;

read_gate_nextwin:
		high_val = low_val = 0;
                /* Find the window start low end */
		err = -1;
                while ((offset <= 0x3ff) && (err != 1)) {
			memc_fifo_we_dly(rank, slice, offset++);
			err = rdslicetest(rank, SW_CALIB_SZ, slice);
                }
                low_val = offset - 1;

		/* Find the window end  high end*/
		err = 1;
                while ((offset <= 0x3ff) && (err == 1)) {
                        memc_fifo_we_dly(rank, slice, offset++);
                        err = rdslicetest(rank, SW_CALIB_SZ, slice);
                }
		high_val = offset + 1;

		if ((high_val - low_val) > 0x20) {
#if 1 
			debug("Slice: %d, high %08x low %08x width %d\n", 
				slice, high_val, low_val, high_val - low_val);
#endif
			if (max_width < (high_val - low_val)) {
				/*
				 * We could have multiple window
				 * Lets test for linearity
				 * If fail, discard the result.
				 */
				if (slice > 1) {
					opt_val = low_val + (high_val - low_val) / 4;
					if (opt_val > ratio_save[slice - 1]) {
						if ((opt_val - ratio_save[slice - 1]) > (4 * avg_delta))
							goto read_gate_slice_done;
					}
				}
				max_width = high_val - low_val;
				max_high = high_val;
				max_low = low_val;
			}
		}
			
		if (offset <= 0x3ff)
			goto read_gate_nextwin;
			
read_gate_slice_done:
                opt_val = max_low + ((max_high - max_low) / 4);
                debug("Slice %d : Orig Off: %08x High: %08x Low: %08x" 
			"New: %08x delta 0x%x\n",
			slice, ratio_save[slice], max_high, 
			max_low, opt_val, avg_delta);
                ratio_save[slice] = opt_val;
                memc_fifo_we_dly(0, slice, opt_val);
		max_width = max_high = max_low = 0;

		/* Compute avg delta */
		if (slice > 0) {
			if (avg_delta == 0x3ff)
				avg_delta = ratio_save[slice] - ratio_save[slice - 1];
			else
				avg_delta = (avg_delta + (ratio_save[slice] - ratio_save[slice - 1]))/2;
		}
        }
        memc_dimm_mr_wr(0x3, 0x0);
	
	if (pass++ < MULTI_PASS_NO)
		goto read_gate_pass; 
}

void software_read_gate_training_2(u8 rank) 
{
        u32 slice, err, offset, low_val, high_val, opt_val;
        int ratio_save[9];

	/*
	 * First seek a good starting position
	 */
	//software_read_gate_seek_start(rank); 
	
        debug("\nStart software read gate calibration: method 2\n");
        memc_dimm_mr_wr(0x3, 0x0004);
        for(slice = 0; slice < 8; slice++) {
                ratio_save[slice] =  memc_fifo_we_dly_rd(rank, slice);
                offset = ratio_save[slice];

		high_val = low_val = 0;
                /* Push up */
		err = 1;
                while ((offset <= 0x3ff) && (err == 1)) {
			memc_fifo_we_dly(rank, slice, offset++);
			err = rdslicetest(rank, SW_CALIB_SZ, slice);
                }
                high_val = offset - 1;
                offset = ratio_save[slice];

		/* Push down */
		err = 1;
                while ((offset > 0x0) && (err == 1)) {
                        memc_fifo_we_dly(rank, slice, offset--);
                        err = rdslicetest(rank, SW_CALIB_SZ, slice);
                }
		low_val = offset + 1;

			
                opt_val = low_val + ((high_val - low_val) / 4);
                debug("Slice %d : Orig Off: %08x High: %08x Low: %08x New: %08x\n",
				slice, ratio_save[slice], high_val, low_val, opt_val);
                ratio_save[slice] = opt_val;
                memc_fifo_we_dly(0, slice, opt_val);
        }
        memc_dimm_mr_wr(0x3, 0x0);
	
}

void software_read_eye_training(u8 rank) 
{
        int slice,err,offset,low_val,high_val,opt_val;
        int ratio_save[9];
        err = 1;

        debug("\nStart software read DQS calibration:");
        memc_dimm_mr_wr(0x3,0x0004);
        for(slice=0;slice<8;slice++) {
                debug("\nSlice : %d",slice);
                ratio_save[slice] =  memc_rd_dqs_dly_rd(0,slice);
                offset = ratio_save[slice];
                /* Decrement Loop */
                while(err==1 && offset > 0x0) {
                        offset -= SW_CALIB_GRAIN;
			offset = (offset < 0) ? 0 : offset;
                        memc_rd_dqs_dly(0,slice,offset);
                        err = rdslicetest(rank, SW_CALIB_SZ, slice);
                }
                err = 1;
                low_val = offset;
                offset = ratio_save[slice];
                /* Increment Loop */
                while(err==1 && offset < 0x3ff) {
                        offset += SW_CALIB_GRAIN;
                        memc_rd_dqs_dly(0,slice,offset);
                        err = rdslicetest(rank, SW_CALIB_SZ, slice);
                }
                err = 1;
                high_val = offset;
                opt_val = low_val + ((high_val - low_val) / 2);
                debug(": Original Offset: %x .Highest offset: %x.Lowest offset: %x.Optimal offset: %x",ratio_save[slice],high_val,low_val,opt_val);
                memc_rd_dqs_dly(0,slice,opt_val);
        }
        memc_dimm_mr_wr(0x3,0x0);
}

#ifndef HW_WRITE_LEVELING
void software_write_leveling(u8 rank, u8 seek, u32 *save_array) {
        int slice,err,offset,low_val,high_val,opt_val;
        int ratio_save[9];
	u32 max_width = 0, max_high, max_low;
        err = 1;
#ifndef DEBUG
	char slash[] = "\\||/--\\||//--";
#endif

        debug("\nStart software write leveling: ");

        for(slice=0;slice<8;slice++) {
                ratio_save[slice] =  memc_wr_dqs_dly_rd(0,slice);

		/*
		 * If seeking boundary use upper limit
		 */
		switch (seek) {
			case 1:
			case 2:
				offset = 0x3ff;
				break;
			case 3:
				offset = save_array[slice];
				break;
			default:
				offset = ratio_save[slice];
				break;
		}
		debug(" Seek from 0x%x\n", offset);
			

#ifndef DEBUG
		putc(' ');
#endif
write_leveling_next_win:
                /* Find start of window */
                while(err != 1 && offset > 0x0) {
                        memc_wr_dqs_dly(rank, slice, offset);
                        memc_wr_data_dly(rank, slice, offset + 0x40);
                        err = mslicetest(rank, SW_CALIB_SZ, slice, 0, 1);
			offset --;
#ifndef DEBUG
			if ((offset % 0x2) == 0) {
				putc('\b');
				putc(slash[offset % 8]);
			}
#endif
                }
                high_val = offset;

                /* Find end of window */
                err = 1;
                while(err == 1 && offset > 0) {
                        memc_wr_dqs_dly(rank, slice, offset);
                        memc_wr_data_dly(rank, slice, offset + 0x40);
                        err = mslicetest(rank, SW_CALIB_SZ, slice, 0, 1);
			offset --;
#ifndef DEBUG
			if ((offset % 0x2) == 0) {
				putc('\b');
				putc(slash[offset % 8]);
			}
#endif
                }
                low_val = offset;

		if (max_width < (high_val - low_val)) {
			debug("Slice %d : Orig Off: %08x High: %08x Low: %08x\n",
				slice, ratio_save[slice], high_val, low_val);
			max_width = high_val - low_val;
			max_high = high_val;
			max_low = low_val;
		}

		if (offset > 0)
			goto write_leveling_next_win;
			

#ifndef DEBUG
		putc('\b');
		if (max_width < 0x10)
			printf("x");
		else
			printf(".");
#endif
                opt_val = max_low + ((max_high - max_low) / 2);
                debug("Slice %d : Final write leveling  High: %08x Low: %08x Mid %08x\n",
				slice, max_high, max_low, opt_val);
		/*
		 * If only seeking boundary, set the high value 
		 * for the next pass. Otherwise use midpoint.
		 */
		if (seek == 1)
			opt_val = max_high;

		save_array[slice] = max_high;

		memc_wr_dqs_dly(rank, slice, opt_val);
		memc_wr_data_dly(rank, slice, opt_val + 0x40);

		max_width = max_high = max_low = 0;
        }
}

void software_write_leveling_alt(u8 rank) {
        int slice,err,offset,low_val,high_val,opt_val;
        int ratio_save[9];
        err = 1;
#ifndef DEBUG
	char slash[] = "\\||/--\\||//--";
#endif

        debug("\nStart software write leveling: 2 ");

        for(slice=0;slice<8;slice++) {
                ratio_save[slice] =  memc_wr_dqs_dly_rd(0,slice);
		offset = ratio_save[slice];

		debug(" Seek from 0x%x\n", offset);
			
#ifndef DEBUG
		putc(' ');
#endif

                /* Sweep up to find high of window */
                err = 1;
                while(err == 1 && offset <= 0x3ff) {
                        memc_wr_dqs_dly(rank, slice, offset);
                        memc_wr_data_dly(rank, slice, offset + 0x40);
                        err = mslicetest(rank, SW_CALIB_SZ, slice, 1, 2);
			offset ++;
#ifndef DEBUG
			if ((offset % 0x2) == 0) {
				putc('\b');
				putc(slash[offset % 8]);
			}
#endif
                }
                high_val = offset;
		offset = ratio_save[slice];
		err = 1;

                /* Sweep down to find low of window */
                while(err == 1 && offset >= 0x0) {
                        memc_wr_dqs_dly(rank, slice, offset);
                        memc_wr_data_dly(rank, slice, offset + 0x40);
                        err = mslicetest(rank, SW_CALIB_SZ, slice, 0, 2);
			offset --;
#ifndef DEBUG
			if ((offset % 0x2) == 0) {
				putc('\b');
				putc(slash[offset % 8]);
			}
#endif
                }
                low_val = offset;

		debug("Slice %d : Orig Off: %08x High: %08x Low: %08x\n",
				slice, ratio_save[slice], high_val, low_val);

#ifndef DEBUG
		putc('\b');
		if ((high_val - low_val) < 0x10)
			printf("x");
		else
			printf(".");
#endif
		memc_wr_dqs_dly(rank, slice, ratio_save[slice]);
		memc_wr_data_dly(rank, slice, ratio_save[slice] + 0x40);
                ratio_save[slice] = opt_val = low_val + ((high_val - low_val) / 2);
                debug("Slice %d : Final write leveling  High: %08x Low: %08x Mid %08x\n",
				slice, high_val, low_val, opt_val);

        }

        for(slice=0;slice<8;slice++) {
		/*
		 * If only seeking boundary, set the high value 
		 * for the next pass. Otherwise use midpoint.
		 */

		memc_wr_dqs_dly(rank, slice, ratio_save[slice]);
		memc_wr_data_dly(rank, slice, ratio_save[slice] + 0x40);
	}
}
#endif

int apm_sw_ddrcal(void) {

#ifndef HW_WRITE_LEVELING
        u32 save_array[9];
#endif
	u32 ddrspeed;

	apm86xxx_get_freq(APM86xxx_DDR, &ddrspeed);
	ddrspeed /= 500000;

	precal_setup(ddrspeed);

	reset_ddr_controller();

	/* Read gate training sweep */
#ifndef HW_READ_GATE_LEVELING
	program_tlb(0, 0, 0x100000, SA_I|SA_G|AC_R|AC_W|AC_X);
	software_read_gate_training(0);
	software_read_gate_training_2(0);
	remove_tlb(0,0x100000);
#endif

	/* Write leveling sweep */
#ifndef HW_WRITE_LEVELING
	program_tlb(0, 0, 0x100000, AC_R|AC_W|AC_X);
	/*
	 * Seek values:
	 * 0 - find the big windows upper point from top
	 * 1 - find the big window mid point from register
	 * 2 - find the big window mid point from top
	 * 3 - find the big window mid point from array
	 */
	software_write_leveling(0, 2, save_array);
#ifdef SW_FINE_WRLVLING
	software_write_leveling_alt(0);
#endif
	remove_tlb(0,0x100000);
#endif

	/* Reset DDR Phy */
	memc_regmod(DDRC0_00, 0, 8, 8); 
	udelay(3);
	memc_regmod(DDRC0_00, 1, 8, 8);
	return 0;
}
#endif /* CONFIG_APM86XXX_DDR_AUTOCALIBRATION */
#endif /* CONFIG_APM86XXX */
