/*
 * Copyright (C) 2013, Broadcom Corporation. All Rights Reserved.
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef __IPROC_H
#define __IPROC_H 1
#include <linux/types.h>
#include <asm/arch/iproc_regs.h>

extern iproc_clk_struct iproc_clk;

/* APIs */
/* Function to get current CPU id */
unsigned int get_cpuid(void);

/* Function to enable interrupt at distributor level */
void gic_enable_interrupt(uint8_t intid);
/* Function to disable interrupt at distributor level */
void gic_disable_interrupt(uint8_t intid);
/* Function to set Software Generated Interrupt */
void gic_set_sgi(uint8_t intid, uint8_t filter, uint8_t targets);
/* Function to configure the interrupt properties in GIC distributor */
void gic_config_interrupt(uint8_t intid, uint8_t secure_mode, uint8_t trig_mode, uint8_t priority, uint8_t targets);
/* Function to install interrupt handler */
void irq_install_handler (int irq, interrupt_handler_t handle_irq, void *data);
void systick_isr(void *data);
int l2cc_enable();
void scu_enable();

#endif /*__IPROC_H */
