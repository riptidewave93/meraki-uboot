/**
 * APM SoC APM86xxx PCIe Driver 
 *
 * Copyright (c) 2010 Applied Micro Circuits Corporation.
 * All rights reserved. Loc Ho <lho@apm.com>.
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
 * This module implements PCIe functionality for APM86xxx SoC. This setups and
 * configures PCIe controllers as either root complex or endpoint.
 *
 */
#include <common.h>
#include <pci.h>
#include <asm/byteorder.h>
#include <asm/io.h>
#include <asm/mmu.h>
#include <asm/processor.h>
#include <asm/mp.h>
#include <asm/atomic.h>
#include <asm/apm_ipp_csr.h>
#include <asm/apm_scu.h>

#if defined(CONFIG_PCI)
#include "asm/apm_pcie.h"
#include "asm/apm_pcie_csr.h"
#include "asm/apm_mosys.h"

DECLARE_GLOBAL_DATA_PTR;

#if 0
#define PCIE_CSR_DEBUG		printf
#else
#define PCIE_CSR_DEBUG(x, ...)
#endif
#if 0
#define PCIE_DEBUG		printf
#else
#define PCIE_DEBUG(x, ...)
#endif

#ifndef HIDWORD
#	define MAKE_U64(h, l)	((((u64) (h)) << 32) | (l))
#	define HIDWORD(x)	((u32) (((u64)(x)) >> 32))
#	define LODWORD(x)	((u32) ((x) & 0xFFFFFFFFULL))
#endif

#define PCIE_BAR_EN		0x00000001

enum {
	PTYPE_ENDPOINT		    	= 0x0,
	PTYPE_LEGACY_ENDPOINT		= 0x1,
	PTYPE_ROOTPORT		    	= 0x4,

	LNKW_X1			        = 0x1,
	LNKW_X4			        = 0x4,
	LNKW_X8			        = 0x8
};

#define PCI_ADDR_LO(h, l, O)	LODWORD(MAKE_U64((h), (l)) - (O))
#define PCI_ADDR_HI(h, l, O)	HIDWORD(MAKE_U64((h), (l)) - (O))

struct apm_pcie_map_tbl pcie_map_tbl[APM_PCIE_HOST_TOTAL] = {
	{
	  /* PCIE 0 SPACE */
	  MAKE_U64(PCIE0_CSR_BASE_HI, PCIE0_CSR_BASE_LO), PCIE_CSR_SIZE,
	  PCIE0_CFGBASE_HI, PCIE0_CFGBASE_LO, PCIE_CFGSIZE, 
	  PCI_ADDR_HI(PCIE0_CFGBASE_HI, PCIE0_CFGBASE_LO, PCIE0_ADDR_OFFSET),
	  PCI_ADDR_LO(PCIE0_CFGBASE_HI, PCIE0_CFGBASE_LO, PCIE0_ADDR_OFFSET),
	  CONFIG_SYS_PCIE0_CFGBASE,
	  { { PCIE0_OB_MEMBASE0_HI, PCIE0_OB_MEMBASE0_LO, PCIE0_OB_MEMSIZE0, 
		PCI_ADDR_HI(PCIE0_OB_MEMBASE0_HI, PCIE0_OB_MEMBASE0_LO, 
			PCIE0_ADDR_OFFSET),
		PCI_ADDR_LO(PCIE0_OB_MEMBASE0_HI, PCIE0_OB_MEMBASE0_LO, 
			PCIE0_ADDR_OFFSET) },
	    { PCIE0_OB_MEMBASE1_HI, PCIE0_OB_MEMBASE1_LO, PCIE0_OB_MEMSIZE1,
		PCI_ADDR_HI(PCIE0_OB_MEMBASE1_HI, PCIE0_OB_MEMBASE1_LO, 
			PCIE0_ADDR_OFFSET),
		PCI_ADDR_LO(PCIE0_OB_MEMBASE1_HI, PCIE0_OB_MEMBASE1_LO, 
			PCIE0_ADDR_OFFSET) },
	    { PCIE0_OB_IOBASE_HI, PCIE0_OB_IOBASE_LO, PCIE0_OB_IOSIZE,
		PCI_ADDR_HI(PCIE0_OB_IOBASE_HI, PCIE0_OB_IOBASE_LO,
			    PCIE0_ADDR_OFFSET),
		PCI_ADDR_LO(PCIE0_OB_IOBASE_HI, PCIE0_OB_IOBASE_LO,
			    PCIE0_ADDR_OFFSET) } },
	  PCIE0_MSGBASE_HI, PCIE0_MSGBASE_LO, PCIE0_MSGSIZE,
		PCI_ADDR_HI(PCIE0_MSGBASE_HI, PCIE0_MSGBASE_LO,
			    PCIE0_ADDR_OFFSET),		
		PCI_ADDR_LO(PCIE0_MSGBASE_HI, PCIE0_MSGBASE_LO,
			    PCIE0_ADDR_OFFSET),		
	  { { PCIE0_IB_MEMBASE0_PCIE_HI, PCIE0_IB_MEMBASE0_PCIE_LO,
	  	PCIE0_IB_MEMSIZE0, 0, 0, 0, 0, PCIE0_IB_MEMBASE0_PIM_SIZE },
	    { PCIE0_IB_MEMBASE1_PCIE_HI, PCIE0_IB_MEMBASE1_PCIE_LO,
	    	PCIE0_IB_MEMSIZE1, 0, 0, 0, 0, PCIE0_IB_MEMBASE1_PIM_SIZE } },
	  PCIE0_IB_ROMBASE_HI, PCIE0_IB_ROMBASE_LO, PCIE0_IB_ROMSIZE },
	  /* PCIE 1 SPACE */
	{ MAKE_U64(PCIE1_CSR_BASE_HI, PCIE1_CSR_BASE_LO), PCIE_CSR_SIZE,
	  PCIE1_CFGBASE_HI, PCIE1_CFGBASE_LO, PCIE_CFGSIZE, 
	  PCI_ADDR_HI(PCIE1_CFGBASE_HI, PCIE1_CFGBASE_LO, PCIE1_ADDR_OFFSET),
	  PCI_ADDR_LO(PCIE1_CFGBASE_HI, PCIE1_CFGBASE_LO, PCIE1_ADDR_OFFSET),
	  CONFIG_SYS_PCIE1_CFGBASE,
	  { { PCIE1_OB_MEMBASE0_HI, PCIE1_OB_MEMBASE0_LO, PCIE1_OB_MEMSIZE0,
		PCI_ADDR_HI(PCIE1_OB_MEMBASE0_HI, PCIE1_OB_MEMBASE0_LO,
			    PCIE1_ADDR_OFFSET),
		PCI_ADDR_LO(PCIE1_OB_MEMBASE0_HI, PCIE1_OB_MEMBASE0_LO,
			    PCIE1_ADDR_OFFSET) },
	    { PCIE1_OB_MEMBASE1_HI, PCIE1_OB_MEMBASE1_LO, PCIE1_OB_MEMSIZE1,
	        PCI_ADDR_HI(PCIE1_OB_MEMBASE1_HI, PCIE1_OB_MEMBASE1_LO,
			    PCIE1_ADDR_OFFSET),
		PCI_ADDR_LO(PCIE1_OB_MEMBASE1_HI, PCIE1_OB_MEMBASE1_LO,
			    PCIE1_ADDR_OFFSET) },
	    { PCIE1_OB_IOBASE_HI, PCIE1_OB_IOBASE_LO, PCIE1_OB_IOSIZE,
	        PCI_ADDR_HI(PCIE1_OB_IOBASE_HI, PCIE1_OB_IOBASE_LO,
			    PCIE1_ADDR_OFFSET),
		PCI_ADDR_LO(PCIE1_OB_IOBASE_HI, PCIE1_OB_IOBASE_LO,
			    PCIE1_ADDR_OFFSET) } },
	  PCIE1_MSGBASE_HI, PCIE1_MSGBASE_LO, PCIE1_MSGSIZE,
		PCI_ADDR_HI(PCIE1_MSGBASE_HI, PCIE1_MSGBASE_LO, 
			PCIE1_ADDR_OFFSET),
		PCI_ADDR_LO(PCIE1_MSGBASE_HI, PCIE1_MSGBASE_LO, 
			PCIE1_ADDR_OFFSET),
	  { { PCIE1_IB_MEMBASE0_PCIE_HI, PCIE1_IB_MEMBASE0_PCIE_LO,
	  	PCIE1_IB_MEMSIZE0, 0, 0, 0, 0, PCIE1_IB_MEMBASE0_PIM_SIZE },
	    { PCIE1_IB_MEMBASE1_PCIE_HI, PCIE1_IB_MEMBASE1_PCIE_LO,
	    	PCIE1_IB_MEMSIZE1, 0, 0, 0, 0, PCIE1_IB_MEMBASE1_PIM_SIZE } },
	  PCIE1_IB_ROMBASE_HI, PCIE1_IB_ROMBASE_LO, PCIE1_IB_ROMSIZE },
	  /* PCIE 2 SPACE */
	{ MAKE_U64(PCIE2_CSR_BASE_HI, PCIE2_CSR_BASE_LO), PCIE_CSR_SIZE,
	  PCIE2_CFGBASE_HI, PCIE2_CFGBASE_LO, PCIE_CFGSIZE,
	  PCI_ADDR_HI(PCIE2_CFGBASE_HI, PCIE2_CFGBASE_LO, PCIE2_ADDR_OFFSET),
	  PCI_ADDR_LO(PCIE2_CFGBASE_HI, PCIE2_CFGBASE_LO, PCIE2_ADDR_OFFSET),
	  CONFIG_SYS_PCIE2_CFGBASE,
	  { { PCIE2_OB_MEMBASE0_HI, PCIE2_OB_MEMBASE0_LO, PCIE2_OB_MEMSIZE0,
		PCI_ADDR_HI(PCIE2_OB_MEMBASE0_HI, PCIE2_OB_MEMBASE0_LO,
			    PCIE2_ADDR_OFFSET),
		PCI_ADDR_LO(PCIE2_OB_MEMBASE0_HI, PCIE2_OB_MEMBASE0_LO,
			    PCIE2_ADDR_OFFSET) },
	    { PCIE2_OB_MEMBASE1_HI, PCIE2_OB_MEMBASE1_LO, PCIE2_OB_MEMSIZE1,
	        PCI_ADDR_HI(PCIE2_OB_MEMBASE1_HI, PCIE2_OB_MEMBASE1_LO,
			    PCIE2_ADDR_OFFSET),
		PCI_ADDR_LO(PCIE2_OB_MEMBASE1_HI, PCIE2_OB_MEMBASE1_LO,
			    PCIE2_ADDR_OFFSET) },
	    { PCIE2_OB_IOBASE_HI, PCIE2_OB_IOBASE_LO, PCIE2_OB_IOSIZE,
	        PCI_ADDR_HI(PCIE2_OB_IOBASE_HI, PCIE2_OB_IOBASE_LO,
			    PCIE2_ADDR_OFFSET),
		PCI_ADDR_LO(PCIE2_OB_IOBASE_HI, PCIE2_OB_IOBASE_LO,
			    PCIE2_ADDR_OFFSET) } },
	  PCIE2_MSGBASE_HI, PCIE2_MSGBASE_LO, PCIE2_MSGSIZE,
		PCI_ADDR_HI(PCIE2_MSGBASE_HI, PCIE2_MSGBASE_LO, 
			PCIE2_ADDR_OFFSET),
		PCI_ADDR_LO(PCIE2_MSGBASE_HI, PCIE2_MSGBASE_LO, 
			PCIE2_ADDR_OFFSET),
	  { { PCIE2_IB_MEMBASE0_PCIE_HI, PCIE2_IB_MEMBASE0_PCIE_LO,
	  	PCIE2_IB_MEMSIZE0, 0, 0, 0, 0, PCIE2_IB_MEMBASE0_PIM_SIZE },
	    { PCIE2_IB_MEMBASE1_PCIE_HI, PCIE2_IB_MEMBASE1_PCIE_LO,
	    	PCIE2_IB_MEMSIZE1, 0, 0, 0, 0, PCIE2_IB_MEMBASE1_PIM_SIZE } },
	  PCIE2_IB_ROMBASE_HI, PCIE2_IB_ROMBASE_LO, PCIE2_IB_ROMSIZE } 
};

