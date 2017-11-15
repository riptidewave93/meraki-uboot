/**
 * AppliedMicro APM862xx SoC Ethernet Driver
 *
 * Copyright (c) 2010 Applied Micro Circuits Corporation.
 * All rights reserved. Keyur Chudgar <kchudgar@apm.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * @file apm_enet.c
 *
 * This file implements APM862xx SoC Ethernet driver
 *
 */

#include <config.h>
#include <common.h>
#include <net.h>
#include <asm/processor.h>
#include <asm/io.h>
#include <asm/mp.h>
#include <malloc.h>
#include <miiphy.h>

#include "apm_enet_access.h"
#include "apm_enet.h"
#include "apm_enet_csr.h"
#include "apm_preclass_data.h"
#include "apm_cle_access.h"
#include <asm/apm_qm_core.h>
#include <asm/apm_qm_reg.h>
#include <asm/apm_ipp_csr.h>
#include <asm/apm_scu.h>
#include <asm/4xx_pcie.h>
#if defined(CONFIG_BOARD_EMAC_COUNT)
#define LAST_EMAC_NUM	board_emac_count()
#else /* CONFIG_BOARD_EMAC_COUNT */
#if defined(CONFIG_HAS_ETH3)
#define LAST_EMAC_NUM	4
#elif defined(CONFIG_HAS_ETH2)
#define LAST_EMAC_NUM	3
#elif defined(CONFIG_HAS_ETH1)
#define LAST_EMAC_NUM	2
#else
#define LAST_EMAC_NUM	1
#endif
#endif /* CONFIG_BOARD_EMAC_COUNT */

/* normal boards start with EMAC0 */
#if !defined(CONFIG_EMAC_NR_START)
#define CONFIG_EMAC_NR_START	0
#endif

#if !defined(CONFIG_NET_MULTI)
struct eth_device *emac0_dev;
#endif

struct apm_data_priv gpriv;
#define APM_ENET_BASE_ADDRESS		CONFIG_SYS_ETH_CSR_BASE
#define APM_ENET_GBL_BASE_ADDRESS	APM_ENET_BASE_ADDRESS + 0x800
#define APM_GLBL_DIAG_OFFSET		0x7000
unsigned char *eth_rx_buf_list;
extern struct mb_qm_qstate queue_states[];
extern u32 pbn_valid[MB_MAX_QUEUES];
extern int is_coherent(void);

extern int emac4xx_athr16_read (char *devname, unsigned char addr,
								unsigned char reg, unsigned short *value);
extern int emac4xx_athr16_write (char *devname, unsigned char addr,
								 unsigned char reg, unsigned short value);
extern int athrs16_phy_write(struct eth_device *dev, u32 phy_addr, u32 reg, u16 data);

u32 athrs17_reg_read(unsigned int reg_addr);
void athrs17_reg_write(unsigned int reg_addr, unsigned int reg_val);
extern void gpio_ds_write_bit(int pin, int val);

#define APM_ENET_DEBUG

#if 0
#define CACHE_FLUSH
#endif

/* Queues to be used by ENET0 and PPC0 */
static struct eth_queue_ids eth_queues[LAST_EMAC_NUM];

int apm_enet_wr32(struct apm_data_priv *priv, u8 block_id,
		u32 reg_offset, u32 value)
{
	u32 command_done;
	u8  indirect = 0;
	u32 count = 0;
	u32 addr_reg_offst, cmd_reg_offst, wr_reg_offst;
	u32 cmd_done_reg_offst;

	switch (block_id) {
	case BLOCK_ETH_MAC:
		addr_reg_offst =
			priv->mac_base_addr_v + MAC_ADDR_REG_OFFSET;
		cmd_reg_offst =
			priv->mac_base_addr_v + MAC_COMMAND_REG_OFFSET;
		wr_reg_offst =
			priv->mac_base_addr_v + MAC_WRITE_REG_OFFSET;
		cmd_done_reg_offst =
			priv->mac_base_addr_v + MAC_COMMAND_DONE_REG_OFFSET;
		indirect = 1;
		ENET_DEBUG_WR("MAC write\n");
		break;
	
	case BLOCK_ETH_STATS:
		addr_reg_offst =
			priv->stats_base_addr_v + STAT_ADDR_REG_OFFSET;
		cmd_reg_offst =
			priv->stats_base_addr_v + STAT_COMMAND_REG_OFFSET;
		wr_reg_offst =
			priv->stats_base_addr_v + STAT_WRITE_REG_OFFSET;
		cmd_done_reg_offst =
			priv->stats_base_addr_v + STAT_COMMAND_DONE_REG_OFFSET;
		indirect = 1;
		ENET_DEBUG_WR("STATS write\n");
		break;

	case BLOCK_ETH_GBL:	   
		addr_reg_offst = priv->eth_gbl_base_addr_v + reg_offset;
		ENET_DEBUG_WR("ENET Global CSR write\n");
		break;
		
	case BLOCK_ETH_FFDIV:
		addr_reg_offst = priv->eth_ffdiv_base_addr_v + reg_offset;
		ENET_DEBUG_WR("ENET FFDIV CSR write\n");
		break;
		
	case BLOCK_ETH_MAC_GBL:
		addr_reg_offst = priv->mac_gbl_base_addr_v + reg_offset;
		ENET_DEBUG_WR("ENET MAC Global CSR write\n");
		break;
		
	case BLOCK_ETH_PTP:
		addr_reg_offst = priv->eth_ptp_base_addr_v + reg_offset;
		ENET_DEBUG_WR("ENET PTP CSR write\n");
		break;
		
	case BLOCK_ETH_UNISEC:
		addr_reg_offst = priv->eth_unisec_base_addr_v + reg_offset;
		ENET_DEBUG_WR("ENET UNISEC CSR write\n");
		break;
		
	case BLOCK_ETH_DIAG:
		addr_reg_offst = priv->eth_diag_base_addr_v + reg_offset;
		ENET_DEBUG_WR("ETH DIAG CSR write\n");
		break;

	case BLOCK_ETH_QMI_SLAVE:
		addr_reg_offst = priv->eth_qmi_base_addr_v + reg_offset;
		ENET_DEBUG_WR("ENET QMI SLAVE CSR write\n");
		break;

	case  BLOCK_ETH_MACIP_IND:
		addr_reg_offst = priv->vpaddr_base + reg_offset;
		ENET_DEBUG_WR("ENET MACIP INDIRECT CSR write\n");
		break;
	
	default:
		printf("Invalid blockid in write reg: %d\n", block_id);
		return -1;
	}

	if (indirect) {
		/* Write the register offset in DCR */
		out_be32((void __iomem *) addr_reg_offst, reg_offset);
		ENET_DEBUG_WR("Indirect addr_reg_offst : 0x%X,"
			      "value(reg_offset) 0x%X\n",
			      addr_reg_offst, reg_offset);

		/* Write the data in the wrData register */
		out_be32((void __iomem *) wr_reg_offst, value);
		ENET_DEBUG_WR("Indirect wr_reg_offst: 0x%X, value 0x%X\n",
			      wr_reg_offst, value);

		/* Invoke write command */
		out_be32((void __iomem *) cmd_reg_offst, WRITE0_WR(1));
		ENET_DEBUG_WR("Indirect cmd_reg_offst: 0x%X,"
			      "value(cmd) 0x%X\n",
			      cmd_reg_offst,  WRITE0_WR(1) );

		/* Check command done */
		while (1) {
			command_done =
				in_be32((void __iomem *) cmd_done_reg_offst);
			ENET_DEBUG_WR("Indirect cmd_done_reg_offst: 0x%X,"
				      "command_done:0x%X \n",
				      cmd_done_reg_offst, command_done);
			
			if (command_done)
				break;
			
			udelay(ACCESS_DELAY_TIMEMS);
			
			if(count++ > MAX_LOOP_POLL_CNT) {
				printf("Write failed for blk: %d\n",block_id);
				return -1;
			}
		}
		/* Reset command reg */
		ENET_DEBUG_RD("Reset command reg[0X%08X] \n ", cmd_reg_offst);
		out_be32((void __iomem *) cmd_reg_offst, 0);
	} 
	else 
	{
		out_be32((void __iomem *) addr_reg_offst, value);
		ENET_DEBUG_WR("Direct write addr: 0x%X,"
			"value 0x%X\n", addr_reg_offst, value);

	}
	return 0;
}

