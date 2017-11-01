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
#include <pci.h>
#include <asm/io.h>
#include <post.h>

#include "pcie_iproc.h"



#define USE_PORT_0    1
unsigned long  PORT_IN_USE = 0;
unsigned long  OUT_PCI_ADDR = 0x08000000;
unsigned long  PCI_SLOT_ADDR = 0x18012000;
u32 linkError;

//#define PCI_SLOT_ADDR   0x18012000
//#define OUT_PCI_ADDR    0x08000000


/*
 * Per port control structure
 */
static struct soc_pcie_port {
	void * reg_base;
} soc_pcie_ports[3] = {
	{
	.reg_base = 0x18012000,
	},
	{
	.reg_base = 0x18013000,    
	},
	{
	.reg_base = 0x18014000,
	}
};


static int conf_trace = 0;

static void pci_dump_standard_conf(struct soc_pcie_port * port);

static void pci_dump_extended_conf(struct soc_pcie_port * port);


static int __pci_bus0_find_next_cap_ttl(unsigned int devfn,
		u8 pos, int cap, int *ttl)
{
	u8 id;

	while ((*ttl)--) {
		pci_bus0_read_config_byte( devfn, pos, &pos);
		if (pos < 0x40)
			break;
		pos &= ~3;
		pci_bus0_read_config_byte(devfn, pos + PCI_CAP_LIST_ID,
				&id);
		if (id == 0xff)
			break;
		if (id == cap)
			return pos;
		pos += PCI_CAP_LIST_NEXT;
	}
	return 0;
}
#define PCI_FIND_CAP_TTL        48

 static int __pci_bus0_find_next_cap( unsigned int devfn,
                                u8 pos, int cap)
 {
         int ttl = PCI_FIND_CAP_TTL;

         return __pci_bus0_find_next_cap_ttl( devfn, pos, cap, &ttl);
 }


static int __pci_bus0_find_cap_start(unsigned int devfn, u8 hdr_type)
 {
         u16 status;

         pci_bus0_read_config_word( devfn, PCI_STATUS, &status);
         if (!(status & PCI_STATUS_CAP_LIST))
                 return 0;

         switch (hdr_type) {
         case PCI_HEADER_TYPE_NORMAL:
         case PCI_HEADER_TYPE_BRIDGE:
                 return PCI_CAPABILITY_LIST;
         case PCI_HEADER_TYPE_CARDBUS:
                 return PCI_CB_CAPABILITY_LIST;
         default:
                 return 0;
         }

         return 0;
}
static int pci_bus0_find_capability( unsigned int devfn, int cap)
{
         int pos;
         u8 hdr_type;

         pci_bus0_read_config_byte(devfn, PCI_HEADER_TYPE, &hdr_type);

         pos = __pci_bus0_find_cap_start( devfn, hdr_type & 0x7f);
         if (pos)
                 pos = __pci_bus0_find_next_cap( devfn, pos, cap);

         return pos;
 }
//Link status register definitions



//3:0
//RO
//Link Speed. The negotiated Link speed.
//   0001b = 2.5 Gb/s
//All other encodings are reserved.

//9:4
//RO
//Negotiated Link Width. The negotiated Link width.
//   000001b = x1
//   000010b = x2
//   000100b = x4
//   001000b = x8
//   001100b = x12
//   010000b = x16
//   100000b = x32
//   All other encodings are reserved.

//10
//RO
//Training Error. 1 = indicates that a Link training error occurred. Reserved on Endpoint devices and Switch upstream ports.
//Cleared by hardware upon successful training of the Link to the L0 Link state.

//11
//RO
//Link Training. When set to one, indicates that Link training is in progress (Physical Layer LTSSM is
//in the Configuration or Recovery state) or that the Retrain Link bit was set to one but Link training has not yet begun.
//Hardware clears this bit once Link training is complete.
// This bit is not applicable and reserved on Endpoint devices and the Upstream Ports of Switches.

//12
//HWInit
//Slot Clock Configuration. This bit indicates that the component uses the same physical reference clock
//that the platform provides on the connector. If the device uses an independent clock irrespective of the
//presence of a reference on the connector, this bit must be clear.

static int soc_pcie_check_link(struct soc_pcie_port * port)
{
    u32 devfn = 0;
	u16 pos, tmp16;
	u8 nlw, tmp8;


	linkError = 0;

	/* See if the port is in EP mode, indicated by header type 00 */
    pci_bus0_read_config_byte(devfn, PCI_HEADER_TYPE, &tmp8);
	if( tmp8 != PCI_HEADER_TYPE_BRIDGE ) {
		printf("PCIe port %d in End-Point mode - ignored\n");
		linkError = 1;
		return -1;
	}
    else
        printf("PCIe port in RC mode\n");

	/* NS PAX only changes NLW field when card is present */
    pos = pci_bus0_find_capability( devfn, PCI_CAP_ID_EXP);

    printf(L"\n pos is %d\n", pos);

    pci_bus0_read_config_word(devfn, pos + PCI_EXP_LNKSTA, &tmp16);

	printf("==>PCIE: LINKSTA reg %#x val %#x\n",
		pos+PCI_EXP_LNKSTA, tmp16 );



	nlw = (tmp16 & PCI_EXP_LNKSTA_NLW) >> PCI_EXP_LNKSTA_NLW_SHIFT ;
	if ( nlw == 0 )
	{
		linkError = 1;
	}
	//port->link = tmp16 & PCI_EXP_LNKSTA_DLLLA ;

	//if( nlw != 0 ) port->link = 1;

	return( (nlw)? 0: -1);
}

/*
 * Initializte the PCIe controller
 */
static void  soc_pcie_hw_init(struct soc_pcie_port * port)
{
	/* Turn-on Root-Complex (RC) mode, from reset defailt of EP */

	/* The mode is set by straps, can be overwritten via DMU
	   register <cru_straps_control> bit 5, "1" means RC
	 */

	/* Send a downstream reset */
	__raw_writel( 0x3, port->reg_base + SOC_PCIE_CONTROL);
	udelay(2500);
	__raw_writel( 0x1, port->reg_base + SOC_PCIE_CONTROL);
	udelay(100000);

	printf("\n soc_pcie_hw_init : port->reg_base = 0x%x , its value = 0x%x \n", port->reg_base, __raw_readl(port->reg_base + SOC_PCIE_CONTROL));
	/* TBD: take care of PM, check we're on */
}


