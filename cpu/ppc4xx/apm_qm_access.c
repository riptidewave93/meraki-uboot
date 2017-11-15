/****************************************************************************
 *
 * AMCC Mamba SoC QM Driver U-boot file
 *
 * Copyright (c) 2010 Applied Micro Circuits Corporation.
 * All rights reserved. Keyur Chudgar <kchudgar@amcc.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * @file apm_qm_access.c
 *
 * This file contains u-boot platform specific QM-TM driver code for Mamba SoC.
 *
 ******************************************************************************
 */

#include <common.h>
#include <asm/io.h>
#include <asm/apm_qm_access.h>
#include <asm/apm_qm_core.h>
#include <asm/apm_qm_reg.h>

u32 qm_csr_addr = CONFIG_SYS_QM_CSR_BASE;
u32 qm_fabric_addr = CONFIG_SYS_QM_FABRIC_BASE;
u32 qml_fabric_addr = 0; /* QM lite not supported in u-boot */
#if !defined (CONFIG_MB_DDR)
u32 qm_ocm_enq_mbox_addr = ENQ_MEMQ_MAILBOX_VADDR;
u32 qm_ocm_dq_mbox_addr = DQ_MEMQ_MAILBOX_VADDR;
u32 qm_ocm_fp_mbox_addr = FP_MEMQ_MAILBOX_VADDR;
#else
u32 qm_ocm_enq_mbox_addr; 
u32 qm_ocm_dq_mbox_addr;
u32 qm_ocm_fp_mbox_addr;
#endif
u32 max_qid = 255;
u32 start_qid = 1;
u32 is_smp = 0;		/* For U-Boot, CPU0 initialize QM only */
u32 start_mboxes = 0;	/* We assign start at 0 */                       
u32 maxmboxes = 8;	/* Assign only 8 mail box to work safely with AMP */
int is_noqml = 1;	/* Assume no QM lite under U-Boot */

