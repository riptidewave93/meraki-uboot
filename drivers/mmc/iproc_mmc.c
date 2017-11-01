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
#include <mmc.h>
#include <asm/io.h>

#include "iproc_mmc.h"
#include "brcm_rdb_emmcsdxc.h"

#if defined(CONFIG_NS_PLUS)

#define DRIVER_NAME           "iproc_mmc"
#define IPROC_SD_BASE_ADDR    0x18021000
#define IPROC_SDIO_HCVERSIRQ		0x180210fc
#define SDIO_IDM_IDM_RESET_CONTROL	0x18117800

#define IPROC_DMU_BASE_REG      0x1803f000
#define CRU_LCPLL_CONTROL0      0x100
#define CRU_LCPLL_CONTROL2      0x108
#define LCPLL_CH1_MDIV_MASK     0xFF
#define LCPLL_CH1_MDIV_SHIFT    16
#define CRU_CLKSET_KEY          0x180

#else

#define DRIVER_NAME           "iproc_mmc"
#define IPROC_SD_BASE_ADDR    0x18020000
#define IPROC_SDIO_HCVERSIRQ		0x180200fc
#define SDIO_IDM_IDM_RESET_CONTROL	0x18116800

#define IPROC_DMU_BASE_REG      0x1800c000
#define CRU_LCPLL_CONTROL0      0x100
#define CRU_LCPLL_CONTROL2      0x108
#define LCPLL_CH1_MDIV_MASK     0xFF
#define LCPLL_CH1_MDIV_SHIFT    16
#define CRU_CLKSET_KEY          0x180
#endif



/* support 1 mmc host -> SDIO */
struct mmc 		mmc_dev;
struct mmc_host mmc_host;

static void mmc_prepare_data(struct mmc_host *host, struct mmc_data *data)
{
	unsigned int temp = 0 ;

	debug("Enter %s\n", __FUNCTION__);

	debug("data->dest: %08x, size: %d\n", (u32)data->dest, data->blocksize);
	writel((u32)data->dest, &host->reg->sysad);
    debug("dest read back: %08x\n", readl(&host->reg->sysad));

	// Set up block size and block count. 
	// For big island it is required to set HSBS field of block register tp 0x7.
	// Problem : When data is DMA across page boundaries, if HSBS field is set to 0,
	//           we see that transfer doesn't finish, and we see a hang.
	//           E.g : let's say a buffer in memory has a start address of 0x86234EF0.
	//                 During a read operation, data is read from SD in sizes of 512 bytes ( 0x200).
	//                 So now DMA of data will happen across address 0x86234F00, which is
	//                 a 4K page boundary. 
	//                 We see failures in this case when HSBS field is set to 0.
	//                 Solution is to set HSBS field val to 0x7 so that boundary DMA transactions are safe.
	//
//	temp = ( 7 << EMMCSDXC_BLOCK_HSBS_SHIFT ) | data->blocksize | ( data->blocks << EMMCSDXC_BLOCK_BCNT_SHIFT ) ;
	temp = ( 0 << EMMCSDXC_BLOCK_HSBS_SHIFT ) | data->blocksize | ( data->blocks << EMMCSDXC_BLOCK_BCNT_SHIFT ) ;
	writel(temp, &host->reg->blkcnt_sz);
}

static void iproc_mmc_clear_all_intrs(struct mmc_host *host)
{
	// Clear all interrupts as per FPGA code.
	//printf("Clearing all intrs... \n") ;
	writel(0xFFFFFFFF, &host->reg->norintsts ) ; // Clear all interrupts.
	// udelay(100000); // Working value.
	udelay(1000);
}

static void iproc_mmc_decode_cmd(unsigned int cmd)
{
    debug("Cmd=%08x,", cmd);
    debug("Cidx=%d,", cmd>>24);
    debug("ctyp=%d,", (cmd>>22)&3);
    debug("dps=%d,",  (cmd>>21)&1);
    debug("cchk=%d,", (cmd>>20)&1);
    debug("crc=%d,",  (cmd>>19)&1);
    debug("rtsel=%d,", (cmd>>16)&3);
    debug("msbs=%d,", (cmd>>5)&1);
    debug("r/w=%d,",  (cmd>>4)&1);
    debug("bcen=%d,", (cmd>>1)&1);
    debug("dma=%d\n",  (cmd>>0)&1);
}