int iproc_pcie_rd_conf_dword(struct pci_controller *hose, pci_dev_t dev, int offset, u32 * value)
{
	if(conf_trace) printf("Pcie_rd_conf_dword: dev: %08x <B%d, D%d, F%d>, where: %08x\n", 
				dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), offset); 
	__raw_writel(PCIE_CONF_BUS( PCI_BUS(dev) ) |
					PCIE_CONF_DEV( PCI_DEV(dev) ) |
					PCIE_CONF_FUNC( PCI_FUNC(dev) ) |
					PCIE_CONF_REG(offset),
					PCI_SLOT_ADDR + PCIE_CONF_ADDR_OFF);

	*value = __raw_readl(PCI_SLOT_ADDR + PCIE_CONF_DATA_OFF);

	if(conf_trace) printf("Return : dev: %08x, where: %08x, val: %08x\n\n", dev, offset, *value);

	return PCIBIOS_SUCCESSFUL;
}

int iproc_pcie_rd_conf_word(struct pci_controller *hose, pci_dev_t dev, int offset, u16 * value)
{
    volatile unsigned int tmp;

	if(conf_trace) printf("Pcie_rd_conf_word: dev: %08x <B%d, D%d, F%d>, where: %08x\n", 
				dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), offset); 
	__raw_writel(PCIE_CONF_BUS( PCI_BUS(dev) ) |
					PCIE_CONF_DEV( PCI_DEV(dev) ) |
					PCIE_CONF_FUNC( PCI_FUNC(dev) ) |
					PCIE_CONF_REG(offset),
					PCI_SLOT_ADDR + PCIE_CONF_ADDR_OFF);

	tmp = __raw_readl(PCI_SLOT_ADDR + PCIE_CONF_DATA_OFF);
    *value = (tmp >> (8 * (offset & 3))) & 0xffff;

	if(conf_trace) printf("Return : dev: %08x, where: %08x, val: %04x\n\n", dev, offset, *value);

	return PCIBIOS_SUCCESSFUL;
}

int iproc_pcie_rd_conf_byte(struct pci_controller *hose, pci_dev_t dev, int offset, u8 * value)
{
    volatile unsigned int tmp;

	if(conf_trace) printf("Pcie_rd_conf_byte: dev: %08x <B%d, D%d, F%d>, where: %08x\n", 
				dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), offset); 
	__raw_writel(PCIE_CONF_BUS( PCI_BUS(dev) ) |
					PCIE_CONF_DEV( PCI_DEV(dev) ) |
					PCIE_CONF_FUNC( PCI_FUNC(dev) ) |
					PCIE_CONF_REG(offset),
					PCI_SLOT_ADDR + PCIE_CONF_ADDR_OFF);

	tmp = __raw_readl(PCI_SLOT_ADDR + PCIE_CONF_DATA_OFF);
    *value = (tmp >> (8 * (offset & 3))) & 0xff;

	if(conf_trace) printf("Return : dev: %08x, where: %08x, val: %02x\n\n", dev, offset, *value);

	return PCIBIOS_SUCCESSFUL;
}

int iproc_pcie_wr_conf_dword(struct pci_controller *hose, pci_dev_t dev, int offset, u32 value)
{
	int ret = PCIBIOS_SUCCESSFUL;

	if(conf_trace) printf("Pcie_wr_conf_dword: dev: %08x <B%d, D%d, F%d>, where: %08x, val: %08x\n", 
				dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), offset, value); 
	__raw_writel(PCIE_CONF_BUS( PCI_BUS(dev) ) |
					PCIE_CONF_DEV( PCI_DEV(dev) ) |
					PCIE_CONF_FUNC( PCI_FUNC(dev) ) |
					PCIE_CONF_REG(offset),
					PCI_SLOT_ADDR + PCIE_CONF_ADDR_OFF);

	__raw_writel(value, PCI_SLOT_ADDR + PCIE_CONF_DATA_OFF);
	return ret;
}

int iproc_pcie_wr_conf_word(struct pci_controller *hose, pci_dev_t dev, int offset, u16 value)
{
	int ret = PCIBIOS_SUCCESSFUL;

	if(conf_trace) printf("Pcie_wr_conf_word: dev: %08x <B%d, D%d, F%d>, where: %08x, val: %04x\n", 
				dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), offset, value); 
	__raw_writel(PCIE_CONF_BUS( PCI_BUS(dev) ) |
					PCIE_CONF_DEV( PCI_DEV(dev) ) |
					PCIE_CONF_FUNC( PCI_FUNC(dev) ) |
					PCIE_CONF_REG(offset),
					PCI_SLOT_ADDR + PCIE_CONF_ADDR_OFF);

	__raw_writew(value, PCI_SLOT_ADDR + PCIE_CONF_DATA_OFF + (offset & 3) );
	return ret;
}

int iproc_pcie_wr_conf_byte(struct pci_controller *hose, pci_dev_t dev, int offset, u8 value)
{
	int ret = PCIBIOS_SUCCESSFUL;

	if(conf_trace) printf("Pcie_wr_conf_byte: dev: %08x <B%d, D%d, F%d>, where: %08x, val: %02x\n", 
				dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), offset, value); 
	__raw_writel(PCIE_CONF_BUS( PCI_BUS(dev) ) |
					PCIE_CONF_DEV( PCI_DEV(dev) ) |
					PCIE_CONF_FUNC( PCI_FUNC(dev) ) |
					PCIE_CONF_REG(offset),
					PCI_SLOT_ADDR + PCIE_CONF_ADDR_OFF);

	__raw_writeb(value, PCI_SLOT_ADDR + PCIE_CONF_DATA_OFF + (offset & 3) );
	return ret;
}


