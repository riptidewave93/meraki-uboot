/*
 * Copyright 2011, 2012 Freescale Semiconductor, Inc.
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

#include <common.h>
#include <malloc.h>
#include <asm/immap_85xx.h>
#include <jr.h>
#include <asm/fsl_pamu.h>
#include <fsl_secboot_err.h>

#define CIRC_CNT(head, tail, size)	(((head) - (tail)) & (size - 1))
#define CIRC_SPACE(head, tail, size)	CIRC_CNT((tail), (head) + 1, (size))

static inline void jr_reset_liodn(void)
{
	ccsr_sec_t *sec = (void *)CONFIG_SYS_FSL_SEC_ADDR;
	out_be32(&sec->jrliodnr[0].ls, 0);
	asm volatile("sync" : : : "memory");
}

static inline void jr_disable_irq(void)
{
	struct jr_regs *regs = (struct jr_regs *)CONFIG_SYS_FSL_JR0_ADDR;
	uint32_t jrcfg = in_be32(&regs->jrcfg1);

	jrcfg = jrcfg | JR_INTMASK;

	out_be32(&regs->jrcfg1, jrcfg);
	asm volatile("sync" : : : "memory");

}

static void jr_initregs(struct jobring *jr)
{
	struct jr_regs *regs = (struct jr_regs *)CONFIG_SYS_FSL_JR0_ADDR;
	phys_addr_t ip_base = virt_to_phys((void *)jr->input_ring);
	phys_addr_t op_base = virt_to_phys((void *)jr->output_ring);

#ifdef CONFIG_PHYS_64BIT
	out_be32(&regs->irba_h, ip_base >> 32);
#else
	out_be32(&regs->irba_h, 0x0);
#endif
	out_be32(&regs->irba_l, (uint32_t)ip_base);
#ifdef CONFIG_PHYS_64BIT
	out_be32(&regs->orba_h, op_base >> 32);
#else
	out_be32(&regs->orba_h, 0x0);
#endif
	out_be32(&regs->orba_l, (uint32_t)op_base);
	out_be32(&regs->ors, JR_SIZE);
	out_be32(&regs->irs, JR_SIZE);

	asm volatile("sync" : : : "memory");
	if (!jr->irq)
		jr_disable_irq();
}

int jr_init(struct jobring *jr)
{
	memset(jr, 0, sizeof(struct jobring));

	jr->jq_id = DEFAULT_JR_ID;
	jr->irq = DEFAULT_IRQ;

#ifdef CONFIG_FSL_CORENET
	jr->liodn = DEFAULT_JR_LIODN;
#endif
	jr->size = JR_SIZE;
	jr->input_ring = (dma_addr_t *) malloc(JR_SIZE * sizeof(dma_addr_t));
	if (!jr->input_ring)
		return -1;
	jr->output_ring =
	    (struct op_ring *)malloc(JR_SIZE * sizeof(struct op_ring));
	if (!jr->output_ring)
		return -1;

	memset(jr->input_ring, 0, JR_SIZE * sizeof(dma_addr_t));
	memset(jr->output_ring, 0, JR_SIZE * sizeof(struct op_ring));

	jr_initregs(jr);

	return 0;
}

int jr_sw_cleanup(struct jobring *jr)
{
	jr->head = 0;
	jr->tail = 0;
	jr->read_idx = 0;
	jr->write_idx = 0;
	memset(jr->info, 0, sizeof(jr->info));
	memset(jr->input_ring, 0, jr->size * sizeof(dma_addr_t));
	memset(jr->output_ring, 0, jr->size * sizeof(struct op_ring));

	return 0;
}

int sec_init(struct jobring *jr)
{
	int ret = 0;

#ifdef CONFIG_FSL_CORENET
	uint32_t liodn;
#endif

#ifdef CONFIG_PHYS_64BIT
	ccsr_sec_t *sec = (void *)CONFIG_SYS_FSL_SEC_ADDR;
	uint32_t mcr = in_be32(&sec->mcfgr);

	out_be32(&sec->mcfgr, mcr | 1 << MCFGR_PS_SHIFT);
#endif

#ifdef CONFIG_FSL_CORENET
	liodn = in_be32(&sec->jrliodnr[0].ls);
	liodn = liodn >> JRNSLIODN_SHIFT;
#endif
	ret = jr_init(jr);
	if (ret < 0)
		return -1;

#ifdef CONFIG_FSL_CORENET
	ret = sec_config_pamu_table(liodn);
	if (ret < 0)
		return -1;
#endif

	return ret;
}

int jr_hw_reset(void)
{
	struct jr_regs *regs = (struct jr_regs *)CONFIG_SYS_FSL_JR0_ADDR;
	uint32_t timeout = 100000;
	uint32_t jrint, jrcr;

	out_be32(&regs->jrcr, JRCR_RESET);
	do {
		jrint = in_be32(&regs->jrint);
	} while (((jrint & JRINT_ERR_HALT_MASK) ==
		  JRINT_ERR_HALT_INPROGRESS) && --timeout);

	jrint = in_be32(&regs->jrint);
	if (((jrint & JRINT_ERR_HALT_MASK) !=
	     JRINT_ERR_HALT_INPROGRESS) && timeout == 0)
		return -1;

	timeout = 100000;
	out_be32(&regs->jrcr, JRCR_RESET);
	do {
		jrcr = in_be32(&regs->jrcr);
	} while ((jrcr & JRCR_RESET) && --timeout);

	if (timeout == 0)
		return -1;

	return 0;
}

int jr_reset(struct jobring *jr)
{
	if (jr_hw_reset() < 0)
		return -1;

	/* Clean up the jobring structure maintained by software */
	jr_sw_cleanup(jr);

	return 0;
}

