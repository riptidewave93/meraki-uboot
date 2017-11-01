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

#ifndef __IPROC_PWMC_H
#define __IPROC_PWMC_H
#if defined(CONFIG_NS_PLUS)
#define  CCB_PWM_CTL                                                  0x18031000
#define  CCB_PWM_CTL_BASE                                                  0x000
#define  CCB_PWM_CTL_DATAMASK                                         0x00078fff
#define  CCB_PWM_CTL_RDWRMASK                                         0xfff87000
#define  CCB_PWM_PERIOD_COUNT0                                        0x18031004
#define  CCB_PWM_PERIOD_COUNT0_BASE                                        0x004
#define  CCB_PWM_PERIOD_COUNT0_DATAMASK                               0xffffffff
#define  CCB_PWM_PERIOD_COUNT0_RDWRMASK                               0x00000000
#define  CCB_PWM_DUTY_HI_COUNT0                                       0x18031008
#define  CCB_PWM_DUTY_HI_COUNT0_BASE                                       0x008
#define  CCB_PWM_DUTY_HI_COUNT0_DATAMASK                              0xffffffff
#define  CCB_PWM_DUTY_HI_COUNT0_RDWRMASK                              0x00000000
#define  CCB_PWM_PERIOD_COUNT1                                        0x1803100c
#define  CCB_PWM_PERIOD_COUNT1_BASE                                        0x00c
#define  CCB_PWM_PERIOD_COUNT1_DATAMASK                               0xffffffff
#define  CCB_PWM_PERIOD_COUNT1_RDWRMASK                               0x00000000
#define  CCB_PWM_DUTY_HI_COUNT1                                       0x18031010
#define  CCB_PWM_DUTY_HI_COUNT1_BASE                                       0x010
#define  CCB_PWM_DUTY_HI_COUNT1_DATAMASK                              0xffffffff
#define  CCB_PWM_DUTY_HI_COUNT1_RDWRMASK                              0x00000000
#define  CCB_PWM_PERIOD_COUNT2                                        0x18031014
#define  CCB_PWM_PERIOD_COUNT2_BASE                                        0x014
#define  CCB_PWM_PERIOD_COUNT2_DATAMASK                               0xffffffff
#define  CCB_PWM_PERIOD_COUNT2_RDWRMASK                               0x00000000
#define  CCB_PWM_DUTY_HI_COUNT2                                       0x18031018
#define  CCB_PWM_DUTY_HI_COUNT2_BASE                                       0x018
#define  CCB_PWM_DUTY_HI_COUNT2_DATAMASK                              0xffffffff
#define  CCB_PWM_DUTY_HI_COUNT2_RDWRMASK                              0x00000000
#define  CCB_PWM_PERIOD_COUNT3                                        0x1803101c
#define  CCB_PWM_PERIOD_COUNT3_BASE                                        0x01c
#define  CCB_PWM_PERIOD_COUNT3_DATAMASK                               0xffffffff
#define  CCB_PWM_PERIOD_COUNT3_RDWRMASK                               0x00000000
#define  CCB_PWM_DUTY_HI_COUNT3                                       0x18031020
#define  CCB_PWM_DUTY_HI_COUNT3_BASE                                       0x020
#define  CCB_PWM_DUTY_HI_COUNT3_DATAMASK                              0xffffffff
#define  CCB_PWM_DUTY_HI_COUNT3_RDWRMASK                              0x00000000
#define  CCB_PWM_PRESCALE                                             0x18031024
#define  CCB_PWM_PRESCALE_BASE                                             0x024
#define  CCB_PWM_PRESCALE_DATAMASK                                    0xffffffff
#define  CCB_PWM_PRESCALE_RDWRMASK                                    0x00000000

#else

