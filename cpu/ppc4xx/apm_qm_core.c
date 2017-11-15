/**
 * AppliedMicro AM862xx QM Driver
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
 * @file apm_qm_core.c
 **
 */

#include <asm/io.h>
#include <asm/apm_qm_access.h>
#include <asm/apm_qm_core.h>
#include <asm/apm_qm_reg.h>
#include <asm/apm_ipp_csr.h>

#if defined FLUSH_MB || defined INVALIDATE_MB
#include <asm/cacheflush.h>
#endif

/* enqueue for work queue and free pool */
struct mb_mailbox_ctxt mb_enq_mbox_ctxt[ENQUE_MAIL_BOXES];
/* dequeue for work queue (pq or vq) */
struct mb_mailbox_ctxt mb_dq_mbox_ctxt[DQ_MAIL_BOXES];
/* dequeue for free pool */
struct mb_mailbox_ctxt mb_fp_mbox_ctxt[FP_MAIL_BOXES];

/* pointer to maibox for enqueue for work queue and free pool */
struct mailbox *enq_mboxes;
/* pointer to maibox for dequeue for work queue (pq or vq) */
struct mailbox *dq_mboxes;
/* pointer to maibox for dequeue for free pool */
struct fp_mailbox *fp_mboxes;

/* global queue configuration table */
extern struct mb_qm_qstate mb_cfg_pqs0[];
extern struct mb_qm_qstate mb_cfg_pqs1[];
extern u32 max_qid;
extern u32 start_qid;
extern u32 qml_ipp_csr_addr;
extern u32 is_smp;
extern u32 start_mboxes;
extern u32 maxmboxes;
extern int is_noqml;

#ifdef CONFIG_MB_DDR
extern u32 qm_ocm_enq_mbox_addr; 
extern u32 qm_ocm_dq_mbox_addr;
extern u32 qm_ocm_fp_mbox_addr;
#endif

/* qm message callback function table */
mb_qm_msg_fn_ptr mb_qm_cb_fn_table[MB_QM_MAX_RTYPE];

/* total 4 comp queues in the system:
	- Ethx to PPC0 
	- Ethx to PPC1 
	- All other blocks to PPC0 
	- All other blocks to PPC1 
*/
struct mb_qm_qstate comp_qstate[4];

/* queue to mailbox and mailbox to queue mappings */
u32 q_to_mb[MB_MAX_QUEUES];
u32 mb_to_q[MB_MAX_QUEUES];

/* resource distribution mappings */
u32 ip_to_slvid[IP_MAX]; /* IP block to slave ID mapping */

u32 dq_fp_ip_to_mb[MAX_SLAVES]; /* Mailbox used to deq msg from IP blk (FQ) */
u32 ip_to_pbn_wq[MAX_CORES][IP_MAX]; /* Current PBN to be given for this IP (WQ) */
u32 ip_to_pbn_fq[MAX_CORES][IP_MAX]; /* Current PBN to be given for this IP (FQ) */ 

struct mb_qm_qstate queue_states[MB_MAX_QUEUES];
struct q_raw_state q_raw_states[MB_MAX_QUEUES];
u32 pbn_valid[MB_MAX_QUEUES];

#ifdef CHECK_UINFO
u32 slot_uinfo[DQ_MAIL_BOXES][TOTAL_SLOTS_32BYTE_MSG];
#endif

int mb_qm_pb_overwrite(int ip, int slv_id, int pbn, int qnum, u8 is_fp)
{
	u32 pbn_buf = 0;
	u32 val = (slv_id << SLAVE_ID_SHIFT) | pbn;

	QM_DBG("Config PBN: PBM addr: %x val: %x\n", CSR_PBM_ADDR, val);

	mb_qm_wr32(ip, CSR_PBM_ADDR, val);

	pbn_buf |= ((1 << 31) | (qnum << 4) | (is_fp << 13) | (1 << 14) | 0x4) ;

	QM_DBG("PBM WR addr: %x val: %x\n", CSR_PBM_BUF_WR_ADDR, pbn_buf);

	mb_qm_wr32(ip, CSR_PBM_BUF_WR_ADDR, pbn_buf);

	pbn_valid[qnum] = 1;

	return 0;
}

int mb_qm_pb_get(int ip, int slv_id, int pbn)
{
	u32 pbn_buf = 0;
	u32 val = (slv_id << SLAVE_ID_SHIFT) | pbn;

	QM_DBG("Prefetch buffer manager addr: %x val: %x\n",
	       CSR_PBM_ADDR, val);

	mb_qm_wr32(ip, CSR_PBM_ADDR, val);

	mb_qm_rd32(ip, CSR_PBM_BUF_RD_ADDR,&pbn_buf);

	QM_DBG("Prefetch buffer manager RD addr: %x val: %x\n", 
		CSR_PBM_BUF_RD_ADDR, pbn_buf);

	return pbn_buf;
}

int mb_qm_pb_set(int ip, int slv_id, int pbn, int pbn_buf)
{
	u32 val = (slv_id << SLAVE_ID_SHIFT) | pbn;

	QM_DBG("Prefetch buffer manager addr: %x val: %x\n",
	       CSR_PBM_ADDR, val);

	mb_qm_wr32(ip, CSR_PBM_ADDR, val);

	QM_DBG("Prefetch buffer manager WR addr: %x val: %x\n", 
	       CSR_PBM_BUF_WR_ADDR, pbn_buf);
	
	mb_qm_wr32(ip, CSR_PBM_BUF_WR_ADDR, pbn_buf);
	return pbn_buf;
}

int mb_qm_pb_disable(int ip, int slv_id, int pbn, int qnum)
{

	u32 pbn_buf = (1 << 31);
	u32 val = (slv_id << SLAVE_ID_SHIFT) | pbn;
	int pbn_val = 0;

	pbn_val = mb_qm_pb_get(ip, slv_id, pbn);

	mb_qm_wr32(ip, CSR_PBM_ADDR, val);
	mb_qm_wr32(ip, CSR_PBM_BUF_WR_ADDR, pbn_buf);
	pbn_valid[qnum] = 0;
	
	pbn_val = mb_qm_pb_get(ip, slv_id, pbn);
	
	return 0;
}
int mb_qm_pb_config(int ip, int slv_id, int pbn, int qnum, u8 is_fp, u8 is_vq)
{
	u32 pbn_buf = 0;
	u32 val = (slv_id << SLAVE_ID_SHIFT) | pbn;

	QM_DBG("Configuring prefetch buffer manager\n");

	QM_DBG("Prefetch buffer manager addr: %x val: %x\n",
	    	CSR_PBM_ADDR, val);

	mb_qm_wr32(ip, CSR_PBM_ADDR, val);

	pbn_buf |= ((qnum << 4) | (is_fp << 13) | (is_vq << 12) | (1 << 14));

	QM_DBG("Prefetch buffer manager WR addr: %x val: %x\n", 
     		CSR_PBM_BUF_WR_ADDR, pbn_buf);

	mb_qm_wr32(ip, CSR_PBM_BUF_WR_ADDR, pbn_buf);

	pbn_valid[qnum] = 1;

#ifdef QM_DEBUG
	if (is_vq) {
		QM_PRINT("Prefetch buffer manager WR addr: %x val: %x\n",
			 CSR_PBM_BUF_WR_ADDR, pbn_buf);
	}
#endif
	return 0;
}

int mb_qm_raw_qstate_rd(int ip, int q_num, struct q_raw_state *raw_qstate)
{
	int rc = 0;
	u32 queue_id = 0;
	u32 val;

#ifdef QM_DEBUG
	if (q_num < 0 || q_num > 255) {
		QM_PRINT("Queue number is not valid\n");
		return -1;
	}
#endif
	/* write queue number */
	queue_id = QNUMBER_F2_SET(queue_id, q_num);
	QM_STATE_DBG("Writing queue number %d to location: 0x%x\n", queue_id, CSR_QSTATE_ADDR);
	mb_qm_wr32(ip, CSR_QSTATE_ADDR, queue_id);

	/* word 0 */
	mb_qm_rd32(ip, CSR_QSTATE_RD_0_ADDR, &val); 
	QM_STATE_DBG("Reading queue state word 0: 0x%x\n", val); 
	memcpy(&raw_qstate->w0, &val, sizeof(u32));
	QM_STATE_DBG("Word0 in RAW: 0x%x\n", raw_qstate->w0); 

	/* word 1 */
	mb_qm_rd32(ip, CSR_QSTATE_RD_1_ADDR, &val); 
	QM_STATE_DBG("Reading queue state word 1: 0x%x\n", val); 
	memcpy(&raw_qstate->w1, &val, sizeof(u32));
	QM_STATE_DBG("Word1 in RAW: 0x%x\n", raw_qstate->w1); 

	/* word 2 */
	mb_qm_rd32(ip, CSR_QSTATE_RD_2_ADDR, &val); 
	QM_STATE_DBG("Reading queue state word 2: 0x%x\n", val); 
	memcpy(&raw_qstate->w2, &val, sizeof(u32));
	QM_STATE_DBG("Word2 in RAW: 0x%x\n", raw_qstate->w2); 

	/* word 3 */
	mb_qm_rd32(ip, CSR_QSTATE_RD_3_ADDR, &val); 
	QM_STATE_DBG("Reading queue state word 3: 0x%x\n", val); 
	memcpy(&raw_qstate->w3, &val, sizeof(u32));
	QM_STATE_DBG("Word3 in RAW: 0x%x\n", raw_qstate->w3);
	return rc;
}

int mb_qm_qstate_rd_cfg(int q_num, struct mb_qm_qstate *qstate)
{
	if (q_num < 0 || q_num > MB_MAX_QUEUES) {
		return -1;
	}

	memcpy(qstate, &queue_states[q_num], sizeof (struct mb_qm_qstate));
	return 0;
}

