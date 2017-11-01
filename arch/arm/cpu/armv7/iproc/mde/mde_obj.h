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
    *  Filename: mde_obj.h
    *
    *  Function: 
    *
    *  Author:  Chung Kwong (C.K.) Der (ckder@broadcom.com)
    *
 */
#ifndef MDE_OBJ_H
#define MDE_OBJ_H

// memory access support


// prepare struct MDE to be referenced before it is fully defined
struct MDE0;

// prepare MDE_CMD so that it can use the struct MDE before it is fully defined
struct SMDE_CMD {
  /* Properties */
  char *name;
  char *alias;
  char *usage;
  char *details;
  /* Methods */
  unsigned (*handler)(struct MDE0 *);
};

typedef struct SMDE_CMD MDE_CMD;

typedef struct {
  MDE_CMD *cmds;
  int cmd_count;
} MDE_CMD_GROUP;

//#define MEMBUF_SIZE_BYTES 256
#define MEMBUF_SIZE_BYTES 256
#define MEMBUF_SIZE_INTS  (MEMBUF_SIZE_BYTES/4)

typedef struct {
  unsigned address;
  unsigned size_ints;
  unsigned data [MEMBUF_SIZE_INTS];
} MEM_ACCESS_BUF;

typedef struct {
  unsigned id;
  unsigned start;
  unsigned bsize;
  unsigned physical_addr;
} MEM_REGION;

typedef struct {
  unsigned sec;
  unsigned msec;
} MT_TIME;

/// Memory tester

typedef struct {
  unsigned id;
  char     name [BSIZE_64];
  unsigned enabled;       // for locking out the test if necessary
  unsigned done;          // for exit condition check during main test loop
  // memory range - need both cached and uncached address
  // if this is not appropriate - then set both addresses the same
  unsigned start_addr_cached;
  unsigned start_addr_uncached;
  unsigned test_bsize;    // size in bytes
  unsigned test_wsize;    // size in INTS

  unsigned *p_read;
  unsigned seed;
  unsigned uPeriodStartInSec;
  unsigned uPeriodStartInMilliSec;

  // random number generator
  tinymt32_t tiny;
  // error handling support

  unsigned uAddxFail[ADDR_FAIL_CNT];  // failed address array 
  unsigned uAddrFailCnt;
  unsigned uTotalErr;
  unsigned uTotalErrMap;

  unsigned addx;
  unsigned act_data;
  unsigned exp_data;
  // DQ, cyc for this tester
  unsigned result [32][16]; // accomodates 16bit PHY

  // interface stressor info
  // The stressor performs write/read test in the background
  unsigned stressor_present; // 1 = present 0 = no stressor
  unsigned stressor_address; // The address of memory to test
  unsigned stressor_bsize;    // size in bytes
  unsigned stressor_enabled;
  // methods
  unsigned (*stressor_init)  ( struct MDE0 *);
  unsigned (*stressor_start) ( struct MDE0 *);
  unsigned (*stressor_status)( struct MDE0 *);
  unsigned (*stressor_stop)  ( struct MDE0 *);
} MEM_TESTER;

typedef struct {
  unsigned id;
  char name [DIAG_SIZE_64];
  unsigned (*handler) ( struct MDE0 *);
} MT_MODE_HANDLER;


// fill in the details of struct MDE
typedef struct MDE0 {
  // common properties
  unsigned version;
  unsigned date;
  char     name [DIAG_SIZE_64];
  // CLI related support
  char     user_line [DIAG_SIZE_256];
  char     history   [HISTORY_DEPTH][DIAG_SIZE_256];
  unsigned cli_history_depth;
  MDE_CMD_GROUP common;
  MDE_CMD_GROUP local;

  MDE_CMD  *current_command;
  // Memory access support
  MEM_ACCESS_BUF bread;
  MEM_ACCESS_BUF bwrit;

  // Local memory region: LOCAL_MEM_BLOCK_COUNT is defined in mde_local_defs.h
  // The size of MDE is dependent of how many memory blocks are defined on the target chip
  unsigned mem_blks_count; // should be the same as LOCAL_MEM_BLOCK_COUNT
  unsigned mem_total_bytes;
  MEM_REGION mem_blks  [ LOCAL_MEM_BLOCK_COUNT ];


  // memory tester related support
  unsigned memtester_count;
  unsigned memtester_pass;
  unsigned memtester_time_seconds;

  // This is the memory test mode ( 0, 1, 2, 3, 5 )
  unsigned memtester_mode;
  unsigned memtester_loop;
  unsigned memtester_exit;

  MEM_TESTER memtester [ LOCAL_MEM_TESTER_COUNT];

  // test mode related - one handler per mode
  unsigned mt_mode_count;
  unsigned mt_total_mb_tested;
  unsigned mt_total_mb_per_loop;
  unsigned mt_total_read_time_ms;

  // This is the handler per the mode specified
  // This is filled in at runtime
  MT_MODE_HANDLER *mt;

  // random generator related
  unsigned random_seed;

  // time related

  MT_TIME time_start;
  MT_TIME time_now;

  // chip specific properties
  char     chip_name [DIAG_SIZE_64];
  unsigned chip_id;
  unsigned phy_data_width; // 32 or 16

  // 
  unsigned reg_access_base;

  // build specific - DDR frequency is NOT a build option
  // The actual DDR frequency is set by the host at boot up time
  // 
  unsigned ddr_frequency;
  // methods
  // initialization related

  // memory test related 
  // This routine checks and show via printf any local information
  // The output from this routine is shown in the post-loop status report
  void     (*mt_local_status_report)(struct MDE0 *);

  // tester - fill in with a handler according to the mode specified
  unsigned (*memory_test) ( struct MDE0 * );

  // system hardware specific support
  unsigned (*get_chip_temperature) (void);
  unsigned (*get_avs_voltage) (void);
  unsigned (*get_chip_id)(void);

  // find_dq support
  void     (*fdq_init)       (struct MDE0 *, int which);
  int      (*fdq_log_error)  (struct MDE0 *, int which);
  int      (*fdq_show_result)(struct MDE0 *, int which);
} MDE;


typedef MDE sMde_t;

// For accessing bytes within an INT
typedef union {
  unsigned ival;
  unsigned char bval [4];
} ITOB;

#endif // MDE_OBJ_H