static int iproc_mmc_send_cmd(struct mmc *mmc, struct mmc_cmd *cmd,
			struct mmc_data *data)
{
	struct mmc_host *host = (struct mmc_host *)mmc->priv;
	int flags = 0 ;
	int i = 0 ;
	unsigned int timeout;
	unsigned int mask;
	unsigned int cmd_complete_retry = 100;
	unsigned int resp_retry = 10000;

	debug("Send_Cmd: ");

	/* Wait max 10 ms */
	timeout = 10;

	/*
	 * PRNSTS
	 * CMDINHDAT[1]	: Command Inhibit (DAT)
	 * CMDINHCMD[0]	: Command Inhibit (CMD)
	 */
	mask = (1 << EMMCSDXC_PSTATE_CMDINH_SHIFT);  // Set command inhibit.
	if ((data != NULL) || (cmd->resp_type & MMC_RSP_BUSY))
		mask |= (1 << EMMCSDXC_PSTATE_DATINH_SHIFT); // Set dat inhibit.

	/*
	 * We shouldn't wait for data inihibit for stop commands, even
	 * though they might use busy signaling
	 */
	if (data)
		mask &= ~(1 << EMMCSDXC_PSTATE_DATINH_SHIFT);

    while (readl(&host->reg->prnsts) & mask) {
            if (timeout == 0) {
                    printf("%s : timeout error\n", __func__);
                    return TIMEOUT ;
            }
            timeout--;
            udelay(10000);
    }

	// Set up block cnt, and block size.
	if (data)
       		mmc_prepare_data(host, data);

	debug("cmd->arg: %08x\n", cmd->cmdarg);
	if ( cmd->cmdidx == 17 || cmd->cmdidx == 24 ) { printf(".") ; }
	writel(cmd->cmdarg, &host->reg->argument);


	flags = 0 ;
	if ( data ) 
	{
//		flags = (1 << EMMCSDXC_CMD_BCEN_SHIFT) | (1 << EMMCSDXC_CMD_DMA_SHIFT);
        flags = (1 << EMMCSDXC_CMD_BCEN_SHIFT);
        debug("Data_Send: PIO, BCEN=1\n");

		if (data->blocks > 1)
			flags |= (1 << EMMCSDXC_CMD_MSBS_SHIFT); // Multiple block select.
		if (data->flags & MMC_DATA_READ)
			flags |= (1 << EMMCSDXC_CMD_DTDS_SHIFT); // 1= read, 0=write.
	} 

	if ((cmd->resp_type & MMC_RSP_136) && (cmd->resp_type & MMC_RSP_BUSY))
		return -1;

	/*
	 * CMDREG
	 * CMDIDX[29:24]: Command index
	 * DPS[21]	: Data Present Select
	 * CCHK_EN[20]	: Command Index Check Enable
	 * CRC_EN[19]	: Command CRC Check Enable
	 * RTSEL[1:0]
	 *	00 = No Response
	 *	01 = Length 136
	 *	10 = Length 48
	 *	11 = Length 48 Check busy after response
	 */
	if (!(cmd->resp_type & MMC_RSP_PRESENT))
		flags |= (0 << EMMCSDXC_CMD_RTSEL_SHIFT);
	else if (cmd->resp_type & MMC_RSP_136)
		flags |= (1 << EMMCSDXC_CMD_RTSEL_SHIFT);
	else if (cmd->resp_type & MMC_RSP_BUSY)
		flags |= (3 << EMMCSDXC_CMD_RTSEL_SHIFT);
	else
		flags |= (2 << EMMCSDXC_CMD_RTSEL_SHIFT);

	if (cmd->resp_type & MMC_RSP_CRC)
		flags |= (1 << EMMCSDXC_CMD_CRC_EN_SHIFT);
	if (cmd->resp_type & MMC_RSP_OPCODE)
		flags |= (1 << EMMCSDXC_CMD_CCHK_EN_SHIFT);
	if (data)
		flags |= (1 << EMMCSDXC_CMD_DPS_SHIFT);