int mb_qm_qstate_rd(int ip, int q_num, struct mb_qm_qstate *qstate)
{
	struct pqstate_w0 *w0;
	struct pqstate_w1 *w1;
	struct pqstate_w2 *w2;
	struct pqstate_w3 *w3;

	struct pqstate_w0 w0v;
	struct pqstate_w1 w1v;
	struct pqstate_w2 w2v;
	struct pqstate_w3 w3v;
	
	struct mb_qm_qstate *q_sw_state = &queue_states[q_num];

	int rc = 0;
	u32 queue_id = 0;
	u32 val;
	
	if ((!qstate->valid) || (qstate->q_type == QUEUE_DISABLED)) {
		QM_PRINT("Queue config is not valid\n");
		return -1;
	}

	if (q_num < 0 || q_num > 255) {
		QM_PRINT("Queue number is not valid\n");
		return -1;
	}

	/* write queue number */
	queue_id = QNUMBER_F2_SET(queue_id, q_num);
	QM_STATE_DBG("Writing queue number %d to location: 0x%x\n", queue_id, CSR_QSTATE_ADDR);
	mb_qm_wr32(ip, CSR_QSTATE_ADDR, queue_id);

	/* word 0 */
	mb_qm_rd32(ip, CSR_QSTATE_RD_0_ADDR, &val); 
	QM_STATE_DBG("Reading queue state word 0: 0x%x\n", val); 
	memcpy(&w0v, &val, sizeof(u32));
	w0 = &w0v;

	qstate->q_type = w0->cfgqtype; 
	qstate->thr_set = w0->cfgselthrsh; 
	qstate->fp_mode = w0->fp_mode; 
	qstate->q_size = w0->cfgqsize;

	/* word 1 */
	mb_qm_rd32(ip, CSR_QSTATE_RD_1_ADDR, &val); 
	QM_STATE_DBG("Reading queue state word 1: 0x%x\n", val); 
	memcpy(&w1v, &val, sizeof(u32));
	w1 = &w1v;

	qstate->q_start_addr = w1->cfgstartaddr;

	/* word 2 */
	mb_qm_rd32(ip, CSR_QSTATE_RD_2_ADDR, &val); 
	QM_STATE_DBG("Reading queue state word 2: 0x%x\n", val); 
	memcpy(&w2v, &val, sizeof(u32));
	w2 = &w2v;
	qstate->nummsgs = w2->nummsg;

	/* word 3 */
	mb_qm_rd32(ip, CSR_QSTATE_RD_3_ADDR, &val); 
	QM_STATE_DBG("Reading queue state word 3: 0x%x\n", val); 
	memcpy(&w3v, &val, sizeof(u32));
	w3 = &w3v;

	qstate->rid = w3->rid;
	qstate->cfgsaben = w3->cfgsaben;
	qstate->ppc_notify = w3->ppc_notify;

	/* fill in some more info from sw state */
	qstate->q_id = q_sw_state->q_id;
	qstate->ip_blk = q_sw_state->ip_blk;
	qstate->valid = q_sw_state->valid;
	qstate->mb_id = q_sw_state->mb_id;
	qstate->slave_id = q_sw_state->slave_id;
	qstate->pbn = q_sw_state->pbn;

	return rc;
}

int mb_qm_raw_qstate_wr(int qm_blk_id, int q_num, struct q_raw_state *raw_qstate)
{
	int queue_id = 0;
	int rc = 0;

	if (q_num < 0 || q_num > 255) {
		QM_PRINT("Queue number not valid in cfg write: 0x%x\n", q_num);
		return -1;
	}

	/* write queue number */
	queue_id = QNUMBER_F2_SET(queue_id, q_num);
	QM_STATE_DBG("State Write: QID %d location 0x%x\n", 
		queue_id, CSR_QSTATE_ADDR);
	mb_qm_wr32(qm_blk_id, CSR_QSTATE_ADDR, queue_id);

	mb_qm_wr32(qm_blk_id, CSR_QSTATE_WR_0_ADDR, raw_qstate->w0);
	mb_qm_wr32(qm_blk_id, CSR_QSTATE_WR_1_ADDR, raw_qstate->w1);
	mb_qm_wr32(qm_blk_id, CSR_QSTATE_WR_2_ADDR, raw_qstate->w2);
	mb_qm_wr32(qm_blk_id, CSR_QSTATE_WR_3_ADDR, raw_qstate->w3);

	QM_STATE_DBG("Writing Word 0x%08X 0x%08X 0x%08X 0x%08X\n", 
		raw_qstate->w0,raw_qstate->w1, raw_qstate->w2, raw_qstate->w3);

	return rc;
}

int mb_qm_qstate_wr(struct mb_qm_qstate *qstate)
{
	struct pqstate_w0 w0 = {0};
	struct pqstate_w1 w1 = {0}; 
	struct pqstate_w3 w3 = {0};  
	int rc = 0;
	u32 queue_id = 0;
	u32 val;
	u32 q_size = 0;
	u8 *qaddr = NULL;
	u32 qaddr_phy;
	u32 mem_size;
	int q_num = qstate->q_id;
#ifdef CONFIG_QUEUE_OCM 
	static u32 ocm_qcnt = 0;
#endif

	if (q_num < 0 || q_num > 255) {
		QM_PRINT("Queue number not valid in cfg write: 0x%x\n", q_num);
		return -1;
	}

	if ((!qstate->valid) || (qstate->q_type == QUEUE_DISABLED)) {
		QM_PRINT("Queue config not valid in cfg write\n");
		return -1;
	}

	if (qstate->ip_blk != IP_BLK_QML && qstate->ip_blk != IP_BLK_QM) {
		QM_PRINT("Invalid Block ID in cfg write: %d\n", qstate->ip_blk);
		return -1;
	}

	if (qstate->ip_blk != IP_BLK_QML) {
		if (qstate->direction) { /* egress direction */
			QM_DBG("Using MB: %d for queue: %d in EGRESS\n", 
							qstate->mb_id, q_num);
			q_to_mb[q_num] = qstate->mb_id;
		} else {
			QM_DBG("Using MB: %d for queue: %d in INGRESS\n", 
							qstate->mb_id, q_num);
			if (qstate->vqen == ENABLE_VQ) {
				mb_to_q[qstate->mb_id] = qstate->parent_vqid;
			} else {
				mb_to_q[qstate->mb_id] = q_num;
			}
		}
	}

	/* write queue number */
	queue_id = QNUMBER_F2_SET(queue_id, q_num);
	QM_STATE_DBG("State Write: Writing QID %d location: 0x%x\n", 
		queue_id, CSR_QSTATE_ADDR);
	mb_qm_wr32(qstate->ip_blk, CSR_QSTATE_ADDR, queue_id);

	/* word0 */
	w0.cfgqtype = qstate->q_type;
	w0.cfgselthrsh = qstate->thr_set;
	w0.cfgqsize = qstate->q_size; 
	w0.fp_mode = qstate->fp_mode;

	/* FIXME - Work around for Enet Fragmented packet,
	   enable ERR_QUEUE_ID as a single error queue
	*/
	w0.cfgacceptlerr = 1;
        
	switch (qstate->q_size) {
	case SIZE_512B:
		q_size = 512;
		break;
	case SIZE_2KB:
		q_size = 2 * 1024;
		break;
	case SIZE_16KB:
		q_size = 16 * 1024;
		break;
	case SIZE_64KB:
		q_size = 64 * 1024;
		break;
	case SIZE_512KB:
		q_size = 512 * 1024;
		break;
	default:
		QM_PRINT("Invalid queue size in cfg write: %d\n", 
			qstate->q_size);
		return -1;
	}

	/* if its a free queue, ask QM to set len to 0 when dealloc */
	if(qstate->q_type == FREE_POOL)
		w0.fp_mode = 0x3;	/* FIXME, replace with enum once tested */
	

#ifdef CONFIG_QUEUE_OCM /* Create queues in OCM */
	mem_size = q_size;
	qaddr = (u8 *) OCM_QUEUE_BASE_VADDR + (ocm_qcnt * q_size);
	qaddr_phy = OCM_QUEUE_BASE_PADDR + (ocm_qcnt * q_size);
	qaddr_phy = qaddr_phy >> 8;
	ocm_qcnt++;
	qstate->q_start_addr = qaddr_phy; /* 256 byte aligned */
        w0.cfgstartaddr = OCM_UPPER_ADDR_NIBBLE;
	w0.cfgacceptlerr = 1;
#else
	if (qstate->ip_blk != IP_BLK_QML || /* QM queues reside in DDR */
		(qstate->q_id == ENET_RX_FQ_TO_DDR)) { /* This is intermediate 
					queue, should be residing in DDR */
		mem_size = q_size + 512;
		qaddr = (u8 *) MEMALLOC(mem_size);

		if (qaddr == NULL) {
			QM_PRINT("Could not allocate memory for queue\n");
			return -1;
		} else {
			u8 *qaddr_aligned = NULL;
			u32 tmp_qaddr;
			qaddr_aligned = qaddr + 256;
			tmp_qaddr = (u32) qaddr_aligned;
			tmp_qaddr &= 0xFFFFFF00; /* queue addresses 256 byte 
						    aligned */
#ifdef CONFIG_QUEUE_FAM
                	w0.cfgstartaddr = FAM_UPPER_ADDR_NIBBLE;
#endif
			qaddr_phy = (u32) virt_to_phys((u32*)tmp_qaddr);
			qstate->q_start_addr = qaddr_phy >> 8; /* 256 byte
								aligned */
			QM_DBG("Queue Phy addr: %x, virtual addr: %x\n", 
							qaddr_phy, tmp_qaddr);
		}
	} else {
		switch(qstate->q_type) {
		case FREE_POOL:
			qaddr_phy = QML_ETH_IPP_INGRESS_FP_ADDR;
			break;
		case P_QUEUE:
                        if(qstate->direction) /* Egress */
                                qaddr_phy = QML_ETH_IPP_EGRESS_WQ_ADDR;
                        else
                                qaddr_phy = QML_ETH_IPP_INGRESS_WQ_ADDR;
			break;
		default:
			QM_PRINT("Invalid queue type in QML queue config\n");
			return -1;
		}

		qstate->q_start_addr = qaddr_phy >> 8; /* 256 byte aligned */
                w0.cfgstartaddr = QML_UPPER_ADDR_NIBBLE;
        }
#endif

	/* Not touching start address in w0, as most significant 4 bits will be
	   0 or set above in 36 bits address */
	memcpy(&val, &w0, sizeof(u32));
	QM_STATE_DBG("Writing Word 0 0x%x\n", val);
	mb_qm_wr32(qstate->ip_blk, CSR_QSTATE_WR_0_ADDR, val);
#ifdef QM_DEBUG
	if (qstate->vqen)
		QM_PRINT("Writing Word 0 0x%x\n", val);
#endif
	/* word1 */
	w1.cfgstartaddr = qstate->q_start_addr & 0xFFFFFF;
	memcpy(&val, &w1, sizeof(u32));
	QM_STATE_DBG("Writing Word 1 0x%x\n", val);
	mb_qm_wr32(qstate->ip_blk, CSR_QSTATE_WR_1_ADDR, val);
#ifdef QM_DEBUG
	if (qstate->vqen)
		QM_PRINT("Writing Word 1 0x%x\n", val);
#endif
	/* don't need to write word2 */
	val = 0;
	QM_STATE_DBG("Writing Word 2 0x%x\n", val);
	mb_qm_wr32(qstate->ip_blk, CSR_QSTATE_WR_2_ADDR, val);
#ifdef QM_DEBUG
	if (qstate->vqen)
		QM_PRINT("Writing Word 2 0x%x\n", val);
#endif
	/* word3 */
	w3.ppc_notify = qstate->ppc_notify; 
	w3.cfgnotifyqne = qstate->q_not_empty_intr; 
	w3.cfgsaben = qstate->cfgsaben;

	if (qstate->q_type == P_QUEUE) {
		w3.cfgtmvq = qstate->parent_vqid; 
		w3.cfgtmvqen = qstate->vqen; 
	}

	memcpy(&val, &w3, sizeof(u32));
	QM_STATE_DBG("Writing Word 3 0x%x\n", val);
	mb_qm_wr32(qstate->ip_blk, CSR_QSTATE_WR_3_ADDR, val);
#ifdef QM_DEBUG
	if (qstate->vqen)
		QM_PRINT("Writing Word 3 0x%x\n", val);
#endif
	/* copy software queue state */
	memcpy(&queue_states[qstate->q_id], qstate,
	    	sizeof (struct mb_qm_qstate));

	return rc;
}

