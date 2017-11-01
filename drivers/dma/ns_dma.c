/******************************************************************************/
/*                                                                            */
/*  Copyright 2012  Broadcom Corporation                                      */
/*                                                                            */
/*     Unless you and Broadcom execute a separate written software license    */
/*     agreement governing  use of this software, this software is licensed   */
/*     to you under the terms of the GNU General Public License version 2     */
/*     (the GPL), available at                                                */
/*                                                                            */
/*          http://www.broadcom.com/licenses/GPLv2.php                        */
/*                                                                            */
/*     with the following added to such license:                              */
/*                                                                            */
/*     As a special exception, the copyright holders of this software give    */
/*     you permission to link this software with independent modules, and to  */
/*     copy and distribute the resulting executable under terms of your       */
/*     choice, provided that you also meet, for each linked independent       */
/*     module, the terms and conditions of the license of that module.        */
/*     An independent module is a module which is not derived from this       */
/*     software.  The special exception does not apply to any modifications   */
/*     of the software.                                                       */
/*     Notwithstanding the above, under no circumstances may you combine      */
/*     this software in any way with any other Broadcom software provided     */
/*     under a license other than the GPL, without Broadcom's express prior   */
/*     written consent.                                                       */
/*                                                                            */
/******************************************************************************/
/*
 * Generic Broadcom Home Networking Division (HND) DMA engine SW interface
 * This supports the following chips: BCM42xx, 44xx, 47xx .
 *
 * $Id: ethHw_dma.h 241182 2011-02-17 21:50:03Z gmo $
 */


#include <config.h>
#include <common.h>
#include <asm/io.h>
#include <asm/arch-northstar/reg_utils.h>
#include <asm/arch-northstar/ns_dma.h>


#if (defined(CONFIG_NS_PLUS))
#define POST_UNCACHED_MEM_START     0x70000000
#define POST_UNCACHED_MEM_END       0x72000000
#else
#define POST_UNCACHED_MEM_START     0x89000000
#define POST_UNCACHED_MEM_END       0x8B000000
#endif
#define POST_UNCACHED_SIZE           0x2000000 //32MB

//#define DEBUG
#ifdef DEBUG
#define dbg_print   printf
#else
#define dbg_print
#endif

static dma_info_t dma_info, *di;

static uint32_t rxDescBuf = 0;
static uint32_t rxDescAlignedBuf = 0;
static uint32_t rxDataBuf = 0;
static uint32_t txDescBuf = 0;
static uint32_t txDescAlignedBuf = 0;
static uint32_t txDataBuf = 0;


static int dma_txload(int index, size_t len, uint8_t * tx_buf);
static int dma_rxenable(dma_info_t *di);
static void dma_txinit(dma_info_t *di);
static void dma_rxrefill(dma_info_t *di, int index);
static uint dma_ctrlflags(dma_info_t *di, uint mask, uint flags);
static void dma_rxinit(dma_info_t *di);
static void *dma_getnextrxp(dma_info_t *di, int *index, size_t *len, uint32_t *stat0, uint32_t *stat1);
static uint32_t chip_getintr_events(I2S_Block *p);


extern void iproc_i2s_DMA_copy_buffer( unsigned char *p, unsigned int len );

