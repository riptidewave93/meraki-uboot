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

#if CONFIG_POST & CONFIG_SYS_POST_I2S


int I2S_post_test (int flags)
{
    int status = 0;


#if defined(CONFIG_NS_PLUS)

    if ( post_get_board_diags_type() & HR_ER_BOARDS )
	{
		printf("\nThis diag is not supported on this platform\n ");
		return 2;
	}
#endif

    post_log("\nI2S - WM8750 diags starts  !!!");


    if ( (flags & POST_SEMI_AUTO) !=  POST_SEMI_AUTO )
    {

        post_log("\nSet strap pin: A2=0, A3=0x3, A4=0xf, A5=0x28, A6=0xf4", 0);
        post_getConfirmation("\nReady? (Y/N)");

    }
    else
    {
        post_set_strappins("A2 0%A3 3%A4 f%A5 38%A6 f4%");
    }



    if ( WM8750_SPI_Enable() != 0 )
    {
        post_log("\nSPI not present, can't configure WM8750\n");
        goto failed;
    }

    // Reset the codec
    WM8750_reset_codec();

    WM8750_poweron_channels();

    WM8750_configure_sigal_path();

    WM8750_configure_volume();

    WM8750_configure_format_mode();

    post_log("\nConnect an audio input device to the upper port of the J3301 and start an audio playback\n ");
    post_log(" Connect an audio output device to the lower port of the J3301\n ");
    post_getConfirmation("\nReady? (Y/N)");

    iproc_i2s_recording_start();


    post_log("\nAudio is recorded. start playback");

    //Disable the codec from playback recording data
    WM8750_disable_ADC_data();


    iproc_i2s_playback_recorded_audio();


    status = post_getUserResponse("\nSound output OK? (Y/N)");

    if ( -1 == status )
    {
       post_log("\nTest Failed !");
    }
    else
    {
       post_log("\nTest Passed !");
    }

    post_log("\n\nI2S test done\n");

    //I2S__w8955_post_test(0);
    return status;

failed:
    post_log("\nTest Failed !");
    return -1;
}




#endif /* CONFIG_POST & CONFIG_SYS_POST_I2S */
