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
#include "../../arch/arm/include/asm/bitops.h"

#include <iproc_pwmc.h>



#define PWM_PRESCALER_MAX    63 /* 6 bits field */


#define PWM_CTL_ENABLE_SHIFT        (0)
#define PWM_CTL_POLARITY_SHIFT      (8)

#define PWM_PRESCALE_PWM3_PRESCALE_SHIFT         (0)
#define PWM_PRESCALE_PWM3_PRESCALE_MASK          (0x0000003F)
#define PWM_PRESCALE_PWM2_PRESCALE_SHIFT         (6)
#define PWM_PRESCALE_PWM2_PRESCALE_MASK          (0x00000FC0)
#define PWM_PRESCALE_PWM1_PRESCALE_SHIFT         (12)
#define PWM_PRESCALE_PWM1_PRESCALE_MASK          (0x0003F000)
#define PWM_PRESCALE_PWM0_PRESCALE_SHIFT         (18)
#define PWM_PRESCALE_PWM0_PRESCALE_MASK          (0x00FC0000)

#define PWM_DUTY_HI_CNT0_SHIFT      (0)
#define PWM_DUTY_HI_CNT0_MASK       (0xFFFF)
#define PWM_DUTY_HI_CNT1_SHIFT      (0)
#define PWM_DUTY_HI_CNT1_MASK       (0xFFFF)
#define PWM_DUTY_HI_CNT2_SHIFT      (0)
#define PWM_DUTY_HI_CNT2_MASK       (0xFFFF)
#define PWM_DUTY_HI_CNT3_SHIFT      (0)
#define PWM_DUTY_HI_CNT3_MASK       (0xFFFF)

#define PWM_PERIOD_CNT0_SHIFT      (0)
#define PWM_PERIOD_CNT0_MASK       (0xFFFF)
#define PWM_PERIOD_CNT1_SHIFT      (0)
#define PWM_PERIOD_CNT1_MASK       (0xFFFF)
#define PWM_PERIOD_CNT2_SHIFT      (0)
#define PWM_PERIOD_CNT2_MASK       (0xFFFF)
#define PWM_PERIOD_CNT3_SHIFT      (0)
#define PWM_PERIOD_CNT3_MASK       (0xFFFF)


struct pwm_reg_def {
    u32 mask ;
    u32 shift ;
    u32 offset ;
} ;

#define PWM_REG_DEF(c, m, s, a) \
    [c] = { 	\
        .mask 		= 	m,  \
        .shift 		= 	s, \
        .offset 	= 	a \
    }

static const 
struct pwm_reg_def pwm_chan_pre_scaler_info[IPROC_PWM_CHANNEL_CNT] = { 
    PWM_REG_DEF(0, PWM_PRESCALE_PWM0_PRESCALE_MASK, 
        PWM_PRESCALE_PWM0_PRESCALE_SHIFT, CCB_PWM_PRESCALE_BASE),
    PWM_REG_DEF(1, PWM_PRESCALE_PWM1_PRESCALE_MASK, 
        PWM_PRESCALE_PWM1_PRESCALE_SHIFT, CCB_PWM_PRESCALE_BASE),
    PWM_REG_DEF(2, PWM_PRESCALE_PWM2_PRESCALE_MASK, 
        PWM_PRESCALE_PWM2_PRESCALE_SHIFT, CCB_PWM_PRESCALE_BASE),
    PWM_REG_DEF(3, PWM_PRESCALE_PWM3_PRESCALE_MASK, 
        PWM_PRESCALE_PWM3_PRESCALE_SHIFT, CCB_PWM_PRESCALE_BASE),
} ;

static const 
struct pwm_reg_def pwm_chan_period_cnt_info[IPROC_PWM_CHANNEL_CNT] = { 
    PWM_REG_DEF(0, PWM_PERIOD_CNT0_MASK, 
        PWM_PERIOD_CNT0_SHIFT, CCB_PWM_PERIOD_COUNT0_BASE),
    PWM_REG_DEF(1, PWM_PERIOD_CNT1_MASK, 
        PWM_PERIOD_CNT1_SHIFT, CCB_PWM_PERIOD_COUNT1_BASE),
    PWM_REG_DEF(2, PWM_PERIOD_CNT2_MASK, 
        PWM_PERIOD_CNT2_SHIFT, CCB_PWM_PERIOD_COUNT2_BASE),
    PWM_REG_DEF(3, PWM_PERIOD_CNT3_MASK, 
        PWM_PERIOD_CNT3_SHIFT, CCB_PWM_PERIOD_COUNT3_BASE),
} ;