static struct pci_info {
	struct pci_controller	hose;
	u8	link;
}  pcie_ctx[APM_PCIE_HOST_TOTAL] = {
	{ { 0 }, 0 },
	{ { 0 }, 0 },
	{ { 0 }, 0 }
};

u32 PHY2VADDR(phys_addr_t paddr)
{
	return (u32) paddr;
}

/* PCIE Out/In to CSR */
int pcie_csr_out32(u32 addr, u32 val)
{
	if ((addr & 0x7fff) >= 0x4000) {
		PCIE_CSR_DEBUG("PCIE CSR WR(BE): 0x%08X value: 0x%08X\n", 
				addr, val);
		out_be32((void *) addr, val);
	} else {
		out_le32((void *) addr, val);
		PCIE_CSR_DEBUG("PCIE CSR WR(LE): 0x%08X value: 0x%08X\n", 
			addr, val);
	}
	udelay(1000);
	return 0;
}

int pcie_csr_in32(u32 addr, u32 *val)
{
	udelay(10000);
	if ((addr & 0x7fff) >= 0x4000) {
		*val = in_be32((void *) addr);
		PCIE_CSR_DEBUG("PCIE CSR RD(BE): 0x%08X value: 0x%08X\n", 
				addr, *val);
	} else {
		*val = in_le32((void *) addr);
		PCIE_CSR_DEBUG("PCIE CSR RD(LE): 0x%08X value: 0x%08X\n", 
				addr, *val);
	}
	return 0;
}

static inline int csr_outbe32(u32 addr, u32 val)
{
	PCIE_CSR_DEBUG("PCIE CSR WR(BE): 0x%08X value: 0x%08X\n", addr, val);
	out_be32((void *) addr, val);
	return 0;
}

static inline int csr_inbe32(u32 addr, u32 *val)
{
	*val = in_be32((void *) addr);
	PCIE_CSR_DEBUG("PCIE CSR RD(BE): 0x%08X value: 0x%08X\n", addr, *val);
	return 0;
}

/* PCIE Out/In to CSR */
static inline int pcie_cfg_out32(u32 addr, u32 val)
{
	PCIE_CSR_DEBUG("PCIE CFG WR32: 0x%08X value: 0x%08X\n", addr, val);
	out_le32((void *) addr, val);
	return 0;
}

static inline int pcie_cfg_out16(u32 addr, u16 val)
{
	u32 temp, mask;
	PCIE_CSR_DEBUG("PCIE CFG WR16: 0x%08X value: 0x%04X ", addr, val); 
	temp = in_le32((void *) (addr & 0xfffffffc));
	PCIE_CSR_DEBUG("read32: 0x%08X ", temp); 
	mask = 0xffff << ((addr & 0x3) * 8);
	temp &= ~mask;
	temp |= val << ((addr & 0x3) * 8);
	out_le32((void *)(addr & 0xfffffffc), temp);
	PCIE_CSR_DEBUG("out32: 0x%08X\n", temp); 
	return 0;
}

static inline int pcie_cfg_out8(u32 addr, u8 val)
{
	u32 temp, mask;
	PCIE_CSR_DEBUG("PCIE CFG WR8: 0x%08X value: 0x%02X ", addr, val); 
	temp = in_le32((void *) (addr & 0xfffffffc));
	PCIE_CSR_DEBUG("read32: 0x%08X ", temp); 
	mask = 0xff << ((addr & 0x3) * 8);
	temp &= ~mask;
	temp |= val << ((addr & 0x3) * 8);
	out_le32((void *)(addr & 0xfffffffc), temp);
	PCIE_CSR_DEBUG("out32: 0x%08X\n", temp); 
	return 0;
}

static inline int pcie_cfg_in32(u32 addr, u32 *val)
{
	*val = in_le32((void *) addr);
	PCIE_CSR_DEBUG("PCIE CFG RD32: 0x%08X value: %08X\n", addr, *val);
	return 0;
}