int ns_dmaInit( void )
{
    int buflen;
    uint32_t alloc_ptr = POST_UNCACHED_MEM_START;

    /* allocate rx descriptors buffer */
    buflen = RX_DESC_LEN+0x10;

    //rxDescBuf = (uint32_t)MALLOC(0, buflen);
    rxDescBuf = alloc_ptr;
    alloc_ptr += buflen;
    if (rxDescBuf == 0) {
        dbg_print("%s: Failed to allocate RX Descriptor memory\n", __FUNCTION__);
        return -1;
    }
    rxDescAlignedBuf = rxDescBuf;
    dbg_print("RX Descriptor Buffer: 0x%x; length: 0x%x\n", rxDescBuf, buflen);
    /* check if need to align buffer */
    if (rxDescAlignedBuf&0x0f) {
        /* align buffer */
        rxDescAlignedBuf = (rxDescBuf+0x10)&0xfffffff0;
    }
    //if(!(iproc_get_chipid() == 53010 && iproc_get_chiprev() < CHIP_REV_A2))
    //swang    rxDescAlignedBuf += 0x80000000; /* Move to ACP region */
    dbg_print("RX Descriptor Buffer (aligned): 0x%x; length: 0x%x\n", rxDescAlignedBuf, RX_DESC_LEN);

    /* allocate rx data buffer */
    buflen = RX_BUF_LEN;
    //rxDataBuf = (uint32_t)MALLOC(0, buflen);
    rxDataBuf = alloc_ptr;
    alloc_ptr += buflen;
    if (rxDataBuf == 0) {
        dbg_print("%s: Failed to allocate RX Data Buffer memory\n", __FUNCTION__);
        return -1;
    }
    //swangif(!(iproc_get_chipid() == 53010 && iproc_get_chiprev() < CHIP_REV_A2))
    //swang    rxDataBuf += 0x80000000; /* Move to ACP region */
    dbg_print("RX Data Buffer: 0x%x; length: 0x%x\n", rxDataBuf, buflen);

    /* allocate tx descriptors buffer */
    buflen = TX_DESC_LEN+0x10;
    //txDescBuf = (uint32_t)MALLOC(0, buflen);
    txDescBuf = alloc_ptr;
    alloc_ptr += buflen;
    if (txDescBuf == 0) {
        dbg_print("%s: Failed to allocate TX Descriptor memory\n", __FUNCTION__);
        return -1;
    }
    txDescAlignedBuf = txDescBuf;
    dbg_print("TX Descriptor Buffer: 0x%x; length: 0x%x\n", txDescBuf, buflen);
    /* check if need to align buffer */
    if (txDescAlignedBuf&0x0f) {
        /* align buffer */
        txDescAlignedBuf = (txDescBuf+0x10)&0xfffffff0;
    }
    //swang if(!(iproc_get_chipid() == 53010 && iproc_get_chiprev() < CHIP_REV_A2))
    //swang     txDescAlignedBuf += 0x80000000; /* Move to ACP region */
    dbg_print("TX Descriptor Buffer (aligned): 0x%x; length: 0x%x\n", txDescAlignedBuf, TX_DESC_LEN);

    /* allocate tx data buffer */
    buflen = TX_BUF_LEN;
    //txDataBuf = (uint32_t)MALLOC(0, buflen);
    txDataBuf = alloc_ptr;
    alloc_ptr += buflen;
    if (txDataBuf == 0) {
        dbg_print("%s: Failed to allocate TX Data Buffer memory\n", __FUNCTION__);
        return -1;
    }
    //swang if(!(iproc_get_chipid() == 53010 && iproc_get_chiprev() < CHIP_REV_A2))
        //swang txDataBuf += 0x80000000; /* Move to ACP region */
    dbg_print("TX Data Buffer: 0x%x; length: 0x%x\n", txDataBuf, buflen);



    di  = &dma_info;

    return 0;
}


int ns_dmaRxInit(void)
{
    dma_info_t *dma = di;
    uint32_t lastDscr;
    dma64dd_t *descp = NULL;
    uint8_t *bufp;
    int i;

    dbg_print(" %s enter\n",  __FUNCTION__);

    /* clear descriptor memory */
    BZERO_SM((void*)RX_DESC_BASE, RX_DESC_LEN);
    /* clear buffer memory */
    BZERO_SM((void*)RX_BUF_BASE, RX_BUF_LEN);

    /* Initialize RX DMA descriptor table */
    for (i = 0; i < RX_BUF_NUM; i++) {
        uint32_t ctrl;
        bufp = (uint8_t *) RX_BUF(i);
        descp = (dma64dd_t *) RX_DESC(i);
        ctrl = 0;
        /* if last descr set endOfTable */
        if (i==RX_BUF_NUM-1)
            ctrl = D64_CTRL1_EOT;
        descp->ctrl1 = ctrl;
        descp->ctrl2 = RX_BUF_SIZE;
        descp->addrlow = (uint32_t)bufp;
        descp->addrhigh = 0;
        /* flush descriptor */
        //gmac_cache_flush((u32)descp, sizeof(dma64dd_t));

        dbg_print("RX buf %d, desc reg ctl1 0x%x, ctl value 0x%x\n ",i, &descp->ctrl1, (unsigned int)descp->ctrl1);
        dbg_print("RX buf %d, desc reg ctl2 0x%x, RX buf size 0x%x\n ",i, &descp->ctrl2, (unsigned int)descp->ctrl2);
        dbg_print("RX buf %d, desc reg addrlow 0x%x, RX buf at 0x%x\n ",i, &descp->addrlow,(unsigned int) descp->addrlow);

        lastDscr = ((uint32_t)(descp) & D64_XP_LD_MASK) + sizeof(dma64dd_t);
    }

    /* initailize the DMA channel */
    reg32_write(&dma->d64rxregs->addrlow, RX_DESC_BASE);
    reg32_write(&dma->d64rxregs->addrhigh, 0);

    dbg_print("desc base reg 0x%x, value  0x%x\n ",&dma->d64rxregs->addrlow, reg32_read(&dma->d64rxregs->addrlow) );

    /* now update the dma last descriptor */
    reg32_write(&dma->d64rxregs->ptr, lastDscr);

    dbg_print("last desc reg 0x%x, lastDscr 0x%x\n ",&dma->d64rxregs->ptr, reg32_read(&dma->d64rxregs->ptr) );

    dma->rcvptrbase = RX_DESC_BASE;

    dbg_print("rcvptrbase 0x%x\n ",dma->rcvptrbase );

    return 0;
}

