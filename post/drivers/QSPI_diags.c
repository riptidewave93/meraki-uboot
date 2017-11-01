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


#if CONFIG_POST & CONFIG_SYS_POST_QSPI

#if 1 //  //
#define  FLASH_START_ADDR   0x1e000000
#define  TEST_BLOCK_OFFSET  0xFF0000
#define  TEST_BLOCK_SIZE    4096

#define QSPI_BASE_ADDRESS		0x18027000
#define QSPI_MSPI_SPCR0_LSB		(QSPI_BASE_ADDRESS + 0x200)
#define QSPI_MSPI_SPCR0_MSB		(QSPI_BASE_ADDRESS + 0x204)
#define QSPI_MSPI_SPCR1_LSB		(QSPI_BASE_ADDRESS + 0x208)
#define QSPI_MSPI_SPCR1_MSB		(QSPI_BASE_ADDRESS + 0x20c)

#define QSPI_MSPI_NEWQP			(QSPI_BASE_ADDRESS + 0x210)
#define QSPI_MSPI_ENDQP			(QSPI_BASE_ADDRESS + 0x214)
#define QSPI_MSPI_SCPR2			(QSPI_BASE_ADDRESS + 0x218)

#define QSPI_MSPI_STATUS		(QSPI_BASE_ADDRESS + 0x220)

#define QSPI_MSPI_TXRAM00		(QSPI_BASE_ADDRESS + 0x240)
#define QSPI_MSPI_TXRAM01		(QSPI_BASE_ADDRESS + 0x244)  //use for 16 bit operation only
#define QSPI_MSPI_TXRAM02		(QSPI_BASE_ADDRESS + 0x248)

#define QSPI_MSPI_TXRAM04		(QSPI_BASE_ADDRESS + 0x250)

#define QSPI_MSPI_TXRAM06		(QSPI_BASE_ADDRESS + 0x258)

#define QSPI_MSPI_TXRAM08		(QSPI_BASE_ADDRESS + 0x260)

#define QSPI_MSPI_TXRAM10		(QSPI_BASE_ADDRESS + 0x268)

#define QSPI_MSPI_TXRAM12		(QSPI_BASE_ADDRESS + 0x270)

#define QSPI_MSPI_TXRAM14		(QSPI_BASE_ADDRESS + 0x278)

#define QSPI_MSPI_CDRAM00		(QSPI_BASE_ADDRESS + 0x340)
#define QSPI_MSPI_CDRAM01		(QSPI_BASE_ADDRESS + 0x344)
#define QSPI_MSPI_CDRAM02		(QSPI_BASE_ADDRESS + 0x348)
#define QSPI_MSPI_CDRAM03		(QSPI_BASE_ADDRESS + 0x34C)
#define QSPI_MSPI_CDRAM04		(QSPI_BASE_ADDRESS + 0x350)
#define QSPI_MSPI_CDRAM06		(QSPI_BASE_ADDRESS + 0x354)
#define QSPI_MSPI_CDRAM07		(QSPI_BASE_ADDRESS + 0x358)


unsigned char pu8_4KBytes[TEST_BLOCK_SIZE];   	//To store the 4kB back up data
unsigned char pu8DataPattern[16];  	//To store the data pattern for testing
const int iBlockSize = 		TEST_BLOCK_SIZE;
const u32 u32TestBlockOffset = 	TEST_BLOCK_OFFSET;


/*
 * Wait until certain bits of the register become what we want
 */
static void wait_for_complete(u32 reg, u32 mask, u32 value)
{
    for(;;) {
        if (((*(unsigned int *)reg) & mask) == value) {
            break;
        }
        udelay(10);
    }
}

/*
 * Generic MSPI translation (max 4 bytes tx)
 *   Return: address to read data back (with +8 increment)
 */