static inline int pcie_cfg_in16(u32 addr, u16 *val)
{
	u32 temp;
	temp = in_le32((void *) (addr & 0xfffffffc));
	PCIE_CSR_DEBUG("PCIE CFG RD16: fullreg 0x%08X", temp);
	*val = temp >> ((addr & 0x3) * 8);
	PCIE_CSR_DEBUG(" addr 0x%08X value: 0x%04X\n", addr, *val); 
	return 0;
}

static inline int pcie_cfg_in8(u32 addr, u8 *val)
{
	u32 temp;
	temp = in_le32((void *) (addr & 0xfffffffc));
	PCIE_CSR_DEBUG("PCIE CFG RD8: fullreg 0x%08X", temp);
	*val = temp >> ((addr & 0x3) * 8);
	PCIE_CSR_DEBUG(" addr 0x%08X value: 0x%02X\n", addr, *val);
	return 0;
}

int pcie_host2port(struct pci_controller *host)
{
	if (host == &pcie_ctx[0].hose)
		return 0;
	if (host == &pcie_ctx[1].hose)
		return 1;
	return 2;
}

/* PCIE Configuration Access Functions
 * We limited bus to 16 as there is only 16MB configuration space.
 */
#define PCIE_OP(rw, size, type, op, mask)                         	\
static int pcie_##rw##_config_##size(struct pci_controller *host,       \
                                     pci_dev_t              dev,        \
                                     int                    offset,     \
                                     type                   val)        \
{                                                                       \
	unsigned int b, d, f;                                           \
	unsigned int port = pcie_host2port(host);                       \
	b = PCI_BUS(dev);                                               \
	d = PCI_DEV(dev);                                               \
	f = PCI_FUNC(dev);                                              \
	if (b >= 16) 							\
		return 0; 						\
	if (port == 2 && f >= 2) {					\
		/* Limit function on x4 to 2. If you need more, 	\
		   increase this limit */				\
		val = 0;						\
		return -1;						\
	}								\
	if (!pcie_ctx[port].link)					\
		return -1;						\
	pcie_csr_out32(pcie_map_tbl[port].csr_addr + RTDID_ADDR,	\
		b << 24 | d << 19 | f << 16 );				\
	offset += dev >> 4;						\
	(op)((u32) host->cfg_data + (offset & (~mask)), (val));		\
	return 0;                                                       \
}

PCIE_OP(read, byte, unsigned char *, pcie_cfg_in8, 0)
PCIE_OP(read, word, unsigned short *, pcie_cfg_in16, 1)
PCIE_OP(read, dword, unsigned int *, pcie_cfg_in32, 3)
PCIE_OP(write, byte, unsigned char, pcie_cfg_out8, 0)
PCIE_OP(write, word, unsigned short, pcie_cfg_out16, 1)
PCIE_OP(write, dword, unsigned int, pcie_cfg_out32, 3)

int apm_pcie_alloc_core(int port)
{
	char s[10], *tk;
	char *pcie_mode;
	u32 i;

        /* Setup PCIE nodes */
	pcie_mode = getenv("pcie_mode");

	strcpy(s, pcie_mode);
	tk = strtok(s, ":");
	for (i = 0; i < port; i++)
		tk = strtok(NULL, ":");

	if ((tk == NULL) || !strcmp(tk,"na") || !strcmp(tk,"NA"))
		return 0;
	
#if CONFIG_SYS_PCIE0_CFGBASE != 0
	if (port == 0) {
		char *str_val = getenv("pcie0");
		if (str_val == NULL) {
			if (CPUID == 0)
				return 1;
			else
				return 0;
		} else if (CPUID == simple_strtol(str_val, NULL, 10)) {
			return 1;
		} else {
			return 0;
		}
	}
#endif
#if CONFIG_SYS_PCIE1_CFGBASE != 0
	if (port == 1) {
		char *str_val = getenv("pcie1");
		if (str_val == NULL) {
			if (CPUID == 0)
				return 1;
			else
				return 0;
		} else if (CPUID == simple_strtol(str_val, NULL, 10)) {
			return 1;
		} else {
			return 0;
		}
	}
#endif
#if CONFIG_SYS_PCIE2_CFGBASE != 0
	if (port == 2) {
		char *str_val = getenv("pcie2");
		if (str_val == NULL) {
			if (CPUID == 0)
				return 1;
			else
				return 0;
		} else if (CPUID == simple_strtol(str_val, NULL, 10)) {
			return 1;
		} else {
			return 0;
		}
	}
#endif
        return 0;
}

int apm_pcie_is_card_host(int port)
{
	u32    val32 = 0;

	if (port >= APM_PCIE_HOST_TOTAL)
		return 0;
	if (pcie_csr_in32(pcie_map_tbl[port].csr_addr + PCIECORECTRL_ADDR,
			  &val32) < 0)
		return 0;

	switch(val32 & DEVICETYPE_F6_MASK) {
	case PTYPE_ENDPOINT:
	case PTYPE_LEGACY_ENDPOINT:
		return 0;
	case PTYPE_ROOTPORT:
	default:
		return 1;
	}
}

int is_pci_root(int port)
{
	char s[10], *tk;
	char *pcie_mode = getenv("pcie_mode");

	if (pcie_mode == NULL)
		return -1;

	strcpy(s, pcie_mode);
	tk = strtok(s, ":");

	/*
	 * return 1 if root point 0 end point
	 */
	switch (port) {
	case 0:
		if (tk != NULL) {
			if (!(strcmp(tk, "ep") && strcmp(tk, "EP")))
				return 0;
			else
				return 1;
		} else {
			return -1;
		}

	case 1:
		tk = strtok(NULL, ":");
		if (tk != NULL) {
			if (!(strcmp(tk, "ep") && strcmp(tk, "EP")))
				return 0;
			else
				return 1;
		} else {
			return -1;
		}

	case 2:
		tk = strtok(NULL, ":");
		if (tk != NULL)
			tk = strtok(NULL, ":");
		if (tk != NULL) {
			if (!(strcmp(tk, "ep") && strcmp(tk, "EP")))
				return 0;
			else
				return 1;
		}
		else
			return -1;
	}

	return -1;
}

void pci_init_board(void)
{
	int busno = 0;

	apm_pcie_setup_hosts(busno);
}

