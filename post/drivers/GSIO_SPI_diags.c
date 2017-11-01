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

#if CONFIG_POST & CONFIG_SYS_POST_GSIO_SPI

#include <iproc_gsio.h>
#include <iproc_i2s.h>

static int gsio_write_wm8955( unsigned short regAddr, unsigned short registerValue )
{
    unsigned short sixteenbitCommand;

    sixteenbitCommand = (regAddr << 9) | ( 0x1FF & registerValue );

    if ( SPI_ERR_NONE ==cca_spi_write((unsigned char*)&sixteenbitCommand, 2 ))
    {
        //dbg_print("\nCommand issued through GSIO, register 0x%x, value 0x%x\n",regAddr, ( 0x1FF & registerValue )  );
        return 0;
    }
    return -1;
}

int gsiospi_post_test ( void )
{

    int status = 0;
    int ret=0;
#if defined(CONFIG_NS_PLUS)

    if ( post_get_board_diags_type() & HR_ER_BOARDS )
	{
		printf("\nThis diag is not supported on this platform\n ");
		return 2;
	}
#endif

    post_log("\nGSIOSPI diags starts  !!!");
    post_log("\nSet strap pin to A4 0x1, A6 0x0, make sure the 24MHz XTAL is connected (J3204 pin 1 and 2");
    post_getConfirmation("\nReady? (Y/N)");

    if ( cca_spi_init() == SPI_ERR_NONE )
    {

        status = gsio_write_wm8955 (SAMPLE_RATE_REG,0 );
         status = gsio_write_wm8955 (CLK_PLL_REG,CLK_OUT_ENABLE );
         ret = post_getUserResponse("\nPut a scope on the pin 8 of WM8955, is there a 24MHz clock? (Y/N)");
         if ( ret == -1 )
         {
             status = -1;
         }


         status = gsio_write_wm8955 (SAMPLE_RATE_REG,MCLK_DIV_BY_2 );

         ret = post_getUserResponse("\nPut a scope on the pin 8 of WM8955, is there a 12MHz clock? (Y/N)");
         if ( ret == -1 )
         {
             status = -1;
         }


    }
    else
    {
        post_log("\nSPI not present !");
        status = -1;

    }
    if ( -1 == status )
    {
       post_log("\nTest Failed !");
    }
    else
    {
       post_log("\nTest Passed!");
    }
    post_log("\nGSIOSPI diags done  !!!");

    return status;
}

#endif /* CONFIG_POST & CONFIG_SYS_POST_GSIO_SPI */
