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
#include <spl.h>

DECLARE_GLOBAL_DATA_PTR;


u32 spl_boot_device(void)
{
#ifdef CONFIG_NAND_IPROC_BOOT
   return BOOT_DEVICE_NAND;
#else
   return BOOT_DEVICE_SPI;
#endif
}

/*
 * In the context of SPL, board_init_f must ensure that any clocks/etc for
 * DDR are enabled, ensure that the stack pointer is valid, clear the BSS
 * and call board_init_f.  We provide this version by default but mark it
 * as __weak to allow for platforms to do this in their own way if needed.
 */
void board_init_f(ulong dummy)
{

    /* Set the stack pointer. */
    asm volatile("mov sp, %0\n" : : "r"(CONFIG_SPL_STACK));

    /* Clear the BSS. */
    memset(__bss_start, 0, __bss_end__ - __bss_start);

    gd = &gdata;
    memset((void *)gd, 0, sizeof(gd_t));

    /* Console initialization */
    preloader_console_init();
    timer_init();
    dram_init();
    board_init_r(NULL, 0);
}
