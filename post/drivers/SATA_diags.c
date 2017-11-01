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
#include <ahci.h>
#include <scsi.h>
#include "../../arch/arm/include/asm/arch-northstar-plus/socregs.h"


#if CONFIG_POST & CONFIG_SYS_POST_SATA

#define NUM_OF_PORTS  2
#define NUM_OF_BLOCKS  1000
#define START_LBA     1000

/* Size in byte */
#define SATA_DATA_BUF_SIZE	 1024*1024
#define SATA_DATA_BUF_ADDR   0x70000000

static int run_pcie_sata(void);

int sata_post_test (int flags)
{

	int j,port=0;
#if defined(CONFIG_NS_PLUS)

    if ( (post_get_board_diags_type() == BCM958522ER ) ||
    		(post_get_board_diags_type() == BCM953022K ))
	{
		printf("\nThis diag is not supported on this platform\n ");
		return 2;
	}
#endif
    //22HR has a SATA sitting on the PCI bus
    //it is different from the native sata
    if ( post_get_board_diags_type() == BCM958622HR )
    {
    	return (run_pcie_sata());
    }

    //Config IDM clock control, AHCI endianess
    config_AHCI();
    //Config PHY
    configure_SATA_PHY ();


    if ( post_get_board_diags_type() & (BCM953025K|BCM953025K_REV2))
    {

		if ( (flags & POST_AUTO) !=  POST_AUTO )
		{
			post_log("\nPlease connect the SATA drive to the port to be tested,make sure it is powered on(0/1)");
			post_log("\n---Please power cycle the hard drive after switching ports---");
			post_getConfirmation("\nReady? (Y/N)");
		}
    }
    //Auto test still defaults to port 0
    //Manual test - user selectable
    if ( (flags & POST_AUTO) !=  POST_AUTO )
    {
    	// 23HR has only one port SATA 0
    	if ( post_get_board_diags_type() != BCM958623HR )
    	{
			post_log("\n\nSelect SATA port to test:0 - iSATA ; 1 - eSATA \n");
			port = post_getUserInput("port #? (0/1) : ");

			if ( (port != 0) && ( port != 1) )
			{
				printf("\nWrong port !!!");
				return -1;
			}
    	}
    }


    if ( -1 == ahci_init(port) )
    {
    	printf("\nPort init failed!!");
    	return -1;
    }

    for ( j = 0; j <=NUM_OF_BLOCKS; j+=4 )
    {
    	if ( 0 == iproc_SATA_DMA_RD_WR_verify(port, 0, 4, START_LBA))
    	{
    		printf(" . ");
    	}
    	else
    	{
    		printf("\nFailed!");
    		return -1;
    	}
    }
    printf("\nPassed\n");
    return 0;

}
static int run_pcie_sata(void)
{
    int j;
    struct pci_controller * hose;
    extern unsigned long  PORT_IN_USE;
	extern unsigned long OUT_PCI_ADDR;
	extern unsigned long PCI_SLOT_ADDR;

    PORT_IN_USE= 0;
    OUT_PCI_ADDR = 0x08000000;
    PCI_SLOT_ADDR = 0x18012000;

    pci_iproc_init_board (hose);
    dumpSATAinfo (hose );

    if ( -1 == pcie_ahci_init(0) )
    {
    	printf("\nDevice Spinup failed!!");
    	return -1;
    }
    printf("\nPassed\n");
    return 0;
}
#endif /* CONFIG_POST & CONFIG_SYS_POST_RGMII */
