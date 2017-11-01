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
#include <iproc_gsio.h>

#include "iproc_i2s_data.h"
#include <asm/arch-northstar/ns_dma.h>

#if defined(CONFIG_NS_PLUS)
#define IPROC_I2S_BASE_ADDR    0x18028000
// IDM Register defines
///#define ROM_S0_IDM_IO_STATUS			0x1810D500
//#define I2S_M0_IDM_IO_CONTROL_DIRECT 	0x18117408
#define I2S_M0_IDM_RESET_CONTROL 		0x18118800
#define I2S_M0_IDM_RESET_STATUS 		0x18118804

#define AUDIO_DMA_BUF                   (0x71000000)


#define POST_RECORD_BUFFER              0x71000000
#define POST_RECORD_BUF_SIZE            0x200000

#else
#define IPROC_I2S_BASE_ADDR    0x1802A000

// IDM Register defines
#define ROM_S0_IDM_IO_STATUS			0x1810D500
#define I2S_M0_IDM_IO_CONTROL_DIRECT 	0x18117408
#define I2S_M0_IDM_RESET_CONTROL 		0x18117800
#define I2S_M0_IDM_RESET_STATUS 		0x18117804

#define AUDIO_DMA_BUF                   (0x8a000000)


#define POST_RECORD_BUFFER              0x8a000000
#define POST_RECORD_BUF_SIZE            0x200000
#endif

#define AUDIO_PLL_MDIV      0x1803f50c
#define AUDIO_PLL_NDIV      0x1803f510
#define AUDIO_PLL_PDIV      0x1803f514
#define AUDIO_PLL_EXT_CTRL  0x1803f518
#define AUDIO_PLL_CONFIG    0x1803f500
#define AUDIO_PLL_STATUS    0x1803f504


I2S_Block i2sBlk;


int stopRecording = 0;

static void iproc_i2s_DMA_tx_recording(void);
void iproc_i2s_DMA_tx(void);

static void iproc_i2s_DMA_RX_Disable(void);
static void iproc_i2s_DMA_RX_Disable(void);
//static void iproc_i2s_DMA_copy_buffer( unsigned char *p, unsigned int len );
static void iproc_i2s_DMA_rx( void );
static void iproc_i2s_DMA_RX_Enable(void);
void iproc_i2s_DMA_init(void);

#ifdef DEBUG
#define dbg_print printf
#else
#define dbg_print
#endif

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

static u32 sin_loop, total_len, ptr, fifo_addr;

static void audio_pll_writel(u32 val, unsigned int addr)
{
	debug("Write [0x%08x] = 0x%08x\n", (u32)(addr), val);
	*(volatile unsigned int *)(addr) = (volatile unsigned int)val;
}
static u32 audio_pll_readl(unsigned int addr)
{
	volatile unsigned int val = *(volatile unsigned int *)(addr);
	debug("Read  [0x%08x] = 0x%08x\n", (u32)(addr), val);
	return (u32)val;
}

int iproc_i2s_enable_pll(void)
{
    //Configure Audio PLL, I2S 12.288MHz
    audio_pll_writel(200,               AUDIO_PLL_MDIV);
    audio_pll_writel((98<<20)+75498,    AUDIO_PLL_NDIV);
    audio_pll_writel(1,                 AUDIO_PLL_PDIV);
    audio_pll_writel(0,                 AUDIO_PLL_EXT_CTRL);
    audio_pll_writel((1<<23)+(1<<21)+(1<<18),  AUDIO_PLL_CONFIG);

    // bit13, lock bit
    mdelay ( 100 );
    if( !(audio_pll_readl(AUDIO_PLL_STATUS) & (1<<13)) )
    {
        printf("\nI2S AUDIO PLL is not locked. Fail.");
        return -1;
    }
    printf("\nI2S AUDIO PLL enabled and locked"); 
}

