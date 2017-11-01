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
/*  $Id::                                                         $:*/
/*  $Rev:: : Global SVN Revision = 1950                           $:*/
/* ========================================================================== */
/*                                                                            */
/*   cde_plat.h                                                               */
/*   (c) 2012 Broadcom Corporation                                                         */
/*                                                                            */
/*                                                              */
/*                                                                            */
/* ========================================================================== */
#ifndef MDE_PLAT_H
#define MDE_PLAT_H

#include "tinymt32.h"

#define PLAT_MEMC_MAX_NUM_DIS 2
//albert: change fro  2 to 1
#define PLAT_MAX_NUM_MEMC 1

// use by cde_amt to set up auto-memory-test size of block to test
#define TMT_DEFAULT_SIZE_WORDS   0x04000000

#define DQ_OVERRIDE_OFF 0xFFFFFFFF

#define BITFLIP_LOOP_LIM  20
#define DEFAULT_ADDRCHUNK_SIZE  64

#define CHUNK_END_POS  27

typedef enum
{
   DIS_DISABLE = 0,         // DIS is disabled
   DIS_LFSR,                // pattern = 0 (LFSR)
   DIS_VSSO,                // pattern = 1 (Victim SSO)
   DIS_AVSSO,               // pattern = 2 (Anti-Victim SSO)
   
} DisPattern_e;

typedef struct OverrideInfo
{
   uint32_t uDqOverride;    // bit x = 1 means DQx is overridden with corresponding 
                            // bit in uDqOverrideVal
                            // 0xFFFFFFFF has special meaning (don't apply mask)
   uint32_t uDqOverrideVal; // see uDqOverride
   
   // Fields for pointers to internal private data used for override processing
   unsigned int * pMaskNonShuff; 
   unsigned int * pMaskShuff;  
   unsigned int * pMaskNonShuffOverrideValue; 
   unsigned int * pMaskShuffOverrideValue; 
    
}  OverrideInfo_t;

typedef struct MemcDisInfo
{
   uint32_t       uStartAddx; // 32bit aligned Start Addx
   uint32_t       uEndAddx;   // one Byte AFTER last dis test memory area address
   DisPattern_e   eDisPattern;// DIS pattern
   
}  MemcDisInfo_t;

typedef struct MemcInfo
{
   int32_t        iMode; // test run modality. 0=normal read only test: write once, read many
                         //  1=write-read test, 2=bitflip, 3=write-read with cached address
                         //  4=bitflip in random chunk, 5=write-read in random chunk
   uint32_t       uStartAddxCached;   // 32bit aligned Start Addx in cached space 
   uint32_t       uStartAddxUncached; // 32bit aligned Start Addx in uncached space
   uint32_t       uSizeLongWords;     // size in long words (32b)
   uint32_t       uSeed;              // random number seed.
   uint32_t       uTestLengthInSec;   // test length in seconds 0= use RunLoopCount
   uint32_t       uRunLoopCount;      // run loop count 0=use uTestLengthInS
   uint32_t       uIsOverride;        // 0 = don't override any DQ bits
                                      // 1 = use sOverride to define Override.
   OverrideInfo_t sOverride;          // Override behavior specification
  
   MemcDisInfo_t  sMemDisInfo[PLAT_MEMC_MAX_NUM_DIS]; // Dis test specification
   tinymt32_t     sTiny;              // TMT data structure
  
}  MemcInfo_t;

typedef struct AutoTestInfo
{
   uint32_t uChip;            // 7425, 7346, etc
   char  *  ChipRev;          // B1, B0, etc
   
   uint32_t   uNumMemc;
   MemcInfo_t sMemInfo[PLAT_MAX_NUM_MEMC];
  
   uint32_t uVerbose;         // debug verbosity level. 0=none.
   uint32_t uDoubleRefresh;   // double refresh. 0=do not set
   uint32_t uBitflipLoopCnt;  // number of loops for bitflip test
   uint32_t uChunkSizeLongWords;  // chunk size in long words (32b)

} AutoTestInfo_t;


#define ADDR_FAIL_CNT 64

#else
#endif
