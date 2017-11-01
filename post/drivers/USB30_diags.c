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
#include <xhci.h>

#if CONFIG_POST & CONFIG_SYS_POST_USB30

#if defined(CONFIG_NS_PLUS)

/* Size in byte */
#define USB_DATA_BUF_SIZE	32*1024*1024
#define USB_DATA_BUF_ADDR   0x6e000000

/* 4M byte bulk_in/out data buffer */
#define  XHCI_MSC_READ_BUF      (0x6f200000)
#define  XHCI_MSC_WRITE_BUF     (0x6f800000)
#else

/* Size in byte */
#define USB_DATA_BUF_SIZE	256*1024*1024
#define USB_DATA_BUF_ADDR   0x89000000

/* 4M byte bulk_in/out data buffer */
#define  XHCI_MSC_READ_BUF      (0x8A200000)
#define  XHCI_MSC_WRITE_BUF     (0x8A800000)
#endif

/* DISK write starting block address = 256K */
/* 256K * blk_size 512 = 128M, so disk must be at least 200M */
#define  XHCI_DISK_WR_LBA       0x40000
#define  BLK_CNT_PER_XFER       32


int USB30_post_test (int flags)
{
    char argv_buf[7*10];
    char * usb_argv[7];
    int  i,j, rc = 0;

    post_log("\nUSB 3.0 diags starts ...");

#if defined(CONFIG_NS_PLUS)
    if ( post_get_board_diags_type() & SVK_BOARDS )
	{

		if ( (flags & POST_SEMI_AUTO) !=  POST_SEMI_AUTO )
		{
			post_log("\nSet strap pin: A1=0xfc, A2=0, A3=0x3, A4=0xf, A5=0x28, A6=0xf4", 0);
			//post_log("\nDon't plug in USB 3.0 cable for now");
			post_getConfirmation("\nReady? (Y/N)");
		}
		else
		{
			post_set_strappins("A2 0%A3 3%A4 f%A5 38%A6 f4%");
		}
	}
#else
    if ( (flags & POST_SEMI_AUTO) !=  POST_SEMI_AUTO )
    {
        post_log("\nSet strap pin: A1=0xfc, A2=0, A3=0x3, A4=0xf, A5=0x28, A6=0xf4", 0);
        //post_log("\nDon't plug in USB 3.0 cable for now");
        post_getConfirmation("\nReady? (Y/N)");
    }
    else
    {
        post_set_strappins("A2 0%A3 3%A4 f%A5 38%A6 f4%");
    }
#endif

    memset(&argv_buf, 0, 7*10);
    for(i=0; i<7; i++)
	usb_argv[i] = &argv_buf[i*10];

    /* Memory Fill */
    //post_log("\n\nZero out buffer, 256Mbyte, addr=0x8900_0000\n");
    for(i=0; i<USB_DATA_BUF_SIZE/4; i++)
    {
		*(unsigned long *)(USB_DATA_BUF_ADDR + i*4) = 0; 
	}

    /* Incremental data pattern for USB write buffer */
    for(i=0; i<1024*1024; i++)
    {
		*(unsigned char *)(XHCI_MSC_WRITE_BUF + i) = i; 
	}


    /* Enumerate USB3, retry three times if failed */
    for(i=0; i<3; i++)
    {
        /* Memory Fill */
        //post_log("\n\nZero out buffer, 256Mbyte, addr=0x8900_0000\n");
        for(j=0; j<USB_DATA_BUF_SIZE/4; j++)
        {
            *(unsigned long *)(USB_DATA_BUF_ADDR + j*4) = 0;
        }

        /* Incremental data pattern for USB write buffer */
        for(j=0; j<1024*1024; j++)
        {
            *(unsigned char *)(XHCI_MSC_WRITE_BUF + j) = j;
        }

        xhci_hcd_init();
        rc = xhci_hcd_enum();

        if( rc )
        {
            post_log("\n\nLink connection failed,retry...\n");
            //post_log("\nUnplug the cable and retry...\n");
            //post_getConfirmation("\nReady? (Y/N)");
            //continue;
            //return -1;
        }
        else
        {
            post_log("\n\nLink setup OK. Enumeration done\n");
            break;
        }
    }

    if ( i == 3 )
    {
        post_log("\nUSB 3 device enumeration failed, test failed");
    }
    post_log("\n\nStart USB read/write test\n");
    //post_getConfirmation("\nReady? (Y/N)");


    /* Write 1M data, 16KB per command, 64x commands */
    post_log("\n\nWrite 1M data to disk, ");
    for(i=0; i<10; i++)
    {
        xhci_write_test(XHCI_DISK_WR_LBA + i * BLK_CNT_PER_XFER,     // incr 32 block per run
                        BLK_CNT_PER_XFER,                            // blk_cnt = 32 * 512 = 16K
                        XHCI_MSC_WRITE_BUF + i*16*1024               // incr 16K per run 
                        );
        udelay(10000);
        post_log(".");
    }

    /* Read 1M data, 16KB per command, 64x commands */
    post_log("\n\nRead 1M data from disk, ");
    for(i=0; i<10; i++)
    {
        xhci_read_test(XHCI_DISK_WR_LBA + i * BLK_CNT_PER_XFER,     
                        BLK_CNT_PER_XFER,                           
                        XHCI_MSC_READ_BUF + i*16*1024   
                        );
        udelay(10000);
        post_log(".");
    }

    post_log("\n\nVerify data : ");
    /* Incremental data pattern for USB write buffer */
    for(i=0; i<160*1024/4; i++)
    {
	if( *(u32 *)(XHCI_MSC_WRITE_BUF + i*4) != *(u32 *)(XHCI_MSC_READ_BUF + i*4) )
        {
            post_log("Data Error at 0x%x, wr=0x%x, rd=0x%x\n", 
                        XHCI_MSC_WRITE_BUF + i*4,
                        *(u32 *)(XHCI_MSC_WRITE_BUF + i*4),
                        *(u32 *)(XHCI_MSC_READ_BUF + i*4) );
            return -1;
        }
        if ( ( i % 1024 ) == 0 )
        {
            post_log(".");
        }
    }

    post_log("\n\nData verified OK, test passed\n");
    return 0;
}
#endif /* CONFIG_POST & CONFIG_SYS_POST_MMC */
