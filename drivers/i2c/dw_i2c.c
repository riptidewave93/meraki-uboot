/*
 * APM SoC APM86xxx I2C driver using Synopsys Designware core (master only) 
 *
 * Copyright (c) 2010 Applied Micro Circuits Corporation.
 * All rights reserved. Loc Ho <lho@apm.com>.
 *                      Ravi Patel <rapatel@apm.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */
#include <common.h>
#include <ppc4xx.h>
#include <i2c.h>
#include <asm-ppc/io.h>
#ifdef CONFIG_APM86XXX
#include <asm/apm_scu.h>
#include <asm/apm_ipp_csr.h>
#endif
#include "dw_i2c.h"

#ifdef CONFIG_HARD_I2C

DECLARE_GLOBAL_DATA_PTR;

#if defined(CONFIG_I2C_MULTI_BUS)
/* 
 * Initialize the bus pointer to whatever one the SPD EEPROM is on.
 * Default is bus 0.  This is necessary because the DDR initialization
 * runs from ROM, and we can't switch buses because we can't modify
 * the global variables.
 */
#ifndef CONFIG_SYS_SPD_BUS_NUM
#define CONFIG_SYS_SPD_BUS_NUM	0
#endif
static unsigned int i2c_bus_num __attribute__ ((section (".data"))) = CONFIG_SYS_SPD_BUS_NUM;
#endif /* CONFIG_I2C_MULTI_BUS */

/**
 * The physical address of the APM86xxx I2C1 is so far apart from the I2C0, a
 * TLB is required to map the virtual address closer to the I2C0.
 * The TLB at 0xD.0040.0000 is mapped to virtual address 0xA020.0000.
 */
#define I2C0_BUS_OFFSET_TLB	(0x00002000)		/* From AHBC I2C0 */
#define I2C1_BUS_OFFSET_TLB	(0x00200000 + 0x11000)	/* From Deep sleep fabric I2C1 */

#define i2c_sync() { __asm__ ("sync;");}

static unsigned int i2c_hw_init_done __attribute__ ((section (".data"))) = 0;

/* Keeping i2c_poll in micro second, 10 times the signaling period */
dw_i2c_t dw_i2c[CONFIG_SYS_MAX_I2C_BUS] = {
	{
		.base = (I2C_BASE + I2C0_BUS_OFFSET_TLB),
		.bus_endian = DW_BE_BUS,
		.bus_speed = CONFIG_SYS_I2C_SPEED,
		.rx_buffer = CONFIG_DW_I2C_RX_BUFFER_DEPTH,
		.tx_buffer = CONFIG_DW_I2C_TX_BUFFER_DEPTH,
		.device_addr = CONFIG_SYS_I2C_EEPROM_ADDR,
		.device_addr_len = CONFIG_SYS_I2C_EEPROM_ADDR_LEN,
		.page_write_size = CONFIG_SYS_I2C_EEPROM_PAGE_WRITE_LEN,
		.poll_time = ((10 * 1000000) / CONFIG_SYS_I2C_SPEED),
#ifdef CONFIG_APM86XXX
		.clk_base = APM86xxx_APB,
#endif
		.init_done = 1,
	},
	{
		.base = (I2C_BASE + I2C1_BUS_OFFSET_TLB),
#ifdef CONFIG_APM86XXX
		.bus_endian = DW_LE_BUS,
		.bus_speed = CONFIG_SYS_I2C1_SPEED,
#else
		.bus_endian = DW_BE_BUS,
		.bus_speed = CONFIG_SYS_I2C_SPEED,
#endif
		.rx_buffer = CONFIG_DW_I2C_RX_BUFFER_DEPTH,
		.tx_buffer = CONFIG_DW_I2C_TX_BUFFER_DEPTH,
#ifdef CONFIG_APM86XXX
		.device_addr = CONFIG_SYS_I2C1_EEPROM_ADDR,
		.device_addr_len = CONFIG_SYS_I2C1_EEPROM_ADDR_LEN,
		.page_write_size = CONFIG_SYS_I2C1_EEPROM_PAGE_WRITE_LEN,
		.poll_time = ((10 * 1000000) / CONFIG_SYS_I2C1_SPEED),
		.clk_base = APM86xxx_IAHB,
#else
		.device_addr = CONFIG_SYS_I2C_EEPROM_ADDR,
		.device_addr_len = CONFIG_SYS_I2C_EEPROM_ADDR_LEN,
		.page_write_size = CONFIG_SYS_I2C_EEPROM_PAGE_WRITE_LEN,
		.poll_time = ((10 * 1000000) / CONFIG_SYS_I2C_SPEED),
#endif
		.init_done = 1,
	},
};