int apm_init_pcie(int port, int type_root_endpoint)
{
	u32 csr_base;
	u32 val;
	struct apm86xxx_reset_ctrl reset_ctrl;
	u32 val_phy_reset_on;
	
	csr_base = (unsigned int) (long) pcie_map_tbl[port].csr_addr;
	
	/* Reset the IP */
	memset(&reset_ctrl, 0, sizeof(reset_ctrl));
	reset_ctrl.reg_group = REG_GROUP_SRST_CLKEN;

	switch (port) {
	case 0:
		reset_ctrl.clken_mask = PCIE1_F1_MASK;
		reset_ctrl.csr_reset_mask = PCIE1_F3_MASK;
		reset_ctrl.reset_mask = PCIE1_MASK;
		reset_ctrl.ram_shutdown_addr = (u32 __iomem *)
			(csr_base + APM_GLBL_DIAG_OFFSET +
			PCIE_CFG_MEM_RAM_SHUTDOWN_ADDR);
		reset_ctrl.mem_rdy_mask = PCIE1_F2_MASK;
		reset_apm86xxx_block(&reset_ctrl);
		break;
	case 1:
		reset_ctrl.clken_mask = PCIE2_F1_MASK;
		reset_ctrl.csr_reset_mask = PCIE2_F3_MASK;
		reset_ctrl.reset_mask = PCIE2_MASK;
		reset_ctrl.ram_shutdown_addr = (u32 __iomem *)
			(csr_base + APM_GLBL_DIAG_OFFSET +
			PCIE_CFG_MEM_RAM_SHUTDOWN_ADDR);
		reset_ctrl.mem_rdy_mask = PCIE2_F2_MASK;
		reset_apm86xxx_block(&reset_ctrl);
		break;
	case 2:
		reset_ctrl.clken_mask = PCIE3_F1_MASK;
		reset_ctrl.csr_reset_mask = PCIE3_F3_MASK;
		reset_ctrl.reset_mask = PCIE3_MASK;
		reset_ctrl.ram_shutdown_addr = (u32 __iomem *)
			(csr_base + APM_GLBL_DIAG_OFFSET +
			PCIE_CFG_MEM_RAM_SHUTDOWN_ADDR);
		reset_ctrl.mem_rdy_mask = PCIE3_F2_MASK;
		reset_apm86xxx_block(&reset_ctrl);
		break;
	}
	
	/* For PCIE1, it is shared with SATA and PCIe2 is shared with trace. 
	 * Select the MUX for PCIE1 
	*/
	if (port == 0 || port == 1) {
		/* SerDes definition for PCIE2, PCIE1, and SATA are identical*/
		csr_inbe32(csr_base + 0x4000 + X1_PCSCSR_CTL0_ADDR, &val);
		val = X1_PCIE_MODE_SEL0_SET(val, 1);
		csr_outbe32(csr_base + 0x4000 + X1_PCSCSR_CTL0_ADDR, val);
	}	
	
	/* Set all CSR registers to little endian */
	pcie_csr_out32(csr_base + ENDIAN_ADDR, 0xFFFFFFFF);
	udelay(100);

	/* Configure PCIEx as root/endpoint complex mode */
	csr_inbe32(csr_base + DEVICETYPE_ADDR, &val);
	val = DEVICETYPE_SET(val, type_root_endpoint);
	csr_outbe32(csr_base + DEVICETYPE_ADDR, val);
	
	switch (port) {
	case 0: /* 1-lane */
	case 1: 
		/* Configure PHY */		
		val = X1_PHY_PCS_RX0_SIGDET0_F2_WR(1) |
			X1_PHY_TX_TERM_CAL_IN0_F2_WR(0xF) |
			X1_MAC_PCS_X2_MODE0_WR(0) |
			X1_MAC_PCS_PLL_PCS_DIV0_F2_WR(0x13) |
			X1_MAC_PCS_PLL_DIV0_WR(0x31) |
			X1_MAC_PCS_PLL_OP_RANGE0_WR(0) |
			X1_MAC_PCS_PLL_REFCLK_SEL0_F2_WR(0) |
			X1_RESET0_F4_WR(1) |
			X1_INIT_CTLIFC_USE_MGMT0_F2_WR(0) |
			X1_INIT_CTLIFC_PIPE_COMPLIANT0_F2_WR(1) |
			X1_INIT_REFCLK_AC_MODE0_F2_WR(0) |
			X1_INIT_TX_TERM_SEL0_F2_WR(1) |
			X1_PCIE_MODE_SEL0_WR(1);
		pcie_csr_out32(csr_base + 0x4000 + X1_PCSCSR_CTL0_ADDR, val);
		pcie_csr_out32(csr_base + 0x4000 + X1_PCSCSR_CTL1_ADDR, 
				X1_MAC_PCS_RX0_ENCDET1_WR(0) |
				X1_MAC_PCS_SATA0_HOST1_WR(1) |
				X1_MAC_PCS_RX0_EDET_LB1_WR(0) |
				X1_MAC_PCS_RX0_EDET_EIE1_WR(0) |
				X1_MAC_PCS_RX0_EDET_TS1_WR(0) |
				X1_MAC_PCS_RX0_EDET_SKP1_WR(0) |
				X1_MAC_PCS_CTL0_OP_RANGE1_F2_WR(1) |
				X1_MAC_PCS_CTL0_SMPL_RATE1_WR(1) |
				X1_MAC_PCS_CTL0_RX_LANE_DIV1_WR(0) |
				X1_MAC_PCS_CTL0_TX_LANE_DIV1_WR(0) |
				X1_MAC_PCS_CTL0_LANE_PWR_OFF1_WR(0) |
				X1_MAC_PCS_CTL0_RX_MODE1_F2_WR(0x4) |
				X1_MAC_PCS_CTL0_TX_MODE1_F2_WR(0x4));

		/* Saving X1_PCSCSR_CTL0_ADDR value before we cleared reset bit */
		val_phy_reset_on = val;		/* Remove PHY out of reset */		
		val &= ~X1_RESET0_F4_MASK;
		pcie_csr_out32(csr_base + 0x4000 + X1_PCSCSR_CTL0_ADDR, val);

		/* Fixme: Due to bug in PHY, again put a PHY in to reset */
		pcie_csr_out32(csr_base + 0x4000 + X1_PCSCSR_CTL0_ADDR, val_phy_reset_on);

		/* Fixme: Due to bug in PHY, Remove PHY out of the reset again */
		pcie_csr_out32(csr_base + 0x4000 + X1_PCSCSR_CTL0_ADDR, val);

		/* Poll PLL for lock and ready */
		while (1) {
			pcie_csr_in32(csr_base + 0x4000 + 
					X1_PCSCSR_STATUS1_ADDR, &val);
			val &= X1_SDS_PCS_CLOCK_READY1_F2_MASK | 
				X1_SDS_PCS_PLL_LOCK1_F2_MASK;
			if (val == (X1_SDS_PCS_CLOCK_READY1_F2_MASK | 
				    X1_SDS_PCS_PLL_LOCK1_F2_MASK))
				break;
			udelay(10);
#ifdef CONFIG_ISS	
			break;	/* For Model, just break out */
#endif		
		}
		break;
	case 2: /* 4-lane */
		/* Configure PHY - reset PHY and take out of reset */		
		pcie_csr_in32(csr_base + 0x4000+X4_PCSCSR_COMMONCTL0_ADDR, &val);
#if 0
		val |= X4_RESET0_F2_MASK; 
		pcie_csr_out32(csr_base + 0x4000+X4_PCSCSR_COMMONCTL0_ADDR, val);
		udelay(1000);
		pcie_csr_in32(csr_base + 0x4000+X4_PCSCSR_COMMONCTL0_ADDR, &val);
#endif
		 val_phy_reset_on = val;
		/* Remove PHY out of reset */
		val &= ~X4_RESET0_F2_MASK;
		pcie_csr_out32(csr_base + 0x4000+X4_PCSCSR_COMMONCTL0_ADDR, val);

		/* Fixme: Due to bug in PHY, Again put a PHY in to reset */
		pcie_csr_out32(csr_base + 0x4000+X4_PCSCSR_COMMONCTL0_ADDR,  val_phy_reset_on);

		/* Fixme: Due to bug in PHY, Remove PHY out of the reset again */
		pcie_csr_out32(csr_base + 0x4000+X4_PCSCSR_COMMONCTL0_ADDR, val);

		/* Due to timing requirement, the RX_CLK in the pcs must be inverted */
		mosys_rdx4(csr_base, 0x318001, &val);
                PCIE_DEBUG("PCIE%d: mosys rd at addr 0x318001: %x\n", port, val);
                val |= 1<<15;
                PCIE_DEBUG("PCIE%d: modified val: %x\n", port, val);
                mosys_wrx4_op(csr_base, 0x3f8001, val);
                mosys_rdx4(csr_base, 0x318001, &val);

		/* Poll PLL for lock and ready */
		while (1) {
			pcie_csr_in32(csr_base + 0x4000 + X4_PCSCSR_STATUS1_ADDR,
					&val);
			val &= X1_SDS_PCS_CLOCK_READY1_F2_MASK | 
				X1_SDS_PCS_PLL_LOCK1_F2_MASK;
			if (val == (X1_SDS_PCS_CLOCK_READY1_F2_MASK | 
				X1_SDS_PCS_PLL_LOCK1_F2_MASK))
				break;
			udelay(10);
#ifdef CONFIG_ISS	
			break;	/* For Model, just break out */
#endif		
		}

		/*
		 * This is a temp fix for E1000 card to get a link
		 * should not have to do this: force lane to x1
		 */
#if 0
		/* To be test and remove for Mamba - FIXME */
		pcie_csr_in32(csr_base + PCIE_PORT_LOGIC_REG_GEN2, &val);
		val = ((val & 0xffff00ff) | 0x0100);
		pcie_csr_out32(csr_base + PCIE_PORT_LOGIC_REG_GEN2, val);
#endif		 
		break;
	}	


	/* Enable ECC generation and checking */		
	pcie_csr_out32(csr_base + PCIE_ADV_CAP_CTRL_ADDR, 0x1E0);

	/* Enable all PCIE event, errror, and its interrupt  */
	pcie_csr_out32(csr_base + EVENTCAPTUREENABLE_ADDR, 0xFFFFFFFF); 
	pcie_csr_out32(csr_base + EVENTINTERUPTSTATUSMASK_ADDR, 0x00000000);
	pcie_csr_out32(csr_base + ERRORCAPTUREENABLE_ADDR, 0xFFFFFFFF);
	pcie_csr_out32(csr_base + ERRORINTERRUPTSTATUSMASK_ADDR, 0x00000000);
	pcie_csr_out32(csr_base + INTXSTATUSMASK_ADDR, 0x00000000);
	/* Enable Correctable Error, Non-Fatal Error reporting,
	   Fatal error reporting, unsupported request reporting,
	   set max_Payload_size to 2. Enable Extended Tag field,
	   Phantom function, AUX power PM, enable snoop Not require,
	   and set MAX_read_request_size to 2. */
	pcie_csr_out32(csr_base + PCIE_DEV_CTRL_ADDR, 0x2F0F);						

#if 0
	/*
         * This is to fix shortcoming where only two outbound request
         * is supported. Together with mtu size, fix the large ping
	 * problem.
	 * This is a performance tunning setting, use with caution
         */
        pcie_csr_out32(csr_base + MASTER_RESP_COMP_CTRL_REG0_ADDR, 0x00000002);
        pcie_csr_out32(csr_base + MASTER_RESP_COMP_CTRL_REG1_ADDR, 0xffffffff);
#endif
	return 0;
}
                   
