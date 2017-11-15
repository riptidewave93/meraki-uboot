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

#if !defined _MAMBA_MOSYS_H_ 
#define _MAMBA_MOSYS_H_

#define MAMBA_SERDES0		0		
#define MAMBA_SERDES1		1		
#define MAMBA_SERDES2		2		
#define MAMBA_SERDES3		3	
#define MAMBA_SERDESMIN		MAMBA_SERDES0
#define MAMBA_SERDESMAX		MAMBA_SERDES3

#define MOSYS_PHY_LB_SET		1	
#define MOSYS_PHY_LB_CLEAR		0	

/* r/w to mosys csr (indirect) */
int mosys_wr(u32 addr, u32 offset, u32 * pdata);
/* r/w to mosys csr (indirect), takes data directly, not pointer */
int mosys_wr_op(u32 addr, u32 offset, u32 data);
int mosys_rd(u32 addr, u32 offset, u32 * pdata);
int mosys_wrx4_op(u32 addr, u32 offset, u32 data);
int mosys_rdx4(u32 addr, u32 offset, u32 * pdata);

#endif
