/*
 * Copyright (C) 1999-2013, Broadcom Corporation 
 *  
 *      Unless you and Broadcom execute a separate written software license 
 * agreement governing use of this software, this software is licensed to you 
 * under the terms of the GNU General Public License version 2 (the "GPL"), 
 * available at http://www.broadcom.com/licenses/GPLv2.php, with the 
 * following added to such license: 
 *  
 *      As a special exception, the copyright holders of this software give you 
 * permission to link this software with independent modules, and to copy and 
 * distribute the resulting executable under terms of your choice, provided that 
 * you also meet, for each linked independent module, the terms and conditions of 
 * the license of that module.  An independent module is a module which is not 
 * derived from this software.  The special exception does not apply to any 
 * modifications of the software. 
 *  
 *      Notwithstanding the above, under no circumstances may you combine this 
 * software in any way with any other Broadcom software provided under a license 
 * other than the GPL, without Broadcom's express prior written consent. 
 */
/*  *********************************************************************
    *********************************************************************
    *  Broadcom Memoroy Diagnostics Environment (MDE)
    *********************************************************************
    *  Filename: mde_common_headers.h
    *
    *  Function: declare all MDE headers
	*            This is included in all MDE C files
    *
    *  Author:  Chung Kwong (C.K.) Der (ckder@broadcom.com)
    *
 */
#ifndef MDE_COMMON_HEADERS_H
#define MDE_COMMON_HEADERS_H

//// Include local header files

#include "mde_local_headers.h"

#include "tinymt32.h"

#include "mde_types.h"
#include "mde.h"
#include "mde_def.h"
#include "mde_mt.h"
#include "mde_obj.h"

#include "mde_proto.h"
#include "mde_plat.h"

// access for PHY registers
// These two header files provide 0 based offset

#include "mde_phy_word_lane.h"
#include "mde_phy_ctrl_regs.h"
#include "mde_shim_addr_cntl.h"

#include "mde_rd_eye.h"

#endif // MDE_COMMON_HEADERS_H