int mb_qm_cstate_wr(u32 qid, u32 cstate[2])
{
	u32 queue_id = 0;
	int rc = 0;
	
	if (qid < 0 || qid > 255) {
		QM_PRINT("Queue number not valid in cfg write: 0x%x\n", qid);
		return -1;
	}

	/* write queue number */
	queue_id = QNUMBER_F2_SET(queue_id, qid);
	QM_DBG("Credit RAM State Write: Writing queue number %d"
		 " to location: 0x%x\n",
		 queue_id, CSR_QSTATE_ADDR);
	mb_qm_wr32(IP_BLK_QM, CSR_QSTATE_ADDR, queue_id);

	QM_DBG("Writing Credit RAM State Word 0 0x%x \n", cstate[0]);
	mb_qm_wr32(IP_BLK_QM, CSR_CSTATE_WR_0_ADDR, cstate[0]);
	

	QM_DBG("Writing Credit RAM State Word 1 0x%x \n", cstate[1]);
	mb_qm_wr32(IP_BLK_QM, CSR_CSTATE_WR_1_ADDR, cstate[1]);
 	
	return rc;
}

int mb_qm_cstate_rd(u32 qid, u32 * cstate)
{
	u32 queue_id = 0;
	int rc = 0;

	if (qid < 0 || qid > 255) {
		QM_PRINT("Queue number not valid in cfg write: 0x%x\n", qid);
		return -1;
	}

	/* write queue number */
	queue_id = QNUMBER_F2_SET(queue_id, qid);
	QM_DBG("Credit RAM State Write: Writing queue number %d"
		 " to location: 0x%x\n",
		 queue_id, CSR_QSTATE_ADDR);
	mb_qm_wr32(IP_BLK_QM, CSR_QSTATE_ADDR, queue_id);

	mb_qm_rd32(IP_BLK_QM, CSR_CSTATE_RD_0_ADDR, cstate);
	QM_DBG("Credit RAM State Word 0 0x%x \n", *cstate);

	cstate++;
	mb_qm_rd32(IP_BLK_QM, CSR_CSTATE_RD_1_ADDR, cstate);
	QM_DBG("Credit RAM State Word 1 0x%x \n", *cstate);
	return rc;
}


int mb_qm_vqstate_wr(struct mb_qm_qstate *qstate)
{
	struct vqstate_w0 w0 = {0};
	struct vqstate_w1 w1 = {0};
	struct vqstate_w2 w2 = {0}; 
	struct vqstate_w3 w3 = {0};  
	int rc = 0;
	u32 queue_id = 0;
	u32 val;
	int q_num = qstate->q_id;
	int i;
	u32 cstate[2] = {0};

	if (q_num < 0 || q_num > 255) {
		QM_PRINT("Queue number not valid in cfg write: 0x%x\n", q_num);
		return -1;
	}

	if ((!qstate->valid) || (qstate->q_type != V_QUEUE)) {
		QM_PRINT("Virtual queue config not valid in cfg write\n");
		return -1;
	}

	if (qstate->ip_blk != IP_BLK_QM) {
		QM_PRINT("Invalid Block ID in cfg write for VQ: %d\n",
			 qstate->ip_blk);
		return -1;
	}

	/* write queue number */
	queue_id = QNUMBER_F2_SET(queue_id, q_num);
	QM_DBG("State Write: Writing queue number %d to location: 0x%x\n",
		 queue_id, CSR_QSTATE_ADDR);
	mb_qm_wr32(qstate->ip_blk, CSR_QSTATE_ADDR, queue_id);

	/* word0 */
	w0.cfgqtype = qstate->q_type;
	w0.cfgselthrsh = qstate->thr_set;
	w0.q0_sel = qstate->pq_sel[0];
	w0.q0selarb = qstate->q_sel_arb[0];
	if (qstate->q_sel_arb[0] == DRR_ARB)
		w0.q0txallowed = 1;	
	
	w0.q1_sel = qstate->pq_sel[1];
	w0.q1selarb = qstate->q_sel_arb[1];
	if (qstate->q_sel_arb[1] == DRR_ARB)
		w0.q1txallowed = 1;	

	w0.q2_sel_3b = qstate->pq_sel[2] >> 5;
	
	memcpy(&val, &w0, sizeof(u32));
	QM_DBG("Writing VQ Word 0 0x%x\n", val);
	mb_qm_wr32(qstate->ip_blk, CSR_QSTATE_WR_0_ADDR, val);

	/* word1 */
	
	w1.q2_sel_5b = qstate->pq_sel[2] & 0x1F;
	w1.q2selarb = qstate->q_sel_arb[2];
	if (qstate->q_sel_arb[2] == DRR_ARB)
		w1.q2txallowed = 1;	

	
	w1.q3_sel = qstate->pq_sel[3];
	w1.q3selarb = qstate->q_sel_arb[3];
	if (qstate->q_sel_arb[3] == DRR_ARB)
		w1.q3txallowed = 1;	

	w1.q4_sel = qstate->pq_sel[4];
	w1.q4selarb = qstate->q_sel_arb[4] >> 1;	
	if (qstate->q_sel_arb[4] == DRR_ARB)
		w1.q4txallowed = 1;	

		
	memcpy(&val, &w1, sizeof(u32));
	QM_DBG("Writing VQ Word 1 0x%x\n", val);
	mb_qm_wr32(qstate->ip_blk, CSR_QSTATE_WR_1_ADDR, val);

	/* Write word2 */
	w2.q4selarb = qstate->q_sel_arb[4] & 1;


	w2.q5_sel = qstate->pq_sel[5];
	w2.q5selarb = qstate->q_sel_arb[5];
	if (qstate->q_sel_arb[5] == DRR_ARB)
		w2.q5txallowed = 1;	

	w2.q6_sel = qstate->pq_sel[6];
	w2.q6selarb = qstate->q_sel_arb[6];
	if (qstate->q_sel_arb[6] == DRR_ARB)
		w2.q6txallowed = 1;	
	
	w2.q7_sel_7b = qstate->pq_sel[7] >> 1;
	
	memcpy(&val, &w2, sizeof(u32));
	QM_DBG("Writing VQ Word 2 0x%x\n", val);
	mb_qm_wr32(qstate->ip_blk, CSR_QSTATE_WR_2_ADDR, val);

	/* word3 */
	w3.q7_sel_1b = qstate->pq_sel[7] & 1;
	w3.q7selarb = qstate->q_sel_arb[7];
	if (qstate->q_sel_arb[7] == DRR_ARB)
		w3.q7txallowed = 1;	
		
	w3.ppc_notify = qstate->ppc_notify; 
	w3.cfgnotifyqne = qstate->q_not_empty_intr;

	memcpy(&val, &w3, sizeof(u32));
	QM_DBG("Writing VQ Word 3 0x%x\n", val);
	mb_qm_wr32(qstate->ip_blk, CSR_QSTATE_WR_3_ADDR, val);

	/* copy software queue state */
	memcpy(&queue_states[qstate->q_id], qstate,
	    	   sizeof (struct mb_qm_qstate));

	if (qstate->q_sel_arb[0] == SP_ARB)
		goto exit;

	/* Update Credit Ram State for each of PQ */
	for (i = 0; i < 8; i++) {
		QM_DBG("q_sel_arb[%d] = %d, pq_sel[%d] = %d rate/weight[%d] = %d \n",
			 i, qstate->q_sel_arb[i],i,  qstate->pq_sel[i], i, qstate->shape_rate[i]);
		
		if (qstate->q_sel_arb[i] == DRR_ARB) {
			u32 credit = qstate->shape_rate[i] * DRR_CREDIT_GRAN;
			cstate[0] = credit >> 10;
			cstate[1] = ((credit & 0X3FF) << 22) |
					(credit & 0X3FFFFF );
			QM_DBG("Credit: %d \n", credit);
			QM_DBG("Writing Cstate for pq_sel[%d]:%d - cstate[0]:0x%x, cstate[1]:0x%x \n", i, qstate->pq_sel[i], cstate[0], cstate[1]);
			
		} else if (qstate->q_sel_arb[i] == AVB_ARB) {
			u32 slope = qstate->shape_rate[i];
			cstate[0] |= slope >> 4 ;
			cstate[1] |= (slope & 0xf) << 28;
			QM_DBG("Writing Cstate for pq_sel[%d]:%d - cstate[0]:0x%x, cstate[1]:0x%x \n", i, qstate->pq_sel[i], cstate[0], cstate[1]);
		} else
			continue;
		mb_qm_cstate_wr(qstate->pq_sel[i], cstate);
		cstate[0] = cstate[1] = 0;
	}
	/* Update Credit Ram State for each of PQ */
	mb_qm_cstate_wr(qstate->q_id, cstate);
	
exit:	
	return rc;
}

int mb_config_qstate(u8 port, int no_of_queues)
{
	int i;
	struct mb_qm_qstate *qstate;
	int is_fp = 0;
	int rc = 0;
	int is_vq; 

	QM_DBG("Configure Number Of Queues: %d\n", no_of_queues);
	if (port == 0) {
		/* configure QM */
		for (i = 0; i < no_of_queues; i++) {
			qstate = &mb_cfg_pqs0[i];
			if (qstate->ip_blk == IP_BLK_QML && is_noqml) {
				/* Skip QM Lite configuration if hardware not
					available */
				continue;
			}
			if ((rc = mb_qm_qstate_wr(qstate)) != 0) {
				QM_PRINT("Error in queue configuration\n");
				break;
			}
			is_fp = ((qstate->q_type == FREE_POOL) ? 1 : 0);
			is_vq = ((qstate->q_type == V_QUEUE) ? 1 : 0);
			mb_qm_pb_config(qstate->ip_blk, qstate->slave_id, qstate->pbn,
					qstate->q_id, is_fp, is_vq);
		}
	}
	else {
		/* configure QM */
		for (i = 0; i < no_of_queues; i++) {
			qstate = &mb_cfg_pqs1[i];
			if (qstate->ip_blk == IP_BLK_QML && is_noqml) {
				/* Skip QM Lite configuration if hardware not
					available */
				continue;
			}
			if ((rc = mb_qm_qstate_wr(qstate)) != 0) {
				QM_PRINT("Error in queue configuration\n");
				break;
			}
			is_fp = ((qstate->q_type == FREE_POOL) ? 1 : 0);
			is_vq = ((qstate->q_type == V_QUEUE) ? 1 : 0);
			mb_qm_pb_config(qstate->ip_blk, qstate->slave_id, qstate->pbn,
					qstate->q_id, is_fp, is_vq);
		}
	}
	
	return rc;
}

