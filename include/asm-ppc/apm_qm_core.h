/**
 * AppliedMicro APM862xxx QM Driver
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
 * @file apm_qm_core.h
 **
 */

#ifndef __MB_QM_CORE_H__
#define __MB_QM_CORE_H__

#include <asm/apm_qm_access.h>

#define QM_BASE_ADDR		qm_fabric_addr
#define QML_BASE_ADDR		qml_fabric_addr
#define QBASE_ADDR_QID_MASK	6
#define MAX_CPU_CORES		2
#define PPRO_MAX_INT		512
#define MAX_32B_MAIL_SLOTS 	8 
#define MAX_16B_MAIL_SLOTS 	8 

#define PPC_FP_MB_PBN_OFFSET	0x20

#define QML_B_ADDR_QID(x) 	(QML_BASE_ADDR | (x << QBASE_ADDR_QID_MASK))
#define QM_B_ADDR_QID(x) 	(QM_BASE_ADDR | (x << QBASE_ADDR_QID_MASK))
#define DEC_OFFSET		0x3c
#define QM_B_ADDR_DEC	 	(QM_BASE_ADDR | DEC_OFFSET)
#define PPC_SLAVE_ID		0xFF
#define SLAVE_ID_SHIFT		6
#define PPC_SLAVE_SHIFT		9
#define DEC_PBN_SHIFT		3
#define QM_DEC_MSG_VAL(pbn, dec) ((PPC_SLAVE_ID << PPC_SLAVE_SHIFT) | (pbn << DEC_PBN_SHIFT) | dec)

/* QM lite queues */
#define INGRESS_WQ_PBID 	1
#define WQ_SZ_32B     		1

#define APM_QM_MBID_SHIFT	5
#define APM_QM_SLOTID_SHIFT	2

#define NO_SLOTS_16BYTE_MSG	4
#define NO_SLOTS_32BYTE_MSG	8
#define NO_SLOTS_64BYTE_MSG	16

#define TOTAL_SLOTS		8
#define TOTAL_SLOTS_16BYTE_MSG	8
#define TOTAL_SLOTS_32BYTE_MSG	8
#define TOTAL_SLOTS_64BYTE_MSG	4

#define FREE_QUEUE_ENQ_MB	3
#define QML_FREE_Q_DEALLOC_MB	3

#define SIZE_64_MSG		3
#define SIZE_32_MSG		2
#define SIZE_16_MSG		1

#define ENQUE_MAIL_BOXES	16
#define DQ_MAIL_BOXES		16
#define FP_MAIL_BOXES		16

#define MB_QM_ETH_RTYPE		1
#define MB_QM_SEC_RTYPE		2
#define MB_QM_DMA_RTYPE		3
#define MB_QM_CLASS_RTYPE	4
#define MB_QM_VENET_RTYPE	5
#define MB_QM_PPC_RTYPE		6
#define MB_INVALID_RTYPE	0xF
#define EMPTY_SLOT		0x22222222

#define MB_QM_MAX_RTYPE		64

#if defined(CONFIG_SLAB_HW)	/* disable error queue if HW based buff pool */
#define ERR_QUEUE_ID	        0
#else
#define ERR_QUEUE_ID	        255 
#endif
#define MB_MAX_QUEUES		256
#define QM_QID_MASK             0XFF

/*  QML Queues used by Enet and iPP for WoL */
#define ENET_RX_FP_Q 		200
#define ENET_TX_Q    		201
#define ENET_RX_Q    		202
#define ENET_RX_FQ_TO_DDR    	203 /* Intermediate queue in WoL */

/* Prefetch buffer slave ids */
#define PB_SLAVE_ID_DMA		0x0
#define PB_SLAVE_ID_OCMM	0x1
#define PB_SLAVE_ID_SEC		0x2
#define PB_SLAVE_ID_CLASS	0x3
#define PB_SLAVE_ID_IPP		0x4
#define PB_SLAVE_ID_ETH		0x5
#define PB_SLAVE_ID_PPC		0xF
#define MAX_SLAVES		0x10

#define CORE0			0
#define CORE1			1
#define MAX_CORES		2

#define GET_MB_FROM_QUEUE(q)	q_to_mb[q]

#define IP_BLK_QM		1
#define IP_BLK_QML		2
#define IP_BLK_IPP_QML		3
#define MAX_DELAY_CNT   	10000

