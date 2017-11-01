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
#include <asm/io.h>
#include <xhci.h>
#include <scsi.h>

#if defined(CONFIG_NS_PLUS)
#define IPROC_CCB_MDIO_CTRL              (0x18032000)
#define IPROC_CCB_MDIO_DATA              (0x18032004)

#define MII_CTRL_INIT_VAL       0x9a
#define MII_MGMT_CMD_DATA_VAL   0x587e8000
#define MII_MGMT_CMD_DATA_VAL1  0x582a6400


#define IPROC_IDM_USB3_RESET_CONTROL	 (0x18104800)
#define IPROC_USB3_BASE_ADDR	         (0x18029000)

#define  XHCI_BASE               0x6e000000
#define  XHCI_DCB_TABLE         (XHCI_BASE)
#define  DCB_SLOT0_SCATCHBU     (XHCI_BASE + 0x110000)
#define  DCB_SLOT1_DEVICE_0     (XHCI_BASE  + 0x120000)
#define  DCB_SLOT2_DEVICE_1     (XHCI_BASE  + 0x130000)
#define  XHCI_EVENT_SEGMENT     (XHCI_BASE  + 0x00C000)

#define  XHCI_SCRATCH_BUF0      (XHCI_BASE  + 0x180000)
#define  XHCI_SCRATCH_BUF1      (XHCI_BASE  + 0x1c0000)
#define  XHCI_INPUT_CONTEXT     (XHCI_BASE  + 0x100000)

#define  XHCI_COMMAND_RING      (XHCI_BASE +  0x200000)
#define  XHCI_EVENT_RING        (XHCI_BASE +  0x300000)
#define  XHCI_EP0_RING          (XHCI_BASE +  0x600000)
#define  XHCI_BULK_IN_RING      (XHCI_BASE +  0x800000)
#define  XHCI_BULK_OUT_RING     (XHCI_BASE +  0xa00000)

#define XHCI_CHAP9_BASE         (0x6f000000)
#define  XHCI_CH9_DESC          (XHCI_CHAP9_BASE)
#define  XHCI_CH9_CONFIG        (XHCI_CHAP9_BASE + 0x000020)
#define  XHCI_CH9_POOL          (XHCI_CHAP9_BASE + 0x001000)

#define  XHCI_MSC_CMD_BUF       (XHCI_CHAP9_BASE + 0x004000)
#define  XHCI_MSC_STATUS_BUF    (XHCI_CHAP9_BASE + 0x008000)
#define  XHCI_MSC_REPLY_BUF     (XHCI_CHAP9_BASE + 0x00c000)


#else

#define IPROC_CCB_MDIO_CTRL              (0x18003000)
#define IPROC_CCB_MDIO_DATA              (0x18003004)

#define MII_CTRL_INIT_VAL       0x9a
#define MII_MGMT_CMD_DATA_VAL   0x587e8000
#define MII_MGMT_CMD_DATA_VAL1  0x582a6400


#define IPROC_IDM_USB3_RESET_CONTROL	 (0x18105800)
#define IPROC_USB3_BASE_ADDR	         (0x18023000)

#define  XHCI_DCB_TABLE         (0x89000000)
#define  DCB_SLOT0_SCATCHBU     (0x89110000)
#define  DCB_SLOT1_DEVICE_0     (0x89120000)
#define  DCB_SLOT2_DEVICE_1     (0x89130000)
#define  XHCI_EVENT_SEGMENT     (0x8900C000)

#define  XHCI_SCRATCH_BUF0      (0x89180000)
#define  XHCI_SCRATCH_BUF1      (0x891c0000)
#define  XHCI_INPUT_CONTEXT     (0x89100000)

#define  XHCI_COMMAND_RING      (0x89200000)
#define  XHCI_EVENT_RING        (0x89300000)
#define  XHCI_EP0_RING          (0x89600000)
#define  XHCI_BULK_IN_RING      (0x89800000)
#define  XHCI_BULK_OUT_RING     (0x89a00000)

#define  XHCI_CH9_DESC          (0x8A000000)
#define  XHCI_CH9_CONFIG        (0x8A000020)
#define  XHCI_CH9_POOL          (0x8A001000)

#define  XHCI_MSC_CMD_BUF       (0x8A004000)
#define  XHCI_MSC_STATUS_BUF    (0x8A008000)
#define  XHCI_MSC_REPLY_BUF     (0x8A00c000)

#endif

#define USB3_RUNTIME            (0x4A0)
#define USB3_DBOFF              (0x8c0)
#define XHCI_DB_CMD_RING        (USB3_DBOFF)
#define XHCI_DB_DEV_1           (USB3_DBOFF + 4)




volatile struct xhci_event_cmd_trb   *pEvent_trb, *pEvent_root;
volatile struct xhci_event_cmd_trb   *pCmd_trb, *pCmd_root;

volatile struct xhci_slot_ctx           *pInput_slot_ctx;
volatile struct xhci_ep_ctx             *pInput_ep0_ctx;
volatile struct xhci_ep_ctx             *pInput_ep_bulk_in_ctx;
volatile struct xhci_ep_ctx             *pInput_ep_bulk_out_ctx;
volatile struct xhci_input_control_ctx  *pInput_inputCtrl_ctx;

volatile struct xhci_slot_ctx        *pOutput_slot_ctx;
volatile struct xhci_ep_ctx          *pOutput_ep0_ctx;
volatile struct xhci_ep_ctx          *pOutput_ep_bulk_in_ctx;

volatile struct xhci_transfer_event  *pEp0_ring;
volatile struct xhci_transfer_event  *pEp_in_ring;
volatile struct xhci_transfer_event  *pEp_out_ring;

volatile umass_bbb_cbw_t    *pCbw_cmd;
volatile umass_bbb_csw_t    *pCsw_status;
volatile char               *pCbw_data;

static u32 event_ring_dequeue_ptr;
static u32 CBWTag = 1;
static u32 link_state = 0;
static u32 link_cnt = 0;
static u32 ep_halt_state = 0;
static u32 ep_in_num, ep_out_num, DCI_ep_in, DCI_ep_out;

/* 64K entries of TRB, * (16 bytes per TRB), = 1MByte, 0x10_0000 */
#define  MAX_EVENT_RING_SIZE    64*1024*16
#define  MAX_CMD_RING_SIZE      64*1024*16

/* WAIT_TIME_OUT, 5s */
#define  EVT_TIME_OUT           (500*1000)

#define debug printf

static inline void xhci_writel(u32 addr, unsigned int val)
{
	debug("Write [0x%08x] = 0x%08x\n", (u32)addr, val);
	*(volatile unsigned int *)(IPROC_USB3_BASE_ADDR + addr) = (volatile unsigned int)val;
}

static inline u32 xhci_readl(u32 addr)
{
	volatile unsigned int val = *(volatile unsigned int *)(IPROC_USB3_BASE_ADDR + addr);
	debug("Read  [0x%08x] = 0x%08x\n", (u32)addr, val);
	return (u32)val;
}

void xhci_hcd_reset(void)
{
    u32 tmp;
    debug("\nHCD Hard Reset\n");
    xhci_writel(USB3_USBCMD, CMD_RESET);
    do {
        udelay(1);
        tmp = xhci_readl( USB3_USBCMD );
    } while(tmp & CMD_RESET);
}

