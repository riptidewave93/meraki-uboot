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
 * @file apm_enet.h
 *
 * This file implements APM862xx SoC Ethernet driver
 *
 */

#ifndef __APM_ENET_H__
#define __APM_ENET_H__

#include <common.h>
#include <net.h>
#include "apm_enet_mac.h"
#include "apm_enet_misc.h"

#define physical_addr(x) 	((unsigned int) (x))
#define virtual_addr(x)	 	((void *) (x))

#define NO_RX_BUFF		16

#define ENET_ADDR_LENGTH	6
#define EMAC_NUM_DEV		1

#define TX_DATA_LEN_MASK	0XFFF
#define ENET_MAX_MTU_ALIGNED	PKTSIZE_ALIGN
#define ETH_FP_PBN		0x29

struct enet_frame {
	unsigned char dest_addr[ENET_ADDR_LENGTH];
	unsigned char source_addr[ENET_ADDR_LENGTH];
	unsigned short type;
	unsigned char enet_data[1];
};

/* private information regarding device */
struct apm_enet_dev {
	struct eth_device *ndev;
	unsigned int port_id;
	struct apm_data_priv priv;
	struct apm_emac_error_stats estats;
	struct apm_emac_stats stats;
};

#endif /* __APM_ENET_H__ */

