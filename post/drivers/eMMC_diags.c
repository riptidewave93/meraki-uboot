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
#include <mmc.h>

#if CONFIG_POST & CONFIG_SYS_POST_EMMC

/* Size in byte */
#define MMC_DATA_BUF_SIZE	32*1024

#if defined(CONFIG_NS_PLUS)
#define MMC_DATA_BUF_ADDR      0x70000000
#define MMC_DATA_BUF_ADDR_STR  "0x70000000"
#define MMC_DATA_DST_BUF_ADDR  0x71000000
#define eMMC_IPROC_CONFIG_ADDR 0x1803F408       
#else
#define MMC_DATA_BUF_ADDR      0x89000000
#define MMC_DATA_BUF_ADDR_STR  "0x89000000"
#define MMC_DATA_DST_BUF_ADDR  0x8a000000
#endif

#define SD_BLK_SIZE         512
#define TEST_BLK_CNT        MMC_DATA_BUF_SIZE/SD_BLK_SIZE
#define SD_START_BLK        10     

int eMMC_post_test (int flags)
{
        char argv_buf[7*10];
        char * cmd_argv[7];
        int  i, n;
		void *addr = (void *)MMC_DATA_BUF_ADDR;
		u32 blk = 0x10000;
		struct mmc *mmc;

#if defined(CONFIG_NS_PLUS)

		if ( 0 == (post_get_board_diags_type() & (BCM953025K|BCM953025K_REV2)))
		{
			printf("\nThis diag is not supported on this platform\n ");
			return 2;
		}

		post_log("\nHardware issue is being fixed, this test is going to fail!\n");

	    if ( (flags & POST_AUTO) !=  POST_AUTO )
	    {
	        post_log("\nPlease set strap pin: A1=0xfc, A2=0, A3=0x3, A4=0xf, A5=0x6f, A6=0xf3");
            post_log("\nMake sure NAND chip U1702 is removed");
            post_log("\nMake sure resistors R4074-4084 are installed");
	        post_getConfirmation("\nReady? (Y/N)");
	    }
	    else
	    {
	        post_set_strappins("A2 0%A3 3%A4 f%A5 7f%A6 f3%");
	    }

        memset(&argv_buf, 0, 7*10);
        for(i=0; i<7; i++)
		cmd_argv[i] = &argv_buf[i*10];

        /* Chip Reg select eMMC on NAND pins */
        /* Nand chip removed, SDIO slot is NOT used */
        writel(0x1, eMMC_IPROC_CONFIG_ADDR);

        /* MMC Scan/Info */
        printf("\n\nMMC Info\n");
        strcpy(cmd_argv[0], "mmcinfo");
        strcpy(cmd_argv[1], "0");
        do_mmcinfo(0, 0, 2, cmd_argv);
		udelay(2000*100);

        /* Memory Fill */
        post_log("\n\nFill memory with incremental data\n");
        for(i=0; i<MMC_DATA_BUF_SIZE/4; i++)
        {
		    *(unsigned long *)(MMC_DATA_BUF_ADDR + i*4) = i; 
	    }

        /* Memory Display */
        post_log("\n\n- Display filled memory\n");
        strcpy(cmd_argv[0], "md");
        strcpy(cmd_argv[1], MMC_DATA_BUF_ADDR_STR);
        do_mem_md(0, 0, 2, cmd_argv);

        /* MMC write */
	    mmc = find_mmc_device(0);
	    if (!mmc) goto failed;
        mmc_init(mmc);
        
        post_log("\n\nWrite data to eMMC chip\n");
        post_log("mmc write 0 ddr_addr=0x%x,blk_addr=%d cnt=%d\n",
        		MMC_DATA_BUF_ADDR, SD_START_BLK, TEST_BLK_CNT);

		addr = (void *)MMC_DATA_BUF_ADDR;
		blk = SD_START_BLK;
        for(i=0; i<TEST_BLK_CNT; i++)
        {
			n = mmc->block_dev.block_write(0, blk, 1, addr);
            if(n<1) goto failed;
            blk++;
            addr += SD_BLK_SIZE;
            udelay(10);
        }

        /* MMC read back */        
        post_log("\n\nRead back from eMMC and compare\n");
        post_log("mmc read 0 ddr_addr=0x%x, blk_addr=%d cnt=%d\n",
        		MMC_DATA_DST_BUF_ADDR, SD_START_BLK, TEST_BLK_CNT);

        mmc_init(mmc);
		addr = (void *)MMC_DATA_DST_BUF_ADDR;
		blk = SD_START_BLK;
        for(i=0; i<TEST_BLK_CNT; i++)
        {
			n = mmc->block_dev.block_read(0, blk, 1, addr);
            if(n<1) goto failed;
            blk++;
            addr += SD_BLK_SIZE;
            udelay(10);
        }

        /* Compare result */
        for(i=0; i<MMC_DATA_BUF_SIZE/4; i++)
        {
		    if(*(unsigned long *)(MMC_DATA_DST_BUF_ADDR + i*4) != *(unsigned long *)(MMC_DATA_BUF_ADDR + i*4))
		    {
			    post_log("\n !! Compare failed at address <0x%08x> = 0x%08x\n", 
			    		MMC_DATA_DST_BUF_ADDR + i*4, *(unsigned long *)(MMC_DATA_DST_BUF_ADDR + i*4) );
			    break;
		    }
	    }

        if(i==MMC_DATA_BUF_SIZE/4)
	    {
        	post_log("\n\nRead back and compare OK\n");
        	post_log("\n\neMMC test done\n");
	        return 0;
	    }
	    else
	    {
failed:
        	post_log("\n\neMMC test failed\n");
		    return -1;
	    }
#else
        post_log("\neMMC not available on NS platform !\n");
        return -1;
#endif
}
#endif /* CONFIG_POST & CONFIG_SYS_POST_EMMC */