void xhci_port_warm_reset(void)
{
    u32 tmp;
    debug("\nPort Warm Reset\n");

    tmp = xhci_readl( USB3_PORTSC );
    /* clear reset status bits first */
    tmp |= (1<<19) | (1<<21);
    xhci_writel(USB3_PORTSC, tmp);
    
    tmp = xhci_readl( USB3_PORTSC );
    xhci_writel(USB3_PORTSC, tmp | PORT_WR);
    xhci_writel(USB3_PORTSC, tmp | PORT_RESET | PORT_WR);

    /* Wait Warm Reset Complete, bit19 warm completed, bit21 reset completed */
    do {
        udelay(10);
        tmp = xhci_readl( USB3_PORTSC );
    } while( (tmp & (1<<19)) && (tmp & (1<<21)) );

    /* clear reset status bits */
    tmp |= (1<<19) | (1<<21);
    xhci_writel(USB3_PORTSC, tmp);
    debug("\nPort Warm Reset Done\n");
}

/**************************************************** 
    USB Init, memory init, data structure build
*****************************************************/
int xhci_hcd_init(void)
{
//	int retval = 0;
    int usb3_reset_state, usb3_cnr;

#if ORIGINAL
    printf("\n Configure USB 3.0 PHY \n");
    writel(MII_CTRL_INIT_VAL ,     IPROC_CCB_MDIO_CTRL);
    udelay(10*1000);
    writel(MII_MGMT_CMD_DATA_VAL,  IPROC_CCB_MDIO_DATA);
    udelay(10*1000);
    writel(MII_MGMT_CMD_DATA_VAL1, IPROC_CCB_MDIO_DATA);
    udelay(100*1000);
#endif

#if defined(CONFIG_NS_PLUS)
    /* Configure for MII needed for usb3, has to be done before usb2 */
    #define MII_CTRL_INIT_VAL     0x9a
    #define MII_MGMT_CMD_DATA_VAL 0x587e8000
    #define MII_MGMT_CMD_DATA_VAL1  0x582a6400
    #define MII_MGMT_CMD_DATA_VAL2  0x58061000
    #define MII_MGMT_CMD_DATA_VAL3  0x582EC000
    #define MII_MGMT_CMD_DATA_VAL4  0x582E8000
    #define MII_MGMT_CMD_DATA_VAL5  0x58069000
    #define IPROC_IDM_USB2_RESET_CONTROL	 (0x18115800)
    #define IPROC_IDM_USB2_RESET_CONTROL_VA   IPROC_IDM_USB2_RESET_CONTROL//HW_IO_PHYS_TO_VIRT(IPROC_IDM_USB2_RESET_CONTROL)
    #define IPROC_IDM_USB3_RESET_CONTROL	 (0x18104800)
    #define IPROC_IDM_USB3_RESET_CONTROL_VA   IPROC_IDM_USB3_RESET_CONTROL //HW_IO_PHYS_TO_VIRT(IPROC_IDM_USB3_RESET_CONTROL)


    printf("\n Configure USB 3.0 PHY \n");
    	__raw_writel(0x1, IPROC_IDM_USB2_RESET_CONTROL_VA);
    	mdelay(10);
    	__raw_writel(0x1, IPROC_IDM_USB3_RESET_CONTROL_VA);



    	{
    		__raw_writel(MII_CTRL_INIT_VAL, IPROC_CCB_MDIO_CTRL);
    		mdelay(10);
    		__raw_writel(MII_MGMT_CMD_DATA_VAL, IPROC_CCB_MDIO_DATA);
    		mdelay(10);
    		__raw_writel(MII_MGMT_CMD_DATA_VAL2, IPROC_CCB_MDIO_DATA);
    		mdelay(10);
    		__raw_writel(MII_MGMT_CMD_DATA_VAL1, IPROC_CCB_MDIO_DATA);
    		mdelay(10);
    		__raw_writel(MII_MGMT_CMD_DATA_VAL3, IPROC_CCB_MDIO_DATA);
    		mdelay(10);
    		__raw_writel(MII_MGMT_CMD_DATA_VAL4, IPROC_CCB_MDIO_DATA);
    		mdelay(10);
    		__raw_writel(MII_MGMT_CMD_DATA_VAL5, IPROC_CCB_MDIO_DATA);
    	}
    	__raw_writel(0x0, IPROC_IDM_USB2_RESET_CONTROL_VA);
    	mdelay(10);
    	__raw_writel(0x0, IPROC_IDM_USB3_RESET_CONTROL_VA);
#else
        printf("\n Configure USB 3.0 PHY \n");
        writel(MII_CTRL_INIT_VAL ,     IPROC_CCB_MDIO_CTRL);
        udelay(10*1000);
        writel(MII_MGMT_CMD_DATA_VAL,  IPROC_CCB_MDIO_DATA);
        udelay(10*1000);
        writel(MII_MGMT_CMD_DATA_VAL1, IPROC_CCB_MDIO_DATA);
        udelay(100*1000);
#endif
#if ORIGINAL
    printf("\n xhci_hcd_init: release USB3.0 Reset\n");
    usb3_reset_state = readl(IPROC_IDM_USB3_RESET_CONTROL);
    if ((usb3_reset_state & 1) == 1)
    {
        writel(0x0, IPROC_IDM_USB3_RESET_CONTROL);
        usb3_reset_state = readl(IPROC_IDM_USB3_RESET_CONTROL);
    }
#endif
    // Wait CNR    
    debug("\n xhci_hcd_init: Polling CNR\n");
    do {
        usb3_cnr = xhci_readl(USB3_USBSTS);
    } while(usb3_cnr & STS_CNR);

    // HCD Hard Reset
    xhci_hcd_reset();
    // Wait CNR again    
    debug("\n xhci_hcd_init: Polling CNR\n");
    do {
        usb3_cnr = xhci_readl(USB3_USBSTS);
    } while(usb3_cnr & STS_CNR);

    // Build critical data structures
    debug("\n malloc critical structures\n");
    xhci_hcd_mem_init();

    // MaxSlot
    xhci_writel(USB3_CONFIG, MAX_DEVS(0x1));

    // DCB context
    // Add Scatch Buffer to Scatch Buffer Array
    writel(XHCI_SCRATCH_BUF0,  DCB_SLOT0_SCATCHBU);     // Scatch Buffer Array item 0
    writel(XHCI_SCRATCH_BUF1,  DCB_SLOT0_SCATCHBU + 8); // Scatch Buffer Array item 1

    // Populate DCB Array
    writel(DCB_SLOT0_SCATCHBU,  XHCI_DCB_TABLE);        // Item 0, reserved to Scatch buffer array
    writel(DCB_SLOT1_DEVICE_0,  XHCI_DCB_TABLE + 0x8);    // Item 1, first Slot/device
    writel(DCB_SLOT2_DEVICE_1,  XHCI_DCB_TABLE + 0x10);

    xhci_writel(USB3_DCBAAPL, XHCI_DCB_TABLE);
    xhci_writel(USB3_DCBAAPH, 0);

    // Command Ring
    xhci_writel(USB3_CRCRL, XHCI_COMMAND_RING | 0x1);   // RCS, Ring Cycle State = 1 
    memset( XHCI_COMMAND_RING, 0, 0x200000);
    xhci_writel(USB3_CRCRH, 0);

    // Event Ring Seg Table Size = 1
    xhci_writel(USB3_RUNTIME + 0x28 + 0, 1);

    // Populate Segment item, only 1 event segment, 2K event TRB entries
    // Event_Segment_table(0) = XHCI_EVENT_RING (first TRB entry)
    writel(XHCI_EVENT_RING,  XHCI_EVENT_SEGMENT);       // Address, u64
    writel(2048,             XHCI_EVENT_SEGMENT + 8);   // Size, u32
    
    // Event Ring Dequeue PTR
    xhci_writel(USB3_RUNTIME + 0x38 + 0, XHCI_EVENT_RING);
    xhci_writel(USB3_RUNTIME + 0x3C + 0, 0);

    // Event Segment Table, ESTBA
    xhci_writel(USB3_RUNTIME + 0x30 + 0, XHCI_EVENT_SEGMENT);
    xhci_writel(USB3_RUNTIME + 0x34 + 0, 0);

    // Cmd Run
    xhci_writel(USB3_USBCMD, CMD_RUN);

    link_state = 0;    
    return 0;
}

