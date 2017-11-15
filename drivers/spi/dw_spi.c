/*
 * Ported from Linux - apm86xxx_spi.c
 *
 * Synopsys DesignWare SPI controller driver (master mode only)
 *
 * Copyright (c) 2010 Applied Micro Circuits Corporation.
 * All rights reserved. Anup Patel <apatel@apm.com>.
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
#include <spi.h>
#include <malloc.h>
#include <asm/processor.h>
#include <asm/io.h>
#include <asm/errno.h>
#include "dw_spi.h"

#ifdef CONFIG_APM86XXX
#include <asm/apm_scu.h>
#include <asm/apm_ipp_csr.h>
#endif

static inline u32 dwspi_read32(void* addr)
{
	return in_be32(addr);
}

static inline void dwspi_write32(void* addr, u32 val)
{
	out_be32(addr, val);
}

u32 dwspi_version(u32 base)
{
	return dwspi_read32((void *)DWSPI_SSI_VER(base));
} 

int dwspi_isenabled(u32 base) 
{
	u32 dwspi_stat;
	dwspi_stat = dwspi_read32((void *)DWSPI_SSIENR(base));
	if (dwspi_stat & DWSPI_SSIENR_SSI_EN_MASK)
		return 1;
	return 0;
}

void dwspi_enable(u32 base) 
{
	u32 dwspi_stat;
	dwspi_stat = dwspi_read32((void *)DWSPI_SSIENR(base));
	if (!(dwspi_stat & DWSPI_SSIENR_SSI_EN_MASK))
		dwspi_write32((void *)DWSPI_SSIENR(base), 0x1);
} 

void dwspi_disable(u32 base) 
{
	u32 dwspi_stat;
	dwspi_stat = dwspi_read32((void *)DWSPI_SSIENR(base));
	if (dwspi_stat & DWSPI_SSIENR_SSI_EN_MASK)
		dwspi_write32((void *)DWSPI_SSIENR(base), 0x0);
} 

void dwspi_enable_slave(u32 base, u32 slave_num) 
{
	u32 ser;
	if (DWSPI_MAXCS <= slave_num)
		return;
	ser = dwspi_read32((void *)DWSPI_SER(base));
	ser = ser | (0x1 << slave_num);
	dwspi_write32((void *)DWSPI_SER(base), ser);
}

void dwspi_disable_slave(u32 base, u32 slave_num) 
{
	u32 ser;
	if (DWSPI_MAXCS <= slave_num)
		return;
	ser = dwspi_read32((void *)DWSPI_SER(base));
	ser = ser & ~(0x1 << slave_num);
	dwspi_write32((void *)DWSPI_SER(base), ser);
} 

void dwspi_set_mode(u32 base, u32 mode) 
{
	u32 ctrlr0;
	u32 dwspi_stat;
	dwspi_stat = dwspi_read32((void *)DWSPI_SSIENR(base));
	if (dwspi_stat & DWSPI_SSIENR_SSI_EN_MASK)
		dwspi_write32((void *)DWSPI_SSIENR(base), 0x0);
	ctrlr0 = dwspi_read32((void *)DWSPI_CTRLR0(base));
	ctrlr0 &= ~DWSPI_CTRLR0_TMOD_MASK;
	ctrlr0 |= (DWSPI_CTRLR0_TMOD_EEPROM << DWSPI_CTRLR0_TMOD_SHIFT);
	ctrlr0 &= ~DWSPI_CTRLR0_DFS_MASK;
	ctrlr0 |= DWSPI_CTRLR0_DFS_VAL;
	if (mode & SPI_CPHA)
		ctrlr0 |= DWSPI_CTRLR0_SCPH_MASK;
	else
		ctrlr0 &= ~DWSPI_CTRLR0_SCPH_MASK;
	if (mode & SPI_CPOL)
		ctrlr0 |= DWSPI_CTRLR0_SCPOL_MASK;
	else
		ctrlr0 &= ~DWSPI_CTRLR0_SCPOL_MASK;
	dwspi_write32((void *)DWSPI_CTRLR0(base), ctrlr0);
	if (dwspi_stat & DWSPI_SSIENR_SSI_EN_MASK)
		dwspi_write32((void *)DWSPI_SSIENR(base), 0x1);
} 

void dwspi_baudcfg(u32 base, u32 ssi_clk, u32 speed_hz) 
{
	u32 div = (speed_hz) ? ssi_clk / speed_hz : 0xFFFE;
	u32 dwspi_stat;
	dwspi_stat = dwspi_read32((void *)DWSPI_SSIENR(base));
	if (dwspi_stat & DWSPI_SSIENR_SSI_EN_MASK)
		dwspi_write32((void *)DWSPI_SSIENR(base), 0x0);
	if (div % 2)
		div = div - div % 2;
	if (div == 0)
		div = 2;
	dwspi_write32((void *)DWSPI_BAUDR(base), div);
	if (dwspi_stat & DWSPI_SSIENR_SSI_EN_MASK)
		dwspi_write32((void *)DWSPI_SSIENR(base), 0x1);
} 

void dwspi_init(u32 base) 
{	
	/* Disable controller */ 
	dwspi_disable(base);

	/* Disable all the interrupts just in case */ 
	dwspi_write32((void *)DWSPI_IMR(base), DWSPI_IMR_VAL);

	/* Set TX full IRQ threshold */ 
	dwspi_write32((void *)DWSPI_TXFTLR(base), DWSPI_TXFTLR_VAL);
	
	/* Set RX empty IRQ threshold */ 
	dwspi_write32((void *)DWSPI_RXFTLR(base), DWSPI_RXFTLR_VAL);
	
	/* Set default mode */ 
	dwspi_set_mode(base, SPI_MODE_3);
	
	/* Disable chip select for all slaves */ 
	dwspi_write32((void *)DWSPI_SER(base), 0x0);
} 

