/**
 * AppliedMicro APM862xx SoC Ethernet Driver
 *
 * Copyright (c) 2010 Applied Micro Circuits Corporation.
 * All rights reserved. Mahesh Pujara <mpujara@apm.com>
 *                      Ravi Patel <rapatel@apm.com>
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
 * @file apm_cle_cfg.c
 *
 * This file implements Classifier configurations in use by Ethernet driver.
 *
 */

#include "apm_enet_access.h"
#include "apm_cle_access.h"
#include "apm_preclass_data.h"

#define CLE_DEF_DB_INDEX	0
#define CLE_DB_INDEX		1

#if 1 
#define UBOOT_TFTP 
#endif 

static struct ptree_kn kn;
static struct apm_cle_dbptr dbptr[2];

static struct ptree_branch branch[] = {
	{      0, 0x0806, EQT, 1, 0, 12, 1, 0, 0 }, 	/* ARP --> */
	{      0, 0x0800, EQT, 1, 1, 22, 0, 0, 0 }, 	/* IPv4 ->*/
	{      0, 0x0806, EQT, 3, 0,  0, 0, 0, 0 }, 	/* ARP <-- */
	{ 0xff00, 0x0001, EQT, 3, 0,  0, 1, 0, 0 }, 	/* Allow ICMP  <- */
	{ 0XFF00, 0x0011, EQT, 2, 0, 36, 0, 0, 0 }, 	/* Allow UDP <- */
	{ 0xFFFF,      0, EQT, 3, 0,  0, 0, 0, 0 },  	/* UDP Port for tftp */
	{ 0xffff,      0, EQT, 4, 0,  0, 0, 0, 0 },
						/* Allow all of the above */
};

static struct ptree_dn dn[] = {
	{ START_NODE,	CLE_DEF_DB_INDEX, 0, 0, 0, 0, 2, &branch[0] },
	{ INTERIM_NODE, CLE_DEF_DB_INDEX, 0, 0, 0, 0, 1, &branch[2] },
#ifdef UBOOT_TFTP
	{ INTERIM_NODE, CLE_DEF_DB_INDEX, 0, 0, 0, 0, 2, &branch[3] },
#else 
	{ INTERIM_NODE, CLE_DEF_DB_INDEX, 0, 0, 0, 0, 1, &branch[3] },
#endif
	{ INTERIM_NODE, CLE_DEF_DB_INDEX, 0, 0, 0, 0, 1, &branch[5] },
	{ LAST_NODE,	CLE_DEF_DB_INDEX, 0, 0, 0, 0, 1, &branch[6] },
};

static struct ptree_node node[] = {
	{ 0, EWDN, 0, (struct ptree_dn*)&dn[0] },
	{ 1, FWDN, 0, (struct ptree_dn*)&dn[1] },
	{ 1, FWDN, 1, (struct ptree_dn*)&dn[2] },
	{ 2, FWDN, 0, (struct ptree_dn*)&dn[3] },
	{ 3, EWDN, 0, (struct ptree_dn*)&dn[4] },
	{ 4,   KN, 0,    (struct ptree_kn*)&kn },
};

int apm_preclass_init(u8 port_id, struct eth_queue_ids *eth_q)
{
	int rc = APM_RC_OK;
	int i = 0;

	memset(&dbptr, 0, sizeof(dbptr));
	memset(&kn, 0, sizeof(kn));

	PCLS_DBG("Create Preclassifier DB entries for Uboot Tree \n");
	for (i = 0; i < ARRAY_SIZE(dbptr); i++) {
		if (i == CLE_DEF_DB_INDEX) {
			/* Default entry should Drop */ 
			dbptr[i].drop = 1;
		} else {
			dbptr[i].dstqid = eth_q->rx_qid;
			dbptr[i].fpsel  = eth_q->rx_fp_pbn - 0x20;
		}
		dbptr[i].index = i;
		apm_set_cle_dbptr(&dbptr[i]);
	}
	kn.result_pointer = CLE_DB_INDEX;

	PCLS_DBG("Create Patricia Tree Nodes for Uboot Tree \n");
	for (i = 0; i < ARRAY_SIZE(node); i++) {
		if ((rc = apm_ptree_node_config(port_id, &node[i]))
		    != APM_RC_OK) {
			PCLS_ERR("Preclass init error: %d\n", rc);
			break;
		}
	}

	return rc;
}