int xhci_hcd_mem_init(void)
{
    /* Build all rings */
    pEvent_trb   = (struct xhci_event_cmd_trb *)(XHCI_EVENT_RING);
    pCmd_trb     = (struct xhci_event_cmd_trb *)(XHCI_COMMAND_RING);
    pEvent_root  = pEvent_trb;
    pCmd_root    = pCmd_trb;
    event_ring_dequeue_ptr = XHCI_EVENT_RING;

    pEp0_ring       = (struct xhci_transfer_event *)(XHCI_EP0_RING);
    pEp_in_ring     = (struct xhci_transfer_event *)(XHCI_BULK_IN_RING);
    pEp_out_ring    = (struct xhci_transfer_event *)(XHCI_BULK_OUT_RING);

    /* Create Input DCB context */
    pInput_inputCtrl_ctx = (struct xhci_input_control_ctx *)(XHCI_INPUT_CONTEXT);
    pInput_slot_ctx       = (struct xhci_slot_ctx *)(XHCI_INPUT_CONTEXT + DCB_ENTRY_SZIE);       // 16 or 32 Dwords of CTX
    pInput_ep0_ctx         = (struct xhci_ep_ctx *)(XHCI_INPUT_CONTEXT + DCB_ENTRY_SZIE * 2);

    xhci_create_input_ctx(); 

    /* Create Output DCB context */
    /* Since Slot_0 context init as 0, the dev_state = 0 (disabled) */
    pOutput_slot_ctx       = (struct xhci_slot_ctx *)(DCB_SLOT1_DEVICE_0);
    pOutput_ep0_ctx         = (struct xhci_ep_ctx *)(DCB_SLOT1_DEVICE_0 + DCB_ENTRY_SZIE);

    /* USB MSC CMD buffers */
    pCbw_cmd    = (umass_bbb_cbw_t *)(XHCI_MSC_CMD_BUF);
    pCsw_status = (umass_bbb_csw_t *)(XHCI_MSC_STATUS_BUF);
    pCbw_data   = (char *)(XHCI_MSC_REPLY_BUF);

    return 0;
}

void xhci_create_input_ctx(void)
{
    u32 temp;

    pInput_inputCtrl_ctx->add_flags = 0x3;   // Add A0 and A1

    temp = SLOT_SPEED_SS;
    temp |= 0;  // ROUTE_STRING_MASK, Find out route string
    temp |= LAST_CTX(1);        // Add EP0 for now
     
    pInput_slot_ctx->dev_info     = temp;
    pInput_slot_ctx->dev_info2    = ROOT_HUB_PORT(1); 
    pInput_slot_ctx->tt_info      = 0;
    pInput_slot_ctx->dev_state    = 0;

    /* EP0 Context */
    pInput_ep0_ctx->ep_info    = 0;
    pInput_ep0_ctx->ep_info2   = EP_TYPE(CTRL_EP) | MAX_BURST(0) | MAX_PACKET(512) | ERROR_COUNT(3);
    pInput_ep0_ctx->deq_l      = XHCI_EP0_RING | 1;    // DCS = 1
    pInput_ep0_ctx->deq_h      = 0;
    pInput_ep0_ctx->tx_info    = 0;
}

void xhci_update_input_ctx(void)
{
    u32 DCI_last_ctx;

    /* find out which bulk endpoint from Get_Config */
    /* Item 0, input_ctrl; Item 1, Slot_ctx; Itme 2, EP0_ctx */
    /* Item 3, EP1_OUT; Item 4, EP1_IN; Item 5, EP2_OUT */
    /* Get_Config, EP-1, IN, item 4; EP-2, OUT, item 5 */

    DCI_ep_in   = (ep_in_num * 2) + 1;    // IN plus 1
    DCI_ep_out  = (ep_out_num * 2);       // OUT plus 0
    debug("\n Update DCI index, ep_in=%d, ep_out=%d, ", DCI_ep_in, DCI_ep_out);

    /* ICI = DCI + 1 */
    pInput_ep_bulk_in_ctx = (struct xhci_ep_ctx *)(XHCI_INPUT_CONTEXT + DCB_ENTRY_SZIE * (DCI_ep_in + 1) );
    pInput_ep_bulk_out_ctx = (struct xhci_ep_ctx *)(XHCI_INPUT_CONTEXT + DCB_ENTRY_SZIE * (DCI_ep_out + 1) );

    pInput_ep_bulk_in_ctx->ep_info    = 0;
    pInput_ep_bulk_in_ctx->ep_info2   = EP_TYPE(BULK_IN_EP) | MAX_BURST(0) | MAX_PACKET(1024) | ERROR_COUNT(3);
    pInput_ep_bulk_in_ctx->deq_l      = XHCI_BULK_IN_RING | 1;
    pInput_ep_bulk_in_ctx->deq_h      = 0;
    pInput_ep_bulk_in_ctx->tx_info    = 0;

    pInput_ep_bulk_out_ctx->ep_info    = 0;
    pInput_ep_bulk_out_ctx->ep_info2   = EP_TYPE(BULK_OUT_EP) | MAX_BURST(0) | MAX_PACKET(1024) | ERROR_COUNT(3);
    pInput_ep_bulk_out_ctx->deq_l      = XHCI_BULK_OUT_RING | 1; 
    pInput_ep_bulk_out_ctx->deq_h      = 0;
    pInput_ep_bulk_out_ctx->tx_info    = 0;

    /* Change Slot_ctx entries */
    if(DCI_ep_in > DCI_ep_out)
        DCI_last_ctx = DCI_ep_in;
    else
        DCI_last_ctx = DCI_ep_out;

    pInput_slot_ctx->dev_info   &= ~LAST_CTX_MASK;
    pInput_slot_ctx->dev_info   |= LAST_CTX( DCI_last_ctx );     // last valid EP DCI 

    /* Add Bulk EP into Context, Set A0, Ingore A1 */
    /* Add A3 (EP1-IN) and A4 (EP2-OUT) */
    pInput_inputCtrl_ctx->add_flags = (1<<0) | (1<<DCI_ep_in) | (1<<DCI_ep_out);   

    /* Get hold of Bulk_in Context, DCB index = 3 */
    pOutput_ep_bulk_in_ctx = (struct xhci_ep_ctx *)(DCB_SLOT1_DEVICE_0 + DCB_ENTRY_SZIE * DCI_ep_in);
}
void xhci_ring_doorbell(u32 ep_num, u32 direction)
{
    if(ep_num == 0)
        xhci_writel(XHCI_DB_DEV_1, 0x1);
    else
        xhci_writel(XHCI_DB_DEV_1, (ep_num*2)+direction );
}