#define DIR_INGRESS		0
#define DIR_EGRESS		1

#define ING_GEN_WQUEUE_MB	5
#define ETH_TO_PPC0_COMP_MB	6
#define ETH_TO_PPC1_COMP_MB	15
#define GEN_TO_PPC0_COMP_MB	7
#define GEN_TO_PPC1_COMP_MB	14

#define ETH_TO_PPC0_COMP_QID	150
#define ETH_TO_PPC1_COMP_QID	151
#define GEN_TO_PPC0_COMP_QID	152
#define GEN_TO_PPC1_COMP_QID	153

#define ENABLE_VQ		1
#define DRR_CREDIT_GRAN		4500
enum apm_qm_arb_type {
	NO_ARB,
	SP_ARB,
	DRR_ARB,
	AVB_ARB,
};

/* QM light queues start at 0x1000 offset*/
#define QML_ETH_IPP_INGRESS_FP_ADDR	0x00401000
#define QML_ETH_IPP_INGRESS_WQ_ADDR	0x00401200
#define QML_ETH_IPP_EGRESS_WQ_ADDR      0x00401400
#define QML_ETH_IPP_FREE_POOL_ADDR	0x00401600
#define QML_UPPER_ADDR_NIBBLE	        0xD

/* QM return codes */
#define QM_ERR				-1
#define QM_OK				0

/* Check for invalid Rtype in mailbox */
/* :CD: Change to undefines instead of if 0 */
/* :CD: Keep only the options which are finally used, get rid of all unused ones */
#if 0
#define CHECK_FOR_INVALID_RTYPE
#endif

/* QM mailbox locations */
/* :CD: read base address from DTS, if address is not there, create it in DDR */

/* Locate QM mailboxes in OCM */
#if 0
#define CONFIG_MB_OCM
#endif

/* Locate QM mailboxes in FAM */
#if 0
#define CONFIG_MB_FAM
#endif

/* Locate QM mailboxes in MEMQ */
#if 1
#define CONFIG_MB_MEMQ
#endif

/* Locate QM mailboxes in DDR */
#if 0
#define CONFIG_MB_DDR
#endif

/* Locate ENQ QM mailboxes in MEMQ */
#if 0
#define CONFIG_ENQ_MB_MEMQ
#endif

/* Check for mailbox available before writing msg */
#if 1
#define CONFIG_MB_CHECK_TX_MSG
#endif

#if 1
#define CHECK_EMPTY_SLOT
#endif

#if 0
#define CHECK_MB_STATUS
#endif

#if 0
#define CHECK_UINFO
#endif

#if 0
#define CONFIG_MB_CHECK_EACH_TX_MSG
#undef CONFIG_MB_CHECK_TX_MSG
#endif

#ifdef CONFIG_MB_CHECK_TX_MSG
#undef CONFIG_MB_CHECK_EACH_TX_MSG
#endif

/* QM Queues locations */
/* :CD: read base address from DTS, if address is not there, create it in DDR */

/* Create queues in OCM */
#if 0
#define CONFIG_QUEUE_OCM
#endif

/* Create queues in FAM */
#if 0
#define CONFIG_QUEUE_FAM
#endif

#define virt_to_phys_ocm(x) (x + 0x6FFF8000)
#define OCM_UPPER_ADDR_NIBBLE 0xe
#define FAM_UPPER_ADDR_NIBBLE 0x4

enum apm_qm_ips {
	IP_PPC0,
	IP_PPC1,
	IP_ETH0,
	IP_ETH1,
	IP_IPP,
	IP_DMA,
	IP_SEC,
	IP_OCMM,
	IP_CLASS,
	IP_MAX,
};

enum mb_q_type {
	QUEUE_DISABLED,
	P_QUEUE,
	FREE_POOL,
	V_QUEUE,
};

enum mb_msg_size {
	MSG_16B,
	MSG_32B,
	MSG_64B
};

enum mb_fp_mode {
	MSG_NO_CHANGE,
	ROUND_ADDR,
	REDUCE_LEN,
	CHANGE_LEN
};

enum mb_q_size {
	SIZE_512B,
	SIZE_2KB,
	SIZE_16KB,
	SIZE_64KB,
	SIZE_512KB,
};

enum mb_notify_ppc {
	NO_NOTIFY,
	NOTIFY_CPU0,
	NOTIFY_CPU1,
	NOTIFY_BOTH,
};

