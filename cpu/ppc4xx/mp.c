/*
 * Copyright (c) 2010 Applied Micro Circuits Corporation.
 * All rights reserved. Feng Kan <fkan@apm.com>.
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
#include <asm/processor.h>
#include <asm/mmu.h>
#include <ioports.h>
#include <lmb.h>
#include <asm/io.h>
#include <asm/mp.h>

DECLARE_GLOBAL_DATA_PTR;

#define MPIC_FRR	0x1000
#define MPIC_PRR	0x1070

/*
 * Board-specific Platform code can reimplement board_cpu_reset and
 * board_cpu_release if needed
 */
void __board_cpu_reset(int nr)
{
}
void board_cpu_reset(int nr)
	__attribute__ ((weak, alias("__board_cpu_reset")));

void __board_cpu_release(int nr)
{
}
void board_cpu_release(int nr)
	__attribute__ ((weak, alias("__board_cpu_release")));

int cpu_reset(int nr)
{
	u32 reset;
        volatile u32 *prr = (void *)(CONFIG_SYS_MPIC_BASE + MPIC_PRR);

	board_cpu_reset(nr);

        reset = in_be32(prr);
        out_be32(prr, reset | (1 << nr));
	udelay(1000);
        out_be32(prr, reset & ~(1 << nr));
	return 0;
}

int cpu_status(int nr)
{
	u32 reset;
        volatile u32 *prr = (void *) (CONFIG_SYS_MPIC_BASE + MPIC_PRR);

        reset = in_be32(prr);
	debug("CPU PRR: 0x%x\n", reset);
	return reset;
}

int cpu_release(int nr, int argc, char *argv[])
{
	u32 reset;
        volatile u32 *prr = (void *)(CONFIG_SYS_MPIC_BASE + MPIC_PRR);

	/* For now, ignore the arguments */
        reset = in_be32(prr);
        if (reset & (1 << nr)) {
		board_cpu_release(nr);
		out_be32(prr, reset & ~(1 << nr));
	}
	return 0;
}

u32 determine_mp_bootpg(void)
{
	return 0;
}

void cpu_mp_lmb_reserve(struct lmb *lmb)
{
}

/*
 * Copy the code for other cpus to execute into an
 * aligned location accessible via BPTR
 */
void setup_mp(void)
{
}

int cpu_numcores() 
{
        return ((in_be32((void *) (CONFIG_SYS_MPIC_BASE + 0x1000)) >> 8)
        		& 0x1F) + 1;
}

void mp_msg_trigger(unsigned int message_index, unsigned int message_value, 
		unsigned int message_dest)
{
        /* setup message interrupt vector priority register*/
        *((u32 *)(CONFIG_SYS_MPIC_BASE + 0x11600 + 
        	0x20 * message_index)) = 0xf0000 + 116 + message_index;
        /* setup message interrupt destination register */
        *((u32 *)(CONFIG_SYS_MPIC_BASE + 0x11600 + 
        	0x20 * message_index + 0x10)) = 1<<message_dest;
        /* Message Enable Register */
        *((u32 *)(CONFIG_SYS_MPIC_BASE + 0x1500)) = 1 << message_index;
        /* Message Register */
        *((u32 *)(CONFIG_SYS_MPIC_BASE + 0x1400 + 
        	0x10 * message_index)) = message_value;
}