#define  CCB_PWM_CTL                                                  0x18002000
#define  CCB_PWM_CTL_BASE                                                  0x000
#define  CCB_PWM_CTL_DATAMASK                                         0x00078fff
#define  CCB_PWM_CTL_RDWRMASK                                         0xfff87000
#define  CCB_PWM_PERIOD_COUNT0                                        0x18002004
#define  CCB_PWM_PERIOD_COUNT0_BASE                                        0x004
#define  CCB_PWM_PERIOD_COUNT0_DATAMASK                               0xffffffff
#define  CCB_PWM_PERIOD_COUNT0_RDWRMASK                               0x00000000
#define  CCB_PWM_DUTY_HI_COUNT0                                       0x18002008
#define  CCB_PWM_DUTY_HI_COUNT0_BASE                                       0x008
#define  CCB_PWM_DUTY_HI_COUNT0_DATAMASK                              0xffffffff
#define  CCB_PWM_DUTY_HI_COUNT0_RDWRMASK                              0x00000000
#define  CCB_PWM_PERIOD_COUNT1                                        0x1800200c
#define  CCB_PWM_PERIOD_COUNT1_BASE                                        0x00c
#define  CCB_PWM_PERIOD_COUNT1_DATAMASK                               0xffffffff
#define  CCB_PWM_PERIOD_COUNT1_RDWRMASK                               0x00000000
#define  CCB_PWM_DUTY_HI_COUNT1                                       0x18002010
#define  CCB_PWM_DUTY_HI_COUNT1_BASE                                       0x010
#define  CCB_PWM_DUTY_HI_COUNT1_DATAMASK                              0xffffffff
#define  CCB_PWM_DUTY_HI_COUNT1_RDWRMASK                              0x00000000
#define  CCB_PWM_PERIOD_COUNT2                                        0x18002014
#define  CCB_PWM_PERIOD_COUNT2_BASE                                        0x014
#define  CCB_PWM_PERIOD_COUNT2_DATAMASK                               0xffffffff
#define  CCB_PWM_PERIOD_COUNT2_RDWRMASK                               0x00000000
#define  CCB_PWM_DUTY_HI_COUNT2                                       0x18002018
#define  CCB_PWM_DUTY_HI_COUNT2_BASE                                       0x018
#define  CCB_PWM_DUTY_HI_COUNT2_DATAMASK                              0xffffffff
#define  CCB_PWM_DUTY_HI_COUNT2_RDWRMASK                              0x00000000
#define  CCB_PWM_PERIOD_COUNT3                                        0x1800201c
#define  CCB_PWM_PERIOD_COUNT3_BASE                                        0x01c
#define  CCB_PWM_PERIOD_COUNT3_DATAMASK                               0xffffffff
#define  CCB_PWM_PERIOD_COUNT3_RDWRMASK                               0x00000000
#define  CCB_PWM_DUTY_HI_COUNT3                                       0x18002020
#define  CCB_PWM_DUTY_HI_COUNT3_BASE                                       0x020
#define  CCB_PWM_DUTY_HI_COUNT3_DATAMASK                              0xffffffff
#define  CCB_PWM_DUTY_HI_COUNT3_RDWRMASK                              0x00000000
#define  CCB_PWM_PRESCALE                                             0x18002024
#define  CCB_PWM_PRESCALE_BASE                                             0x024
#define  CCB_PWM_PRESCALE_DATAMASK                                    0xffffffff
#define  CCB_PWM_PRESCALE_RDWRMASK                                    0x00000000

#endif


#define IPROC_PWM_CHANNEL_0     0
#define IPROC_PWM_CHANNEL_1     1
#define IPROC_PWM_CHANNEL_2     2
#define IPROC_PWM_CHANNEL_3     3
#define IPROC_PWM_CHANNEL_CNT   4

enum {
	PWM_CONFIG_DUTY_TICKS	= 0x1,
	PWM_CONFIG_PERIOD_TICKS	= 0x2,
	PWM_CONFIG_POLARITY	= 0x4,
	PWM_CONFIG_START	= 0x8,
	PWM_CONFIG_STOP		= 0x10,
};

/**
 * struct pwm_config - configuration data for a PWM device
 *
 * @config_mask: which fields are valid
 * @duty_ticks: requested duty cycle, in ticks
 * @period_ticks: requested period, in ticks
 * @polarity: active high (1), or active low (0)
 */
typedef struct spwm_config{
	unsigned long	config_mask;
	unsigned long	duty_ticks;
	unsigned long	period_ticks;
	int		        polarity;
}pwm_config;

typedef struct siproc_pwm_device{
	//struct device	dev;
	//const struct pwm_device_ops *ops;
	//const char	*label;
	//unsigned long	flags;
	unsigned long	tick_hz;
	int		polarity;
	unsigned long	period_ticks;
	unsigned long	duty_ticks;
}iproc_pwm_device;

typedef struct siproc_pwmc{
    iproc_pwm_device p[IPROC_PWM_CHANNEL_CNT];
    //struct pwm_device_ops ops;
    void   *iobase;
    struct clk *clk;
}iproc_pwmc;

int iproc_pwmc_config(iproc_pwmc *ap, int chan, pwm_config *c);
void iproc_pwmc_stop( const iproc_pwmc *ap, int chan);
void iproc_pwmc_start(const iproc_pwmc *ap, int chan);
iproc_pwmc *iproc_pwmc_getDeviceHandle(void);
void iproc_pwmc_init(void );
void iproc_pwmc_deinit(void);
#endif
