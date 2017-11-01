/*
 * nand_spl - Startup Code for iProc CPU-core
 *
 * Copyright (c) 2013	Broadcom Corporation <support@broadcom.com>
 *
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <asm-offsets.h>
#include <config.h>
#include <version.h>
#include <asm/system.h>
#include <linux/linkage.h>

.globl _start
_start: b relocate_spl

/* Copy u-boot-spl image from FLASH to sram */
relocate_spl:
        ldr     r0, =(CONFIG_SPL_UBOOT_START)
        ldr     r1, =(CONFIG_SPL_TEXT_BASE)
        ldr     r2, =(CONFIG_SPL_UBOOT_END)
relocate_spl_loop:
	ldmia   r0!, {r3-r10}           /* copy from source address [r0]    */
        stmia   r1!, {r3-r10}           /* copy to   target address [r1]    */
        cmp     r0, r2                  /* until source end address [r2]    */
        beq     jump_to_spl
        b       relocate_spl_loop
/* Jump to u-boot-spl image entry point */
jump_to_spl:
        ldr     pc, =(CONFIG_SPL_TEXT_BASE)