static u32 mspi_translation(u32 tx_data, u32 tx_len, u32 rx_len)
{
    int i;
    unsigned int dst;
    unsigned int total;

    // Fill TXRAM
    *(unsigned int *)QSPI_MSPI_TXRAM00 = tx_data & 0xff;
    *(unsigned int *)QSPI_MSPI_TXRAM02 = (tx_data >> 8) & 0xff;
    *(unsigned int *)QSPI_MSPI_TXRAM04 = (tx_data >> 16) & 0xff;
    *(unsigned int *)QSPI_MSPI_TXRAM06 = (tx_data >> 24) & 0xff;

    // Fill CDRAM
    total = tx_len + rx_len;
    dst = QSPI_MSPI_CDRAM00;
    for(i=0; i<total; i++, dst+=4) {
        *(unsigned int *)dst = 0x82;
        if (i == total - 1) {
            *(unsigned int *)dst = 0x02;
        }
    }

    // Queue pointers
    *(unsigned int *)QSPI_MSPI_NEWQP = 0;
    *(unsigned int *)QSPI_MSPI_ENDQP = total - 1;

    // Start it
    *(unsigned int *)0x18027220 = 0x00;
    *(unsigned int *)0x18027218 = 0xc0;

    // Wait for complete
    wait_for_complete(0x18027220, 1, 1);

    // Return the address of the first byte to read from
    return 0x180272c4 + 8 * tx_len;
}
//
//This is a hack version for Program command
//
static u32 mspi_translation_w(u32 tx_data, u32 tx_len, u32 rx_len, unsigned char *pchar)
{
    int i;
    unsigned int dst;
    unsigned int total;

    // Fill TXRAM
    *(unsigned int *)QSPI_MSPI_TXRAM00 = tx_data & 0xff;
    *(unsigned int *)QSPI_MSPI_TXRAM02 = (tx_data >> 8) & 0xff;
    *(unsigned int *)QSPI_MSPI_TXRAM04 = (tx_data >> 16) & 0xff;
    *(unsigned int *)QSPI_MSPI_TXRAM06 = (tx_data >> 24) & 0xff;
    if(pchar != NULL) {
        for (i = 0; i < 8; i++) {
	    *(unsigned int *)(QSPI_MSPI_TXRAM08 + 8*i) = *(pchar+i);
            //printf("I%x ", *(pchar+i));
        }
    }

    // Fill CDRAM
    total = tx_len + rx_len;
    dst = QSPI_MSPI_CDRAM00;
    for(i=0; i<total; i++, dst+=4) {
        *(unsigned int *)dst = 0x82;
        if (i == total - 1) {
            *(unsigned int *)dst = 0x02;
        }
    }

    // Queue pointers
    *(unsigned int *)0x18027210 = 0;
    *(unsigned int *)0x18027214 = total - 1;

    // Start it
    *(unsigned int *)0x18027220 = 0x00;
    *(unsigned int *)0x18027218 = 0xc0;

    // Wait for complete
    wait_for_complete(0x18027220, 1, 1);

    // Return the address of the first byte to read from
    return 0x180272c4 + 8 * tx_len;
}


/*
 * Initialize QSPI and serial flash
 */
void qspi_init(void)
{
    unsigned int rxaddr, cmd, u32FlashAddress;
    int i, j;

    // Switch to MSPI if not yet
    if (*(unsigned int *)0x18027008 == 0) {
        // Wait for BSPI ready
        //post_log("\nWait for BSPI ready");
        wait_for_complete(0x1802700c, 1, 0);

        // Switch to MSPI
        //post_log("\nSwitch to MSPI");
        *(unsigned int *)0x18027008 = 1;
    }

    // Basic MSPI configuration
    *(unsigned int *)0x18027200 = 0x40;    //baud rate = system_clcok/(2 * 62)
    *(unsigned int *)0x18027204 = 0xa3;    //SPI master, no waiting before transaction, SCK=1.

    //---------------------------------------------------------------------------
    //**Step 1**: copy the content of 4K bytes from address Flash 0xFF0000 to RAM.
    //This 4K space will be used for test.
    //---------------------------------------------------------------------------
    
    u32FlashAddress = u32TestBlockOffset;
    for(i = 0; i < 4096/8; i++) {
       cmd = 0x02;
       cmd |= ((u32FlashAddress >> 8) & 0xff00);
       cmd |= ((u32FlashAddress << 8) & 0xff0000);
       cmd |= ((u32FlashAddress << 24) & 0xff000000);
       rxaddr = mspi_translation(cmd, 4, 8);  //read out 8 bytes each time
    }


    //---------------------------------------------------------------------------
    //**Step 2**: erase 4K space and porgram 16-byte data pattern
    //---------------------------------------------------------------------------

    // Clear software protection bits if any
    // Write enable (precondition for erase)
    mspi_translation(0x06, 1, 0);
    // Erase one block/sector
    // 4KB sector erase
    if(iBlockSize == 4096)
        cmd = 0x20;
    else 
        cmd = 0xd8;

    u32FlashAddress = u32TestBlockOffset;
    cmd |= ((u32FlashAddress >> 8) & 0xff00);
    cmd |= ((u32FlashAddress << 8) & 0xff0000);
    cmd |= ((u32FlashAddress << 24) & 0xff000000);
    mspi_translation(cmd, 4, 0);    //erase the contenet
    
    // Wait for erase complete
    for(;;) {
        rxaddr = mspi_translation(0x05, 1, 1);
        if ((*(unsigned int *)rxaddr & 1) == 0) {
            break;
        }
    }
    // Generate a data pattern.
    for(i = 0; i < 16; i++) 
    {
        *(pu8DataPattern+i) = (32 + i);
    }

    mspi_translation(0x06, 1, 0);
    //write 16-byte data pattern 
    mspi_translation_w((0x02|0x0000FF00), (4+8), 0, pu8DataPattern);    
                                                //page program, with 16-byte data trailing
    // Wait for program complete
    for(;;) {
        rxaddr = mspi_translation(0x05, 1, 1);
        if ((*(unsigned int *)rxaddr & 1) == 0) {
            break;
        }
    }

    mspi_translation(0x06, 1, 0);
    //write 16-byte data pattern 
    mspi_translation_w((0x02|0x0800FF00), (4+8), 0, pu8DataPattern+8);    
                                                //page program, with 16-byte data trailing
    // Wait for program complete
    for(;;) {
        rxaddr = mspi_translation(0x05, 1, 1);
        if ((*(unsigned int *)rxaddr & 1) == 0) {
            break;
        }
    }

    rxaddr = mspi_translation(0x03|0x0000FF00, 4, 8);  //read out 16 bytes each time

}