int pci_read_config_dword (pci_dev_t dev, int where, unsigned int *val)
{
	if(conf_trace) printf("pci_read_config_dword: dev: %08x <B%d, D%d, F%d>, where: %08x\n", 
				dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), where); 
	__raw_writel(PCIE_CONF_BUS( PCI_BUS(dev) ) |
					PCIE_CONF_DEV( PCI_DEV(dev) ) |
					PCIE_CONF_FUNC( PCI_FUNC(dev) ) |
					PCIE_CONF_REG(where),
					PCI_SLOT_ADDR + PCIE_CONF_ADDR_OFF);

	*val = __raw_readl(PCI_SLOT_ADDR + PCIE_CONF_DATA_OFF);

	if(conf_trace) printf("Return : dev: %08x, where: %08x, val: %08x\n\n", dev, where, *val);

	return PCIBIOS_SUCCESSFUL;
}

int pci_read_config_word (pci_dev_t dev, int where, unsigned short *val)
{
    unsigned int tmp;

	if(conf_trace) printf("pci_read_config_word: dev: %08x <B%d, D%d, F%d>, where: %08x\n", 
				dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), where); 
	__raw_writel(PCIE_CONF_BUS( PCI_BUS(dev) ) |
					PCIE_CONF_DEV( PCI_DEV(dev) ) |
					PCIE_CONF_FUNC( PCI_FUNC(dev) ) |
					PCIE_CONF_REG(where),
					PCI_SLOT_ADDR + PCIE_CONF_ADDR_OFF);

	tmp = __raw_readl(PCI_SLOT_ADDR + PCIE_CONF_DATA_OFF);
    *val = (tmp >> (8 * (where & 3))) & 0xffff;

	if(conf_trace) printf("Return : dev: %08x, where: %08x, val: %04x\n\n", dev, where, *val);

	return PCIBIOS_SUCCESSFUL;
}

int pci_read_config_byte (pci_dev_t dev, int where, unsigned char *val)
{
    unsigned int tmp;

	if(conf_trace) printf("pci_read_config_byte: dev: %08x <B%d, D%d, F%d>, where: %08x\n", 
				dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), where); 
	__raw_writel(PCIE_CONF_BUS( PCI_BUS(dev) ) |
					PCIE_CONF_DEV( PCI_DEV(dev) ) |
					PCIE_CONF_FUNC( PCI_FUNC(dev) ) |
					PCIE_CONF_REG(where),
					PCI_SLOT_ADDR + PCIE_CONF_ADDR_OFF);

	tmp = __raw_readl(PCI_SLOT_ADDR + PCIE_CONF_DATA_OFF);
    *val = (tmp >> (8 * (where & 3))) & 0xff;

	if(conf_trace) printf("Return : dev: %08x, where: %08x, val: %02x\n\n", dev, where, *val);

	return PCIBIOS_SUCCESSFUL;
}


void pci_bus0_read_config_word (pci_dev_t dev, int where, unsigned short *val)
{
    unsigned int tmp;
	if(conf_trace) printf("pci_bus0_read_config_word: dev: %08x <B%d, D%d, F%d>, where: %08x\n", 
				dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), where);

    __raw_writel( where & 0xffc, PCI_SLOT_ADDR + SOC_PCIE_EXT_CFG_ADDR ); // BDF = 0

    tmp = __raw_readl(PCI_SLOT_ADDR + SOC_PCIE_EXT_CFG_DATA);
    *val = (tmp >> (8 * (where & 3))) & 0xffff;
	if(conf_trace) printf("Return : dev: %08x, where: %08x, val: %04x\n\n", dev, where, *val);
}

void pci_bus0_read_config_byte (pci_dev_t dev, int where, unsigned char *val)
{
    unsigned int tmp;
	if(conf_trace) printf("pci_bus0_read_config_byte: dev: %08x <B%d, D%d, F%d>, where: %08x\n", 
				dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), where);

    __raw_writel( where & 0xffc, PCI_SLOT_ADDR + SOC_PCIE_EXT_CFG_ADDR ); // BDF = 0

    tmp = __raw_readl(PCI_SLOT_ADDR + SOC_PCIE_EXT_CFG_DATA);
    *val = (tmp >> (8 * (where & 3))) & 0xff;
	if(conf_trace) printf("Return : dev: %08x, where: %08x, val: %02x\n\n", dev, where, *val);
}

int pci_write_config_dword (pci_dev_t dev, int where, unsigned int val)
{
	int ret = PCIBIOS_SUCCESSFUL;

	if(conf_trace) printf("pci_write_config_dword: dev: %08x <B%d, D%d, F%d>, where: %08x, val: %08x\n", 
				dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), where, val); 
	__raw_writel(PCIE_CONF_BUS( PCI_BUS(dev) ) |
					PCIE_CONF_DEV( PCI_DEV(dev) ) |
					PCIE_CONF_FUNC( PCI_FUNC(dev) ) |
					PCIE_CONF_REG(where),
					PCI_SLOT_ADDR + PCIE_CONF_ADDR_OFF);

	__raw_writel(val, PCI_SLOT_ADDR + PCIE_CONF_DATA_OFF);
	return ret;
}

int pci_write_config_word (pci_dev_t dev, int where, unsigned short val)
{
	int ret = PCIBIOS_SUCCESSFUL;

	if(conf_trace) printf("pci_write_config_word: dev: %08x <B%d, D%d, F%d>, where: %08x, val: %04x\n", 
				dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), where, val); 
	__raw_writel(PCIE_CONF_BUS( PCI_BUS(dev) ) |
					PCIE_CONF_DEV( PCI_DEV(dev) ) |
					PCIE_CONF_FUNC( PCI_FUNC(dev) ) |
					PCIE_CONF_REG(where),
					PCI_SLOT_ADDR + PCIE_CONF_ADDR_OFF);

	__raw_writew(val, PCI_SLOT_ADDR + PCIE_CONF_DATA_OFF + (where & 3) );
	return ret;
}

int pci_write_config_byte (pci_dev_t dev, int where, unsigned char val)
{
	int ret = PCIBIOS_SUCCESSFUL;

	if(conf_trace) printf("pci_write_config_byte: dev: %08x <B%d, D%d, F%d>, where: %08x, val: %02x\n", 
				dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), where, val); 
	__raw_writel(PCIE_CONF_BUS( PCI_BUS(dev) ) |
					PCIE_CONF_DEV( PCI_DEV(dev) ) |
					PCIE_CONF_FUNC( PCI_FUNC(dev) ) |
					PCIE_CONF_REG(where),
					PCI_SLOT_ADDR + PCIE_CONF_ADDR_OFF);

	__raw_writeb(val, PCI_SLOT_ADDR + PCIE_CONF_DATA_OFF + (where & 3) );
	return ret;
}

