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
#include <asm/io.h>
#include <iproc_i2s.h>
#include "iproc_i2s_data.h"
#include <asm/arch-northstar/ns_dma.h>

#if defined(CONFIG_NS_PLUS)
#define IPROC_I2S_BASE_ADDR    0x18028000
// IDM Register defines
///#define ROM_S0_IDM_IO_STATUS         0x1810D500
//#define I2S_M0_IDM_IO_CONTROL_DIRECT  0x18117408
#define I2S_M0_IDM_RESET_CONTROL        0x18118800
#define I2S_M0_IDM_RESET_STATUS         0x18118804

#define AUDIO_DMA_BUF                   (0x71000000)


#define POST_RECORD_BUFFER              0x71000000
#define POST_RECORD_BUF_SIZE            0x200000

#else
#define IPROC_I2S_BASE_ADDR    0x1802A000

// IDM Register defines
#define ROM_S0_IDM_IO_STATUS            0x1810D500
#define I2S_M0_IDM_IO_CONTROL_DIRECT    0x18117408
#define I2S_M0_IDM_RESET_CONTROL        0x18117800
#define I2S_M0_IDM_RESET_STATUS         0x18117804

#define AUDIO_DMA_BUF                   (0x8a000000)


#define POST_RECORD_BUFFER              0x8a000000
#define POST_RECORD_BUF_SIZE            0x200000
#endif


extern I2S_Block i2sBlk;


//#define debug printf

static void i2s_writel(u32 val, unsigned int addr)
{
    debug("Write [0x%08x] = 0x%08x\n", (u32)(addr+IPROC_I2S_BASE_ADDR), val);
    *(volatile unsigned int *)(addr + IPROC_I2S_BASE_ADDR) = (volatile unsigned int)val;
}
static u32 i2s_readl(unsigned int addr)
{
    volatile unsigned int val = *(volatile unsigned int *)(addr + IPROC_I2S_BASE_ADDR);
    debug("Read  [0x%08x] = 0x%08x\n", (u32)(addr+IPROC_I2S_BASE_ADDR), val);
    return (u32)val;
}


static void iproc_i2s_block_reset(void);
int iproc_PCM_init( void );


void iproc_PCM_DMA_tx(void)
{
    int status;
    int count = 0;


    /* Init the DMA to capture data */
    iproc_i2s_DMA_init();

    iproc_i2s_DMA_TX_init();
    iproc_i2s_DMA_TX_Enable();

    //start PCM playback


    while(count < 100)
    {
        ns_dmaTx(1600, (uint8_t *)sinwave16_800);

        printf("\n Data TX...");

        status = ns_dmaTxWait(&i2sBlk);
        count++;
    }

}

int iproc_PCM_init( void )
{
    u32 flags;
    u32 txrx;
    u32 sct_num =127; //assum 8MHZ PCLK

    debug("Enter %s\n", __FUNCTION__);

    debug("Enter %s\n", __FUNCTION__);

    iproc_i2s_block_reset();

    /* Configure device, TDM , 8 bit rx/tx word length
    /* DPX=0 half_duplex, tx
     *
     */
    flags = (1<<I2S_DC_TX8_BIT) |
            (1<<I2S_DC_RX8_BIT) |
            (0 << I2S_DC_DPX_BIT)| //Half duplex , tx only
            (1<<I2S_DC_TDM_SEL_BIT)|( 1<< 5);


    //i2s_writel(flags, I2S_DEVCONTROL_REG);
    i2s_writel(flags, I2S_DEVCONTROL_REG);



    flags = ( 0 << 2 ) |
            (sct_num << 8);  //Based on the PCLK

    i2s_writel(flags, I2S_TDMCONTROL_REG );

    i2s_writel(1, I2S_CONTROL_REG);



    i2s_writel(1, I2S_TDMCH0_CTRL_REG );


    printf("\nIPROC_PCM INITED!!!!!\n");


    /* Reduce tx threshold */
    //i2s_writel(0x7f, I2S_TXPLAYTH_REG);

    /* INT ENABLE */
    i2s_writel(0xFF7FFFE0, I2S_INTMASK_REG);


    debug("PCM Init Done\n");

    return 0;
}
void iproc_i2s_block_reset(void)
{
    unsigned int timeout;

    debug("Enter %s\n", __FUNCTION__);

    // Reset the device
    writel(1, I2S_M0_IDM_RESET_CONTROL);
    udelay(1000);
    writel(0, I2S_M0_IDM_RESET_CONTROL);
    udelay(1000);

    i2s_writel((1<<6), I2S_DEVCONTROL_REG);
    udelay(1000);

    debug("\nReset Done\n");
}