	debug("cmd: %d\n", cmd->cmdidx);
	flags |= ( cmd->cmdidx << EMMCSDXC_CMD_CIDX_SHIFT ) ;
    iproc_mmc_decode_cmd(flags);

	writel(flags, &host->reg->cmdreg);

    iproc_mmc_decode_cmd(readl(&host->reg->cmdreg));
	udelay(10000) ;
	
	for (i = 0; i < cmd_complete_retry; i++) {
		mask = readl(&host->reg->norintsts);
		/* Command Complete */
		if (mask & (1 << 0)) {
			if (!data)
				writel(mask, &host->reg->norintsts);
			break;
		}
		udelay(1000); /* Needed for cmd0 */
	}

   if (i == cmd_complete_retry) {
      	debug("%s: waiting for status update for cmd %d\n", __func__, cmd->cmdidx);
		// Set CMDRST and DATARST bits. 
		// Problem :
		// -------
		// When a command 8 is sent in case of MMC card, it will not respond, and CMD INHIBIT bit
		// of PRSTATUS register will be set to 1, causing no more commands to be sent from host controller.
		// This causes things to stall. 
		// Solution :
		// ---------
		// In order to avoid this situation, we clear the CMDRST and DATARST bits in the case when card 
		// doesn't respond back to a command sent by host controller.
        writel( ( ( 0x3 << EMMCSDXC_CTRL1_CMDRST_SHIFT )| ( readl(&host->reg->ctrl1_clkcon_timeout_swrst))), &host->reg->ctrl1_clkcon_timeout_swrst);
		iproc_mmc_clear_all_intrs(host) ;
		return TIMEOUT;
    }

    if (mask & (1 << 16)) {
	    /* Timeout Error */
        printf("timeout: %08x cmd %d\n", mask, cmd->cmdidx);
		// Clear up the CMD inhibit and DATA inhibit bits.
	    return TIMEOUT;

    } else if (mask & (1 << 15)) {
        /* Error Interrupt */
        printf("error: %08x cmd %d\n", mask, cmd->cmdidx);
	    return -1;
    }

	if (cmd->resp_type & MMC_RSP_PRESENT) {
		if (cmd->resp_type & MMC_RSP_136) {
			/* CRC is stripped so we need to do some shifting. */
			for (i = 0; i < 4; i++) {
				unsigned int offset =
					(unsigned int)(&host->reg->rspreg3 - i);
				cmd->response[i] = readl(offset) << 8;

				if (i != 3) {
					cmd->response[i] |= readb(offset - 1);
				}
				debug("cmd->resp[%d]: %08x\n", i, cmd->response[i]);
			}
		} else if (cmd->resp_type & MMC_RSP_BUSY) {
			for (i = 0; i < resp_retry; i++) {
				/* PRNTDATA[23:20] : DAT[3:0] Line Signal */
				if (readl(&host->reg->prnsts)
					& (1 << 20))	/* DAT[0] */
					break;
			}

			if (i == resp_retry) {
				printf("%s: card is still busy resp_retry=%u\n", __func__, resp_retry);
				return TIMEOUT;
			}

			cmd->response[0] = readl(&host->reg->rspreg0);
			debug("cmd->resp[0]: %08x\n", cmd->response[0]);
		} else {
			cmd->response[0] = readl(&host->reg->rspreg0);
			debug("cmd->resp[0]: %08x\n", cmd->response[0]);
		}
	}