void iproc_i2s_reset(void)
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
void iproc_i2s_fill_fifo(void)
{
    u32 val_l, val_r, flags;
    u32 i;

    for(i=0; i<128; i++)
    {
        /* Enable FIFO mode, 4=write */
        flags = (1<<31) | (4<<16) | i;
        i2s_writel(flags, I2S_FIFOADDRESS_REG);

        val_l = sinwave16_800[ptr];
        val_r = sinwave16_800[ptr++];
        flags = (val_l << 16) | (val_r & 0xffff);
        i2s_writel(flags, I2S_FIFODATA_REG);
    }

    for(i=0; i<128; i++)
    {
        /* Enable FIFO mode, 8=read */
        flags = (1<<31) | (8<<16) | i;
        i2s_writel(flags, I2S_FIFOADDRESS_REG);

        i2s_readl(I2S_FIFODATA_REG);
    }


    fifo_addr = 128;
}

void iproc_i2s_refill_fifo(void)
{
    u32 val_l, val_r, flags;

    do
    {
        /* Enable FIFO mode, 4=write */
        flags = (1<<31) | (4<<16) | (fifo_addr & 127);
        writel(flags, IPROC_I2S_BASE_ADDR + I2S_FIFOADDRESS_REG);

        val_l = sinwave16_800[ptr];
        val_r = sinwave16_800[ptr++];
        
        flags = (val_l << 16) | (val_r & 0xffff);
        writel(flags, IPROC_I2S_BASE_ADDR + I2S_FIFODATA_REG);

        fifo_addr ++;
        if(ptr >= total_len)
        {
            ptr = 0;
            debug("%d\t", sin_loop++);
            dbg_print(".");
        }
   
        flags = readl(IPROC_I2S_BASE_ADDR + I2S_DEVSTATUS_REG);
    } while( flags & (1<<10) );
}
int iproc_i2s_init( int op )
{
    u32 flags;
    u32 txrx;

	debug("Enter %s\n", __FUNCTION__);


	/* If tx op */
	if ( op == 1 )
	{
	    txrx = 0 << I2S_DC_DPX_BIT;
	}
	/* if rx */
	else
	{
	    txrx = 1 << I2S_DC_DPX_BIT;
	}

    iproc_i2s_reset();

    /* Configure device, master mode, i2s, 16bit rx/tx word length, 48K sample rate
    / * L+R, BCLK generated internally */
    /* DPX=0 half_duplex, tx, BCLKD=0 internal */
    flags = (0<<I2S_DC_WL_RX_BIT) |
            (0<<I2S_DC_WL_TX_BIT) |
            (txrx)                |
            (0<<I2S_DC_BCLKD_BIT) | 
            (0<<I2S_DC_TDM_SEL_BIT);


    i2s_writel(flags, I2S_DEVCONTROL_REG);

    /* SPDIF off */
    i2s_writel(0, I2S_STXCTRL_REG);
    i2s_writel(0, I2S_TDMCH0_CTRL_REG);
    i2s_writel(0, I2S_TDMCH1_CTRL_REG);

    /* Reduce tx threshold */
    i2s_writel(0x7f, I2S_TXPLAYTH_REG);

    /* SR 48K , with 12.288Mhz MCLK input, 0x4-16khz, 0x1-48Khz */
    i2s_writel(/*0x4*/ 0x1, I2S_CLOCKDIVIDER_REG);


    /* INT ENABLE */
    i2s_writel(0xFF7FFFE0, I2S_INTMASK_REG);

    i2s_writel(0x0, I2S_CONTROL_REG);

	debug("i2s_Init Done\n");

    return 0;
}
#if swang

int iproc_i2s_rx_test(void)
{
    u32 flags;

    /* Set DMA */
    i2s_writel(0x89000000, I2S_DMA0_RCVADDRLOW_REG);
    i2s_writel(0, I2S_DMA0_RCVADDRHIGH_REG);

    i2s_writel(0x1800, I2S_DMA0_RCVPTR_REG);
    i2s_writel((1<<0)|(64<<1), I2S_DMA0_RCVCONTROL_REG);

    /* I2S control, start rec/rx */
    flags = (1<<1);
    i2s_writel(flags, I2S_CONTROL_REG);

	dbg_print("i2s_Rx test Start Recing\n");

    return 0;
}