int sec_reset(void)
{
	ccsr_sec_t *sec = (void *)CONFIG_SYS_FSL_SEC_ADDR;
	uint32_t mcfgr = in_be32(&sec->mcfgr);
	uint32_t timeout = 100000;

	mcfgr |= MCFGR_SWRST;
	out_be32(&sec->mcfgr, mcfgr);

	mcfgr |= MCFGR_DMA_RST;
	out_be32(&sec->mcfgr, mcfgr);
	do {
		mcfgr = in_be32(&sec->mcfgr);
	} while ((mcfgr & MCFGR_DMA_RST) == MCFGR_DMA_RST && --timeout);

	if (timeout == 0)
		return -1;

	timeout = 100000;
	do {
		mcfgr = in_be32(&sec->mcfgr);
	} while ((mcfgr & MCFGR_SWRST) == MCFGR_SWRST && --timeout);

	if (timeout == 0)
		return -1;

	return 0;
}

/* -1 --- error, can't enqueue -- no space available */
int jr_enqueue(struct jobring *jr, uint32_t * desc_addr,
	       void (*callback) (uint32_t desc, uint32_t status, void *arg),
	       void *arg)
{
	struct jr_regs *regs = (struct jr_regs *)CONFIG_SYS_FSL_JR0_ADDR;
	int head = jr->head;
	dma_addr_t desc_phys_addr = virt_to_phys(desc_addr);

	if (in_be32(&regs->irsa) == 0
	    || CIRC_SPACE(jr->head, jr->tail, jr->size) <= 0)
		return ERROR_ESBC_SEC_ENQ;

	jr->input_ring[head] = desc_phys_addr;
	jr->info[head].desc_phys_addr = desc_phys_addr;
	jr->info[head].desc_addr = (uint32_t) desc_addr;
	jr->info[head].callback = (void *) callback;
	jr->info[head].arg = arg;
	jr->info[head].op_done = 0;

	jr->head = (head + 1) & (jr->size - 1);

	out_be32(&regs->irja, 1);
	asm volatile("sync" : : : "memory");

	return 0;
}

int jr_dequeue(struct jobring *jr)
{
	struct jr_regs *regs = (struct jr_regs *)CONFIG_SYS_FSL_JR0_ADDR;
	int head = jr->head;
	int tail = jr->tail;
	int idx, i, found;
	void (*callback) (uint32_t desc, uint32_t status, void *arg);
	void *arg = NULL;

	while (in_be32(&regs->orsf) && CIRC_CNT(jr->head, jr->tail, jr->size)) {
		found = 0;

		dma_addr_t op_desc = jr->output_ring[jr->tail].desc;
		uint32_t status = jr->output_ring[jr->tail].status;
		uint32_t desc_virt;

		for (i = 0; CIRC_CNT(head, tail + i, jr->size) >= 1; i++) {
			idx = (tail + i) & (jr->size - 1);
			if (op_desc == jr->info[idx].desc_phys_addr) {
				desc_virt = jr->info[idx].desc_addr;
				found = 1;
				break;
			}
		}

		/* Error condition if match not found */
		if (!found)
			return ERROR_ESBC_SEC_DEQ;

		jr->info[idx].op_done = 1;
		callback = (void *) jr->info[idx].callback;
		arg = jr->info[idx].arg;

		/* When the job on tail idx gets done, increment
		 * tail till the point where job completed out of oredr has
		 * been taken into account
		 */
		if (idx == tail)
			do {
				tail = (tail + 1) & (jr->size - 1);
			} while (jr->info[tail].op_done);

		jr->tail = tail;
		jr->read_idx = (jr->read_idx + 1) & (jr->size - 1);

		out_be32(&regs->orjr, 1);
		asm volatile("sync" : : : "memory");
		jr->info[idx].op_done = 0;

		callback(desc_virt, status, arg);
	}

	return 0;
}
