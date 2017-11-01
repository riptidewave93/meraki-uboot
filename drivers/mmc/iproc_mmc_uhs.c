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
#include <uhs_mmc.h>
#include <asm/io.h>

#include "iproc_mmc.h"
#include "brcm_rdb_emmcsdxc.h"
#define readl(addr) (*(volatile unsigned int*)(addr))
#define writew(b,addr) ((*(volatile unsigned int *) (addr)) = (b))
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


#define SDHCI_MAX_DIV_SPEC_300	2046
#define SDHCI_DIVIDER_SHIFT	8
#define SDHCI_DIVIDER_HI_SHIFT	6
#define SDHCI_DIV_MASK	0xFF
#define SDHCI_DIV_MASK_LEN	8
#define SDHCI_DIV_HI_MASK	0x300
#define SDHCI_CLOCK_V3_BASE_MASK	0x0000FF00



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
		printf("\niproc_mmc_send_cmd() returned timeout\n");
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

#define  SDHCI_DIVIDER_SHIFT	8
#define  SDHCI_CLOCK_CARD_EN	0x0004
#define  SDHCI_CLOCK_INT_STABLE	0x0002
#define  SDHCI_CLOCK_INT_EN	0x0001
static void iproc_mmc_change_clock(struct mmc_host *host, uint clock)
{
	int div = 0;
	unsigned int clk;
	unsigned long timeout;

	debug("Enter %s\n", __FUNCTION__);

	//printf("\niproc_mmc_change_clock: clock = %d ",clock );



	//sdhci_writew(host, 0, SDHCI_CLOCK_CONTROL);
	writel(0, &host->reg->ctrl1_clkcon_timeout_swrst);


	if (clock == 0)
	{
		//host->clock = clock;
		return;
	}

	if(clock > 100000000){
	    printf("\ncould work @ max 100MHz down the clock %d to 100MHz\n", clock);
        clock = 100000000;

	}

	/* Version 3.00 divisors must be a multiple of 2. */
	if (host->clock <= clock)
		div = 1;
	else {
		for (div = 2; div < SDHCI_MAX_DIV_SPEC_300; div += 2) {
			if ((host->clock / div) <= clock)
				break;
		}
	}

	div >>= 1;

	//printf("\n div is 0x%x",div);

	clk = (div & SDHCI_DIV_MASK) << SDHCI_DIVIDER_SHIFT;
	clk |= ((div & SDHCI_DIV_HI_MASK) >> SDHCI_DIV_MASK_LEN)
		<< SDHCI_DIVIDER_HI_SHIFT;

	clk |= SDHCI_CLOCK_INT_EN;
	//sdhci_writew(host, clk, SDHCI_CLOCK_CONTROL);
	writel(clk, &host->reg->ctrl1_clkcon_timeout_swrst);

	/* Wait max 20 ms */
	timeout = 20;
	while (!((clk = readl(&host->reg->ctrl1_clkcon_timeout_swrst))
		& SDHCI_CLOCK_INT_STABLE)) {
		if (timeout == 0) {
			printf( "\nInternal clock never "
				"stabilised.\n");
			return;
		}
		timeout--;
		mdelay(1);
	}

	clk |= SDHCI_CLOCK_CARD_EN;

	clk |=( 0xd << EMMCSDXC_CTRL1_DTCNT_SHIFT ); //swang
	//sdhci_writew(host, clk, SDHCI_CLOCK_CONTROL);
	writel(clk, &host->reg->ctrl1_clkcon_timeout_swrst);

	host->clock = clock;

}

static void iproc_mmc_set_ios(struct mmc *mmc, uint flag)
{
	struct mmc_host *host = mmc->priv;
	unsigned long ctrl;

	debug("Enter %s\n", __FUNCTION__);
	//printf("bus_width: %x, clock: %d\n", mmc->bus_width, mmc->clock);

	if ( 1 == flag )
	{
		iproc_mmc_change_clock(host, mmc->clock);
		//printf("\n Changed clock in iproc_mmc_set_ios\n");
		return;
	}
	//printf("\n Changed width in iproc_mmc_set_ios\n");
	// Width and edge setting.
	// WIDTH : 
	ctrl = readl(&host->reg->ctrl_host_pwr_blk_wak);
	/*  1 = 4-bit mode , 0 = 1-bit mode */
	if (mmc->bus_width == 4)
		ctrl |= (1 << EMMCSDXC_CTRL_DXTW_SHIFT);
	else
		ctrl &= ~(1 << EMMCSDXC_CTRL_DXTW_SHIFT);
	ctrl |= (1 << EMMCSDXC_CTRL_HSEN_SHIFT);

	ctrl |= 0x14;

	writel(ctrl, &host->reg->ctrl_host_pwr_blk_wak);

#if defined(CONFIG_NS_PLUS)

	//writel(0xd14, 0x18021028);
	//writel(0x90207, 0x1802102c);
	//writel(0x90207, 0x1802102c);
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

    //host->clock = 0;

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
	printf("Reset Done\n");
}
#define SDHCI_CLOCK_CONTROL	0x2C
#define  SDHCI_DIVIDER_SHIFT	8
#define  SDHCI_DIVIDER_HI_SHIFT	6
#define  SDHCI_DIV_MASK	0xFF
#define  SDHCI_DIV_MASK_LEN	8
#define  SDHCI_DIV_HI_MASK	0x300
#define  SDHCI_PROG_CLOCK_MODE	0x0020
#define  SDHCI_CLOCK_CARD_EN	0x0004
#define  SDHCI_CLOCK_INT_STABLE	0x0002
#define  SDHCI_CLOCK_INT_EN	0x0001