/* LErr(3b) Decoding */
enum mb_qm_lerr {
        QM_NO_ERR,
        QM_MSG_SIZE_ERR,
        QM_HOP_COUNT_ERR,
        QM_VQ_ENQ_ERR,
        QM_DISABLEDQ_ENQ_ERR,
        QM_Q_OVERFLOW_ERR,
        QM_ENQUEUE_ERR,
        QM_DEQUEUE_ERR,
};

/* Userinfo encodings for LERR code 6 */

/* err[2:0] Encoding */
#define QM_AXI_READ_ERR             0  /* AXI error on read from PPC mailbox */ 
#define QM_AXI_ENQ_VQ_ERR           3  /* Alternate enq cmd to a VQ */
#define QM_AXI_ENQ_DQ_ERR           4  /* Alternate enq cmd to a Disabled Q */
#define QM_AXI_ENQ_OVERFLOWQ_ERR    5  /* Alternate enq cmd overfills a Q  */

/* cmd_acr_enq_err[1:0] Encoding  */
enum mb_qm_enq_err {
        QM_NO_AXI_ERR,
        QM_AXI_SLAVE_ERR,  /* AXI slave error on PPC mailbox read   */
        QM_AXI_DECODE_ERR, /* AXI decode error on PPC mailbox read  */
};

/* Userinfo encodings for LERR code 7 */
#define QM_CHILD_VQ_ERR  6     /* VQ was assigned as a child of another VQ  */
#define QM_DEQUEUE_DQ    7     /* dequeue was requested from a disabled PQ */


/*
 * @struct  vqstate_w0
 * @brief   This structure represents word 0 of virtual queue state 
 **
 */
struct vqstate_w0 {
	u32 cfgqtype 		: 2; /* queue type, refer mb_q_type */
	u32 cfgselthrsh 	: 3; /*  associated threshold set */
	
	u32 q0_sel              : 8; 
	u32 q0reqvld            : 1; 
	u32 q0txallowed         : 1; 
	u32 q0selarb            : 2;
	
	u32 q1_sel              : 8; 
	u32 q1reqvld            : 1; 
	u32 q1txallowed         : 1;
	u32 q1selarb            : 2;
	
	u32 q2_sel_3b           : 3; 
} __attribute__ ((packed));

/*
 * @struct  vqstate_w1
 * @brief   This structure represents word 1 of virtual queue state 
 **
 */
struct vqstate_w1 {
	u32 q2_sel_5b           : 5;
	u32 q2reqvld            : 1; 
	u32 q2txallowed         : 1;
	u32 q2selarb            : 2;

	u32 q3_sel              : 8; 
	u32 q3reqvld            : 1; 
	u32 q3txallowed         : 1; 
	u32 q3selarb            : 2;

	u32 q4_sel              : 8; 
	u32 q4reqvld            : 1; 
	u32 q4txallowed         : 1;
	u32 q4selarb            : 1;
} __attribute__ ((packed));

/*
 * @struct  vqstate_w2
 * @brief   This structure represents word 2 of virtual queue state 
 **
 */
struct vqstate_w2 {
	u32 q4selarb            : 1;

	u32 q5_sel              : 8; 
	u32 q5reqvld            : 1; 
	u32 q5txallowed         : 1; 
	u32 q5selarb            : 2;
	
	u32 q6_sel              : 8; 
	u32 q6reqvld            : 1; 
	u32 q6txallowed         : 1; 
	u32 q6selarb            : 2;
	
	u32 q7_sel_7b           : 7; 
} __attribute__ ((packed));

/*
 * @struct  vqstate_w3
 * @brief   This structure represents word 3 of virtual queue state 
 **
 */
struct vqstate_w3 {
	u32 q7_sel_1b           : 1; 
	u32 q7reqvld            : 1; 
	u32 q7txallowed         : 1; 
	u32 q7selarb            : 2;
	
	u32 rid 		: 3; /* current region id of queue fill level */
	u32 ppc_notify 		: 2; /* see mb_notify_ppc  */
	u32 cfgcrid 		: 1; /* critical rid config */
	u32 cfgnotifyqne 	: 1; /* enable queue not empty interrupt */
	u32 cfgsaben 		: 1; /* enable SAB broadcasting */
	u32 rsvd                : 1;
	u32 nummsg 		: 18; 
} __attribute__ ((packed));

