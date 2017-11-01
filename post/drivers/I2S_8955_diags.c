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
#include <iproc_i2s.h>
#include <iproc_gsio.h>

#if CONFIG_POST & CONFIG_SYS_POST_I2S_W8955

int I2S__w8955_post_test (int flags)
{
    int status = 0;
    int ret=0;
    u32 sel; u64 i;


#if defined(CONFIG_NS_PLUS)

    if ( post_get_board_diags_type() & HR_ER_BOARDS )
	{
		printf("\nThis diag is not supported on this platform\n ");
		return 2;
	}
#endif


    post_log("\nI2S - WM8955 diags starts  !!!");

    if ( (flags & POST_SEMI_AUTO) !=  POST_SEMI_AUTO )
    {

        post_log("\nSet strap pin: A2=0, A3=0x3, A4=0xf, A5=0x28, A6=0xf8", 0);

    }
    else
    {
        post_set_strappins("A2 0%A3 3%A4 f%A5 38%A6 f8%");
    }
    post_log("\nPlug in the audio plack device into wm8955 audio jack");
    post_getConfirmation("\nReady? (Y/N)");

#if defined(CONFIG_NS_PLUS)
    post_log("\nEnable On-Chip I2S Audio PLL");
    iproc_i2s_enable_pll();
#else
    post_log("\nUse external PLL");
    post_log("\nmake sure the internal XTAL clk is connected (J3204 pin 2 and 3)");
    post_getConfirmation("\nReady? (Y/N)");
#endif
    
    WM8955_SPI_Enable();

	// Reset the codec
    WM8955_reset_codec();

    WM8955_poweron_channels();


    WM8955_configure_sigal_path();

    WM8955_configure_volume();

    WM8955_configure_format_mode();

    iproc_i2s_DMA_tx();

    //Wait here until get a Y/N
    status = post_getUserResponse("\nSound output OK? (Y/N)");

    //iproc_i2s_DMA_stop();

    if ( -1 == status )
    {
       post_log("\nTest Failed !");
    }
    else
    {
       post_log("\nTest Passed !");
    }

    post_log("\n\nI2S test done\n");
    return status;

failed:
    post_log("\nTest Failed !");
    return -1;
}

#if BEFORE_CAPTUR
int I2S__w8955_post_test (int flags)
{
    int status = 0;
    int ret=0;
    u32 sel; u64 i;

    post_log("\nI2S - WM8955 diags starts  !!!");
    post_log("\nSet strap pin: A2=0, A3=0x3, A4=0xf, A5=0x28, A6=0xf8", 0);
    post_log("\nmake sure the internal XTAL clk is connected (J3204 pin 2 and 3)");
    post_getConfirmation("\nReady? (Y/N)");

    if ( cca_spi_init() != SPI_ERR_NONE )
    {
        post_log("\nSPI not present, can't configure WM8750\n");
        goto failed;
    }

    // Reset the codec
    status = WM8955_write(WM8955_RESET, 0);

    if(status < 0)
    {
        post_log("\nWrite to WM8750 failed\n");
        goto failed;
    }
    udelay(100*1000);


    //Left Mixer 1, Left DAC to Left Mixer
    gsio_write_wm8955(WM8955_L_MIXER_1, 0x100);

    udelay(100*1000);
    //Right Mixer 2, Right DAC to Right Mixer
    gsio_write_wm8955(WM8955_R_MIXER_2, 0x100);

    udelay(100*1000);
    //Enable both DACS and mixers
    gsio_write_wm8955(WM8955_PWR2, 0x1f8);

    udelay(100*1000);
    //Enable 50Ohm divider
    WM8955_write(WM8955_PWR1, 0x80);

    udelay(100*1000);
    //Unmute DAC
    gsio_write_wm8955(WM8955_DAC_CTRL, 0x0);

    udelay(100*1000);
    // Normal mode ( CLK )
    // SR =  01010    16Khz sample rate
    // SR = 00000 48Khz
    //MCLK/2 enabled
    //gsio_write_wm8955(WM8955_SRATE_CTRL, 0x54);

    //SRATE 48Khz
    gsio_write_wm8955(WM8955_SRATE_CTRL, 0x40);  //?????


    udelay(100*1000);

    //CLKOUT pin enable, source of CLKOUT, MCLK
    //Dont enable CLKOUT for now. there is a wire supplues the clk ?
    //gsio_write_wm8955(CLK_PLL_CTRL, 0x80);

    //I2S format, 16 bit data, codec in slave mode
    gsio_write_wm8955(AUDIO_INT_CTRL, 0x2);


    udelay(100*1000);
    // Enable headphone switch
    //gsio_write_wm8955(0x18, (0x3 << 5));


    /* Send sincwave through I2S */
    //post_getConfirmation("\nReady to play loop back (Y/N)");

    //Wait here until get a Y/N
    //post_getUserResponse("\nSound output OK? (Y/N)");


    /* Configure I2S */
    iproc_i2s_init(1);

    iproc_i2s_DMA_init();


    /* Send sincwave through I2S */
    post_getConfirmation("\nReady to stream? (Y/N)");

    iproc_i2s_DMA_start();

    //Wait here until get a Y/N
    status = post_getUserResponse("\nSound output OK? (Y/N)");

    iproc_i2s_DMA_stop();

    if ( -1 == status )
    {
       post_log("\nTest Failed !");
    }
    else
    {
       post_log("\nTest Passed !");
    }

    post_log("\n\nI2S test done\n");
    return 0;

failed:
    post_log("\nTest Failed !");
    return -1;
}
#endif

#endif /* CONFIG_POST & CONFIG_SYS_POST_I2S_W8955 */