int apm_enet_rd32(struct apm_data_priv *priv, u8 block_id,
		u32 reg_offset, u32 *value)
{
	u8 indirect = 0;
	u32 command_done;
	u32 count = 0;
	u32 addr_reg_offst, cmd_reg_offst, rd_reg_offst;
	u32 cmd_done_reg_offst;
	
	switch (block_id) {
	case BLOCK_ETH_MAC:
		addr_reg_offst =
			priv->mac_base_addr_v + MAC_ADDR_REG_OFFSET;
		cmd_reg_offst =
			priv->mac_base_addr_v + MAC_COMMAND_REG_OFFSET;
		rd_reg_offst =
			priv->mac_base_addr_v + MAC_READ_REG_OFFSET;
		cmd_done_reg_offst =
			priv->mac_base_addr_v + MAC_COMMAND_DONE_REG_OFFSET;
		indirect = 1;
		ENET_DEBUG_RD("MAC read \n");
		break;
		
	case BLOCK_ETH_STATS:
		addr_reg_offst =
			priv->stats_base_addr_v + STAT_ADDR_REG_OFFSET;
		cmd_reg_offst =
			priv->stats_base_addr_v + STAT_COMMAND_REG_OFFSET;
		rd_reg_offst =
			priv->stats_base_addr_v + STAT_READ_REG_OFFSET;
		cmd_done_reg_offst =
			priv->stats_base_addr_v + STAT_COMMAND_DONE_REG_OFFSET;
		indirect = 1;
		ENET_DEBUG_RD("STATS read \n");
		break;
		
	case BLOCK_ETH_GBL:
		addr_reg_offst = priv->eth_gbl_base_addr_v + reg_offset;
		ENET_DEBUG_RD("ENET Global CSR read\n");
		break;
   
	case BLOCK_ETH_FFDIV:
		addr_reg_offst = priv->eth_ffdiv_base_addr_v + reg_offset;
		ENET_DEBUG_RD("ENET FFDIV CSR read\n");
		break;
		
	case BLOCK_ETH_MAC_GBL:
		addr_reg_offst = priv->mac_gbl_base_addr_v + reg_offset;
		ENET_DEBUG_RD("ENET MAC Global CSR read\n");
		break;
		
	case BLOCK_ETH_PTP:
		addr_reg_offst = priv->eth_ptp_base_addr_v + reg_offset;
		ENET_DEBUG_RD("ENET PTP CSR read\n");
		break;
		
	case BLOCK_ETH_UNISEC:
		addr_reg_offst = priv->eth_unisec_base_addr_v + reg_offset;
		ENET_DEBUG_RD("ENET UNISEC CSR read\n");
		break;
		
	case BLOCK_ETH_DIAG:
		addr_reg_offst = priv->eth_diag_base_addr_v + reg_offset;
		ENET_DEBUG_RD("ETH DIAG CSR read\n");
		break;

	case BLOCK_ETH_QMI_SLAVE:
		addr_reg_offst = priv->eth_qmi_base_addr_v + reg_offset;
		ENET_DEBUG_RD("ENET QMI SLAVE CSR read\n");
		break;

	case  BLOCK_ETH_MACIP_IND:
		addr_reg_offst = priv->vpaddr_base + reg_offset;
		ENET_DEBUG_RD("ENET MACIP INDIRECT CSR read\n");
		break;
		
	default:
		printf( "Invalid blockid in read reg: %d\n", block_id);
		return -1;
	}
	
	if (indirect) {
		/* Write the MAC register offset in DCR */
		out_be32((void __iomem *) addr_reg_offst, reg_offset);
		ENET_DEBUG_RD("Indirect addr_reg_offst: 0x%X,"
			      "value(reg_offset) 0x%X\n",
			      addr_reg_offst, reg_offset);
		
		/* Invoke read command */
		out_be32((void __iomem *) cmd_reg_offst, READ0_WR(1));
		ENET_DEBUG_RD("Indirect cmd_reg_offst: 0x%X,"
			      "value(cmd) 0x%X\n",
			      cmd_reg_offst,  READ0_WR(1) );

		/* Poll for command done */
		while (1) {
			command_done =
				in_be32((void __iomem *) cmd_done_reg_offst);
			ENET_DEBUG_WR("Indirect cmd_done_reg_offst: 0x%X,"
				      "command_done:0x%X \n",
				      cmd_done_reg_offst, command_done);
			
			if (command_done)
				break;
			udelay(ACCESS_DELAY_TIMEMS);

			if(count++ > MAX_LOOP_POLL_CNT) {
				printf("Read failed for blk: %d\n",block_id);
				return -1;
			}
		}

		*value = in_be32((void __iomem *) rd_reg_offst);
		ENET_DEBUG_RD("Direct read value 0x%X\n", *value);

		/* Reset command reg */
		ENET_DEBUG_RD("Reset command reg[0X%08X] \n ", cmd_reg_offst);
		out_be32((void __iomem *) cmd_reg_offst, 0);
	} 
	else
	{
		*value = in_be32((void __iomem *) addr_reg_offst);
		ENET_DEBUG_RD("Direct read addr: 0x%X, value: 0x%X\n",
			      addr_reg_offst, *value);
	}
	return 0;
}

#ifdef DEBUG_HEXDUMP
static int putshex(u8 *buf, int len)
{
    int i;
    for (i = 0; i < len; i++)
    {
	if (i == 0) {
	printf("%02X", buf[i]);
	}
	else
	{ 
	    if ((i % 32) == 0)
		printf("\n%02X", buf[i]);
	    else if ((i % 4) == 0)
		printf(" %02X", buf[i]);
	    else 
		printf("%02X", buf[i]);
	}	       
    }
    printf("\n");
    return 0;
}
#endif

#ifdef FAM_UBOOT
u64 PADDR(void *ptr)
{
	u64 paddress = 0x400000000ULL;
	paddress |= (unsigned long) ptr;
	return paddress;
}
#else
u64 PADDR(void *ptr)
{
	u64 paddress = 0x000000000ULL; /* Macro for DDR less than 4 GB */
	paddress |= (unsigned long) ptr;
	return paddress;
}
#endif

static int apm_eth_fp_cfg(u8 port)
{
	int i;
	struct mb_qm_msg_desc fp_msg_desc;
	struct mb_qm_msg16 msg;
	u64 phy;

	
	memset(&msg, 0, sizeof(msg));
	fp_msg_desc.msg = &msg;
	
	/* Common fields */ 
	msg.BufDataLen = (5 << 12);	 /* 2K buffer size */ 
	msg.FPQNum = eth_queues[port].rx_fp_qid;
	
	fp_msg_desc.qid = eth_queues[port].rx_fp_qid;
	fp_msg_desc.mb_id = 0;
	fp_msg_desc.msg = &msg;
	
	for (i = 0; i < NO_RX_BUFF; i++) {
		/* Assign addresses as per configuration */
		phy = PADDR(eth_rx_buf_list + i * ENET_MAX_MTU_ALIGNED);
		msg.DataAddrMSB = (u8) (phy >> 32);
		msg.DataAddrLSB = (u32) phy;
#if defined(CONFIG_NOT_COHERENT_CACHE)
		msg.C = 0;
		if (msg.DataAddrMSB < 4)
			invalidate_dcache_range(msg.DataAddrLSB,
				msg.DataAddrLSB + ENET_MAX_MTU_ALIGNED - 1);
#else
		/* Set coherent bit only if DDR region */ 
		msg.C = msg.DataAddrMSB < 4 ?  1 : 0;			
#endif
		mb_qm_fp_dealloc_buf(&fp_msg_desc);
	}
	return 0;
}