int apm_setup_pcie_rootpoint(struct pci_controller *host, int port)
{
	struct apm_pcie_map_tbl *hb;
	u32   csr_base;
	u64   val64;
	u32   val32;
	u8    count;

	if (port < 0 || port >= APM_PCIE_HOST_TOTAL) {
		return 0;
	}
	
	pci_set_ops(host,
		pcie_read_config_byte,
		pcie_read_config_word,
		pcie_read_config_dword,
		pcie_write_config_byte,
		pcie_write_config_word,
		pcie_write_config_dword);

	hb = &pcie_map_tbl[port];

	/* High address is appended in function as AXI is 40 bit address
	   range */
	csr_base       = (unsigned int) (long) hb->csr_addr;
	host->cfg_addr = (unsigned int *) (long) hb->csr_addr;
	host->cfg_data = (void *) hb->cfg_vaddr;

        /*
         * Set bus numbers on our root port
	 * Primary 0, second 1, sub 1
         */
        pcie_csr_out32(csr_base + 0x18, 0x00010100);

	/* Call board configuration function */
	#if defined(CONFIG_PCI) && defined(CONFIG_SYS_PCI_MASTER_INIT)
		pci_master_init(host);
	#endif

	/* Set up outbound PCIE translation Memory Region #1:
	 * Translate from host memory address to PCIE address
	 */
	pcie_csr_out32(csr_base + OMR1BARL_ADDR, hb->ob_mem_addr[0].lo);
	pcie_csr_out32(csr_base + OMR1BARH_ADDR, hb->ob_mem_addr[0].hi);
	if (hb->ob_mem_addr[0].size)
		val64 = ~(hb->ob_mem_addr[0].size - 1) | VAL_F2_MASK;
	else
		val64 = 0;
	pcie_csr_out32(csr_base + OMR1MSKL_ADDR, LODWORD(val64));
	pcie_csr_out32(csr_base + OMR1MSKH_ADDR, HIDWORD(val64));
	pcie_csr_out32(csr_base + POM1L_ADDR, hb->ob_mem_addr[0].pcie_lo);
	pcie_csr_out32(csr_base + POM1H_ADDR, hb->ob_mem_addr[0].pcie_hi);
	PCIE_DEBUG("PCIE%d: Outbound 0 HBF 0x%02X_%08X -> PCIE 0x%02X_%08X "
		   "Mask 0x%08X_%08X\n", port,
		hb->ob_mem_addr[0].hi, hb->ob_mem_addr[0].lo,
		hb->ob_mem_addr[0].pcie_hi, hb->ob_mem_addr[0].pcie_lo,
		HIDWORD(val64), LODWORD(val64));		 

	/* Set up outbound PCIE translation memory Region #2:
	 * Translate from host memory address (ie. 0x07_XXXX_XXXX) to PCIE
	 * address at 0x00_0000_0000 + size of 1st memory map.
	 */
	pcie_csr_out32(csr_base + OMR2BARL_ADDR, hb->ob_mem_addr[1].lo);
	pcie_csr_out32(csr_base + OMR2BARH_ADDR, hb->ob_mem_addr[1].hi);
	if (hb->ob_mem_addr[1].size > 0)
		val64 = ~(hb->ob_mem_addr[1].size - 1) | VAL_F3_MASK;
	else
		val64 = 0;
	pcie_csr_out32(csr_base + OMR2MSKL_ADDR, LODWORD(val64));
	pcie_csr_out32(csr_base + OMR2MSKH_ADDR, HIDWORD(val64));
	pcie_csr_out32(csr_base + POM2L_ADDR, hb->ob_mem_addr[1].pcie_lo);
	pcie_csr_out32(csr_base + POM2H_ADDR, hb->ob_mem_addr[1].pcie_hi);
	PCIE_DEBUG("PCIE%d: Outbound 1 HBF 0x%02X_%08X -> PCIE 0x%02X_%08X "
		   "Mask 0x%08X_%08X\n", port,
		hb->ob_mem_addr[1].hi, hb->ob_mem_addr[1].lo,
		hb->ob_mem_addr[1].pcie_hi, hb->ob_mem_addr[1].pcie_lo,
		HIDWORD(val64), LODWORD(val64));		 

	/* Set up outbound PCIE translation I/O Region #3:
	 * Translate from host memory address (ie. 0x07_XXXX_XXXX) to PCIE
	 * address at 0x00_0000_0000 + size of 1st & 2nd memory map.
	 */
	pcie_csr_out32(csr_base + OMR3BARL_ADDR, hb->ob_mem_addr[2].pcie_lo);
	pcie_csr_out32(csr_base + OMR3BARH_ADDR, hb->ob_mem_addr[2].pcie_hi);
	if (hb->ob_mem_addr[2].size > 0)
		val64 = ~(hb->ob_mem_addr[2].size - 1) | VAL_F4_MASK | IO_MASK;
	else
		val64 = 0;
	pcie_csr_out32(csr_base + OMR3MSKL_ADDR, LODWORD(val64));
	pcie_csr_out32(csr_base + OMR3MSKH_ADDR, HIDWORD(val64));
	pcie_csr_out32(csr_base + POM3L_ADDR, hb->ob_mem_addr[2].pcie_lo);
	pcie_csr_out32(csr_base + POM3H_ADDR, hb->ob_mem_addr[2].pcie_hi);
	PCIE_DEBUG("PCIE%d: Outbound 2 HBF 0x%02X_%08X -> PCIE 0x%02X_%08X "
		   "Mask 0x%08X_%08X\n", port,
		hb->ob_mem_addr[2].hi, hb->ob_mem_addr[2].lo,
		hb->ob_mem_addr[2].pcie_hi, hb->ob_mem_addr[2].pcie_lo,
		HIDWORD(val64), LODWORD(val64));		 

	/* Set up outbound Message Region: 2^7 - 2^15
	 * Translate from host memory address (ie. 0x07_XXXX_XXXX) to PCIE
	 * address at 0x00_0000_0000 + size of 1st & 2nd memory map + size
	 * of I/O region.
	 */
	pcie_csr_out32(csr_base + MSGBARL_ADDR, hb->ob_msg_addr_lo);
	pcie_csr_out32(csr_base + MSGBARH_ADDR, hb->ob_msg_addr_hi);
	if (hb->ob_msg_addr_size > 0x8000)
		val32 = 0x7FFF | VAL_F5_MASK;
	else if (hb->ob_msg_addr_size > 0)
		val32 = ((u32) ~(hb->ob_msg_addr_size - 1)) | VAL_F5_MASK;
	else
		val32 = 0;
	pcie_csr_out32(csr_base + MSGMSK_ADDR, val32);
	pcie_csr_out32(csr_base + POM4L_ADDR, hb->ob_msg_pcie_addr_lo);
	pcie_csr_out32(csr_base + POM4H_ADDR, hb->ob_msg_pcie_addr_hi);
	PCIE_DEBUG("PCIE%d: Outbound Msg HBF 0x%02X_%08X -> PCIE 0x%02X_%08X "
		   "Mask 0x%08X\n", port,
		hb->ob_msg_addr_hi, hb->ob_msg_addr_lo,
		hb->ob_msg_pcie_addr_hi, hb->ob_msg_pcie_addr_lo, val32);		 

	/* Set up outbound Configuration Region:
	 * Translate from host memory address (ie. 0x07_XXXX_XXXX) to PCIE
	 * address at 0x00_0000_0000 + size of 1st & 2nd memory map + size
	 * of I/O region + size of message region.
	 */
	pcie_csr_out32(csr_base + CFGBARL_ADDR, hb->cfg_addr_lo);
	pcie_csr_out32(csr_base + CFGBARH_ADDR, hb->cfg_addr_hi);
	if (hb->cfg_addr_size != 0)
		val32 = ((u32) ~(hb->cfg_addr_size - 1)) | VAL_F6_MASK;
	else
		val32 = 0;
	pcie_csr_out32(csr_base + CFGMSK_ADDR, val32);
	pcie_csr_out32(csr_base + POM5L_ADDR, hb->cfg_pcie_lo);
	pcie_csr_out32(csr_base + POM5H_ADDR, hb->cfg_pcie_hi);
	PCIE_DEBUG("PCIE%d: Outbound Cfg HBF 0x%02X_%08X -> PCIE 0x%02X_%08X "
		   "Mask 0x%08X\n", port,
		hb->cfg_addr_hi, hb->cfg_addr_lo,
		hb->cfg_pcie_hi, hb->cfg_pcie_lo, val32);	 

	/* Set up in bound PCIE Memory Region #1:
	*   Translate PCIE Address (ie. 0x00_0000_0000 - 0x0F_FFFF_FFFF) to
	*    AXI range (ie. 0x00_0000_0000 - 0x0F_FFFF_FFFF).
	*/
	pcie_csr_out32(csr_base + PCI_BASE_ADDRESS_0,
		(hb->ib_mem_addr[0].pcie_lo & PCI_BASE_ADDRESS_MEM_MASK) 
		| PCI_BASE_ADDRESS_MEM_TYPE_64);
	pcie_csr_out32(csr_base + PCI_BASE_ADDRESS_1, 
		hb->ib_mem_addr[0].pcie_hi);
	if (hb->ib_mem_addr[0].pcie_size >= (1024*1024))
		val64 = hb->ib_mem_addr[0].pcie_size - 1;	/* min 1 MB */
	else
		val64 = 0;
	pcie_csr_out32(csr_base + BAR0_MSK_L_ADDR, val64);
	pcie_csr_out32(csr_base + BAR0_MSK_H_ADDR, val64 >> 32);
	pcie_csr_out32(csr_base + PIM1_1L_ADDR, hb->ib_mem_addr[0].pim1_lo);
	pcie_csr_out32(csr_base + PIM1_1H_ADDR, hb->ib_mem_addr[0].pim1_hi);
	pcie_csr_out32(csr_base + PIM1_2L_ADDR, hb->ib_mem_addr[0].pim2_lo);
	pcie_csr_out32(csr_base + PIM1_2H_ADDR, hb->ib_mem_addr[0].pim2_hi);
	pcie_csr_out32(csr_base + PIM1SL_ADDR, 
			LODWORD(hb->ib_mem_addr[0].pim_size));
	pcie_csr_out32(csr_base + PIM1SH_ADDR, 
			HIDWORD(hb->ib_mem_addr[0].pim_size));
	PCIE_DEBUG("PCIE%d: Inbound 0 PCIE 0x%02X_%08X -> HBF 0x%02X_%08X "
		   "Mask 0x%08X_%08X\n"
		   "                                    -> 0x%02X_%08X\n",
		port,
		hb->ib_mem_addr[0].pcie_hi, hb->ib_mem_addr[0].pcie_lo,
		hb->ib_mem_addr[0].pim1_hi, hb->ib_mem_addr[0].pim1_lo,
		HIDWORD(val64), LODWORD(val64),	 
		hb->ib_mem_addr[0].pim2_hi, hb->ib_mem_addr[0].pim2_lo);

	/* Set up in bound PCIE Memory Region #2 */
	pcie_csr_out32(csr_base + IBAR2L_ADDR,
		(hb->ib_mem_addr[1].pcie_lo & PCI_BASE_ADDRESS_MEM_MASK) 
		| PCI_BASE_ADDRESS_MEM_TYPE_64);
	pcie_csr_out32(csr_base + IBAR2H_ADDR, hb->ib_mem_addr[1].pcie_hi);
	if (hb->ib_mem_addr[1].pcie_size >= (1024*1024))
		val64 = ~(hb->ib_mem_addr[1].pcie_size - 1)
			| VAL_MASK; /* min is 1 MB */
	else
		val64 = 0;
	pcie_csr_out32(csr_base + IR2MSKL_ADDR, LODWORD(val64));
	pcie_csr_out32(csr_base + IR2MSKH_ADDR, HIDWORD(val64));
	pcie_csr_out32(csr_base + PIM2_1L_ADDR, hb->ib_mem_addr[1].pim1_lo);
	pcie_csr_out32(csr_base + PIM2_1H_ADDR, hb->ib_mem_addr[1].pim1_hi);					   
	pcie_csr_out32(csr_base + PIM2_2L_ADDR, hb->ib_mem_addr[1].pim2_lo);
	pcie_csr_out32(csr_base + PIM2_2H_ADDR, hb->ib_mem_addr[1].pim2_hi);
	pcie_csr_out32(csr_base + PIM2SL_ADDR,
		     LODWORD(hb->ib_mem_addr[1].pim_size));
	pcie_csr_out32(csr_base + PIM2SH_ADDR,
		     HIDWORD(hb->ib_mem_addr[1].pim_size));
	PCIE_DEBUG("PCIE%d: Inbound 1 PCIE 0x%02X_%08X -> HBF 0x%02X_%08X "
		   "Mask 0x%02X_%08X\n"
		   "                                    ->     0x%02X_%08X\n",
		port,
		hb->ib_mem_addr[1].pcie_hi, hb->ib_mem_addr[1].pcie_lo,
		hb->ib_mem_addr[1].pim1_hi, hb->ib_mem_addr[1].pim1_lo,
		HIDWORD(val64), LODWORD(val64),	 
		hb->ib_mem_addr[1].pim2_hi, hb->ib_mem_addr[1].pim2_lo);

	/* Set up in bound PCIE I/O Region: Not used */
	pcie_csr_out32(csr_base + IR3MSK_ADDR, 0);

	/* Expansion ROM Region #4 */
	pcie_csr_out32(csr_base + PIM4L_ADDR, hb->ib_rom_addr_lo);
	pcie_csr_out32(csr_base + PIM4H_ADDR, hb->ib_rom_addr_hi);
	if (hb->ib_rom_addr_size >= (2*1024))	/* min 2KB */
		val32 = ~(hb->ib_rom_addr_size - 1) | PCIE_BAR_EN;
	else
		val32 = 0;
	pcie_csr_out32(csr_base + EXPROM_RC_MSK_ADDR, val32);

	/* Enable I/O, Mem, and Busmaster cycles
	   NOTE: The status register (higher 2 bytes) is not affected
		as we are writing 0's
	 */
	pcie_csr_in32(csr_base + PCI_COMMAND, &val32);
	pcie_csr_out32(csr_base + PCI_COMMAND, 
		val32 | PCI_COMMAND_IO | PCI_COMMAND_MEMORY | 
		PCI_COMMAND_MASTER);

	/* Set us up as bridge */
	pcie_csr_out32(csr_base + PCI_CLASS_REVISION, 0x06040001); 

retry_link:
	/* Set Core as Root Complex and Enable core Link establishment */
	pcie_csr_out32(csr_base + PCIECORECTRL_ADDR, APPLTSSMENABLE_MASK);

	/* Wait until PHY and Data Link are up */		
	count = 10;
	do {
		pcie_csr_in32(csr_base + PCIECORESTATUS_ADDR, &val32);
		val32 &= RDHLLINKUP_MASK | XMLHLINKUP_MASK;
		udelay(1000);
#ifdef CONFIG_ISS
		break; /* For model, just break out */
#endif
	} while (val32 != (RDHLLINKUP_MASK | XMLHLINKUP_MASK) && --count > 0);

	if (count == 0) {
		/* force link speed */
		pcie_csr_in32(csr_base + PCIE_LINK_CAP_ADDR, &val32);
		if (val32 & 0x2) {
			pcie_csr_out32(csr_base + PCIE_LINK_CAP_ADDR,
						(val32 & 0xfffffff0) | 0x1);
			pcie_csr_out32(csr_base + PCIECORECTRL_ADDR, 0);
			goto retry_link;
		}

	} else {
		pcie_ctx[port].link = 1;
	}

	printf("PCIE%d: RC ", port);
	if (count == 0)
		printf("(link down)\n");
	else
		printf("(link up)\n");

	return 0;
}