    /* Read PIO */
    if (data && (data->flags & MMC_DATA_READ) )
    {
        unsigned int len = data->blocksize;
        unsigned int chunk, scratch;
    	unsigned char *buf;

        while (1) {
			mask = readl(&host->reg->norintsts);
            debug("INT_sts: 0x%08x\n", mask);

            if (mask & (1<<0))
                debug("cmd completed\n");
			if (mask & (1 << 5)) {
				debug("Buf Rd Ready\n");
                break;
            }
			if (mask & (1 << 15)) {
        		/* Error Interrupt */
	            writel(mask, &host->reg->norintsts);
       		 	printf("%s: error during transfer: 0x%08x\n", __func__, mask);
                return -1;
            }
        }

        /* Loop read */
		buf = (unsigned char *)data->dest;
        chunk = 0; scratch = 0;
		while (len) {
			if (chunk == 0) {
				scratch = readl(&host->reg->bdata);
				chunk = 4;
                debug("bdata=%08x\t", scratch);
			}

			*buf = scratch & 0xFF;

			buf++;
			scratch >>= 8;
			chunk--;
			len--;
		}
        debug("\n");

        while (1) {
			mask = readl(&host->reg->norintsts);
            debug("INT_sts: 0x%08x\n", mask);

            if (mask & (1<<0))
                debug("cmd completed\n");
			if (mask & (1 << 1)) {
				debug("Rx Completed\n");
                break;
            }
			if (mask & (1 << 15)) {
        		/* Error Interrupt */
	            writel(mask, &host->reg->norintsts);
       		 	printf("%s: error during transfer: 0x%08x\n", __func__, mask);
                return -1;
            }
        }
    }

    /* Write PIO */
    else if (data && !(data->flags & MMC_DATA_READ) )
    {
        unsigned int len = data->blocksize;
        unsigned int chunk, scratch, blksize=512;
    	unsigned char *buf;

        while (1) {
			mask = readl(&host->reg->norintsts);
            debug("INT_sts: 0x%08x\n", mask);

            if (mask & (1<<0))
                debug("cmd completed\n");
			if (mask & (1 << 4)) {
				debug("Buf Wr Ready\n");
                break;
            }
			if (mask & (1 << 15)) {
        		/* Error Interrupt */
	            writel(mask, &host->reg->norintsts);
       		 	printf("%s: error during transfer: 0x%08x\n", __func__, mask);
                return -1;
            }
        }

        buf = (unsigned int *)data->dest;
        chunk = 0; scratch = 0;
		while (len) {
			scratch |= (u32)*buf << (chunk * 8);

			buf++;
			chunk++;
			len--;

			if ((chunk == 4) || ((len == 0) && (blksize == 0))) {
				writel(scratch, &host->reg->bdata);
                debug("bdata=%08x\n", scratch );
				chunk = 0;
				scratch = 0;
			}
		}
        debug("\n");

        while (1) {
			mask = readl(&host->reg->norintsts);
            debug("INT_sts: 0x%08x\n", mask);

            if (mask & (1<<0))
                debug("cmd completed\n");
			if (mask & (1 << 1)) {
				debug("Tx Completed\n");
                break;
            }
			if (mask & (1 << 15)) {
        		/* Error Interrupt */
	            writel(mask, &host->reg->norintsts);
       		 	printf("%s: error during transfer: 0x%08x\n", __func__, mask);
                return -1;
            }
        }

    } // if PIO

	// Clear all interrupts as per FPGA code.
	writel(0xFFFFFFFF, &host->reg->norintsts ) ;
	// udelay(100000); // Working value.
	udelay(1000);
	// udelay(10000);

	debug("\n");
	return 0;
}


static void iproc_mmc_change_clock(struct mmc_host *host, uint clock)
{
	int div = 0;
	unsigned int clk;
	unsigned long timeout;

	debug("Enter %s\n", __FUNCTION__);

	clk = readl(&host->reg->ctrl1_clkcon_timeout_swrst) ;
	clk = clk & 0xFFFF0000  ; // Clean up all bits related to clock.
	writel(clk, &host->reg->ctrl1_clkcon_timeout_swrst);
	clk = 0 ;

	if (clock == 0)
		return;
	if (clock <= 400000)
		div = 130;
	else if (clock <= 20000000)
		div = 4;
	else if (clock <= 26000000)
		div = 2;
	else
		div = 1;
	debug("div: %d\n", div);

	div >>= 1;

	// Write divider value, and enable internal clock.
	clk = readl(&host->reg->ctrl1_clkcon_timeout_swrst) | (div << EMMCSDXC_CTRL1_SDCLKSEL_SHIFT) | (1 << EMMCSDXC_CTRL1_ICLKEN_SHIFT) ;
	writel(clk, &host->reg->ctrl1_clkcon_timeout_swrst);

	/* Wait for clock to stabilize */
	/* Wait max 10 ms */
	timeout = 10;
	while (!(readl(&host->reg->ctrl1_clkcon_timeout_swrst) & (1 << EMMCSDXC_CTRL1_ICLKSTB_SHIFT))) {
		if (timeout == 0) {
			printf("%s: timeout error\n", __func__);
			return;
		}
		timeout--;
		udelay(10000);
	}

	// Enable sdio clock now.
	clk |= (1 << EMMCSDXC_CTRL1_SDCLKEN_SHIFT) | readl(&host->reg->ctrl1_clkcon_timeout_swrst) ;
	writel(clk, &host->reg->ctrl1_clkcon_timeout_swrst);

	host->clock = clock;
}