/*************************************** 
    USB Mass Storage functions,
    Work on Bulk In and Out EP.
****************************************/
void xhci_queue_bulk_tx(u32 buf_ptr, u32 xfer_len, u32 IOC_bit)
{
    /* Bulk-out data */
    pEp_out_ring->buffer_l     = buf_ptr;
    pEp_out_ring->buffer_h     = 0;
    pEp_out_ring->transfer_len = xfer_len;
    pEp_out_ring->flags        = TRB_TYPE(TRB_NORMAL) | IOC_bit | 0x1;
    pEp_out_ring ++;
}

void xhci_queue_bulk_data_write(u32 buf_ptr, u32 xfer_len, u32 IOC_bit)
{
    u32 td_count;

    td_count = (xfer_len+1023)/1024;
    if(td_count == 0)
        td_count = 1;

    debug("\nWrite_10 : xfer_len=%d, td_cnt=%d, ", xfer_len, td_count);
    /* Bulk-out data */
    pEp_out_ring->buffer_l     = buf_ptr;
    pEp_out_ring->buffer_h     = 0;
    pEp_out_ring->transfer_len = xfer_len | (td_count<<17);
    pEp_out_ring->flags        = TRB_TYPE(TRB_NORMAL) | IOC_bit | (1<<2) | 0x1;
    pEp_out_ring ++;
}

void xhci_queue_bulk_rx(u32 buf_ptr, u32 xfer_len, u32 IOC_bit)
{
    /* Bulk-in data */
    pEp_in_ring->buffer_l     = buf_ptr;
    pEp_in_ring->buffer_h     = 0;
    pEp_in_ring->transfer_len = xfer_len;
    pEp_in_ring->flags        = TRB_TYPE(TRB_NORMAL) | IOC_bit | 0x1;
    pEp_in_ring ++;
}

void xhci_queue_bulk_data_read(u32 buf_ptr, u32 xfer_len, u32 IOC_bit)
{
    u32 td_count;

    td_count = (xfer_len+1023)/1024;
    debug("\nRead_10 : xfer_len=%d, td_cnt=%d, ", xfer_len, td_count);
    /* Bulk-in data */
    pEp_in_ring->buffer_l     = buf_ptr;
    pEp_in_ring->buffer_h     = 0;
    pEp_in_ring->transfer_len = xfer_len | (td_count<<17);
    /* ISP short packet is on */
    pEp_in_ring->flags        = TRB_TYPE(TRB_NORMAL) | IOC_bit | (1<<2) | 0x1;
    pEp_in_ring ++;
}

void xhci_msc_inquiry_cmd(void)
{
    /* INQUIRY */
    pCbw_cmd->dCBWSignature         = CBWSIGNATURE;
    pCbw_cmd->dCBWTag               = CBWTag ++;
    pCbw_cmd->dCBWDataTransferLength    = 36;   // expect 36 byte vendor ID etc.
    pCbw_cmd->bCBWFlags             = CBWFLAGS_IN;
    pCbw_cmd->bCBWLUN               = 0;        // WDC LUN =0, get from GET_LUN
    pCbw_cmd->bCDBLength            = 6;        // Inquiry cmd_len = 6

    memset((u8 *)pCbw_cmd->CBWCDB, 0, CBWCDBLENGTH);
    pCbw_cmd->CBWCDB[0]  = SCSI_INQUIRY;    
    pCbw_cmd->CBWCDB[4]  = 36;

    xhci_queue_bulk_tx((u32)pCbw_cmd, UMASS_BBB_CBW_SIZE, 0);

    /* Move to next Command */
    pCbw_cmd ++;    
}

void xhci_msc_read_capacity(void)
{
    /* READ_CAPACITY */
    pCbw_cmd->dCBWSignature         = CBWSIGNATURE;
    pCbw_cmd->dCBWTag               = CBWTag ++;
    pCbw_cmd->dCBWDataTransferLength    = 8;   // expect 8 byte capacity info.
    pCbw_cmd->bCBWFlags             = CBWFLAGS_IN;
    pCbw_cmd->bCBWLUN               = 0;        // WDC LUN =0
    pCbw_cmd->bCDBLength            = 10;       // ReadCap cmd_len = 10

    memset((u8 *)pCbw_cmd->CBWCDB, 0, CBWCDBLENGTH);
    pCbw_cmd->CBWCDB[0]  = SCSI_RD_CAPAC;    

    xhci_queue_bulk_tx((u32)pCbw_cmd, UMASS_BBB_CBW_SIZE, 0);

    /* Move to next Command */
    pCbw_cmd ++;    
}

void xhci_msc_mode_sense(void)
{
    /* MODE_SENSE */
    pCbw_cmd->dCBWSignature         = CBWSIGNATURE;
    pCbw_cmd->dCBWTag               = CBWTag ++;
    pCbw_cmd->dCBWDataTransferLength    = 0xC0;   // expect 8 byte capacity info.
    pCbw_cmd->bCBWFlags             = CBWFLAGS_IN;
    pCbw_cmd->bCBWLUN               = 0;        // WDC LUN =0
    pCbw_cmd->bCDBLength            = 6; 

    memset((u8 *)pCbw_cmd->CBWCDB, 0, CBWCDBLENGTH);
    pCbw_cmd->CBWCDB[0]  = SCSI_MODE_SEN6;    
    pCbw_cmd->CBWCDB[2]  = 0x3f;        // Page Code
    pCbw_cmd->CBWCDB[4]  = 0xC0;

    xhci_queue_bulk_tx((u32)pCbw_cmd, UMASS_BBB_CBW_SIZE, 0);

    /* Move to next Command */
    pCbw_cmd ++;    
}

void xhci_msc_request_sense(void)
{
    /* MODE_SENSE */
    pCbw_cmd->dCBWSignature         = CBWSIGNATURE;
    pCbw_cmd->dCBWTag               = CBWTag ++;
    pCbw_cmd->dCBWDataTransferLength    = 0x60;   
    pCbw_cmd->bCBWFlags             = CBWFLAGS_IN;
    pCbw_cmd->bCBWLUN               = 0;        // WDC LUN =0
    pCbw_cmd->bCDBLength            = 6; 

    memset((u8 *)pCbw_cmd->CBWCDB, 0, CBWCDBLENGTH);
    pCbw_cmd->CBWCDB[0]  = SCSI_REQ_SENSE;    
    pCbw_cmd->CBWCDB[4]  = 0x60;

    xhci_queue_bulk_tx((u32)pCbw_cmd, UMASS_BBB_CBW_SIZE, 0);

    /* Move to next Command */
    pCbw_cmd ++;    
}