int apm_setup_pcie_endpoint(struct pci_controller *host, int port)
{
	struct apm_pcie_map_tbl *hb;
	u64   val64;
	u32   val32;
	u32   csr_base;
	u8    count = 10;

	if (port < 0 || port >= APM_PCIE_HOST_TOTAL) {
		return 0;
	}
	pci_set_ops(host,
		pcie_read_config_byte,
		pcie_read_config_word,
		pcie_read_config_dword,
		pcie_write_config_byte,
		pcie_write_config_word,
		pcie_write_config_dword);

	hb 	       = &pcie_map_tbl[port];
	csr_base       = (unsigned int) (long) hb->csr_addr;
	host->cfg_addr = (unsigned int *) (ulong) hb->csr_addr;
	host->cfg_data = (void *) hb->cfg_vaddr;
	
	/* Call board configuration function */
	#if defined(CONFIG_PCI) && defined(CONFIG_SYS_PCI_TARGET_INIT)
		pci_target_init(host);			
	#endif
	
	/* Set up outbound PCIE translation Memory Region #1:
	 * Translate from host memory address (ie. 0x07_XXXX_XXXX) to PCIE
	 * address
	 *
	 * Please note that register endianess depends whether the register
	 * resides inside PCIe Core or by our glue logic. See specification
	 * for more info.
	 */
	pcie_csr_out32(csr_base + OMR1BARL_ADDR, hb->ob_mem_addr[0].lo);
	pcie_csr_out32(csr_base + OMR1BARH_ADDR, hb->ob_mem_addr[0].hi);
	if (hb->ob_mem_addr[0].size)
		val64 = ~(hb->ob_mem_addr[0].size - 1) | VAL_F2_MASK;
	else
		val64 = 0;
	pcie_csr_out32(csr_base + OMR1MSKL_ADDR, LODWORD(val64));
	pcie_csr_out32(csr_base + OMR1MSKH_ADDR, HIDWORD(val64));
	pcie_csr_out32(csr_base + POM1L_ADDR, hb->ob_mem_addr[0].pcie_lo);
	pcie_csr_out32(csr_base + POM1H_ADDR, hb->ob_mem_addr[0].pcie_hi);
	PCIE_DEBUG("PCIE%d: Outbound 0 HBF 0x%02X_%08X -> PCIE 0x%02X_%08X "
		   "Mask 0x%02X_%08X\n", port,
		hb->ob_mem_addr[0].hi, hb->ob_mem_addr[0].lo,
		hb->ob_mem_addr[0].pcie_hi, hb->ob_mem_addr[0].pcie_lo,
		HIDWORD(val64), LODWORD(val64));		 

	/* Set up outbound PCIE translation memory Region #2:
	 * Translate from host memory address (ie. 0x07_XXXX_XXXX) to PCIE
	 * address
	 */
	pcie_csr_out32(csr_base + OMR2BARL_ADDR, hb->ob_mem_addr[1].lo);
	pcie_csr_out32(csr_base + OMR2BARH_ADDR, hb->ob_mem_addr[1].hi);
	if (hb->ob_mem_addr[1].size > 0)
		val64 = ~(hb->ob_mem_addr[1].size - 1) | VAL_F3_MASK;
	else
		val64 = 0;
	pcie_csr_out32(csr_base + OMR2MSKL_ADDR, LODWORD(val64));
	pcie_csr_out32(csr_base + OMR2MSKH_ADDR, HIDWORD(val64));
	pcie_csr_out32(csr_base + POM2L_ADDR, hb->ob_mem_addr[1].pcie_lo);
	pcie_csr_out32(csr_base + POM2H_ADDR, hb->ob_mem_addr[1].pcie_hi);
	PCIE_DEBUG("PCIE%d: Outbound 1 HBF 0x%02X_%08X -> PCIE 0x%02X_%08X "
		   "size 0x%02X_%08X\n", port,
		hb->ob_mem_addr[1].hi, hb->ob_mem_addr[1].lo,
		hb->ob_mem_addr[1].pcie_hi, hb->ob_mem_addr[1].pcie_lo,
		HIDWORD(val64), LODWORD(val64));		 

	/* Set up outbound PCIE translation I/O Region #3: not used */
	pcie_csr_out32(csr_base + OMR3BARL_ADDR, 0x000000);
	pcie_csr_out32(csr_base + OMR3BARH_ADDR, 0x000000);
	pcie_csr_out32(csr_base + OMR3MSKL_ADDR, 0x000000);
	pcie_csr_out32(csr_base + OMR3MSKH_ADDR, 0x000000);
	pcie_csr_out32(csr_base + POM3L_ADDR, 0x000000);
	pcie_csr_out32(csr_base + POM3H_ADDR, 0x000000);

	/* Set up outbound Message Region: 2^7 - 2^15
	 * Translate from host memory address (ie. 0x07_XXXX_XXXX) to PCIE
	 * address
	 */
	pcie_csr_out32(csr_base + MSGBARL_ADDR, 0x000000);
	pcie_csr_out32(csr_base + MSGBARH_ADDR, 0x000000);
	pcie_csr_out32(csr_base + MSGMSK_ADDR, 0x000000);
	pcie_csr_out32(csr_base + POM4L_ADDR, 0x000000);
	pcie_csr_out32(csr_base + POM4H_ADDR, 0x000000);

	/* No outbound configuration region for endpoint device */

	/* Set up in bound PCIE Memory Region #1:
	*   PCIE Address at 0 ==> AXI range from 0 - 0x0F_FFFF_FFFF
	*   64-bits address/No prefetchable/Memory BAR
	*/
	pcie_csr_out32(csr_base + PCI_BASE_ADDRESS_0,
		   (hb->ib_mem_addr[0].pcie_lo & PCI_BASE_ADDRESS_MEM_MASK)
		   | PCI_BASE_ADDRESS_MEM_TYPE_64);
	pcie_csr_out32(csr_base + PCI_BASE_ADDRESS_1,
		   hb->ib_mem_addr[0].pcie_hi);
	if (hb->ib_mem_addr[0].pcie_size > 0)
		val64 = hb->ib_mem_addr[0].pcie_size - 1;
	else
		val64 = 0;
	pcie_csr_out32(csr_base + BAR0_MSK_L_ADDR, LODWORD(val64));
	pcie_csr_out32(csr_base + BAR0_MSK_H_ADDR, HIDWORD(val64));
	pcie_csr_out32(csr_base + PIM1_1L_ADDR, hb->ib_mem_addr[0].pim1_lo);
	pcie_csr_out32(csr_base + PIM1_1H_ADDR, hb->ib_mem_addr[0].pim1_hi);
	pcie_csr_out32(csr_base + PIM1_2L_ADDR, hb->ib_mem_addr[0].pim2_lo);
	pcie_csr_out32(csr_base + PIM1_2H_ADDR, hb->ib_mem_addr[0].pim2_hi);
	pcie_csr_out32(csr_base + PIM1SL_ADDR,
			LODWORD(hb->ib_mem_addr[0].pim_size));
	pcie_csr_out32(csr_base + PIM1SH_ADDR,
			HIDWORD(hb->ib_mem_addr[0].pim_size));
	PCIE_DEBUG("PCIE%d: Inbound 0 PCIE 0x%02X_%08X -> HBF 0x%02X_%08X "
		   "Mask 0x%02X_%08X\n"
		   "                                    ->     0x%02X_%08X\n",
		port,
		hb->ib_mem_addr[0].pcie_hi, hb->ib_mem_addr[0].pcie_lo,
		hb->ib_mem_addr[0].pim1_hi, hb->ib_mem_addr[0].pim1_lo,
		HIDWORD(val64), LODWORD(val64),	 
		hb->ib_mem_addr[0].pim2_hi, hb->ib_mem_addr[0].pim2_lo);

	/* Set up in bound PCIE Memory Region #2 */
	pcie_csr_out32(csr_base + PCI_BASE_ADDRESS_4,
		   (hb->ib_mem_addr[1].pcie_lo & PCI_BASE_ADDRESS_MEM_MASK) 
		   | PCI_BASE_ADDRESS_MEM_TYPE_64);
	pcie_csr_out32(csr_base + PCI_BASE_ADDRESS_5,
		   hb->ib_mem_addr[1].pcie_hi);
	if (hb->ib_mem_addr[1].pcie_size > 0)
		val64 = (hb->ib_mem_addr[1].pcie_size - 1) | 0xFFFFE |
			PCIE_BAR_EN;	 /* min is 1 MB */
	else
		val64 = 0;
	pcie_csr_out32(csr_base + BAR2_MSK_L_ADDR, LODWORD(val64));
	pcie_csr_out32(csr_base + BAR2_MSK_H_ADDR, HIDWORD(val64));
	pcie_csr_out32(csr_base + PIM2_1L_ADDR, hb->ib_mem_addr[1].pim1_lo);
	pcie_csr_out32(csr_base + PIM2_1H_ADDR, hb->ib_mem_addr[1].pim1_hi);
	pcie_csr_out32(csr_base + PIM2_2L_ADDR, hb->ib_mem_addr[1].pim2_lo);
	pcie_csr_out32(csr_base + PIM2_2H_ADDR, hb->ib_mem_addr[1].pim2_hi);
	pcie_csr_out32(csr_base + PIM2SL_ADDR,
		     LODWORD(hb->ib_mem_addr[1].pim_size));
	pcie_csr_out32(csr_base + PIM2SH_ADDR,
		     HIDWORD(hb->ib_mem_addr[1].pim_size));
	PCIE_DEBUG("PCIE%d: Inbound 1 PCIE 0x%02X_%08X -> HBF 0x%02X_%08X "
		   "Mask 0x%02X_%08X\n"
		   "                                    ->     0x%02X_%08X\n",
		port,
		hb->ib_mem_addr[1].pcie_hi, hb->ib_mem_addr[1].pcie_lo,
		hb->ib_mem_addr[1].pim1_hi, hb->ib_mem_addr[1].pim1_lo,
		HIDWORD(val64), LODWORD(val64),	 
		hb->ib_mem_addr[1].pim2_hi, hb->ib_mem_addr[1].pim2_lo);

	/* Set up in bound PCIE I/O Region: Not used */
	pcie_csr_out32(csr_base + PCI_BASE_ADDRESS_2, 0x00000000);
	pcie_csr_out32(csr_base + BAR3_MSK_ADDR, 0x00000000);
	pcie_csr_out32(csr_base + PIM3L_ADDR, 0x00000000);
	pcie_csr_out32(csr_base + PIM3H_ADDR, 0x00000000 & ~0xFF00);

	/* Expansion ROM Region #4: */
	pcie_csr_out32(csr_base + PIM4L_ADDR, hb->ib_rom_addr_lo);
	pcie_csr_out32(csr_base + PIM4H_ADDR, hb->ib_rom_addr_hi);
	if (hb->ib_rom_addr_size > 0)	/* min 2KB */
		val32 = (hb->ib_rom_addr_size - 1) | 0x7FE | PCIE_BAR_EN;
	else
		val32 = 0;
	pcie_csr_out32(csr_base + EXPROM_MSK_ADDR, val32);

	/* Enable I/O, Mem, and Busmaster cycles
	NOTE: The status register (higher 2 bytes) is not affected
		as we are writing 0's
	*/
	pcie_csr_in32(csr_base + PCI_COMMAND, &val32);
	pcie_csr_out32(csr_base + PCI_COMMAND, 
		val32 | PCI_COMMAND_IO | PCI_COMMAND_MEMORY | 
		PCI_COMMAND_MASTER);

	/* Set Core as Root Complex and Enable core Link establishment */
	pcie_csr_out32(csr_base + PCIECORECTRL_ADDR,
		PTYPE_ENDPOINT | APPLTSSMENABLE_MASK);

	/* Wait until PHY and Data Link are up */		
	do {
		pcie_csr_in32(csr_base + PCIECORESTATUS_ADDR, &val32);
		val32 &= RDHLLINKUP_MASK | XMLHLINKUP_MASK;
		udelay(1000);
#ifdef CONFIG_ISS
		break; /* For model, just break out */
#endif
	} while (val32 != (RDHLLINKUP_MASK | XMLHLINKUP_MASK) && --count > 0);

	if (count != 0) {
		pcie_ctx[port].link = 1;
		printf("PCIE%d: No link detected for endpoint\n",port);
	}

	printf("PCIE%d: EP ",port);
	if (count == 0)
		printf("(link down)\n");
	else
		printf("(link up)\n");
	return 0;
}