#endif ////

int qspi_post_test (int flags)
{

    int status = 0;
    u32 tmp, i;
    unsigned char chTmp;
    u32 u32FailureCounter = 0;
    unsigned int rxaddr, cmd, u32FlashAddress;


#if defined(CONFIG_NS_PLUS)
    if ( post_get_board_diags_type() & SVK_BOARDS )
	{

		if ( (flags & POST_AUTO) !=  POST_AUTO )
		{
			post_log("\nQSPI test: Please set strap pin: A1=0xfc", 0);
			post_getConfirmation("\nReady? (Y/N)");
		}
		else
		{
			post_set_strappins("A1 FC%");
		}
	}
#else
    if ( (flags & POST_AUTO) !=  POST_AUTO )
    {
        post_log("\nQSPI test: Please set strap pin: A1=0xfc", 0);
        post_getConfirmation("\nReady? (Y/N)");
    }
    else
    {
        post_set_strappins("A1 FC%");
    }
#endif
#if 1 //  //
    qspi_init();

    //----------------------------------------------------------
    // **Step 3**: Switch back to BSPI, single lane mode
    // Read out the data pattern and verify the result
    //----------------------------------------------------------
    *(unsigned int *)0x18027008 = 0;     //SPI bus is driven by BSPI

    // Invalidate bspi buffer
    *(unsigned int *)0x18027018 = 1;     //clear data in buffer 0
    *(unsigned int *)0x18027020 = 1;     //clear data in buffer 1

    // Read back Serial Flash through QSPI-BSPI
    post_log("\n\nQSPI: Switch to BSPI, verify data: \n");
    for(i = 0; i < 16; i++)
    {
        chTmp = *(unsigned char *)(FLASH_START_ADDR+i+TEST_BLOCK_OFFSET);
        //printf(" D%d ", chTmp);
        if( chTmp != (32 + i))
            u32FailureCounter++;
    }
    post_log("\n");


    if(u32FailureCounter > 0) {
		post_log("\n\nQSPI BSPI (Single lane) Read: Test Failed\n");
		u32FailureCounter = 0;
        status = -1;
    }
    else {
    	post_log("\n\nQSPI BSPI (Single lane) Read: Test Passed\n");
    }
  

    //----------------------------------------------------------------------------------------------------
    // **Step 4**: Configure QSPI BSPI (dual lane mode). Then read out data and compare with data pattern.
    //----------------------------------------------------------------------------------------------------

    // Switch back to BSPI
    *(unsigned int *)0x18027008 = 0;     //SPI bus is driven by BSPI

    // Invalidate bspi buffer
    *(unsigned int *)0x18027018 = 1;       //clear data in buffer 0
    *(unsigned int *)0x18027020 = 1;       //clear data in buffer 1
   
    //now test BSPI read in dual mode 
    *(unsigned int *)0x18027024 = 3;       //3-byte address, dual mode, override strap
    *(unsigned int *)0x18027028 = 1;       //enable flex mode
    *(unsigned int *)0x1802702C = 0x1010101; //dual mode to recv data, 2 mode bits/cycle, 2 address bits/cycle, 2 cmd bits/cycle
    *(unsigned int *)0x18027030 = 0x8;     //dummy cycle=8, 0 mode bits, 24-byte address
    *(unsigned int *)0x18027034 = 0x3;     //default command is fast read

    // Read back Serial Flash through QSPI-BSPI
    post_log("\n\nQSPI: Switch to BSPI dual lane mode, verify data: \n");

    for(i=0; i<16; i++)
    {
        chTmp = *(unsigned char *)(FLASH_START_ADDR+i+TEST_BLOCK_OFFSET);
        if( chTmp != (32 + i))
            u32FailureCounter++; 
    }

    if(u32FailureCounter > 0) {
		post_log("\n\nQSPI BSPI Dual lane Read Test: Test Failed\n");
		u32FailureCounter = 0;
        status = -1;
    }
    else {
    	post_log("\n\nQSPI_Dual dual lane Read Test: Test Passed\n");
    }

    //----------------------------------------------------------------------------------------------------
    // **Step 5**: Configure QSPI BSPI (quad-lane mode). Then read out data and compare with data pattern.
    //----------------------------------------------------------------------------------------------------

    //now test BSPI read in quad mode
    *(unsigned int *)0x18027024 = 9;  //3-byte address, quad mode, override strap
    *(unsigned int *)0x18027028 = 1;  //enable flex mode
    *(unsigned int *)0x1802702C = 0x2020202; //quad mode to recv data, 4 mode bits/cycle, 4 address bits/cycle, 4 cmd bits/cycle
    *(unsigned int *)0x18027030 = 8;    //dummy cycle=8, 0 mode bits, 24-byte address
    *(unsigned int *)0x18027034 = 0x3;     //default command is fast read

    // Invalidate bspi buffer
    *(unsigned int *)0x18027018 = 1;
    *(unsigned int *)0x18027020 = 1;

    // Read back Serial Flash through QSPI-BSPI
    post_log("\n\nQSPI: Switch to BSPI quad lane mode, verify data: \n");
    for(i=0; i< 16; i++)
    {
        chTmp = *(unsigned char *)(FLASH_START_ADDR+i+TEST_BLOCK_OFFSET);
        if( chTmp != (32 + i))
            u32FailureCounter++;   //return -1 
     }

     if(u32FailureCounter > 0) {
		post_log("\n\nQSPI BSPI-Quad Read: Test Failed\n");
		u32FailureCounter = 0;
		status = -1;
     }
     else {
		post_log("\n\nQSPI_Quad Read: Test Passed\n");
     }

    //----------------------------------------------------------------------------------------------------
    // **Step 6**: Erase the test block; flash the back-up data to the block.
    //----------------------------------------------------------------------------------------------------

    // Clear software protection bits if any
    // Write enable (precondition for erase)
    mspi_translation(0x06, 1, 0);
    // Erase one block/sector
    // 4KB sector erase
    if(iBlockSize == 4096)
        cmd = 0x20;
    else 
        cmd = 0xd8;

    u32FlashAddress = 0xFF0000;
    cmd |= ((u32FlashAddress >> 8) & 0xff00);
    cmd |= ((u32FlashAddress << 8) & 0xff0000);
    cmd |= ((u32FlashAddress << 24) & 0xff000000);
    mspi_translation(cmd, 4, 0);    //erase the contenet
    
    // Wait for erase complete
    for(;;) {
        rxaddr = mspi_translation(0x05, 1, 1);
        if ((*(unsigned int *)rxaddr & 1) == 0) {
            break;

        }
    }

    for (i = 0; i < 4096/8; i++) {
        cmd = 0x02;
	cmd |= ((u32FlashAddress >> 8) & 0xff00);
        cmd |= ((u32FlashAddress << 8) & 0xff0000);
        cmd |= ((u32FlashAddress << 24) & 0xff000000);
        mspi_translation_w(cmd, (4+8), 0, (pu8DataPattern + i * 8));
        u32FlashAddress += 8;
    } 

    return status;
#endif ////
}