void pci_bus0_write_config_word (pci_dev_t dev, int where, unsigned short val)
{
    unsigned int tmp;

	if(conf_trace) printf("pci_bus0_write_config_word: dev: %08x <B%d, D%d, F%d>, where: %08x, val: %04x\n", 
				dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), where, val);

    __raw_writel( where & 0xffc, PCI_SLOT_ADDR + SOC_PCIE_EXT_CFG_ADDR ); // BDF = 0

    tmp = __raw_readl(PCI_SLOT_ADDR + SOC_PCIE_EXT_CFG_DATA);
 
	if(conf_trace) printf("pci_bus0_write_config_word read first: dev: %08x <B%d, D%d, F%d>, where: %08x, tmp_val: %04x\n", 
				dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), where, tmp);
    tmp &= ~(0xffff << (8 * (where & 3)) );
    tmp |= (val << (8 * (where & 3)) );

	if(conf_trace) printf("pci_bus0_write_config_word write back: dev: %08x <B%d, D%d, F%d>, where: %08x, tmp_val: %04x\n", 
				dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), where, tmp);
	__raw_writel( where & 0xffc, PCI_SLOT_ADDR + SOC_PCIE_EXT_CFG_ADDR );
	__raw_writel(tmp, PCI_SLOT_ADDR + SOC_PCIE_EXT_CFG_DATA);

    if(conf_trace) printf("pci_bus0_write_config_word write done\n");
}

void pci_bus0_write_config_byte (pci_dev_t dev, int where, unsigned char val)
{
    unsigned int tmp;

	if(conf_trace) printf("pci_bus0_write_config_byte: dev: %08x <B%d, D%d, F%d>, where: %08x, val: %02x\n", 
				dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), where, val);

    __raw_writel( where & 0xffc, PCI_SLOT_ADDR + SOC_PCIE_EXT_CFG_ADDR ); // BDF = 0

    tmp = __raw_readl(PCI_SLOT_ADDR + SOC_PCIE_EXT_CFG_DATA);

    tmp &= ~(0xff << (8 * (where & 3)) );
    tmp |= (val << (8 * (where & 3)) );

	__raw_writel( where & 0xffc, PCI_SLOT_ADDR + SOC_PCIE_EXT_CFG_ADDR );
	__raw_writel(tmp, PCI_SLOT_ADDR + SOC_PCIE_EXT_CFG_DATA);
}


/*
 * Setup the address translation
 */
static void soc_pcie_map_init(struct soc_pcie_port * port)
{

	/* pass thru' address translation */
}


/*
 * Setup PCIE Host bridge
 */
static void soc_pcie_bridge_init(struct soc_pcie_port * port)
{
        u32 devfn = 0;
        u8 tmp8;
	    u16 tmp16;

	    /* Fake <bus> object */
        pci_bus0_read_config_byte(devfn, PCI_PRIMARY_BUS, &tmp8);
        pci_bus0_read_config_byte(devfn, PCI_SECONDARY_BUS, &tmp8);
        pci_bus0_read_config_byte(devfn, PCI_SUBORDINATE_BUS, &tmp8);

        pci_bus0_write_config_byte(devfn, PCI_PRIMARY_BUS, 0);
        pci_bus0_write_config_byte(devfn, PCI_SECONDARY_BUS, 1);
        pci_bus0_write_config_byte(devfn, PCI_SUBORDINATE_BUS, 4);

        pci_bus0_read_config_byte(devfn, PCI_PRIMARY_BUS, &tmp8);
        pci_bus0_read_config_byte(devfn, PCI_SECONDARY_BUS, &tmp8);
        pci_bus0_read_config_byte(devfn, PCI_SUBORDINATE_BUS, &tmp8);

        printf("membase %#x memlimit %#x\n",
               OUT_PCI_ADDR, OUT_PCI_ADDR + SZ_128M);

        pci_bus0_read_config_word(devfn, PCI_CLASS_DEVICE, &tmp16);
        pci_bus0_read_config_word(devfn, PCI_MEMORY_BASE, &tmp16);
        pci_bus0_read_config_word(devfn, PCI_MEMORY_LIMIT, &tmp16);

        pci_bus0_write_config_word(devfn, PCI_MEMORY_BASE, 
               OUT_PCI_ADDR >> 16 );
        pci_bus0_write_config_word(devfn, PCI_MEMORY_LIMIT, 
               (OUT_PCI_ADDR + SZ_128M) >> 16 );

	/* Force class to that of a Bridge */
        pci_bus0_write_config_word(devfn, PCI_CLASS_DEVICE,
		PCI_CLASS_BRIDGE_PCI);

        pci_bus0_read_config_word(devfn, PCI_CLASS_DEVICE, &tmp16);
        pci_bus0_read_config_word(devfn, PCI_MEMORY_BASE, &tmp16);
        pci_bus0_read_config_word(devfn, PCI_MEMORY_LIMIT, &tmp16);
	
}

/* ****************************************************
 * this function is only applicable to type 1 command
 ******************************************************/
int pcie_diag_wr_conf_dword(struct pci_controller *hose, pci_dev_t dev, int offset, u32 value)
{
	int ret = PCIBIOS_SUCCESSFUL;

	if(conf_trace) printf("Pcie_wr_conf_dword: dev: %08x <B%d, D%d, F%d>, where: %08x, val: %08x\n", 
				dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), offset, value); 
		__raw_writel(PCIE_CONF_BUS( PCI_BUS(dev) ) |
					PCIE_CONF_DEV( PCI_DEV(dev) ) |
					PCIE_CONF_FUNC( PCI_FUNC(dev) ) |
					PCIE_CONF_REG(offset) | 0x01,
					PCI_SLOT_ADDR + PCIE_CONF_ADDR_OFF);

		__raw_writel(value, PCI_SLOT_ADDR + PCIE_CONF_DATA_OFF);
	return ret;
}

/* ****************************************************
 * this function is only applicable to type 1 command
 ******************************************************/
