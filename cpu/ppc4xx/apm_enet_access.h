/**
 * AppliedMicro APM862xx SoC Ethernet Driver
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * @file apm_enet_access.h
 *
 * This file defines access layer for APM862xx SoC Ethernet driver
 *
 **/

#ifndef __APM_ENET_ACCESS_H__
#define __APM_ENET_ACCESS_H__

#include <common.h>
#include <asm/apm_ipp_csr.h>
#include <asm/apm_scu.h>

/*#undef PHY_DEBUG*/
#undef FAM_UBOOT
/*#undef DEBUG_HEXDUMP*/
/*#undef APM_ENET_DEBUG*/
// #undef DEBUG_RD   
// #undef DEBUG_WR   
// #undef DEBUG_RX 
// #undef DEBUG_TX

// #define APM_ENET_DEBUG
// #define PHY_DEBUG
// #define DEBUG_ERR 
// #define DEBUG_RD   
// #define DEBUG_WR   
// #define DEBUG_RX 
// #define DEBUG_TX
// #define DEBUG_HEXDUMP

#ifdef PHY_DEBUG
#define PHY_PRINT(x, ...)		printf((x), ##__VA_ARGS__)
#else
#define PHY_PRINT(x, ...)
#endif

#ifdef APM_ENET_DEBUG
#define ENET_DEBUG(x, ...)		printf((x), ##__VA_ARGS__)
#else
#define ENET_DEBUG(x, ...)	
#endif

#ifdef DEBUG_ERR
#define ENET_DEBUG_ERR(x, ...)		printf((x), ##__VA_ARGS__)
#else
#define ENET_DEBUG_ERR(x, ...)	    
#endif

#ifdef DEBUG_RD
#define ENET_DEBUG_RD(x, ...)		printf((x), ##__VA_ARGS__)
#else
#define ENET_DEBUG_RD(x, ...)		
#endif

#ifdef DEBUG_WR
#define ENET_DEBUG_WR(x, ...)		printf((x), ##__VA_ARGS__)
#else
#define ENET_DEBUG_WR(x, ...)		
#endif

#ifdef DEBUG_HEXDUMP
#define ENET_RXHEXDUMP(b, l)		putshex(b, l)
#define ENET_TXHEXDUMP(b, l)		putshex(b, l)
#else
#define ENET_RXHEXDUMP(b, l)
#define ENET_TXHEXDUMP(b, l)
#endif

#ifdef DEBUG_RX
#define ENET_DEBUG_RX(x, ...)		printf((x), ##__VA_ARGS__)
#else
#define ENET_DEBUG_RX(x, ...)		
#endif

#ifdef DEBUG_TX
#define ENET_DEBUG_TX(x, ...)		printf((x), ##__VA_ARGS__)
#else
#define ENET_DEBUG_TX(x, ...)		
#endif

/* Queues related parameters per Enet port */
struct eth_queue_ids {
	u32 rx_qid;
	u32 rx_mbid; 	/* mailbox id for receive queue */
	u32 rx_fp_qid;
	u32 rx_fp_mbid;
	u32 rx_fp_pbn;
	u32 tx_qid;		/* Endque q id	      */
	u32 comp_qid;		/* completion q id    */
	u32 comp_mb;		/* completion mailbox */
};
/**
 * @brief   This function performs preclassifier engine Initialization
 * 	    and node configurations.  
 * @param   port_id - GE Port number
 * @param   *eth_q  - Pointer to queue configurations used by Ethernet 
 * @return  0 - success or -1 - failure
 */
int apm_preclass_init(u8 port_id, struct eth_queue_ids *eth_q);

#endif	/* __APM_ENET_ACCESS_H__ */