#else

/*
 * Wait until certain bits of the register become what we want
 */
static void wait_for_complete(u32 reg, u32 mask, u32 value)
{
    for(;;) {
        if (((*(unsigned int *)reg) & mask) == value) {
            break;
        }
        udelay(10);
    }
}

/*
 * Generic MSPI translation (max 4 bytes tx)
 *   Return: address to read data back (with +8 increment)
 */
static u32 mspi_translation(u32 tx_data, u32 tx_len, u32 rx_len)
{
    int i;
    unsigned int dst;
    unsigned int total;
    
    // Fill TXRAM
    *(unsigned int *)0x18029240 = tx_data & 0xff;
    *(unsigned int *)0x18029248 = (tx_data >> 8) & 0xff;
    *(unsigned int *)0x18029250 = (tx_data >> 16) & 0xff;
    *(unsigned int *)0x18029258 = (tx_data >> 24) & 0xff;
    
    // Fill CDRAM
    total = tx_len + rx_len;
    dst = 0x18029340;
    for(i=0; i<total; i++, dst+=4) {
        *(unsigned int *)dst = 0x82;
        if (i == total - 1) {
            *(unsigned int *)dst = 0x02;
        }
    }
    
    // Queue pointers
    *(unsigned int *)0x18029210 = 0;
    *(unsigned int *)0x18029214 = total - 1;
    
    // Start it
    *(unsigned int *)0x18029220 = 0x00;
    *(unsigned int *)0x18029218 = 0xc0;
    
    // Wait for complete
    wait_for_complete(0x18029220, 1, 1);
    
    // Return the address of the first byte to read from
    return 0x180292c4 + 8 * tx_len;
}

