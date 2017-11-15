/*
 * designware.h - platform glue for the Synopsys DesignWare SPI controller
 */  
#ifndef __DW_SPI_H__
#define	__DW_SPI_H__
    
#include <spi.h>
    
/* Register definitions as per "DesignWare DW_apb_ssi Databook", Capture 6. */ 
#define	DWSPI_TX_FIFO_DEPTH		(CONFIG_DW_SPI_FIFO_DEPTH)
#define DWSPI_RX_FIFO_DEPTH		(CONFIG_DW_SPI_FIFO_DEPTH)
#define DWSPI_SYSCLK			(CONFIG_DW_SPI_SYSCLK)
#define DWSPI_MAXCS			(CONFIG_DW_SPI_MAX_CS)
#define DWSPI_VERID			(CONFIG_DW_SPI_VER_ID)
#define	DWSPI_BASE			(CONFIG_DW_SPI_BASE)
#define DWSPI_CTRLR0(base)		((base) + 0x00)
#define DWSPI_CTRLR1(base)		((base) + 0x04)
#define DWSPI_SSIENR(base)		((base) + 0x08)
#define DWSPI_MWCR(base)		((base) + 0x0C)
#define DWSPI_SER(base)			((base) + 0x10)
#define DWSPI_BAUDR(base)		((base) + 0x14)
#define DWSPI_TXFTLR(base)		((base) + 0x18)
#define DWSPI_RXFTLR(base)		((base) + 0x1C)
#define DWSPI_TXFLR(base)		((base) + 0x20)
#define DWSPI_RXFLR(base)		((base) + 0x24)
#define DWSPI_SR(base)			((base) + 0x28)
#define DWSPI_IMR(base)			((base) + 0x2C)
#define DWSPI_ISR(base)			((base) + 0x30)
#define DWSPI_RISR(base)		((base) + 0x34)
#define DWSPI_TXOICR(base)		((base) + 0x38)
#define DWSPI_RXOICR(base)		((base) + 0x3C)
#define DWSPI_RXUICR(base)		((base) + 0x40)
#define DWSPI_ICR(base)			((base) + 0x44)
#define DWSPI_DMACR(base)		((base) + 0x4C)
#define DWSPI_DMATDLR(base)		((base) + 0x50)
#define DWSPI_DMARDLR(base)		((base) + 0x54)
#define DWSPI_IDR(base)			((base) + 0x58)
#define DWSPI_SSI_VER(base)		((base) + 0x5C)
#define DWSPI_DR(base)			((base) + 0x60)
    
#define DWSPI_SSIENR_SSI_EN_MASK	0x1
    
#define DWSPI_CTRLR0_DFS_MASK		0x000f
#define DWSPI_CTRLR0_DFS_VAL		0x7
#define DWSPI_CTRLR0_SCPOL_MASK		0x0080
#define DWSPI_CTRLR0_SCPH_MASK		0x0040
#define DWSPI_CTRLR0_TMOD_MASK		0x0300
#define DWSPI_CTRLR0_TMOD_SHIFT		8
#define DWSPI_CTRLR0_TMOD_TXRX		0x0
#define DWSPI_CTRLR0_TMOD_RX		0x1
#define DWSPI_CTRLR0_TMOD_TX		0x2
#define DWSPI_CTRLR0_TMOD_EEPROM	0x3
    
#define DWSPI_CTRLR1_MASK		0xFFFF
    
#define DWSPI_SR_BUSY_MASK		0x01
#define DWSPI_SR_TFNF_MASK		0x02
#define DWSPI_SR_TFE_MASK		0x04
#define DWSPI_SR_RFNE_MASK		0x08
#define DWSPI_SR_RFF_MASK		0x10
    
#define DWSPI_ISR_TXEIS_MASK		0x01
#define DWSPI_ISR_RXFIS_MASK		0x10
#define DWSPI_IMR_TXEIM_MASK		0x01
#define DWSPI_IMR_RXFIM_MASK		0x10
#define DWSPI_IMR_VAL			0x0
    
#define DWSPI_TXFTLR_VAL		(DWSPI_TX_FIFO_DEPTH-1)
#define DWSPI_RXFTLR_VAL		0x0
    
#define DWSPI_TUNNING_DELAY		0x100000
    
struct spi_device {

	struct spi_slave spi_slave;
	u32 max_speed_hz;
	u8 chip_select;
	u8 mode;
	u8 bits_per_word;
	int irq;
	void *cont_state;
	char modalias[32];
	    /*
	     * likely need more hooks for more protocol options affecting how
	     * the controller talks to each chip, like:
	     *  - memory packing (12 bit samples into low bits, others zeroed)
	     *  - priority
	     *  - drop chipselect after each word
	     *  - chipselect delays
	     *  - ...
	     */ 
};

static inline struct spi_device *to_dw_spi(struct spi_slave *spi_slave) 
{
	return container_of(spi_slave, struct spi_device, spi_slave);
}

u32 dwspi_version(u32 base);
int dwspi_isenabled(u32 base);
void dwspi_enable(u32 base);
void dwspi_disable(u32 base);
void dwspi_enable_slave(u32 base, u32 slave_num);
void dwspi_disable_slave(u32 base, u32 slave_num);
void dwspi_set_mode(u32 base, u32 mode);
void dwspi_baudcfg(u32 base, u32 ssi_clk, u32 speed_hz);
int dwspi_rx_dual(u32 base, u8 * rx1, u32 rx1_cnt, u8 * rx2, u32 rx2_cnt);
int dwspi_tx_dual(u32 base, u8 * tx1, u32 tx1_cnt, u8 * tx2, u32 tx2_cnt);
int dwspi_eeprom_read(u32 base, u8 * tx, u32 tx_cnt, u8 * rx, u32 rx_cnt);
void dwspi_init(u32 base);

#endif	/* __DW_SPI_H__ */