static u32 i2c_scl_min[4][3] = {	/* in nano seconds */
	/* High, Low, tf */
	[I2C_SS] =	 {4000, 4700, 300},	/* SS (Standard Speed) */
	[I2C_FS] =	 { 600, 1300, 300},	/* FS (Fast Speed) */
	[I2C_HS_400PF] = { 160,  320, 300},	/* HS (High Speed) 400pf */
	[I2C_HS_100PF] = {  60,  120, 300},	/* HS (High Speed) 100pf */
};

static unsigned int read32(unsigned int addr)
{
	unsigned int value;

	if (i2c_bus_endian == DW_LE_BUS)
		value = in_le32((volatile unsigned *) addr);
	else
		value = in_be32((volatile unsigned *) addr);

	debug("I2C RD 0x%x val 0x%08X\n", addr, value);
	return value;
}

static void write32(unsigned int addr, unsigned int val)
{
	if (i2c_bus_endian == DW_LE_BUS)
		out_le32((volatile unsigned *) addr, val);
	else
		out_be32((volatile unsigned *) addr, val);

	debug("I2C WR 0x%x val 0x%08X\n", addr, val);
}

static void i2c_hw_init(void)
{
	int i;
	u32 i2c_base, param;
	u32 dw_signature, dw_signature_swap32;

#ifdef CONFIG_APM86XXX
	u32 scu_reg;

	apm86xxx_read_scu_reg(SCU_SOCDIV3_ADDR, &scu_reg);
	if (!IICCLK_MUX_SEL3_RD(scu_reg)) {
		scu_reg |= IICCLK_MUX_SEL3_WR(1);
		apm86xxx_write_scu_reg(SCU_SOCDIV3_ADDR, scu_reg);
	}
#endif
	for (i = 0; i < CONFIG_SYS_MAX_I2C_BUS; i++) {
		i2c_base = dw_i2c[i].base;
		dw_signature = in_be32((volatile unsigned *)(i2c_base + DW_IC_COMP_TYPE));
		dw_signature_swap32 = byte_swap32(dw_signature);

		if ((dw_signature & DW_SIGNATURE) != DW_SIGNATURE &&
			(dw_signature_swap32 & DW_SIGNATURE) == DW_SIGNATURE) {
			dw_i2c[i].bus_endian = DW_LE_BUS;
			debug("[ I2C Bus %d LE ] ", i);
			param = in_le32((volatile unsigned *)(i2c_base + DW_IC_COMP_PARAM_1));
		} else {
			dw_i2c[i].bus_endian = DW_BE_BUS;
			debug("[ I2C Bus %d BE ] ", i);
			param = in_be32((volatile unsigned *)(i2c_base + DW_IC_COMP_PARAM_1));
		}
		dw_i2c[i].rx_buffer = ((param >> 8) & 0xff) + 1;
		dw_i2c[i].tx_buffer = ((param >> 16) & 0xff) + 1;
	}
	i2c_hw_init_done = 1;
}

/*
 * Check for errors on i2c bus
 */
