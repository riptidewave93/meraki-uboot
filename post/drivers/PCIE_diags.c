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

#include <ns16550.h>


#if CONFIG_POST & CONFIG_SYS_POST_PCIE
//#define USE_PORT_0    1


extern u32 linkError;
/*
 * Per port control structure
 */
typedef struct
{
	char *reg_base_str;
	unsigned long reg_base;
	unsigned long slotRegBase;
}PCIE_PORT_DEF;

static PCIE_PORT_DEF pciePorts[3]=
{
	{"0x08000000", 0x08000000, 0x18012000},
	{"0x40000000", 0x40000000, 0x18013000},
	{"0x48000000", 0x48000000, 0x18014000}
};

extern unsigned long  PORT_IN_USE;
extern unsigned long  OUT_PCI_ADDR;
extern unsigned long  PCI_SLOT_ADDR;

#define UART_MEM_BASE	(0x1000)

#define PCIE_UART_THR	0
#define PCIE_UART_RHR	0
#define PCIE_UART_IER	1
#define PCIE_UART_FCR	2
#define PCIE_UART_ISR	2
#define PCIE_UART_LCR	3
#define PCIE_UART_MCR	4
#define PCIE_UART_LSR	5
#define PCIE_UART_MSR	6
#define PCIE_UART_SPR	7
#define PCIE_UART_DLL	0
#define PCIE_UART_DLM	1

#define IPROC_CCB_MDIO_CTRL              (0x18032000)
#define IPROC_CCB_MDIO_DATA              (0x18032004)

/* Configure for MII needed for usb3, has to be done before usb2 */
#define MII_CTRL_INIT_VAL     0x9a
#define MII_MGMT_CMD_DATA_VAL 0x587e8000
#define MII_MGMT_CMD_DATA_VAL1  0x582EC000
#define MII_MGMT_CMD_DATA_VAL4  0x582E8000


