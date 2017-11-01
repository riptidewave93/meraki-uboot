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
    *  Filename: mde.h
    *
    *  Function: 
    *
    *  Author:  Chung Kwong (C.K.) Der (ckder@broadcom.com)
    *
    * 
    * $Id::                                                       $:
    * $Rev:: : Global SVN Revision = 1950                         $:
    * 
 */
#ifndef MDE_H
#define MDE_H
/*
  Revision History

  Version    Date     Reason

  0.0        20120516 First release
  0.1        20120606 simplified SPACE handling
  0.2        20120622 CDE migration
  0.3        20120703 CDE integrated with SHMOO based on 053012 package from Srini
  0.4        20120822 Delivery to NORTHSTAR
  0.5        20120907 Read-eye function
 */

#define MDE_NAME "BROADCOM Memory Diagnostics Environment (MDE)"
#define MDE_VER_MAJ   00
#define MDE_VER_MIN   05

#define VER_MAJ_POS   16

#define THIS_YEAR   0x2012
#define THIS_MON    0x09
#define THIS_DAY    0x08

#define YEAR_POS    16
#define MON_POS     8
#define DAY_POS     0

#define MDE_VERSION ( MDE_VER_MAJ << VER_MAJ_POS ) | ( MDE_VER_MIN )

#define MDE_DATE_CODE ((THIS_YEAR << YEAR_POS) | (THIS_MON << MON_POS) | (THIS_DAY << DAY_POS))


#endif // MDE_H