void qm_pbn_init(void)
{
	memset(&ip_to_pbn_wq, 0, sizeof (ip_to_pbn_wq));
	memset(&ip_to_pbn_fq, 0, sizeof (ip_to_pbn_fq));

	/* PPC0 */
	ip_to_pbn_wq[CORE1][IP_PPC0] = 4;
	ip_to_pbn_fq[CORE0][IP_PPC0] = 0x20;
	ip_to_pbn_fq[CORE1][IP_PPC0] = 0x22;

	/* PPC1 */
	ip_to_pbn_wq[CORE0][IP_PPC1] = 8;
	ip_to_pbn_wq[CORE1][IP_PPC1] = 12;
	ip_to_pbn_fq[CORE0][IP_PPC1] = 0x24;
	ip_to_pbn_fq[CORE1][IP_PPC1] = 0x26;

	/* DMA */
	ip_to_pbn_wq[CORE1][IP_DMA] = 2;
	ip_to_pbn_fq[CORE0][IP_DMA] = 0x20;
	ip_to_pbn_fq[CORE1][IP_DMA] = 0x22;

	/* ETH 0 */
	ip_to_pbn_wq[CORE1][IP_ETH0] = 3;
	ip_to_pbn_fq[CORE0][IP_ETH0] = 0x21; /* TBD, use 20 after model fixed */
	ip_to_pbn_fq[CORE1][IP_ETH0] = 0x24;

	/* ETH 1 */
	ip_to_pbn_wq[CORE0][IP_ETH1] = 0x8;
	ip_to_pbn_wq[CORE1][IP_ETH1] = 0xC;
	ip_to_pbn_fq[CORE0][IP_ETH1] = 0x28;
	ip_to_pbn_fq[CORE1][IP_ETH1] = 0x2C;

	/* SEC */
	ip_to_pbn_fq[CORE0][IP_SEC] = 0x20;
	ip_to_pbn_fq[CORE1][IP_SEC] = 0x22;

	/* OCMM */
	ip_to_pbn_wq[CORE1][IP_OCMM] = 1;
	ip_to_pbn_fq[CORE0][IP_OCMM] = 0x20;
	ip_to_pbn_fq[CORE1][IP_OCMM] = 0x21;

	/* CLASS */
	ip_to_pbn_wq[CORE1][IP_CLASS] = 1;
	ip_to_pbn_fq[CORE0][IP_CLASS] = 0x20;
	ip_to_pbn_fq[CORE1][IP_CLASS] = 0x21;

	/* iPP */
	ip_to_pbn_fq[CORE0][IP_IPP] = 0x20;
	ip_to_pbn_fq[CORE1][IP_IPP] = 0x22;
}

int mb_qm_alloc_mbox_ocm(void)
{
#ifdef CHECK_EMPTY_SLOT
	struct mb_qm_msg32 *mb_slot_ptr;
	struct mb_qm_msg32 *cur_slot_ptr;
	u32 *slot_ptr;
	u8 j;
#elif defined CHECK_UINFO
	u8 j;
#endif
	u8 i;

	enq_mboxes = (struct mailbox *) qm_ocm_enq_mbox_addr;
	dq_mboxes = (struct mailbox *) qm_ocm_dq_mbox_addr;
	fp_mboxes = (struct fp_mailbox *) qm_ocm_fp_mbox_addr;

#ifdef CHECK_EMPTY_SLOT
	/* assign invalid RType to all slots in all mailboxes */
	for (i = start_mboxes; i < start_mboxes + maxmboxes; i++) { 
		/* for all mailboxes */
		mb_slot_ptr = (struct mb_qm_msg32 *) &dq_mboxes[i];
		/* for all slots in given mailbox */ 
		for (j = 0; j < TOTAL_SLOTS_32BYTE_MSG; j++) { 
			cur_slot_ptr = mb_slot_ptr + j;
#ifdef INVALIDATE_MB
			/* first invalidate slot */
			invalidate_dcache_range((u32) cur_slot_ptr, (u32) (cur_slot_ptr + 8)); 
#endif
			slot_ptr = (u32 *) cur_slot_ptr;
			slot_ptr[2] = EMPTY_SLOT; /* Initially all slots are empty */
#ifdef FLUSH_MB
			/* flush msg to MEMQ*/
			flush_dcache_range((u32) cur_slot_ptr, (u32) (cur_slot_ptr + 8));
#endif
		}
	}
#endif
#ifdef CHECK_UINFO
	for (i = 0; i < DQ_MAIL_BOXES; i++) {
		for (j = 0; j < TOTAL_SLOTS_32BYTE_MSG; j++) { 
			slot_uinfo[i][j] = 0;
		}
	}
#endif

	/* init mailbox context structures */

	/*configure enqeue mailboxes */
	for (i = 0; i < ENQUE_MAIL_BOXES; i++) {
		mb_enq_mbox_ctxt[i].slot = 0;
		mb_enq_mbox_ctxt[i].total_slots = TOTAL_SLOTS;
	}

	/* configure work queue dequeue mailboxes */
	for (i = 0; i < DQ_MAIL_BOXES; i++) {
		mb_dq_mbox_ctxt[i].slot = 0;
		mb_dq_mbox_ctxt[i].total_slots = TOTAL_SLOTS;
	}

	/* configure free queue dequeue mailboxes */
	for (i = 0; i < FP_MAIL_BOXES; i++) {
		mb_fp_mbox_ctxt[i].slot = 0;
		mb_fp_mbox_ctxt[i].total_slots = TOTAL_SLOTS;
	}

	/* initialize mailbox assignments */
	for (i = 0; i < MAX_SLAVES; i++) {
		dq_fp_ip_to_mb[i] = i;
	}

	/* initialize mailbox assignments */
	for (i = 0; i < IP_MAX; i++) {
		switch(i) {
		case IP_PPC0:
			ip_to_slvid[i] = PB_SLAVE_ID_PPC;
			break;
		case IP_PPC1:
			ip_to_slvid[i] = PB_SLAVE_ID_PPC;
			break;
		case IP_ETH0:
			ip_to_slvid[i] = PB_SLAVE_ID_ETH;
			break;
		case IP_ETH1:
			ip_to_slvid[i] = PB_SLAVE_ID_ETH;
			break;
		case IP_DMA:
			ip_to_slvid[i] = PB_SLAVE_ID_DMA;
			break;
		case IP_SEC:
			ip_to_slvid[i] = PB_SLAVE_ID_SEC;
			break;
		case IP_OCMM:
			ip_to_slvid[i] = PB_SLAVE_ID_OCMM;
			break;
		case IP_CLASS:
			ip_to_slvid[i] = PB_SLAVE_ID_CLASS;
			break;
		case IP_IPP:
			ip_to_slvid[i] = PB_SLAVE_ID_IPP;
			break;
		}
	}

	return 0;
}

int mb_qm_msg_rx_unregister(u32 rtype)
{
        if (rtype >= MB_QM_MAX_RTYPE) {
                QM_PRINT("Unsupported rtype number %d\n", rtype);
                return -1;
	} else if (mb_qm_cb_fn_table[rtype] == NULL) {
                QM_PRINT("RType never registered before%d\n", rtype);
                return -1;
	}

        mb_qm_cb_fn_table[rtype] = NULL;
	return 0;
}

int mb_qm_msg_rx_register(u32 rtype, mb_qm_msg_fn_ptr fn_ptr)
{
        if (rtype >= MB_QM_MAX_RTYPE) {
                QM_PRINT("Unsupported rtype number %d\n", rtype);
                return -1;
        } else if (fn_ptr == NULL) {
                QM_PRINT("fn_ptr can't be NULL in QM callback register\n");
                return -1;
        } else if (mb_qm_cb_fn_table[rtype] != NULL) {
                QM_PRINT("rtype [%d] already registered.\n",rtype);
                return -1;
        }
        /* register call back */
        mb_qm_cb_fn_table[rtype] = fn_ptr;

        return 0;
}

/* :CD: move this to platform specific */
int config_comp_queues(void)
{
#if defined (UBOOT_HW_TEST) || defined (UBOOT_DMA_BRINGUP)
	/* No completion queues to be created in initial bring up
	   by this function */
	return 0;
#elif defined (UBOOT_ETH_BRINGUP)
	struct mb_qm_qstate *tmpst;
	int rc = 0;

	memset(comp_qstate, 0, sizeof(comp_qstate));

	/* configure Ethx to PPC0 comp queue */
	tmpst = &comp_qstate[0];

	tmpst->ip_blk = IP_BLK_QM;
	tmpst->valid = 1;
	tmpst->q_id = ETH_TO_PPC0_COMP_QID;
	tmpst->mb_id = ETH_TO_PPC0_COMP_MB;
	tmpst->slave_id = PB_SLAVE_ID_PPC;
	tmpst->pbn = ETH_TO_PPC0_COMP_MB;
	tmpst->q_type = P_QUEUE;
	tmpst->msg_size = MSG_32B;
	tmpst->q_size = SIZE_16KB;
	tmpst->thr_set = 1;

	if ((rc = mb_qm_qstate_wr(tmpst)) != 0) {
		QM_PRINT("Error in queue configuration\n");
	} 

	if ((rc = mb_qm_pb_config(tmpst->ip_blk, tmpst->slave_id,
				  tmpst->pbn, tmpst->q_id, 0, 0)) != 0) {
		QM_PRINT("Error in PBN configuration line: %d\n", __LINE__);
	}
	return rc;
#else
	struct mb_qm_qstate *tmpst;
	int rc = 0;

	memset(comp_qstate, 0, sizeof(comp_qstate));

	/* configure Ethx to PPC0 comp queue */
	tmpst = &comp_qstate[0];

	tmpst->src_ip_blk = IP_PPC0;
	tmpst->ip_blk = IP_BLK_QM;
	tmpst->ppc_id = IP_PPC0;
	tmpst->valid = 1;
	tmpst->q_id = ETH_TO_PPC0_COMP_QID;
	tmpst->mb_id = ETH_TO_PPC0_COMP_MB;
	tmpst->slave_id = PB_SLAVE_ID_PPC;
	tmpst->pbn = ETH_TO_PPC0_COMP_MB;
	tmpst->q_type = P_QUEUE;
	tmpst->msg_size = MSG_32B;
	tmpst->q_size = SIZE_16KB;
	tmpst->thr_set = 1;

	/* Configure only completion assigned to this system */
	if (is_smp || mfspr(SPRN_PIR) == 0) {
		if ((rc = mb_qm_qstate_wr(tmpst)) != 0) {
			QM_PRINT("Error in queue configuration\n");
		} 

		if ((rc = mb_qm_pb_config(tmpst->ip_blk, tmpst->slave_id,
			     	tmpst->pbn, tmpst->q_id, 0, 0)) != 0) {
			QM_PRINT("Error in PBN configuration line: %d\n", 
				__LINE__);
		}
	}		
	/* configure Ethx to PPC1 comp queue */
	tmpst = &comp_qstate[1];

	tmpst->src_ip_blk = IP_PPC1;
	tmpst->ip_blk = IP_BLK_QM;
	tmpst->ppc_id = IP_PPC1;
	tmpst->valid = 1;
	tmpst->q_id = ETH_TO_PPC1_COMP_QID;
	tmpst->mb_id = ETH_TO_PPC1_COMP_MB;
	tmpst->slave_id = PB_SLAVE_ID_PPC;
	tmpst->pbn = ETH_TO_PPC1_COMP_MB;
	tmpst->q_type = P_QUEUE;
	tmpst->msg_size = MSG_32B;
	tmpst->q_size = SIZE_16KB;
	tmpst->thr_set = 1;

	/* Configure only completion assigned to this system */
	if (is_smp || mfspr(SPRN_PIR) == 1) {
		if ((rc = mb_qm_qstate_wr(tmpst)) != 0) {
			QM_PRINT("Error in queue configuration\n");
		} 

		if ((rc = mb_qm_pb_config(tmpst->ip_blk, tmpst->slave_id,
			     	tmpst->pbn, tmpst->q_id, 0, 0)) != 0) {
			QM_PRINT("Error in PBN configuration line: %d\n", 
				__LINE__);
		}
	}		
	/* configure general IP blocks to PPC0 comp queue */
	tmpst = &comp_qstate[2];

	tmpst->src_ip_blk = IP_PPC0;
	tmpst->ip_blk = IP_BLK_QM;
	tmpst->ppc_id = IP_PPC0;
	tmpst->valid = 1;
	tmpst->q_id = GEN_TO_PPC0_COMP_QID;
	tmpst->mb_id = GEN_TO_PPC0_COMP_MB;
	tmpst->slave_id = PB_SLAVE_ID_PPC;
	tmpst->pbn = GEN_TO_PPC0_COMP_MB;
	tmpst->q_type = P_QUEUE;
	tmpst->msg_size = MSG_32B;
	tmpst->q_size = SIZE_16KB;
	tmpst->thr_set = 1;

	/* Configure only completion assigned to this system */
	if (is_smp || mfspr(SPRN_PIR) == 0) {
		if ((rc = mb_qm_qstate_wr(tmpst)) != 0) {
			QM_PRINT("Error in queue configuration\n");
		} 
	
		if ((rc = mb_qm_pb_config(tmpst->ip_blk, tmpst->slave_id,
			     	tmpst->pbn, tmpst->q_id, 0, 0)) != 0) {
			QM_PRINT("Error in PBN configuration line: %d\n", 
				__LINE__);
		}
	}		
	/* configure general IP blocks to PPC1 comp queue */
	tmpst = &comp_qstate[3];

	tmpst->src_ip_blk = IP_PPC1;
	tmpst->ip_blk = IP_BLK_QM;
	tmpst->ppc_id = IP_PPC1;
	tmpst->valid = 1;
	tmpst->q_id = GEN_TO_PPC1_COMP_QID;
	tmpst->mb_id = GEN_TO_PPC1_COMP_MB;
	tmpst->slave_id = PB_SLAVE_ID_PPC;
	tmpst->pbn = GEN_TO_PPC1_COMP_MB;
	tmpst->q_type = P_QUEUE;
	tmpst->msg_size = MSG_32B;
	tmpst->q_size = SIZE_16KB;
	tmpst->thr_set = 1;

	/* Configure only completion assigned to this system */
	if (is_smp || mfspr(SPRN_PIR) == 1) {
		if ((rc = mb_qm_qstate_wr(tmpst)) != 0) {
			QM_PRINT("Error in queue configuration\n");
		} 
	
		if ((rc = mb_qm_pb_config(tmpst->ip_blk, tmpst->slave_id,
					  tmpst->pbn, tmpst->q_id, 0, 0)) != 0) {
			QM_PRINT("Error in PBN configuration line: %d\n", __LINE__);
		}
	}		
	return rc;
#endif
}