static int i2c_dw_check_errors(unsigned int i2c_base, int print_err)
{
	u32 i2c_status_cnt = DW_STATUS_WAIT_RETRY;
	u32 tx_abrt = 0;
	int status = read32(i2c_base + DW_IC_RAW_INTR_STAT);

	if (status & DW_IC_INTR_RX_UNDER)
		read32(i2c_base + DW_IC_CLR_RX_UNDER);

	if (status & DW_IC_INTR_RX_OVER)
		read32(i2c_base + DW_IC_CLR_RX_OVER);

	if (status & DW_IC_INTR_TX_ABRT) {
		tx_abrt = read32(i2c_base + DW_IC_TX_ABRT_SOURCE);
		read32(i2c_base + DW_IC_CLR_TX_ABRT);
		if (print_err)
			printf("TX_ABORT ");
	}

	if (status & DW_IC_ERR_CONDITION) {
		if (print_err)
			printf("Errors on i2c bus %08x %08x: ",
				status, tx_abrt);
		/* Disable the adapter */
		write32(i2c_base + DW_IC_ENABLE, 0);
		do {
			udelay(i2c_poll_time);
			i2c_sync();
			if (i2c_status_cnt == 0)
				printf("I2C disable timeout: ");
		} while (((read32(i2c_base + DW_IC_ENABLE_STATUS) & 0x01))
				&& i2c_status_cnt--);
		i2c_init_done = 0;
		/* Unset the target adddress */
		write32(i2c_base + DW_IC_TAR, 0);
	}

	return (status & DW_IC_ERR_CONDITION);
}

/*
 * Waiting for bus not busy
 */
static int i2c_dw_wait_bus_not_busy(unsigned int i2c_base)
{
	int timeout = DW_BUS_WAIT_TIMEOUT;

	while (read32(i2c_base + DW_IC_STATUS) & 0x20) {
		if (timeout <= 0) {
			printf("I2C: timeout waiting for bus ready\n");
			return -1;
		}
		timeout--;
		udelay(1000);
	}
	return 0;
}

/*
 * Check for successful response from slave
 */
static int i2c_dw_check_status(unsigned int i2c_base, int print_err)
{
	int status = read32(i2c_base + DW_IC_RAW_INTR_STAT);

	if (status & DW_IC_INTR_ACTIVITY)
		read32(i2c_base + DW_IC_CLR_ACTIVITY);

	if (status & DW_IC_INTR_STOP_DET)
		read32(i2c_base + DW_IC_CLR_STOP_DET);

	if (status & DW_IC_INTR_START_DET)
		read32(i2c_base + DW_IC_CLR_START_DET);

	status = i2c_dw_check_errors(i2c_base, print_err);

	if ((status & DW_IC_ERR_CONDITION) && print_err) {
		printf("device response\n");
	}

	return (status & DW_IC_ERR_CONDITION);
}

/*
 * Waiting for TX FIFO buffer available
 */
static int i2c_dw_wait_tx_data(unsigned int i2c_base)
{
	int timeout = DW_TX_DATA_RETRY;

	while (read32(i2c_base + DW_IC_TXFLR) == i2c_tx_buffer) {
		if (timeout <= 0) {
			printf("I2C: timeout waiting for TX buffer available\n");
			return -1;
		}
		timeout--;
		udelay(i2c_poll_time);
	}
	return 0;
}