int PCIE_post_enable_phy(void)
{
		__raw_writel(MII_CTRL_INIT_VAL, IPROC_CCB_MDIO_CTRL);
  		mdelay(10);
  		__raw_writel(MII_MGMT_CMD_DATA_VAL, IPROC_CCB_MDIO_DATA);
   		mdelay(10);
   		__raw_writel(MII_MGMT_CMD_DATA_VAL1, IPROC_CCB_MDIO_DATA);
  		mdelay(10);
  		__raw_writel(MII_MGMT_CMD_DATA_VAL4, IPROC_CCB_MDIO_DATA);
  		mdelay(10);
}
int PCIE_post_test (int flags)
{
#ifdef CONFIG_CMD_PCI
        char argv_buf[7*10];
        char * cmd_argv[7];
        int  i;
		unsigned long uart_base;
		unsigned char c = 0x30;
		unsigned char tmp1, tmp2;
		char ch;
		u32 slot = 0,index=0;

#if defined(CONFIG_NS_PLUS)
        if ( post_get_board_diags_type() & SVK_BOARDS )
		{

			if ( (flags & POST_AUTO) !=  POST_AUTO )
			{
				post_log("\nPlease set strap pin:A1=F4 A2=0, A3=0x3, A4=0xf, A5=0x4f, A6=0xf3", 0);
				post_log("\nIf using PCIE 2, please power cycle after changing A1 to F4 ");
				post_getConfirmation("\nReady? (Y/N)");
			}
			else
			{
				post_set_strappins("A1 F4%A2 0%A3 3%A4 f%A5 5f%A6 f3%");
			}
		}
		else
		{
			return (runPCIETestForHR_ER());
		}
#elif defined(CONFIG_NORTHSTAR)
        if ( (flags & POST_AUTO) !=  POST_AUTO )
        {
            post_log("\nPlease set strap pin:A1=F4 A2=0, A3=0x3, A4=0xf, A5=0x4f, A6=0xf3", 0);
            post_log("\nIf using PCIE 2, please power cycle after changing A1 to F4 ");
            post_getConfirmation("\nReady? (Y/N)");
        }
        else
        {
            post_set_strappins("A1 F4%A2 0%A3 3%A4 f%A5 5f%A6 f3%");
        }
#endif

        //Auto test still defaults to port 1
        //Manual test - user selectable
        if ( (flags & POST_AUTO) !=  POST_AUTO )
        {

#if (defined(CONFIG_NS_PLUS) || defined(CONFIG_NORTHSTAR))
			post_log("\n\nSelect PCI slot # to test:0 - PCIe0; 1 - PCIe1; 2 - PCIe2 \n");
#elif (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2))
			post_log("\n\nSelect PCI slot # to test:0 - PCIe0; 1 - PCIe1\n");
#endif
#if (defined(CONFIG_NS_PLUS) || defined(CONFIG_NORTHSTAR))
			slot = post_getUserInput("Slot #? (0/1/2) : ");
#elif (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2))
			slot = post_getUserInput("Slot #? (0/1) : ");
#elif defined(CONFIG_HURRICANE2)
			slot = 0;
#endif

			switch(slot)
			{
				case 0:
					index = 0;
					break;
				case 1:
					index = 1;
					break;
				case 2:
                    post_log("\n\nSet PCIe2 PHY through MDIO\n");  
                    PCIE_post_enable_phy();
					index = 2;
					break;
				default:
					index = 0;
					break;
			}

			PORT_IN_USE = index;
			OUT_PCI_ADDR = pciePorts[index].reg_base;
			PCI_SLOT_ADDR = pciePorts[index].slotRegBase;
        }

        uart_base = pciePorts[index].reg_base + UART_MEM_BASE;
        memset(&argv_buf, 0, 7*10);
        for(i=0; i<7; i++)
		cmd_argv[i] = &argv_buf[i*10];


        /* PCI Enum */
        printf("\n\nPCIE Enumeration\n");
        strcpy(cmd_argv[0], "pci");
        strcpy(cmd_argv[1], "enum");
        do_pci(0, 0, 2, cmd_argv);
		udelay(2000*100);

		if ( linkError != 0 )
		{
			goto failed;
		}

        printf("\n\nPCIE header display, BDF<00:00:00>\n");
        strcpy(cmd_argv[0], "pci");
        strcpy(cmd_argv[1], "header");
        strcpy(cmd_argv[2], "0.0.0");
        do_pci(0, 0, 3, cmd_argv);
		udelay(2000*100);


		printf("\n\nPCIE EP memory raw access, enable port0/1\n");
		printf("Device ID: 0x%08x\n", __raw_readl(pciePorts[index].reg_base ) );
		__raw_writel(0x3, pciePorts[index].reg_base+4);
		printf("Port enabled: 0x%08x\n", __raw_readl(pciePorts[index].reg_base  +4) );
		udelay(2000);


		/* PCI memory dump */
        printf("\n\nPCIE memory dump\n");
        strcpy(cmd_argv[0], "md");
        strcpy(cmd_argv[1], pciePorts[index].reg_base_str);
        do_mem_md(0, 0, 2, cmd_argv);

		/* Serial init */		
		{
			/* Force 450 mode */
			__raw_writeb(0x00, 			uart_base + PCIE_UART_FCR);	
			__raw_writeb(UART_LCR_BKSE | UART_LCR_8N1, 	uart_base + PCIE_UART_LCR);	

			/* Set Baud 115K */
			__raw_writeb(0x22, uart_base + PCIE_UART_DLL);
			__raw_writeb(0,    uart_base + PCIE_UART_DLM);
			
			tmp1 = __raw_readb(uart_base + PCIE_UART_DLM);
			tmp2 = __raw_readb(uart_base + PCIE_UART_DLL);
			printf("\nRead out Baud DLM=0x%02x, DLL=0x%02x\n\n", tmp1, tmp2);
			if((tmp1!=0) || (tmp2!=0x22))
				goto failed;
		
			__raw_writeb(UART_LCR_8N1, 	uart_base + PCIE_UART_LCR);	

		    udelay(1000);
			__raw_writeb(0x00, 			uart_base + PCIE_UART_IER);	
			__raw_writeb((UART_FCR_RXSR | UART_FCR_TXSR),   uart_base + PCIE_UART_FCR);
			__raw_writeb((UART_MCR_DTR | UART_MCR_RTS),     uart_base + PCIE_UART_MCR);	

            udelay(1000);
			printf("\nRead out UART Regs <0x%08x>=0x%08x, <0x%08x>=0x%08x\n\n", 
						uart_base, 		__raw_readl(uart_base),
						uart_base + 4, 	__raw_readl(uart_base + 4) );

			if ( flags & POST_AUTO)
			{

#if (defined(CONFIG_NORTHSTAR) || defined(CONFIG_NS_PLUS))

			    __raw_writeb('Y', uart_base + PCIE_UART_THR);
			    mdelay(100);

			   // while( _serial_tstc(2) == 0  ){}

                if (_serial_tstc(2) != 0 )
                {
                    ch = (char)_serial_getc(2);
                    if ( ('y' == ch) || ('Y'==ch ))
                    {
                        post_log("\nReceived char on console 2\n PCIE test passed");
                        return 0;
                    }
                }
                post_log("\n PCIE test failed");
                return -1;
#else
                printf("\nUART setting: 8bit data, NoneParity, 1bit stop, 115K baud rate\n");
                printf("\nStart to output 2K chars....\n\n");
                /* Send char out */
                for(i=0; i<2000; i++)
                {
                    if(c++ > 0x7a) c = 0x30;
                    while ((__raw_readb(uart_base + PCIE_UART_LSR) & UART_LSR_THRE) == 0);
                    __raw_writeb(c, uart_base + PCIE_UART_THR);
                    udelay(1000*10);
                }
#endif
			}
			else
			{
	            printf("\nUART setting: 8bit data, NoneParity, 1bit stop, 115K baud rate\n");
	            printf("\nStart to output 2K chars....\n\n");
                /* Send char out */
                for(i=0; i<2000; i++)
                {
                    if(c++ > 0x7a) c = 0x30;
                    while ((__raw_readb(uart_base + PCIE_UART_LSR) & UART_LSR_THRE) == 0);
                    __raw_writeb(c, uart_base + PCIE_UART_THR);
                    udelay(1000*10);
                }
			}	
		}
#endif /* CONFIG_CMD_PCI */

    	post_log("\n\nPCIE test done\n");
	    return 0;

failed:
    	post_log("\n\nPCIE test failed\n");
		return -1;
}
#if defined(CONFIG_NS_PLUS)
#define NUM_PORT 2
int runPCIETestForHR_ER ( void )
{
    char argv_buf[7*10];
    char * cmd_argv[7];
	unsigned long uart_base;
	char ch;
	u32 slot,i,index=1;
	u32 ret = 0;
	int errorCode[NUM_PORT]= {0};


	for ( index = 0; index < NUM_PORT; index++ )
	{

		PORT_IN_USE = index;
		OUT_PCI_ADDR = pciePorts[index].reg_base;
		PCI_SLOT_ADDR = pciePorts[index].slotRegBase;


		uart_base = pciePorts[index].reg_base + UART_MEM_BASE;
		memset(&argv_buf, 0, 7*10);
		for(i=0; i<7; i++)
		cmd_argv[i] = &argv_buf[i*10];


		/* PCI Enum */
		post_log("\n\nPCIE Enumeration\n");
		strcpy(cmd_argv[0], "pci");
		strcpy(cmd_argv[1], "enum");
		do_pci(0, 0, 2, cmd_argv);
		udelay(2000*100);

		if ( linkError == 0 )
		{

            if ( (post_get_board_diags_type() == BCM958622HR) && ( 0 == index ) )
			{
				dumpPCIEDeviceOnSwitch();
			}
			else
			{
                    
				post_log("\n\nPCIE header display, BDF<01:00:00>\n");
				strcpy(cmd_argv[0], "pci");
				strcpy(cmd_argv[1], "header");
				strcpy(cmd_argv[2], "1.0.0");
				do_pci(0, 0, 3, cmd_argv);
				udelay(2000*100);


				/* PCI memory dump */
				post_log("\n\nPCIE memory dump\n");
				strcpy(cmd_argv[0], "md");
				strcpy(cmd_argv[1], pciePorts[index].reg_base_str);
				do_mem_md(0, 0, 2, cmd_argv);
			 }
		}
		else
		{
			ret = -1;

			errorCode[index] = -1;
		}
	}

	for ( index = 0; index < NUM_PORT; index++ )
	{
		if(-1 == errorCode[index])
		{
			post_log("\nPort %d failed\n", index);
		}
		else
		{
			post_log("\nPort %d passed\n", index);
		}
	}
    return ret;
}
#endif
#endif /* CONFIG_POST & CONFIG_SYS_POST_PCIE */