/*
 * @struct  pqstate_w0
 * @brief   This structure represents word 0 of queue state (pq or fp)
 **
 */
struct pqstate_w0 {
	u32 cfgqtype 		: 2; /* queue type, refer mb_q_type */
	u32 cfgselthrsh 	: 3; /*  associated threshold set */
	u32 qstatelock 		: 1;
	u32 cfgqsize 		: 3; /* queue size, see mb_q_size */
	u32 fp_mode 		: 3; /* free pool mode */
	u32 cfgacceptlerr 	: 1; 
	u32 reserved 		: 15; 
	u32 cfgstartaddr 	: 4; /* msb 4 bits from 28 bit address */
}__attribute__ ((packed));

/*
 * @struct  pqstate_w1
 * @brief   This structure represents word 1 of queue state (pq or fp)
 **
 */
struct pqstate_w1 {
	u32 cfgstartaddr 	: 24; /* lsb 24 bits from 28 bit address */
	u32 reserved_1 		: 8;
}__attribute__ ((packed));

/*
 * @struct  pqstate_w2
 * @brief   This structure represents word 2 of queue state (pq or fp)
 **
 */
struct pqstate_w2 {
	u32 reserved_2 		: 1;
	u32 headptr 		: 15; 
	u32 nummsg 		: 16; 
}__attribute__ ((packed));

/*
 * @struct  pqstate_w3
 * @brief   This structure represents word 3 of queue state (pq or fp)
 **
 */
struct pqstate_w3 {
	u32 reserved_3 		: 5;
	u32 rid 		: 3; /* current region id of queue fill level */
	u32 ppc_notify 		: 2; /* see mb_notify_ppc  */
	u32 cfgcrid 		: 1; /* critical rid config */
	u32 cfgnotifyqne 	: 1; /* enable queue not empty interrupt */
	u32 cfgsaben 		: 1; /* enable SAB broadcasting */
	u32 cfgtmvq 		: 8; /* parent vq */
	u32 cfgtmvqen 		: 1; /* enable pq to belong to vq */
	u32 reserved_4 		: 10;
}__attribute__ ((packed));

union uqstate_w0 {
	u32 *val;
	struct pqstate_w0 *w0;
};

union uqstate_w1 {
	u32 *val;
	struct pqstate_w1 *w1;
};

union uqstate_w2 {
	u32 *val;
	struct pqstate_w2 *w2;
};

union uqstate_w3 {
	u32 *val;
	struct pqstate_w3 *w3;
};

struct q_raw_state {
	u32 w0;
	u32 w1;
	u32 w2;
	u32 w3;
};

/*
 * @struct  mb_qm_qstate
 * @brief   This structure represents per queue state database
 **
 */
struct mb_qm_qstate {
	u8 ip_blk; /**< which ip block this queue belongs to (QM or QM light) */
	u8 valid;  /**< 1 if configured else 0 */
	u8 q_id;   /**< queue id */
	u8 mb_id;  /**< mailbox id, used for ingress only */
	u8 slave_id;		/**< prefetch mgr slave id */
	u8 pbn;			/**< prefetch buffer number */
	u8 q_type;		/**< queue type, see mb_q_type */
	u8 msg_size;		/**< message size supported for this queue */
	u8 vqen;		/**< Virtual Queue enable/disable */
	int parent_vqid; 	/**< parent qid, if this pq belongs to any vq */
	u8 ppc_notify; 		/**< 	
				0: Notification not required by PPC,
				1: Notify CPU 0 only,
				2: Notify CPU 1 only,
				3: Notify both CPUs */
	u8 q_size; 		/**< 0: 2KB, 1: 16KB, 2: 64KB, 3: 512KB */
	u32 q_start_addr; 	/**< 28 bit, 256 byte aligned  start addr */ 
	u8 fp_mode; 		/**< handle addr and len of buf, refer 
				     mb_fp_mode 
				0: No modification to message,
				1: Round the start address down to an address 
				   boundary consistant with the size of the buf
				2: Reduce the BufDataLen field according to 
				   BufSize - DataAddr(LSBs).
				3: Change the BufDataLen field to indicate the 
				   BufSize (zero it out to indicate the maximum
				   value). */
	u8 thr_set; /**< configured threshold set (one of eight thresholds) */
	u32 nummsgs; 	/**< Number of messages in the queue */  
	u8 rid; 	/**< Current region ID of the queue */   
	u8 q_not_empty_intr; /**< Enable/disable queue not empty interrupt */   
	u8 direction;	/**< 0: ingress, 1: egress */
	u8 cfgsaben;	/**< Enable broadcasting of information on sab bus */		 
	u8 pq_sel[8];           /**< PQs to be added to VQ */
	u8 q_sel_arb[8]; 	/**< For VQ only, select queue arbitration policy:
				   0: This queue is not enabled for dequeue
				1: strict priority
				2: WRR priority
				3: AVB arbitration */
	 u8 shape_rate[8];      /**< Shaping rate in case of AVB ARB,
				     Weight for DRR ARB and not used
				     for SP ARB */
	u8 src_ip_blk;  /**< Requested by IP block */
	u8 ppc_id;      /**< Destination IP block */
};