void xhci_msc_test_unit(void)
{
    /* TEST_UNIT_READY */
    pCbw_cmd->dCBWSignature         = CBWSIGNATURE;
    pCbw_cmd->dCBWTag               = CBWTag ++;
    pCbw_cmd->dCBWDataTransferLength    = 0;   // No return
    pCbw_cmd->bCBWFlags             = CBWFLAGS_OUT;
    pCbw_cmd->bCBWLUN               = 0;        // WDC LUN =0
    pCbw_cmd->bCDBLength            = 6;        // cmd_len = 6

    memset((u8 *)pCbw_cmd->CBWCDB, 0, CBWCDBLENGTH);
    pCbw_cmd->CBWCDB[0]  = SCSI_TST_U_RDY;    

    xhci_queue_bulk_tx((u32)pCbw_cmd, UMASS_BBB_CBW_SIZE, 0);

    /* Move to next Command */
    pCbw_cmd ++;    
}

void xhci_msc_read_10(u32 xfer_len, u32 lba, u32 block_len)
{
    /* READ_10 */
    pCbw_cmd->dCBWSignature         = CBWSIGNATURE;
    pCbw_cmd->dCBWTag               = CBWTag ++;
    pCbw_cmd->dCBWDataTransferLength    = xfer_len;   // Total bytes read
    pCbw_cmd->bCBWFlags             = CBWFLAGS_IN;
    pCbw_cmd->bCBWLUN               = 0;        // WDC LUN =0
    pCbw_cmd->bCDBLength            = 10;       // cmd_len = 10

    memset((u8 *)pCbw_cmd->CBWCDB, 0, CBWCDBLENGTH);
    pCbw_cmd->CBWCDB[0]  = SCSI_READ10;
    pCbw_cmd->CBWCDB[5]  = lba & 0xff;          // Disk logical block Addr, order is little-endian
    pCbw_cmd->CBWCDB[4]  = (lba>>8) & 0xff;
    pCbw_cmd->CBWCDB[3]  = (lba>>16) & 0xff;
    pCbw_cmd->CBWCDB[2]  = (lba>>24) & 0xff;
    pCbw_cmd->CBWCDB[8]  = block_len & 0xff;    // Disk block len to transfer
    pCbw_cmd->CBWCDB[7]  = (block_len>>8) & 0xff;

    xhci_queue_bulk_tx((u32)pCbw_cmd, UMASS_BBB_CBW_SIZE, 0);

    /* Move to next Command */
    pCbw_cmd ++;    
}

void xhci_msc_write_10(u32 xfer_len, u32 lba, u32 block_len)
{
    /* READ_10 */
    pCbw_cmd->dCBWSignature         = CBWSIGNATURE;
    pCbw_cmd->dCBWTag               = CBWTag ++;
    pCbw_cmd->dCBWDataTransferLength    = xfer_len;   // Total bytes read
    pCbw_cmd->bCBWFlags             = CBWFLAGS_OUT;
    pCbw_cmd->bCBWLUN               = 0;        // WDC LUN =0
    pCbw_cmd->bCDBLength            = 10;       // cmd_len = 10

    memset((u8 *)pCbw_cmd->CBWCDB, 0, CBWCDBLENGTH);
    pCbw_cmd->CBWCDB[0]  = SCSI_WRITE10;
    pCbw_cmd->CBWCDB[5]  = lba & 0xff;          // Disk logical block Addr
    pCbw_cmd->CBWCDB[4]  = (lba>>8) & 0xff;
    pCbw_cmd->CBWCDB[3]  = (lba>>16) & 0xff;
    pCbw_cmd->CBWCDB[2]  = (lba>>24) & 0xff;
    pCbw_cmd->CBWCDB[8]  = block_len & 0xff;    // Disk block len to transfer
    pCbw_cmd->CBWCDB[7]  = (block_len>>8) & 0xff;

    xhci_queue_bulk_tx((u32)pCbw_cmd, UMASS_BBB_CBW_SIZE, 0);

    /* Move to next Command */
    pCbw_cmd ++;    
}

/*************************************** 
    USB CH9 Enumeration functions,
    Work on Control EP0.
****************************************/
void xhci_queue_ctrl_setup(u32 field_1, u32 field_2, u32 trt)
{
    /* Control Setup Stage, always 8 bytes Immediate data */
    pEp0_ring->buffer_l     = field_1;
    pEp0_ring->buffer_h     = field_2;
    pEp0_ring->transfer_len = 8;
    pEp0_ring->flags        = TRB_TYPE(TRB_SETUP) | TRB_IDT | TRB_TX_TYPE(trt) | 0x1;   // Cycle bit
    pEp0_ring ++;
}

void xhci_queue_ctrl_data(u32 buf_ptr, u32 xfer_len, u32 direction)
{
   /* Contrl Data Stage, DIR=IN or OUT */
    pEp0_ring->buffer_l     = buf_ptr;
    pEp0_ring->buffer_h     = 0;
    pEp0_ring->transfer_len = xfer_len;
    pEp0_ring->flags        = TRB_TYPE(TRB_DATA) | direction | 0x1;
    pEp0_ring ++;
}

void xhci_queue_ctrl_status(u32 direction)
{
   /* Contrl Data Stage, DIR=IN or OUT, IOC - trigger an event */
    pEp0_ring->buffer_l     = 0;
    pEp0_ring->buffer_h     = 0;
    pEp0_ring->transfer_len = 0;
    pEp0_ring->flags        = TRB_TYPE(TRB_STATUS) | direction | TRB_IOC | 0x1;
    pEp0_ring ++;
}

void xhci_ch9_clear_feature(void)
{
    u32 f1, f2;

    /* 2 stage */
    f1 = 0x0102; f2 = 0x80 | ep_in_num;
    xhci_queue_ctrl_setup( f1, f2, 0 );
    xhci_queue_ctrl_status( TRB_DIR_IN );
}

void xhci_ch9_get_descriptor(void)
{
    u32 f1, f2, data_buf;

    /* Get_Descriptor, len = 18 bytes */
    f1 = 0x1000680; f2 = 0x120000;
    xhci_queue_ctrl_setup( f1, f2, TRB_DATA_IN );

    data_buf = XHCI_CH9_DESC;
    xhci_queue_ctrl_data( data_buf, 18, TRB_DIR_IN);

    xhci_queue_ctrl_status( TRB_DIR_OUT );
}

void xhci_ch9_get_configuration(u32 len)
{
    u32 f1, f2, data_buf;

    /* Get_Descriptor, len = 9 or xx bytes */
    f1 = 0x2000680; f2 = (len&0xff)<<16;
    xhci_queue_ctrl_setup( f1, f2, TRB_DATA_IN );

    data_buf = XHCI_CH9_CONFIG;
    xhci_queue_ctrl_data( data_buf, len, TRB_DIR_IN);

    xhci_queue_ctrl_status( TRB_DIR_OUT );
}

void xhci_ch9_set_configuration(void)
{
    u32 f1, f2;

    /* Per Spec 4.3.5, Only Need Setup stage is needed, Config_id=1  */
    f1 = 0x00010900; f2 = 0;
    xhci_queue_ctrl_setup( f1, f2, 0 );

    xhci_queue_ctrl_status( TRB_DIR_IN );
}

void xhci_ch9_get_lun(void)
{
    u32 f1, f2, data_buf;

    /* Get_LUN, len = 1 */
    f1 = 0xfea1; f2 = (0x1)<<16;
    xhci_queue_ctrl_setup( f1, f2, TRB_DATA_IN );

    data_buf = XHCI_CH9_POOL;
    xhci_queue_ctrl_data( data_buf, 1, TRB_DIR_IN);

    xhci_queue_ctrl_status( TRB_DIR_OUT );
}


