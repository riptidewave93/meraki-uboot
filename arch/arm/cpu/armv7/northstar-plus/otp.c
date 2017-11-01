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
#include <config.h>
#include <common.h>
#include <asm/arch/iproc_regs.h>
#include "asm/arch/socregs.h"
#include "asm/arch/reg_utils.h"

uint32_t get_otp_mem(uint32_t offset, uint32_t *regval)
{
	uint32_t timeout;

	/* config OTP to enable CPU IF */
	reg32_write((volatile uint32_t *)CRU_OTP_CPU_CONFIG_OFFSET, 0x01); //258


	/* IF clean */
	reg32_write((volatile uint32_t *)CRU_OTP_CPU_CONTROL0_OFFSET, 0); //240
	reg32_write((volatile uint32_t *)CRU_OTP_CPU_CONTROL1_OFFSET, 0); //244
	reg32_write((volatile uint32_t *)CRU_OTP_CPU_ADDRESS_OFFSET, 0); //24c
	reg32_write((volatile uint32_t *)CRU_OTP_CPU_BITSEL_OFFSET, 0); //250

	timeout = 10000;
	while(timeout) {
		if(reg32_read((volatile uint32_t *)CRU_OTP_CPU_STATUS_OFFSET)& 0x1)
			break;
		timeout--;
	}
	if(!timeout) return(1);

	/* disable cmd */
	reg32_write((volatile uint32_t *)CRU_OTP_CPU_CONTROL0_OFFSET, 0x00a00600); //240
	reg32_write((volatile uint32_t *)CRU_OTP_CPU_CONTROL1_OFFSET, 0x00a81005); //244

	timeout = 10000;
	while(timeout) {
		if(reg32_read((volatile uint32_t *)CRU_OTP_CPU_STATUS_OFFSET)& 0x1)
			break;
		timeout--;
	}
	if(!timeout) return(1);

	reg32_write((volatile uint32_t *)CRU_OTP_CPU_CONTROL0_OFFSET, 0x00a00600); //240
	reg32_write((volatile uint32_t *)CRU_OTP_CPU_CONTROL1_OFFSET, 0x00a81004); //244


	/* offset data or address */
	reg32_write((volatile uint32_t *)CRU_OTP_CPU_ADDRESS_OFFSET, offset); //24c

	/* read cmd */
	reg32_write((volatile uint32_t *)CRU_OTP_CPU_CONTROL0_OFFSET, 0x00a00600); //240
	reg32_write((volatile uint32_t *)CRU_OTP_CPU_CONTROL1_OFFSET, 0x00881001); //244

	timeout = 10000;
	while(timeout) {
		if(reg32_read((volatile uint32_t *)CRU_OTP_CPU_STATUS_OFFSET)& 0x1)
			break;
		timeout--;
	}
	if(!timeout) return(1);

	/* read reg (offset) */
	*regval = reg32_read((volatile uint32_t *)CRU_OTP_CPU_RDATA_OFFSET);
	/* stop cmd */
	reg32_write((volatile uint32_t *)CRU_OTP_CPU_CONTROL0_OFFSET, 0x00a00600); //240
	reg32_write((volatile uint32_t *)CRU_OTP_CPU_CONTROL1_OFFSET, 0x00a81016); //244

	return(0);
}
