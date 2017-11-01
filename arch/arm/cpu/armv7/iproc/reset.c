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

#include <common.h>
#include <asm/io.h>
#include <asm/arch/iproc_regs.h>
#if (defined(CONFIG_NORTHSTAR) || defined(CONFIG_NS_PLUS))
#define RESET_BASE_ADDR CRU_RESET_OFFSET 
#else
#define RESET_BASE_ADDR DMU_CRU_RESET 
#endif
/*
 * Reset the cpu by setting software reset request bit
 */
void reset_cpu(ulong ignored)
{
#if (defined(CONFIG_NORTHSTAR) || defined(CONFIG_NS_PLUS))
	*(unsigned int *)(RESET_BASE_ADDR) = 0x1;
#else
	*(unsigned int *)(RESET_BASE_ADDR) = 0; /* Reset all */
#endif
	while (1) {
		// Never reached
	}
}
