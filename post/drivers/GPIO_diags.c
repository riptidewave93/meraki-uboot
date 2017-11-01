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
#include <iproc_gpio.h>
#include <iproc_gpio_cfg.h>


volatile int countLevelHigh;
volatile int countLevelLow;
volatile int countRisingEdge;
volatile int countFalliingEdge;

extern volatile int IntReceived;

#if CONFIG_POST & CONFIG_SYS_POST_GPIO

int numberOfGPIOs = 24;
unsigned int  allGPIOs = 0xFFFFFF;

int receivedINT = 0;


static iproc_gpio_chip *testDev;


static void GPIO_testInit(void)
{
    //Perform hardware init
    iprocGPIOInit();
    //Get the device handle
    testDev=getGPIODev();
#if defined(CONFIG_NS_PLUS)
    if ( post_get_board_diags_type() == BCM953025K_REV2 )
    {
    	numberOfGPIOs= 32;
    	allGPIOs = 0xFFFFFFFF;
    }
#endif

}
static int GPIO_testCleanup(void)
{
    if ( iproc_gpio_restoreRegisters() == -1 )
    {
        post_log("\nClean up failed ");
        return -1;
    }
    return 0;
}
static int GPIO_funcConfigTest( int cfg )
{
    int i,ret = 0;

    //Test GPIO DMU MUX select, there is a documentation error
    //in the Register programming guide, the MUX select does not
    // select GPIO 18-20
    for ( i = 0; i < numberOfGPIOs; i++ )
    {
        //Do not set for GPIO 18-20 there is no MUX available
        if ( (i >=18) && ( i <=20 ) )
        {
            continue;
        }
        else if ( i > 20 )
        {
            //Use bit 18-20  in the register for GPIO pin 21-24
            iproc_gpio_set_config(testDev,i-3, cfg);
        }
        iproc_gpio_set_config(testDev,i, cfg);
    }
    for ( i = 0; i < numberOfGPIOs; i++ )
    {
        if ( (i >=18) && ( i <=20 ) )
        {
            continue;
        }
        if ( i > 20 )
        {
            if ( cfg != iproc_gpio_get_config(testDev,i-3))
            {
                post_log("\nGPIO pin %d is not configured as expected", i);
                ret = -1;
            }
        }
        else if ( cfg != iproc_gpio_get_config(testDev,i))
        {
            post_log("\nGPIO pin %d is not configured as expected", i);
            ret = -1;
        }
    }
    if ( ret == -1 )
    {
        post_log("\nGPIO function configuration test failed");
    }
    else
    {
        post_log("\nGPIO function configuration test passed");
    }
    return ret;
}
static int GPIO_PullUpDownConfigTest( void )
{
    iproc_gpio_pull_t type;
    int ret = 0;
    int i;
    static char *pullUpDown[]={ "No Pull","Pull Down","Pull Up"};

    for ( type = IPROC_GPIO_PULL_NONE; type <=IPROC_GPIO_PULL_UP;type++ )
    {
        for ( i = 0; i < numberOfGPIOs; i++ )
        {
            iproc_gpio_setpull_updown(testDev,i, type);
        }
        for ( i = 0; i < numberOfGPIOs; i++ )
        {

            if ( type != iproc_gpio_getpull_updown(testDev,i))
            {
                post_log("\nGPIO pin %d can not be configured as %s ", i,pullUpDown[type]);
                ret = -1;
            }
        }
    }
    if ( ret == -1 )
    {
        post_log("\nGPIO pull up/down configuration test failed");
    }
    else
    {
        post_log("\nGPIO pull up/down configuration test passed");
    }
    return ret;
}
static int GPIO_HYsteresisConfigTest( void )
{
    HysteresisEnable type;
    int ret = 0;
    int i;

    for ( type = DISABLE; type <=ENABLE;type++ )
    {
        for ( i = 0; i < numberOfGPIOs; i++ )
        {
            iproc_gpio_setHyeteresis(testDev,i, type);
        }
        for ( i = 0; i < numberOfGPIOs; i++ )
        {

            if ( type != iproc_gpio_getHyeteresis(testDev,i))
            {
                post_log("\nGPIO pin %d can not be configured to enable Hysteresis ", i);
                ret = -1;
            }
        }
    }
    if ( ret == -1 )
    {
        post_log("\nGPIO Hysteresis configuration test failed");
    }
    else
    {
        post_log("\nGPIO Hysteresis configuration test passed");
    }
    return ret;
}
static int GPIO_DriveStrengthConfigTest( void )
{
    driveStrengthConfig ds;
    int ret = 0;
    int i;

    for ( ds = d_2mA; ds <=d_16mA;ds++ )
    {
        for ( i = 0; i < numberOfGPIOs; i++ )
        {
            iproc_gpio_setDriveStrength(testDev,i, ds);
            if ( ds != iproc_gpio_getDriveStrength(testDev,i))
            {
                post_log("\nGPIO pin %d can not be configured drive strength %d ", i,ds);
                ret = -1;
            }
        }
    }
    if ( ret == -1 )
    {
        post_log("\nGPIO drive strength configuration test failed");
    }
    else
    {
        post_log("\nGPIO drive strength configuration test passed");
    }
    return ret;
}
static int GPIO_InputDisableConfigTest( void )
{
    int ret = 0,i;
    //InputDisableCfg cfg;
    int j;

    //for ( cfg = INPUT_DISABLE; cfg >= INPUT_ENABLE;cfg--)
    for ( j=1; j >= 0 ;j--)
    {
        for ( i = 0; i < numberOfGPIOs; i++ )
        {
            iproc_gpio_setInputdisable(testDev,i, j);
        }
        for ( i = 0; i < numberOfGPIOs; i++ )
        {

            if ( j != iproc_gpio_getInputdisable(testDev,i))
            {
                post_log("\nGPIO pin %d can not be configured to be disabled/enabled %d ", i,j);
                ret = -1;
            }
        }
    }
    if ( ret == -1 )
    {
        post_log("\nGPIO input disable test failed");
    }
    else
    {
        post_log("\nGPIO input disable test passed");
    }
    return ret;
}
static int GPIO_post_testGPIOconfig(int flags )
{
    int ret = 0;


    //Save initial register settings before test
    iproc_gpio_saveCFGRegisters();
    if ( (flags & POST_AUTO) !=  POST_AUTO )
    {
       ret |= GPIO_funcConfigTest(IPROC_GPIO_GENERAL);
    }
    ret |= GPIO_funcConfigTest(IPROC_GPIO_AUX_FUN);

    ret |= GPIO_PullUpDownConfigTest();

    ret |= GPIO_HYsteresisConfigTest();

    ret |=GPIO_DriveStrengthConfigTest();

    if ( (flags & POST_AUTO) !=  POST_AUTO )
    {
       ret |= GPIO_InputDisableConfigTest();
    }


    ret |= GPIO_testCleanup();
    return ret;

}