static const 
struct pwm_reg_def pwm_chan_duty_cycle_info[IPROC_PWM_CHANNEL_CNT] = { 
    PWM_REG_DEF(0, PWM_DUTY_HI_CNT0_MASK, 
        PWM_DUTY_HI_CNT0_SHIFT, CCB_PWM_DUTY_HI_COUNT0_BASE),
    PWM_REG_DEF(1, PWM_DUTY_HI_CNT1_MASK, 
        PWM_DUTY_HI_CNT1_SHIFT, CCB_PWM_DUTY_HI_COUNT1_BASE),
    PWM_REG_DEF(2, PWM_DUTY_HI_CNT2_MASK, 
        PWM_DUTY_HI_CNT2_SHIFT, CCB_PWM_DUTY_HI_COUNT2_BASE),
    PWM_REG_DEF(3, PWM_DUTY_HI_CNT3_MASK, 
        PWM_DUTY_HI_CNT3_SHIFT, CCB_PWM_DUTY_HI_COUNT3_BASE),
} ;


static iproc_pwmc iprocDev;

#define readl(addr) (*(volatile unsigned int*)(addr))
#define writel(b,addr) ((*(volatile unsigned int *) (addr)) = (b))
#define clear_bit(b, addr) ((*(volatile unsigned int *) (addr)) &= (~(1 << b )))
#define set_bit(b, addr) ((*(volatile unsigned int *) (addr)) |= (1 << b))



static void iproc_pwmc_clear_set_bit(const iproc_pwmc *ap, unsigned int offset,
    unsigned int shift, unsigned char en_dis)
{
    unsigned long val = readl(ap->iobase + offset ) ;

    // Clear bit.
    clear_bit(shift,&val) ;
    if ( en_dis == 1 ) 
        set_bit(shift,&val);

    writel(val, (ap->iobase + offset ));
}


static void iproc_pwmc_set_field(const iproc_pwmc *ap, unsigned int offset,
    unsigned int mask, unsigned int shift, unsigned int wval)
{
    unsigned int val = readl(ap->iobase + offset ) ;

    val = (val & ~mask) | ( wval << shift ) ;
    writel(val, (ap->iobase + offset ));
}

static void iproc_pwmc_get_field(const iproc_pwmc *ap, unsigned int offset,
    unsigned int mask, unsigned int shift, unsigned int *val)
{
    *val = readl(ap->iobase + offset ) ;
    *val = ( *val & mask ) >> shift ; 
}


void iproc_pwmc_start(const iproc_pwmc *ap, int chan)
{

    iproc_pwmc_clear_set_bit(ap, CCB_PWM_CTL_BASE, 
        (PWM_CTL_ENABLE_SHIFT + chan), 1) ;
}

void iproc_pwmc_stop(const iproc_pwmc *ap, int chan)
{

    iproc_pwmc_clear_set_bit(ap, CCB_PWM_CTL_BASE, 
        (PWM_CTL_ENABLE_SHIFT + chan), 0) ;


}
void iproc_pwmc_config_polarity(iproc_pwmc *ap, int chan,
					 pwm_config *c)
{
    iproc_pwm_device *p = &(ap->p[chan]);

    if ( c->polarity ){
        iproc_pwmc_clear_set_bit(ap, CCB_PWM_CTL_BASE, 
            (PWM_CTL_POLARITY_SHIFT + chan), 1) ;
    }else{ 
        iproc_pwmc_clear_set_bit(ap, CCB_PWM_CTL_BASE, 
            (PWM_CTL_POLARITY_SHIFT + chan), 0) ;
    }
    p->polarity = c->polarity ? 1 : 0;
}