int pcie_diag_rd_conf_dword(struct pci_controller *hose, pci_dev_t dev, int offset, u32 * value)
{
	if(conf_trace) printf("Pcie_rd_conf_dword: dev: %08x <B%d, D%d, F%d>, where: %08x\n", 
				dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), offset); 
	__raw_writel(PCIE_CONF_BUS( PCI_BUS(dev) ) |
					PCIE_CONF_DEV( PCI_DEV(dev) ) |
					PCIE_CONF_FUNC( PCI_FUNC(dev) ) |
					PCIE_CONF_REG(offset)|0x01,
					PCI_SLOT_ADDR + PCIE_CONF_ADDR_OFF);

	*value = __raw_readl(PCI_SLOT_ADDR + PCIE_CONF_DATA_OFF);

	if(conf_trace) printf("Return : dev: %08x, where: %08x, val: %08x\n\n", dev, offset, *value);

	return PCIBIOS_SUCCESSFUL;
}

#if defined(CONFIG_NS_PLUS)

static void pcieSwitchInit( pci_dev_t busdevfn)
{

#define   SZ_48M    (0x100000 * 48)




	u32 	dRead = 0;
	u32		bm = 0;
	struct pci_controller  *hose = NULL;
	unsigned int base_temp, limit_temp;
	u32 u32Readout;

	u32 busno = PCI_BUS(busdevfn);
	conf_trace = 0;


	//printf("\n Enters pcieSwitchInit: BDF  0x%x \n\n",busdevfn);


	if(conf_trace) printf("==>pcieSwitchInit: dev: %08x <B%d, D%d, F%d>",
			busdevfn, PCI_BUS(busdevfn), PCI_DEV(busdevfn), PCI_FUNC(busdevfn));




    pcie_diag_rd_conf_dword(hose, busdevfn, 0x100,/* 4,*/ &dRead);


    if(conf_trace) printf("PCIE: Doing PLX switch Init...Test Read = %08x\n",(unsigned int)dRead);

	//Debug control register.
    pcie_diag_rd_conf_dword(hose, busdevfn, 0x1dc, /*4,*/ &dRead);

	dRead &= ~(1<<22);

    pcie_diag_wr_conf_dword/*soc_pci_write_config*/(hose, busdevfn, 0x1dc, /*4,*/ dRead);

/***/
	//Set GPIO enables.
    pcie_diag_rd_conf_dword(hose, busdevfn, 0x62c, /*4,*/ &dRead);

	if(conf_trace) printf("PCIE: Doing PLX switch Init...GPIO Read = %08x\n",(unsigned int)dRead);

	dRead &= ~((1<<0)|(1<<1)|(1<<3));
	dRead |= ((1<<4)|(1<<5)|(1<<7));

    pcie_diag_wr_conf_dword/*soc_pci_write_config*/(hose, busdevfn, 0x62c, /*4,*/ dRead);

	mdelay(50);
	dRead |= ((1<<0)|(1<<1));

    pcie_diag_wr_conf_dword/*soc_pci_write_config*/(hose, busdevfn, 0x62c, /*4,*/ dRead);

    pcie_diag_rd_conf_dword/*soc_pci_read_config*/(hose, busdevfn, 0x4, /*2,*/ &bm);

#if NS_PCI_DEBUG
	printf("bus master: %08x\n", bm);
#endif
	bm |= 0x06;
	pcie_diag_wr_conf_dword/*soc_pci_write_config*/(hose, busdevfn, 0x4,/*2,*/ bm);
        //pcie_diag_wr_conf_dword(hose, busdevfn, 0x4, bm);
	bm = 0;

	pcie_diag_rd_conf_dword/*soc_pci_read_config*/(hose, busdevfn, 0x4, /*2,*/ &bm);
	printf("bus master after: %08x\n", bm);
	bm =0;
	//Bus 1 if the upstream port of the switch. Bus 2 has the two downstream ports, one on each device number.


	if(PCI_BUS(busdevfn) == 1)
	{
		pcie_diag_wr_conf_dword/*soc_pci_write_config*/(hose, busdevfn, 0x18, /*4,*/ pcieSwitchPrimSecBusNum);
		pcie_diag_rd_conf_dword(hose, busdevfn, 0x18, &u32Readout);
		//printf("==>pcieSwitchInit: 0x18 Readout %x\n", u32Readout);
		//TODO: We need to scan all outgoing windows, to look for a base limit pair for this register.
		//npciConfigOutLong(instance, busNo, deviceNo, 0, 0x20,0xcff0c000);
		/* MEM_BASE, MEM_LIM require 1MB alignment */

		//limit_temp = (0x8000000 + SZ_32M) >> 16; 
		limit_temp = (0x8000000 + SZ_4M - 1) >> 16; 
		base_temp = 0x8000000 >> 16;

		base_temp = base_temp | (limit_temp << 16  );

		pcie_diag_wr_conf_dword(hose, busdevfn, PCI_MEMORY_BASE, base_temp);

		pcie_diag_rd_conf_dword(hose, busdevfn, PCI_MEMORY_BASE, &u32Readout);
		//printf("==>pcieSwitchInit: PCI_MEMORY_BASE Readout %x\n", u32Readout);

	}
	else if(PCI_BUS(busdevfn) == 2)
	{

		//TODO: I need to fix these hard coded addresses.
		if(PCI_DEV(busdevfn) == (0x8 >> 3))
		{
			pcie_diag_wr_conf_dword/*soc_pci_write_config*/(hose, busdevfn, 0x18,/* 4,*/ (0x00000000 | ((busno+1)<<16) | ((busno+1)<<8) | busno));

			pcie_diag_rd_conf_dword(hose, busdevfn, 0x18, &u32Readout);

			//limit_temp = (0x8000000 + SZ_48M + SZ_32M) >> 16;
			//base_temp = ( 0x8000000 + SZ_48M) >> 16;
			limit_temp = (0x8000000 + SZ_2M - 1) >> 16;
			base_temp = ( 0x8000000 ) >> 16;

			base_temp = base_temp | (limit_temp << 16  );

			pcie_diag_wr_conf_dword(hose, busdevfn, PCI_MEMORY_BASE, base_temp);
		       
                        pcie_diag_rd_conf_dword(hose, busdevfn, PCI_MEMORY_BASE, &u32Readout);
		        //printf("==>pcieSwitchInit: PCI_MEMORY_BASE Readout %x\n", u32Readout);

			iproc_pcie_rd_conf_word/*soc_pci_read_config*/(hose, busdevfn, 0x7A, /*2,*/ &bm);

			if(conf_trace) printf("bm = %04x\n busdevfn = = %08x, bus = %08x\n", bm, busdevfn, PCI_BUS(busdevfn));
		}
		else if(PCI_DEV(busdevfn) == (0x10>>3))
		{
			pcie_diag_wr_conf_dword/*soc_pci_write_config*/(hose, busdevfn, 0x18, /*4,*/ (0x00000000 | ((busno+2)<<16) | ((busno+2)<<8) | busno));
			pcie_diag_rd_conf_dword(hose, busdevfn, 0x18, &u32Readout);
			//printf("==>pcieSwitchInit: 0x18 Readout %x\n", u32Readout);

			//limit_temp = (0x8000000 +  (SZ_48M * 2 ) + SZ_32M)  >> 16;
			//base_temp = ( 0x8000000 + SZ_48M *2) >> 16;
			limit_temp = (0x8000000 +  SZ_4M -1)  >> 16;
			base_temp = ( 0x8000000 + SZ_2M) >> 16;

			base_temp = base_temp | (limit_temp << 16  );

			pcie_diag_wr_conf_dword(hose, busdevfn, PCI_MEMORY_BASE, base_temp);

			pcie_diag_rd_conf_dword(hose, busdevfn, PCI_MEMORY_BASE, &u32Readout);
			//printf("==>pcieSwitchInit: PCI_MEMORY_BASE Readout %x\n", u32Readout);

			if(conf_trace) printf("bm = %04x\n busdevfn = = %08x, bus = %08x\n", bm, busdevfn, PCI_BUS(busdevfn));
		}

	}

}

