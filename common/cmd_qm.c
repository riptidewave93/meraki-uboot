/*
 * (C) Copyright 2010-2011
 * Keyur Chudgar <kchudgar@amcc.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * Misc boot support
 */
#include <config.h>
#include <common.h>
#include <command.h>
#include <asm/processor.h>
#include <asm/io.h>
#include <asm/apm_ipp_csr.h>
#include <asm/apm_scu.h>
#include <asm/apm_qm_reg.h>
#include <malloc.h>
#include <asm/apm_qm_core.h>
#include <asm/apm_qm_access.h>

extern char *mb_qtype_str[];
extern char *mb_msg_size_str[];
extern void dump_qstate(struct mb_qm_qstate *qstate);
extern int mb_qm_send_msg_util(int qid, int mbid);
extern int mb_qm_rx_msg_util(int qid, int mbid);
extern int mb_qm_dealloc_buf_util(int qid, int mbid);
extern int mb_qm_alloc_buf_util(int qid, int mbid);
extern u32 qm_csr_addr;

/* This is a trivial atoi implementation since we don't have one available */
int atoi(char *string)
{
	int length;
	int retval = 0;
	int i;
	int sign = 1;

	length = strlen(string);
	for (i = 0; i < length; i++) {
		if (0 == i && string[0] == '-') {
			sign = -1;
			continue;
		}
		if (string[i] > '9' || string[i] < '0') {
			break;
		}
		retval *= 10;
		retval += string[i] - '0';
	}
	retval *= sign;
	return retval;
}

int mb_qm_util_init(void)
{
	int eth_num = 0;
	/* create one physical queue destined to PPC0 for loopback testing */
#if 0
	static struct mb_qm_qstate enet_qstates;
	struct apm_qm_qalloc eth_qalloc = {0};
	int err = 0;
#endif
	struct apm86xxx_reset_ctrl reset_ctrl;
	int rc;
	
	/* Reset QM */
	printf("Reseting QM\n");

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

	printf("Reset QM done...\n");

	/* enable QPcore */
	mb_qm_init(eth_num, NO_OF_QUEUES);

#if 0
	printf("Allocate one work queue for PPC0......................\n");

	eth_qalloc.qstates = &enet_qstates;
	eth_qalloc.qm_ip_blk = IP_BLK_QM;
	eth_qalloc.ip_blk = IP_PPC0;
	eth_qalloc.q_type = P_QUEUE;
	eth_qalloc.q_count = 1;
	eth_qalloc.direction = DIR_INGRESS;
	eth_qalloc.qsize = SIZE_2KB;
	eth_qalloc.thr_set = 1;

	if ((err = apm_qm_alloc_q(&eth_qalloc)) != 0) {
		printf("Error allocating work queue for PPC0\n");
		return -1;
	}

	printf("Allocate one error queue for PPC0......................\n");

	if ((err = apm_qm_alloc_q(&eth_qalloc)) != 0) {
		printf("Error allocating work queue for PPC0\n");
		return -1;
	}

	dump_qstate(&enet_qstates); 
#endif

	return 0;
}

#if 0
int mb_qm_util_init(void)
{
	/* create one physical queue destined to PPC0 for loopback testing */
	static struct mb_qm_qstate *enet_qstates = NULL;
	struct apm_qm_qalloc eth_qalloc = {0};
	int err = 0;
	struct apm86xxx_reset_ctrl reset_ctrl;
	
	/* Reset QM */
	printf("Reseting QM\n");

	memset(&reset_ctrl, 0, sizeof(reset_ctrl));
	reset_ctrl.reg_group = REG_GROUP_SRST_CLKEN;
	reset_ctrl.clken_mask = QMTM_F1_MASK;
	reset_ctrl.csr_reset_mask = QMTM_F3_MASK;
	reset_ctrl.reset_mask = QMTM_MASK;
	err = reset_apm86xxx_block(&reset_ctrl);
	if (err) return err;

	printf("Reset QM done...\n");

	/* enable QPcore */
	mb_qm_init(0);

	/* allocate queue state memory if not done so already */
	if (enet_qstates == NULL) {
		enet_qstates = 
			malloc(sizeof(struct mb_qm_qstate));

		if (enet_qstates == NULL) {
			printf("Couldn't allocate memory for Enet queues state\n");
			return -1;
		}

		memset(enet_qstates, 0, sizeof(struct mb_qm_qstate));
	}

	printf("Allocate one work queue for PPC0\n");

	eth_qalloc.qstates = enet_qstates;
	eth_qalloc.qm_ip_blk = IP_BLK_QM;
	eth_qalloc.ip_blk = IP_PPC0;
	eth_qalloc.q_type = P_QUEUE;
	eth_qalloc.q_count = 1;
	eth_qalloc.direction = DIR_INGRESS;
	eth_qalloc.qsize = SIZE_16KB;
	eth_qalloc.thr_set = 1;

	if ((err = apm_qm_alloc_q(&eth_qalloc)) != 0) {
		printf("Error allocating work queue for PPC0\n");
		return -1;
	}

	dump_qstate(enet_qstates);

	return 0;
}
#endif