#define SDHCI_PRESENT_STATE	0x24
#define  SDHCI_CMD_INHIBIT	0x00000001
#define  SDHCI_DATA_INHIBIT	0x00000002
#define  SDHCI_DOING_WRITE	0x00000100
#define  SDHCI_DOING_READ	0x00000200
#define  SDHCI_SPACE_AVAILABLE	0x00000400
#define  SDHCI_DATA_AVAILABLE	0x00000800
#define  SDHCI_CARD_PRESENT	0x00010000
#define  SDHCI_WRITE_PROTECT	0x00080000
#define  SDHCI_DATA_LVL_MASK	0x00F00000
#define   SDHCI_DATA_LVL_SHIFT	20

#define SDHCI_HOST_CONTROL2		0x3C
#define  SDHCI_CTRL_UHS_MASK		0x0007
#define   SDHCI_CTRL_UHS_SDR12		0x0000
#define   SDHCI_CTRL_UHS_SDR25		0x0001
#define   SDHCI_CTRL_UHS_SDR50		0x0002
#define   SDHCI_CTRL_UHS_SDR104		0x0003
#define   SDHCI_CTRL_UHS_DDR50		0x0004
#define   SDHCI_CTRL_HS_SDR200		0x0005 /* reserved value in SDIO spec */
#define  SDHCI_CTRL_VDD_180		     (0x0008 << 16 )
#define  SDHCI_CTRL_DRV_TYPE_MASK	0x0030
#define   SDHCI_CTRL_DRV_TYPE_B		0x0000
#define   SDHCI_CTRL_DRV_TYPE_A		0x0010
#define   SDHCI_CTRL_DRV_TYPE_C		0x0020
#define   SDHCI_CTRL_DRV_TYPE_D		0x0030
#define  SDHCI_CTRL_EXEC_TUNING		(0x0040 << 16 )
#define  SDHCI_CTRL_TUNED_CLK		( 0x0080 << 16)
#define  SDHCI_CTRL_PRESET_VAL_ENABLE	(0x8000 << 16)


#define SDHCI_BLOCK_SIZE	0x04
#define  SDHCI_MAKE_BLKSZ(dma, blksz) (((dma & 0x7) << 12) | (blksz & 0xFFF))

#define SDHCI_TRANSFER_MODE	0x0C
#define  SDHCI_TRNS_DMA		0x01
#define  SDHCI_TRNS_BLK_CNT_EN	0x02
#define  SDHCI_TRNS_AUTO_CMD12	0x04
#define  SDHCI_TRNS_AUTO_CMD23	0x08
#define  SDHCI_TRNS_READ	0x10
#define  SDHCI_TRNS_MULTI	0x20

void iproc_enable_preset ( void )
{
	u32 ctrl;

	ctrl = readl(0x18021000+ SDHCI_HOST_CONTROL2);
    ctrl |=  SDHCI_CTRL_PRESET_VAL_ENABLE;
	writel(ctrl, 0x18021000+SDHCI_HOST_CONTROL2);
}
void iproc_set_tuning( void )
{
	u32 ctrl;

	ctrl = readl(0x18021000+ SDHCI_HOST_CONTROL2);

   //swang ctrl |= (SDHCI_CTRL_EXEC_TUNING | ( SDHCI_CTRL_PRESET_VAL_ENABLE));

	ctrl |= SDHCI_CTRL_EXEC_TUNING;

	writel(ctrl, 0x18021000+SDHCI_HOST_CONTROL2);

	//printf("\niproc_set_tuning register 0x%x = 0x%x ",0x18021000+SDHCI_HOST_CONTROL2,  readl(0x18021000+ SDHCI_HOST_CONTROL2));


	writel(SDHCI_MAKE_BLKSZ(7, 64),
			0x18021000 +SDHCI_BLOCK_SIZE);


	//writel(SDHCI_TRNS_READ, 0x18021000 + SDHCI_TRANSFER_MODE);


	//printf("\niproc_set_tuning register 0x%x = 0x%x ",0x18021000+SDHCI_HOST_CONTROL2,  readl(0x18021000+ SDHCI_HOST_CONTROL2));


}

