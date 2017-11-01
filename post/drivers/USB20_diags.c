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


#if defined(CONFIG_CMD_USB) && (CONFIG_POST & CONFIG_SYS_POST_USB20)

/* Size in byte */
#define USB_DATA_BUF_SIZE	1024*1024

#if defined(CONFIG_NS_PLUS)
#define USB_DATA_BUF_ADDR   0x70000000
#define USB_DATA_BUF_ADDR_STR  "70000000"
#define USB_DST_DATA_BUF_ADDR 0x71000000
#define USB_DST_DATA_BUF_ADDR_STR "71000000"
#else
#define USB_DATA_BUF_ADDR   0x89000000
#define USB_DATA_BUF_ADDR_STR  "89000000"
#define USB_DST_DATA_BUF_ADDR 0x8A000000
#define USB_DST_DATA_BUF_ADDR_STR "8A000000"
#endif
int USB20_post_test (int flags)
{
    char argv_buf[7*10];
    char * usb_argv[7];
    int  i, rc = 0;

#if defined(CONFIG_NS_PLUS)
    if ( post_get_board_diags_type() & SVK_BOARDS )
	{

		if ( (flags & POST_AUTO) !=  POST_AUTO )
		{
			post_log("\nPlease set strap pin: A2=0, A3=0x3, A4=0xf, A5=0x4f, A6=0xf3", 0);
			post_getConfirmation("\nReady? (Y/N)");
		}
		else
		{
			post_set_strappins("A2 0%A3 3%A4 f%A5 5f%A6 f3%");
		}
	}
#else
    if ( (flags & POST_AUTO) !=  POST_AUTO )
    {
        post_log("\nPlease set strap pin: A2=0, A3=0x3, A4=0xf, A5=0x4f, A6=0xf3", 0);
        post_getConfirmation("\nReady? (Y/N)");
    }
    else
    {
        post_set_strappins("A2 0%A3 3%A4 f%A5 5f%A6 f3%");
    }
#endif


    memset(&argv_buf, 0, 7*10);
    for(i=0; i<7; i++)
	usb_argv[i] = &argv_buf[i*10];

    /* USB Reset */
    post_log("\n\nUSB reset - Reset/Enumerate USB device\n");
    strcpy(usb_argv[0], "usb");
    strcpy(usb_argv[1], "reset");
    rc = do_usb(0, 0, 2, usb_argv);
	if(rc != 0) goto failed;

    /* USB Tree */
    post_log("\n\nUSB tree - Display USB device\n");
    strcpy(usb_argv[0], "usb");
    strcpy(usb_argv[1], "tree");
    rc = do_usb(0, 0, 2, usb_argv);
	if(rc != 0) goto failed;

    /* USB Info */
    post_log("\n\nUSB info - Display USB device info\n");
    strcpy(usb_argv[0], "usb");
    strcpy(usb_argv[1], "info");
    rc = do_usb(0, 0, 2, usb_argv);
	if(rc != 0) goto failed;

    /* USB Storage */
    post_log("\n\nUSB storage - Display USB disk info\n");
    strcpy(usb_argv[0], "usb");
    strcpy(usb_argv[1], "storage");
    rc = do_usb(0, 0, 2, usb_argv);
	if(rc != 0) goto failed;

    /* USB read */
    post_log("\n\nUSB raw data read, 1Mbyte\n");
    post_log("USB read ddr_addr=0x%x blk_addr=0x8000 cnt=2048\n",USB_DATA_BUF_ADDR);
    strcpy(usb_argv[0], "usb");
    strcpy(usb_argv[1], "read");
    strcpy(usb_argv[2], USB_DATA_BUF_ADDR_STR );
    strcpy(usb_argv[3], "8000");
    strcpy(usb_argv[4], "800");
    rc = do_usb(0, 0, 5, usb_argv);
	if(rc != 0) goto failed;

    /* Memory Fill */
    post_log("\n\nFill memory with incremental data, 1Mbyte, addr=0x%x\n",USB_DATA_BUF_ADDR);
    for(i=0; i<USB_DATA_BUF_SIZE/4; i++)
    {
		*(unsigned long *)(USB_DATA_BUF_ADDR + i*4) = i; 
	}

    /* Memory Display */
    post_log("\n\Addr 0x%x - Display filled memory\n",USB_DATA_BUF_ADDR);
    strcpy(usb_argv[0], "md");
    strcpy(usb_argv[1], USB_DATA_BUF_ADDR_STR);
    do_mem_md(0, 0, 2, usb_argv);

    /* USB write */
    post_log("\n\nUSB raw data write, 1Mbyte\n");
    post_log("USB write ddr_addr=0x%x blk_addr=0x8000 cnt=2048\n",USB_DATA_BUF_ADDR);
    strcpy(usb_argv[0], "usb");
    strcpy(usb_argv[1], "write");
    strcpy(usb_argv[2], USB_DATA_BUF_ADDR_STR);
    strcpy(usb_argv[3], "8000");
    strcpy(usb_argv[4], "800");
    rc = do_usb(0, 0, 5, usb_argv);
	if(rc != 0) goto failed;

    /* USB read back and compare */
    post_log("\n\nUSB read back and compare\n");
    post_log("USB read ddr_addr=0x%x blk_addr=0x8000 cnt=2048\n",USB_DST_DATA_BUF_ADDR);
    strcpy(usb_argv[0], "usb");
    strcpy(usb_argv[1], "read");
    strcpy(usb_argv[2], USB_DST_DATA_BUF_ADDR_STR);
    strcpy(usb_argv[3], "8000");
    strcpy(usb_argv[4], "800");
    rc = do_usb(0, 0, 5, usb_argv);
	if(rc != 0) goto failed;

    for(i=0; i<USB_DATA_BUF_SIZE/4; i++)
    {
		if(*(unsigned long *)(USB_DST_DATA_BUF_ADDR + i*4) != i)
		{
			post_log("\n !! Compare failed at address <0x%08x> = 0x%08x\n", 
						USB_DATA_BUF_ADDR + i*4, *(unsigned long *)(USB_DST_DATA_BUF_ADDR + i*4) );
			break;
		}
	}

    if(i==USB_DATA_BUF_SIZE/4)
	{
    	post_log("\n\nRead back and compare OK\n");
    	post_log("\n\nUSB test done\n");
	    return 0;
	}
	else
	{
failed:
    	post_log("\n\nUSB test failed\n");
		return -1;
	}
}
#endif /* CONFIG_POST & CONFIG_SYS_POST_USB20 */
