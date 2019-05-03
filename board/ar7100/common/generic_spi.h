/*
 * Copyright (c) 2009, Atheros Communications Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 */
/*****************************************************************************/
/*! file generic_spi.h
** /brief Generic SPI interface support
**    
** This interface file contains the definitions for the SPI interface functions.
** It requires the i2c support.
**
*/



/* generic_spi.h */

#ifndef _GENERIC_SPI_H
#define _GENERIC_SPI_H

#ifndef CEXTERN
#define  CEXTERN static inline
#endif

#include "generic_i2c.h"

#define GENERIC_SPI_VSC73XX_CS    GENERIC_I2C_VSC73XX_CS
#define GENERIC_SPI_STEREO_CS     GENERIC_I2C_STEREO_CS
#define GENERIC_SPI_SLIC_CS0      GENERIC_I2C_SLIC_CS0
#define GENERIC_SPI_SLIC_CS1      GENERIC_I2C_SLIC_CS1

int
generic_spi_init(int cs);

int
generic_spi_access_enable(int cs);

int
generic_spi_access_done(void);

void
generic_spi_raw_output_u8(unsigned char val);

unsigned int 
generic_spi_raw_input_u32(void);

unsigned int 
generic_spi_raw_input_u8(void);

#endif