static int apm_eth_init(struct eth_device *dev, bd_t * bis)
{
	struct apm_ptree_config ptree_config;
	struct apm_enet_dev *priv_dev;
	struct apm_data_priv *priv;
	int rc = APM_RC_OK;
	u32 data;
	u32 speed = SPEED_1000;

	/* before doing anything, figure out if we have a MAC address */
	if (memcmp(dev->enetaddr, "\0\0\0\0\0\0", 6) == 0) {
		printf("ERROR: ethaddr not set!\n");
	      return -1;
	}
	priv_dev = dev->priv;
	priv = &priv_dev->priv;
	/* Initialize preclass tree for this port */
	ENET_DEBUG("Initialize Preclass Tree\n");
	apm_preclass_init(priv->port, &eth_queues[priv->port]);

	/* start preclass engine for this port */
	ENET_DEBUG("Initialize Preclass HW Config \n");
	ptree_config.start_node_ptr = SYSTEM_START_NODE;
	ptree_config.start_pkt_ptr = 12;
	ptree_config.default_result = DEFAULT_DBPTR;
	ptree_config.start_engine = 1;
	ptree_config.max_hop = 8;
	apm_set_sys_ptree_config(priv->port, &ptree_config);
#ifndef 	CONFIG_MERAKI
	ENET_DEBUG("Initialize MAC\n" );

#if !defined(CONFIG_ATHRS16_PHY) /* modified to check speed after reset and init phy*/
	speed = apm_get_link_speed(priv);
	ENET_DEBUG("Link Speed is : %d \n", speed);
#endif
#endif
	apm_gmac_full_reset(priv);
#ifndef 	CONFIG_MERAKI
	/* Reset the Phy */
#if defined(CONFIG_PHY_RESET)

	apm_genericmiiphy_read(priv, priv->phy_addr,
			       RTL_BMCR_ADR, &data);
	data |= 0x8000;
	
	apm_genericmiiphy_write(priv, priv->phy_addr,
				RTL_BMCR_ADR, data);
#endif /* defined(CONFIG_PHY_RESET) */

	apm86xxx_read_scu_reg(SCU_SOCDIV4_ADDR, &data);
	if (speed == SPEED_10) {
		data = RGMII0_CLK_FREQ_SEL4_SET(data, 0xC8);
	} else if (speed == SPEED_100) {
		data = RGMII0_CLK_FREQ_SEL4_SET(data, 0x14);
	} else {
		data = RGMII0_CLK_FREQ_SEL4_SET(data, 0x04);
	}
#endif 

#ifdef 	CONFIG_MERAKI
data = RGMII0_CLK_FREQ_SEL4_SET(data, 0x04);
speed = SPEED_1000;
#endif
	//apm86xxx_write_scu_reg(SCU_SOCDIV4_ADDR, data);
	apm_set_link_speed(priv, speed, bis->bi_enetaddr);

	return rc;
}

/* This will be called by application, not running in interrupt context */
static int apm_eth_tx(struct eth_device *dev, volatile void *ptr, int len)
{
	struct enet_frame *ef_ptr;
	struct apm_enet_dev *priv_dev;
	struct apm_data_priv *priv;
	struct mb_qm_msg_desc tx_msg_desc;
	struct mb_qm_msg_desc rx_msg_desc;
	struct mb_qm_msg32 msg;
	struct mb_qm_msg16 *msg16    = &(msg.msg16);
	struct mb_qm_msg_up8 *msg8_1 = &msg.msgup8_1;
	u32 data_len;
	int rc;
	u64 phy;
	int poll;

	memset(&msg, 0, sizeof(msg));
	priv_dev = dev->priv;
	priv = &priv_dev->priv;

	ef_ptr = (struct enet_frame *) ptr;

	/* Copy in our mac address into the frame */
	memcpy(ef_ptr->source_addr, dev->enetaddr, ENET_ADDR_LENGTH);
	
	data_len  = (len & TX_DATA_LEN_MASK);

	/* Prepare the Msg to send out the packet */
	ENET_DEBUG("packet len: %d address: %p\n", data_len, ef_ptr);
	ENET_TXHEXDUMP((u8*)ef_ptr, data_len);
	msg16->BufDataLen = data_len;
	
	phy = PADDR(ef_ptr);
	msg16->DataAddrMSB = (u8) (phy >> 32);
	msg16->DataAddrLSB = (u32) phy;

#if defined(CONFIG_NOT_COHERENT_CACHE)
	msg16->C = 0;
	if (msg16->DataAddrMSB < 4)
		flush_dcache_range(msg16->DataAddrLSB,
			 	msg16->DataAddrLSB + data_len - 1);
#else
	/* Set coherent bit only if DDR region */ 
	msg16->C = msg16->DataAddrMSB < 4 ? 1 : 0;
#endif 	
	/* For Receiving completion message */
	msg8_1->HE = 1;
	msg8_1->H0Enq_Num = eth_queues[priv->port].comp_qid;
	/* Set TYPE_SEL for egress work message */
	msg8_1->H0Info_msb |= (TYPE_SEL_WORK_MSG << 4);
	tx_msg_desc.qid = eth_queues[priv->port].tx_qid;
	tx_msg_desc.msg = &msg;

	ENET_DEBUG("Enqueue QM msg QID %d len %d\n", 
		tx_msg_desc.qid, msg16->BufDataLen);
        ENET_TXHEXDUMP((u8*) &msg, 32);
	
	if ((rc = mb_qm_push_msg(&tx_msg_desc)) != APM_RC_OK) {
		printf("Error in Packet Transmit \n");
		return -1;
	}

	/* Check for completion message to make sure transmit succeeded */
	memset(&msg, 0, sizeof(msg));
	rx_msg_desc.msg = &msg;
	rx_msg_desc.qid = eth_queues[priv->port].comp_qid;
	rx_msg_desc.mb_id = eth_queues[priv->port].comp_mb;
	rx_msg_desc.is_msg16 = 0;
	
	ENET_DEBUG("Checking completion msg QID %d mailbox %d\n",
		rx_msg_desc.qid, rx_msg_desc.mb_id);
	poll = 0;
	while (poll++ < 10) {
		rc = mb_qm_pull_msg(&rx_msg_desc);
		if (rc ==  APM_RC_OK) 
			break;
		/* Wait a while so we receive completion msg */
		udelay(500);
	}
	if (poll >= 10) {
		ENET_DEBUG("Time out completion msg QID %d mailbox %d\n",
		       rx_msg_desc.qid, rx_msg_desc.mb_id);
	}
	return rc;
}

/* Deliver received packets to higher layers */
static int apm_eth_rx(struct eth_device *dev)
{
	int rc;
	struct apm_enet_dev *priv_dev;
	struct apm_data_priv *priv;
	struct mb_qm_msg_desc rx_msg_desc;
	struct mb_qm_msg32 msg;
	struct mb_qm_msg_desc fp_msg_desc;
	struct mb_qm_msg16 fp_msg;
	struct mb_qm_msg16 *msg16 = &(msg.msg16);
	u32 data_len;
    
	priv_dev = dev->priv;
	priv = &priv_dev->priv;

	memset(&msg, 0, sizeof(msg));
	rx_msg_desc.msg = &msg;
	rx_msg_desc.qid = eth_queues[priv->port].rx_qid;
	rx_msg_desc.mb_id = eth_queues[priv->port].rx_mbid;
	rx_msg_desc.is_msg16 = 0;
	
	/* Get the Pkts if any */ 
	rc = mb_qm_pull_msg(&rx_msg_desc);
	if (rc != 0)
		return 0;
	
	if (msg16->LErr && msg16->LErr != 7) {
		/* Parse error */
		apm_enet_parse_error(msg16->LErr);
		printf("ENET Receive Error: LErr[%d] for Qid[%d]\n",
			msg16->LErr,rx_msg_desc.qid);
		if (msg16->DataAddrLSB)
			goto dealloc;
		return -1;
	}
	
	/* Pass the Packet to Higher Layer for processing */
	data_len = msg16->BufDataLen & TX_DATA_LEN_MASK;
	
	ENET_DEBUG("Rcvd Pkt of Len[%d] Passing it to Higher UP	 \n",
 			data_len);
#ifdef DEBUG_HEXDUMP
	printf("Message Dump:: \n");
	putshex((uchar*)&msg,  sizeof(struct mb_qm_msg32));
	
	printf("Packet Dump:: \n");
	putshex((uchar*)msg16->DataAddrLSB, data_len);
#endif
	NetReceive((uchar*)msg16->DataAddrLSB, data_len);
dealloc:	
	/* Dealloc the Buff to FP */
	ENET_DEBUG("Dealloc the Buff to FP \n");
	memset(&fp_msg, 0, sizeof(fp_msg));
	fp_msg_desc.msg = &fp_msg;

	/* Common fields  */
	fp_msg.BufDataLen = (5 << 12);	/* 2K buffer size */
	fp_msg.FPQNum = eth_queues[priv->port].rx_fp_qid;
	fp_msg.DataAddrLSB = msg16->DataAddrLSB;
	fp_msg.DataAddrMSB = msg16->DataAddrMSB;

#if defined(CONFIG_NOT_COHERENT_CACHE)
	fp_msg.C = 0;
	if (msg16->DataAddrMSB < 4)
		invalidate_dcache_range(fp_msg.DataAddrLSB,
				fp_msg.DataAddrLSB + ENET_MAX_MTU_ALIGNED - 1);
#else
	/* Set coherent bit only if DDR region */ 
	fp_msg.C = fp_msg.DataAddrMSB < 4 ? 1 : 0;
#endif
	fp_msg_desc.qid = eth_queues[priv->port].rx_fp_qid;
	fp_msg_desc.msg = &fp_msg;

	rc = mb_qm_fp_dealloc_buf(&fp_msg_desc);

	if (rc != 0) {
		printf("Can not de-allocate buffer to QM\n");
	}
	
	return rc;
}