int mb_qm_init(u8 port, int no_of_queues)
{
	u32 i;
	int rc = 0;

	/* QM INIT */
	for (i = 0; i < MB_MAX_QUEUES; i++) {
		queue_states[i].q_id = i;
		pbn_valid[i] = 0;
		q_to_mb[i] = 0;
		mb_to_q[i] = 0;
	}

	qm_pbn_init();
	config_comp_queues();

	/* configure queue descriptors for given queues */
	if ((rc = mb_config_qstate(port, no_of_queues)) != 0) {
		QM_PRINT("Error in qm_init in config queue database\n");
		return rc;
	}

#ifdef CONFIG_MB_OCM
	/* move mailboxes to OCM */
	mb_qm_wr32(IP_BLK_QM, CSR_ENQ_BASE_A_ADDR, 0x3bffff0);
	mb_qm_wr32(IP_BLK_QM, CSR_PPC_MBOX_BASE_ADDR, 0xeffffd);
	mb_qm_wr32(IP_BLK_QM, CSR_PPC_FPOOL_BASE_ADDR, 0xeffffe);
#elif defined (CONFIG_MB_FAM)
	/* move mailboxes to FAM */
	mb_qm_wr32(IP_BLK_QM, CSR_ENQ_BASE_A_ADDR, 0x1000000);
	mb_qm_wr32(IP_BLK_QM, CSR_PPC_MBOX_BASE_ADDR, 0x400001);
	mb_qm_wr32(IP_BLK_QM, CSR_PPC_FPOOL_BASE_ADDR, 0x400002);
#endif

#if defined(CONFIG_COHERENT_MB)
	/* enable coherent read in  mailboxes */
	mb_qm_rd32(IP_BLK_QM, CSR_ENQ_BASE_A_ADDR, &val);
	val |= 0x80000000;
	mb_qm_wr32(IP_BLK_QM, CSR_ENQ_BASE_A_ADDR, val);

	mb_qm_rd32(IP_BLK_QM, CSR_PPC_MBOX_BASE_ADDR, &val);
	val |= 0x80000000;
	mb_qm_wr32(IP_BLK_QM, CSR_PPC_MBOX_BASE_ADDR, val);

	mb_qm_rd32(IP_BLK_QM, CSR_PPC_FPOOL_BASE_ADDR, &val);
	val |= 0x80000000;
	mb_qm_wr32(IP_BLK_QM, CSR_PPC_FPOOL_BASE_ADDR, val);
#endif

	/* Enable QPcore and assign error queue from Core 0 */
	if (is_smp || mfspr(SPRN_PIR) == 0)
		if ((rc = mb_qm_init_errq(0)) != 0)
			QM_PRINT("Error in mb_qm_init_errq\n");

	return rc;
}

struct mb_qm_qstate *get_comp_queue(int ip_blk, int ppc_id)
{
	if (ip_blk != IP_ETH0 && ip_blk != IP_ETH1) {
		return (&comp_qstate[(2 + ppc_id)]);
	} else {
		return (&comp_qstate[ppc_id]);
	}
}

int apm_qm_alloc_q(struct apm_qm_qalloc *qalloc)
{
	int rc = 0;
	struct mb_qm_qstate *qstate = qalloc->qstates;
	u32 i;
	u32 qid = start_qid;
	int is_fp = 0;

		start_qid += qalloc->q_count;

	if (start_qid > max_qid) {
		QM_PRINT("QM queues not available\n");
		QM_PRINT("%d %s\n", __LINE__, __FILE__);
		return -1;
	} else if ((max_qid - start_qid) < qalloc->q_count) {
		QM_PRINT("Not enough QM queues available\n");
		QM_PRINT("Asking %d, available %d\n", qalloc->q_count, 
							(max_qid - start_qid));
		QM_PRINT("%d %s\n", __LINE__, __FILE__);
		return -1;
	}

	/* configure queue state */
	for (i = 0; i < qalloc->q_count; i++) {
		qstate->src_ip_blk = qalloc->ip_blk;
		qstate->ip_blk = qalloc->qm_ip_blk;
		qstate->ppc_id = qalloc->ppc_id;
		qstate->valid = 1;
		qstate->q_type = qalloc->q_type;
		qstate->q_size = qalloc->qsize;
		qstate->thr_set = qalloc->thr_set;
		qstate->direction = qalloc->direction;
		qstate->q_id = qid++;
		qstate->parent_vqid = qalloc->parent_vq;
		if (qalloc->vqen == ENABLE_VQ) {
			qstate->vqen = qalloc->vqen;
			qstate->parent_vqid = qalloc->parent_vq;
		}
#ifdef QM_DEBUG
		if (qstate->vqen)
			QM_PRINT("***qstate->parent_vqid: %d \n",
				 qstate->parent_vqid);
#endif
		/* mailbox assignment */
		if (qalloc->direction) { /* Egress direction */
			/* Use CPU ID as mailbox ID */
			if (qalloc->ip_blk == IP_DMA)
				/* FIXME - temporary hard assignment */
				qstate->mb_id = qalloc->ppc_id * 8 + 1;
			else if (qalloc->ip_blk == IP_SEC)
				/* FIXME - temporary hard assignment */
				qstate->mb_id = qalloc->ppc_id * 8 + 2;
			else
				qstate->mb_id = qalloc->ppc_id * 8;
			if (qstate->vqen != ENABLE_VQ)
				qstate->slave_id = ip_to_slvid[qalloc->ip_blk];
		} else { /* Ingress direction */
			/* if IP block is Security, Classifier or OCMM,
			   use general mailbox (6 or 14 as per core ID) */
			if (qalloc->ip_blk == IP_SEC || 
					qalloc->ip_blk == IP_OCMM ||
					qalloc->ip_blk == IP_CLASS) {
				qstate->mb_id = 8 * qalloc->ppc_id +
							ING_GEN_WQUEUE_MB;
			} else if (qalloc->ip_blk == IP_IPP) {
				 qstate->mb_id = 0;
			} else {
				qstate->mb_id = 8 * qalloc->ppc_id + 
							qalloc->ip_blk;
			}
			if (qstate->vqen != ENABLE_VQ) {
				if (qalloc->ip_blk == IP_IPP)
					qstate->slave_id = PB_SLAVE_ID_IPP;
				else
				qstate->slave_id = PB_SLAVE_ID_PPC;
		}
		}
		/* Message size */
		if (qalloc->ip_blk == IP_IPP) {
			if (qalloc->q_type == FREE_POOL) {
				qstate->msg_size = MSG_16B;
			} else {
				qstate->msg_size = MSG_32B;
			}
		}
		/* Prefetch buffer number assignment */
		if (qalloc->q_type == FREE_POOL) {
			if (qalloc->ip_blk == IP_PPC0) {
				qstate->pbn = 0x20;
			} else if (qalloc->ip_blk == IP_PPC1) {
				qstate->pbn = 0x21;
			} else {
				qstate->pbn = (ip_to_pbn_fq[qalloc->ppc_id][qalloc->ip_blk])++;
			}
			is_fp = 1;
		} else if (qstate->vqen != ENABLE_VQ) {
			if (qalloc->direction) /* Egress direction */ 
				qstate->pbn = (ip_to_pbn_wq[qalloc->ppc_id][qalloc->ip_blk])++;
			else if (qalloc->ip_blk == IP_IPP)
				qstate->pbn = 0;
			else /* PBN is same as MB */
				qstate->pbn = qstate->mb_id;
		}

		/* PPC is enqueuing and wants to check fill status with interrupt */
		if (qalloc->direction && qalloc->en_fill_chk) {
			qstate->ppc_notify = qalloc->ppc_id + 1; /* trigger interrupt to this PPC */
			qstate->cfgsaben = 1;
		}

		if ((rc = mb_qm_qstate_wr(qstate)) != 0) {
			QM_PRINT("Error in queue configuration\n");
			break;
		}
		
		QM_DBG("PQueue configuration\n");
		QM_DBG("IP blk: %d\n", qstate->ip_blk);
		QM_DBG("Slave ID: %d\n", qstate->slave_id);
		QM_DBG("PBN: %d\n", qstate->pbn);
		QM_DBG("Queue ID: %d\n", qstate->q_id);
		QM_DBG("MB ID: %d\n", qstate->mb_id);
		QM_DBG("Queue Type: %d\n", qstate->q_type);
		
		if ((rc = mb_qm_pb_config(qstate->ip_blk, qstate->slave_id,
					  qstate->pbn, qstate->q_id, is_fp, 0)) != 0) {
			QM_PRINT("Error in PBN configuration\n");
			break;
		}
		
		qstate++;
	}
	
	return rc;
}