int do_qm_util (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int qnum, mbid;
	struct mb_qm_qstate qstate;

	if (argc == 1) {
		cmd_usage(cmdtp);
		return 0;
	}

	if (strcmp(argv[1], "init") == 0) {	
		printf ("## Initializing QM ...\n");
		if (!mb_qm_util_init())
			printf ("## QM init done...\n");
		else
			printf ("## Error in QM init...\n");
	} else if (strcmp(argv[1], "qstate") == 0) {
		qnum = atoi(argv[2]);
		printf("dumping queue state for queue: %d\n", qnum);
		mb_qm_qstate_rd(IP_BLK_QM, qnum, &qstate);
		dump_qstate(&qstate);
	} else if (strcmp(argv[1], "sendmsg") == 0) {
		if (argc < 4) {
			printf("sendmsg parameter error\n");
			cmd_usage(cmdtp);
			return 0;
		}

		qnum = atoi(argv[2]);
		mbid = atoi(argv[3]);
		printf("sending message on queue: %d using mailbox: %d\n", qnum, mbid);
		mb_qm_send_msg_util(qnum, mbid);
	} else if (strcmp(argv[1], "rcvmsg") == 0) {
		if (argc < 4) {
			printf("rcvmsg parameter error\n");
			cmd_usage(cmdtp);
			return 0;
		}

		qnum = atoi(argv[2]);
		mbid = atoi(argv[3]);
		printf("Receiving message from queue: %d using mailbox: %d\n", qnum, mbid);
		mb_qm_rx_msg_util(qnum, mbid);
	} else if (strcmp(argv[1], "deallocbuf") == 0) {
		if (argc < 4) {
			printf("deallocbuf parameter error\n");
			cmd_usage(cmdtp);
			return 0;
		}

		qnum = atoi(argv[2]);
		mbid = atoi(argv[3]);
		printf("Deallocating buffer to queue: %d using mailbox: %d\n", qnum, mbid);
		mb_qm_dealloc_buf_util(qnum, mbid);
	} else if (strcmp(argv[1], "allocbuf") == 0) {
		if (argc < 4) {
			printf("allocbuf parameter error\n");
			cmd_usage(cmdtp);
			return 0;
		}

		qnum = atoi(argv[2]);
		mbid = atoi(argv[3]);
		printf("Allocating buffer from queue: %d using mailbox: %d\n", qnum, mbid);
		mb_qm_alloc_buf_util(qnum, mbid);
	}
	
	return 0;
}

void qm_test(int i)
{
	int qnum;
	int mbid;

	switch (i) {
	case 1:
		qnum = 21;
		mbid = 0;
		printf("sending message on queue: %d using mailbox: %d\n", qnum, mbid);
		mb_qm_send_msg_util(qnum, mbid);
		break;   
	case 2:
                qnum = 21;
                mbid = 8;
                printf("Receiving message from queue: %d using mailbox: %d\n", qnum, mbid);
                mb_qm_rx_msg_util(qnum, mbid);
		break;
	}
}

/* -------------------------------------------------------------------- */

U_BOOT_CMD(
	qm_util, 4, 1,	do_qm_util,
	"Mamba QM util",
	"[init, qstate [qnum], sendmsg [qnum][mbid] rcvmsg [qnum][mbid] \
	deallocbuf [qnum][mbid] allocbuf [qnum][mbid]"
);