void iproc_i2s_check_dma(void)
{
    dbg_print("\n\n");
    i2s_readl(I2S_DMA0_RCVSTATUS0_REG);
    i2s_readl(I2S_DMA0_RCVSTATUS1_REG);
    i2s_readl(I2S_DEVSTATUS_REG);
    i2s_readl(I2S_INTSTATUS_REG);
}

int iproc_i2s_tx_test(void)
{
    /* Fill Tx Fifo */
//    total_len = sizeof(sinwave16_800);
    total_len = 800;
    ptr = 0; sin_loop = 0;
    debug("\nSin_Buffer len = %d\n\n", total_len);
    iproc_i2s_fill_fifo();
}

int iproc_i2s_play(void)
{
    u32 flags;

    /* I2S control, start play/tx */
    flags = (1<<0);
    i2s_writel(flags, I2S_CONTROL_REG);

	debug("i2s_Tx test Start Playing\n");
    return 0;
}




//bit 28  EndOfTable (ET)  When set to 1, this field indicates that the current descriptor is the last entry in the table. The next descriptor is fetched from the address in the RcvAddrHigh/Low registers for receive descriptors or the XmtAddrHigh/Low registers for transmit descriptors.
//bit 29  IntOnComp (IC)  When this field is set to 1, the descriptor processor channel generates an interrupt when the transfer of data to or from the buffer associated with this descriptor is completed. When this field is cleared to 0, the descriptor processor channel does not generate an interrupt when the transfer completes. In transmit descriptors, this field is ignored unless the EOF field is also set. For receive descriptors, this field should be set only when transferring unframed data. When transferring framed data, the behavior of this field for receive descriptors is undefined; the lazy receive interrupt mechanism should be used instead. See the description of the IntRcvLazy register in the documentation for individual cores. Some cores do not support completion interrupts; see the interrupts descriptions in each core for more information.
//bit 30  EndOfFrame (EF)
//bit 31  StartOfFrame (SF)  When set to
#define  ET   (1<< 28)
#define  IntOnComp  ( 1<< 29 )
#define  EF   (1 << 30 )
#define  SF   (0x80000000)


//bit 46:32  BufCount (BC)  This field contains the length, in bytes, of the data buffer associated with this descriptor. A descriptor with a BufCount value greater than 0x4000 causes a descriptor protocol error.
//bit 49:48  AddrExt (AE)  When the DataBufPtr field addresses the PCIE core's large address window, the low bit of this field is used as bit 63 of the PCIE address used to access data. When the DataBufPtr field addresses the PCI core's large address window, the contents of this field is used as bits 31:30 of the PCI address used to access data. If DataBufPtr does not address either of these windows, this field must contain 0.
//bit 95:64  DataBufPtrLow (DL)  This field contains the lower half of the memory address of the first byte of the data buffer.
//bit 127:96  DataBufPtrHigh (DH) This field contains the upper half of the memory address of the first byte of the data buffer.



//DMA 0 control register

#define TXDMAPARITYDISABLE ( 1<< 11 )
#define TX_EN              (1)


