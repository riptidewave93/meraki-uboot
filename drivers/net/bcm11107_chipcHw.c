/*****************************************************************************
* Copyright 2003 - 2008 Broadcom Corporation.  All rights reserved.
*
* Unless you and Broadcom execute a separate written software license
* agreement governing use of this software, this software is licensed to you
* under the terms of the GNU General Public License version 2, available at
* http://www.broadcom.com/licenses/GPLv2.php (the "GPL").
*
* Notwithstanding the above, under no circumstances may you combine this
* software in any way with any other Broadcom software provided under a
* license other than the GPL, without Broadcom's express prior written
* consent.
*****************************************************************************/


/****************************************************************************/
/**
*  @file    chipcHw.c
*
*  @brief   Low level Various CHIP clock controlling routines
*
*  @note
*
*   These routines provide basic clock controlling functionality only.
*/
/****************************************************************************/

/* ---- Include Files ---------------------------------------------------- */

//#include <csp/errno.h>
//#include <csp/stdint.h>
//#include <csp/module.h>

#include <asm/arch/csp/chipcHw_def.h>
#include <asm/arch/csp/chipcHw_inline.h>

#include <asm/arch/csp/reg.h>
#include <asm/arch/csp/delay.h>

/* ---- Private Constants and Types --------------------------------------- */

/* VPM alignment algorithm uses this */
#define MAX_PHASE_ADJUST_COUNT         0xFFFF         /* Max number of times allowed to adjust the phase */
#define MAX_PHASE_ALIGN_ATTEMPTS       10             /* Max number of attempt to align the phase */

/* Local definition of clock type */
#define PLL_CLOCK                      1              /* PLL Clock */
#define NON_PLL_CLOCK                  2              /* Divider clock */