int GPIO_outputTest( void )
{
    int i=0;
    int ret=0;
    //Note: SET strap pin A4 to 0 , A5 t0 0x8, A6 to 0

    post_log("\nPlease set strap pin A4 to 0, A5 to 0x8 and A6 to 0", i);
    post_getConfirmation("\nReady? (Y/N)");



    for ( i = 0; i < numberOfGPIOs; i++ )
    {
        setGPIOasOutput(i, 1);
    }
    for ( i = 0; i < numberOfGPIOs; i++ )
    {
        if ( 1 != getGPIOpinValue(i))
        {
            post_log("\nGPIO pin % is not set to output 1 as GPIO", i);
            ret = -1;
        }
    }

#if defined(CONFIG_NS_PLUS)
    if ( post_get_board_diags_type() & BCM953025K_REV2 )
    {
    	ret |= post_getUserResponse("\nAre LEDs D1901-D1916, D3501-D3508 and D3810-D3817 turned on? (Y/N)");
    }
    else
    {
    	ret |= post_getUserResponse("\nAre LEDs D1901-D1916 and D3501-D3508 turned on? (Y/N)");
    }
#else
    ret |= post_getUserResponse("\nAre LEDs D1901-D1916 and D3501-D3508 turned on? (Y/N)");
#endif


    for ( i = 0; i < numberOfGPIOs; i++ )
    {
        setGPIOasOutput(i, 0 );
    }

    for ( i = 0; i < numberOfGPIOs; i++ )
    {
        if ( 0 != getGPIOpinValue(i))
        {
            post_log("\nGPIO pin % is not set to output 0 as GPIO", i);
            ret |= -1;
        }
    }
#if defined(CONFIG_NS_PLUS)
    if ( post_get_board_diags_type() & BCM953025K_REV2 )
    {
    	ret |= post_getUserResponse("\nAre LEDs D1901-D1916, D3501-D3508 and D3810-D3817 turned off? (Y/N)");
    }
    else
    {
    	ret |= post_getUserResponse("\nAre LEDs D1901-D1916 and  D3501-D3508 turned off? (Y/N)");
    }
#else
    ret |= post_getUserResponse("\nAre LEDs D1901-D1916 and D3501-D3508 turned off? (Y/N)");
#endif

    return ret;


}
int GPIO_inputTest( void )
{
    int i=0;
    int ret =0;
    int retall=0;
    //TEST INPUT
    //SET A4 to 0xF0, A5 0xC8 A6 0
#if defined(CONFIG_NS_PLUS)
    if ( post_get_board_diags_type() & BCM953025K_REV2 )
    {
    	//A6, bit 6 & 7 = 1, GPIO 24-31 used as input
    	post_log("\nPlease set strap pin A4 to 0xF0, A5 to 0xC8 and A6 to 0xC0", i);
    }
    else
    {
    	post_log("\nPlease set strap pin A4 to 0xF0, A5 to 0xC8 and A6 to 0", i);
    }
#else
    post_log("\nPlease set strap pin A4 to 0xF0, A5 to 0xC8 and A6 to 0", i);
#endif
    post_getConfirmation("\nReady? (Y/N)");
//#if (defined(CONFIG_NS_PLUS))
//    post_log("\nDisable input register setting : 0x%x \n", *((unsigned long*)0x1803f1d0));
//#endif

    //SET to off position SW3501 GPIO 16-23 Pull up/Down   RED DOTS on sw is off position
    //    SW1901 GPIO 8-15
    //    SW1902 GPIO 0-7
#if defined(CONFIG_NS_PLUS)
    if ( post_get_board_diags_type() & BCM953025K_REV2 )
    {
    	post_log("\nMake sure  SW1901,SW1902,SW3501 and SW3502 are in ( ON position,switch is closed ) ");
    }
    else
    {
    	post_log("\nMake sure  SW1901,SW1902 and SW3501 are in ( ON position,switch is closed ) ");
    }
#else
    post_log("\nMake sure  SW1901,SW1902 and SW3501 are in ( ON position,switch is closed ) ");
#endif
    post_getConfirmation("\nAre all switches on? (Y/N)");

    for ( i = 0; i < numberOfGPIOs; i++ )
    {
        setGPIOasInput(i);
    }

    for ( i = 0; i < numberOfGPIOs; i++ )
    {
        if ( 0 != getGPIOpinValue(i) )
        {
            post_log("\nGPIO pin % is not reading 0 as expected", i);
            ret = -1;
            retall |= -1;
        }
        else
        {
        	post_log("\nGPIO pin % is reading 0 as expected", i);
        }
    }
    if ( ret == -1 )
    {
        post_log("\nGPIO input low test failed");
    }
    else
    {
        post_log("\nGPIO input low test passed");

    }

    post_log("\nput all switches in ( Off position,switch is open )");
    post_getConfirmation("\nAre all switches off? (Y/N)");

    //SET to ON position SW3501 GPIO 16-23 Pull up/Down   RED DOTS on sw is off position
    //    SW1901 GPIO 8-15
    //    SW1902 GPIO 0-7

    ret = 0;

    for ( i = 0; i < numberOfGPIOs; i++ )
    {
        if ( 1 != getGPIOpinValue(i) )
        {
            post_log("\nGPIO pin %  is not reading 1 as expected", i);
            ret |= -1;
            retall |= -1;
        }
        else
        {
        	post_log("\nGPIO pin %  is reading 1 as expected", i);
        }
    }

    if ( ret == -1 )
    {
        post_log("\nGPIO input high test failed");
    }
    else
    {
        post_log("\nGPIO input high test passed");

    }

    return retall;

}