int apm_qm_get_vq(u32 ip_blk)
{
	u32 qid = start_qid;
	
	++start_qid;

	if (start_qid > max_qid) {
		QM_PRINT("QM queues not available\n");
		QM_PRINT("%d %s\n", __LINE__, __FILE__);
		return -1;
	}

	return qid;
}

int apm_qm_alloc_vq(struct apm_qm_qalloc *qalloc, u32 qid)
{
	int rc = 0;
	struct mb_qm_qstate *qstate = qalloc->qstates;
	int j;
	int is_fp = 0;

	if (qid > max_qid || qalloc->q_count != 1) {
		QM_PRINT("QM queues not available\n");
		QM_PRINT("%d %s\n", __LINE__, __FILE__);
		return -1;
	} else if (qalloc->q_type != V_QUEUE) {
		QM_PRINT("Invalid parameter for VQ config \n");
		return -1;
	}

	/* configure queue state */
	qstate->src_ip_blk = qalloc->ip_blk;
	qstate->ip_blk = qalloc->qm_ip_blk;
	qstate->ppc_id = qalloc->ppc_id;
	qstate->valid = 1;
	qstate->q_type = qalloc->q_type;
	qstate->q_size = qalloc->qsize;
	qstate->thr_set = qalloc->thr_set;
	qstate->direction = qalloc->direction;
	qstate->q_id = qid;
	qstate->parent_vqid = qalloc->parent_vq;
	qstate->vqen = qalloc->vqen;
	
	memcpy(qstate->pq_sel, qalloc->pq_sel,
	       sizeof(qstate->pq_sel));
	memcpy(qstate->q_sel_arb, qalloc->q_sel_arb,
	       sizeof(qstate->q_sel_arb));
	memcpy(qstate->shape_rate, qalloc->shape_rate,
	       sizeof(qstate->shape_rate));
		
	/* mailbox assignment */
	if (qalloc->direction) { /* Egress direction */
		/* Use CPU ID as mailbox ID */
		if (qalloc->ip_blk == IP_DMA)
			/* FIXME - temporary hard assignment */
			qstate->mb_id = qalloc->ppc_id * 8 + 1;
		else if (qalloc->ip_blk == IP_SEC)
			/* FIXME - temporary hard assignment */
			qstate->mb_id = qalloc->ppc_id * 8 + 2;
		else
			qstate->mb_id = qalloc->ppc_id * 8;
			qstate->slave_id = ip_to_slvid[qalloc->ip_blk];
	} else { /* Ingress direction */
		/* if IP block is Security, Classifier or OCMM,
		   use general mailbox (6 or 14 as per core ID) */
		if (qalloc->ip_blk == IP_SEC || 
		    qalloc->ip_blk == IP_OCMM ||
		    qalloc->ip_blk == IP_CLASS) {
			qstate->mb_id = 8 * qalloc->ppc_id +
				ING_GEN_WQUEUE_MB;
		} else {
			qstate->mb_id = 8 * qalloc->ppc_id + 
				qalloc->ip_blk;
		}
		qstate->slave_id = PB_SLAVE_ID_PPC;
	}
	
	/* Prefetch buffer number assignment */
	if (qalloc->q_type == FREE_POOL) {
		if (qalloc->ip_blk == IP_PPC0) {
			qstate->pbn = 0x20;
		} else if (qalloc->ip_blk == IP_PPC1) {
			qstate->pbn = 0x21;
		} else {
			qstate->pbn = (ip_to_pbn_fq[qalloc->ppc_id]
				       [qalloc->ip_blk])++;
		}
		is_fp = 1;
	} else {
		if (qalloc->direction) /* Egress direction */ 
			qstate->pbn = (ip_to_pbn_wq[qalloc->ppc_id][qalloc->ip_blk])++;
		else /* PBN is same as MB */
			qstate->pbn = qstate->mb_id;
	}
	
	if ((rc = mb_qm_vqstate_wr(qstate)) != 0) {
		QM_PRINT("Error in queue configuration\n");
		return rc;
	}
	
	QM_DBG("Virtual Queue configuration\n");
	QM_DBG("IP blk: %d\n", qstate->ip_blk);
	QM_DBG("Slave ID: %d\n", qstate->slave_id);
	QM_DBG("PBN: %d\n", qstate->pbn);
	QM_DBG("Queue ID: %d\n", qstate->q_id);
	QM_DBG("MB ID: %d\n", qstate->mb_id);
	QM_DBG("Queue Type: %d\n", qstate->q_type);
	
	for (j = 0; j < 8; j++)
		QM_DBG("PQ_SEL[%d]= %d, Q_SEL_ARB[%d] = %d and RATE[%d] = %d\n",
			 j, qstate->pq_sel[j],j,
			 qstate->q_sel_arb[j], j, qstate->shape_rate[j]);
	
	if ((rc = mb_qm_pb_config(qstate->ip_blk, qstate->slave_id,
				  qstate->pbn, qstate->q_id, is_fp, 1)) != 0) {
		QM_PRINT("Error in PBN configuration\n");
		return rc;
	}
	return rc;
}

inline u32 mb_qm_pull_comp_msg(u32 mb_id)
{
	u32 temp;
	u32 *slot_ptr;
	struct mb_qm_msg16 *msg16;
	int NV; 
#ifdef CHECK_MB_STATUS
	u32 new_msg_status;
#endif

	msg16 = (struct mb_qm_msg16 *) 
		((struct mb_qm_msg32 *) &dq_mboxes[mb_id] +
		 mb_dq_mbox_ctxt[mb_id].slot);
	slot_ptr = (u32 *) msg16;

#ifdef CHECK_MB_STATUS
	/* Make sure there is a message to read */
	mb_qm_rd32(IP_BLK_QM, CSR_QM_MBOX_NE_ADDR, &new_msg_status);
	if (!(new_msg_status & (1 << (31 - mb_id)))) {
		QM_DBG("No msg to dequeue from queue: %d\n", msg_desc->qid);
		return -1;
	}
#endif

#ifdef INVALIDATE_MB
	invalidate_dcache_range((u32) slot_ptr, (u32) (slot_ptr + 32) - 1); 
#endif

#ifdef CHECK_EMPTY_SLOT
	/* Make sure there is a message to read */
	if (slot_ptr[2] == EMPTY_SLOT) {
		/* message slot is empty */
		QM_DBG("No msg to dequeue from MB: %d\n", mb_id);
		return -1;
	}
#endif

#ifdef CHECK_UINFO
	/* Make sure there is a message to read */
	if (slot_ptr[1] == slot_uinfo[mb_id][mb_dq_mbox_ctxt[mb_id].slot]) {
		/* message slot is empty */
		QM_DBG("No msg to dequeue from MB: %d\n", mb_id);
		return -1;
	} 
#endif
	temp = (u32) msg16->UserInfo;
	NV = msg16->NV;
	
	QM_DBG("Found new msg slot: %d\n", mb_dq_mbox_ctxt[mb_id].slot);
					
#ifdef CHECK_EMPTY_SLOT
	slot_ptr[2] = EMPTY_SLOT;
#endif

#ifdef CHECK_UINFO
	slot_uinfo[mb_id][mb_dq_mbox_ctxt[mb_id].slot] = slot_ptr[1];
#endif

#if defined FLUSH_MB && defined CHECK_EMPTY_SLOT
	flush_dcache_range((u32) slot_ptr, (u32) (slot_ptr + 8) - 1); 
#endif
	mb_dq_mbox_ctxt[mb_id].slot++;
	if (mb_dq_mbox_ctxt[mb_id].slot == mb_dq_mbox_ctxt[mb_id].total_slots)
		mb_dq_mbox_ctxt[mb_id].slot = 0;

	if (!NV) {
		/* decrement number of messages in this slave id and PBN */
		*(u32 *) QM_B_ADDR_DEC = QM_DEC_MSG_VAL(mb_id, 1);
	} else {
		msg16 = (struct mb_qm_msg16 *) 
			((struct mb_qm_msg32 *) &dq_mboxes[mb_id] +
			 mb_dq_mbox_ctxt[mb_id].slot);
		slot_ptr = (u32 *) msg16;
#ifdef CHECK_EMPTY_SLOT
		slot_ptr[2] = EMPTY_SLOT;
#endif
#if defined FLUSH_MB && defined CHECK_EMPTY_SLOT
		flush_dcache_range((u32) slot_ptr, (u32) (slot_ptr + 8) - 1); 
#endif
		mb_dq_mbox_ctxt[mb_id].slot++;
		if (mb_dq_mbox_ctxt[mb_id].slot == mb_dq_mbox_ctxt[mb_id].total_slots)
			mb_dq_mbox_ctxt[mb_id].slot = 0;
		/* decrement number of messages in this slave id and PBN */
		*(u32 *) QM_B_ADDR_DEC = QM_DEC_MSG_VAL(mb_id, 2);
	}

	return temp;
}

