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
#ifndef IPROC_MMC_H
#define IPROC_MMC_H

struct iproc_mmc {
	unsigned int	sysad;          // SYSADDR 
	unsigned int    blkcnt_sz ;     // BLOCK
	unsigned int	argument;       // ARG
    unsigned int    cmdreg ;		// CMD

	unsigned int	rspreg0;        // RESP0
	unsigned int	rspreg1;        // RESP2
	unsigned int	rspreg2;        // RESP4
	unsigned int	rspreg3;        // RESP6
	unsigned int	bdata;          // BUFDAT
	unsigned int	prnsts;         // PSTATE
	unsigned int    ctrl_host_pwr_blk_wak ;  // CTRL 
	unsigned int    ctrl1_clkcon_timeout_swrst ;  // CTRL1 
	unsigned int	norintsts;	// INTR
	unsigned int	norintstsen;	// INTREN1 
	unsigned int	norintsigen;	// INTREN2

	unsigned short	acmd12errsts;   // ERRSTAT
	unsigned char	res1[2];        // ERRSTAT

	unsigned int	capareg;        // CAPABILITIES1
#if defined(CONFIG_NS_PLUS)
	unsigned int	cap2areg;        // CAPABILITIES2
#else
	unsigned char	res2[4];        // CAPABILITIES2
#endif
	unsigned int	maxcurr;        // MAX A1 0x0000_0048 -- 

	unsigned int    max_a2 ;        // MAX A2

	unsigned int    cmdentstat  ;   // CMDENTSTAT 
	unsigned int    admaerr     ;   // 0x0000_0054
        unsigned int    admaddr0    ;   // 0x0000_0058
	unsigned int    empty1      ;   // 0x0000_005C
	unsigned int    presetval1  ;   // 0x0000_0060
	unsigned int    presetval2  ;   // 0x0000_0064
	unsigned int    presetval3  ;   // 0x0000_0068
	unsigned int    presetval4  ;   // 0x0000_006C
	unsigned int    boottimeout ;   // 0x0000_0070
	unsigned int    dbgsel      ;   // 0x0000_0074
};

struct iproc_mmc_p2 {
	unsigned int    sbus        ; // 0x0000_00E0
	unsigned char   empty[0x10] ; // 
	unsigned int    spi_int     ; // 0x0000_00F0
	unsigned char   empty1[0x0C] ; // 
	unsigned int    hcversirq   ; // 0x0000_00FC
};


struct mmc_host {
	volatile struct iproc_mmc *reg;
//	volatile struct iproc_mmc_p2 *reg_p2 ;
	unsigned int version;	// SDHCI spec. version 
	unsigned int clock;	// Current clock (MHz)
};

int iproc_mmc_init(int dev_index);

#endif
