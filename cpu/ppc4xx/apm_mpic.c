/*
 * Copyright (c) 2010 Applied Micro Circuits Corporation.
 * All rights reserved. Loc Ho <lho@apm.com>.
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

#include <common.h>
#include <watchdog.h>
#include <command.h>
#include <asm/processor.h>
#include <asm/interrupt.h>
#include <ppc4xx.h>
#include <ppc_asm.tmpl>
#include <commproc.h>
#include <asm/mp.h>
#include <asm/io.h>

DECLARE_GLOBAL_DATA_PTR;

#if 0
#define MPIC_DEBUG		printf
#else
#define MPIC_DEBUG(x, ...)
#endif

#define MPIC_EXINT_MAX                  	12
#define MPIC_ININT_MAX                  	96
#define MPIC_GTINT_MAX				8
#define MPIC_ININT_OFFSET			0x10200
#define MPIC_MSGINT_OFFSET			0x01600
#define MPIC_GTINT_OFFSET			0x01100
#define MPIC_EXTINT_OFFSET			0x10000

#define MPIC_CPU_INTACK                 	0x000a0
#define MPIC_CPU_EOI                    	0x000b0

/*
 * Per-source registers
 */
#define MPIC_IRQ_STRIDE                 	0x00020
#define MPIC_IRQ_VECTOR_PRI             	0x00000
#define         MPIC_VECPRI_MASK                0x80000000
#define         MPIC_VECPRI_ACTIVITY            0x40000000 /* Read Only */
#define         MPIC_VECPRI_PRIORITY_MASK       0x000f0000
#define         MPIC_VECPRI_PRIORITY_SHIFT      16
#define         MPIC_VECPRI_VECTOR_MASK         0x000007ff
#define         MPIC_VECPRI_POLARITY_POSITIVE	0x00800000
#define         MPIC_VECPRI_POLARITY_NEGATIVE   0x00000000
#define         MPIC_VECPRI_POLARITY_MASK       0x00800000
#define         MPIC_VECPRI_SENSE_LEVEL         0x00400000
#define         MPIC_VECPRI_SENSE_EDGE          0x00000000
#define         MPIC_VECPRI_SENSE_MASK          0x00400000
#define MPIC_IRQ_DESTINATION            	0x00010

#define mpic_read(r) 		in_be32((void *) (r))
#define mpic_write(r, v) 	out_be32((void *) (r), (v))

static unsigned int pic_vec_map_addr(unsigned int vec)
{
	unsigned int ivpr_addr;

	if (vec < MPIC_ININT_MAX) {
		/* Internal interrupt sources */
		ivpr_addr = CONFIG_SYS_MPIC_BASE + MPIC_ININT_OFFSET + 
				vec * MPIC_IRQ_STRIDE;
	} else if (vec < (MPIC_ININT_MAX + MPIC_EXINT_MAX))
		/* external interrupt sourcess */
		ivpr_addr = CONFIG_SYS_MPIC_BASE + MPIC_EXTINT_OFFSET + 
				(vec - MPIC_ININT_MAX) * MPIC_IRQ_STRIDE;
	else if (vec < (MPIC_ININT_MAX + MPIC_EXINT_MAX + MPIC_GTINT_MAX)) {
		/* timer interrupt sources */
		ivpr_addr = CONFIG_SYS_MPIC_BASE + MPIC_GTINT_OFFSET +
				(vec - MPIC_ININT_MAX - MPIC_EXINT_MAX - 
				 MPIC_GTINT_MAX) * 0x40 + 0x20;
	} else {
		/* message interrupt sources */
		ivpr_addr = CONFIG_SYS_MPIC_BASE + MPIC_MSGINT_OFFSET +
				(vec - MPIC_ININT_MAX - MPIC_EXINT_MAX) * 
				MPIC_IRQ_STRIDE;
	}
	return ivpr_addr;
}

void pic_enable(void)
{
	int i;
	
	if ((mfspr(DBSR) & 0x30000000) != 0x00000000)
		return;		/* On core reset, do not re-initialize MPIC */
	
	for (i = 0; i < IRQ_MAX; i++) {
		u32 vec_addr = pic_vec_map_addr(i);
		u32 vecpri   = mpic_read(vec_addr);
		vecpri &= ~MPIC_VECPRI_VECTOR_MASK;
		vecpri |= MPIC_VECPRI_MASK | i;				
		mpic_write(vec_addr, vecpri);
		mpic_write(vec_addr + MPIC_IRQ_DESTINATION, 1 << CPUID);
		MPIC_DEBUG("MPIC: set IRQ %d addr 0x%08X pri 0x%08X "
			"dest 0x%08X\n", i, vec_addr, vecpri, 1 << CPUID);
	}
}

/*
 * Handle external interrupts
 */
void external_interrupt(struct pt_regs *regs)
{
	/* MPIC Interrupt Acknowledge Register */
	unsigned int vec = mpic_read(CONFIG_SYS_MPIC_BASE + MPIC_CPU_INTACK);
	
	if (vec <  IRQ_MAX)
		interrupt_run_handler(vec);
	else
		pic_irq_ack(vec);
}

void pic_irq_ack(unsigned int vec)
{
	/* MPIC end of interrupt */
	mpic_write(CONFIG_SYS_MPIC_BASE + MPIC_CPU_EOI, 0);	
}

/*
 * Install and free a interrupt handler.
 */
void pic_irq_enable(unsigned int vec)
{
	unsigned int ivpr_addr;
	unsigned int val;

	MPIC_DEBUG("MPIC: enable IRQ %d destination 0x%02X\n", vec, 1<<CPUID);		
	
	ivpr_addr = pic_vec_map_addr(vec);
	mpic_write(ivpr_addr + MPIC_IRQ_DESTINATION, 1 << CPUID);
	val  = mpic_read(ivpr_addr);
	val &= ~MPIC_VECPRI_MASK; /* Clear mask to enable */
	mpic_write(ivpr_addr, val);
}

void pic_irq_disable(unsigned int vec)
{
	unsigned int ivpr_addr;
	unsigned int val;
	
	MPIC_DEBUG("MPIC: disable IRQ %d\n", vec);		
	
	ivpr_addr = pic_vec_map_addr(vec);
	val  = mpic_read(ivpr_addr);
	val |= MPIC_VECPRI_MASK; /* Set mask to disable */
	mpic_write(ivpr_addr, val);
}
