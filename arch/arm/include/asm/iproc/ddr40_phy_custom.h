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
/*
** Copyright 2000, 2001  Broadcom Corporation
** All Rights Reserved
**
** No portions of this material may be reproduced in any form 
** without the written permission of:
**
**   Broadcom Corporation
**   16215 Alton Parkway
**   P.O. Box 57013
**   Irvine, CA  92619-7013
**
** All information contained in this document is Broadcom 
** Corporation company private proprietary, and trade secret.
**
** ----------------------------------------------------------
**
**
/*  $Id:: ddr40_phy_custom.h 1306 2012-06-21 14:10:10Z jeung      $:
/*  $Rev::file =  : Global SVN Revision = 1780                    $:
**
*/

#ifndef __DDR40_PHY_CUSTOM_H__

#define __DDR40_PHY_CUSTOM_H__  

// #ifdef MIPS_CODE

#ifndef DDR40_TYPES__
#define DDR40_TYPES__
typedef unsigned long   ddr40_addr_t;
typedef u32             ddr40_uint32_t;
#endif

  #define  DDR40_PHY_RegWr(addr,value)   tb_w(addr,value)
  #define  DDR40_PHY_RegRd(addr)         tb_r(addr)

  #define  DDR40_PHY_Print(args...)      print_log(args)
  #define  DDR40_PHY_Error(args...)      error_log(args)
  #define  DDR40_PHY_Fatal(args...)      fatal_log(args)

  #define  DDR40_PHY_Timeout(args...)      timeout_ns(args)

  #define FUNC_PROTOTYPE_PREFIX inline
  #define FUNC_PROTOTYPE_SUFFIX
  #define FUNC_PREFIX inline
  #define FUNC_SUFFIX


#endif


/*
**
** $Log: $
**
*/