int dwspi_eeprom_read(u32 base, u8 * tx, u32 tx_cnt, u8 * rx, u32 rx_cnt) 
{
	u32 tx_pos, rx_pos;
	u32 sr, old_ser, old_ctrlr0, old_ctrlr1;
	
	if ((DWSPI_TX_FIFO_DEPTH < tx_cnt) || 
	      ((DWSPI_CTRLR1_MASK + 1) < rx_cnt))
		return -1;
	
	/* Wait for previous transfer to complete */ 
	while (dwspi_read32((void *)DWSPI_SR(base)) & DWSPI_SR_BUSY_MASK) ;
	
	/* Save chip select and disable all slaves */ 
	old_ser = dwspi_read32((void *)DWSPI_SER(base));
	dwspi_write32((void *)DWSPI_SER(base), 0x0);
	
	/* Save control registers */ 
	old_ctrlr0 = dwspi_read32((void *)DWSPI_CTRLR0(base));
	old_ctrlr1 = dwspi_read32((void *)DWSPI_CTRLR1(base));
	
	/* Overwrite control registers */ 
	dwspi_disable(base);
	dwspi_write32((void *)DWSPI_CTRLR0(base), 
		  (old_ctrlr0 & ~DWSPI_CTRLR0_TMOD_MASK) | 
		  (DWSPI_CTRLR0_TMOD_EEPROM << DWSPI_CTRLR0_TMOD_SHIFT));
	dwspi_write32((void *)DWSPI_CTRLR1(base), (rx_cnt) ? (rx_cnt - 1) : 0);
	dwspi_enable(base);
	
	/* Write to TX FIFO */ 
	tx_pos = 0;
	while (tx_pos < tx_cnt) {
		sr = dwspi_read32((void *)DWSPI_SR(base));
		if (sr & DWSPI_SR_TFNF_MASK) {
			dwspi_write32((void *)DWSPI_DR(base), tx[tx_pos]);
			tx_pos++;
		}
	}

	/* Enable slaves to start transfer */ 
	dwspi_write32((void *)DWSPI_SER(base), old_ser);
	
	/* Read from RX FIFO */ 
	rx_pos = 0;
	while (rx_pos < rx_cnt) {
		sr = dwspi_read32((void *)DWSPI_SR(base));
		while (sr & DWSPI_SR_RFNE_MASK) {
			rx[rx_pos] = (u8) (dwspi_read32((void *)DWSPI_DR(base)));
			rx_pos++;
			sr = dwspi_read32((void *)DWSPI_SR(base));
		} 
	} 

	/* Wait for transfer complete */ 
	while (dwspi_read32((void *)DWSPI_SR(base)) & DWSPI_SR_BUSY_MASK) ;
	
	/* Restore control registers */ 
	dwspi_disable(base);
	dwspi_write32((void *)DWSPI_CTRLR0(base), old_ctrlr0);
	dwspi_write32((void *)DWSPI_CTRLR1(base), old_ctrlr1);
	dwspi_enable(base);

	return 0;
}

