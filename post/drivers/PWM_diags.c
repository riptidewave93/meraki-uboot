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
#include <post.h>
#include <iproc_pwmc.h>

#if CONFIG_POST & CONFIG_SYS_POST_PWM
#define IPROC_PWM_CHANNEL_0     0
#define IPROC_PWM_CHANNEL_1     1
#define IPROC_PWM_CHANNEL_2     2
#define IPROC_PWM_CHANNEL_3     3

#define CH0     1
#define CH1     2
#define CH2     4
#define CH3     8
#define CH_ALL  0xF

#if defined(CONFIG_NS_PLUS)
#define REFCLK 25000000
#else
#define REFCLK 1000000
#endif

static pwm_config testPWMConfig;
static iproc_pwmc  *testPWCDev;

static void PWM_post_init( void )
{
	//This is an undocumented register in the register programmer's guide
	//To turn on AUX function
#if defined(CONFIG_NS_PLUS)
	*(volatile unsigned long*)0x18030028 |= 0xF;
#else
	*(volatile unsigned long*)0x18001028 |= 0xF;
#endif
	iproc_pwmc_init();
}
static void PWM_post_deinit( void )
{
	//This is an undocumented register in the register programmer's guide
	//To turn off AUX function
	*(volatile unsigned long*)0x18001028 &= ~0xF;
	iproc_pwmc_deinit();
}

static int PWM_post_configPWM(unsigned long dutyPercent, float freqInHz,
		int polarity, int ch  )
{
	int i;
	//Setup config packet to config duty cycle
	//freq and plority
	testPWMConfig.config_mask =	PWM_CONFIG_DUTY_TICKS |
								PWM_CONFIG_PERIOD_TICKS |
								PWM_CONFIG_POLARITY |
								PWM_CONFIG_START |
								PWM_CONFIG_STOP;



    if ( freqInHz > 500000 || freqInHz < 0.238 )
    {
    	post_log("\nRequested frequency is out of range !\n");
    	return -1;
    }
    if ( dutyPercent > 100)
    {
    	post_log("\nRequested frequency is out of range !\n");
    	return -1;
    }
	testPWMConfig.period_ticks = REFCLK / freqInHz;
	testPWMConfig.duty_ticks = testPWMConfig.period_ticks * ((float)dutyPercent/100);


	testPWMConfig.polarity = polarity;

	for ( i = 0; i <4; i++ )
	{
		if ( (1 << i) & ch )
		{
			iproc_pwmc_config(testPWCDev,  i ,&testPWMConfig);
		}
	}

	return 0;
}
static void PWM_post_startPWM(int ch)
{
	int i;
	for ( i = 0; i < 4; i++ )
	{
		if ( (1 << i) & ch )
		{
			iproc_pwmc_start(testPWCDev, i);
		}
	}
}
static void PWM_post_stopPWM(ch)
{
	int i;
	for ( i = 0; i < 4; i++ )
	{
		if ( (1 << i) & ch )
		{
			iproc_pwmc_stop(testPWCDev, i);
		}
	}
}
int PWM_post_test (int flags)
{
	int ret=0,i;
#if defined(CONFIG_NS_PLUS)

    if ( post_get_board_diags_type() & HR_ER_BOARDS )
	{
		printf("\nThis diag is not supported on this platform\n ");
		return 2;
	}
#endif


	post_log("\nPWM diags starts  !!!");
	post_log("\nPlease setup scope to measure duty cycles and frequency...");
	post_log("\nMake sure scope probe is connected to pins at J1801");
	post_log("\nMake sure SW1801 PWM all switches are set to OFF position");

	post_getUserResponse("\nReady? (Y/N)");

    if ( (flags & POST_SEMI_AUTO) !=  POST_SEMI_AUTO )
    {

        post_log("\nMake sure strap pin A4 is set to 0xF4");
        post_getConfirmation("\nReady? (Y/N)");

    }
    else
    {
        post_set_strappins("A2 0%A3 3%A4 f4%A5 38%A6 f4%");
    }





	//init
	PWM_post_init();
	//Get PWM handle
	testPWCDev = iproc_pwmc_getDeviceHandle();

	for ( i = 0; i < 4; i++ )
	{
		post_log("\nMake sure SW1801 PWM %d is set to ON position,all other pins are off", i);
		post_getUserResponse("\nReady? (Y/N)");
		post_log("\nGenerate signals to ch %d with duty cycle:50%, Freq 3000 Hz",i);
		post_log("\nYou should be hearing an audible tone also");
		//Config  duty cycle %, freq in Hz and Polarity
		PWM_post_configPWM( 50, 3000, 0,1<<i );
		//Turn on the channel
		PWM_post_startPWM(1<<i);
		ret |=post_getUserResponse("\nSignal generated correctly? (Y/N)");
		//Turn off the channel
		PWM_post_stopPWM(1<<i);
	}

	for ( i = 0; i < 4; i++ )
	{
		post_log("\nMake sure SW1801 PWM %d is set to ON position,all other pins are off", i);
		post_getUserResponse("\nReady? (Y/N)");
		post_log("\nGenerate signals to ch %d with duty cycle: 30%, Freq 30KHz",i);
		//Config  duty cycle %, freq in Hz and Polarity
		PWM_post_configPWM( 70, 30000, 0,1<<i );
		//Turn on the channel
		PWM_post_startPWM(1<<i );
		ret |=post_getUserResponse("\nSignal generated correctly? (Y/N)");

		//Turn off the channel
		PWM_post_stopPWM(1<<i );
	}

	for ( i = 0; i < 4; i++ )
	{
		post_log("\nMake sure SW1801 PWM %d is set to ON position,all other pins are off", i);
		post_getUserResponse("\nReady? (Y/N)");
		post_log("\nGenerate signals to ch %d with duty cycle: 80%, Freq 50KHz",i);
		//Config  duty cycle %, freq in Hz and Polarity
		PWM_post_configPWM( 80, 50000,1,1<<i );
		//Turn on the channel
		PWM_post_startPWM(1<<i );
		ret |= post_getUserResponse("\nSignal generated correctly? (Y/N)");

		//Turn off the channel
		PWM_post_stopPWM(1<<i );
	}

	if ( ret == 0 )
	{
		post_log("\nTests are successful ");
	}
	else
	{
		post_log("\Test failed");
                ret = -1;
	}

    post_log("\nSuccessful PWM diags done  !!!\n");

    PWM_post_deinit();
    return ret;
}

#endif /* CONFIG_POST & CONFIG_SYS_POST_PWM */