static void iproc_mmc_set_ios(struct mmc *mmc)
{
	struct mmc_host *host = mmc->priv;
	unsigned char ctrl;

	debug("Enter %s\n", __FUNCTION__);
	debug("bus_width: %x, clock: %d\n", mmc->bus_width, mmc->clock);

	iproc_mmc_change_clock(host, mmc->clock);

	// Width and edge setting.
	// WIDTH : 
	ctrl = readl(&host->reg->ctrl_host_pwr_blk_wak);
	/*  1 = 4-bit mode , 0 = 1-bit mode */
	if (mmc->bus_width == 4)
		ctrl |= (1 << EMMCSDXC_CTRL_DXTW_SHIFT);
	else
		ctrl &= ~(1 << EMMCSDXC_CTRL_DXTW_SHIFT);

	// No HS mode.
	ctrl &= ~(1 << EMMCSDXC_CTRL_HSEN_SHIFT);
	writel(ctrl, &host->reg->ctrl_host_pwr_blk_wak);

#if defined(CONFIG_NS_PLUS)
	writel(0xd14, 0x18021028);
	//writel(0x90207, 0x1802102c);
	writel(0x90207, 0x1802102c);
#else
    /* Control setting, 1-bit, High-Capa, SDR25 */
	writel(0xd14, 0x18020028);
	writel(0x90207, 0x1802002c);
#endif

}

static void iproc_mmc_reset(struct mmc_host *host)
{
    unsigned int timeout;

	debug("Enter %s\n", __FUNCTION__);

    /* Software reset for all * 1 = reset * 0 = work */
    writel((1 << EMMCSDXC_CTRL1_RST_SHIFT), &host->reg->ctrl1_clkcon_timeout_swrst);

    host->clock = 0;

    /* Wait max 100 ms */
    timeout = 100;

    /* hw clears the bit when it's done */
    while (readl(&host->reg->ctrl1_clkcon_timeout_swrst) & (1 << EMMCSDXC_CTRL1_RST_SHIFT)) {
        if (timeout == 0) {
            printf("%s: timeout error\n", __func__);
            return;
        }
        timeout--;
        udelay(1000);
    }
	debug("Reset Done\n");
}

static int iproc_mmc_core_init(struct mmc *mmc)
{
    struct mmc_host *host = (struct mmc_host *)mmc->priv;
    unsigned int mask;

	debug("Enter %s\n", __FUNCTION__);

    // Set power now.
    mask = readl(&host->reg->ctrl_host_pwr_blk_wak) | ( 7 << EMMCSDXC_CTRL_SDVSEL_SHIFT ) | EMMCSDXC_CTRL_SDPWR_MASK ;
    writel(mask, &host->reg->ctrl_host_pwr_blk_wak ) ;    

    iproc_mmc_reset(host);

    host->version = ( readl(IPROC_SDIO_HCVERSIRQ) | EMMCSDXC_HCVERSIRQ_VENDVER_MASK ) >> EMMCSDXC_HCVERSIRQ_VENDVER_SHIFT ;

    /* mask all */
    writel(0xffffffff, &host->reg->norintstsen);
    writel(0xffffffff, &host->reg->norintsigen);

    writel( ( ( 0xd << EMMCSDXC_CTRL1_DTCNT_SHIFT )| ( readl(&host->reg->ctrl1_clkcon_timeout_swrst))), &host->reg->ctrl1_clkcon_timeout_swrst);	/* TMCLK * 2^26 */

    /*
    * Interrupt Status Enable Register init
    * bit 5 : Buffer Read Ready Status Enable
    * bit 4 : Buffer write Ready Status Enable
    * bit 1 : Transfre Complete Status Enable
    * bit 0 : Command Complete Status Enable
    */
    mask = readl(&host->reg->norintstsen);
    mask &= ~(0xffff);
    mask |= (1 << EMMCSDXC_INTREN1_BUFRREN_SHIFT) | (1 << EMMCSDXC_INTREN1_BUFWREN_SHIFT) | (1 << EMMCSDXC_INTREN1_TXDONEEN_SHIFT) | (1 << EMMCSDXC_INTREN1_CMDDONEEN_SHIFT) | (1 << EMMCSDXC_INTREN1_DMAIRQEN_SHIFT) ;
    writel(mask, &host->reg->norintstsen);

    /*
    * Interrupt Signal Enable Register init
    * bit 1 : Transfer Complete Signal Enable
    */
    mask = readl(&host->reg->norintsigen);
    mask &= ~(0xffff);
    mask |= (1 << EMMCSDXC_INTREN2_TXDONE_SHIFT);
    writel(mask, &host->reg->norintsigen);

	debug("MMC_Init Done\n");
    return 0;
}