int GPIO_interruptTest( void )
{
    int i,ret=0;
    volatile unsigned long delay;

    countLevelHigh = 0;
    countLevelLow = 0;
    countRisingEdge = 0;
    countFalliingEdge = 0;



    //Set all gpio pins to init state
    initGPIOState();
    registerGPIOEdgeInterrupt();
    post_log("\nGPIO rising edge interrupt test");
#if defined(CONFIG_NS_PLUS)
    if ( post_get_board_diags_type() & BCM953025K_REV2 )
    {
    	post_log("\nMake sure  SW1901,SW1902, SW3501 and SW3502 are in ( ON position,switch is closed )");
    }
    else
    {
    	post_log("\nMake sure  SW1901,SW1902 and SW3501 are in ( ON position,switch is closed )");
    }
#else
    post_log("\nMake sure  SW1901,SW1902 and SW3501 are in ( ON position,switch is closed )");
#endif

    post_getConfirmation("\nAre all switches on? (Y/N)");
    for ( i = 0; i < numberOfGPIOs; i++)
    {
        setGPIOasInput(i);
        //IntReceived = 0;
        enableGPIOInterrupt( i, IRQ_TYPE_EDGE_RISING );

        //while ( (*(volatile unsigned long*)0x1800007c) & ( 1<< i )){}
    }
    post_log("\nPut all switches to OFF (open) position SEQUENTIALLY");
    post_log("\nWaiting for interrupt...");
    delay = 0xFFFFFFF;

    IntReceived= 0;
    while( (countRisingEdge & allGPIOs)!=allGPIOs )
    {
    	if ( IntReceived == 1)
    	{
    		IntReceived = 0;
    		delay = 0xFFFFFFF;
    		post_log("\nReceived int on %d, Waiting for interrupt...",receivedINT);
    	}
        if ( delay == 0 )
        {
            ret = -1;
            break;
        }
        delay--;
    }
    if ( ret == 0 )
    {
        post_log("\nReceived all %d rising edge interrupts, passed",numberOfGPIOs);
    }
    else
    {
        for ( i = 0; i < numberOfGPIOs; i++)
        {
            if ( (countRisingEdge & ( 1 << i )) == 0 )
            {
                post_log("\nDid not receive interrupt for GPIO %d", i);
            }
            else
            {
            	post_log("\nReceived interrupt for GPIO %d", i);
            }
        }
        post_log("\nRising edge test failed");
    }


    //Set all gpio pins to init state
    initGPIOState();

    post_log("\nGPIO falling edge interrupt test");
    post_getConfirmation("\nReady? (Y/N)");



    for ( i = 0; i < numberOfGPIOs; i++)
    {
        setGPIOasInput(i);
        IntReceived = 0;
        //post_log("\nSet GPIO %d to off", i);
        enableGPIOInterrupt( i, IRQ_TYPE_EDGE_FALLING );
    }
    post_log("\nPut each of the switches to ON( closed) position SEQUENTIALLY");
    post_log("\nWaiting for interrupt...");
    delay = 0xFFFFFFF;
    IntReceived = 0;
    ret = 0;
    while( (countFalliingEdge & allGPIOs)!=allGPIOs )
    {
    	if ( IntReceived == 1)
    	{
    		IntReceived = 0;
    		delay = 0xFFFFFFF;
    		post_log("\nReceived int on %d, Waiting for interrupt...",receivedINT);
    	}
        if ( delay == 0 )
        {
            ret = -1;
            break;
        }
        delay--;
    }
    if ( ret == 0 )
    {
        post_log("\nReceived all %d Falling edge interrupts, passed",numberOfGPIOs);
    }
    else
    {
        for ( i = 0; i < numberOfGPIOs; i++)
        {
            if ( (countFalliingEdge & ( 1 << i )) == 0 )
            {
                post_log("\nDid not receive interrupt for GPIO %d", i);
            }
            else
            {
            	post_log("\nReceived interrupt for GPIO %d", i);
            }
        }
        post_log("\nFalling edge test failed");
    }


    //Set all gpio pins to init state
    initGPIOState();
    registerGPIOLevelInterrupt();
    post_log("\nGPIO level low interrupt test");
#if defined(CONFIG_NS_PLUS)
    if ( post_get_board_diags_type() & BCM953025K_REV2 )
    {
    	post_log("\nMake sure SW1901,SW1902, SW3501 and SW3502 are in ( ON position,switch is closed )");
    }
    else
    {
    	post_log("\nMake sure SW1901,SW1902 and SW3501 are in ( ON position,switch is closed )");
    }
#else
    post_log("\nMake sure SW1901,SW1902 and SW3501 are in ( ON position,switch is closed )");
#endif
    post_getConfirmation("\nReady? (Y/N)");

    for ( i = 0; i < numberOfGPIOs; i++)
    {
        setGPIOasInput(i);
        //post_log("\nSet pin %d off,Wait for interrupt...");
        enableGPIOInterrupt( i, IRQ_TYPE_LEVEL_LOW );
    }

    ret = 0;
    delay = 0xFFFFFF;
    while( (countLevelLow & allGPIOs)!=allGPIOs )
    {
    	if ( delay == 0 )
    	{
    		ret = -1;
            for ( i = 0; i < numberOfGPIOs; i++)
            {
                if ( (countLevelLow & ( 1 << i )) == 0 )
                {
                    post_log("\nDid not receive interrupt for GPIO %d", i);
                }
                else
                {
                	//post_log("\nReceived interrupt for GPIO %d", i);
                }
            }

            break;
    	}
    	delay --;
    }
    if ( ret == 0 )
    {
        post_log("\nReceived all %d level low interrupts, passed",numberOfGPIOs);
    }
    else
    {
        post_log("\nLevel low interrupt test failed");

    }


    post_log("\nGPIO level high interrupt test");
#if defined(CONFIG_NS_PLUS)
    if ( post_get_board_diags_type() & BCM953025K_REV2 )
    {
    	post_log("\nMake sure SW1901,SW1902, SW3501 and SW3502 are in ( OFF position,switch is open )");
    }
    else
    {

    	post_log("\nMake sure SW1901,SW1902 and SW3501 are in ( OFF position,switch is open )");
    }
#else
    post_log("\nMake sure SW1901,SW1902 and SW3501 are in ( OFF position,switch is open )");
#endif
    post_getConfirmation("\nReady? (Y/N)");


    //Set all gpio pins to init state
    //initGPIOState();


    for ( i = 0; i < numberOfGPIOs; i++)
    {
        setGPIOasInput(i);

        //IntReceived = 0;
        //post_log("\nSet pin %d on,Wait for interrupt...",i);

        enableGPIOInterrupt( i, IRQ_TYPE_LEVEL_HIGH);

        //delay = 0xFFFFFF;
    }

    ret = 0;
    delay = 0xFFFFFF;
    while( (countLevelHigh & allGPIOs)!=allGPIOs )
    {
    	if ( delay == 0 )
    	{
    		ret = -1;
            for ( i = 0; i < numberOfGPIOs; i++)
            {
                if ( (countLevelHigh & ( 1 << i )) == 0 )
                {
                    post_log("\nDid not receive interrupt for GPIO %d", i);
                }
                else
				{
					post_log("\nReceived interrupt for GPIO %d", i);
				}
            }
            break;
    	}
    	delay --;
    }
    if ( ret == 0 )
    {
        post_log("\nReceived all %d level high interrupts, passed",numberOfGPIOs);
    }
    else
    {
        post_log("\nLevel high interrupt test failed");

    }


    initGPIOState();


    return ret;
}

