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

#ifndef	_bcm5301x_cca_gsio_h_
#define	_bcm5301x_cca_gsio_h_

/* SPI device number :
 *  - Supports only one device.
 */
#define CCA_SPI_NUM_DEV 1

/* SPI Mode :
 *  - Supports only one mode 0.
 */
typedef enum cca_spi_mode_e {
    CCA_SPI_MODE_CPOL_0_CPHA_0 = 0,

    CCA_SPI_NUM_MODE
}cca_spi_mode_t;

/* reutrn value for SPI driver */
#define SPI_ERR_NONE          0
#define SPI_ERR_TIMEOUT       -1
#define SPI_ERR_INTERNAL      -2
#define SPI_ERR_PARAM         -3
#define SPI_ERR_UNAVAIL       -4
#define SPI_ERR_UNKNOW        -5

#define SPI_PRESENT                                                   0x180000ac

/* GSIO register definition */
#define  GSIO_CTRL                                                    0x180000e4
#define  GSIO_CTRL_BASE                                                    0x0e4
#define  GSIO_CTRL_DATAMASK                                           0xffffffff
#define  GSIO_CTRL_RDWRMASK                                           0x00000000
#define  GSIO_ADDR                                                    0x180000e8
#define  GSIO_ADDR_BASE                                                    0x0e8
#define  GSIO_ADDR_DATAMASK                                           0xffffffff
#define  GSIO_ADDR_RDWRMASK                                           0x00000000
#define  GSIO_DATA                                                    0x180000ec
#define  GSIO_DATA_BASE                                                    0x0ec
#define  GSIO_DATA_DATAMASK                                           0xffffffff
#define  GSIO_DATA_RDWRMASK                                           0x00000000

#define  CLKDIV2                                                      0x180000f0
#define  CLKDIV2_BASE                                                      0x0f0
#define  CLKDIV2_DATAMASK                                             0xffffffff
#define  CLKDIV2_RDWRMASK                                             0x00000000

/* fields in GSIO_CTRL */
#define GSIO_CTRL_GO_MASK		0x000000ff
#define GSIO_CTRL_GO_SHIFT	    0
#define GSIO_CTRL_GC_MASK		0x00000700
#define GSIO_CTRL_GC_SHIFT		8
#define GSIO_CTRL_NOA_MASK	    0x00003000
#define GSIO_CTRL_NOA_SHIFT  	12
#define GSIO_CTRL_NOX_MASK	    0x0000c000
#define GSIO_CTRL_NOX_SHIFT  	14
#define GSIO_CTRL_NOD_MASK	    0x00030000
#define GSIO_CTRL_NOD_SHIFT  	16
#define GSIO_CTRL_GG_MASK	    0x00200000
#define GSIO_CTRL_GG_SHIFT  	21
#define GSIO_CTRL_ENDIAN_MASK	0x00400000
#define GSIO_CTRL_ENDIAN_SHIFT 	22
#define GSIO_CTRL_ERROR_MASK	0x00800000
#define GSIO_CTRL_ERROR_SHIFT 	23
#define GSIO_CTRL_MODE_MASK	    0x40000000
#define GSIO_CTRL_MODE_SHIFT 	30
#define GSIO_CTRL_SB_MASK	    0x80000000
#define GSIO_CTRL_SB_SHIFT 	    31

/* fields in CLKDIV2 */
#define CLKDIV2_GD_MASK	        0x001ffff0
#define CLKDIV2_GD_SHIFT 	    4

/* external functions for SPI driver */
extern int cca_spi_cs_release(void);
extern int cca_spi_read(uint8_t *buf, int len);
extern int cca_spi_write(uint8_t *buf, int len);
extern unsigned char cca_spi_read_command(uint8_t *buf, int len);
extern unsigned char cca_spi_write_command( uint8_t *buf, int len );
extern int cca_spi_freq_set(int speed_hz);
extern int cca_spi_init(void);
extern int cca_spi_exit(void);

#endif /* _bcm5301x_cca_gsio_h_ */