void xhci_queue_cmd(void)
{
    // Toggle Cycle bit
    pCmd_trb->flags |= 1;

    // Ring Command Queue DoorBell, = 0
    xhci_writel(XHCI_DB_CMD_RING, 0);

    // Move the pointer of Cmd_Ring, and wrap around
    pCmd_trb ++;
    if(pCmd_trb >= pCmd_root + MAX_CMD_RING_SIZE)
        pCmd_trb = pCmd_root;
}

void xhci_queue_enable_slot(void)
{
    /* Create Address Device Cmd */
    pCmd_trb->cmd_trb_l = 0;
    pCmd_trb->cmd_trb_h = 0;
    pCmd_trb->status = 0;
    pCmd_trb->flags = TRB_TYPE(TRB_ENABLE_SLOT);
    xhci_queue_cmd();
}

void xhci_queue_address_device(u32 slot_id)
{
    /* Create Address Device Cmd */
    pCmd_trb->cmd_trb_l = (XHCI_INPUT_CONTEXT);
    pCmd_trb->cmd_trb_h = 0;
    pCmd_trb->status = 0;
    pCmd_trb->flags = TRB_TYPE(TRB_ADDR_DEV) | (slot_id<<24); // SlotId = 1
    xhci_queue_cmd();
}

void xhci_queue_evaluate_context(u32 slot_id)
{
    pCmd_trb->cmd_trb_l = (XHCI_INPUT_CONTEXT);
    pCmd_trb->cmd_trb_h = 0;
    pCmd_trb->status = 0;
    pCmd_trb->flags = TRB_TYPE(TRB_EVAL_CONTEXT) | (slot_id<<24);    
    xhci_queue_cmd();
}

void xhci_queue_configure_endpoint(u32 slot_id)
{
    pCmd_trb->cmd_trb_l = (XHCI_INPUT_CONTEXT);
    pCmd_trb->cmd_trb_h = 0;
    pCmd_trb->status = 0;
    pCmd_trb->flags = TRB_TYPE(TRB_CONFIG_EP) | (slot_id<<24);
    xhci_queue_cmd();
}

void xhci_queue_reset_endpoint(u32 slot_id, u32 ep)
{
    pCmd_trb->cmd_trb_l = 0;
    pCmd_trb->cmd_trb_h = 0;
    pCmd_trb->status = 0;
    /* bit9 TSP =0, No perserve ep state */
    pCmd_trb->flags = TRB_TYPE(TRB_RESET_EP) | (slot_id<<24) | (ep<<16) | (0<<9);
    xhci_queue_cmd();
}

void xhci_queue_tr_dequeue(u32 slot_id, u32 ep, u32 address)
{
    pCmd_trb->cmd_trb_l = address | (1<<0);     // DCS=1
    pCmd_trb->cmd_trb_h = 0;
    pCmd_trb->status = 0;
    /* bit9 TSP =0, No perserve ep state */
    pCmd_trb->flags = TRB_TYPE(TRB_SET_DEQ) | (slot_id<<24) | (ep<<16);
    xhci_queue_cmd();
}

/*************************************** 
    USB Interrupt processing.
    Event coming from interrupt
****************************************/
void xhci_check_slot_state(void)
{
    /* Add a timeout later!! */
    while(1)
    {
        /* break when Slot is addressed */
        if(GET_SLOT_STATE(pOutput_slot_ctx->dev_state) == 0x2)
        {
            printf("\nUSB Slot Addressed\n");
            return;
        }
        udelay(1);
    }
}

int xhci_wait_event_queue( u32 trb_type )
{
    u32 event, event_ring_dequeue_ptr, tmp;
    u32 slot_state, ep_state, i;

    while(1)
    {
        /* Polling event queue, C bit */
        for(i=EVT_TIME_OUT; i>0; i--)
        {
            if(pEvent_trb->flags & 0x1)
                break;
            
            udelay(10);
        }

        if(i==0)
        {
            printf("\nEvent Queue time-out\n");
            return -1;
        }
        
        /* Get an event in the queue */
        event = TRB_FIELD_TO_TYPE(pEvent_trb->flags);
        debug("\nRecv Event #%d ", event);

        // pre-process trb_complete
        if( event == TRB_COMPLETION )
        {
            debug("\nCompleted cmd_trb: prt=%x, status=%d, slot_id=%d, ",
                         pEvent_trb->cmd_trb_l,  GET_COMP_CODE(pEvent_trb->status), TRB_TO_SLOT_ID(pEvent_trb->flags) );
        }
        else if( event == TRB_PORT_STATUS )
        {
            debug("\nPORT_STATUS_Change: port_id=%d, ",
                         GET_PORT_ID(pEvent_trb->cmd_trb_l) );
        }
        else if( event == TRB_TRANSFER )
        {
            debug("\nXfer_Done, EP-ID %d: EP-state=%d, trb_ptr=%x, xfer_status=%d, xfer_len=%d, ",
                        TRB_TO_EP_INDEX(pEvent_trb->flags), (pOutput_ep0_ctx->ep_info)&EP_STATE_MASK, pEvent_trb->cmd_trb_l, 
                        GET_COMP_CODE(pEvent_trb->status), pEvent_trb->status & 0xFFFFFF );
        }
        else
            debug("\nNot Processed Event, Check this!!!!!!!!!!!!!\n");

        /* Monitor Port Status */
        tmp = xhci_readl( USB3_PORTSC );
        debug("Port_SC: Port_state=%d, PP/Power=%d, CCS/Cont=%d, PED/Enabled=%d, PR/Reset=%d, CAS=%d, ",
                         (tmp>>5)&0xf, (tmp>>9)&0x1, tmp&0x1, (tmp>>1)&0x1, (tmp>>4)&0x1, (tmp>>24)&0x1 );

        /* After Port connected, report slot_state */
        if(link_state)
        {
            slot_state = GET_SLOT_STATE(pOutput_slot_ctx->dev_state);
            debug("\nSlot_state=%d, ", slot_state );

            /* Configured Slot */
            if(slot_state == 3)
            {
                ep_state = pOutput_ep_bulk_in_ctx->ep_info & EP_STATE_MASK;
                debug("\nEP-IN<%d>_state=%d, ", ep_in_num, ep_state);
                
                /* Halted EP need clear */
                if(ep_state == EP_STATE_HALTED)
                {
                    ep_halt_state = 1;
                    debug("\nEP Halted, Take care.....\n");
                }
            }
        }

        /* Move Event index, move de-queue pointer */
        pEvent_trb ++;
        event_ring_dequeue_ptr = (u32)pEvent_trb;
        xhci_writel(USB3_RUNTIME + 0x38 + 0, event_ring_dequeue_ptr);

        /* Check trb_type, if match, return; no match, discard, continue. */
        if( event == trb_type )
            return 0; 
           
    }
}