static void apm_eth_halt(struct eth_device *dev)
{
#ifndef PCM_LOOPBACK
	struct apm_enet_dev *priv_dev = dev->priv;
	struct apm_data_priv *priv = &priv_dev->priv;
	/* Disable MAC */
	apm_gmac_rx_disable(priv);
	apm_gmac_tx_disable(priv);
#endif
	return;
}

#if defined(CONFIG_MII) || defined(CONFIG_CMD_MII)
int apm_eth_miiphy_read(char *devname, unsigned char addr, unsigned char reg,
			unsigned short *value)
{
	struct eth_device *dev;
	struct apm_enet_dev *priv;
	int ret;
	u32 data;

	dev = eth_get_dev_by_name(devname);
	priv = dev->priv;
	ret = apm_genericmiiphy_read((struct apm_data_priv *)&priv->priv, addr, reg, &data);
	if (ret == 0) {
		*value = (unsigned short)(0x0000ffff & data);
	}
	return ret;
}

int apm_eth_miiphy_write(char *devname, unsigned char addr, unsigned char reg,
			 unsigned short value)
{
	struct eth_device *dev;
	struct apm_enet_dev *priv;
	u32 data = (0x0000ffff & (u32)value);
	dev = eth_get_dev_by_name(devname);
	priv = dev->priv;
	return apm_genericmiiphy_write((struct apm_data_priv *)&priv->priv, addr, reg, data);
}
#endif

#ifdef CONFIG_MERAKI
u16 phy_reg_read(u32 dev_id, unsigned char phy_addr, unsigned char phy_reg)
{
	u16	data;	
	char		*devname;  

	devname = miiphy_get_current_dev();    
	apm_eth_miiphy_read  (devname,  phy_addr,phy_reg, &data);	
	return data;
}	

int phy_reg_write(u32 dev_id, unsigned char phy_addr, 	unsigned char phy_reg, 	unsigned short phy_data)
{

	char		*devname;
	devname = miiphy_get_current_dev(); 
	apm_eth_miiphy_write (devname, phy_addr,phy_reg, phy_data);		 
	return 0;
}


u32
athrs17_reg_read(unsigned int reg_addr)
{
	u32 reg_word_addr;
	u32 phy_addr, tmp_val, reg_val;
	u16 phy_val;
	u8 phy_reg;

	/* change reg_addr to 16-bit word address, 32-bit aligned */
	reg_word_addr = (reg_addr & 0xfffffffc) >> 1;

	/* configure register high address */
	phy_addr = 0x18;
	phy_reg = 0x0;
	phy_val = (u16) ((reg_word_addr >> 8) & 0x3ff);  /* bit16-8 of reg address */


	phy_reg_write(0, phy_addr, phy_reg, phy_val);
	//printf("Write PHY 0x%x Reg 0x%x val %04X\n",phy_addr,phy_reg, phy_val);	

	/* For some registers such as MIBs, since it is read/clear, we should */
	/* read the lower 16-bit register then the higher one */

	/* read register in lower address */
	phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
	phy_reg = (u8) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
	reg_val = (u32) phy_reg_read(0, phy_addr, phy_reg);	
	//printf("Read PHY 0x%x Reg 0x%x\n",phy_addr,phy_reg);	

	/* read register in higher address */
	reg_word_addr++;
	phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
	phy_reg = (u8) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
	tmp_val = (u32) phy_reg_read(0, phy_addr, phy_reg);   
	reg_val |= (tmp_val << 16);
	//printf("Read PHY 0x%x Reg 0x%x\n",phy_addr,phy_reg);

	return reg_val;   
}

void
athrs17_reg_write(unsigned int reg_addr, unsigned int reg_val)
{
	u32 reg_word_addr;
	u32 phy_addr;
	u16 phy_val;
	u8 phy_reg;

	/* change reg_addr to 16-bit word address, 32-bit aligned */
	reg_word_addr = (reg_addr & 0xfffffffc) >> 1;

	/* configure register high address */
	phy_addr = 0x18;
	phy_reg = 0x0;
	phy_val = (u16) ((reg_word_addr >> 8) & 0x3ff);  /* bit16-8 of reg address */
	phy_reg_write(0, phy_addr, phy_reg, phy_val);

	/* For S17 registers such as ARL and VLAN, since they include BUSY bit */
	/* in higher address, we should write the lower 16-bit register then the */
	/* higher one */

	/* write register in lower address */
	phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
	phy_reg = (u8) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
	phy_val = (u16) (reg_val & 0xffff);
	phy_reg_write(0, phy_addr, phy_reg, phy_val); 

	/* read register in higher address */
	reg_word_addr++;
	phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
	phy_reg = (u8) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
	phy_val = (u16) ((reg_val >> 16) & 0xffff);
	phy_reg_write(0, phy_addr, phy_reg, phy_val);
}
#endif