/* QM messages */

/*
 * @struct  mb_qm_msg16
 * @brief   This structure represents 16 byte QM message format
 **
 */
struct mb_qm_msg16 {
	u32 srcDomID 	: 8;
	u32 C 		: 1;
	u32 BufDataLen 	: 15;
	u32 DataAddrMSB : 8;

	u32 DataAddrLSB : 32;	

	u32 HL 		: 1;
	u32 LErr 	: 3;
	u32 RType 	: 4;
	u32 PV 		: 1;
	u32 SB 		: 1;
	u32 HB 		: 1;
	u32 PB 		: 1;
	u32 LL 		: 1;
	u32 NV 		: 1;
	u32 HC 		: 2;
	u32 Rv 		: 2;
	u32 FPQNum 	: 14;

	u32 UserInfo;
}__attribute__ ((packed)); 

/*
 * @struct  mb_qm_msg_up8
 * @brief   This structure represents 8 byte portion of QM message format
 **
 */
struct mb_qm_msg_up8 {
	u32 H0FPSel 	: 6;
	u32 HR 		: 1;
	u32 HE 		: 1;
	u32 DR 		: 1;
	u32 SZ 		: 1;
	u32 H0Enq_Num 	: 14;
	u32 H0Info_msb 	: 8;

	u32 H0Info_lsb : 32;

}__attribute__ ((packed)); 

/*
 * @struct  mb_qm_msg32
 * @brief   This structure represents 32 byte QM message format
 **
 */
struct mb_qm_msg32 {
	struct mb_qm_msg16 msg16;
	struct mb_qm_msg_up8 msgup8_1;
	struct mb_qm_msg_up8 msgup8_2;
}__attribute__ ((packed));

/*
 * @struct  mb_qm_msg64
 * @brief   This structure represents 64 byte QM message format
 **
 */
struct mb_qm_msg64 {
	struct mb_qm_msg32 msg32_1;
	struct mb_qm_msg32 msg32_2;
}__attribute__ ((packed));

/*
 * @struct  mb_mailbox_ctxt
 * @brief   This structure contains mailbox context information
 **
 */
struct mb_mailbox_ctxt {
	u8 slot; 	/* current mailbox slot to write to */
	u8 total_slots; /* total mailbox slots */ 
};

/*
 * @struct  mailbox
 * @brief   This structure represents a work queue mailbox 
 **
 */
struct mailbox {
	struct mb_qm_msg32 mailslots[MAX_32B_MAIL_SLOTS]; 
};

/*
 * @struct  fp_mailbox
 * @brief   This structure represents a free pool mailbox 
 **
 */
struct fp_mailbox {
	struct mb_qm_msg16 mailslots[MAX_16B_MAIL_SLOTS]; 
};

/*
 * @struct  mb_qm_msg_desc
 * @brief   This structure represents a QM msg descriptor
 **
 */
struct mb_qm_msg_desc {
	u32 qid; 	/**< destination QID to send message to */
	u16 mb_id; 	/**< mailbox id to push/pull this message to/from */
	u16 is_msg16;   /**< set this to get 1st 16 bytes message */
	void *msg;	/**< Pointer to message */
};

/*
 * @struct  apm_qm_qalloc
 * @brief   This structure contains info about allocating queues for IP blocks
 **
 */