int ns_dmaTxInit( void )
{
    dma_info_t *dma = di;
    dma64dd_t *descp = NULL;
    uint8_t *bufp;
    int i;

    dbg_print(" %s enter\n",__FUNCTION__);

    /* clear descriptor memory */
    BZERO_SM((void*)TX_DESC_BASE, TX_DESC_LEN);
    /* clear buffer memory */
    BZERO_SM((void*)TX_BUF_BASE, TX_BUF_LEN);

    /* Initialize TX DMA descriptor table */
    for (i = 0; i < TX_BUF_NUM; i++) {
        uint32_t ctrl;
        bufp = (uint8_t *) TX_BUF(i);
        descp = (dma64dd_t *) TX_DESC(i);
        ctrl = 0;
        /* if last descr set endOfTable */
        if (i==TX_BUF_NUM-1)
            ctrl = D64_CTRL1_EOT;
        descp->ctrl1 = ctrl;
        descp->ctrl2 = 0;
        descp->addrlow = (uint32_t)bufp;
        descp->addrhigh = 0;
        /* flush descriptor */
        //gmac_cache_flush((u32)descp, sizeof(dma64dd_t));
    }

    /* initailize the DMA channel */
    reg32_write(&dma->d64txregs->addrlow, TX_DESC_BASE);
    reg32_write(&dma->d64txregs->addrhigh, 0);

    dma->xmtptrbase = TX_DESC_BASE;

    /* now update the dma last descriptor */
    reg32_write(&dma->d64txregs->ptr, TX_DESC_BASE&D64_XP_LD_MASK);

    return 0;
}


int ns_dmaTx(size_t len, uint8_t * tx_buf)
{
    /* kick off the dma */
    //bcm_eth_t *eth_data = &g_eth_data;
    //dma_info_t *di = eth_data->bcmgmac.di[0];
    int txout = di->txout;
    int ntxd = di->ntxd;
    uint32_t flags;
    dma64dd_t *descp = NULL;
    uint8_t *bufp;
    uint32_t ctrl2;
    uint32_t lastDscr = ((uint32_t)(TX_DESC(1)) & D64_XP_LD_MASK);
    size_t buflen;

    REG_DEBUG(1);
    dbg_print(" %s enter\n", __FUNCTION__);

    if (txout!=0 && txout!=1) {
        dbg_print("%s: ERROR - invalid txout 0x%x\n", __FUNCTION__, txout);
        REG_DEBUG(0);
        return -1;
    }

    /* load the buffer */
    buflen = dma_txload(txout, len, tx_buf);
    ctrl2 = (buflen & D64_CTRL2_BC_MASK);

    /* the transmit will only be one frame or set SOF, EOF */
    /* also set int on completion */
    flags = D64_CTRL1_SOF | D64_CTRL1_IOC | D64_CTRL1_EOF;

    /* txout points to the descriptor to uset */
    /* if last descriptor then set EOT */
    if (txout == (ntxd - 1)) {
        flags |= D64_CTRL1_EOT;
        lastDscr = ((uint32_t)(TX_DESC(0)) & D64_XP_LD_MASK);
    }

    /* write the descriptor */
    bufp = (uint8_t *) TX_BUF(txout);
    descp = (dma64dd_t *) TX_DESC(txout);
    descp->addrlow = (uint32_t)bufp;
    descp->addrhigh = 0;
    descp->ctrl1 = flags;
    descp->ctrl2 = ctrl2;

    /* flush descriptor and buffer */
    //gmac_cache_flush((u32)descp, sizeof(dma64dd_t));
    //gmac_cache_flush((u32)bufp, buflen);

    /* now update the dma last descriptor */
    reg32_write(&di->d64txregs->ptr, lastDscr);

    /* tx dma should be enabled so packet should go out */

    /* update txout */
    di->txout = NEXTTXD(txout);
    dbg_print("Next txout is %d\n", di->txout);
    REG_DEBUG(0);

    return 0;
}