int mb_qm_pull_msg(struct mb_qm_msg_desc *msg_desc)
{
	u8 mb_id = msg_desc->mb_id;
	u32 *temp;
	u32 *slot_ptr;
	struct mb_qm_msg32 *mb_slot_ptr;
	struct mb_qm_msg16 *msg16; 

#ifdef CHECK_MB_STATUS
	u32 new_msg_status;
#endif
	mb_slot_ptr = (struct mb_qm_msg32 *) &dq_mboxes[mb_id];
	mb_slot_ptr += mb_dq_mbox_ctxt[mb_id].slot;
	msg16 = &mb_slot_ptr->msg16; 
	slot_ptr = (u32 *) mb_slot_ptr;

#ifdef CHECK_MB_STATUS
	/* Make sure there is a message to read */
	mb_qm_rd32(IP_BLK_QM, CSR_QM_MBOX_NE_ADDR, &new_msg_status);
	if (!(new_msg_status & (1 << (31 - mb_id)))) {
		QM_DBG("No msg to dequeue from queue: %d\n", msg_desc->qid);
		return -1;
	}
#endif

#ifdef INVALIDATE_MB
	invalidate_dcache_range((u32) slot_ptr, (u32) (slot_ptr + 32) - 1); 
#endif

#ifdef CHECK_UINFO
	/* Make sure there is a message to read */
	if (slot_ptr[1] == slot_uinfo[mb_id][mb_dq_mbox_ctxt[mb_id].slot]) {
		/* message slot is empty */
		QM_DBG("No msg to dequeue from MB: %d\n", mb_id);
		return -1;
	} 
#endif
#ifdef CHECK_EMPTY_SLOT
	if (*((u32 *) ((u32 *) slot_ptr) + 2) == EMPTY_SLOT) {
		/* message slot is empty */
		QM_DBG("No msg to dequeue from queue %d\n", msg_desc->qid);
		return -1;
	}
#endif

	QM_DBG("Found new msg slot: %d\n", 
		mb_dq_mbox_ctxt[mb_id].slot);

	temp = (u32 *) msg_desc->msg;

	/* read message from the current slot */
	if (msg_desc->is_msg16) 
		memcpy(temp, slot_ptr, 16);
	else
		memcpy(temp, slot_ptr, 32);

#ifdef CHECK_UINFO
	slot_uinfo[mb_id][mb_dq_mbox_ctxt[mb_id].slot] = slot_ptr[1];
#endif

	mb_dq_mbox_ctxt[mb_id].slot++;
	if (mb_dq_mbox_ctxt[mb_id].slot == mb_dq_mbox_ctxt[mb_id].total_slots)
		mb_dq_mbox_ctxt[mb_id].slot = 0;

#ifdef CHECK_EMPTY_SLOT
	*((u32 *) ((u32 *) msg16) + 2) = EMPTY_SLOT;
#endif

#if defined FLUSH_MB && defined CHECK_EMPTY_SLOT
	flush_dcache_range((u32) slot_ptr, (u32) (slot_ptr + 4) - 1);
#endif

	/* decrement number of messages in this slave id and PBN */
	/* if message is only 16 bytes, then no need to process for 64 bytes */
	if (msg_desc->is_msg16) {
		*(u32 *) QM_B_ADDR_DEC = QM_DEC_MSG_VAL(mb_id, 1);
		return 0;
	}

	/* if the message is 64 bytes, then read remaining 32 bytes */
	if (!((struct mb_qm_msg16 *) msg_desc->msg)->NV) {
		*(u32 *) QM_B_ADDR_DEC = QM_DEC_MSG_VAL(mb_id, 1);
	} else {
		mb_slot_ptr = (struct mb_qm_msg32 *) 
					&dq_mboxes[mb_id];
		mb_slot_ptr += mb_dq_mbox_ctxt[mb_id].slot;
		msg16 = &mb_slot_ptr->msg16; 
		slot_ptr = (u32 *) mb_slot_ptr;
#ifdef INVALIDATE_MB
		invalidate_dcache_range((u32) slot_ptr, 
					(u32) (slot_ptr + 8) - 1); 
#endif
		temp += 8;
		memcpy(temp, slot_ptr, 32);
		mb_dq_mbox_ctxt[mb_id].slot++;
		if (mb_dq_mbox_ctxt[mb_id].slot == mb_dq_mbox_ctxt[mb_id].total_slots)
			mb_dq_mbox_ctxt[mb_id].slot = 0;

#ifdef CHECK_EMPTY_SLOT
		*((u32 *) ((u32 *) msg16) + 2) = EMPTY_SLOT;
#endif
#if defined FLUSH_MB && defined CHECK_EMPTY_SLOT
	flush_dcache_range((u32) slot_ptr, (u32) (slot_ptr + 4) - 1);
#endif
		QM_DBG("Got 64 byte msg\n");
		*(u32 *) QM_B_ADDR_DEC = QM_DEC_MSG_VAL(mb_id, 2);
	}

	QM_DBG("Decrement number of messages: %p, val: %x\n", 
		(u32 *) QM_B_ADDR_DEC, *(u32 *) QM_B_ADDR_DEC);
	return 0;
}

int mb_qml_push_msg(struct mb_qm_msg_desc *msg_desc)
{
	return 0;
}

int mb_qm_push_msg(struct mb_qm_msg_desc *msg_desc)
{
	u32 *qbase_addr_ptr;
	u32 mb_desc;
	u32 *temp;
	u32 *slot_ptr;
	u8 mb_id = GET_MB_FROM_QUEUE(msg_desc->qid);
	u32 cur_slot;
	u32 init_slot;
#if defined (CONFIG_MB_CHECK_TX_MSG)
	/* check mailbox status after enq */
	u32 csr_enq_status;
	u32 val;
	u32 status_byte; 
	u32 st_byte; 
	int wait_required;
#endif

	struct mb_qm_msg32 *mb_slot_ptr;
	struct mb_qm_msg32 *msg = (struct mb_qm_msg32 *) msg_desc->msg;

	temp = (u32 *) msg_desc->msg;
	cur_slot = mb_enq_mbox_ctxt[mb_id].slot;
	QM_DBG("Push message QID %d mailbox %d slot %d\n",
		msg_desc->qid, mb_id, cur_slot);
        
#ifdef CONFIG_MB_CHECK_TX_MSG
	if (msg->msg16.NV &&
	    cur_slot == (mb_enq_mbox_ctxt[mb_id].total_slots - 1))
		wait_required = 1;
	else
		wait_required = 0;		
			
	if (cur_slot == 0 || wait_required) {
		st_byte = mb_id % 4;
		/* wait for QM to be done reading the message from mailbox */
		if (mb_id < 4)
			csr_enq_status = CSR_ENQ_STATUS_0_ADDR;
		else if (mb_id < 8) 
			csr_enq_status = CSR_ENQ_STATUS_1_ADDR;
		else if (mb_id < 12)
			csr_enq_status = CSR_ENQ_STATUS_2_ADDR;
		else if (mb_id < 16)
			csr_enq_status = CSR_ENQ_STATUS_3_ADDR;
		else {
			QM_DBG("Invalid MBID in QM push message\n");
			return -1;
		}

		while (1) {
			mb_qm_rd32(IP_BLK_QM, csr_enq_status, &val);
			status_byte = val >> ((3 - st_byte) * 8);
			if (cur_slot == 0) {
				if (!((status_byte >> (7 - 7)) & 0x1))
					break;
			} else {
				if (!((status_byte >> (7 - 6)) & 0x1))
					break;
			}
		}
	}
#endif

	init_slot = cur_slot;
	mb_slot_ptr = (struct mb_qm_msg32 *) &enq_mboxes[mb_id];
	mb_slot_ptr += cur_slot;
	slot_ptr = (u32 *) mb_slot_ptr;

	/* write message in the current slot */
	memcpy(slot_ptr, temp, 32);

#ifdef FLUSH_MB
	flush_dcache_range((u32) mb_slot_ptr, (u32) (mb_slot_ptr + 8) - 1);
#endif

	/* adjust current slot */
	mb_enq_mbox_ctxt[mb_id].slot++;
	if (mb_enq_mbox_ctxt[mb_id].slot == mb_enq_mbox_ctxt[mb_id].total_slots)
		mb_enq_mbox_ctxt[mb_id].slot = 0;

	/* prepare qm base address to write to */
	qbase_addr_ptr = (u32 *) QM_B_ADDR_QID(msg_desc->qid);
	QM_DBG("qbase_addr_ptr after: %p\n", qbase_addr_ptr);

	if (!msg->msg16.NV) {
		/* prepare data to write to qm base address */
		mb_desc = (mb_id << APM_QM_MBID_SHIFT) | 
				(init_slot << APM_QM_SLOTID_SHIFT) | 
				SIZE_32_MSG;
	} else {
		cur_slot = mb_enq_mbox_ctxt[mb_id].slot;
		mb_slot_ptr = (struct mb_qm_msg32 *) &enq_mboxes[mb_id];
		mb_slot_ptr += cur_slot;
		slot_ptr = (u32 *) mb_slot_ptr;

		mb_enq_mbox_ctxt[mb_id].slot++;
		if (mb_enq_mbox_ctxt[mb_id].slot == mb_enq_mbox_ctxt[mb_id].total_slots)
			mb_enq_mbox_ctxt[mb_id].slot = 0;
		
		/* write remaining 32 byte message in the next slot */
		temp += 8;
		memcpy(slot_ptr, temp, 32);
#ifdef FLUSH_MB
		flush_dcache_range((u32) mb_slot_ptr, 
				(u32) (mb_slot_ptr + 8) - 1);
#endif
		/* prepare data to write to qm base address */
		mb_desc = (mb_id << APM_QM_MBID_SHIFT) | 
				(init_slot << APM_QM_SLOTID_SHIFT) | 
				SIZE_64_MSG;
	}
	QM_DBG("Notify QM HW new msg base addr 0x%p val 0x%x\n",
		qbase_addr_ptr, mb_desc);
	*qbase_addr_ptr = mb_desc; 
	return 0;
}

int mb_qml_fp_dealloc_buf(struct mb_qm_msg_desc *msg_desc)
{
	u32 *qml_base_addr_ptr;
	u32 mb_desc = 0;
	u32 *temp;
	u32 *slot_ptr;
	u8 mb_id = QML_FREE_Q_DEALLOC_MB;
	struct mb_qm_msg32 *mb_slot_ptr;

	/* Check if QML hardware available */
	if (is_noqml)
		return -1;

	temp = (u32 *) msg_desc->msg;

	mb_slot_ptr = (struct mb_qm_msg32 *) &enq_mboxes[mb_id];
	mb_slot_ptr += mb_enq_mbox_ctxt[mb_id].slot;
	slot_ptr = (u32 *) mb_slot_ptr;

	/* write message in the current slot */
	*slot_ptr++ = *temp++; *slot_ptr++ = *temp++;
	*slot_ptr++ = *temp++; *slot_ptr++ = *temp++;

        /* prepare qm base address to write to */
	QM_DBG("msg_desc->qid: %x\n", msg_desc->qid);
	
	qml_base_addr_ptr = (u32 *) QML_B_ADDR_QID(msg_desc->qid);

	QM_DBG("qbase_addr_ptr after: %p\n", qml_base_addr_ptr);

	/* prepare data to write to qm base address */
	mb_desc = (mb_id << APM_QM_MBID_SHIFT) | 
			(mb_enq_mbox_ctxt[mb_id].slot << APM_QM_SLOTID_SHIFT) |
								 SIZE_16_MSG;
	mb_enq_mbox_ctxt[mb_id].slot++;
	if (mb_enq_mbox_ctxt[mb_id].slot == mb_enq_mbox_ctxt[mb_id].total_slots)
		mb_enq_mbox_ctxt[mb_id].slot = 0;

	QM_DBG("Letting QM know about dealloc message, QM base addr: %p, \
					val: %x\n", qml_base_addr_ptr, mb_desc);

	*qml_base_addr_ptr = mb_desc;

	return 0;
}

