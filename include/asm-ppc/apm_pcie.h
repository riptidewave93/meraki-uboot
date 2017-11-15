/**
 * APM SoC APM86xxx PCIe Header File
 *
 * Copyright (c) 2010 Applied Micro Circuits Corporation.
 * All rights reserved. Loc Ho <lho@amcc.com>.
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
 * Definition for PCIE inbound and outbound mapping and configuration
 * PCIE as endpoint and root complex.
 *
 */
#if !defined(__APM_PCIE_H__) && defined(CONFIG_PCI) 
#define __APM_PCIE_H__

/* Offset map from host to PCI. PCIE address for output will be offset
   by this value */
#define PCIE0_ADDR_OFFSET	((((u64) CONFIG_SYS_PCIE0_PHYMEMBASE_HI << 32)|\
					CONFIG_SYS_PCIE0_PHYMEMBASE_LO) + CONFIG_SYS_PCIE0_MEMBASE)
#define PCIE1_ADDR_OFFSET	((((u64) CONFIG_SYS_PCIE1_PHYMEMBASE_HI << 32)|\
					CONFIG_SYS_PCIE1_PHYMEMBASE_LO) + CONFIG_SYS_PCIE1_MEMBASE)
#define PCIE2_ADDR_OFFSET	((((u64) CONFIG_SYS_PCIE2_PHYMEMBASE_HI << 32)|\
					CONFIG_SYS_PCIE2_PHYMEMBASE_LO) + CONFIG_SYS_PCIE2_MEMBASE)

/* PCIE0 - CSR Base Address */
#define PCIE_CSR_SIZE       		(8*1024)
#define PCIE0_CSR_BASE_HI		((u32) (PCIE_CSR_REGS1_BASE_ADDR>>32))
#define PCIE0_CSR_BASE_LO		((u32) PCIE_CSR_REGS1_BASE_ADDR)
/* PCIE0 - Configuration Base Address */
#define PCIE0_CFGBASE_HI		CONFIG_SYS_PCIE0_PHYMEMBASE_HI
#define PCIE0_CFGBASE_LO		(CONFIG_SYS_PCIE0_PHYMEMBASE_LO + CONFIG_SYS_PCIE0_CFGBASE)
#define PCIE_CFGSIZE        		CONFIG_SYS_PCIEX_CFGSIZE
/* PCIE0 - Message Base Address */
#define PCIE0_MSGBASE_HI		0x00000000
#define PCIE0_MSGBASE_LO		0x00000000
#define PCIE0_MSGSIZE			0 	/* 0 for not used */
/* PCIE0 - Outbound I/O Base Address */
#define PCIE0_OB_IOBASE_HI		0x00000000
#define PCIE0_OB_IOBASE_LO		0x00000000
#define PCIE0_OB_IOSIZE			0	/* 0 for not used */
/* PCIE0 - Outbound Memory 1 Base Address */
#define PCIE0_OB_MEMBASE0_HI		CONFIG_SYS_PCIE0_PHYMEMBASE_HI
#define PCIE0_OB_MEMBASE0_LO		(CONFIG_SYS_PCIE0_PHYMEMBASE_LO + CONFIG_SYS_PCIE0_MEMBASE)
#define PCIE0_OB_MEMSIZE0		CONFIG_SYS_PCIE0_MEMSIZE
/* PCIE0 - Outbound Memory 2 Base Address */
#define PCIE0_OB_MEMBASE1_HI		0x00000000
#define PCIE0_OB_MEMBASE1_LO		0x00000000
#define PCIE0_OB_MEMSIZE1		0	/* 0 for not used */
/* * PCIE0 - Inbound Memory 1 Base Address */
#define PCIE0_IB_MEMBASE0_PCIE_HI	((u32) (CONFIG_SYS_PCIE_INBOUND_BASE >> 32))
#define PCIE0_IB_MEMBASE0_PCIE_LO	((u32) CONFIG_SYS_PCIE_INBOUND_BASE)
#define PCIE0_IB_MEMSIZE0		0x10000000ULL
#define PCIE0_IB_MEMBASE0_PIM_SIZE	0xfffffffff0000000ULL
/* PCIE0 - Inbound Memory 1 Base Address */
#define PCIE0_IB_MEMBASE1_PCIE_HI	0x00000000
#define PCIE0_IB_MEMBASE1_PCIE_LO	0x00000000
#define PCIE0_IB_MEMSIZE1		0
#define PCIE0_IB_MEMBASE1_PIM_SIZE	0
#define PCIE0_IB_ROMBASE_HI		0
#define PCIE0_IB_ROMBASE_LO		0
#define PCIE0_IB_ROMSIZE		0