int dwspi_rx_dual(u32 base, u8 * rx1, u32 rx1_cnt, u8 * rx2, u32 rx2_cnt) 
{
	/* FIXME: Not required as of now */ 
	return 0;
}

int dwspi_tx_dual(u32 base, u8 * tx1, u32 tx1_cnt, u8 * tx2, u32 tx2_cnt) 
{
	u32 tx_pos;
	u32 sr, old_ser, old_ctrlr0;

	/* Wait for previous transfer to complete */ 
	while (dwspi_read32((void *)DWSPI_SR(base)) & DWSPI_SR_BUSY_MASK) ;

	/* Save chip select and disable all slaves */ 
	old_ser = dwspi_read32((void *)DWSPI_SER(base));
	dwspi_write32((void *)DWSPI_SER(base), 0x0);
	
	/* Save control registers */
	old_ctrlr0 = dwspi_read32((void *)DWSPI_CTRLR0(base));

	/* Overwrite control registers */ 
	dwspi_disable(base);
	dwspi_write32((void *)DWSPI_CTRLR0(base), 
		  (old_ctrlr0 & ~DWSPI_CTRLR0_TMOD_MASK) | 
		  (DWSPI_CTRLR0_TMOD_TXRX << DWSPI_CTRLR0_TMOD_SHIFT));
	dwspi_enable(base);
	
	/* Fill the TX FIFO */ 
	tx_pos = 0;
	while (tx_pos < (tx1_cnt + tx2_cnt)) {
		sr = dwspi_read32((void *)DWSPI_SR(base));
		if (sr & DWSPI_SR_TFNF_MASK) {
			if (tx_pos < tx1_cnt) {
				dwspi_write32((void *)DWSPI_DR(base), tx1[tx_pos]);
			} else {
				dwspi_write32((void *)DWSPI_DR(base),
					  tx2[tx_pos - tx1_cnt]);
			} 
			tx_pos++;
		} else {
			break;
		}
	}
	
	/* Enable slaves to start transfer */ 
	dwspi_write32((void *)DWSPI_SER(base), old_ser);
	
	/* Write remaing data to TX FIFO */ 
	while (tx_pos < (tx1_cnt + tx2_cnt)) {
		sr = dwspi_read32((void *)DWSPI_SR(base));
		if (sr & DWSPI_SR_TFNF_MASK) {
			if (tx_pos < tx1_cnt) {
				dwspi_write32((void *)DWSPI_DR(base), tx1[tx_pos]);
			} else {
				dwspi_write32((void *)DWSPI_DR(base),
					  tx2[tx_pos - tx1_cnt]);
			}
			tx_pos++;
		}
	} 

	/* Wait till TX FIFO is not empty */ 
	while (!(dwspi_read32((void *)DWSPI_SR(base)) & DWSPI_SR_TFE_MASK)) ;
	
	/* Wait for TX to complete */ 
	while (dwspi_read32((void *)DWSPI_SR(base)) & DWSPI_SR_BUSY_MASK) ;
	
	/* Restore control registers */ 
	dwspi_disable(base);
	dwspi_write32((void *)DWSPI_CTRLR0(base), old_ctrlr0);
	dwspi_enable(base);

	return 0;
}

struct dwspi_xfer {
	unsigned int count;
	u8 * tx_ptr;
	u8 * rx_ptr;
};