static int apm_enet_qconfig(u8 port)
{
	static struct mb_qm_qstate *enet_qstates = NULL;
	struct mb_qm_qstate *cur_qstate = NULL;
	struct mb_qm_qstate *tmp_qstate = NULL;
	int qcnt = 4; 
	struct apm_qm_qalloc eth_qalloc = {0};
	int err = 0;
	
	/* allocate queue state memory if not done so already */
	if (enet_qstates == NULL) {
		enet_qstates = malloc(qcnt * sizeof(struct mb_qm_qstate));
		if (enet_qstates == NULL) {
			printf("Couldn't allocate memory for Enet"
			       "queues state\n");
			return -1;
		}
		memset(enet_qstates, 0, qcnt * sizeof(struct mb_qm_qstate));
	}
	cur_qstate = enet_qstates;
	
	/* Allocate egress work queue from PPC0 to ETH0 */
	eth_qalloc.qstates = cur_qstate;
	eth_qalloc.qm_ip_blk = IP_BLK_QM;
	if (port == 0)
		eth_qalloc.ip_blk = IP_ETH0;
	else
		eth_qalloc.ip_blk = IP_ETH1;
	eth_qalloc.ppc_id = 0;
	eth_qalloc.q_type = P_QUEUE;
	eth_qalloc.q_count = 1;
	eth_qalloc.direction = DIR_EGRESS;
	eth_qalloc.qsize = SIZE_16KB;
	eth_qalloc.thr_set = 1;
	
	if ((err = apm_qm_alloc_q(&eth_qalloc)) != 0) {
		printf("Error allocating two egress work queues for ETH0\n");
		return -1;
	}

	eth_queues[port].tx_qid = cur_qstate->q_id;
	cur_qstate++;
	
	/* Allocate one ingress queue for Eth0 to PPC0 normal work queue */
	eth_qalloc.qstates = cur_qstate;
	eth_qalloc.direction = DIR_INGRESS;
	
	if ((err = apm_qm_alloc_q(&eth_qalloc)) != 0) {
		printf("Error allocating three ingress queues for ETH to PPC0\n");
		return -1;
	}
	eth_queues[port].rx_qid = cur_qstate->q_id;
	eth_queues[port].rx_mbid = cur_qstate->mb_id;
	cur_qstate++;
       
	/* Get completion queue info for Eth0 to PPC0 */
	if (port == 0)
		tmp_qstate = get_comp_queue(IP_ETH0, 0);
	else
		tmp_qstate = get_comp_queue(IP_ETH1, 0);
	memcpy(cur_qstate, tmp_qstate, sizeof (struct mb_qm_qstate));
	
	eth_queues[port].comp_qid = cur_qstate->q_id;
	eth_queues[port].comp_mb = cur_qstate->mb_id;
	cur_qstate++;

	/* Allocate free pool for ETH0 from PPC0 */
	eth_qalloc.qstates = cur_qstate;
	eth_qalloc.q_type = FREE_POOL;
	eth_qalloc.direction = DIR_EGRESS;
	
	if ((err = apm_qm_alloc_q(&eth_qalloc)) != 0) {
		printf("Error allocating free pool for PPC0\n");
		return -1;
	}
	eth_queues[port].rx_fp_qid = cur_qstate->q_id;
	eth_queues[port].rx_fp_pbn = cur_qstate->pbn;
	eth_queues[port].rx_fp_mbid = cur_qstate->mb_id;
	cur_qstate++;

	ENET_DEBUG("RX QID %d RX MB %d TX QID %d CQID %d CMB %d FP QID %d"
		" FP PBD %d\n", eth_queues[port].rx_qid, eth_queues[port].rx_mbid,
		eth_queues[port].tx_qid, eth_queues[port].comp_qid, eth_queues[port].comp_mb,
		eth_queues[port].rx_fp_qid, eth_queues[port].rx_fp_pbn);
	return err;
	
}

void apm_eth_qm_initialize (void) {
	int i;
	struct q_raw_state raw_qstate;
	struct mb_qm_msg_desc rx_msg_desc;
	struct mb_qm_msg32 msg;
	
	if (CPUID != CONFIG_SYS_MASTER_CPUID)
		return;

	if (is_coherent())
		return;
	
	/* Disable MAC */
	apm_gmac_rx_disable(&gpriv);
	apm_gmac_tx_disable(&gpriv);
	
	memset(&msg, 0, sizeof(msg));
	rx_msg_desc.msg = &msg;
	rx_msg_desc.qid = 0;
	rx_msg_desc.is_msg16 = 0;
	
	for (i = CORE0_MB_START; i < CORE0_NO_MB; i++) {
		rx_msg_desc.mb_id = i;
		while(mb_qm_pull_msg(&rx_msg_desc) == 0) {
			ENET_DEBUG("Dequeue messsage from mb:%d \n", i);
		}
	}
	
	memset(&raw_qstate, 0, sizeof(struct q_raw_state));
	ENET_DEBUG("Initialize QM Queues \n");
	for (i = CORE0_Q_START; i < CORE0_NO_Q; i++) {
		mb_qm_raw_qstate_wr(IP_BLK_QM, i, &raw_qstate);
	}
	ENET_DEBUG("Initialize ENET WQ PBN \n");
	for (i = CORE0_ENET_WQ_PBN_START; i < CORE0_NO_ENET_WQ_PBN; i++) {
		mb_qm_pb_set(IP_BLK_QM, PB_SLAVE_ID_ETH, i, 0);
	}
	
	ENET_DEBUG("Initialize and Flush ENET FP PBN \n");
	for (i = CORE0_ENET_FP_PBN_START;
	 	i < (CORE0_ENET_FP_PBN_START + CORE0_NO_ENET_FP_PBN); i++) {
		mb_qm_pb_set(IP_BLK_QM, PB_SLAVE_ID_ETH, i, 0);
		apm_enet_fp_pb_flush(&gpriv, i - 0x20);
	}
}

int apm_eth_reset(int eth_num)
{
	struct apm86xxx_reset_ctrl reset_ctrl;
	int rc;

	switch (eth_num) {
	default:		/* fall through */
	case 0:
		ENET_DEBUG("reset Eth0 \n");
		/* reset Eth0 */
		memset(&reset_ctrl, 0, sizeof(reset_ctrl));
		reset_ctrl.reg_group = REG_GROUP_SRST_CLKEN;
		reset_ctrl.clken_mask = ENET0_F1_MASK;
		reset_ctrl.csr_reset_mask = ENET0_F3_MASK;
		reset_ctrl.reset_mask = ENET0_MASK;
		reset_ctrl.ram_shutdown_addr = (u32 __iomem *)
			(APM_ENET_BASE_ADDRESS + APM_GLBL_DIAG_OFFSET +
			ENET_CFG_MEM_RAM_SHUTDOWN_ADDR);
		reset_ctrl.mem_rdy_mask = ENET0_F2_MASK;
		rc = reset_apm86xxx_block(&reset_ctrl);
		if (rc) goto  exit;

		ENET_DEBUG("reset CLE0 \n");
		/* reset CLE0 */
		memset(&reset_ctrl, 0, sizeof(reset_ctrl));
		reset_ctrl.reg_group = REG_GROUP_SRST1_CLKEN1;
		reset_ctrl.clken_mask = CLE0_IL1_MASK;
		reset_ctrl.csr_reset_mask = CLE1_MASK;
		reset_ctrl.reset_mask =	 CLE01_MASK;
		reset_ctrl.ram_shutdown_addr = (u32 __iomem *)
			(CLE_GBL_BASE_ADDR + APM_GLBL_DIAG_OFFSET +
			CLE_CFG_MEM_RAM_SHUTDOWN_ADDR);
		reset_ctrl.mem_rdy_mask = CLE_MASK;
		rc = reset_apm86xxx_block(&reset_ctrl);
		if (rc) goto  exit;
		break;
	case 1:
		ENET_DEBUG("reset Eth1 \n");
		/* reset Eth1 */
		memset(&reset_ctrl, 0, sizeof(reset_ctrl));
		reset_ctrl.reg_group = REG_GROUP_SRST_CLKEN;
		reset_ctrl.clken_mask = ENET1_F1_MASK;
		reset_ctrl.csr_reset_mask = ENET1_F2_MASK;
		reset_ctrl.reset_mask = ENET1_MASK;
		reset_ctrl.ram_shutdown_addr = (u32 __iomem *)
			(APM_ENET_BASE_ADDRESS + APM_GLBL_DIAG_OFFSET +
			ENET_CFG_MEM_RAM_SHUTDOWN_ADDR);
		reset_ctrl.mem_rdy_mask = ENET0_F2_MASK;
		rc = reset_apm86xxx_block(&reset_ctrl);
		if (rc) goto  exit;

		ENET_DEBUG("reset CLE1 \n");
		/* reset CLE1 */
		memset(&reset_ctrl, 0, sizeof(reset_ctrl));
		reset_ctrl.reg_group = REG_GROUP_SRST1_CLKEN1;
		reset_ctrl.clken_mask = CLE1_IL1_MASK;
		reset_ctrl.csr_reset_mask = CLE1_MASK;
		reset_ctrl.reset_mask =	 CLE11_MASK;
		reset_ctrl.ram_shutdown_addr = (u32 __iomem *)
			(CLE_GBL_BASE_ADDR + APM_GLBL_DIAG_OFFSET +
			CLE_CFG_MEM_RAM_SHUTDOWN_ADDR);
		reset_ctrl.mem_rdy_mask = CLE_MASK;
		rc = reset_apm86xxx_block(&reset_ctrl);
		if (rc) goto  exit;
		break;
	}
exit:
	if(rc) {
		printf("%s: dev[%d] fail !!!\n",__FUNCTION__,eth_num);
	}
	return rc;
}