struct apm_qm_qalloc {
	u8 qm_ip_blk;		/**< 1: QM or 2: QM lite */
	u8 ip_blk;		/**< Requesting (source) IP block */
	u8 ppc_id;		/**< Destination IP block */
	u8 q_type;		/**< 0: PQ, 1: VQ, 2: FP */
	u32 q_count;		/**< Number of queues to allocate */
	u8 vqen; 		/**< Enable this PQ to be part of the VQ */
	u32 parent_vq;		/**< If need to attach VQ to PQs, provide VQ */
	u8 direction;		/**< 0: Ingress, 1: Egress */
	u8 qsize;		/**< Size of queue to be created */
	u8 thr_set;		/**< Queue threshold set to be used */
	u8 en_fill_chk;		/**< Enable queue fill check on this queue */
	u8 q_sel_arb[8];        /**< Arbitration mechanism for VQ */
	u8 pq_sel[8];           /**< PQs to be added to VQ */
	u8 shape_rate[8];       /**< Shaping rate in case of AVB ARB,
				     Weight for DRR ARB and not used
				     for SP ARB */
	struct mb_qm_qstate *qstates;	/**< Store queue states here */
};

/* QM callback function type */
typedef int (*mb_qm_msg_fn_ptr) (struct mb_qm_msg_desc *msg_desc);

/* API declarations */

/**
 * @brief   Initialize Error Q
 * @param   PowerPC Processor ID to which error Q will interrupt
 * @return  0 - success or -1 - failure
 **
 */
int mb_qm_init_errq(int ppc_id);

/**
 * @brief   Initialize QM HW block
 * @param   no_of_queues Number of queues to initialize
 * @return  0 - success or -1 - failure
 **
 */
int mb_qm_init(u8 port, int no_of_queues);

/**
 * @brief   Reads global queue table and configs state of each queues in QM
 * @param   no_of_queues Number of queues to configure
 * @return  0 - success or -1 - failure
 **
 */
int mb_config_qstate(u8 port, int no_of_queues);

/**
 * @brief   Return the current qstate configuration without talking to QM HW
 * @param   q_num Queue number
 * 	    qstate Pointer to queue state structure for this queue
 * @return  0 - success or -1 - failure
 **
 */
int mb_qm_qstate_rd_cfg(int q_num, struct mb_qm_qstate *qstate);

/**
 * @brief   Reads the state of the given queue number from QM
 * @param   ip QM or QM light
 * 	    q_num Queue number 
 *          qstate Pointer to queue state structure for this queue
 * @return  0 - success or -1 - failure
 **
 */
int mb_qm_qstate_rd(int ip, int q_num, struct mb_qm_qstate *qstate);

/**
 * @brief   Writes the state of the given queue number to QM
 * @param   qstate Pointer to queue state structure for this queue
 * @return  0 - success or -1 - failure
 **
 */
int mb_qm_qstate_wr(struct mb_qm_qstate *qstate);

/**
 * @brief   Reads the state of the given queue number from QM in RAW format
 * @param   ip QM or QM light
 * 	    q_num Queue number to read the state for
 *          raw_qstate Pointer to queue state structure for this queue
 * @return  0 - success or -1 - failure
 **
 */
int mb_qm_raw_qstate_rd(int ip, int q_num, struct q_raw_state *raw_qstate);

/**
 * @brief   Writes the state of the given queue number in QM in RAW format
 * @param   ip QM or QM light
 * 	    q_num Queue number to write the state for
 *          raw_qstate Pointer to queue state structure for this queue
 * @return  0 - success or -1 - failure
 **
 */
int mb_qm_raw_qstate_wr(int ip, int q_num, struct q_raw_state *raw_qstate);

/**
 * @brief   Enqueues (Pushes) the message to mailbox on given QML queue
 * @param   msg_desc Descriptor of the message to push
 * @return  0 - success or -1 - failure
 **
 */
int mb_qml_push_msg(struct mb_qm_msg_desc *msg_desc);

/**
 * @brief   Enqueues (Pushes) the message to mailbox on given QM queue
 * @param   msg_desc Descriptor of the message to push
 * @return  0 - success or -1 - failure
 **
 */
int mb_qm_push_msg(struct mb_qm_msg_desc *msg_desc);

/**
 * @brief   Gives the userinfo from completion queue
 * @param   mb_id Mailbox ID
 * @return  userinfo - success or -1 - failure
 **
 */
