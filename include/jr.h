/*
 * Copyright 2012 Freescale Semiconductor, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __JR_H
#define __JR_H

#include <linux/compiler.h>

#define JR_SIZE 4
/* Timeout currently defined as 10 sec */
#define CONFIG_SEC_DEQ_TIMEOUT	10000000U

#define DEFAULT_JR_ID		0
#define DEFAULT_JR_LIODN	0
#define DEFAULT_IRQ		0	/* Interrupts not to be configured */

#define MCFGR_SWRST       ((uint32_t)(1)<<31) /* Software Reset */
#define MCFGR_DMA_RST     ((uint32_t)(1)<<28) /* DMA Reset */
#define MCFGR_PS_SHIFT          16
#define JR_INTMASK	  0x00000001
#define JRCR_RESET                  0x01
#define JRINT_ERR_HALT_INPROGRESS   0x4
#define JRINT_ERR_HALT_MASK         0xc
#define JRNSLIODN_SHIFT		16
#define JRNSLIODN_MASK		0x0fff0000

#define JQ_DEQ_ERR		-1
#define JQ_DEQ_TO_ERR		-2
#define JQ_ENQ_ERR		-3

struct op_ring {
	dma_addr_t desc;
	uint32_t status;
} __packed;

typedef struct jr_info {
	void (*callback)(dma_addr_t desc, uint32_t status, void *arg);
	dma_addr_t desc_phys_addr;
	uint32_t desc_addr;
	uint32_t desc_len;
	uint32_t op_done;
	void *arg;
} jr_info_t;

typedef struct jobring {
	int jq_id;
	int irq;
	int liodn;
	/* Head is the index where software would enq the descriptor in
	 * the i/p ring
	 */
	int head;
	/* Tail index would be used by s/w ehile enqueuing to determine if
	 * there is any space left in the s/w maintained i/p rings
	 */
	/* Also in case of deq tail will be incremented only in case of
	 * in-order job completion
	 */
	int tail;
	/* Read index of the output ring. It may not match with tail in case
	 * of out of order completetion
	 */
	int read_idx;
	/* Write index to input ring. Would be always equal to head */
	int write_idx;
	/* Size of the rings. */
	int size;
	/* The ip and output rings have to be accessed by SEC. So the
	 * pointers will ahve to point to the housekeeping region provided
	 * by SEC
	 */
	/*Circular  Ring of i/p descriptors */
	dma_addr_t *input_ring;
	/* Circular Ring of o/p descriptors */
	/* Circula Ring containing info regarding descriptors in i/p
	 * and o/p ring
	 */
	/* This ring can be on the stack */
	jr_info_t info[JR_SIZE];
	struct op_ring *output_ring;
} jobring_t;

struct result {
	int done;
	int err;
	uint32_t status;
	char outstr[256];
};

int sec_init(struct jobring *jr);
int jr_enqueue(struct jobring *jr, uint32_t *desc_addr,
		void (*callback)(uint32_t desc, uint32_t status, void *arg),
		void *arg);
int jr_dequeue(struct jobring *jr);
int caam_jr_strstatus(char *outstr, u32 status);
int jr_reset(struct jobring *jr);

#endif