int mb_qm_fp_dealloc_buf(struct mb_qm_msg_desc *msg_desc)
{
	u32 *qbase_addr_ptr;
	u32 mb_desc = 0;
	u32 *temp;
	u32 *slot_ptr;
	struct mb_qm_msg32 *mb_slot_ptr;
	u8 mb_id = GET_MB_FROM_QUEUE(msg_desc->qid);

#if defined (CONFIG_MB_CHECK_TX_MSG)
	/* check mailbox status after enq */
	u32 csr_enq_status = 0;
	u32 val;
	u32 status_byte; 
	u32 st_byte; 
#endif
	
	temp = (u32 *) msg_desc->msg;

	mb_slot_ptr = (struct mb_qm_msg32 *) &enq_mboxes[mb_id];
	mb_slot_ptr += mb_enq_mbox_ctxt[mb_id].slot;
	slot_ptr = (u32 *) mb_slot_ptr;

#ifdef CONFIG_MB_CHECK_TX_MSG
	if (mb_enq_mbox_ctxt[mb_id].slot == 0) {
		st_byte = mb_id % 4;
		/* wait for QM to be done reading the message from mailbox */
		if (mb_id < 4)
			csr_enq_status = CSR_ENQ_STATUS_0_ADDR;
		else if (mb_id < 8) 
			csr_enq_status = CSR_ENQ_STATUS_1_ADDR;
		else if (mb_id < 12)
			csr_enq_status = CSR_ENQ_STATUS_2_ADDR;
		else if (mb_id < 16)
			csr_enq_status = CSR_ENQ_STATUS_3_ADDR;

		while (1) {
			mb_qm_rd32(IP_BLK_QM, csr_enq_status, &val);
			status_byte = val >> ((3 - st_byte) * 8);
			if (!(status_byte & 0x1))
				break;
		}
	}
#endif
	/* write message in the current slot */
	memcpy(slot_ptr, temp, 16);

#ifdef FLUSH_MB
	/* flush msg to MEMQ*/
	flush_dcache_range((u32) mb_slot_ptr, (u32) (mb_slot_ptr + 4));
#endif
        /* prepare qm base address to write to */
	qbase_addr_ptr = (u32 *) QM_B_ADDR_QID(msg_desc->qid);
	/* prepare data to write to qm base address */
	mb_desc = (mb_id << APM_QM_MBID_SHIFT) | 
			(mb_enq_mbox_ctxt[mb_id].slot << APM_QM_SLOTID_SHIFT) |
								 SIZE_16_MSG;
	QM_DBG("Notify QM dealloc msg QID %d QM base addr %p val %x\n", 
		msg_desc->qid, qbase_addr_ptr, mb_desc);

	*qbase_addr_ptr = mb_desc;

	mb_enq_mbox_ctxt[mb_id].slot++;
	if (mb_enq_mbox_ctxt[mb_id].slot == mb_enq_mbox_ctxt[mb_id].total_slots)
		mb_enq_mbox_ctxt[mb_id].slot = 0;

	return 0;
}

int mb_qm_fp_alloc_buf(struct mb_qm_msg_desc *msg_desc)
{
	u8 mb_id = msg_desc->mb_id;
	u32 *qbase_addr_ptr;
	u32 mb_desc;
	u32 *temp;
	u32 *slot_ptr;
	u8 size;
	u8 slot;
	u8 pbn = mb_id + PPC_FP_MB_PBN_OFFSET;
	struct mb_qm_msg16 *mb_slot_ptr;

	slot = mb_fp_mbox_ctxt[msg_desc->mb_id].slot;

	/* If there is a message, read it as 16 byte msg */
	size = 1;

	temp = (u32 *) msg_desc->msg;
	mb_slot_ptr = (struct mb_qm_msg16 *) &fp_mboxes[msg_desc->mb_id];
	mb_slot_ptr += slot;
	slot_ptr = (u32 *) mb_slot_ptr;

#ifdef INVALIDATE_MB
	/* first invalidate slot */
	invalidate_dcache_range((u32) mb_slot_ptr, (u32) (mb_slot_ptr + 4)); 
#endif

	/* Make sure there is a message to read */
#ifdef CHECK_EMPTY_SLOT
	if (*((u32 *) ((u32 *) mb_slot_ptr) + 2) == EMPTY_SLOT) {
		/* message slot is empty */
		QM_DBG("No msg to dequeue from queue: %d\n", msg_desc->qid);
		return -1;
	}
#endif

	/* read message from the current slot */
	memcpy(temp, slot_ptr, 16);

	QM_DBG("Current slot: %d\n", slot);
	mb_fp_mbox_ctxt[msg_desc->mb_id].slot++;
	if (mb_fp_mbox_ctxt[msg_desc->mb_id].slot == mb_fp_mbox_ctxt[msg_desc->mb_id].total_slots)
		mb_fp_mbox_ctxt[msg_desc->mb_id].slot = 0;
	
#ifdef CHECK_EMPTY_SLOT
	*((u32 *) ((u32 *) mb_slot_ptr) + 2) = EMPTY_SLOT;
#endif

#ifdef FLUSH_MB
	/* flush msg to MEMQ*/
	flush_dcache_range((u32) mb_slot_ptr, (u32) (mb_slot_ptr + 8));
#endif

	/* Decrement number of messages in this slave id and PBN */
	qbase_addr_ptr = (u32 *) QM_B_ADDR_DEC;
	mb_desc = QM_DEC_MSG_VAL(pbn, 1);
	QM_DBG("Decrement number of messages: %p, val: %x\n", 
						qbase_addr_ptr, mb_desc);
	*qbase_addr_ptr = mb_desc;

	return 0;
}

int apm_qml_pull_msg(struct mb_qm_msg32 *msg)
{
        u32 cnt = 0;
        u32 val = 0;
        u32 *temp;

	/* Check if QML hardware available */
	if (is_noqml)
		return -1;

        val = WQ_BID_SET(val, INGRESS_WQ_PBID);
        val = WQ_MSG_SIZE_SET(val, WQ_SZ_32B);
        val = WQ_FETCH_SET(val, 1); 
	/* fetch message now */
        mb_qm_wr32(IP_BLK_IPP_QML, MPA_QMI_WQ_CTL_ADDR, val);

        /* check for message available */
        while(1) {
                mb_qm_rd32(IP_BLK_IPP_QML, MPA_QMI_WQ_CTL_ADDR, &val);
                if (!WQ_FETCH_RD(val))
                        break;
                if (cnt++ > MAX_DELAY_CNT) {
                        return -1;
                }
        }

        /* message is available, read it */
        temp = (u32 *) msg;

        mb_qm_rd32(IP_BLK_IPP_QML, MPA_QMI_WQ_MSG0_ADDR, temp);
	temp++;
        mb_qm_rd32(IP_BLK_IPP_QML, MPA_QMI_WQ_MSG1_ADDR, temp);
	temp++;
        mb_qm_rd32(IP_BLK_IPP_QML, MPA_QMI_WQ_MSG2_ADDR, temp);
	temp++;
        mb_qm_rd32(IP_BLK_IPP_QML, MPA_QMI_WQ_MSG3_ADDR, temp);
	temp++;
        mb_qm_rd32(IP_BLK_IPP_QML, MPA_QMI_WQ_MSG4_ADDR, temp);
	temp++;
        mb_qm_rd32(IP_BLK_IPP_QML, MPA_QMI_WQ_MSG5_ADDR, temp);
	temp++;
        mb_qm_rd32(IP_BLK_IPP_QML, MPA_QMI_WQ_MSG6_ADDR, temp);
	temp++;
        mb_qm_rd32(IP_BLK_IPP_QML, MPA_QMI_WQ_MSG7_ADDR, temp);

        return 0;
}

/*
 * Parse the exact for the Error Message received on Error Queue
 *
 */
void mb_qm_parse_error(struct mb_qm_msg_desc *err_msg_desc)
{
        struct mb_qm_msg32 *msg = (struct mb_qm_msg32 *) err_msg_desc->msg;
	struct mb_qm_msg16 *msg16 = &(msg->msg16);
	u8 err = 0;
        u8 cmd_acr_enq_err = 0;
        u8 cmd_acr_enq_qid = 0;
        u16 deq_slvid_pbn = 0;
        u8 deq_slot_num = 0;
        
        QM_ERR_CHK("QM Error LErr[%d] for Qid[%d] \n",
                   msg16->LErr, err_msg_desc->qid);
        
        switch(msg16->LErr) {
        case QM_MSG_SIZE_ERR:
                QM_ERR_CHK("Msg Size Error for Enqueue on Queue %d \n",
                           err_msg_desc->qid);
                break;
        case QM_HOP_COUNT_ERR:
                QM_ERR_CHK("Hop count error. QM received a message with"
                           "a hop count of 3 for Queue %d \n",
                           err_msg_desc->qid);
                break;
        case QM_VQ_ENQ_ERR:
                QM_ERR_CHK("Enqueue on Virtual Queue %d \n",
                           err_msg_desc->qid);
                break;
        case QM_DISABLEDQ_ENQ_ERR:
                QM_ERR_CHK("Enqueue on disabled Queue %d \n",
                           err_msg_desc->qid);
                break;
        case QM_Q_OVERFLOW_ERR:
                QM_ERR_CHK("Queue %d overflow, message send to"
                           "Error Queue \n",
                           err_msg_desc->qid);
                break;
        case QM_ENQUEUE_ERR:
                cmd_acr_enq_qid = (msg16->UserInfo & QM_QID_MASK);
                cmd_acr_enq_err = ((msg16->UserInfo >> 22) & 0X2);
                err = ((msg16->UserInfo >> 29) & 0X7);
                QM_ERR_CHK("Enqueue Erro on Qid[%d]\n", cmd_acr_enq_qid);
                switch(err) {
                case QM_AXI_READ_ERR:
                        QM_ERR_CHK("AXI error on read from PPC "
                                   "mailbox: Qid[%d]\n",
                                   cmd_acr_enq_qid);
                        break;
                case QM_AXI_ENQ_VQ_ERR:
                        QM_ERR_CHK("Alternate Enqueue Command to a"
                                   "Virtual Queue: Qid[%d]\n",
                                   cmd_acr_enq_qid);
                        break;
                case QM_AXI_ENQ_DQ_ERR:
                        QM_ERR_CHK("Alternate Enqueue Command to a"
                                   "Disabled Queue: Qid[%d]\n",
                                   cmd_acr_enq_qid);
                        break;
                case QM_AXI_ENQ_OVERFLOWQ_ERR:
                        QM_ERR_CHK("Alternate Enqueue Command "
                                   "overfills Queue: Qid[%d]\n",
                                   cmd_acr_enq_qid);
                        break;
                        
                }
                if (cmd_acr_enq_err == QM_AXI_SLAVE_ERR)
                        QM_ERR_CHK("AXI slave error on PPC mailbox"
                                   "read: Qid[%d]\n",
                                   cmd_acr_enq_qid);
                else if (cmd_acr_enq_err == QM_AXI_SLAVE_ERR)
                        QM_ERR_CHK("AXI decode error on PPC mailbox"
                                       "read: Qid[%d]\n",
                                   cmd_acr_enq_qid);
                break;
        case QM_DEQUEUE_ERR:
                err = ((msg16->UserInfo >> 29) & 0X7);
                deq_slvid_pbn = ((msg16->UserInfo >> 3) & 0XFF3);
                deq_slot_num = (msg16->UserInfo & 0x7);
                QM_ERR_CHK("Dequeue Error for deq_slot_num :%d and \n"
                           "deq_slvid_pbn: %d",
                           deq_slot_num, deq_slvid_pbn);
                if (err ==  QM_CHILD_VQ_ERR)
                        QM_ERR_CHK("VQ was assigned as a child of another"
                                   "VQ, deq_slot_num :%d and \n"
                                   "deq_slvid_pbn: %d",
                                   deq_slot_num, deq_slvid_pbn);
                else if (err == QM_DEQUEUE_DQ)
                        QM_ERR_CHK("A dequeue was requested from a"
                                   "disabled PQ, deq_slot_num :%d and \n"
                                   "deq_slvid_pbn: %d",
                                   deq_slot_num, deq_slvid_pbn);
                break;
        default:
                QM_ERR_CHK("Unknown Error \n");
                break;
        }
        return;
}