static u32
i2c_dw_scl_hcnt(u32 ic_clk, u32 tSYMBOL, u32 tf, int cond, int offset)
{
	/*
	 * DesignWare I2C core doesn't seem to have solid strategy to meet
	 * the tHD;STA timing spec.  Configuring _HCNT based on tHIGH spec
	 * will result in violation of the tHD;STA spec.
	 */
	if (cond)
		/*
		 * Conditional expression:
		 *
		 *   IC_[FS]S_SCL_HCNT + (1+4+3) >= IC_CLK * tHIGH
		 *
		 * This is based on the DW manuals, and represents an ideal
		 * configuration.  The resulting I2C bus speed will be
		 * faster than any of the others.
		 *
		 * If your hardware is free from tHD;STA issue, try this one.
		 */
		return ((ic_clk * tSYMBOL + 500000) / 1000000) - 8 + offset;
	else
		/*
		 * Conditional expression:
		 *
		 *   IC_[FS]S_SCL_HCNT + 3 >= IC_CLK * (tHD;STA + tf)
		 *
		 * This is just experimental rule; the tHD;STA period turned
		 * out to be proportinal to (_HCNT + 3).  With this setting,
		 * we could meet both tHIGH and tHD;STA timing specs.
		 *
		 * If unsure, you'd better to take this alternative.
		 *
		 * The reason why we need to take into account "tf" here,
		 * is the same as described in i2c_dw_scl_lcnt().
		 */
		return ((ic_clk * (tSYMBOL + tf) + 500000) / 1000000) - 3 + offset;
}

static u32 i2c_dw_scl_lcnt(u32 ic_clk, u32 tLOW, u32 tf, int offset)
{
	/*
	 * Conditional expression:
	 *
	 *   IC_[FS]S_SCL_LCNT + 1 >= IC_CLK * (tLOW + tf)
	 *
	 * DW I2C core starts counting the SCL CNTs for the LOW period
	 * of the SCL clock (tLOW) as soon as it pulls the SCL line.
	 * In order to meet the tLOW timing spec, we need to take into
	 * account the fall time of SCL signal (tf).  Default tf value
	 * should be 0.3 us, for safety.
	 */
	return ((ic_clk * (tLOW + tf) + 500000) / 1000000) - 1 + offset;
}

/**
 * i2c_scl_init() - initialize the designware i2c scl counts
 *
 * This functions configures scl clock count for SS, FS, and HS.
 * This function is called during I2C init function.
 */
static void i2c_scl_init(u32 i2c_base, u32 i2c_clk_freq, u32 i2c_speed)
{
	u32 input_clock_khz = i2c_clk_freq / 1000;
	u32 hcnt, lcnt;
	u32 ss_clock_khz = input_clock_khz;
	u32 fs_clock_khz = input_clock_khz;
	u32 hs_clock_khz = input_clock_khz;
	u32 i2c_speed_khz = i2c_speed / 1000;

	if (i2c_speed_khz < 100)
		ss_clock_khz = (ss_clock_khz * 100) / i2c_speed_khz;
	else if (i2c_speed_khz > 100 && i2c_speed_khz < 400)
		fs_clock_khz = (fs_clock_khz * 400) / i2c_speed_khz;
	else if (i2c_speed_khz > 400 && i2c_speed_khz < 3400)
		hs_clock_khz = (hs_clock_khz * 3400) / i2c_speed_khz;

	/* set standard, fast and High speed dividers for high/low periods */

	/* Standard speed mode */
	hcnt = i2c_dw_scl_hcnt(ss_clock_khz,
		i2c_scl_min[I2C_SS][I2C_SCL_HIGH],	/* tHD;STA = tHIGH = 4.0 us */
		i2c_scl_min[I2C_SS][I2C_SCL_TF],	/* tf = 0.3 us */
		0,	/* 0: DW default, 1: Ideal */
		0);	/* No offset */
	lcnt = i2c_dw_scl_lcnt(ss_clock_khz,
		i2c_scl_min[I2C_SS][I2C_SCL_LOW],	/* tLOW = 4.7 us */
		i2c_scl_min[I2C_SS][I2C_SCL_TF],	/* tf = 0.3 us */
		0);	/* No offset */
	write32(i2c_base + DW_IC_SS_SCL_HCNT, hcnt); 
	write32(i2c_base + DW_IC_SS_SCL_LCNT, lcnt); 

	/* Fast speed mode */
	hcnt = i2c_dw_scl_hcnt(fs_clock_khz,
		i2c_scl_min[I2C_FS][I2C_SCL_HIGH],	/* tHD;STA = tHIGH = 0.6 us */
		i2c_scl_min[I2C_FS][I2C_SCL_TF],	/* tf = 0.3 us */
		0,	/* 0: DW default, 1: Ideal */
		0);	/* No offset */
	lcnt = i2c_dw_scl_lcnt(fs_clock_khz,
		i2c_scl_min[I2C_FS][I2C_SCL_LOW],	/* tLOW = 1.3 us */
		i2c_scl_min[I2C_FS][I2C_SCL_TF],	/* tf = 0.3 us */
		0);	/* No offset */
	write32(i2c_base + DW_IC_FS_SCL_HCNT, hcnt); 
	write32(i2c_base + DW_IC_FS_SCL_LCNT, lcnt); 

	/* High speed mode */
	hcnt = i2c_dw_scl_hcnt(hs_clock_khz,
		i2c_scl_min[I2C_HS][I2C_SCL_HIGH],	/* tHD;STA = tHIGH = 0.06 us for 100pf 0.16 for 400pf */
		i2c_scl_min[I2C_HS][I2C_SCL_TF],	/* tf = 0.3 us */
		0,	/* 0: DW default, 1: Ideal */
		0);	/* No offset */
	lcnt = i2c_dw_scl_lcnt(hs_clock_khz,
		i2c_scl_min[I2C_HS][I2C_SCL_LOW],	/* tLOW = 0.12 us for 100pf 0.32 us for 400pf */
		i2c_scl_min[I2C_HS][I2C_SCL_TF],	/* tf = 0.3 us */
		0);	/* No offset */
	write32(i2c_base + DW_IC_HS_SCL_HCNT, hcnt); 
	write32(i2c_base + DW_IC_HS_SCL_LCNT, lcnt); 
}

