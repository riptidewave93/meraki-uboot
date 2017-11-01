/*
 * Copyright (C) 1999-2013, Broadcom Corporation 
 *  
 *      Unless you and Broadcom execute a separate written software license 
 * agreement governing use of this software, this software is licensed to you 
 * under the terms of the GNU General Public License version 2 (the "GPL"), 
 * available at http://www.broadcom.com/licenses/GPLv2.php, with the 
 * following added to such license: 
 *  
 *      As a special exception, the copyright holders of this software give you 
 * permission to link this software with independent modules, and to copy and 
 * distribute the resulting executable under terms of your choice, provided that 
 * you also meet, for each linked independent module, the terms and conditions of 
 * the license of that module.  An independent module is a module which is not 
 * derived from this software.  The special exception does not apply to any 
 * modifications of the software. 
 *  
 *      Notwithstanding the above, under no circumstances may you combine this 
 * software in any way with any other Broadcom software provided under a license 
 * other than the GPL, without Broadcom's express prior written consent. 
 */
/*  *********************************************************************
    *********************************************************************
    *  Broadcom Common Diagnostics Environment (CDE)
    *********************************************************************
    *  Filename: cde_main.c
    *
    *  Function:
    *
    *  Author:  Chung Kwong (C.K.) Der (ckder@broadcom.com)
    *
 */

#include <mde_common_headers.h>

extern MDE_CMD *host_setup_local_cmds ( unsigned *uCommandCount );

void setupNewMdeWithCommonInfo ( sMde_t *newMde )
{
  int inx;

  // setup common MDE properties

  newMde -> version = MDE_VERSION;
  newMde -> date    = MDE_DATE_CODE;

  strcpy ( &newMde->name [0], MDE_NAME );

  newMde -> cli_history_depth = HISTORY_DEPTH;
  for (inx = 0; inx < HISTORY_DEPTH; inx ++) {
	newMde -> history [inx][0] = 0; // terminate history buffer lines
  }
  // setup MDE command commands
  newMde -> common.cmds = getMdeCommonCommands  ( &newMde -> common.cmd_count );
#ifdef HOST_CMD_INCLUDED
  newMde -> local.cmds  = host_setup_local_cmds ( &newMde -> local.cmd_count  );
#endif
  //  newMde -> = ;
}


// Compute the DDR PLL frequency in KHz.
// specific for ANA_DPLL_40G, at x4 mode.
// (joon lee)
// 
// inputs:
//    mult    =  PLL multipler  (P)
//    div_int =  PLL divider interger (N)
//    div_fraq=  PLL divider fraqtional (N fraq)
//    freq_mhz=  external reference clock in MHz
//    post_div=  post divider.
//
// output:
//    PLL freq in KHz.
//
int ana_dpll_40g_pll_khz(int mult, 
                         int div_int, 
                         int div_fraq, 
                         int fref_mhz, 
                         int post_div)
{

  int vco_freq, pll_freq;
  int internal_scaler = 20;
  int ndiv_scaler  = (1<<internal_scaler);
  int dram_freq;
  int left;
  int xtra_scaling = 1;
  int fraq;

  // apply range masks to input
  mult     &= 7;      // 3 bits
  div_int  &= 0xFF;   // 8 bits
  div_fraq &= 0xFFFFF;// 20 bits
  div_fraq >>= (20-internal_scaler);

  vco_freq = xtra_scaling*(div_int*ndiv_scaler + div_fraq) ;
  vco_freq /= mult;
  vco_freq /= post_div;
  vco_freq *= fref_mhz;

  pll_freq = vco_freq/ndiv_scaler;
  
  dram_freq = pll_freq * 2; // x4 mode

  // fractional
  fraq = 1000*(vco_freq % ndiv_scaler);
  fraq /= ndiv_scaler;
  fraq *= 2;

  dram_freq = dram_freq*1000 + fraq;
  return (dram_freq);
}

#define EXTERNAL_REF_CLOCK_MHZ 54

unsigned mde_do_ana_pll_calc ( sMde_t *mde )
{
  //
  // 833, 866 and 900 are derived from 933 adjusting downward
  //
  // 966, 1000, 1033 are derived from 1067 adjusting downward
  // 1107, 1134 are derived from 1067 adjusting upward
  //
  // The reported DDR frequency is calcuated based on the following two registers
  //

  unsigned pll6ch    = 0x2451C71D; // need to read register
  unsigned pll_mdiv0 = 0x00001004; // need to read register

  unsigned q, r;
  unsigned calc_val, tenth, compliment = 0, round_up;
  unsigned freq_mhz = EXTERNAL_REF_CLOCK_MHZ;

  unsigned mult     = (pll_mdiv0 & BCHP_MEMC_DDR23_SHIM_ADDR_CNTL_0_SYS_PLL_MDIV0_CHAN_MDIV_MASK);
  unsigned ndiv_int = (pll6ch & BCHP_MEMC_DDR23_SHIM_ADDR_CNTL_0_PLL6CH_PNDIV_SYS_NDIV_INT_MASK) >> \
	BCHP_MEMC_DDR23_SHIM_ADDR_CNTL_0_PLL6CH_PNDIV_SYS_NDIV_INT_SHIFT;

  unsigned ndiv_fra = (pll6ch & BCHP_MEMC_DDR23_SHIM_ADDR_CNTL_0_PLL6CH_PNDIV_SYS_NDIV_FRAC_MASK) >> \
	BCHP_MEMC_DDR23_SHIM_ADDR_CNTL_0_PLL6CH_PNDIV_SYS_NDIV_FRAC_SHIFT;
  unsigned post_div = ( pll6ch & BCHP_MEMC_DDR23_SHIM_ADDR_CNTL_0_PLL6CH_PNDIV_SYS_PDIV_MASK ) >> \
	BCHP_MEMC_DDR23_SHIM_ADDR_CNTL_0_PLL6CH_PNDIV_SYS_PDIV_SHIFT;

  calc_val = ana_dpll_40g_pll_khz ( mult, ndiv_int, ndiv_fra, freq_mhz, post_div );

  // The returned value is MHz X 1000 and it can be more or less than the target frequency
  q = calc_val / 1000;
  r = calc_val % 1000;

  calc_val = calc_val + ( 1000 - r);
  round_up = calc_val / 1000;
  //  host_printf ("\nDDR speed = %d.%dMHz rounded to %d\n",  q, r, round_up);
  return round_up; 
}

void mdeInit (sMde_t *mde)
{

  host_printf ("\nMDE for %s\n", mde->chip_name );
  host_printf ("size of MDE %d bytes\n", sizeof( sMde_t) );
  // clear the current command point to start
  mde -> current_command = NULL;

  // setup find_dq handling according to PHY data width
  // unless specified, default is 32
  host_printf ("set up find_dq handlers for %d-bit PHY\n", PHY_WIDTH);
#if PHY_WIDTH == 16
  mde->fdq_log_error    = find_all_errors_all_16;
  mde->fdq_show_result  = find_dq_show_result_all_16;
#else

  mde->fdq_log_error    = find_all_errors_all;
  mde->fdq_show_result  = find_dq_show_result_all;
#endif


  mde->fdq_init = find_dq_init_all;

  // DDR frequency calcuation
  mde->ddr_frequency = mde_do_ana_pll_calc ( mde );
}