u32 mb_qm_pull_comp_msg(u32 mb_id);

/**
 * @brief   Dequeues (Pulls) the message from mailbox on given queue
 * @param   msg_desc Descriptor of the message to pull
 * @return  0 - success or -1 - failure
 **
 */
int mb_qm_pull_msg(struct mb_qm_msg_desc *msg_desc);

/**
 * @brief   Deallocates buffer to given QML buffer pool 
 * @param   msg_desc Descriptor of the msg (containing buffer to deallocate)
 * @return  0 - success or -1 - failure
 **
 */
int mb_qml_fp_dealloc_buf(struct mb_qm_msg_desc *msg_desc);

/**
 * @brief   Deallocates buffer to given buffer pool 
 * @param   msg_desc Descriptor of the msg (containing buffer to deallocate)
 * @return  0 - success or -1 - failure
 **
 */
int mb_qm_fp_dealloc_buf(struct mb_qm_msg_desc *msg_desc);

/**
 * @brief   Allocate buffer to given buffer pool 
 * @param   msg_desc Descriptor of the msg (containing buffer to allocate)
 * @return  0 - success or -1 - failure
 **
 */
int mb_qm_fp_alloc_buf(struct mb_qm_msg_desc *msg_desc);

/**
 * @brief   Dequeues (Pulls) the message from QML queue
 * @param   msg Descriptor of the message to pull
 * @return  0 - success or -1 - failure
 **
 */
int apm_qml_pull_msg(struct mb_qm_msg32 *msg);

/**
 * @brief   Unregister QM callback function for given RTYPE
 * @param   rtype RTYPE for which to register callback
 * @return  0 - success or -1 - failure
 **
 */
int mb_qm_msg_rx_unregister(u32 rtype);

/**
 * @brief   Register QM callback function for given RTYPE
 * @param   rtype RTYPE for which to register callback
 *	    fn_ptr Callback function pointer
 * @return  0 - success or -1 - failure
 **
 */
int mb_qm_msg_rx_register(u32 rtype, mb_qm_msg_fn_ptr fn_ptr);

/**
 * @brief   Parse the exact for the Error Message received on Errro Queue
 * @param   err_msg_desc - Descriptor of the Error msg 
 * @return  None
 **
 */
void mb_qm_parse_error(struct mb_qm_msg_desc *err_msg_desc);

/**
 * @brief   Return queues to given IP blocks
 * @param   qalloc Structure filled up with queue allocation information for QM driver
 * @return  0 - success or -1 - failure
 **
 */
int apm_qm_alloc_q(struct apm_qm_qalloc *qalloc);

/**
 * @brief   Return Virtual queue to given IP blocks
 * @param   qalloc Structure filled up with queue allocation information
 *		for QM driver
 * @return  0 - success or -1 - failure
 **
 */
int apm_qm_alloc_vq(struct apm_qm_qalloc *qalloc, u32 qid);

/**
 * @brief   Return completion queue for current PPC
 * @param   ip_blk IP block to get completion queue for
 *	    ppc_id Which power pc the completion queue belongs to
 * @return  0 - success or -1 - failure
 **
 */
struct mb_qm_qstate *get_comp_queue(int ip_blk, int ppc_id);

/**
 * @brief   Configure completion queues 
 * @param   None
 * @return  None
 **
 */
int config_comp_queues(void);

/**
 * @brief   Configure prefetch buffer table
 * @param   ip	QM or QM lite IP to configure the pbn for
 *	    slv_id PBN Slave ID 
 *	    pbn prefetch buffer number to configure
 *	    qnum Queue number to configure PBN for
 *	    is_fp Is it free queue or not
 * @return  None
 **
 */
int mb_qm_pb_config(int ip, int slv_id, int pbn, int qnum, u8 is_fp, u8 is_vq);

int mb_qm_pb_get(int ip, int slv_id, int pbn);

int mb_qm_pb_set(int ip, int slv_id, int pbn, int pbn_buf);
	
int mb_qm_pb_disable(int ip, int slv_id, int pbn, int qnum);
int mb_qm_alloc_mbox_ocm(void);
int mb_qm_cstate_rd(u32 qid, u32 * cstate);
int mb_qm_cstate_wr(u32 qid, u32 cstate[2]);
int apm_qm_get_vq(u32 ip_blk);
	
#endif /* __MB_QM_CORE_H__ */