/**
 * i2c_init() - initialize the designware i2c master hardware
 * This functions configures and enables the I2C master.
 */
void i2c_init(int speed, int slaveadd)
{
	u32 i2c_status_cnt;
	u32 i2c_base;
	u16 ic_con;
	u32 i2c_clk_freq = CONFIG_SYS_CLK_FREQ;
#ifdef CONFIG_APM86XXX
	u32 freq = 0;
#endif
	
#ifdef CONFIG_SYS_I2C_INIT_BOARD
	/* call board specific i2c bus reset routine before accessing the   */
	/* environment, which might be in a chip on that bus. For details   */
	/* about this problem see doc/I2C_Edge_Conditions.                  */
	i2c_init_board();
#endif

	if (!i2c_hw_init_done)
		i2c_hw_init();

	i2c_base = sys_i2c_base;
	if (i2c_bus_speed != speed) {
		i2c_bus_speed = speed;
		i2c_eeprom_speed = speed;
	}
#ifdef CONFIG_APM86XXX
	/* Get I2C CLK frequency */
	if (apm86xxx_get_freq(i2c_clk_base, &freq) == 0)
		i2c_clk_freq = freq;
#endif
	if (i2c_bus_num)
		i2c_poll_time = (10 * 1000000) / speed;
	else
		i2c_poll_time = ((10 * 1000000) / speed) * (i2c_clk_freq / 50000000);

	i2c_status_cnt = DW_STATUS_WAIT_RETRY;
	/* Disable the adapter and interrupt */
	write32(i2c_base + DW_IC_ENABLE, 0);
	do {
		udelay(i2c_poll_time);
		i2c_sync();
		if (i2c_status_cnt == 0) {
			printf("I2C disable timeout\n");
			i2c_init_done = 0;
			/* Unset the target adddress */
			write32(i2c_base + DW_IC_TAR, 0);
			return;
		}
	} while (((read32(i2c_base + DW_IC_ENABLE_STATUS) & 0x01)) && i2c_status_cnt--);
	write32(i2c_base + DW_IC_INTR_MASK, 0);
	/* set standard and fast speed deviders for high/low periods */
	i2c_scl_init(i2c_base, i2c_clk_freq, speed);
	/* Write target address */
	write32(i2c_base + DW_IC_TAR, slaveadd);
	/* Write slave address */
	write32(i2c_base + DW_IC_SAR, IC_SAR); /* Address */
		
	ic_con = DW_IC_CON_MASTER | DW_IC_CON_SLAVE_DISABLE | DW_IC_CON_RESTART_EN;
	
	if (speed > 400000 && speed <= 3400000)
		ic_con |= (DW_IC_CON_SPEED_STD | DW_IC_CON_SPEED_FAST);
	else if (speed > 100000 && speed <= 400000)
		ic_con |= DW_IC_CON_SPEED_FAST;
	else
		ic_con |= DW_IC_CON_SPEED_STD;

	write32(i2c_base + DW_IC_CON, ic_con);

	i2c_status_cnt = DW_STATUS_WAIT_RETRY;
	/* Enable the adapter */
	write32(i2c_base + DW_IC_ENABLE, 1);
	do {
		udelay(i2c_poll_time);
		i2c_sync();
		if (i2c_status_cnt == 0) {
			printf("I2C enable timeout\n");
			i2c_init_done = 0;
			/* Unset the target adddress */
			write32(i2c_base + DW_IC_TAR, 0);
			return;
		}
	} while ((!(read32(i2c_base + DW_IC_ENABLE_STATUS) & 0x01)) && i2c_status_cnt--);

	i2c_init_done = 1;
}