/* global queue configuration table */
#ifdef UBOOT_HW_TEST
struct mb_qm_qstate mb_cfg_pqs[1] = {
	{IP_BLK_QM, 1, 21, 8, PB_SLAVE_ID_PPC, 8, P_QUEUE, MSG_32B, 0, 0, NO_NOTIFY, SIZE_16KB, 0, 0, 1, 0, 0, 0, 0, 0},
};
#elif defined (UBOOT_ETH_BRINGUP)
struct mb_qm_qstate mb_cfg_pqs0[NO_OF_QUEUES] = {
		/* PPC to Eth Egress Work Queue*/
		{IP_BLK_QM, 1, ETH_TX_Q, 0, PB_SLAVE_ID_ETH, 0, P_QUEUE, MSG_32B, 0, 0, NO_NOTIFY, SIZE_2KB, 0, 0, 1, 0, 0, 0, 1, 0},
		/* Eth to PPC Ingress Work Queue*/
		{IP_BLK_QM, 1, ETH_RX_Q, 12, PB_SLAVE_ID_PPC, 12, P_QUEUE, MSG_32B, 0, 0, NO_NOTIFY, SIZE_2KB, 0, 0, 1, 0, 0, 0, 0, 0},
		/* Eth to PPC Ingress Comp Queue*/
		{IP_BLK_QM, 1, ETH_COMP_Q, 10, PB_SLAVE_ID_PPC, 10, P_QUEUE, MSG_32B, 0, 0, NO_NOTIFY, SIZE_2KB, 0, 0, 1, 0, 0, 0, 0, 0},
		/* Eth ingress FP Queue*/
		{IP_BLK_QM, 1, ETH_FP_Q, 0, PB_SLAVE_ID_ETH, 0x29, FREE_POOL, MSG_16B, 0, 0, NO_NOTIFY, SIZE_2KB, 0, 0, 1, 0, 0, 0, 1, 0},
#if 0	
		/* PPC test queue */
		{IP_BLK_QM, 1, 21, 8, PB_SLAVE_ID_PPC, 8, P_QUEUE, MSG_32B, 0, 0, NO_NOTIFY, SIZE_2KB, 0, 0, 1, 0, 0, 0, 0, 0},
#endif
};
struct mb_qm_qstate mb_cfg_pqs1[NO_OF_QUEUES] = {
		/* PPC to Eth Egress Work Queue*/
		{IP_BLK_QM, 1, ETH1_TX_Q, 0, PB_SLAVE_ID_ETH, 8, P_QUEUE, MSG_32B, 0, 0, NO_NOTIFY, SIZE_2KB, 0, 0, 1, 0, 0, 0, 1, 0},
		/* Eth to PPC Ingress Work Queue*/
		{IP_BLK_QM, 1, ETH_RX_Q, 12, PB_SLAVE_ID_PPC, 12, P_QUEUE, MSG_32B, 0, 0, NO_NOTIFY, SIZE_2KB, 0, 0, 1, 0, 0, 0, 0, 0},
		/* Eth to PPC Ingress Comp Queue*/
		{IP_BLK_QM, 1, ETH_COMP_Q, 10, PB_SLAVE_ID_PPC, 10, P_QUEUE, MSG_32B, 0, 0, NO_NOTIFY, SIZE_2KB, 0, 0, 1, 0, 0, 0, 0, 0},
		/* Eth ingress FP Queue*/
		{IP_BLK_QM, 1, ETH_FP_Q, 0, PB_SLAVE_ID_ETH, 0x29, FREE_POOL, MSG_16B, 0, 0, NO_NOTIFY, SIZE_2KB, 0, 0, 1, 0, 0, 0, 1, 0},
#if 0
		/* PPC test queue */
		{IP_BLK_QM, 1, 21, 8, PB_SLAVE_ID_PPC, 8, P_QUEUE, MSG_32B, 0, 0, NO_NOTIFY, SIZE_2KB, 0, 0, 1, 0, 0, 0, 0, 0},
#endif
};
#elif defined (UBOOT_DMA_BRINGUP)
struct mb_qm_qstate mb_cfg_pqs[NO_OF_QUEUES] = {
	/* PPC to PktDMA Egress Work Queue*/
	{IP_BLK_QM, 1, 20, 4, PB_SLAVE_ID_DMA, 0, P_QUEUE, MSG_32B, 0, 0, NO_NOTIFY, SIZE_2KB, 0, 0, 1, 0, 0, 0, 1, 0}, 
	/* PktDMA to PPC Ingress Work Queue (used as a completion queue) */
	{IP_BLK_QM, 1, 21, 8, PB_SLAVE_ID_PPC, 8, P_QUEUE, MSG_32B, 0, 0, NO_NOTIFY, SIZE_2KB, 0, 0, 1, 0, 0, 0, 0, 0},
	/* PPC test queue */
	{IP_BLK_QM, 1, 22, 10, PB_SLAVE_ID_PPC, 10, P_QUEUE, MSG_32B, 0, 0, NO_NOTIFY, SIZE_2KB, 0, 0, 1, 0, 0, 0, 0, 0},
};
#else
struct mb_qm_qstate mb_cfg_pqs[NO_OF_QUEUES] = {
	{IP_BLK_QM, 1, ETH_TX_Q, 0, PB_SLAVE_ID_ETH, 0, P_QUEUE, MSG_32B, 0, 0, NO_NOTIFY, SIZE_16KB, 0, 0, 1, 0, 0, 0, 1, 0},  /* PPC to Eth Egress Work Queue*/
	{IP_BLK_QM, 1, ETH_RX_Q, 12, PB_SLAVE_ID_PPC, 12, P_QUEUE, MSG_32B, 0, 0, NO_NOTIFY, SIZE_16KB, 0, 0, 1, 0, 0, 0, 0, 0}, /* Eth to PPC Ingress Work Queue*/ 
	{IP_BLK_QM, 1, ETH_COMP_Q, 10, PB_SLAVE_ID_PPC, 10, P_QUEUE, MSG_32B, 0, 0, NO_NOTIFY, SIZE_16KB, 0, 0, 1, 0, 0, 0, 0, 0}, /* Eth to PPC Ingress Comp Queue*/  
	{IP_BLK_QM, 1, ETH_FP_Q, 0, PB_SLAVE_ID_ETH, 0x29, FREE_POOL, MSG_16B, 0, 0, NO_NOTIFY, SIZE_16KB, 0, 0, 1, 0, 0, 0, 1, 0}, /* Eth ingress FP Queue*/
	{IP_BLK_QM, 1, 20, 4, PB_SLAVE_ID_DMA, 0, P_QUEUE, MSG_32B, 0, 0, NO_NOTIFY, SIZE_16KB, 0, 0, 1, 0, 0, 0, 1, 0}, /* PPC to IODMA Egress Work Queue*/
	{IP_BLK_QM, 1, 21, 8, PB_SLAVE_ID_PPC, 8, P_QUEUE, MSG_32B, 0, 0, NO_NOTIFY, SIZE_16KB, 0, 0, 1, 0, 0, 0, 0, 0},
	