int ns_dmaTxWait(I2S_Block *p)
{
    /* wait for tx to complete */
    //bcm_eth_t *eth_data = &g_eth_data;
    //bcmgmac_t *ch = &eth_data->bcmgmac;
    uint32_t intstatus;
    int xfrdone=false;
    int i=0;

    //REG_DEBUG(1);
    dbg_print(" %s enter\n",  __FUNCTION__);

    /* clear stored intstatus */
    //ch->intstatus = 0;

    intstatus = chip_getintr_events(p);

    dbg_print("int(0x%x)\n", intstatus);

    //if (intstatus & (I_XI0 | I_XI1 | I_XI2 | I_XI3))
    if (intstatus & (1<<24))
        xfrdone=true;
    //printf("Waiting for TX to complete");

    while (!xfrdone) {
        udelay(1000);
        //printf(".");
        i++;
        if (i > 200)
        {
            printf("\nDMA Tx failure! Already retried 200 times\n");
            //REG_DEBUG(0);
            return -1;
        }

        intstatus = chip_getintr_events(p);

        dbg_print("int(0x%x)", intstatus);
        if (intstatus & (1<<24)) //INT complete flag check
        {
            xfrdone=true;
        }
        else if (intstatus)
        {
            dbg_print("int(0x%x)", intstatus);

        }
    }


    return 0;
}
/* get current and pending interrupt events */
static uint32_t
chip_getintr_events(I2S_Block *p)
{
    i2sregs_t *regs = p->regs;
    //bcmgmac_t *ch = &eth_data->bcmgmac;
    uint32_t intstatus;
    //uint32_t temp;

    /* read the interrupt status register */
    intstatus = reg32_read(&regs->int_status);
    /* clear the int bits */
    reg32_write(&regs->int_status, intstatus);
    //temp = reg32_read(&regs->int_status);

    /* defer unsolicited interrupts */
    //intstatus &= (in_isr ? ch->intmask : ch->def_intmask);

    /* or new bits into persistent intstatus */
    //intstatus = (ch->intstatus |= intstatus);

    /* return intstatus */
    return intstatus;
}

int ns_dmaRx(void)
{
   // uint8_t *buf = (uint8_t *) NetRxPackets[0];
    //bcm_eth_t *eth_data = &g_eth_data;
    //bcmgmac_t *ch = &eth_data->bcmgmac;
    //dma_info_t *di = ch->di[0];
    void *bufp, *datap;
    int index;
    size_t rcvlen, buflen;
    uint32_t stat0, stat1;
    bool rxdata=false;
    int rc=0;
    uint32_t control, offset;
    uint8_t statbuf[HWRXOFF*2];

    //REG_DEBUG(1);
    //dbg_print(" %s enter\n", __FUNCTION__);

    udelay(50);

    while(1) {
        bufp = dma_getnextrxp(di, &index, &buflen, &stat0, &stat1);
        if (bufp) {
            dbg_print("received packet\n");
            dbg_print("bufp(0x%x) index(0x%x) buflen(0x%x) stat0(0x%x) stat1(0x%x)\n", (uint32_t)bufp, index, buflen, stat0, stat1);

            // get buffer offset
            control = reg32_read(&di->d64rxregs->control);
            offset = (control&D64_RC_RO_MASK)>>D64_RC_RO_SHIFT;
            rcvlen = ltoh16(*(uint16 *)bufp);

            if ((rcvlen == 0) || (rcvlen > RX_BUF_SIZE)) {
                dbg_print("Wrong RX packet size 0x%x drop it\n", rcvlen);
                /* refill buffre & descriptor */
                dma_rxrefill(di, index);
                break;
            }

            dbg_print("Received %d bytes\n", rcvlen);
            /* copy status into temp buf (need to copy all data out of buffer) */
            memcpy(statbuf, bufp, offset);
            datap = (void*)((uint32_t)bufp+offset);
            //memcpy(buf, datap, rcvlen);

            iproc_i2s_DMA_copy_buffer(datap, rcvlen);

            udelay(50);

            /* refill buffre & descriptor */
            dma_rxrefill(di, index);
            rxdata = true;
            break;
        } else {
            if (!rxdata) {
                /* Tell caller that no packet was received when Rx queue was polled */
                rc = -1;
                break;
            }
            /* at leasted received one packet */
            break;
        }
    }
    //REG_DEBUG(0);


    return rc;
}


