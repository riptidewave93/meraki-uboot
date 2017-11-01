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
    *  Filename: mde_def.h
    *
    *  Function: 
    *
    *  Author:  Chung Kwong (C.K.) Der (ckder@broadcom.com)
    *
 */
#ifndef MDE_DEF_H
#define MDE_DEF_H

#define BSIZE_16  16
#define BSIZE_64  64 
#define BSIZE_128 128 
#define BSIZE_256 256 

//// Test result, return code
#define CDEC_PASS 1
#define CDEC_FAIL 2
#define CDEC_OK   3
#define CDEC_BAD  4

#define TRUE      1
#define FALSE     0
#define NO_ACTION 0

//// Loop control
#define NON_STOP            1
#define CONTINUE_UNTIL_DONE 1
#define ADD_ONE             1
#define SUB_ONE            -1

//// ASCII 
#if PLATFORM != HOST_LINUX
#define NULL  0
#endif

#define BS    8
#define HTAB  9
#define LF    0xa
#define VTAB  0xb
#define CR    0xd
#define ESC   0x1b
#define DEL            0x7e
#define EOS            0

#define SPACE 0x20
#define LAST_PRINTABLE_ASCII 0x7e

#define SIGNATURE 0x72654443
/////
//  Popular sizes
#define DIAG_SIZE_16         16
#define DIAG_SIZE_32         32
#define DIAG_SIZE_64         64
#define DIAG_SIZE_128        128
#define DIAG_SIZE_256        256
#define DIAG_SIZE_512        512
#define DIAG_SIZE_1024       1024
#define DIAG_SIZE_2048       2048

#define DIAG_SIZE_TINY       16
#define DIAG_SIZE_SMALL      64
#define DIAG_SIZE_MEDIUM     256
#define DIAG_SIZE_BIG        1024
#define DIAG_SIZE_HUGE       4096

#define DIAG_CMDBUF_SIZE     128
#define DIAG_SIZE_ERRLOG     256
////
// RETURN values used by CDE
#define DIAG_RET_OK          1
#define DIAG_RET_BAD         2
#define DIAG_TEST_PASSED     3
#define DIAG_TEST_FAILED     4
#define DIAG_ENABLED         5
#define DIAG_DISABLED        6
#define DIAG_NOT_YET         7

#define TEST_NOT_DONE        8
#define TEST_COMPLETED       9

#define DIAG_NO_DICE         -1

// Bit values
#define BIT_0                0x00000001
#define BIT_1                0x00000002
#define BIT_2                0x00000004
#define BIT_3                0x00000008
#define BIT_4                0x00000010
#define BIT_5                0x00000020
#define BIT_6                0x00000040
#define BIT_7                0x00000080
#define BIT_8                0x00000100
#define BIT_9                0x00000200
#define BIT_10               0x00000400
#define BIT_11               0x00000800
#define BIT_12               0x00001000
#define BIT_13               0x00002000
#define BIT_14               0x00004000
#define BIT_15               0x00008000
#define BIT_16               0x00010000
#define BIT_17               0x00020000
#define BIT_18               0x00040000
#define BIT_19               0x00080000
#define BIT_20               0x00100000
#define BIT_21               0x00200000
#define BIT_22               0x00400000
#define BIT_23               0x00800000
#define BIT_24               0x01000000
#define BIT_25               0x02000000
#define BIT_26               0x04000000
#define BIT_27               0x08000000
#define BIT_28               0x10000000
#define BIT_29               0x20000000
#define BIT_30               0x40000000
#define BIT_31               0x80000000
#define ALL_1                0xFFFFFFFF
#define DO_FOREVER           1
#define DO_UNTIL_FOUND       1
#define DO_UNTIL_DONE        1
#define DO_UNTIL_CR          1
#define KEYHIT_OR_TIMEOUT    1

#define STR_EQU              0
#define MAX_TOKEN_SUPPORTED  16

#define ALIGN_32BITS        0xFFFFFFFC
#define ALIGN_16BITS        0xFFFFFFFE

#define INVALID_ADDRESS     ALL_1
#define BYTES_PER_WORD      sizeof(uint32_t)
#define ONE_MB              (1024 * 1024)

#define HISTORY_DEPTH       8

#endif // MDE_DEF_H