int GPIO_post_test (int flags)
{
    int ret = 0;
    int status = 0;
#if defined(CONFIG_NS_PLUS)
    int boardType= post_get_board_diags_type();
#endif

    post_log("\nGPIO diags starts  !!!");
    //Init for test
    GPIO_testInit();

    //Test GPIO configurations in the DMU
    ret = GPIO_post_testGPIOconfig(flags);
    status |= ret;

    if ( -1 == ret )
    {
        post_log("\nGPIO configuration tests failed");
    }
    else
    {
        post_log("\nGPIO configuration tests passed");
    }

    if ( (flags & POST_AUTO) ==  POST_AUTO )
    {
        post_log("\nGPIO diags done  !!!\n");
        return ret;
    }
#if defined(CONFIG_NS_PLUS)
    if ( post_get_board_diags_type() & HR_ER_BOARDS )
    {
        post_log("\nGPIO diags done  !!!\n");
        return ret;
    }
#endif

    ret = GPIO_funcConfigTest(IPROC_GPIO_GENERAL);
    status |= ret;
    if ( -1 == ret )
    {
        post_log("\nGPIO configuration tests failed");
    }
    else
    {
        post_log("\nGPIO configuration tests passed");
    }


    ret = GPIO_outputTest();
    status |= ret;
    if ( -1 == ret )
    {
        post_log("\nGPIO output tests failed");
    }
    else
    {
        post_log("\nGPIO output tests passed");
    }

    ret = GPIO_inputTest();
    status |= ret;
    if ( -1 == ret )
    {
        post_log("\nGPIO input tests failed");
    }
    else
    {
        post_log("\nGPIO input tests passed");
    }

    ret = GPIO_interruptTest();
    status |= ret;
    if ( -1 == ret )
    {
        post_log("\nGPIO interrupt test failed");
    }
    else
    {
        post_log("\nGPIO interrupt test passed");
    }

    GPIO_funcConfigTest(IPROC_GPIO_AUX_FUN);

    post_log("\nGPIO diags done  !!!\n");

    return status;
}

#endif /* CONFIG_POST & CONFIG_SYS_POST_PWM */