int iproc_return_tuning_status( void )
{
	u32 ctrl;

	ctrl = readl(0x18021000+ SDHCI_HOST_CONTROL2);

	if ( SDHCI_CTRL_EXEC_TUNING & ctrl )
	{
		//printf("\n Tuning is in progress ");
		return 2;
	}
	else
	{

		printf("\nTuning is complete !");

		if ( SDHCI_CTRL_TUNED_CLK & ctrl )
		{
			printf("\nTuning is successful ");
			return 1;
		}
		else
		{
			printf("\nTuning failed");
			return 0;
		}
	}
}
void switchcoreVoltage()
{
	unsigned long clk,present_state,ctrl;

	/* Stop SDCLK */
	clk=readl(0x18021000 + SDHCI_CLOCK_CONTROL);
	clk &= ~SDHCI_CLOCK_CARD_EN;
	writel(clk, 0x18021000+SDHCI_CLOCK_CONTROL);

	/* Check whether DAT[3:0] is 0000 */
	present_state = readl( 0x18021000 + SDHCI_PRESENT_STATE);
	if (!((present_state & SDHCI_DATA_LVL_MASK) >>
	       SDHCI_DATA_LVL_SHIFT)) {
		/*
		 * Enable 1.8V Signal Enable in the Host Control2
		 * register
		 */
		ctrl = readl(0x18021000+ SDHCI_HOST_CONTROL2);
		ctrl |= SDHCI_CTRL_VDD_180;
		writel(ctrl, 0x18021000 + SDHCI_HOST_CONTROL2);

		//printf("\n CTRL 2 reg = 0x%x\n",readl(0x18021000+ SDHCI_HOST_CONTROL2));

		mdelay(500);

		ctrl = readl(0x18021000+ SDHCI_HOST_CONTROL2);
		if (ctrl & SDHCI_CTRL_VDD_180) {
			/* Provide SDCLK again and wait for 1ms*/
			clk = readl(0x18021000+SDHCI_CLOCK_CONTROL);
			clk |= SDHCI_CLOCK_CARD_EN;
			writel(clk, 0x18021000+SDHCI_CLOCK_CONTROL);
			mdelay(500);

			/*
			 * If DAT[3:0] level is 1111b, then the card
			 * was successfully switched to 1.8V signaling.
			 */
			present_state = readl(0x18021000+
						SDHCI_PRESENT_STATE);
			if ((present_state & SDHCI_DATA_LVL_MASK) ==
			     SDHCI_DATA_LVL_MASK)
			{
				//printf("\n Success !");
			}
			else
			{
				//printf("\n Failed!");
			}
		}
	}
}
static int iproc_mmc_core_init(struct mmc *mmc)
{
    struct mmc_host *host = (struct mmc_host *)mmc->priv;
    unsigned int mask;

	debug("Enter %s\n", __FUNCTION__);

    iproc_mmc_reset(host);
    mask = readl(0x1802103c);

    //mask |= (( 1 << 31) | ( 2<< 16 ));
    mask |= ( 2<< 16 );

    writel(mask,0x1802103c);
    // Set power now.
    mask = readl(&host->reg->ctrl_host_pwr_blk_wak) | ( 5 << EMMCSDXC_CTRL_SDVSEL_SHIFT ) | EMMCSDXC_CTRL_SDPWR_MASK ;
    writel(mask, &host->reg->ctrl_host_pwr_blk_wak ) ;    


    host->version = readl(IPROC_SDIO_HCVERSIRQ);
    host->version = (host->version & EMMCSDXC_HCVERSIRQ_SPECVER_MASK ) >> EMMCSDXC_HCVERSIRQ_SPECVER_SHIFT ;

   // printf("\niproc_mmc_core_init ==> Version is 0x%x\n", host->version);

   // printf("\nregister 0x%x= 0x%x \n", IPROC_SDIO_HCVERSIRQ,readl(IPROC_SDIO_HCVERSIRQ) );


    /* mask all */
    writel(0xffffffff, &host->reg->norintstsen);
    writel(0xffffffff, &host->reg->norintsigen);


    //DATA line timeout
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
	u32 caps;

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



    mmc->voltages = MMC_VDD_165_195 | MMC_VDD_27_28 | MMC_VDD_28_29 | MMC_VDD_29_30 | MMC_VDD_30_31 | MMC_VDD_31_32 | MMC_VDD_32_33 | MMC_VDD_33_34 | MMC_VDD_34_35 | MMC_VDD_35_36 ;

	/* uboot core mmc.c supports 4-bit mode so be consistent even though 8-bit works on eMMC cards we have used */
    mmc->host_caps = MMC_MODE_4BIT | SD_UHSI_CAP_SDR50;
    mmc->f_min = 400000;  
    mmc->f_max = 100000000;

    mmc_host.clock = mmc->f_max;//mmc->f_max/ SDHCI_MAX_DIV_SPEC_300;

    //printf("\n Host clock is %d \n", mmc_host.clock);

    mmc_host.reg = (volatile struct iproc_mmc *)IPROC_SD_BASE_ADDR ;


   // printf("\nboard_mmc_init=>> cap reg = 0x%x \n", (mmc_host.reg)->capareg);
   // printf("\nboard_mmc_init=>> cap2 reg = 0x%x \n", (mmc_host.reg)->cap2areg);

    mmc_register(mmc);

    return 0;
}