static void pcieWIFIdeviceInit( pci_dev_t busdevfn)
{

	u32 	dRead = 0;
	u32		bm = 0;
	struct pci_controller  *hose = NULL;
	unsigned int base_temp, limit_temp;
	u32 u32Readout;

	u32 busno = PCI_BUS(busdevfn);
	conf_trace = 0;

	if(conf_trace) printf("SATA Device Init: dev: %08x <B%d, D%d, F%d>",
			busdevfn, PCI_BUS(busdevfn), PCI_DEV(busdevfn), PCI_FUNC(busdevfn));

	//pcie_diag_wr_conf_dword(hose, busdevfn, 0x18, pcieSwitchPrimSecBusNum);
	
	//limit_temp = (0x8000000 + SZ_16K) >> 16; 
	base_temp = 0x8000000 ;

	//base_temp = base_temp | (limit_temp << 16  );

	pcie_diag_wr_conf_dword(hose, busdevfn, 0x10, base_temp);
}

static void pcieSATAdeviceInit( pci_dev_t busdevfn)
{

	u32 	dRead = 0;
	u32		bm = 0;
	struct pci_controller  *hose = NULL;
	unsigned int base_temp, limit_temp;
	u32 u32Readout;

	u32 busno = PCI_BUS(busdevfn);
	conf_trace = 0;

	if(conf_trace) printf("SATA Device Init: dev: %08x <B%d, D%d, F%d>",
			busdevfn, PCI_BUS(busdevfn), PCI_DEV(busdevfn), PCI_FUNC(busdevfn));

	//pcie_diag_wr_conf_dword(hose, busdevfn, 0x18, pcieSwitchPrimSecBusNum);
	
	//limit_temp = (0x8200000 + 0x1FF) >> 16; 


	base_temp = 0xffffffff;

	//base_temp = base_temp | (limit_temp << 16  );

	pcie_diag_wr_conf_dword(hose, busdevfn, 0x20, base_temp);



	base_temp = 0x8200000;

	//base_temp = base_temp | (limit_temp << 16  );

	pcie_diag_wr_conf_dword(hose, busdevfn, 0x20, base_temp);
	base_temp = 0xffffffff;

	//base_temp = base_temp | (limit_temp << 16  );

	pcie_diag_wr_conf_dword(hose, busdevfn, 0x24, base_temp);

	//limit_temp = (0x8600000 + 0xFFFF) >> 16; 
	base_temp = 0x8600000;

	//base_temp = base_temp | (limit_temp << 16  );

	pcie_diag_wr_conf_dword(hose, busdevfn, 0x24, base_temp);
}