/*
 * Initialize QSPI and serial flash
 */
void qspi_init(void)
{
    unsigned int rxaddr;

    // Switch to MSPI if not yet
    if (*(unsigned int *)0x18029008 == 0) {    
        // Wait for BSPI ready
        //post_log("\nWait for BSPI ready");
        wait_for_complete(0x1802900c, 1, 0);
        
        // Switch to MSPI
        //post_log("\nSwitch to MSPI");
        *(unsigned int *)0x18029008 = 1;
    }
    
    // Basic MSPI configuration
    *(unsigned int *)0x18029200 = 0x40;
    *(unsigned int *)0x18029204 = 0xa3;
    
    // Read JEDEC ID
    //post_log("\nMSPI: JEDEC read");
    rxaddr = mspi_translation(0x9f, 1, 5);
    JEDEC[0] = *(unsigned char *)rxaddr;
    JEDEC[1] = *(unsigned char *)(rxaddr + 8);
    JEDEC[2] = *(unsigned char *)(rxaddr + 16);
    JEDEC[3] = *(unsigned char *)(rxaddr + 24);
    JEDEC[4] = *(unsigned char *)(rxaddr + 32);


int qspi_post_test (int flags)
{
    u32 tmp, i;

    if ( (flags & POST_AUTO) !=  POST_AUTO )
    {
        post_log("\nQSPI test: Please set strap pin: A1=0xfc", 0);
        post_getConfirmation("\nReady? (Y/N)");
    }
    else
    {
        post_set_strappins("A1 FC%");
    }

    /* MSPI, Read JEDEC info */
    for(i=0; i<5; i++)
        JEDEC[i] = 0;

    qspi_init();

    post_log("\n\nQSPI: Switch to MSPI and read out JEDEC ID: ");
    for(i=0; i<5; i++)
        post_log("\n  JEDEC[%d]=%02x", i, JEDEC[i]);

    // Switch back to BSPI
    *(unsigned int *)0x18029008 = 0;
      
    // Invalidate bspi buffer
    *(unsigned int *)0x18029018 = 1;
    *(unsigned int *)0x18029020 = 1;

    // Read back Serial Flash through QSPI-BSPI
    post_log("\n\nQSPI: Switch to BSPI, verify data: \n");
    for(i=0; i<8; i++) 
    {
        tmp = *(u32 *)(FLASH_START_ADDR+i*4);
        post_log("%08x\t", tmp );
        //Check the instruction op code not relating to the address
        if( (tmp & 0xFF000000) != (vector[i] & 0xFF000000))
            return -1;
    }        

    post_log("\n\nQSPI: Test Passed\n");
    return 0;
}

#endif /* CONFIG_POST & CONFIG_SYS_POST_QSPI */