void xhci_reset_halt(void)
{
        u32 ret;

        /* Reset halt endpoint, slot_id=1, DCI=3 */
        debug("\n*** Reset Halted Endpoint : ");
        xhci_queue_reset_endpoint(1, DCI_ep_in);
        xhci_wait_event_queue( TRB_COMPLETION );
        //udelay(100);

        /* Clear Halt */
        debug("\n*** Send Clear_feature : ");
        xhci_ch9_clear_feature();
        xhci_ring_doorbell(EP_0, 0);
        xhci_wait_event_queue( TRB_TRANSFER );
        udelay(100);

        /* Set TR_DEqueue */
        ret = pOutput_ep_bulk_in_ctx->deq_l & 0xfffffff0;
        debug("\n*** Set TR_Dequeue Ptr 0x%x : ", ret);
        xhci_queue_tr_dequeue(1, DCI_ep_in, ret);
        xhci_wait_event_queue( TRB_COMPLETION );

        /* Retry last xfer */
        debug("\n*** Retry last transfer : ");
        xhci_ring_doorbell(ep_in_num, EP_IN);
        xhci_wait_event_queue( TRB_TRANSFER );
        udelay(100);
}


void xhci_queue_msc_test_unit(void)
{
    /* MSC_TEST_UNIT_READY */
    debug("\n\n*** MSC_Test_Unit_Ready : ");
    xhci_msc_test_unit();

    pCsw_status += 0x20;
    xhci_queue_bulk_rx((u32)pCsw_status, UMASS_BBB_CSW_SIZE, TRB_IOC);

    xhci_ring_doorbell(ep_out_num, EP_OUT);
    xhci_ring_doorbell(ep_in_num, EP_IN);
    xhci_wait_event_queue( TRB_TRANSFER );
    udelay(1000);
}

void xhci_queue_msc_read_capacity(void)
{
    /* Read Capacity */
    debug("\n\n*** MSC_Read_Capacity : ");
    xhci_msc_read_capacity();

    pCbw_data   += 0x40;
    pCsw_status += 0x20;
    xhci_queue_bulk_rx((u32)pCbw_data, 8, 0);
    xhci_queue_bulk_rx((u32)pCsw_status, UMASS_BBB_CSW_SIZE, TRB_IOC);

    xhci_ring_doorbell(ep_out_num, EP_OUT);
    xhci_ring_doorbell(ep_in_num, EP_IN);
    xhci_wait_event_queue( TRB_TRANSFER );
    if(ep_halt_state)
    {
        xhci_reset_halt();
        ep_halt_state = 0;
    }
    udelay(1000);
}

void xhci_queue_msc_mode_sense(void)
{
    /* Mode Sense 6 */
    debug("\n\n*** MSC_Mode_Sense_6 : ");
    xhci_msc_mode_sense();

    pCbw_data   += 0x40;
    pCsw_status += 0x20;
    xhci_queue_bulk_rx((u32)pCbw_data, 0xc0, 0);
    xhci_queue_bulk_rx((u32)pCsw_status, UMASS_BBB_CSW_SIZE, TRB_IOC);

    xhci_ring_doorbell(ep_out_num, EP_OUT);
    xhci_ring_doorbell(ep_in_num, EP_IN);
    xhci_wait_event_queue( TRB_TRANSFER );

    if(ep_halt_state)
    {
        xhci_reset_halt();
        ep_halt_state = 0;
    }
    udelay(3000);
}

void xhci_queue_msc_request_sense(void)
{
    /* Request Sense */
    debug("\n\n*** MSC_Request_Sense : ");
    xhci_msc_request_sense();

    pCbw_data   += 0x40;
    pCsw_status += 0x20;
    xhci_queue_bulk_rx((u32)pCbw_data, 0x60, 0);
    xhci_queue_bulk_rx((u32)pCsw_status, UMASS_BBB_CSW_SIZE, TRB_IOC);

    xhci_ring_doorbell(ep_out_num, EP_OUT);
    xhci_ring_doorbell(ep_in_num, EP_IN);
    xhci_wait_event_queue( TRB_TRANSFER );

    if(ep_halt_state)
    {
        xhci_reset_halt();
        ep_halt_state = 0;
    }
    udelay(3000);
}


/*************************************** 
    USB Enumeration flow    
****************************************/
int xhci_hcd_link_setup(void)
{
    u32 ret, port_state;


    /* Port change, from Hard reset */
    //swang !!!!

    xhci_port_warm_reset();

    debug("\n==>Port Reset is done !!!");
    xhci_wait_event_queue( TRB_PORT_STATUS ); 
    /* Query Speed_id */
    ret = xhci_readl( USB3_PORTSC ) & DEV_SPEED_MASK;
    debug("\nPort speed is %d\n", ret>>10 );
    printf("\n\*****************************");
    printf("\n\nPlug in USB 3.0 Cable now...");
    printf("\n\*****************************");

    /* Wait for Port Status Change (Device Attach) */
    /* Check compliance mode recovery quirk */
    do {
        ret = (readl( IPROC_USB3_BASE_ADDR + USB3_PORTSC ) >> 5) & 0xF;
        if(ret == 0xA)
        {
            debug("\nDetected port in <%d> Compliance mode, start warm reset...", ret);
            //xhci_port_warm_reset();
            printf("\nDetected port in <%d> Compliance mode\n", ret);
            debug("Disconnect and reset USB disk power\n");
            return -1;
        }

        udelay(1);        
        ret = readl( IPROC_USB3_BASE_ADDR + USB3_PORTSC ) & PORT_CONNECT;

    } while(!ret);

    /* Wait for Device Attach */
#if swang
    if( xhci_wait_event_queue( TRB_PORT_STATUS ) )
        return -1;        
#endif

    /* Port Reset */
    port_state = xhci_readl( USB3_PORTSC );
    port_state = (port_state>>5) & 0xf;
    if(port_state != 0)
    {
        debug("\nPort State is not in U0 state, state %d\n!!!", port_state);
        //xhci_port_warm_reset();
        //udelay(100);
        //xhci_wait_event_queue( TRB_PORT_STATUS );

    }

    /* Confirm CCS connected */
    ret = xhci_readl( USB3_PORTSC ) & PORT_CONNECT;
    if(ret != 0x1)
    {
        printf("\nBad connection, failed\n");
        return -1;
    }

    /* Query Speed_id */
    ret = xhci_readl( USB3_PORTSC ) & DEV_SPEED_MASK;
    printf("\nDevice connected, speed is %d\n", ret>>10 );
    udelay(1000000);

    printf("\nPort State is in %d state!!!\n", (port_state>>5) & 0xf);
    /* Mark link up */
    link_state = 1;
    return 0; 
}


