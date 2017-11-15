/*
 * APM SoC APM86xxx I2C driver using Synopsys Designware core (master only) 
 *
 * Copyright (c) 2010 Applied Micro Circuits Corporation.
 * All rights reserved. Loc Ho <lho@apm.com>.
 *                      Ravi Patel <rapatel@apm.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#ifndef __DW_I2C_H__
#define __DW_I2C_H__

#define SS_SCL_HCNT	250
#define SS_SCL_LCNT	250
#define FS_SCL_HCNT	62
#define FS_SCL_LCNT	63

#define IC_SAR		0x55

#define MAX_BUF_LEN	128
#define I2C_BASE	CONFIG_SYS_PERIPHERAL_BASE

/*
 * Registers offset
 */
#define DW_IC_CON		0x0
#define DW_IC_TAR		0x4
#define DW_IC_SAR		0x8
#define DW_IC_DATA_CMD		0x10
#define DW_IC_SS_SCL_HCNT	0x14
#define DW_IC_SS_SCL_LCNT	0x18
#define DW_IC_FS_SCL_HCNT	0x1c
#define DW_IC_FS_SCL_LCNT	0x20
#define DW_IC_HS_SCL_HCNT	0x24
#define DW_IC_HS_SCL_LCNT	0x28
#define DW_IC_INTR_STAT		0x2c
#define DW_IC_INTR_MASK		0x30
#define DW_IC_RAW_INTR_STAT	0x34
#define DW_IC_CLR_INTR		0x40
#define DW_IC_CLR_RX_UNDER	0x44
#define DW_IC_CLR_RX_OVER	0x48
#define DW_IC_CLR_TX_ABRT	0x54
#define DW_IC_CLR_ACTIVITY	0x5c
#define DW_IC_CLR_STOP_DET	0x60
#define DW_IC_CLR_START_DET	0x64
#define DW_IC_ENABLE		0x6c
#define DW_IC_STATUS		0x70
#define DW_IC_TXFLR		0x74
#define DW_IC_RXFLR		0x78
#define DW_IC_TX_ABRT_SOURCE	0x80
#define DW_IC_ENABLE_STATUS	0x9c
#define DW_IC_COMP_PARAM_1	0xf4
#define DW_IC_COMP_TYPE		0xfc

#define DW_SIGNATURE		0x44570000	/* 'D' 'W' */
#define DW_LE_BUS		0x1234
#define DW_BE_BUS		0x4321

#define DW_IC_CON_MASTER		0x1
#define DW_IC_CON_SPEED_STD		0x2
#define DW_IC_CON_SPEED_FAST		0x4
#define DW_IC_CON_10BITADDR_MASTER	0x10
#define DW_IC_CON_RESTART_EN		0x20
#define DW_IC_CON_SLAVE_DISABLE		0x40

#define DW_IC_TAR_10BITS		(1 << 12)

#define DW_IC_INTR_RX_UNDER	0x01
#define DW_IC_INTR_RX_OVER	0x02
#define DW_IC_INTR_TX_EMPTY	0x10
#define DW_IC_INTR_TX_ABRT	0x40
#define DW_IC_INTR_ACTIVITY	0x100
#define DW_IC_INTR_STOP_DET	0x200
#define DW_IC_INTR_START_DET	0x400
#define DW_IC_ERR_CONDITION	(DW_IC_INTR_RX_UNDER | \
				 DW_IC_INTR_RX_OVER | \
				 DW_IC_INTR_TX_ABRT)

#define DW_IC_STATUS_ACTIVITY	0x1

#define DW_IC_ERR_TX_ABRT	0x1

/*
 * status codes
 */
#define STATUS_IDLE			0x0
#define STATUS_WRITE_IN_PROGRESS	0x1
#define STATUS_READ_IN_PROGRESS		0x2

/* timeout and retry values */
#define DW_BUS_WAIT_TIMEOUT	20 /* ms */
#define DW_RX_DATA_RETRY	5
#define DW_TX_DATA_RETRY	5
#define DW_STATUS_WAIT_RETRY	5
#define DW_BUS_WAIT_INACTIVE	10

/*
 * hardware abort codes from the DW_IC_TX_ABRT_SOURCE register
 *
 * only expected abort codes are listed here
 * refer to the datasheet for the full list
 */
#define ABRT_7B_ADDR_NOACK	0
#define ABRT_10ADDR1_NOACK	1
#define ABRT_10ADDR2_NOACK	2
#define ABRT_TXDATA_NOACK	3
#define ABRT_GCALL_NOACK	4
#define ABRT_GCALL_READ		5
#define ABRT_SBYTE_ACKDET	7
#define ABRT_SBYTE_NORSTRT	9
#define ABRT_10B_RD_NORSTRT	10
#define ARB_MASTER_DIS		11
#define ARB_LOST		12
#define ARB_OK			0xFF

/* I2C SCL counter macros */
enum {
	I2C_SS = 0,
	I2C_FS,
	I2C_HS_400PF,
	I2C_HS_100PF,
};
#define I2C_HS I2C_HS_400PF

enum {
	I2C_SCL_HIGH = 0,
	I2C_SCL_LOW,
	I2C_SCL_TF,
};

#define byte_swap32(u32b) \
	((u32b & 0xFF000000) >> 24 | \
	 (u32b & 0x00FF0000) >>  8 | \
	 (u32b & 0x0000FF00) <<  8 | \
	 (u32b & 0x000000FF) << 24)

/* Bus specific values */
typedef struct dw_i2c_s {
	u32 base;
	u32 bus_endian;
	u32 bus_speed;
	u32 rx_buffer;
	u32 tx_buffer;
	u32 device_addr;
	u32 device_addr_len;
	u32 page_write_size;
	u32 poll_time;
	u32 clk_base;
	u32 init_done;
} dw_i2c_t;

#if defined(CONFIG_I2C_MULTI_BUS)
#define GET_BUS_NUM	i2c_bus_num
#else
#define GET_BUS_NUM	0
#endif /* CONFIG_I2C_MULTI_BUS */

#define sys_i2c_base	(dw_i2c[GET_BUS_NUM].base)
#define i2c_bus_endian	(dw_i2c[GET_BUS_NUM].bus_endian)
#define i2c_bus_speed	(dw_i2c[GET_BUS_NUM].bus_speed)
#define i2c_rx_buffer	dw_i2c[GET_BUS_NUM].rx_buffer
#define i2c_tx_buffer	dw_i2c[GET_BUS_NUM].tx_buffer
#define i2c_device_addr	dw_i2c[GET_BUS_NUM].device_addr
#define i2c_device_addr_len	dw_i2c[GET_BUS_NUM].device_addr_len
#define i2c_page_write_size	dw_i2c[GET_BUS_NUM].page_write_size
#define i2c_poll_time	dw_i2c[GET_BUS_NUM].poll_time
#define i2c_clk_base	dw_i2c[GET_BUS_NUM].clk_base
#define i2c_init_done	dw_i2c[GET_BUS_NUM].init_done

#endif