int ns_dmaAttach(I2S_Block *p)
{
    //dma_info_t *di=NULL;
    //char name[16];

    dbg_print(" %s enter\n",  __FUNCTION__);

    /* DMA private info structure di is already inited */

    BZERO_SM(di, sizeof(dma_info_t));

    di->dma64 = 1;
    di->d64txregs = (dma64regs_t *)&p->regs->dma_regs[0].dmaxmt;
    di->d64rxregs = (dma64regs_t *)&p->regs->dma_regs[0].dmarcv;


    dbg_print("rx reg start 0x%x\n",di->d64rxregs);
    dbg_print("tx reg start 0x%x\n",di->d64txregs);




    /* Default flags: For backwards compatibility both Rx Overflow Continue
     * and Parity are DISABLED.
     * supports it.
     */
    dma_ctrlflags(di, DMA_CTRL_ROC | DMA_CTRL_PEN, 0);

   // di->rxburstlen = (reg32_read(&di->d64rxregs->control) & D64_RC_BL_MASK) >> D64_RC_BL_SHIFT;
   // di->txburstlen = (reg32_read(&di->d64txregs->control) & D64_XC_BL_MASK) >> D64_XC_BL_SHIFT;
   // dbg_print("rx burst len 0x%x\n", di->rxburstlen);
   // dbg_print("tx burst len 0x%x\n", di->txburstlen);

    di->ntxd = NTXD;
    di->nrxd = NRXD;

    di->rxbufsize = RX_BUF_SIZE;

    di->nrxpost = NRXBUFPOST;
    di->rxoffset = 0;//swang HWRXOFF; //swang

    //sprintf(name, "et%d", eth_data->unit);
    //strncpy(di->name, name, MAXNAMEL);

    /* load dma struct addr */
    p->di= di;

    return 0;
}

int ns_dmaDetach(dma_info_t *di)
{

    dbg_print("%s enter\n", __FUNCTION__);

    return 0;
}


int ns_dmaDisable(int dir)
{
    int stat;

    dbg_print("%s enter\n", __FUNCTION__);

    if (dir == DMA_TX)
        stat = dma_txreset();
    else
        stat = dma_rxreset();

    return stat;
}

int ns_dmaEnable(int dir)
{

    dbg_print("%s enter\n", __FUNCTION__);

    if (dir == DMA_TX)
        dma_txinit(di);
    else
        dma_rxinit(di);

    return 0;
}
static uint
dma_ctrlflags(dma_info_t *di, uint mask, uint flags)
{
    uint dmactrlflags;

    dbg_print("%s enter\n", __FUNCTION__);

    dmactrlflags = di->hnddma.dmactrlflags;
    //ASSERT((flags & ~mask) == 0);

    dmactrlflags &= ~mask;
    dmactrlflags |= flags;

    /* If trying to enable parity, check if parity is actually supported */
    if (dmactrlflags & DMA_CTRL_PEN) {
        uint32 control;

        control = reg32_read(&di->d64txregs->control);
        reg32_write(&di->d64txregs->control, control | D64_XC_PD);
        if (reg32_read(&di->d64txregs->control) & D64_XC_PD) {
            /* We *can* disable it so it is supported,
             * restore control register
             */
            reg32_write(&di->d64txregs->control, control);
        } else {
            /* Not supported, don't allow it to be enabled */
            dmactrlflags &= ~DMA_CTRL_PEN;
        }
    }

    di->hnddma.dmactrlflags = dmactrlflags;

    dbg_print("DMA contol 0x%x, set to 0x%x \n",&di->d64rxregs->control,reg32_read(&di->d64rxregs->control));

    return (dmactrlflags);
}