struct dwspi_context {
	u32 ssi_clk;		/* clock in Hz */
	struct spi_device *spi;	/* current spi slave device */
	struct dwspi_xfer xfer[2];
	int xfer_count;
};

static struct dwspi_context dwspi_cntxt;

/* (UBoot Interface Function) */ 
void spi_init(void) 
{
	u32 base = DWSPI_BASE;

	/*------------------------------------------------------------------
	 * Reset the SPI controller.
	 *-----------------------------------------------------------------*/ 
#ifdef CONFIG_APM86XXX
	struct apm86xxx_reset_ctrl reset_ctrl;
	
	/* Reset DWSPI */
	memset(&reset_ctrl,0,sizeof(reset_ctrl));
	reset_ctrl.clken_mask = SPI_F1_MASK;
	reset_ctrl.csr_reset_mask = SPI_MASK;
	reset_ctrl.reset_mask = SPI_F1_MASK;

	if (reset_apm86xxx_block(&reset_ctrl))
		return;
#endif

	/* Clear current context */ 
	memset((void *)&dwspi_cntxt, 0, sizeof(dwspi_cntxt));

	/* Initialize controller */ 
	dwspi_init(base);
#ifdef CONFIG_APM86XXX
	dwspi_init(base + 0x1000);
#endif
}

/* (UBoot Interface Function) */ 
int spi_claim_bus(struct spi_slave *slave) 
{
	u32 base = DWSPI_BASE;
#ifdef CONFIG_APM86XXX
	base += 0x1000 * slave->bus;
#endif
	if (dwspi_isenabled(base))
		return -EBUSY;
	dwspi_enable(base);
	return 0;
}

/* (UBoot Interface Function) */ 
void spi_release_bus(struct spi_slave *slave) 
{
	u32 base = DWSPI_BASE;
#ifdef CONFIG_APM86XXX
	base += 0x1000 * slave->bus;
#endif
	dwspi_disable(base);
} 

/* (UBoot Interface Function) */ 
struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs,
				  unsigned int max_hz, unsigned int mode) 
{
	u32 base = DWSPI_BASE;
	struct spi_device *spi_dev_ptr;
#ifdef CONFIG_APM86XXX
	u32 freq;
	base += 0x1000 * bus;
#endif

	/* Clear current context */ 
	memset((void *)&dwspi_cntxt, 0, sizeof(dwspi_cntxt));

	/* Check Version ID register for sanity */ 
	if (dwspi_version(base) != DWSPI_VERID)
		return NULL;

	/* Initialize controller */ 
	dwspi_init(base);

	/* Retrive input clock frequency */ 
	dwspi_cntxt.ssi_clk = CONFIG_DW_SPI_SYSCLK;
#ifdef CONFIG_APM86XXX
	if (apm86xxx_get_freq(APM86xxx_APB, &freq)==0)
		dwspi_cntxt.ssi_clk = freq;
#endif

	/* Allocate a dw slave device */ 
	spi_dev_ptr = malloc(sizeof(struct spi_device));
	if (!spi_dev_ptr)
		return NULL;

	memset((void *)spi_dev_ptr, 0, sizeof(struct spi_device));

	spi_dev_ptr->spi_slave.bus = bus;
	spi_dev_ptr->spi_slave.cs = cs;
	dwspi_cntxt.spi = spi_dev_ptr;
	
	/* Set mode */
	dwspi_set_mode(base, mode);

	/* Set speed */ 
	dwspi_baudcfg(base, dwspi_cntxt.ssi_clk, max_hz);
	
	/* Enable chip select */ 
	dwspi_enable_slave(base, cs);

	return &spi_dev_ptr->spi_slave;
}

/* (UBoot Interface Function) */ 
void spi_free_slave(struct spi_slave *spi) 
{
	struct spi_device *spi_dev = to_dw_spi(spi);

	free(spi_dev);
	    
	dwspi_cntxt.xfer_count = 0;
} 