void iproc_sd_clock_config(void)
{
	debug("Enter %s\n", __FUNCTION__);

    if(0) {
            /* some card can'r work properly @ 100M , 
            adjust the clock source from 200MHz to 160MHz.
            Then the effective freq will be 160MHz/2 = 80MHz */

            u32 val;
            u32 dmu_base = IPROC_DMU_BASE_REG;
			debug("DMU_Base_VA = 0x%08x\n", dmu_base);

            writel(0xea68, dmu_base + CRU_CLKSET_KEY);
            val = readl(dmu_base + CRU_LCPLL_CONTROL2);
            val &= ~(LCPLL_CH1_MDIV_MASK << LCPLL_CH1_MDIV_SHIFT);
            /* SDIO_FREQ/20 = 160MHz */
            val |= (20 << LCPLL_CH1_MDIV_SHIFT);
            writel(val, dmu_base + CRU_LCPLL_CONTROL2);
            val = readl(dmu_base + CRU_LCPLL_CONTROL0);
            val &= ~0x3;            
            writel(val, dmu_base + CRU_LCPLL_CONTROL0);
            val |= 2;           
            writel(val, dmu_base + CRU_LCPLL_CONTROL0);
            val &= ~0x3;            
            writel(val, dmu_base + CRU_LCPLL_CONTROL0);
            writel(0x0, dmu_base + CRU_CLKSET_KEY);                              
		}	
    else
    {
    	//printf("\n Not touching the clock config!!!!\n");
    }

}


int board_mmc_init(bd_t *bis)
{
    struct mmc *mmc;

	debug("Enter %s\n", __FUNCTION__);

	/* Release reset */
	writel(0x0, SDIO_IDM_IDM_RESET_CONTROL);

	/* Enable DMU clock control */
	iproc_sd_clock_config();

    mmc = &mmc_dev;	 
    sprintf(mmc->name, DRIVER_NAME);
    mmc->priv = &mmc_host;
    mmc->send_cmd = iproc_mmc_send_cmd;
    mmc->set_ios = iproc_mmc_set_ios;
    mmc->init = iproc_mmc_core_init;

    mmc->voltages = MMC_VDD_27_28 | MMC_VDD_28_29 | MMC_VDD_29_30 | MMC_VDD_30_31 | MMC_VDD_31_32 | MMC_VDD_32_33 | MMC_VDD_33_34 | MMC_VDD_34_35 | MMC_VDD_35_36 ;

	/* uboot core mmc.c supports 4-bit mode so be consistent even though 8-bit works on eMMC cards we have used */
    mmc->host_caps = MMC_MODE_4BIT | MMC_MODE_HS_52MHz | MMC_MODE_HS;
    mmc->f_min = 400000;  
    mmc->f_max = 48000000; 

    mmc_host.clock = 400000;
    mmc_host.reg = (volatile struct iproc_mmc *)IPROC_SD_BASE_ADDR ;

    mmc_register(mmc);

    return 0;
}
