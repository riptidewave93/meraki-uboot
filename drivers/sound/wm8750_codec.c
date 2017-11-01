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


static int WM8750_write(unsigned short regAddr, unsigned short registerValue )
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

int WM8750_SPI_Enable( void )
{
    if ( cca_spi_init() != SPI_ERR_NONE )
    {
        dbg_print("\nSPI not present, can't configure WM8750\n");
        return -1;
    }
    return 0;
}

void WM8750_reset_codec ( void )
{
    WM8750_write(WM8750_RESET, 0);
    udelay(100*1000);

}
void WM8750_poweron_channels( void )
{
    // Enable ADCL, ADCR, PGAL, PGAR and VREF
    //pwr1_reg = 0x7C;
    WM8750_write(WM8750_PWR1, 0xFC);

    // Enable LOUT1, ROUT1, DACL and DACLR
    //pwr2_reg = 0x1E0;
    WM8750_write(WM8750_PWR2, 0x1E0);

}

void WM8750_configure_sigal_path( void )
{

    // ADC Signal path, select LIN1 AND RIN1, mic boost to 13db
    WM8750_write(WM8750_LADCIN, 0x10);
    WM8750_write(WM8750_RADCIN, 0x10);

    // Enable Left and Right Mixers !!!! Re-direct and loop back
    //This selects line in + dac in
    //WM8750_write(WM8750_LOUTM1, 0x180);
    //WM8750_write(WM8750_ROUTM2, 0x180);

    WM8750_write(WM8750_LOUTM1, 0x103);
    WM8750_write(WM8750_ROUTM2, 0x100);
    WM8750_write(0x24, 0x3);
}


void WM8750_configure_volume ( void )
{
    // Left and Right channel DAC volumes
    WM8750_write(WM8750_LDAC, 0x1FF);       // Enable volume control, 0db gain
    WM8750_write(WM8750_RDAC, 0x1FF);       // Enable volume control, 0db gain

    // Unmute DAC
    dbg_print("%s: Unmuting DAC\n", __FUNCTION__);
    WM8750_write(WM8750_ADCDAC, 0);
    //WM8750_write(WM8750_ADCDAC, 0x20);

    // Unmute left and right ADCs and set gain to 0db
    WM8750_write(WM8750_LINVOL, 0x117);
    WM8750_write(WM8750_RINVOL, 0x117);

    // Set ALC
    //WM8750_write(WM8750_ALC1, 0x1F);

    // Set output volumes to 0db
    //WM8750_write(WM8750_LOUT1V, 0x179);
    //WM8750_write(WM8750_ROUT1V, 0x179);
    WM8750_write(WM8750_LOUT1V, 0x17F);
    WM8750_write(WM8750_ROUT1V, 0x17F);



    // Mute Left right in
    //swang WM8750_write(WM8750_LINVOL, 0x80);
    //swang WM8750_write(WM8750_RINVOL, 0x80);

    WM8750_write(WM8750_LINVOL, 0x13f);

    WM8750_write(WM8750_RINVOL, 0x13f);

}
void WM8750_configure_format_mode(void)
{

    //set format for 8750
     //audio interface register
     // I2S format, 16 bit data, slave mode
     WM8750_write(0x7, 0x2  );
     //Normal mode, dont div the mclk ( it is exactly 12.288Mhz ), sample rate = 0, so ADC and DAC are both using
     // 48Khz
     WM8750_write(0x8,0);// 0xA << 1);
}

void WM8750_disable_ADC_data( void )
{
    //ADCDAT is tristated, ADCLRC, DACLRC
    //and BCLK are inputs
    WM8750_write(0x18, 8 );
}

#endif /*IPROC_BOARD_DIAGS */