	{IP_BLK_QM, 1, 19, 4, PB_SLAVE_ID_DMA, 0x24, FREE_POOL, MSG_16B, 0, 0, NO_NOTIFY, SIZE_16KB, 0, 0, 1, 0, 0, 0, 1, 0},
	{IP_BLK_QM, 1, 25, 4, PB_SLAVE_ID_PPC, 0x24, FREE_POOL, MSG_16B, 0, 0, NO_NOTIFY, SIZE_16KB, 0, 0, 1, 0, 0, 0, 0, 1},
	{IP_BLK_QM, 1, 22, 5, PB_SLAVE_ID_SEC, 0, P_QUEUE, MSG_32B, 0, 0, NO_NOTIFY, SIZE_16KB, 0, 0, 1, 0, 0, 0, 1, 0}, /* PPC to Sec Work Queue */
	{IP_BLK_QM, 1, 23, 5, PB_SLAVE_ID_PPC, 5, P_QUEUE, MSG_32B, 0, 0, NO_NOTIFY, SIZE_16KB, 0, 0, 1, 0, 0, 0, 0, 0}, /* Sec to PPC work queue */
};
#endif

/* QM raw register read/write routine */
inline int mb_qm_wr32(int ip, u32 offset, u32 data)
{
	u32 addr = qm_csr_addr + offset;
#ifdef QM_DEBUG
	if (offset > CSR_PPC_SAB7_ADDR) {
		QM_ERR_CHK("Invalid offset in QM reg write: %x %s %d\n", offset, 
			__FILE__, __LINE__);
		return -1;
	}
#endif

	QMWRITE_PRINT("Write addr %x data %x\n", addr, data);
	out_be32((void __iomem *) addr, data);

	return 0;
}

inline int mb_qm_rd32(int ip, u32 offset, u32 *data)
{
	u32 addr = qm_csr_addr + offset;
#ifdef QM_DEBUG
	if (offset > CSR_PPC_SAB7_ADDR) {
		QM_ERR_CHK("Invalid offset in QM reg read: %x %s %d\n", offset, 
			__FILE__, __LINE__);
		return -1;
	}
#endif

	*data = in_be32((void __iomem *) addr);
	QMREAD_PRINT("Read addr %x data %x\n", addr, *data);

	return 0;
}

int mb_qm_init_errq(int ppc_id)
{
	u32 val;

	/* Enable QPcore and assign error queue */
	val = 0;
	val = ENABLE_F6_SET(val, 1);
	val = ERROR_QID_F2_SET(val, ERR_QUEUE_ID);
	val = ERROR_QUEUE_ENABLE_F2_SET(val, MB_QM_ERROR_Q_ENABLE);

	mb_qm_wr32(IP_BLK_QM, CSR_QM_CONFIG_ADDR, val);
	
	/* Enable QM lite if hardware available */
	if (!is_noqml)
		mb_qm_wr32(IP_BLK_QML, CSR_QM_CONFIG_ADDR, val);

	return 0;
}