void apm_pcie_setup_hosts(int busno)
{
	struct pci_controller *host;
	int bus;
	int res;
	
	for (bus = 0; 
	     (bus < APM_PCIE_HOST_TOTAL) && (is_pci_root(bus) != -1); 
	     bus++) {
		if (!apm_pcie_alloc_core(bus))
			continue;

		res = apm_init_pcie(bus, is_pci_root(bus) ? 
					      PTYPE_ROOTPORT : PTYPE_ENDPOINT);
		if (res < 0) {
			printf("PCIE%d: initialization failed %d\n", bus, res);
			continue;
		}

		host 		  = &pcie_ctx[bus].hose;
		host->first_busno = busno;
		host->last_busno  = busno++;

		/* setup resource */
		host->region_count = 0;
                /* setup mem resource */
		while (host->region_count < 3 &&
                       pcie_map_tbl[bus].ob_mem_addr[host->region_count].size) {
	                pci_set_region(host->regions + host->region_count,
                               MAKE_U64(pcie_map_tbl[bus].ob_mem_addr[host->region_count].pcie_hi,
                               		pcie_map_tbl[bus].ob_mem_addr[host->region_count].pcie_lo),
                               MAKE_U64(pcie_map_tbl[bus].ob_mem_addr[host->region_count].hi,
                               		pcie_map_tbl[bus].ob_mem_addr[host->region_count].lo),
                               pcie_map_tbl[bus].ob_mem_addr[host->region_count].size,
                               host->region_count <= 1 ? PCI_REGION_MEM :
                               				 PCI_REGION_IO);
			host->region_count++;
		}
		
		pci_register_hose(host);

		if (is_pci_root(bus) == 1) {
			apm_setup_pcie_rootpoint(host, bus);
			if (pcie_ctx[bus].link) {
				debug("Scanning port %d\n", bus);
				host->last_busno = pci_hose_scan(host);
			} else {
				debug("No link found for port %d, skip scan\n", 
					bus);
			}
		} else if (is_pci_root(bus) == 0) {
			apm_setup_pcie_endpoint(host, bus);
			/*
			 * Reson for no scanning is endpoint can not generate
			 * upstream configuration accesses.
			 */
		} else {
			printf("No pcie_mode defined\n");
		}
	}
}

#endif /* CONFIG_PCI */