static void iproc_pwmc_config_duty_ticks(iproc_pwmc *ap, int chan,
					   pwm_config *c)
{
    iproc_pwm_device *p = &(ap->p[chan]);
    unsigned int pre_scaler = 0 ;
    unsigned int duty_cnt = 0 ;

    iproc_pwmc_get_field(ap, pwm_chan_pre_scaler_info[chan].offset, 
        pwm_chan_pre_scaler_info[chan].mask, pwm_chan_pre_scaler_info[chan].shift, 
        &pre_scaler) ;

    /* Read prescaler value from register. */
    duty_cnt = c->duty_ticks / (pre_scaler + 1) ; 

    /* program duty cycle. */
    iproc_pwmc_set_field(ap, pwm_chan_duty_cycle_info[chan].offset, 
                    pwm_chan_duty_cycle_info[chan].mask, 
                    pwm_chan_duty_cycle_info[chan].shift, duty_cnt) ;

    /* disable channel */
    iproc_pwmc_stop(ap, chan) ;

    /* enable channel. */
    iproc_pwmc_start(ap, chan) ;

    p->duty_ticks = c->duty_ticks;

}

static int iproc_pwmc_config_period_ticks(iproc_pwmc *ap, int chan,
					    pwm_config *c)
{
    unsigned int pcnt ;
    unsigned char pre_scaler = 0 ;
    iproc_pwm_device *p = &(ap->p[chan]);
    
    pre_scaler = c->period_ticks / 0xFFFF ;
	if ( pre_scaler > PWM_PRESCALER_MAX ) 
        pre_scaler = PWM_PRESCALER_MAX ;

    pcnt = c->period_ticks / (pre_scaler + 1) ;

    /* programe prescaler */
    iproc_pwmc_set_field(ap, pwm_chan_pre_scaler_info[chan].offset, 
                        pwm_chan_pre_scaler_info[chan].mask, 
                        pwm_chan_pre_scaler_info[chan].shift, pre_scaler) ;

    /* program period count. */
    iproc_pwmc_set_field(ap, pwm_chan_period_cnt_info[chan].offset, 
                       pwm_chan_period_cnt_info[chan].mask, 
                       pwm_chan_period_cnt_info[chan].shift, pcnt) ;

    /* disable channel */
    iproc_pwmc_stop(ap, chan) ;

    /* enable channel. */
    iproc_pwmc_start(ap, chan) ;

    p->period_ticks = c->period_ticks;

    return 0;
}

int iproc_pwmc_config(iproc_pwmc *ap, int chan, pwm_config *c)
{
     int ret;

    if (test_bit(PWM_CONFIG_PERIOD_TICKS, &c->config_mask)) {
        ret = iproc_pwmc_config_period_ticks(ap, chan, c);
        if (ret)
            return ret;
        if (test_bit(PWM_CONFIG_DUTY_TICKS, &c->config_mask)) {
            iproc_pwmc_config_duty_ticks(ap, chan, c);
        }
    }

    if (test_bit(PWM_CONFIG_DUTY_TICKS, &c->config_mask))
        iproc_pwmc_config_duty_ticks(ap, chan, c);

    if (test_bit(PWM_CONFIG_POLARITY, &c->config_mask))
        iproc_pwmc_config_polarity(ap, chan, c);    

    if (test_bit(PWM_CONFIG_START, &c->config_mask)
        || (!test_bit(PWM_CONFIG_STOP, &c->config_mask))) {        
        iproc_pwmc_start(ap, chan);
    }
    if (test_bit(PWM_CONFIG_STOP, &c->config_mask))
        iproc_pwmc_stop(ap, chan);

    return 0;
}

void iproc_pwmc_init(void )
{
	int i;
	iprocDev.iobase = (void*)CCB_PWM_CTL;
	for ( i = CCB_PWM_CTL; i<=CCB_PWM_PRESCALE; i+=4)
	{
		writel(0x0,i );
	}

}
void iproc_pwmc_deinit(void)
{
	int i;

	for ( i = CCB_PWM_CTL; i<=CCB_PWM_PRESCALE; i+=4)
	{
		writel(0x0,i );
	}

}

iproc_pwmc *iproc_pwmc_getDeviceHandle(void)
{
	return ( &iprocDev );
}