void iproc_i2s_DMA_init ( void )
{
    u32 flags;
    u32 descTableBase =0x89200000;
    u32 *p, i;
    //a  DMA descriptor is 128 bit ( 4 32 bit- word)
    //the descriptor table is a linear array of descriptors
    //initialize the base address of the descriptor table , align on at least 4KB boundry
    u32 *pDescTbl = (u32*)descTableBase;

    //copy data to non-cacheable memory
//    memcpy((unsigned char *)AUDIO_DMA_BUF,sinwave16_800,  800 * 2);
    for(i=0; i<40; i++)
    {
    memset((unsigned int *)(AUDIO_DMA_BUF+400*i), 0x7fff7fff,  100);
    memset((unsigned int *)(AUDIO_DMA_BUF+400*i+100), 0x0,  100);
    memset((unsigned int *)(AUDIO_DMA_BUF+400*i+200), 0xffffffff,  100);
    memset((unsigned int *)(AUDIO_DMA_BUF+400*i+300), 0x0,  100);
    }

    //Configure DMA descriptors

    *pDescTbl++ = (ET | IntOnComp | EF |SF);
    *pDescTbl++ = 800 * 2; // This is the buffer count in bytes ( sine wave data )
    *pDescTbl++ = AUDIO_DMA_BUF; //Low half of the data buffer starting address
    *pDescTbl++  = 0;  //upper half Dont need this, it is 32 bit address;


    dbg_print("\n--- DMA descriptor ----\n");
    p = (u32*)descTableBase;
    for ( i = 0; i < 4; i++)
    {
        dbg_print("\n0x%x = 0x%x", (u32)p, *p );
        p++;
    }


    //End of descriptor table, there is only one descriptor in the table

    //Note
    //EndofTable is set for this descriptor and LastDescr points to value greater than this descriptor,
    // then DMA will loop starting with first descriptor to the descriptor with EOT bit set.


    //Configure DMA register

   //a. Program XmtAddrLow with the base address of Transmit Descriptors. As the descriptor
   //   is aligned to 16-byte address, bits 3:0 should always be zero.
   i2s_writel(descTableBase, I2S_DMA0_XMTADDRLOW_REG);

   dbg_print("\nXMT addr low  = 0x%x", i2s_readl(I2S_DMA0_XMTADDRLOW_REG));

   //c. Parity check can be disabled by setting XmtControl.txdmaParityCheckDisable.
   // It is by default enabled.

   flags = i2s_readl(I2S_DMA0_XMTCONTROL_REG);
   flags |= TXDMAPARITYDISABLE;
   i2s_writel(flags, I2S_DMA0_XMTCONTROL_REG);


   dbg_print("\nXMT CTRL  = 0x%x", i2s_readl(I2S_DMA0_XMTCONTROL_REG));



}

void iproc_i2s_DMA_start ( void )
{
    u32 flags;

    flags = i2s_readl(I2S_DMA0_XMTCONTROL_REG);
    flags |= TX_EN;
    i2s_writel(flags, I2S_DMA0_XMTCONTROL_REG);


    //d. XmtPtr.LastDescr is programmed to address of last byte of last descriptor + 1

    //Last byte of last desc + 1
    i2s_writel(100, I2S_DMA0_XMTPTR_REG);

    dbg_print("\nXMT PTR  = 0x%x", i2s_readl(I2S_DMA0_XMTPTR_REG));

    dbg_print("\nXMT CONTROL  = 0x%x", i2s_readl(I2S_DMA0_XMTCONTROL_REG));
}

void iproc_i2s_DMA_stop( void )
{
    u32 flags;

    //Stop DMA
    flags = i2s_readl(I2S_DMA0_XMTCONTROL_REG);
    flags &= ~TX_EN;
    i2s_writel(flags, I2S_DMA0_XMTCONTROL_REG);

    //Stop I2S playing
    flags = i2s_readl(I2S_CONTROL_REG);
    flags &= ~TX_EN;
    i2s_writel(flags,I2S_CONTROL_REG);


}
#endif

void iproc_i2s_DMA_init()
{


    i2sBlk.regs = (i2sregs_t*) IPROC_I2S_BASE_ADDR;

    ns_dmaInit();

    ns_dmaAttach(&i2sBlk);

    dma_txreset();

    dma_rxreset();

}

void iproc_i2s_DMA_RX_Enable(void)
{

    ns_dmaRxInit();
    //Enable receive
    ns_dmaEnable(DMA_RX);

    i2s_writel(0x2, I2S_CONTROL_REG);
}

void iproc_i2s_recording_start ( void )
{
    /* init the audio record buffer */
    memset(POST_RECORD_BUFFER,0, POST_RECORD_BUF_SIZE);

    /* Configure I2S  for receiving/recording */
    iproc_i2s_init(0);

    /* Init the DMA to capture data */
    iproc_i2s_DMA_init();

    /* Enable DMA RX from the FIFO */
    iproc_i2s_DMA_RX_Enable();


    while ( stopRecording == 0 )
    {
        iproc_i2s_DMA_rx();
    }
    //In case we want to do it again
    stopRecording = 1;

}

void iproc_i2s_DMA_rx( void )
{
    ns_dmaRx();
}

