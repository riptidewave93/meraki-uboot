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

#ifdef IPROC_BOARD_DIAGS


#ifdef I2S_DEBUG
#define  dbg_print printf
#else
#define dbg_print
#endif

static int WM8955_write( unsigned short regAddr, unsigned short registerValue );


int WM8955_write( unsigned short regAddr, unsigned short registerValue )
{
    unsigned short sixteenbitCommand;

    sixteenbitCommand = (regAddr << 9) | ( 0x1FF & registerValue );

    if ( SPI_ERR_NONE ==cca_spi_write((unsigned char*)&sixteenbitCommand, 2 ))
    {
        dbg_print("\nCommand issued through GSIO, register 0x%x, value 0x%x\n",regAddr, ( 0x1FF & registerValue )  );
        return 0;
    }
    return -1;
}

int WM8955_SPI_Enable( void )
{
    if ( cca_spi_init() != SPI_ERR_NONE )
    {
        dbg_print("\nSPI not present, can't configure WM8750\n");
        return -1;
    }
    return 0;
}

void WM8955_reset_codec ( void )
{
    WM8955_write(WM8955_RESET, 0);
    udelay(100*1000);

}

void WM8955_poweron_channels( void )
{

    //STANDBY MODE
    WM8955_write(0x19, 0x140);

    udelay(100*1000);
    udelay(100*1000);
    udelay(100*1000);

}

void WM8955_configure_sigal_path( void )
{
    //Left Mixer 1, Left DAC to Left Mixer
    WM8955_write(WM8955_L_MIXER_1, 0x100);

    udelay(100*1000);
    //Right Mixer 2, Right DAC to Right Mixer
    WM8955_write(WM8955_R_MIXER_2, 0x100);

    udelay(100*1000);
    //Enable both DACS and mixers
    WM8955_write(WM8955_PWR2, 0x1f8);

    udelay(100*1000);
    //Enable 50Ohm divider
    WM8955_write(WM8955_PWR1,0xC0); //0x80);

    udelay(100*1000);
    //Unmute DAC
    WM8955_write(WM8955_DAC_CTRL, 0x0);
}


void WM8955_configure_volume ( void )
{
    //Volume
     WM8955_write(0x28, 0x17F);
     WM8955_write(0x29, 0x17f);

     udelay(100*1000);

}
void WM8955_configure_format_mode(void)
{

    //SRATE 48Khz
    WM8955_write(WM8955_SRATE_CTRL, 0x40);  //?????


    udelay(100*1000);

    //CLKOUT pin enable, source of CLKOUT, MCLK
    //Dont enable CLKOUT for now. there is a wire supplues the clk ?
    //gsio_write_wm8955(CLK_PLL_CTRL, 0x80);

    //I2S format, 16 bit data, codec in slave mode
    WM8955_write(AUDIO_INT_CTRL, 0x2);


    udelay(100*1000);
}
#endif /*IPROC_BOARD_DIAGS */
