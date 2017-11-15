/**
 * AppliedMicro APM862xx QM Driver
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * @file apm_qm_access.h
 **
 */

#ifndef __MB_QM_ACCESS_H__
#define __MB_QM_ACCESS_H__

#include <common.h>
#include <config.h>
#include <malloc.h>
#include <asm-ppc/processor.h>
#include <asm/apm_qm_core.h>

/* Enabled debug macros put here */
#define QM_DBG_ERR_CHK
#define QM_PRINT_ENABLE

/* debugging */
#if 0
#define QM_PRINT_ENABLE
#define DQM_DBG
#define DQM_DBG_WR
#define DQM_DBG_RD
#define QM_DBG_ERR_CHK
#define QM_DBG_STATE
#endif

#ifdef DQM_DBG
#define QM_DBG(x, ...)  printf(x, ##__VA_ARGS__)
#else
#define QM_DBG(x, ...)
#endif

#ifdef QM_PRINT_ENABLE
#define QM_PRINT(x, ...)  printf(x, ##__VA_ARGS__)
#else
#define QM_PRINT(x, ...)
#endif

#ifdef QM_DBG_ERR_CHK
#define QM_ERR_CHK(x, ...)  printf(x, ##__VA_ARGS__)
#else
#define QM_ERR_CHK(x, ...)
#endif

#ifdef DQM_DBG_WR
#define QMWRITE_PRINT(x, ...)  printf(x, ##__VA_ARGS__)
#else
#define QMWRITE_PRINT(x, ...)
#endif

#ifdef DQM_DBG_RD
#define QMREAD_PRINT(x, ...)  printf(x, ##__VA_ARGS__)
#else
#define QMREAD_PRINT(x, ...)
#endif

#ifdef QM_DBG_STATE
#define QM_STATE_DBG(x, ...)  printf(x, ##__VA_ARGS__)
#else
#define QM_STATE_DBG(x, ...)
#endif

#define MEMALLOC malloc

/* dummy virt to phys for u-boot */
#define virt_to_phys(x) x
#define phys_to_virt(x) x

/* Disble Reporting Errors to Error Queue */
#define MB_QM_ERROR_Q_ENABLE 0

#if 1 /* disable this code for normal u-boot, enable only for initial eth hw bring up */
#define UBOOT_ETH_BRINGUP
#endif

#if 0 /* disable this code for normal u-boot, enable only for initial hw bring up */
#define UBOOT_HW_TEST
#endif

#if 0 /* disable this code for normal u-boot, enable only for initial DMA hw bring up */
#define UBOOT_DMA_BRINGUP
#endif

#if 0
#define CONFIG_COHERENT_MB
#endif

#ifdef UBOOT_HW_TEST
#define NO_OF_QUEUES	1
#elif defined (UBOOT_ETH_BRINGUP)
#define NO_OF_QUEUES	4
#elif defined (UBOOT_DMA_BRINGUP)
#define NO_OF_QUEUES	3
#else
#define NO_OF_QUEUES	10
#endif

#define ETH_COMP_Q	100
#define ETH_TX_Q	101
#define ETH1_TX_Q	102
#define ETH_RX_Q	103
#define ETH_FP_Q	104


#define ETH_COMP_Q_MBID	10
#define ETH_RX_Q_MBID	12

/* FIXME */
#define CORE0_MB_START 	0	
#define CORE0_NO_MB 	8
#define CORE0_Q_START 	0	
#define CORE0_NO_Q 	125
#define CORE0_ENET_WQ_PBN_START		0
#define CORE0_NO_ENET_WQ_PBN 		8
#define CORE0_ENET_FP_PBN_START  	0X20
#define CORE0_NO_ENET_FP_PBN     	8

#ifdef CONFIG_MB_OCM
#define ENQ_MEMQ_MAILBOX_VADDR   0x90004000
#elif defined (CONFIG_MB_MEMQ) /* mailboxes are in MEMQ */
#define ENQ_MEMQ_MAILBOX_VADDR   (CONFIG_SYS_MB_DDRC_BASE + 0x1000)
#endif
#define DQ_MEMQ_MAILBOX_VADDR    (ENQ_MEMQ_MAILBOX_VADDR + 0x1000)
#define FP_MEMQ_MAILBOX_VADDR    (DQ_MEMQ_MAILBOX_VADDR + 0x1000)

#ifdef CONFIG_QUEUE_OCM
#define OCM_QUEUE_BASE_VADDR 0x90000000
#define OCM_QUEUE_BASE_PADDR 0xffff8000
#define OCM_QUEUE_HIGH_ADDR 0xe
#endif

extern u32 qm_ocm_enq_mbox_addr;
extern u32 qm_ocm_dq_mbox_addr;
extern u32 qm_ocm_fp_mbox_addr;
extern u32 qm_fabric_addr;
extern u32 qml_fabric_addr;
extern u32 maxqueues;

/* QM raw register read/write routine */
inline int mb_qm_wr32(int ip, u32 offset, u32 data);
inline int mb_qm_rd32(int ip, u32 offset, u32 *data);

#endif /* __MB_QM_ACCESS_H__ */
