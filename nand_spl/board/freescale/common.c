/*
 * Copyright 2012 Freescale Semiconductor, Inc.
 * Author: Matthew McClintock <msm@freescale.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 */



#include <common.h>
#include <asm/processor.h>

extern u32 bus_clk;

void udelay(unsigned long usec) {
	u32 ticks_per_usec = bus_clk / (8 * 1000000);
	u32 ticks = ticks_per_usec * usec;
	u32 s = mfspr(SPRN_TBRL);

	while ((mfspr(SPRN_TBRL)-s) < ticks);
}