int apm_eth_initialize(bd_t *bis)
{
	struct eth_device *dev;
	struct apm_enet_dev *priv_dev;
	struct apm_data_priv *priv;
	int eth_num = 0;
#if 0
	struct apm_ptree_config ptree_config;
#endif
	int rc;
	struct apm86xxx_reset_ctrl reset_ctrl;
	struct apm86xxx_pll_ctrl eth_pll;
	int init_done = 0;
	u8 ethaddr[4 + CONFIG_EMAC_NR_START][6];
	
	if (CPUID != CONFIG_SYS_MASTER_CPUID)
		return 0;

#ifdef CONFIG_MP
	/* For AMP Systems, we are partitioning cle resoures */
	if (getenv("AMP") != NULL) {
		apm_cle_system_id = mfspr(SPRN_PIR);
		apm_cle_systems = MAX_SYSTEMS;
	} else {
		apm_cle_system_id = CORE_0;
		apm_cle_systems = 1;
	}
#else
	apm_cle_system_id = CORE_0;
	apm_cle_systems = 1;
#endif

#if 0
	u32 reg_data;
	printf("Before Enable Eth PLL\n");
	reg_data = in_be32((volatile u32 *)(CONFIG_SYS_SCU_BASE + SCU_SOCPLL4_ADDR));
	printf("SCU_SOCPLL4_ADDR 0x%x = 0x%x\n",CONFIG_SYS_SCU_BASE + SCU_SOCPLL4_ADDR, reg_data); 
	reg_data = in_be32((volatile u32 *)(CONFIG_SYS_SCU_BASE + SCU_SOCDIV3_ADDR));
	printf("SCU_SOCDIV3_ADDR 0x%x = 0x%x\n",CONFIG_SYS_SCU_BASE + SCU_SOCDIV3_ADDR, reg_data); 
	reg_data = in_be32((volatile u32 *)(CONFIG_SYS_SCU_BASE + SCU_SOCDIV4_ADDR));
	printf("SCU_SOCDIV4_ADDR 0x%x = 0x%x\n",CONFIG_SYS_SCU_BASE + SCU_SOCDIV4_ADDR, reg_data); 
#endif

	ENET_DEBUG("Enable Ethernet PLL \n"); 
	/* Enable Ethernet PLL */
	eth_pll.clkf_val = 0x13;
	eth_pll.clkod_val = 0;
	eth_pll.clkr_val = 0;
	eth_pll.bwadj_val = 0x13;

	if (!(enable_eth_pll(&eth_pll))) {
 		apm86xxx_write_scu_reg(SCU_SOCDIV4_ADDR, 0x00040004);
	} else {
		if (!is_coherent())
			init_done = 1;
	}
	mdelay(100);

#if 0
	printf("After Enable Eth PLL\n");
	reg_data = in_be32((volatile u32 *)(CONFIG_SYS_SCU_BASE + SCU_SOCPLL4_ADDR));
	printf("SCU_SOCPLL4_ADDR 0x%x = 0x%x\n",CONFIG_SYS_SCU_BASE + SCU_SOCPLL4_ADDR, reg_data); 
	reg_data = in_be32((volatile u32 *)(CONFIG_SYS_SCU_BASE + SCU_SOCDIV3_ADDR));
	printf("SCU_SOCDIV3_ADDR 0x%x = 0x%x\n",CONFIG_SYS_SCU_BASE + SCU_SOCDIV3_ADDR, reg_data); 
	reg_data = in_be32((volatile u32 *)(CONFIG_SYS_SCU_BASE + SCU_SOCDIV4_ADDR));
	printf("SCU_SOCDIV4_ADDR 0x%x = 0x%x\n",CONFIG_SYS_SCU_BASE + SCU_SOCDIV4_ADDR, reg_data); 
#endif

	if (!init_done) {
		ENET_DEBUG("reset QM \n");
		/* reset QM */
		memset(&reset_ctrl, 0, sizeof(reset_ctrl));
		reset_ctrl.reg_group = REG_GROUP_SRST_CLKEN;
		reset_ctrl.clken_mask = QMTM_F1_MASK;
		reset_ctrl.csr_reset_mask = QMTM_F3_MASK;
		reset_ctrl.reset_mask = QMTM_MASK;
		reset_ctrl.ram_shutdown_addr = (u32 __iomem *)
		      	(CONFIG_SYS_QM_CSR_BASE + APM_GLBL_DIAG_OFFSET +
		      	QM_CFG_MEM_RAM_SHUTDOWN_ADDR);
		reset_ctrl.mem_rdy_mask = QMTM_F2_MASK;
		rc = reset_apm86xxx_block(&reset_ctrl);
		if (rc) return rc;
		/* APM Ethernet Reset */
		for (eth_num = 0; eth_num < LAST_EMAC_NUM; eth_num++) {
			if(apm_eth_reset(eth_num) != 0) {
				ENET_DEBUG("Reset eth[%d] fail!!!\n", eth_num);
				return 1;
			}
			mdelay(100);
		}
		for (eth_num = 0; eth_num < LAST_EMAC_NUM; eth_num++)
			memcpy(ethaddr[eth_num], "\0\0\0\0\0\0", 6);

		for (eth_num = 0; eth_num < LAST_EMAC_NUM; eth_num++) {
			switch (eth_num) {
			default:		/* fall through */
			case 0:
				memcpy(ethaddr[eth_num + CONFIG_EMAC_NR_START],
					bis->bi_enetaddr, 6);
				/* set phy num and mode */
				bis->bi_phynum[eth_num] = CONFIG_PHY_ADDR;
				bis->bi_phymode[eth_num] = 0;
				break;
#ifdef CONFIG_HAS_ETH1
		case 1:
			memcpy(ethaddr[eth_num + CONFIG_EMAC_NR_START],
				bis->bi_enet1addr, 6);
			bis->bi_phynum[eth_num] = CONFIG_PHY1_ADDR;
			bis->bi_phymode[eth_num] = 0;
			break;
#endif
#ifdef CONFIG_HAS_ETH2
		case 2:
			memcpy(ethaddr[eth_num + CONFIG_EMAC_NR_START],
				bis->bi_enet2addr, 6);
			bis->bi_phynum[eth_num] = CONFIG_PHY2_ADDR;
			bis->bi_phymode[eth_num] = 0;
			break;
#endif
#ifdef CONFIG_HAS_ETH3
		case 3:
			memcpy(ethaddr[eth_num + CONFIG_EMAC_NR_START],
				bis->bi_enet3addr, 6);
			bis->bi_phynum[eth_num] = CONFIG_PHY3_ADDR;
			bis->bi_phymode[eth_num] = 0;
			break;
#endif
			}
		}
	}
	for (eth_num = 0; eth_num < LAST_EMAC_NUM; eth_num++) {
		/* Allocate device structure */
		dev = (struct eth_device *) malloc(sizeof(struct eth_device));
		if (dev == NULL) {
			printf("apm_eth_initialize: "
				   "Cannot allocate eth_device %d \n", eth_num);
			return -1;
		}
		ENET_DEBUG("Allocated Eth Device: %p\n", dev);
		memset(dev, 0, sizeof(*dev));

		priv_dev = (struct apm_enet_dev *) malloc(sizeof(struct apm_enet_dev));
		if (priv_dev == NULL) {
			free(dev);
			printf("apm_eth_initialize: Cannot allocate priv_dev %d \n",
				   eth_num);
			return -1;
		}
		ENET_DEBUG("Allocated Private Device: %p\n", priv_dev);
		memset(priv_dev, 0, sizeof(struct apm_enet_dev));

		priv = &priv_dev->priv;
	
		/* before doing anything, figure out if we have a MAC address */
		if (memcmp(ethaddr[eth_num], "\0\0\0\0\0\0", 6) == 0) {
			printf("ethaddr[%d]: ethaddr not set!\n",eth_num);
			return -1;
		}
		/* Setup the Ethernet base address and mac address */
		memcpy(dev->enetaddr, ethaddr[eth_num], 6);
	
		ENET_DEBUG("Initialize base addresses \n");
		/* Initialize base addresses for Per Port Indirect access */
		priv->mac_base_addr_v =
			APM_ENET_BASE_ADDRESS + BLOCK_ETH_MAC_OFFSET + BLOCK_ETH_MAC_NUM(eth_num);
		priv->stats_base_addr_v =
			APM_ENET_BASE_ADDRESS + BLOCK_ETH_STATS_OFFSET + BLOCK_ETH_MAC_NUM(eth_num);

		/* Initialize base addresses for Global direct access */
		priv->eth_gbl_base_addr_v =
			APM_ENET_GBL_BASE_ADDRESS + BLOCK_ETH_GBL_OFFSET;
		priv->eth_ffdiv_base_addr_v =
			APM_ENET_GBL_BASE_ADDRESS + BLOCK_ETH_FFDIV_OFFSET;
		priv->mac_gbl_base_addr_v  =
			APM_ENET_GBL_BASE_ADDRESS + BLOCK_ETH_MAC_GBL_OFFSET;
		priv->eth_ptp_base_addr_v =
			APM_ENET_GBL_BASE_ADDRESS + BLOCK_ETH_PTP_OFFSET  ;
		priv->eth_unisec_base_addr_v =
			APM_ENET_GBL_BASE_ADDRESS + BLOCK_ETH_UNISEC_OFFSET;
		priv->eth_diag_base_addr_v =
			APM_ENET_GBL_BASE_ADDRESS + BLOCK_ETH_DIAG_OFFSET;
		priv->eth_qmi_base_addr_v =
			APM_ENET_GBL_BASE_ADDRESS + BLOCK_ETH_QMI_SLAVE_OFFSET ;

		priv->enet_write32 = apm_enet_wr32;
		priv->enet_read32 = apm_enet_rd32;

		ENET_DEBUG("MAC base addresses: 0x%08X\n", priv->mac_base_addr_v);
		ENET_DEBUG("Stat base addresses: 0x%08X\n",priv->stats_base_addr_v);
		ENET_DEBUG("Enet Gbl  base addresses: 0x%08X\n", priv->eth_gbl_base_addr_v);
		priv->phy_addr = bis->bi_phynum[eth_num];
		priv->port = eth_num;
		sprintf(dev->name, "eth%d", eth_num);
		
		/* Initialize device function pointers */
		dev->priv = (void *) priv_dev;
		dev->init = apm_eth_init;
		dev->halt = apm_eth_halt;
		dev->send = apm_eth_tx;
		dev->recv = apm_eth_rx;
		dev->next = NULL;

#if defined(CONFIG_NET_MULTI)
		eth_register(dev);
#else
	emac0_dev = dev;
#endif
		apm_gmac_access_phy(priv);
	
#if defined(CONFIG_MII) || defined(CONFIG_CMD_MII)
        miiphy_register(dev->name, apm_eth_miiphy_read, apm_eth_miiphy_write);
#endif

		/* TBD - Maintaing a global Copy */
		memcpy(&gpriv, priv, sizeof (struct apm_data_priv));

		/* Initialize QM */
		mb_qm_alloc_mbox_ocm();

		if (init_done) {
			apm_eth_qm_initialize();
		}

		mb_qm_init(eth_num, NO_OF_QUEUES);
		/* QM queues configuration */
		if ((rc = apm_enet_qconfig(eth_num)) != 0) {
			printf("Error in QM configuration\n");
			return rc;
		}

		/* Configure FP Queue in the QM */
		ENET_DEBUG("Allocate memory for buffers \n" );
		eth_rx_buf_list = malloc(16 + (NO_RX_BUFF * ENET_MAX_MTU_ALIGNED));
		eth_rx_buf_list = (void *)(((unsigned long)
						eth_rx_buf_list + 15) & ~0xf);
		if (eth_rx_buf_list == NULL) {
			free(priv_dev);
			free(dev);
			printf("Cannot allocate eth_rx_buf_list \n");
			return -1;
		}
		memset(eth_rx_buf_list, 0, NO_RX_BUFF * ENET_MAX_MTU_ALIGNED);

		ENET_DEBUG("Configure free pool \n" );
		if ((rc = apm_eth_fp_cfg(eth_num)) != APM_RC_OK)
		{
			ENET_DEBUG_ERR("U-boot Ethernet Free Pool Config Error \n");
			return rc;
		}

#if 0
		/* Initialize preclass tree for this port */
		ENET_DEBUG("Initialize Preclass Tree\n");
		apm_preclass_init(eth_num, &eth_queues[eth_num]);

		/* start preclass engine for this port */
		ENET_DEBUG("Initialize Preclass HW Config \n");
		ptree_config.start_node_ptr = SYSTEM_START_NODE;
		ptree_config.start_pkt_ptr = 12;
		ptree_config.default_result = DEFAULT_DBPTR;
		ptree_config.start_engine = 1;
		ptree_config.max_hop = 8;
		apm_set_sys_ptree_config(eth_num, &ptree_config);
#endif
	
#ifdef PCM_LOOPBACK /* FIXME */
		apm_eth_init(dev, bis);
#else
		apm_eth_init(dev, bis);
		apm_eth_halt(dev);
#endif
#ifdef CONFIG_MERAKI
		static int ar_init_once;
		if(ar_init_once == 0) {
			int i;
			printf("Init Atheros ...\n");
			//printf("Init Atheros\n");
			athrs17_reg_write(0x0,0x80000000); //switch software reset
			athrs17_reg_write(0x10,0x40000000);
			athrs17_reg_write(0x624,0x007f7f7f);
			athrs17_reg_write(0x4,0x07A00000); //0x07600000
			athrs17_reg_write(0xc,0x05600000); /* Mac6_rgmii_txclk_delay_en set 0 to disable TX clok delay */
			athrs17_reg_write(0x7c, 0x7e);
			athrs17_reg_write(0x94, 0x7e);

			//set default led behavior
			athrs17_reg_write(0x54, 0xcb35cb35);
			athrs17_reg_write(0x58, 0xcf00cf00);

#define PHY_POWER_DOWN 0x800
#define PHY_RESET      0x8000

#define CHOOSE_COPPER 0x8000

			for (i = 0; i < 7; i++) {
				uint16_t val;
				int j;

				//if we're phy 5 or 6, we need to disable both copper and fiber
				for (j = 0; j < (i > 4 ? 2 : 1); j++) {
					if (i > 4) {
						athrs16_phy_read(dev, i, 0x1f, &val);
						/*
						 * it is important that copper is the last one disabled
						 * as the last one disabled must be the first one enabled
						 * which happens below.
						 */
						val = (j == 0) ? (val & ~CHOOSE_COPPER) : (val | CHOOSE_COPPER);
						athrs16_phy_write(dev, i, 0x1f, val);
					}

					athrs16_phy_read(dev, i, 0x0, &val);
					if (!(val & PHY_POWER_DOWN) && i != 1 && i != 4) { //don't power down the internet or crosstalk ports
						val |= PHY_POWER_DOWN;
						athrs16_phy_write(dev, i, 0x0, val);
					}
				}
			}

			for (i = 5; i <= 6; i++) {
				uint16_t val;

				//now enabled the copper on the combo phys so the phy registers aren't disabled later
				athrs16_phy_read(dev, i, 0x0, &val);
				athrs16_phy_write(dev, i, 0x0, val & ~(PHY_POWER_DOWN));

				/*
				 * and set the default led blink rate to 168ms off, then 168ms on
				 * to be close to the other ports' blink rates.
				 */
				athrs16_phy_write(dev, i, 0x18, 0x5300);
			}

#if 1	/* Hw not rework for U28 */
			gpio_ds_write_bit(14, 0); /* swicth MDC clock to U28 */
			udelay(10000);
			athrs17_reg_write(0x0,0x80000000); //switch software reset
			athrs17_reg_write(0x10,0x40000000);
			athrs17_reg_write(0x624,0x007f7f7f);
			athrs17_reg_write(0x4,0x07A00000); //0x07600000
			athrs17_reg_write(0xc,0x05600000); /* Mac6_rgmii_txclk_delay_en set 0 to disable TX clok delay */
			athrs17_reg_write(0x7c, 0x7e);

			athrs17_reg_write(0x7c, 0x27e);
			athrs17_reg_write(0x80, 0x27e);
			athrs17_reg_write(0x84, 0x27e);
			athrs17_reg_write(0x88, 0x27e);
			athrs17_reg_write(0x8c, 0x27e);
			athrs17_reg_write(0x94, 0x27e);

			//set default led behavior
			athrs17_reg_write(0x54, 0xcb35cb35);
			athrs17_reg_write(0x58, 0xcf00cf00);

			//start at one so we don't disable cross talk port
			for (i = 1; i < 5; i++) {
				uint16_t val;
				athrs16_phy_read(dev, i, 0x0, &val);
				if (!(val & PHY_POWER_DOWN)) {
					val |= PHY_POWER_DOWN;
					athrs16_phy_write(dev, i, 0x0, val);
				}
			}

			gpio_ds_write_bit(14, 1); /* swicth MDC clock to U26 */
#endif

#if defined (CONFIG_ETH_PHY_TX_RX_DELAY)
			/* Debug register offset 0x0[15]=1 (RGMII RX clock delay enable) */
			//apm_eth_miiphy_write(dev->name->name,4,0x001D/*ATHR_DBG_PORT_ADDR*/,0x00);
			//apm_eth_miiphy_write(dev->name,4,0x001E/*ATHR_DBG_PORT_DATA*/,0x82ee);

			/* Debug register offset 0x5[8]=1 (RGMII TX clock delay enable) */
			//apm_eth_miiphy_write(dev->name,4,0x001D/*ATHR_DBG_PORT_ADDR*/,0x05);
			//apm_eth_miiphy_write(dev->name,4,0x001E/*ATHR_DBG_PORT_DATA*/,0x3d46);
			/* End of Configure PHY4 RGMII mode */

			/* TX_RX Delay is enabled on AR8033, not on AR8327 , AR8033 SMI address is 0x05 */
			/* Debug register offset 0x12[3]=1 (Select RGMII interface with MAC) */
			athrs16_phy_write(dev,5,0x1d,0x12);
			athrs16_phy_write(dev,5,0x1e,0x4c0c);
		
			/* Debug register offset 0x0[15]=1 (RGMII RX clock delay enable) */
			apm_eth_miiphy_write(dev->name,5,0x001D/*ATHR_DBG_PORT_ADDR*/,0x00);
			apm_eth_miiphy_write(dev->name,5,0x001E/*ATHR_DBG_PORT_DATA*/,0x82ee);
	
			/* Debug register offset 0x5[8]=1 (RGMII TX clock delay enable) */
			apm_eth_miiphy_write(dev->name,5,0x001D/*ATHR_DBG_PORT_ADDR*/,0x05);
			apm_eth_miiphy_write(dev->name,5,0x001E/*ATHR_DBG_PORT_DATA*/,0x3d46);
			/* End of Configure PHY4 RGMII mode */
			athrs16_phy_write(dev,5,0x1d,0xb);
			athrs16_phy_write(dev,5,0x1e,0xbc00);

			/* AR8033(u31) RGMII setting */
			athrs16_phy_write(dev,6,0x1d,0x12);
			athrs16_phy_write(dev,6,0x1e,0x4c0c);

			/* Debug register offset 0x0[15]=1 (RGMII RX clock delay enable) */
			apm_eth_miiphy_write(dev->name,6,0x001D/*ATHR_DBG_PORT_ADDR*/,0x00);
			apm_eth_miiphy_write(dev->name,6,0x001E/*ATHR_DBG_PORT_DATA*/,0x82ee);
	
			/* Debug register offset 0x5[8]=1 (RGMII TX clock delay enable) */
			apm_eth_miiphy_write(dev->name,6,0x001D/*ATHR_DBG_PORT_ADDR*/,0x05);
			apm_eth_miiphy_write(dev->name,6,0x001E/*ATHR_DBG_PORT_DATA*/,0x3d46);
			/* End of Configure PHY4 RGMII mode */
			athrs16_phy_write(dev,6,0x1d,0xb);
			athrs16_phy_write(dev,6,0x1e,0xbc00);

			/* mode_cfg is  set to auto media detect mode, This is for  AR8033 Fiber port and Copper Port */
			athrs16_phy_write(dev,5,0x1f,0x810b);
			athrs16_phy_write(dev,6,0x1f,0x810b);
#endif
			ar_init_once = 1;
		}	
#endif
	}

	return 0;
}