/****************************************************************************/
/**
*  @brief   Set clock fequency for miscellaneous configurable clocks
*
*  This function sets clock frequency
*
*  @return  Configured clock frequency in hertz
*
*/
/****************************************************************************/
chipcHw_freq  chipcHw_getClockFrequency
(
    chipcHw_CLOCK_e clock         /*  [ IN ] Configurable clock */
)
{
   volatile uint32_t *pPLLReg = (uint32_t*)0x0;
   volatile uint32_t *pClockCtrl = (uint32_t*)0x0;
   volatile uint32_t *pDependentClock = (uint32_t*)0x0;
   uint32_t vcoFreqPll1Hz = 0;    /* Effective VCO frequency for PLL1 in Hz */
   uint32_t vcoFreqPll2Hz = 0;    /* Effective VCO frequency for PLL2 in Hz */
   uint32_t dependentClockType = 0;
   uint32_t vcoHz = 0;

   /* Get VCO frequencies */
   if ( ( pChipcHw->PLLPreDivider & chipcHw_REG_PLL_PREDIVIDER_NDIV_MODE_MASK ) != chipcHw_REG_PLL_PREDIVIDER_NDIV_MODE_INTEGER )
   {
      vcoFreqPll1Hz = chipcHw_XTAL_FREQ_Hz * chipcHw_divide( chipcHw_REG_PLL_PREDIVIDER_P2, chipcHw_REG_PLL_PREDIVIDER_P1 ) *
                      ( ( ( reg32_read( &pChipcHw->PLLPreDivider ) & chipcHw_REG_PLL_PREDIVIDER_NDIV_MASK ) >> chipcHw_REG_PLL_PREDIVIDER_NDIV_SHIFT ) + 1);
   }
   else
   {
      vcoFreqPll1Hz = chipcHw_XTAL_FREQ_Hz * chipcHw_divide( chipcHw_REG_PLL_PREDIVIDER_P2, chipcHw_REG_PLL_PREDIVIDER_P1 ) *
                      ( ( reg32_read( &pChipcHw->PLLPreDivider ) & chipcHw_REG_PLL_PREDIVIDER_NDIV_MASK ) >> chipcHw_REG_PLL_PREDIVIDER_NDIV_SHIFT );
   }

   vcoFreqPll2Hz = chipcHw_XTAL_FREQ_Hz * chipcHw_divide( chipcHw_REG_PLL_PREDIVIDER_P2, chipcHw_REG_PLL_PREDIVIDER_P1 ) *
                   ( ( reg32_read( &pChipcHw->PLLPreDivider2 ) & chipcHw_REG_PLL_PREDIVIDER_NDIV_MASK ) >> chipcHw_REG_PLL_PREDIVIDER_NDIV_SHIFT );

   switch ( clock )
   {
      case chipcHw_CLOCK_DDR:
         pPLLReg = &pChipcHw->DDRClock;
         vcoHz = vcoFreqPll1Hz;
         break;
      case chipcHw_CLOCK_ARM:
         pPLLReg = &pChipcHw->ARMClock;
         vcoHz = vcoFreqPll1Hz;
         break;
      case chipcHw_CLOCK_ESW:
         pPLLReg = &pChipcHw->ESWClock;
         vcoHz = vcoFreqPll1Hz;
         break;
      case chipcHw_CLOCK_VPM:
         pPLLReg = &pChipcHw->VPMClock;
         vcoHz = vcoFreqPll1Hz;
         break;
     case chipcHw_CLOCK_ESW125:
         pPLLReg = &pChipcHw->ESW125Clock;
         vcoHz = vcoFreqPll1Hz;
         break;
      case chipcHw_CLOCK_UART:
         pPLLReg = &pChipcHw->UARTClock;
         vcoHz = vcoFreqPll1Hz;
         break;
      case chipcHw_CLOCK_SDIO0:
         pPLLReg = &pChipcHw->SDIO0Clock;
         vcoHz = vcoFreqPll1Hz;
         break;
      case chipcHw_CLOCK_SDIO1:
         pPLLReg = &pChipcHw->SDIO1Clock;
         vcoHz = vcoFreqPll1Hz;
         break;
      case chipcHw_CLOCK_SPI:
         pPLLReg = &pChipcHw->SPIClock;
         vcoHz = vcoFreqPll1Hz;
         break;
      case chipcHw_CLOCK_ETM:
         pPLLReg = &pChipcHw->ETMClock;
         vcoHz = vcoFreqPll1Hz;
         break;
      case chipcHw_CLOCK_USB:
         pPLLReg = &pChipcHw->USBClock;
         vcoHz = vcoFreqPll2Hz;
         break;
      case chipcHw_CLOCK_LCD:
         pPLLReg = &pChipcHw->LCDClock;
         vcoHz = vcoFreqPll2Hz;
         break;
      case chipcHw_CLOCK_APM:
         pPLLReg = &pChipcHw->APMClock;
         vcoHz = vcoFreqPll2Hz;
         break;
      case chipcHw_CLOCK_BUS:
         pClockCtrl = &pChipcHw->ACLKClock;
         pDependentClock = &pChipcHw->ARMClock;
         vcoHz = vcoFreqPll1Hz;
         dependentClockType = PLL_CLOCK;
         break;
      case chipcHw_CLOCK_OTP:
         pClockCtrl = &pChipcHw->OTPClock;
         break;
      case chipcHw_CLOCK_I2C:
         pClockCtrl = &pChipcHw->I2CClock;
         break;
      case chipcHw_CLOCK_I2S0:
         pClockCtrl = &pChipcHw->I2S0Clock;
         break;
      case chipcHw_CLOCK_RTBUS:
         pClockCtrl = &pChipcHw->RTBUSClock;
         pDependentClock = &pChipcHw->ACLKClock;
         dependentClockType = NON_PLL_CLOCK;
         break;
      case chipcHw_CLOCK_APM100:
         pClockCtrl = &pChipcHw->APM100Clock;
         pDependentClock = &pChipcHw->APMClock;
         vcoHz = vcoFreqPll2Hz;
         dependentClockType = PLL_CLOCK;
         break;
      case chipcHw_CLOCK_TSC:
         pClockCtrl = &pChipcHw->TSCClock;
         break;
      case chipcHw_CLOCK_LED:
         pClockCtrl = &pChipcHw->LEDClock;
         break;
      case chipcHw_CLOCK_I2S1:
         pClockCtrl = &pChipcHw->I2S1Clock;
         break;
   }

   if ( pPLLReg )
   {
      /* Obtain PLL clock frequency */
      if ( reg32_read( pPLLReg ) & chipcHw_REG_PLL_CLOCK_BYPASS_SELECT )
      {
         /* Return crystal clock frequency when bypassed */
         return chipcHw_XTAL_FREQ_Hz;
      }
      else if ( clock == chipcHw_CLOCK_DDR )
      {
         /* DDR frequency is configured in PLLDivider register */
         return  ( chipcHw_divide( vcoHz, ( ( ( reg32_read( &pChipcHw->PLLDivider ) & 0xFF000000 ) >> 24 ) ? ( ( reg32_read( &pChipcHw->PLLDivider ) & 0xFF000000 ) >> 24 ) : 256 ) ));
      }
      else
      {
         /* From chip revision number B0, LCD clock is internally divided by 2 */
         if ( pPLLReg == &pChipcHw->LCDClock )
         {
            vcoHz >>= 1;
         }
         /* Obtain PLL clock frequency using VCO dividers */
         return chipcHw_divide( vcoHz, ( ( reg32_read( pPLLReg ) & chipcHw_REG_PLL_CLOCK_MDIV_MASK ) ?  ( reg32_read( pPLLReg ) & chipcHw_REG_PLL_CLOCK_MDIV_MASK ) : 256 ) );
      }
   }
   else if ( pClockCtrl )
   {
      /* Obtain divider clock frequency */
      uint32_t div;
      uint32_t freq = 0;

      if ( reg32_read( pClockCtrl ) & chipcHw_REG_DIV_CLOCK_BYPASS_SELECT )
      {
         /* Return crystal clock frequency when bypassed */
         return chipcHw_XTAL_FREQ_Hz;
      }
      else if ( pDependentClock )
      {
         /* Identify the dependent clock frequency */
         switch ( dependentClockType )
         {
         case PLL_CLOCK:
            if ( reg32_read( pDependentClock ) & chipcHw_REG_PLL_CLOCK_BYPASS_SELECT )
            {
               /* Use crystal clock frequency when dependent PLL clock is bypassed */
               freq = chipcHw_XTAL_FREQ_Hz;
            }
            else
            {
               /* Obtain PLL clock frequency using VCO dividers */
              div = reg32_read( pDependentClock ) & chipcHw_REG_PLL_CLOCK_MDIV_MASK;
              freq = div ? chipcHw_divide( vcoHz, div ) : 0;
            }
            break;
         case NON_PLL_CLOCK:
            if ( pDependentClock == (uint32_t*)&pChipcHw->ACLKClock )
            {
               freq = chipcHw_getClockFrequency ( chipcHw_CLOCK_BUS );
            }
            else
            {
               if ( reg32_read( pDependentClock ) & chipcHw_REG_DIV_CLOCK_BYPASS_SELECT )
               {
                  /* Use crystal clock frequency when dependent divider clock is bypassed */
                  freq = chipcHw_XTAL_FREQ_Hz;
               }
               else
               {
                  /* Obtain divider clock frequency using XTAL dividers */
                  div = reg32_read( pDependentClock ) & chipcHw_REG_DIV_CLOCK_DIV_MASK;
                  freq = chipcHw_divide( chipcHw_XTAL_FREQ_Hz, ( div ? div : 256 ));
               }
            }
            break;
         }
      }
      else
      {
         /* Dependent on crystal clock */
         freq = chipcHw_XTAL_FREQ_Hz;
      }

      /* Find the divisor */
      if ( pClockCtrl == &pChipcHw->ACLKClock )
      {
         div = reg32_read( pClockCtrl ) & chipcHw_REG_ACLKClock_CLK_DIV_MASK;
         div = div ? div : chipcHw_REG_ACLKClock_MAX_DIV;
      }
      else
      {
         div = reg32_read( pClockCtrl ) & chipcHw_REG_DIV_CLOCK_DIV_MASK;
         /* divisor 256 is encoded as 0 */
         div = div ? div : 256;
      }

      return chipcHw_divide(freq , div);
   }
   return 0;
}