int xhci_hcd_enum(void)
{
    u32 ret, slot_id, block_cnt;

    /* Wait for good link */
    if(xhci_hcd_link_setup() < 0)
        return -1;

    /* Enable Slot Command */
    printf("\n\n*** Enable_Slot ");
    xhci_queue_enable_slot();
    /* Wait for event, retrieve slot_id (we know = 1, but have to do it) */
    if( xhci_wait_event_queue( TRB_COMPLETION ) )
        return -1;
    udelay(10000);
    slot_id = 1;    

    printf("\nAdd more timeouts!!!!\n");

    udelay(1000);
    //udelay(10000);
    //udelay(10000);
    //udelay(10000);
    //udelay(10000);
    //udelay(10000);

    printf("\ntimeouts are done!!!!\n");
    /* Set Address Command */
    printf("\n\n*** Set_Address ");
    //xhci_ch9_get_descriptor();
    xhci_queue_address_device(slot_id);
    udelay(5000);
    /* Use slot_state as indicator */
    //xhci_check_slot_state();

    if( xhci_wait_event_queue( TRB_COMPLETION ) )
        return -2;
    udelay(5000);


    /* Get_Descriptor, first Xfer_trb */
    /* Ring Device_1 DoorBell, DB_target = EP0 update = 0x1 */
    debug("\n\n*** Get_Descriptor:");
    xhci_ch9_get_descriptor();
    xhci_ring_doorbell(EP_0, 0);
    if( xhci_wait_event_queue( TRB_TRANSFER ) )
        return -1;
    udelay(10000);

    /* Get_Config (9) */
    debug("\n\n*** Get_Configuration:");    
    xhci_ch9_get_configuration(9);
    xhci_ring_doorbell(EP_0, 0);
    if( xhci_wait_event_queue( TRB_TRANSFER ) )
        return -1;
    udelay(100);

    /* Get_Config (44) */
    debug("\n\n*** Get_Configuration:");    
    xhci_ch9_get_configuration(44);
    xhci_ring_doorbell(EP_0, 0);
    if( xhci_wait_event_queue( TRB_TRANSFER ) )
        return -1;
    udelay(1000);

    /* Decode EP assignment */
    debug("Get_config <20>=0x%x, <33>=0x%x, ",
            *(u8 *)(XHCI_CH9_CONFIG + 20), *(u8 *)(XHCI_CH9_CONFIG + 33));
    ret =  *(u8 *)(XHCI_CH9_CONFIG + 20);
    if( (ret>>4) == 0x8) 
    {
        ep_in_num   = ret & 0xf;
        ret  = *(u8 *)(XHCI_CH9_CONFIG + 33);
        ep_out_num  = ret & 0xf;
    }
    else
    {
        ep_out_num  = ret & 0xf;
        ret  = *(u8 *)(XHCI_CH9_CONFIG + 33);
        ep_in_num   = ret & 0xf;
    }
    printf("\n\n---> Get EP number: bulk-in <%d>, bulk-out <%d>, ", 
                ep_in_num, ep_out_num);    

    /* Populate EP-Bulk IN/OUT into Device Context */
    debug("\n\n*** Update Bulk-EPs and evaluate context:");
    xhci_update_input_ctx();
    xhci_queue_evaluate_context(slot_id);
    xhci_wait_event_queue( TRB_COMPLETION );
    udelay(10000);

    /* Set_Config */
    debug("\n\n*** Set_Configuration_1 :");
    xhci_ch9_set_configuration();
    xhci_queue_configure_endpoint(slot_id);
    xhci_wait_event_queue( TRB_COMPLETION );
    udelay(100000);

    /* Bulk-only command on EP0 */
    debug("\n\n*** Get LUN :");
    xhci_ch9_get_lun();
    xhci_ring_doorbell(EP_0, 0);
    xhci_wait_event_queue( TRB_TRANSFER );
    udelay(100);

    /* Take out an extra transfer event */
    xhci_wait_event_queue( TRB_TRANSFER );
    udelay(1000);

    /* Queue first MSC_INQUIRY Command */
    debug("\n\n*** MSC_Inquiry : ");
    xhci_msc_inquiry_cmd();

    xhci_queue_bulk_rx((u32)pCbw_data, 36, 0);
    xhci_queue_bulk_rx((u32)pCsw_status, UMASS_BBB_CSW_SIZE, TRB_IOC);

    xhci_ring_doorbell(ep_out_num, EP_OUT);
    xhci_ring_doorbell(ep_in_num, EP_IN);
    xhci_wait_event_queue( TRB_TRANSFER );
    udelay(1000);
    
    xhci_queue_msc_test_unit();    
    xhci_queue_msc_read_capacity();

    xhci_queue_msc_mode_sense();
    xhci_queue_msc_test_unit();    
    xhci_queue_msc_request_sense();
    xhci_queue_msc_test_unit();    
    xhci_queue_msc_read_capacity();
    xhci_queue_msc_test_unit();  

    block_cnt = 8;
    /* Read_10, Load FAT table */
    debug("\n\n^^^ MSC_Read_10 : ");
    xhci_msc_read_10(MSC_BLK_SIZE*block_cnt, 0, block_cnt);       // Read first 8 block, start from lba=0

    pCbw_data   = (char *)(XHCI_MSC_REPLY_BUF + 0x1000);    // Real data start from second 4K boundary
    pCsw_status += 0x20;
    xhci_queue_bulk_data_read((u32)pCbw_data, MSC_BLK_SIZE*block_cnt, 0);
    xhci_queue_bulk_rx((u32)pCsw_status, UMASS_BBB_CSW_SIZE, TRB_IOC);

    xhci_ring_doorbell(ep_out_num, EP_OUT);
    xhci_ring_doorbell(ep_in_num, EP_IN);
    xhci_wait_event_queue( TRB_TRANSFER );

    if(ep_halt_state)
    {
        xhci_reset_halt();
        ep_halt_state = 0;
    }

    udelay(1000);
    xhci_queue_msc_test_unit();     
    //xhci_queue_msc_request_sense();
    xhci_queue_msc_test_unit();    
 
    printf("\n\nUSB 3.0 enumeration done\n");

    return 0;

ENUM_FAIL :
    return -1;
}

void xhci_write_test(u32 lba, u32 blk_cnt, u32 write_data_buf)
{
    /* Support 16Kbyte (16packets) in one TRB */
    xhci_queue_msc_test_unit();    
    xhci_queue_msc_test_unit();  

    /* Write_10 */
    debug("\n\n^^^ MSC_Write_test : lba=0x%x, blk_cnt=%d\n", lba, blk_cnt);
    xhci_msc_write_10(MSC_BLK_SIZE * blk_cnt, lba, blk_cnt);

    pCbw_data   = (char *)(write_data_buf);
    pCsw_status += 0x20;
    xhci_queue_bulk_data_write((u32)pCbw_data, MSC_BLK_SIZE * blk_cnt, 0);
    xhci_queue_bulk_rx((u32)pCsw_status, UMASS_BBB_CSW_SIZE, TRB_IOC);

    xhci_ring_doorbell(ep_out_num, EP_OUT);
    xhci_ring_doorbell(ep_in_num, EP_IN);
    xhci_wait_event_queue( TRB_TRANSFER );
    udelay(10000);

    //xhci_queue_msc_test_unit();    
}


void xhci_read_test(u32 lba, u32 blk_cnt, u32 read_data_buf)
{
    //xhci_queue_msc_test_unit();    
    //xhci_queue_msc_test_unit();  

    /* Read_10 */
    debug("\n\n^^^ MSC_Read_test : lba=0x%x, blk_cnt=%d\n", lba, blk_cnt);
    xhci_msc_read_10(MSC_BLK_SIZE * blk_cnt, lba, blk_cnt);

    pCbw_data   = (char *)(read_data_buf);
    pCsw_status += 0x20;
    xhci_queue_bulk_data_read((u32)pCbw_data, MSC_BLK_SIZE * blk_cnt, 0);
    xhci_queue_bulk_rx((u32)pCsw_status, UMASS_BBB_CSW_SIZE, TRB_IOC);

    xhci_ring_doorbell(ep_out_num, EP_OUT);
    xhci_ring_doorbell(ep_in_num, EP_IN);
    xhci_wait_event_queue( TRB_TRANSFER );

    if(ep_halt_state)
    {
        xhci_reset_halt();
        ep_halt_state = 0;
    }

    udelay(3000);
    //xhci_queue_msc_test_unit();     
}