void apm_eth_shutdown(void)
{
	int i, rc;
	struct q_raw_state raw_qstate;
	struct mb_qm_qstate *qstate;
	struct mb_qm_msg_desc rx_msg_desc;
	struct mb_qm_msg32 msg;
	u32 no_rx_msg = 0;
	
	if (CPUID != CONFIG_SYS_MASTER_CPUID)
		return;

	if (is_coherent())
		return;

	ENET_DEBUG("ENET-QM U-boot shutdown starts ... \n");
	memset(&rx_msg_desc, 0, sizeof(struct mb_qm_msg_desc));
	memset(&msg, 0, sizeof(struct mb_qm_msg32));
	rx_msg_desc.msg = &msg;
	rx_msg_desc.mb_id = eth_queues[0].rx_mbid;
	rx_msg_desc.qid = eth_queues[0].rx_qid;
	rx_msg_desc.is_msg16 = 0;
	
	while ((rc = mb_qm_pull_msg(&rx_msg_desc)) == 0) {
		no_rx_msg++;	
	}
	ENET_DEBUG("Emptied :%d Rx Messsage \n", no_rx_msg);
	no_rx_msg = 0;

	/* Empty out the Comp Queue */
	memset(&rx_msg_desc, 0, sizeof(struct mb_qm_msg_desc));
	memset(&msg, 0, sizeof(struct mb_qm_msg32));
	rx_msg_desc.msg = &msg;
	
	rx_msg_desc.mb_id = eth_queues[0].comp_mb;
	rx_msg_desc.qid = eth_queues[0].comp_qid;
	rx_msg_desc.is_msg16 = 0;
	
	while ((rc = mb_qm_pull_msg(&rx_msg_desc)) == 0) {
		no_rx_msg++;	
	}
	ENET_DEBUG("Emptied :%d Comp Messsage \n", no_rx_msg);
	
	memset(&raw_qstate, 0, sizeof(struct q_raw_state));

	for (i = 1; i < 4 ; i++) {	
		qstate = &queue_states[i];
		
		if (pbn_valid[i]) {
			ENET_DEBUG("Disable qid:%d, slave_id: %d, pbn: %d \n",
			       qstate->q_id, qstate->slave_id, qstate->pbn);
			       
			mb_qm_pb_disable(qstate->ip_blk,
				      	qstate->slave_id,
				      	qstate->pbn, qstate->q_id);
			mb_qm_raw_qstate_wr(IP_BLK_QM, i, &raw_qstate);
		}			
	}
	qstate = &queue_states[eth_queues[0].comp_qid];
	
	if  (pbn_valid[eth_queues[0].comp_qid]) {
		ENET_DEBUG("Disable qid:%d, slave_id: %d, pbn: %d \n",
		       qstate->q_id, qstate->slave_id, qstate->pbn);
		
		mb_qm_pb_disable(qstate->ip_blk,
			       	qstate->slave_id,
			      	qstate->pbn, qstate->q_id);
		mb_qm_raw_qstate_wr(IP_BLK_QM, qstate->q_id, &raw_qstate);
	}
	
	ENET_DEBUG("Flush Enet Prefetch Buffer:%d \n",
	     	eth_queues[0].rx_fp_pbn - 0x20);

	apm_enet_fp_pb_flush(&gpriv, eth_queues[0].rx_fp_pbn - 0x20);
}

#if !defined(CONFIG_NET_MULTI)
void eth_halt(void)
{
	if (emac0_dev) {
		apm_eth_halt(emac0_dev);
		free(emac0_dev);
		emac0_dev = NULL;
	}
}

int eth_init(bd_t *bis)
{

	apm_eth_initialize(bis);
	if (emac0_dev) {
		return apm_eth_init(emac0_dev, bis);
	} else {
		printf("ERROR: ethaddr not set!\n");
		return -1;
	}
	return 0;
}

int eth_send(volatile void *packet, int length)
{
	return apm_eth_tx(emac0_dev, packet, length);
}

int eth_rx(void)
{
	return apm_eth_rx(emac0_dev);
}

#endif /* !defined(CONFIG_NET_MULTI) */