static int i2c_set_target(u8 chip)
{
	unsigned int i2c_base = sys_i2c_base;
	int rc = 0;

	if (!i2c_init_done || read32(i2c_base + DW_IC_TAR) != chip) {
		i2c_init(i2c_bus_speed, chip);
	
		/* Check if TAR is set */
		if (read32(i2c_base + DW_IC_TAR) != chip) {
			printf("Cannot set the target on I2c bus to %x\n",
				chip);
			rc = -1;
		}
	}

	return rc;
}

static __inline__ int __i2c_write(uchar chip, uint addr, int alen, uchar *data, int length)
{
        int i;
	int offset = 0;
	u32 page_size = i2c_page_write_size;
	unsigned int i2c_base = sys_i2c_base;
	
	if (i2c_dw_wait_bus_not_busy(i2c_base))
		return -1;

	while (length > (page_size - 1)) {

		if (alen == 2) {
			write32(i2c_base + DW_IC_DATA_CMD, ((addr >> 8) & 0xFF)); /* Write Addr */
		}

		write32(i2c_base + DW_IC_DATA_CMD, (addr & 0xFF)); /* Write Addr */

		for (i = 0; i < page_size; i++) {
			write32(i2c_base + DW_IC_DATA_CMD, data[offset + i]);

			if (i2c_dw_wait_tx_data(i2c_base))
				return -1;
		}

		i2c_dw_wait_bus_not_busy(i2c_base);
		udelay(3000);
		length -= i;
		addr += i;
		offset += i;
	}

	for (i = 0; i < length; i++) {

		if (alen == 2)
			write32(i2c_base + DW_IC_DATA_CMD, (((addr + i) >> 8) & 0xFF)); /* Write Addr */

		write32(i2c_base + DW_IC_DATA_CMD, ((addr + i) & 0xFF)); /* Write Addr */
		write32(i2c_base + DW_IC_DATA_CMD, data[offset + i]);	
		i2c_dw_wait_bus_not_busy(i2c_base);
		udelay(3000);
        }

	return 0;
}