int pci_diag_hose_config_device(struct pci_controller *hose,
			   pci_dev_t dev,
			   unsigned long io,
			   pci_addr_t mem,
			   unsigned long command)
{
	unsigned int bar_response, old_command;
	pci_addr_t bar_value;
	pci_size_t bar_size;
	unsigned char pin;
	int bar, found_mem64;

	debug ("PCI Config: I/O=0x%lx, Memory=0x%llx, Command=0x%lx\n",
		io, (u64)mem, command);

	pcie_diag_wr_conf_dword (hose, dev, PCI_COMMAND, 0);

	for (bar = PCI_BASE_ADDRESS_0; bar <= PCI_BASE_ADDRESS_5; bar += 4) {
		pcie_diag_wr_conf_dword (hose, dev, bar, 0xffffffff);
		pcie_diag_rd_conf_dword (hose, dev, bar, &bar_response);

		if (!bar_response)
			continue;

		found_mem64 = 0;

		/* Check the BAR type and set our address mask */
		if (bar_response & PCI_BASE_ADDRESS_SPACE) {
			bar_size = ~(bar_response & PCI_BASE_ADDRESS_IO_MASK) + 1;
			/* round up region base address to a multiple of size */
			io = ((io - 1) | (bar_size - 1)) + 1;
			bar_value = io;
			/* compute new region base address */
			io = io + bar_size;
		} else {
			if ((bar_response & PCI_BASE_ADDRESS_MEM_TYPE_MASK) ==
				PCI_BASE_ADDRESS_MEM_TYPE_64) {
				u32 bar_response_upper;
				u64 bar64;
				pcie_diag_wr_conf_dword(hose, dev, bar+4, 0xffffffff);
				pcie_diag_rd_conf_dword(hose, dev, bar+4, &bar_response_upper);

				bar64 = ((u64)bar_response_upper << 32) | bar_response;

				bar_size = ~(bar64 & PCI_BASE_ADDRESS_MEM_MASK) + 1;
				found_mem64 = 1;
			} else {
				bar_size = (u32)(~(bar_response & PCI_BASE_ADDRESS_MEM_MASK) + 1);
			}

			/* round up region base address to multiple of size */
			mem = ((mem - 1) | (bar_size - 1)) + 1;
			bar_value = mem;
			/* compute new region base address */
			mem = mem + bar_size;
		}

		/* Write it out and update our limit */
		pcie_diag_wr_conf_dword (hose, dev, bar, (u32)bar_value);

		if (found_mem64) {
			bar += 4;
#ifdef CONFIG_SYS_PCI_64BIT
			pcie_diag_wr_conf_dword(hose, dev, bar, (u32)(bar_value>>32));
#else
			pcie_diag_wr_conf_dword (hose, dev, bar, 0x00000000);
#endif
		}
	}

	/* Configure Cache Line Size Register */
//	pci_hose_write_config_byte (hose, dev, PCI_CACHE_LINE_SIZE, 0x08);  

	// These two settings seem to hang the system.

	/* Configure Latency Timer */
//	pci_hose_write_config_byte (hose, dev, PCI_LATENCY_TIMER, 0x80);

	/* Disable interrupt line, if device says it wants to use interrupts */
	pcie_diag_rd_conf_dword (hose, dev, PCI_COMMAND, &old_command);
	pcie_diag_wr_conf_dword (hose, dev, PCI_COMMAND,
				     (old_command & 0xffff0000) | command);

	return 0;
}
#endif

void pci_iproc_init_board (struct pci_controller * hose)
{
	soc_pcie_hw_init(&soc_pcie_ports[PORT_IN_USE]);

	hose->first_busno = 0;
	hose->last_busno = 0;
	hose->current_busno = 0;
	unsigned int tmp32;

	pci_set_ops(hose,
		    iproc_pcie_rd_conf_byte,
		    iproc_pcie_rd_conf_word,
		    iproc_pcie_rd_conf_dword,
		    iproc_pcie_wr_conf_byte,
		    iproc_pcie_wr_conf_word,
		    iproc_pcie_wr_conf_dword );

	pci_register_hose(hose);

	udelay(1000);

    soc_pcie_map_init(&soc_pcie_ports[PORT_IN_USE]);


    if( soc_pcie_check_link(&soc_pcie_ports[PORT_IN_USE]) != 0 )
	{
    	printf("\n**************\n port %d is not active!!\n**************\n",PORT_IN_USE);
		return;
	}
    printf("\n**************\n port %d is active!!\n**************\n",PORT_IN_USE);

        
    soc_pcie_bridge_init( &soc_pcie_ports[PORT_IN_USE] );

    hose->last_busno = pci_hose_scan(hose);
    //hose->last_busno = 1;
    printf("\n pci_iproc_init_board : hose->last_busno = 0x%x \n", hose->last_busno);

	pci_hose_config_device(hose, PCI_BDF(1,0,0), NULL, OUT_PCI_ADDR, 0x146);

    /* Set IRQ */
	pci_bus0_write_config_word(0, 0x3c, 0x1a9);
	pci_write_config_word(0, 0x3c, 0x1a9);

    /* Set bridge */
	pci_bus0_write_config_byte(0, 0x1b, 0x00);  // latency timer
	pci_bus0_write_config_word(0, 0x28, 0x00);  // prefetch_base
	pci_bus0_write_config_word(0, 0x2a, 0x00);
	pci_bus0_write_config_word(0, 0x4, 0x146);  // cmd

#if defined(CONFIG_NS_PLUS)
	if ( (post_get_board_diags_type() == BCM958622HR) && ( 0 == PORT_IN_USE ) )
	{
		pcieSwitchInit(get_pci_dev("1.0.0"));
		printf("\n Finished PCI switch init 1 \n");
		pcieSwitchInit(get_pci_dev("2.1.0"));
		pcieSwitchInit(get_pci_dev("2.2.0"));

		pcieWIFIdeviceInit("3.0.0");
		pcieSATAdeviceInit("4.0.0");

	    pci_diag_hose_config_device(hose, PCI_BDF(4,0,0), NULL, 0x08200000, 0x146);
	}
	else
	{
#endif

#ifdef CONFIG_PCI_SCAN_SHOW
    printf("\nExtended Config\n");
    pci_dump_extended_conf(&soc_pcie_ports[PORT_IN_USE]);

	printf("\nStandard Config\n");
    pci_dump_standard_conf(&soc_pcie_ports[PORT_IN_USE]);


	printf("PCI:   Bus Dev VenId DevId Class Int\n");
#endif
#if defined(CONFIG_NS_PLUS)
	}
#endif

	printf("Done PCI initialization\n");

    return 0;
}

#if defined(CONFIG_NS_PLUS)

