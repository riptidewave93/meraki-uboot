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
    *  Filename: mde_proto.h
    *
    *  Function: 
    *
    *  Author:  Chung Kwong (C.K.) Der (ckder@broadcom.com)
    *
 */
#ifndef MDE_PROTO_H
#define MDE_PROTO_H
/// defined in mde common src files

extern void mde_entry (unsigned info );

extern void verifyNewMde ( sMde_t *mde );
extern void setupNewMdeWithCommonInfo ( sMde_t *mde );

extern MDE_CMD *mdeParseCmds ( sMde_t *mde);
extern MDE_CMD *getMdeCommonCommands ( unsigned *uCommandCount );

extern int dbg_printf(const char *templat,...);
extern void get_user_input ( sMde_t *mde);
extern void mdeScanf ( sMde_t *mde, char *ctl, ...);
extern void mdeInit ( sMde_t *mde );

#if PLATFORM != HOST_LINUX

extern char *strcpy(char *dest,const char *src);
extern char *strncpy(char *dest,const char *src, int cnt);
extern char *strchr(const char *dest,int c);
#endif

/// Common MDE commands

extern unsigned mdeMemRead    ( sMde_t *mde );
extern unsigned mdeMemWrite   ( sMde_t *mde );
extern unsigned mdeShowMemory ( sMde_t *mde );
extern unsigned mdeMemFill    ( sMde_t *mde );
extern unsigned mdeMemVerify  ( sMde_t *mde );

extern unsigned mdeRandomFill   ( sMde_t *mde );
extern unsigned mdeRandomVerify ( sMde_t *mde );

extern unsigned mdeMemSearch  ( sMde_t *mde );
extern unsigned mdeMemCompare ( sMde_t *mde );
extern unsigned mdeMemCmpBufs ( sMde_t *mde );
extern unsigned mdeGetChecksum( sMde_t *mde );

extern unsigned mdeShowInfo   ( sMde_t *mde );

extern unsigned mdeRegRead    ( sMde_t *mde );
extern unsigned mdeRegWrite   ( sMde_t *mde );

extern unsigned mdeGetTime    ( sMde_t *mde );

////

extern unsigned mdeVerifyBlk (unsigned addr, unsigned count, unsigned data, unsigned incr, sMde_t *mde);
extern void     mdeFillBlk (unsigned *addr, int count, unsigned data, unsigned incr, sMde_t *mde);
extern void     mdeShowBlk (sMde_t *mde, int count);
extern unsigned mdeQuery ( sMde_t *mde, char *question, unsigned original);
/// memory test related

extern unsigned mtEditConfig ( sMde_t *mde );
extern unsigned mtShowConfig ( sMde_t *mde );
extern unsigned mtAutoRun    ( sMde_t *mde );
extern unsigned mtRunTest    ( sMde_t *mde );

//// mips_rd_eye_plot needs to be upgraded to use sMde_t
extern unsigned mips_rd_eye_plot ( char *line );
extern unsigned mde_rd_eye_plot ( sMde_t *mde );
extern unsigned mde_test_srdi (sMde_t *mde );
extern unsigned mde_test_mtm (sMde_t *mde );

extern unsigned mde_test_flush (sMde_t *mde );


extern void mde_fdiv (unsigned val, unsigned divr, int place, sMde_t *mde );
extern void find_dq_init_all (struct MDE0 *, int);

extern inline int find_all_errors_all    (struct MDE0 *, int);
extern inline int find_all_errors_all_16 (struct MDE0 *, int);

extern int find_dq_show_result_all    (struct MDE0 *, int);
extern int find_dq_show_result_all_16 (struct MDE0 *, int);

/// defined in mde_<local>.c

//extern sMde_t *setupNewMde ( sMde_t *newMde );
extern sMde_t *setupNewMdeWithLocalSpecificInfo ( sMde_t *newMde );

#endif // MDE_PROTO_H