/* PCIE1 - CSR Base Address */
#define PCIE1_CSR_BASE_HI		((u32) (PCIE_CSR_REGS2_BASE_ADDR>>32))
#define PCIE1_CSR_BASE_LO		((u32) PCIE_CSR_REGS2_BASE_ADDR)
/* PCIE1 - Configuration Base Address */
#define PCIE1_CFGBASE_HI		CONFIG_SYS_PCIE1_PHYMEMBASE_HI
#define PCIE1_CFGBASE_LO		(CONFIG_SYS_PCIE1_PHYMEMBASE_LO + CONFIG_SYS_PCIE1_CFGBASE)
/* PCIE1 - Message Base Address */
#define PCIE1_MSGBASE_HI		0x00000000
#define PCIE1_MSGBASE_LO		0x00000000
#define PCIE1_MSGSIZE			0	/* 0 for not used */
/* PCIE1 - Outbound I/O Base Address */
#define PCIE1_OB_IOBASE_HI		0x00000000
#define PCIE1_OB_IOBASE_LO		0x00000000
#define PCIE1_OB_IOSIZE			0	/* 0 for not used */
/* PCIE1 - Outbound Memory 1 Base Address */
#define PCIE1_OB_MEMBASE0_HI		CONFIG_SYS_PCIE1_PHYMEMBASE_HI
#define PCIE1_OB_MEMBASE0_LO		(CONFIG_SYS_PCIE1_PHYMEMBASE_LO + CONFIG_SYS_PCIE1_MEMBASE)
#define PCIE1_OB_MEMSIZE0		CONFIG_SYS_PCIE1_MEMSIZE
/* PCIE1 - Outbound Memory 2 Base Address */
#define PCIE1_OB_MEMBASE1_HI		0x00000000
#define PCIE1_OB_MEMBASE1_LO		0x00000000
#define PCIE1_OB_MEMSIZE1		0	/* 0 for not used */
/* * PCIE1 - Inbound Memory 1 Base Address */
#define PCIE1_IB_MEMBASE0_PCIE_HI	((u32) (CONFIG_SYS_PCIE_INBOUND_BASE >> 32))
#define PCIE1_IB_MEMBASE0_PCIE_LO	((u32) CONFIG_SYS_PCIE_INBOUND_BASE)
#define PCIE1_IB_MEMSIZE0		0x10000000ULL
#define PCIE1_IB_MEMBASE0_PIM_SIZE	0xfffffffff0000000ULL
/* PCIE1 - Inbound Memory 1 Base Address */
#define PCIE1_IB_MEMBASE1_PCIE_HI	0x00000000
#define PCIE1_IB_MEMBASE1_PCIE_LO	0x00000000
#define PCIE1_IB_MEMSIZE1		0
#define PCIE1_IB_MEMBASE1_PIM1_HI	0x00000000
#define PCIE1_IB_MEMBASE1_PIM1_LO	0x00000000
#define PCIE1_IB_MEMBASE1_PIM2_HI	0x00000000
#define PCIE1_IB_MEMBASE1_PIM2_LO	0x00000000
#define PCIE1_IB_MEMBASE1_PIM_SIZE	0
#define PCIE1_IB_ROMBASE_HI		0
#define PCIE1_IB_ROMBASE_LO		0
#define PCIE1_IB_ROMSIZE		0

/* PCIE2 - CSR Base Address */
#define PCIE2_CSR_BASE_HI		((u32) (PCIE_CSR_REGS3_BASE_ADDR>>32))
#define PCIE2_CSR_BASE_LO		((u32) PCIE_CSR_REGS3_BASE_ADDR)
/* PCIE2 - Configuration Base Address */
#define PCIE2_CFGBASE_HI		CONFIG_SYS_PCIE2_PHYMEMBASE_HI
#define PCIE2_CFGBASE_LO		(CONFIG_SYS_PCIE2_PHYMEMBASE_LO + CONFIG_SYS_PCIE2_CFGBASE)
/* PCIE2 - Message Base Address */
#define PCIE2_MSGBASE_HI		0x00000000
#define PCIE2_MSGBASE_LO		0x00000000
#define PCIE2_MSGSIZE			0
/* PCIE2 - Outbound I/O Base Address */
#define PCIE2_OB_IOBASE_HI		0x00000000
#define PCIE2_OB_IOBASE_LO		0x00000000
#define PCIE2_OB_IOSIZE			0	/* 0 for not used */
/* PCIE2 - Outbound Memory 1 Base Address */
#define PCIE2_OB_MEMBASE0_HI		CONFIG_SYS_PCIE2_PHYMEMBASE_HI
#define PCIE2_OB_MEMBASE0_LO		(CONFIG_SYS_PCIE2_PHYMEMBASE_LO + CONFIG_SYS_PCIE2_MEMBASE)
#define PCIE2_OB_MEMSIZE0		CONFIG_SYS_PCIE2_MEMSIZE
/* PCIE2 - Outbound Memory 2 Base Address */
#define PCIE2_OB_MEMBASE1_HI		0x00000000
#define PCIE2_OB_MEMBASE1_LO		0x00000000
#define PCIE2_OB_MEMSIZE1		0	/* 0 for not used */
/* * PCIE2 - Inbound Memory 1 Base Address */
#define PCIE2_IB_MEMBASE0_PCIE_HI	((u32) (CONFIG_SYS_PCIE_INBOUND_BASE >> 32))
#define PCIE2_IB_MEMBASE0_PCIE_LO	((u32) CONFIG_SYS_PCIE_INBOUND_BASE)
#define PCIE2_IB_MEMSIZE0		0x10000000ULL
#define PCIE2_IB_MEMBASE0_PIM_SIZE	0xfffffffff0000000ULL
/* PCIE2 - Inbound Memory 1 Base Address */
#define PCIE2_IB_MEMBASE1_PCIE_HI	0x00000000
#define PCIE2_IB_MEMBASE1_PCIE_LO	0x00000000
#define PCIE2_IB_MEMSIZE1		0
#define PCIE2_IB_MEMBASE1_PIM1_HI	0x00000000
#define PCIE2_IB_MEMBASE1_PIM1_LO	0x00000000
#define PCIE2_IB_MEMBASE1_PIM2_HI	0x00000000
#define PCIE2_IB_MEMBASE1_PIM2_LO	0x00000000
#define PCIE2_IB_MEMBASE1_PIM_SIZE	0
#define PCIE2_IB_ROMBASE_HI		0
#define PCIE2_IB_ROMBASE_LO		0
#define PCIE2_IB_ROMSIZE		0