void iproc_i2s_DMA_copy_buffer( unsigned char *p, unsigned int len )
{
    static unsigned char *store = (unsigned char*)POST_RECORD_BUFFER;
    static int totalSize = POST_RECORD_BUF_SIZE;


    dbg_print("==>store is at 0x%x, totalSize  0x%x, len 0x%x\n", (unsigned int )store,totalSize,len);

    memcpy( store,p,len);

    store+=len;

    totalSize -= len;


    if ( totalSize <= 0 )
    {
        dbg_print("Size limit reached, end \n",totalSize );
        stopRecording = 1;
    }

}

void iproc_i2s_DMA_RX_Disable(void)
{
    dma_rxreset();
}

void iproc_i2s_DMA_TX_init(void)
{
    ns_dmaTxInit();
}

void iproc_i2s_DMA_TX_fillPkts( void )
{

}

void iproc_i2s_DMA_TX_Enable(void)
{
    ns_dmaEnable(DMA_TX);
}

void iproc_i2s_DMA_tx(void)
{
    int status;
    int count = 0;

    /* Configure I2S  for receiving/recording */
    iproc_i2s_init(1);

    /* Init the DMA to capture data */
    iproc_i2s_DMA_init();

    iproc_i2s_DMA_TX_init();
    iproc_i2s_DMA_TX_Enable();



    //start I2S playback
    i2s_writel(0x1, I2S_CONTROL_REG);

    while(count < 100)
    {
        ns_dmaTx(1600, (uint8_t *)sinwave16_800);

        status = ns_dmaTxWait(&i2sBlk);
        count++;
    }

}

void iproc_i2s_playback_recorded_audio( void )
{
    int numberOfPlays = 2;

    iproc_i2s_DMA_RX_Disable();

    /* Configure I2S */
    iproc_i2s_init(1);

    iproc_i2s_DMA_TX_init();
    iproc_i2s_DMA_TX_Enable();

    while ( numberOfPlays > 0  )
    {
        iproc_i2s_DMA_tx_recording();
        numberOfPlays--;
    }
}

void iproc_i2s_DMA_tx_recording(void)
{
    int status;
    unsigned char *p = (unsigned char*)POST_RECORD_BUFFER;
    int size = POST_RECORD_BUF_SIZE;
    int stepsize = 0x2048;

    //Start I2 playback
    i2s_writel(0x1, I2S_CONTROL_REG);
    i2s_writel(0x4, I2S_STXCTRL_REG);

    while(size >  0)
    {
        ns_dmaTx(stepsize, p);

        p+=stepsize;

        size -= stepsize;

        i2s_writel(0x1, I2S_CONTROL_REG);

        status = ns_dmaTxWait(&i2sBlk);
        dbg_print("TX start 0x%x, step size 0x%x , size 0x%x",p, stepsize, size);
    }

}



#if swang
///////////////////////////////////////////== OLD IMPLEMENTATION
void iproc_i2s_DMA_start ( void )
{
    u32 flags;

    flags = i2s_readl(I2S_DMA0_XMTCONTROL_REG);
    flags |= TX_EN;
    i2s_writel(flags, I2S_DMA0_XMTCONTROL_REG);


    //d. XmtPtr.LastDescr is programmed to address of last byte of last descriptor + 1

    //Last byte of last desc + 1
    i2s_writel(100, I2S_DMA0_XMTPTR_REG);

    dbg_print("\nXMT PTR  = 0x%x", i2s_readl(I2S_DMA0_XMTPTR_REG));

    dbg_print("\nXMT CONTROL  = 0x%x", i2s_readl(I2S_DMA0_XMTCONTROL_REG));
}

void iproc_i2s_DMA_stop( void )
{
    u32 flags;

    //Stop DMA
    flags = i2s_readl(I2S_DMA0_XMTCONTROL_REG);
    flags &= ~TX_EN;
    i2s_writel(flags, I2S_DMA0_XMTCONTROL_REG);

    //Stop I2S playing
    flags = i2s_readl(I2S_CONTROL_REG);
    flags &= ~TX_EN;
    i2s_writel(flags,I2S_CONTROL_REG);


}
#endif
