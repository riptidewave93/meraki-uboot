/**
 * AppliedMicro SoC 96xxx mosys driver 
 *
 * Copyright (c) 2010 Applied Micro Circuits Corporation.
 * All rights reserved. Avinash Vijayvargia <avijayvergia@apm.com>
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
 * This module implements mosys functionality for APM96xxx. Using these APIs 
 * one can do r/w to mosys.
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

#include "asm/apm_pcie.h"
#include "asm/apm_pcie_csr.h"
#include "asm/apm_mosys.h"

/* mosys is located at 0x4000 in PCIe and SATA */
#define MOSYS_OFFSET 0x4000

#if 0
#define MOSYS_CSR_DEBUG          printf
#else
#define MOSYS_CSR_DEBUG(x, ...)
#endif

/* r/w to mosys csr (indirect) */
int mosys_wr(u32 addr, u32 offset, u32 *pdata)
{
	int rc = 0;
	u32 pcs_wr_done;

	MOSYS_CSR_DEBUG("MOSYS CSR WR(BE): addr: 0x%08x offset: 0x%08x data: "
			"0x%08x\n", addr, offset, *pdata);

	out_be32((void *) (addr + MOSYS_OFFSET + X1_PCSCSR_REGADDR_ADDR), offset);
	out_be32((void *) (addr + MOSYS_OFFSET + X1_PCSCSR_REGWRDATA_ADDR), *pdata);
	out_be32((void *) (addr + MOSYS_OFFSET + X1_PCSCSR_REGCMD_ADDR), 
			X1_MGMT_PCS_REG_WR_F2_MASK);
	pcs_wr_done  = 0;
	while(pcs_wr_done != 0x2) {
		pcs_wr_done = in_be32((void *) (addr + MOSYS_OFFSET + 
					X1_PCSCSR_REGCMDDONE_ADDR));
		pcs_wr_done &= X1_MGMT_PCS_REG_WR_DONE_F2_MASK;
	}

	return rc;
}

/* r/w to mosys csr (indirect), takes data directly, not pointer */
int mosys_wr_op(u32 addr, u32 offset, u32 data)
{
	int rc = 0;
	u32 pcs_wr_done;

	MOSYS_CSR_DEBUG("MOSYS CSR WR(BE): addr: 0x%08x offset: 0x%08x "
			"data: 0x%08x\n", addr, offset, data);

	out_be32((void *) (addr + MOSYS_OFFSET + X1_PCSCSR_REGADDR_ADDR), offset);
	out_be32((void *) (addr + MOSYS_OFFSET + X1_PCSCSR_REGWRDATA_ADDR), data);
	out_be32((void *) (addr + MOSYS_OFFSET + X1_PCSCSR_REGCMD_ADDR), 
			X1_MGMT_PCS_REG_WR_F2_MASK);
	pcs_wr_done  = 0;
	while(pcs_wr_done != 0x2) {
		pcs_wr_done = in_be32((void *) (addr + MOSYS_OFFSET + 
					X1_PCSCSR_REGCMDDONE_ADDR));
		pcs_wr_done &= X1_MGMT_PCS_REG_WR_DONE_F2_MASK;
	}

	return rc;
}

int mosys_rd(u32 addr, u32 offset, u32 *pdata)
{
	int rc = 0;
	u32 pcs_rd_done;

	out_be32((void *) (addr + MOSYS_OFFSET + X1_PCSCSR_REGADDR_ADDR), 
			offset);
	out_be32((void *) (addr + MOSYS_OFFSET + X1_PCSCSR_REGCMD_ADDR), 
			X1_MGMT_PCS_REG_RD_F2_MASK);
	pcs_rd_done  = 0;

	while(pcs_rd_done != 0x1) {
		pcs_rd_done = in_be32((void *) 
				(addr + MOSYS_OFFSET + X1_PCSCSR_REGCMDDONE_ADDR));
		pcs_rd_done &= X1_MGMT_PCS_REG_RD_DONE_F2_MASK;
	}

	*pdata = in_be32((void *) (addr + MOSYS_OFFSET + 
				X1_PCSCSR_REGRDDATA_ADDR));
	MOSYS_CSR_DEBUG("MOSYS CSR RD(BE): addr: 0x%08x offset: 0x%08x "
			"data: 0x%08x\n", addr, offset, *pdata);

	return rc;
}

int mosys_wrx4_op(u32 addr, u32 offset, u32 data)
{
	int rc = 0;
	u32 pcs_wr_done;

	MOSYS_CSR_DEBUG("MOSYS CSR WR(BE): addr: 0x%08x offset: 0x%08x "
			"data: 0x%08x\n", addr, offset, data);

	out_be32((void *) (addr + MOSYS_OFFSET + X4_PCSCSR_REGADDR_ADDR), offset);
	out_be32((void *) (addr + MOSYS_OFFSET + X4_PCSCSR_REGWRDATA_ADDR), data);
	out_be32((void *) (addr + MOSYS_OFFSET + X4_PCSCSR_REGCMD_ADDR), 
			X1_MGMT_PCS_REG_WR_F2_MASK);
	pcs_wr_done  = 0;

	while(pcs_wr_done != 0x2) {
		pcs_wr_done = in_be32((void *) (addr + MOSYS_OFFSET + 
					X4_PCSCSR_REGCMDDONE_ADDR));
		pcs_wr_done &= X1_MGMT_PCS_REG_WR_DONE_F2_MASK;
	}

	return rc;
}

int mosys_rdx4(u32 addr, u32 offset, u32 * pdata)
{
	int rc = 0;
	u32 pcs_rd_done;

	out_be32((void *) (addr + MOSYS_OFFSET + X4_PCSCSR_REGADDR_ADDR), offset);
	out_be32((void *) (addr + MOSYS_OFFSET + X4_PCSCSR_REGCMD_ADDR), 
			X1_MGMT_PCS_REG_RD_F2_MASK);
	pcs_rd_done  = 0;

	while(pcs_rd_done != 0x1) {
		pcs_rd_done = in_be32((void *) (addr + MOSYS_OFFSET + 
					X4_PCSCSR_REGCMDDONE_ADDR));
		pcs_rd_done &= X1_MGMT_PCS_REG_RD_DONE_F2_MASK;
	}

	*pdata = in_be32((void *) (addr + MOSYS_OFFSET + X4_PCSCSR_REGRDDATA_ADDR));
	MOSYS_CSR_DEBUG("MOSYS CSR RD(BE): addr: 0x%08x offset: 0x%08x "
			"data: 0x%08x\n", addr, offset, *pdata);

	return rc;
}