static int
dma_rxenable(dma_info_t *di)
{
    uint32_t dmactrlflags = di->hnddma.dmactrlflags;
    uint32_t rxoffset = di->rxoffset;
    uint32_t rxburstlen = di->rxburstlen;

    dbg_print("%s enter\n", __FUNCTION__);

    uint32 control = (reg32_read(&di->d64rxregs->control) & D64_RC_AE) | D64_RC_RE;

    if ((dmactrlflags & DMA_CTRL_PEN) == 0)
        control |= D64_RC_PD;

    if (dmactrlflags & DMA_CTRL_ROC)
        control |= D64_RC_OC;

    /* These bits 20:18 (burstLen) of control register can be written but will take
     * effect only if these bits are valid. So this will not affect previous versions
     * of the DMA. They will continue to have those bits set to 0.
     */
    //control &= ~D64_RC_BL_MASK;
    //control |= (rxburstlen << D64_RC_BL_SHIFT);
    control |= (rxoffset << D64_RC_RO_SHIFT);

    reg32_write(&di->d64rxregs->control, control);
    dbg_print("DMA contol 0x%x, set to 0x%x \n",&di->d64rxregs->control,reg32_read(&di->d64rxregs->control));
    return 0;
}


static void
dma_txinit(dma_info_t *di)
{
    uint32 control;

    dbg_print("%s enter\n", __FUNCTION__);

    if (di->ntxd == 0)
        return;

    di->txin = di->txout = 0;
    di->hnddma.txavail = di->ntxd - 1;

    /* These bits 20:18 (burstLen) of control register can be written but will take
     * effect only if these bits are valid. So this will not affect previous versions
     * of the DMA. They will continue to have those bits set to 0.
     */
    control = reg32_read(&di->d64txregs->control);

    //tx enable bit set
    control |= D64_XC_XE;
    if ((di->hnddma.dmactrlflags & DMA_CTRL_PEN) == 0)
        control |= D64_XC_PD;

    reg32_write(&di->d64txregs->control, control);

    /* initailize the DMA channel */
    reg32_write(&di->d64txregs->addrlow, TX_DESC_BASE);
    reg32_write(&di->d64txregs->addrhigh, 0);
}


static void
dma_rxinit(dma_info_t *di)
{
    dbg_print("%s enter\n", __FUNCTION__);

    di->rxin = di->rxout = 0;

    dma_rxenable(di);

    /* the rx descriptor ring should have the addresses set properly */
    /* set the lastdscr for the rx ring */
    reg32_write(&di->d64rxregs->ptr, ((uint32_t)RX_DESC(RX_BUF_NUM)&D64_XP_LD_MASK));

    dbg_print("DMA last desc reg 0x%x, set to 0x%x \n",&di->d64rxregs->ptr,reg32_read(&di->d64rxregs->ptr));
    dbg_print("DMA active desc reg 0x%x, is 0x%x \n",&di->d64rxregs->status1,reg32_read(&di->d64rxregs->status1));
    dbg_print("DMA current desc reg 0x%x, is 0x%x \n",&di->d64rxregs->status0,reg32_read(&di->d64rxregs->status0));

}


bool
dma_txreset( void )
{
    uint32 status;

    dbg_print("%s enter\n", __FUNCTION__);

    if (di->ntxd == 0)
        return TRUE;

    /* address PR8249/PR7577 issue */
    /* suspend tx DMA first */
    reg32_write(&di->d64txregs->control, D64_XC_SE);
    SPINWAIT(((status = (reg32_read(&di->d64txregs->status0) & D64_XS0_XS_MASK)) !=
              D64_XS0_XS_DISABLED) &&
             (status != D64_XS0_XS_IDLE) &&
             (status != D64_XS0_XS_STOPPED),
             10000);

    /* PR2414 WAR: DMA engines are not disabled until transfer finishes */
    reg32_write(&di->d64txregs->control, 0);
    SPINWAIT(((status = (reg32_read(&di->d64txregs->status0) & D64_XS0_XS_MASK)) !=
              D64_XS0_XS_DISABLED),
             10000);

    /* wait for the last transaction to complete */
    udelay(300);


    return (status == D64_XS0_XS_DISABLED);
}


