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

/*

 */

#include <common.h>
#include <post.h>

#if CONFIG_POST & CONFIG_SYS_POST_NAND
/* Size in byte */
#define NAND_DATA_BUF_SIZE	 1024*1024

#if defined(CONFIG_NS_PLUS)
#define NAND_READ_BUF_ADDR   0x70000000
#define NAND_WRITE_BUF_ADDR  0x71000000
#define NAND_WRITE_ADDR_STR  "71000000"
#define NAND_READ_ADDR_STR   "70000000"

#else
#define NAND_READ_BUF_ADDR   0x8A000000
#define NAND_WRITE_BUF_ADDR  0x89000000
#define NAND_WRITE_ADDR_STR  "89000000"
#define NAND_READ_ADDR_STR   "8a000000"
#endif

int NAND_post_test (int flags)
{
        char argv_buf[7*10];
        char * cmd_argv[7];
        int  i, rc = 0;
        u32  rd, wr;


#if defined(CONFIG_NS_PLUS)
        if ( post_get_board_diags_type() & SVK_BOARDS )
		{

			if ( (flags & POST_AUTO) !=  POST_AUTO )
			{

				post_log("\n\nPlease set strap pin: A1=0xfc, A2=0xa", 0);
				post_getConfirmation("\nReady? (Y/N)");
			}
			else
			{
				post_set_strappins("A1 fc%A2 a%");
			}
		}
#else
		if ( (flags & POST_AUTO) !=  POST_AUTO )
		{

			post_log("\n\nPlease set strap pin: A1=0xfc, A2=0xa", 0);
			post_getConfirmation("\nReady? (Y/N)");
		}
		else
		{
			post_set_strappins("A1 fc%A2 a%");
		}
#endif

        memset(&argv_buf, 0, 7*10);
        for(i=0; i<7; i++)
    		cmd_argv[i] = &argv_buf[i*10];

        /* NAND Info */
        printf("\n\nNAND Info\n");
        strcpy(cmd_argv[0], "nand");
        strcpy(cmd_argv[1], "info");
        do_nand(0, 0, 2, cmd_argv);
		udelay(200*1000);

        /* Memory Fill */
        post_log("\n\nFill memory with incremental data\n");
        for(i=0; i<NAND_DATA_BUF_SIZE/4; i++)
        {
		    *(unsigned long *)(NAND_WRITE_BUF_ADDR + i*4) = i; 
	    }

        /* NAND write */
        post_log("\n\nNAND data write, 1Mbyte\n");
        post_log("NAND write: from ddr_addr=0x%x to NAND offset=0x4000000 cnt=1MByte\n",NAND_WRITE_BUF_ADDR);
        strcpy(cmd_argv[0], "nand");
        strcpy(cmd_argv[1], "write");
        strcpy(cmd_argv[2], NAND_WRITE_ADDR_STR);
        strcpy(cmd_argv[3], "4000000");
        strcpy(cmd_argv[4], "100000");
        rc = do_nand(0, 0, 5, cmd_argv);
	    if(rc != 0) goto failed;


        /* NAND read */
        post_log("\n\nNAND data read, 1Mbyte\n");
        post_log("NAND read: from NAND offset=0x4000000 to ddr_addr=0x%x cnt=1MByte\n",NAND_READ_BUF_ADDR);
        strcpy(cmd_argv[0], "nand");
        strcpy(cmd_argv[1], "read");
        strcpy(cmd_argv[2], NAND_READ_ADDR_STR);
        strcpy(cmd_argv[3], "4000000");
        strcpy(cmd_argv[4], "100000");
        rc = do_nand(0, 0, 5, cmd_argv);
	    if(rc != 0) goto failed;

        /* NAND Verify */
        for(i=0; i<NAND_DATA_BUF_SIZE/4; i++)
        {
            rd = *(u32 *)(NAND_READ_BUF_ADDR + i*4);
            wr = *(u32 *)(NAND_WRITE_BUF_ADDR + i*4);

		    if(rd != wr)
		    {
			    post_log("\n !! Compare failed at address <0x%08x>, write=0x%08x, read=0x%08x\n", 
						    NAND_READ_BUF_ADDR + i*4, wr, rd );
			    break;
		    }
	    }

        if(i==NAND_DATA_BUF_SIZE/4)
	    {
        	post_log("\n\nRead back and compare OK\n");
        	post_log("\n\nNAND test passed\n");
	        return 0;
	    }

failed:
	    post_log("\nNAND test failed");
	    return -1;
}

#endif /* CONFIG_POST & CONFIG_SYS_POST_NAND */