static __inline__ int __i2c_read(uchar chip, uint addr, int alen, uchar *data, int length)
{
        int i;
	int offset = 0;
	int count = 0;
	unsigned int rx_fifo = i2c_rx_buffer;
	u32 t_i2c_poll = i2c_poll_time;
	unsigned int i2c_base = sys_i2c_base;

	if (i2c_dw_wait_bus_not_busy(i2c_base)) 
		return -1;
	
	i2c_sync();
	debug("Write: %d @ %x\n", addr, i2c_base + DW_IC_DATA_CMD);

	if (alen == 2)
		write32(i2c_base + DW_IC_DATA_CMD, ((addr >> 8) & 0xFF)); /* Write Addr */

	if (alen > 0)
		write32(i2c_base + DW_IC_DATA_CMD, ((addr) & 0xFF)); /* Write Addr */

	while (offset < length) {
		if (i2c_dw_wait_bus_not_busy(i2c_base))
			return -1;

		i2c_sync();
		if (i2c_dw_check_errors(i2c_base, alen)) {
			if (alen)
				printf("writing chip %02x register offset %04x\n", chip, addr);
			return -1;
		}

		count = (((length - offset) < rx_fifo) ? (length - offset) : rx_fifo);
		for (i = 0; i < count; i++) {
			write32(i2c_base + DW_IC_DATA_CMD, 0x100); /* Read Command */

			i2c_sync();
			if (i2c_dw_check_errors(i2c_base, alen)) {
				if (alen)
					printf("reading chip %02x register "
						"offset %04x start %04x length %d\n",
						chip, offset + i, addr, length);
				return -1;
			}

			udelay(t_i2c_poll / 10);
		}

		/* Extra delay required to read the 1st byte */
		if (offset == 0)
			udelay(t_i2c_poll * 6);

		/* Extra delay required when a byte is received in 1st FIFO buffer */
		udelay((t_i2c_poll * 15) / 10);

		for (i = 0; i < count; i++) {
			i2c_sync();
			if (i2c_dw_check_errors(i2c_base, alen)) {
				if (alen)
					printf("reading chip %02x data "
						"offset %04x start %04x length %d\n",
						chip, offset + i, addr, length);
				return -1;
			}

			/* Delay required to consecutive bytes received in FIFO */
			udelay((t_i2c_poll * 25) / 10);
			data[offset + i] = read32(i2c_base + DW_IC_DATA_CMD);
		}

		offset += count;
	}

	i2c_dw_wait_bus_not_busy(i2c_base);

	return i2c_dw_check_status(i2c_base, alen);
}

int i2c_write(uchar chip, uint addr, int alen, uchar * buf, int len)
{
	int rc = -1; /* signal error */

	if (i2c_set_target(chip) == 0)
		rc = __i2c_write(chip, addr, alen, buf, len);

	return rc;
}

int i2c_read(uchar chip, uint addr, int alen, uchar * buf, int len)
{
	int rc = -1; /* signal error */

	if (i2c_set_target(chip) == 0)
		rc = __i2c_read(chip, addr, alen, buf, len);

	return rc;
}

int i2c_probe(uchar chip)
{
	unsigned char buf[1];

	i2c_init(i2c_bus_speed, chip);
	return i2c_read(chip, 0, 0, buf, 1);
}

#if defined(CONFIG_I2C_MULTI_BUS)
/*
 * Functions for multiple I2C bus handling
 */
unsigned int i2c_get_bus_num(void)
{
	return i2c_bus_num;
}

int i2c_set_bus_num(unsigned int bus)
{
	if (bus >= CONFIG_SYS_MAX_I2C_BUS)
		return -1;

	i2c_bus_num = bus;
	i2c_eeprom_speed = i2c_bus_speed;
	i2c_eeprom_addr = i2c_device_addr;
	i2c_eeprom_addr_len = i2c_device_addr_len;
	return 0;
}
#endif	/* CONFIG_I2C_MULTI_BUS */

unsigned int i2c_get_bus_speed(void)
{
	return i2c_bus_speed;
}

int i2c_set_bus_speed(unsigned int speed)
{
	if (speed > 0 && speed <= 3400000) {
		if (i2c_bus_speed != speed)
			i2c_init(speed, i2c_device_addr);
		return 0;
	}

	return -1;
}
#endif	/* CONFIG_HARD_I2C */