struct apm_pcie_map_tbl {
	u64 csr_addr;    		/* Host bridge CSR address */
	u32 csr_addr_size;

	/* Outbound AXI address for translation to PCIE */
	u32 cfg_addr_hi;    		/* Configuration AXI address for all */
	u32 cfg_addr_lo;		/* PCI devices */
	u32 cfg_addr_size;		/* use 0 for not used */
	u32 cfg_pcie_hi;
	u32 cfg_pcie_lo;		
	u32 cfg_vaddr;			/* Virtual address */
	struct {
		u32 hi;   		/* Host address map and size */
		u32 lo;
		u64 size;
		u32 pcie_hi;   		/* PCIE address map  and size */
		u32 pcie_lo;
	} ob_mem_addr[3];		/* Inbound two memory and one IO regions */
	u32 ob_msg_addr_hi;		/* Message region address map */
	u32 ob_msg_addr_lo;
	u64 ob_msg_addr_size;		/* use 0 for not used */
	u32 ob_msg_pcie_addr_hi;	/* PCIE address map */
	u32 ob_msg_pcie_addr_lo;

	/* Inbound AXI address for translation from PCIE */
	struct {
		u32 pcie_hi;   		/* PCIE address map */
		u32 pcie_lo;
		u64 pcie_size; 		/* use 0 for not used */
		u32 pim1_hi;   		/* Host address map first half */
		u32 pim1_lo;
		u32 pim2_hi;   		/* To set R/W attribute, OR's them
					   with address */
		u32 pim2_lo;
		u64 pim_size;  		/* Host address map second half */
	} ib_mem_addr[2];		/* Inbound two memory regions */
	u32 ib_rom_addr_hi;		/* Inbound expansion rom map address */
	u32 ib_rom_addr_lo;
	u32 ib_rom_addr_size;  		/* use 0 for not used */
};

#define APM_PCIE_HOST_TOTAL         	CONFIG_PCIE_MAX_PORTS

/**
 * @brief Initializes the AppliedMicro SoC PCIe Bridge
 *
 */
void apm_pcie_setup_hosts(int busno);

/**
 * @brief  This function determines if PCIE controller at bus 'port' present
 *         or not.
 * @param  port    PCIE controller port to check
 * @return 1 if present, 0 otherwise
 *
 */
int apm_pcie_is_card_present(int port);

/**
 * @brief  This function determines if PCIE controller is host or endpoint
 * @param  port    A integer port number
 * @return 1 if present, 0 otherwise
 *
 */
int apm_pcie_is_card_host(int port);

/**
 * @brief  This function determines if PCIE controller at bus 'port'
 *          should be configured as host controller (Root Complex).
 * @param  host   Host controller structure
 * @return 1 for configure as HOST, 0 otherwise
 *
 */
int apm_pcie_is_host(struct pci_controller *host);

/**
 * @brief  This function determines if PCIE controller at port 'port' should
 *          be configured as host (Root Complex) or endpoint.
 * @param  port   PCIE controller port
 * @return 0 if successfull, < 0 if failed
 * @note This function is implemented by each specify board to indicate
 *       which controller port should be host/endpoint. Its header is declared
 *       here.
 *
 */
int is_pci_root(int port);

/**
 * @brief This function writes to host CSR register 
 * @param addr	Offset addres of CSR
 * @param val   Value for write operation
 * @return 0 if successful, < 0 if failed 
 */
int pcie_csr_out32(u32 addr, u32 val);

/**
 * @brief This function read from host CSR register 
 * @param addr	Offset addres of CSR
 * @param val   Value for read operation
 * @return 0 if successful, < 0 if failed 
 */
int pcie_csr_in32(u32 addr, u32 *val);

#endif