bool
dma_rxreset()
{
    uint32 status;

    dbg_print("%s enter\n", __FUNCTION__);

    if (di->nrxd == 0)
        return TRUE;

    /* PR2414 WAR: DMA engines are not disabled until transfer finishes */
    reg32_write(&di->d64rxregs->control, 0);
    SPINWAIT(((status = (reg32_read(&di->d64rxregs->status0) & D64_RS0_RS_MASK)) !=
              D64_RS0_RS_DISABLED), 10000);

    dbg_print("DMA status 0 reg 0x%x, is 0x%x \n",&di->d64rxregs->status0,reg32_read(&di->d64rxregs->status0));
    dbg_print("DMA status 1 reg 0x%x, is 0x%x \n",&di->d64rxregs->status1,reg32_read(&di->d64rxregs->status1));

    return (status == D64_RS0_RS_DISABLED);
}


static int
dma_txload(int index, size_t len, uint8_t * tx_buf)
{


    //dbg_print("%s enter\n", __FUNCTION__);



    /* copy buffer */
    memcpy((uint8_t *) TX_BUF(index), tx_buf, len);


    //dbg_print("TX DMA buf:0x%x; len:%d\n", (uint32_t)TX_BUF(index), len);
    //txDump((uint8_t *)TX_BUF(index), tlen);


    return len;
}


/* this api will check if a packet has been received.  If so it will return the
   address of the buffer and enter a bogus address into the descriptor table
   rxin will be incremented to the next descriptor.
   Once done with the frame the buffer should be added back onto the descriptor
   and the lastdscr should be updated to this descriptor. */
static void *
dma_getnextrxp(dma_info_t *di, int *index, size_t *len, uint32_t *stat0, uint32_t *stat1)
{
    dma64dd_t *descp = NULL;
    uint i, curr, active;
    void *rxp;

    /* initialize return parameters */
    *index = 0;
    *len = 0;
    *stat0 = 0;
    *stat1 = 0;

    i = di->rxin;
#if swang
    dbg_print("DMA active desc reg 0x%x, is 0x%x \n",&di->d64rxregs->status1,reg32_read(&di->d64rxregs->status1));
    dbg_print("DMA current desc reg 0x%x, is 0x%x \n",&di->d64rxregs->status0,reg32_read(&di->d64rxregs->status0));
#endif
    curr = B2I(((reg32_read(&di->d64rxregs->status0) & D64_RS0_CD_MASK) -
        di->rcvptrbase) & D64_RS0_CD_MASK, dma64dd_t);
    active = B2I(((reg32_read(&di->d64rxregs->status1) & D64_RS0_CD_MASK) -
        di->rcvptrbase) & D64_RS0_CD_MASK, dma64dd_t);

    /* check if any frame */
    if (i == curr)
        return (NULL);

    dbg_print("rxin(0x%x) curr(0x%x) active(0x%x)\n", i, curr, active);

    /* get the packet pointer that corresponds to the rx descriptor */
    rxp = (void*)RX_BUF(i);

    descp = (dma64dd_t *)RX_DESC(i);
    /* invalidate buffer */
    //gmac_cache_inval((u32)rxp, RX_BUF_LEN);
    //gmac_cache_inval((u32)descp, sizeof(dma64dd_t);

    descp->addrlow = 0xdeadbeef;
    descp->addrhigh = 0xdeadbeef;

    *index = i;
    *len = (descp->ctrl2&D64_CTRL2_BC_MASK);
    *stat0 = reg32_read(&di->d64rxregs->status0);
    *stat1 = reg32_read(&di->d64rxregs->status1);

    di->rxin = NEXTRXD(i);

    return (rxp);
}


/* Restore the buffer back on to the descriptor and
   then lastdscr should be updated to this descriptor. */
static void
dma_rxrefill(dma_info_t *di, int index)
{
    dma64dd_t *descp = NULL;
    void *bufp;

    /* get the packet pointer that corresponds to the rx descriptor */
    bufp = (void*)RX_BUF(index);
    descp = (dma64dd_t *)RX_DESC(index);

    /* update descriptor that is being added back on ring */
    descp->ctrl2 = RX_BUF_SIZE;
    descp->addrlow = (uint32_t)bufp;
    descp->addrhigh = 0;
    /* flush descriptor */
    //gmac_cache_flush((u32)descp, sizeof(dma64dd_t));
}