/* (UBoot Interface Function) */ 
int spi_xfer(struct spi_slave *slave, unsigned int bitlen, const void *dout,
	     void *din, unsigned long flags) 
{
	int rc = 0;
	int irq_state;
	u32 base = DWSPI_BASE;

#ifdef CONFIG_APM86XXX
	base += 0x1000 * slave->bus;
#endif /* CONFIG_APM86XXX */
	
	/* Clear saved transfers */ 
	if (flags & SPI_XFER_BEGIN) {
		dwspi_cntxt.xfer_count = 0;
	}

	/* Save transfer requests */ 
	if (dwspi_cntxt.xfer_count < 2 && (dout != NULL || din != NULL)) {
		dwspi_cntxt.xfer[dwspi_cntxt.xfer_count].tx_ptr = (u8 *) dout;
		dwspi_cntxt.xfer[dwspi_cntxt.xfer_count].rx_ptr = (u8 *) din;
		dwspi_cntxt.xfer[dwspi_cntxt.xfer_count].count = bitlen / 8;
		dwspi_cntxt.xfer_count++;
	}

	/* Execute saved transfer requests */ 
	if (flags & SPI_XFER_END) {
		/* Disable interrupts to ensure atomicity */ 
		irq_state = disable_interrupts();
		/* Actual transfer */ 
		if (dwspi_cntxt.xfer_count == 1) {
			if (dwspi_cntxt.xfer[0].tx_ptr) {
				rc = dwspi_tx_dual(base,
						  dwspi_cntxt.xfer[0].tx_ptr,
						  dwspi_cntxt.xfer[0].count,
						  NULL, 0);
				dwspi_cntxt.xfer_count -= 1;
			} else if (dwspi_cntxt.xfer[1].rx_ptr) {
				rc = dwspi_rx_dual(base,
						  dwspi_cntxt.xfer[0].rx_ptr,
						  dwspi_cntxt.xfer[0].count,
						  NULL, 0);
				dwspi_cntxt.xfer_count -= 1;
			}
		} else if (dwspi_cntxt.xfer_count == 2) {
			if (dwspi_cntxt.xfer[0].tx_ptr &&
			    dwspi_cntxt.xfer[1].tx_ptr) {
				rc = dwspi_tx_dual(base,
						   dwspi_cntxt.xfer[0].tx_ptr,
						   dwspi_cntxt.xfer[0].count,
						   dwspi_cntxt.xfer[1].tx_ptr,
						   dwspi_cntxt.xfer[1].count);
				dwspi_cntxt.xfer_count -= 2;
			} else if (dwspi_cntxt.xfer[0].tx_ptr && 
			    dwspi_cntxt.xfer[0].count <= DWSPI_TX_FIFO_DEPTH &&
			    dwspi_cntxt.xfer[1].rx_ptr && 
			    dwspi_cntxt.xfer[0].count <= (DWSPI_CTRLR1_MASK + 1)) {
				rc = dwspi_eeprom_read(base,
						dwspi_cntxt.xfer[0].tx_ptr,
						dwspi_cntxt.xfer[0].count,
						dwspi_cntxt.xfer[1].rx_ptr,
						dwspi_cntxt.xfer[1].count);
				dwspi_cntxt.xfer_count -= 2;
			}
		}
		/* Enable interrupts */ 
		if (irq_state) {
			enable_interrupts();
		}
	}
	
	return rc;
}

/* (UBoot Interface Function) */ 
int spi_cs_is_valid(unsigned int bus, unsigned int cs) 
{
#ifdef CONFIG_APM86XXX
	return ((bus == 0 || bus == 1) && cs < DWSPI_MAXCS);
#else
	return (bus == 0 && cs < DWSPI_MAXCS);
#endif
}

/* (UBoot Interface Function) */ 
void spi_cs_activate(struct spi_slave *slave) 
{
	u32 base = DWSPI_BASE;
#ifdef CONFIG_APM86XXX
	base += 0x1000 * slave->bus;
#endif
	dwspi_enable_slave(base, slave->cs);
} 

/* (UBoot Interface Function) */ 
void spi_cs_deactivate(struct spi_slave *slave) 
{
	u32 base = DWSPI_BASE;
#ifdef CONFIG_APM86XXX
	base += 0x1000 * slave->bus;
#endif
	dwspi_disable_slave(base, slave->cs);
} 