/****************************************************************************/
/**
*  @brief   Set clock fequency for miscellaneous configurable clocks
*
*  This function sets clock frequency
*
*  @return  Configured clock frequency in Hz
*
*/
/****************************************************************************/
chipcHw_freq  chipcHw_setClockFrequency
(
   chipcHw_CLOCK_e clock,          /*  [ IN ] Configurable clock */
   uint32_t freq                   /*  [ IN ] Clock frequency in Hz */
)
{
   volatile uint32_t *pPLLReg = (uint32_t*)0x0;
   volatile uint32_t *pClockCtrl = (uint32_t*)0x0;
   volatile uint32_t *pDependentClock = (uint32_t*)0x0;
   uint32_t vcoFreqPll1Hz = 0;    /* Effective VCO frequency for PLL1 in Hz */
   uint32_t vcoFreqPll2Hz = 0;    /* Effective VCO frequency for PLL2 in Hz */
   uint32_t dependentClockType = 0;
   uint32_t vcoHz = 0;

   /* Get VCO frequencies */
   if ( ( pChipcHw->PLLPreDivider & chipcHw_REG_PLL_PREDIVIDER_NDIV_MODE_MASK ) != chipcHw_REG_PLL_PREDIVIDER_NDIV_MODE_INTEGER )
   {
      vcoFreqPll1Hz = chipcHw_XTAL_FREQ_Hz * chipcHw_divide( chipcHw_REG_PLL_PREDIVIDER_P2, chipcHw_REG_PLL_PREDIVIDER_P1 ) *
                      ( ( ( reg32_read( &pChipcHw->PLLPreDivider ) & chipcHw_REG_PLL_PREDIVIDER_NDIV_MASK ) >> chipcHw_REG_PLL_PREDIVIDER_NDIV_SHIFT ) + 1);
   }
   else
   {
      vcoFreqPll1Hz = chipcHw_XTAL_FREQ_Hz * chipcHw_divide( chipcHw_REG_PLL_PREDIVIDER_P2, chipcHw_REG_PLL_PREDIVIDER_P1 ) *
                      ( ( reg32_read( &pChipcHw->PLLPreDivider ) & chipcHw_REG_PLL_PREDIVIDER_NDIV_MASK ) >> chipcHw_REG_PLL_PREDIVIDER_NDIV_SHIFT );
   }

   vcoFreqPll2Hz = chipcHw_XTAL_FREQ_Hz * chipcHw_divide( chipcHw_REG_PLL_PREDIVIDER_P2, chipcHw_REG_PLL_PREDIVIDER_P1 ) *
                   ( ( reg32_read( &pChipcHw->PLLPreDivider2 ) & chipcHw_REG_PLL_PREDIVIDER_NDIV_MASK ) >> chipcHw_REG_PLL_PREDIVIDER_NDIV_SHIFT );

   switch ( clock )
   {
      case chipcHw_CLOCK_DDR:
         /* Configure the DDR_ctrl:BUS ratio settings */
         /* Dvide DDR_phy by two to obtain DDR_ctrl clock */
         reg32_write_masked ( &pChipcHw->DDRClock,
                              chipcHw_REG_PLL_CLOCK_TO_BUS_RATIO_MASK,
                              ( ( chipcHw_divide( freq >> 1,  chipcHw_getClockFrequency ( chipcHw_CLOCK_BUS ) ) - 1) << chipcHw_REG_PLL_CLOCK_TO_BUS_RATIO_SHIFT ) );
         pPLLReg = &pChipcHw->DDRClock;
         vcoHz = vcoFreqPll1Hz;
         break;
      case chipcHw_CLOCK_ARM:
         pPLLReg = &pChipcHw->ARMClock;
         vcoHz = vcoFreqPll1Hz;
         break;
      case chipcHw_CLOCK_ESW:
         pPLLReg = &pChipcHw->ESWClock;
         vcoHz = vcoFreqPll1Hz;
         break;
      case chipcHw_CLOCK_VPM:
         /* Configure the VPM:BUS ratio settings */
         reg32_write_masked ( &pChipcHw->VPMClock,
                              chipcHw_REG_PLL_CLOCK_TO_BUS_RATIO_MASK,
                              ( ( chipcHw_divide( freq, chipcHw_getClockFrequency ( chipcHw_CLOCK_BUS ) ) - 1) << chipcHw_REG_PLL_CLOCK_TO_BUS_RATIO_SHIFT ) );
         pPLLReg = &pChipcHw->VPMClock;
         vcoHz = vcoFreqPll1Hz;
         break;
     case chipcHw_CLOCK_ESW125:
         pPLLReg = &pChipcHw->ESW125Clock;
         vcoHz = vcoFreqPll1Hz;
         break;
      case chipcHw_CLOCK_UART:
         pPLLReg = &pChipcHw->UARTClock;
         vcoHz = vcoFreqPll1Hz;
         break;
      case chipcHw_CLOCK_SDIO0:
         pPLLReg = &pChipcHw->SDIO0Clock;
         vcoHz = vcoFreqPll1Hz;
         break;
      case chipcHw_CLOCK_SDIO1:
         pPLLReg = &pChipcHw->SDIO1Clock;
         vcoHz = vcoFreqPll1Hz;
         break;
      case chipcHw_CLOCK_SPI:
         pPLLReg = &pChipcHw->SPIClock;
         vcoHz = vcoFreqPll1Hz;
         break;
      case chipcHw_CLOCK_ETM:
         pPLLReg = &pChipcHw->ETMClock;
         vcoHz = vcoFreqPll1Hz;
         break;
      case chipcHw_CLOCK_USB:
         pPLLReg = &pChipcHw->USBClock;
         vcoHz = vcoFreqPll2Hz;
         break;
      case chipcHw_CLOCK_LCD:
         pPLLReg = &pChipcHw->LCDClock;
         vcoHz = vcoFreqPll2Hz;
         break;
      case chipcHw_CLOCK_APM:
         pPLLReg = &pChipcHw->APMClock;
         vcoHz = vcoFreqPll2Hz;
         break;
      case chipcHw_CLOCK_BUS:
         pClockCtrl = &pChipcHw->ACLKClock;
         pDependentClock = &pChipcHw->ARMClock;
         vcoHz = vcoFreqPll1Hz;
         dependentClockType = PLL_CLOCK;
         break;
      case chipcHw_CLOCK_OTP:
         pClockCtrl = &pChipcHw->OTPClock;
         break;
      case chipcHw_CLOCK_I2C:
         pClockCtrl = &pChipcHw->I2CClock;
         break;
      case chipcHw_CLOCK_I2S0:
         pClockCtrl = &pChipcHw->I2S0Clock;
         break;
      case chipcHw_CLOCK_RTBUS:
         pClockCtrl = &pChipcHw->RTBUSClock;
         pDependentClock = &pChipcHw->ACLKClock;
         dependentClockType = NON_PLL_CLOCK;
         vcoHz = vcoFreqPll1Hz;
         break;
      case chipcHw_CLOCK_APM100:
         pClockCtrl = &pChipcHw->APM100Clock;
         pDependentClock = &pChipcHw->APMClock;
         vcoHz = vcoFreqPll2Hz;
         dependentClockType = PLL_CLOCK;
         break;
      case chipcHw_CLOCK_TSC:
         pClockCtrl = &pChipcHw->TSCClock;
         break;
      case chipcHw_CLOCK_LED:
         pClockCtrl = &pChipcHw->LEDClock;
         break;
      case chipcHw_CLOCK_I2S1:
         pClockCtrl = &pChipcHw->I2S1Clock;
         break;
   }

   if ( pPLLReg )
   {
      /* Select XTAL as bypass source */
      reg32_clear_bits( pPLLReg, chipcHw_REG_PLL_CLOCK_SOURCE_GPIO);
      reg32_set_bits( pPLLReg, chipcHw_REG_PLL_CLOCK_BYPASS_SELECT);
      /* For DDR settings use only the PLL divider clock */
      if (pPLLReg == &pChipcHw->DDRClock)
      {
         /* Set M1DIV for PLL1, which controls the DDR clock */
         reg32_write_masked ( &pChipcHw->PLLDivider,
                              0xFF000000,
                              ( (chipcHw_REG_PLL_DIVIDER_MDIV ( vcoHz, freq ) )   << 24 ) );
         /* Calculate expected frequency */
         freq =  chipcHw_divide( vcoHz, ( ( ( pChipcHw->PLLDivider & 0xFF000000 ) >> 24 ) ? ( ( pChipcHw->PLLDivider & 0xFF000000 ) >> 24 ) : 256 ));
      }
      else
      {
         /* From chip revision number B0, LCD clock is internally divided by 2 */
         if ( pPLLReg == &pChipcHw->LCDClock )
         {
           vcoHz >>= 1;
         }
         /* Set MDIV to change the frequency */
         reg32_clear_bits( pPLLReg, chipcHw_REG_PLL_CLOCK_MDIV_MASK );
         reg32_set_bits( pPLLReg, chipcHw_REG_PLL_DIVIDER_MDIV ( vcoHz, freq ));
         /* Calculate expected frequency */
         freq = chipcHw_divide ( vcoHz, ( ( reg32_read( pPLLReg ) & chipcHw_REG_PLL_CLOCK_MDIV_MASK ) ? ( reg32_read( pPLLReg ) & chipcHw_REG_PLL_CLOCK_MDIV_MASK ) : 256 ));
      }
      /* Wait for for atleast 200ns as per the protocol to change frequency */
      udelay ( 1 );
      /* Do not bypass */
      reg32_clear_bits( pPLLReg, chipcHw_REG_PLL_CLOCK_BYPASS_SELECT );
      /* Return the configured frequency */
      return freq;
   }
   else if ( pClockCtrl )
   {
      uint32_t divider = 0;

      /* Divider clock should not be bypassed  */
      reg32_clear_bits( pClockCtrl, chipcHw_REG_DIV_CLOCK_BYPASS_SELECT );

      /* Identify the clock source */
      if ( pDependentClock )
      {
         switch ( dependentClockType )
         {
            case PLL_CLOCK:
               divider = chipcHw_divide( chipcHw_divide( vcoHz, ( reg32_read( pDependentClock ) & chipcHw_REG_PLL_CLOCK_MDIV_MASK ) ), freq);
               break;
            case NON_PLL_CLOCK:
               {
                  uint32_t sourceClock = 0;

                  if ( pDependentClock == (uint32_t*)&pChipcHw->ACLKClock )
                  {
		     /* Actual source (BUS) clock */
                     sourceClock = chipcHw_getClockFrequency ( chipcHw_CLOCK_BUS );

#if ( chipcHw_REG_PLL_DIVIDER_NDIV_f_SS < chipcHw_REG_PLL_DIVIDER_FRAC )
		     /* Adjusted source (BUS) clock  */
		     sourceClock = chipcHw_divide ( vcoHz, chipcHw_divide ( vcoHz, sourceClock ));
#endif
                  }
                  else
                  {
                     uint32_t div = reg32_read( pDependentClock ) & chipcHw_REG_DIV_CLOCK_DIV_MASK;
                     sourceClock = chipcHw_divide( chipcHw_XTAL_FREQ_Hz, ( ( div ) ? div : 256 ));
                  }
                  divider = chipcHw_divide( sourceClock, freq);
               }
               break;
         }
      }
      else
      {
         divider = chipcHw_divide( chipcHw_XTAL_FREQ_Hz, freq);
      }

      if ( divider )
      {
         /* Set the divider to obtain the required frequency */
         if ( pClockCtrl == &pChipcHw->ACLKClock )
         {
            reg32_write_masked( pClockCtrl,
                                chipcHw_REG_ACLKClock_CLK_DIV_MASK,
                                ( ( ( divider > (chipcHw_REG_ACLKClock_MAX_DIV - 1) ) ? 0 : divider ) & chipcHw_REG_ACLKClock_CLK_DIV_MASK ) );
         }
         else
         {
            reg32_write_masked( pClockCtrl,
                                chipcHw_REG_DIV_CLOCK_DIV_MASK,
                                ( ( ( divider > 256 ) ? chipcHw_REG_DIV_CLOCK_DIV_256 : divider ) & chipcHw_REG_DIV_CLOCK_DIV_MASK ) );
         }
         return freq;
      }
   }

   return 0;
}

/****************************************************************************/
/**
*  @brief   Local Divide function
*
*  This function does the divide
*
*  @return divide value
*
*/
/****************************************************************************/
uint32_t chipcHw_divide(uint32_t num, uint32_t denom)
{
  uint32_t r;
  uint32_t t=1;

  /* Skip all the calculations */
  if ( denom == 1 )
  {
     return num;
  }
  /* Shift denom and t up to the largest value to optimize algorithm */
  /* t contains the units of each divide */
  while( (denom & 0x40000000) == 0 ) /* fails if denom=0 */
  {
     denom =denom << 1;
     t = t << 1;
  }

  /* Intialize the result */
  r=0;

  do
  {
     /* Determine if there exists a positive remainder */
     if( ((int64_t)num-(int64_t)denom) >= 0 )
     {
        /* Accumlate t to the result and calculate a new remainder */
        num = num - denom;
        r = r + t;
     }
     /* Continue to shift denom and shift t down to 0 */
     denom = denom >> 1;
     t = t >> 1;
  } while ( t!=0 );

  return r;
}