void dumpSATAinfo( struct pci_controller * hose )
{
	unsigned int tmp32;
        printf("SATA pcie device:\n");
	pcie_diag_rd_conf_dword(hose, get_pci_dev("4.0.0"), 0, &tmp32);
	printf("      Vendor ID: %04x\n", tmp32 & 0xFFFF);
	printf("      Device ID: %04x\n", (tmp32 >>16)& 0xFFFF);
	pcie_diag_rd_conf_dword(hose, get_pci_dev("4.0.0"), 8, &tmp32);
	printf("      Class: %04x\n", (tmp32 >> 16));
	printf("      subclass: %04x\n", (tmp32)& 0xFFFF);
	pcie_diag_rd_conf_dword(hose, get_pci_dev("4.0.0"), 0x20, &tmp32);
	printf("      Bar 5: %08x\n", tmp32 );
        pcie_diag_rd_conf_dword(hose, get_pci_dev("4.0.0"), 0x24, &tmp32);
	printf("      Bar 6: %08x\n", tmp32);

}
void dumpPCIEDeviceOnSwitch( void )
{
struct pci_controller * hose;
	unsigned int tmp32;
	unsigned int memBase;

        printf("Device 0:\n");
	pcie_diag_rd_conf_dword(hose, get_pci_dev("1.0.0"), 0, &tmp32);
	printf("      Vendor ID: %04x\n", tmp32 & 0xFFFF);
	printf("      Device ID: %04x\n", (tmp32 >>16)& 0xFFFF);
	pcie_diag_rd_conf_dword(hose, get_pci_dev("1.0.0"), 8, &tmp32);
	printf("      Class: %04x\n", (tmp32 >> 16));
	printf("      subclass: %04x\n", (tmp32)& 0xFFFF);
	pcie_diag_rd_conf_dword(hose, get_pci_dev("1.0.0"), 0x20, &tmp32);
	printf("      Memory Base: %08x\n", tmp32 & 0xFFFF);
	printf("      Memory Limit: %08x\n", (tmp32 >> 16) & 0xFFFF );

        printf("Device 1:\n");
	pcie_diag_rd_conf_dword(hose, get_pci_dev("2.1.0"), 0, &tmp32);
	printf("      Vendor ID: %04x\n", tmp32 & 0xFFFF);
	printf("      Device ID: %04x\n", (tmp32 >>16)& 0xFFFF);
	pcie_diag_rd_conf_dword(hose, get_pci_dev("2.1.0"), 8, &tmp32);
	printf("      Class: %04x\n", (tmp32 >> 16));
	printf("      subclass: %04x\n", (tmp32)& 0xFFFF);
	pcie_diag_rd_conf_dword(hose, get_pci_dev("2.1.0"), 0x20, &tmp32);
	printf("      Memory Base: %08x\n", tmp32 & 0xFFFF);
	printf("      Memory Limit: %08x\n", (tmp32 >> 16) & 0xFFFF );

        printf("Device 2:\n");
	pcie_diag_rd_conf_dword(hose, get_pci_dev("2.2.0"), 0, &tmp32);
	printf("      Vendor ID: %04x\n", tmp32 & 0xFFFF);
	printf("      Device ID: %04x\n", (tmp32 >>16)& 0xFFFF);
	pcie_diag_rd_conf_dword(hose, get_pci_dev("2.2.0"), 8, &tmp32);
	printf("      Class: %04x\n", (tmp32 >> 16));
	printf("      subclass: %04x\n", (tmp32)& 0xFFFF);
	pcie_diag_rd_conf_dword(hose, get_pci_dev("2.2.0"), 0x20, &tmp32);
	printf("      Memory Base: %08x\n", tmp32 & 0xFFFF);
	printf("      Memory Limit: %08x\n", (tmp32 >> 16) & 0xFFFF );

        printf("Device 3:\n");
	pcie_diag_rd_conf_dword(hose, get_pci_dev("3.0.0"), 0, &tmp32);
	printf("      Vendor ID: %04x\n", tmp32 & 0xFFFF);
	printf("      Device ID: %04x\n", (tmp32 >>16)& 0xFFFF);
	pcie_diag_rd_conf_dword(hose, get_pci_dev("3.0.0"), 8, &tmp32);
	printf("      Class: %04x\n", (tmp32 >> 16));
	printf("      subclass: %04x\n", (tmp32)& 0xFFFF);
	pcie_diag_rd_conf_dword(hose, get_pci_dev("3.0.0"), 0x10, &tmp32);
	printf("      Bar 0: %08x\n", tmp32 );
	//printf("      Memory Limit: %08x\n", ((tmp32 >>16)& 0xFFFF)<<16);

        printf("\n Memory Base : ");

        memBase= (tmp32 & 0xFFFF) <<16;

        printf("Device 4:\n");
	pcie_diag_rd_conf_dword(hose, get_pci_dev("4.0.0"), 0, &tmp32);
	printf("      Vendor ID: %04x\n", tmp32 & 0xFFFF);
	printf("      Device ID: %04x\n", (tmp32 >>16)& 0xFFFF);
	pcie_diag_rd_conf_dword(hose, get_pci_dev("4.0.0"), 8, &tmp32);
	printf("      Class: %04x\n", (tmp32 >> 16));
	printf("      subclass: %04x\n", (tmp32)& 0xFFFF);
	pcie_diag_rd_conf_dword(hose, get_pci_dev("4.0.0"), 0x20, &tmp32);
	printf("      Bar 5: %08x\n", tmp32 );
        pcie_diag_rd_conf_dword(hose, get_pci_dev("4.0.0"), 0x24, &tmp32);
	printf("      Bar 6: %08x\n", tmp32);
}

#endif

static void pci_dump_standard_conf(struct soc_pcie_port * port)
{
    unsigned int i, val, cnt;
	unsigned short val16;
	unsigned char  val8;

	/* Disable trace */
	conf_trace = 0;

    for(i=0; i<64; i++)
    {      
		iproc_pcie_rd_conf_dword(NULL, 0, i*4, &val);

		cnt = i % 4;
		if(cnt==0) printf("i=%d <%x> \t 0x%08x \t", i, i, val);
		else if(cnt==3) printf("0x%08x \n", val);
		else printf("0x%08x \t", val);
	}
	printf("\n");

    for(i=0; i<6; i++)
    {      
		iproc_pcie_rd_conf_dword(NULL, 0, 0x10+i*4, &val);
		printf(" BAR-%d: 0x%08x\n\n", i, val);
	}
}

static void pci_dump_extended_conf(struct soc_pcie_port * port)
{
    unsigned int i, val, cnt;
	unsigned short val16;

	conf_trace = 0;

    for(i=0; i<128; i++)
    {
		pci_bus0_read_config_word(0, i*2, &val16);

		cnt = i % 8;
		if(cnt==0) printf("i=%d <%x> \t 0x%04x  ", i, i, val16);
		else if(cnt==7) printf("0x%04x \n", val16);
		else printf("0x%04x  ", val16);
	}
	printf("\n");
}


int pci_skip_dev(struct pci_controller *hose, pci_dev_t dev)
{
	return 0;
}

#ifdef CONFIG_PCI_SCAN_SHOW
int pci_print_dev(struct pci_controller *hose, pci_dev_t dev)
{
	return 1;
}
#endif /* CONFIG_PCI_SCAN_SHOW */
