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
    *  Broadcom Memory Diagnostics Environment (MDE)
    *********************************************************************
    *  Filename: mde_main.c
    *
    *  Function:
    *
    *  Author:  Chung Kwong (C.K.) Der (ckder@broadcom.com)
    *
 */

#include "mde_common_headers.h"


const char mde_banner [] = \
"\n"
"##############################################\n\n"
"     CDE version %d.%d %08x\n"
"     Built on %s at %s\n\n"
"     Platform: %s:%d\n\n"
"##############################################\n\n"
"Enter ? for help\n"
"Enter 'help <command | alias>' for details\n\n";
/////

static unsigned hdeHandleNewKey (unsigned new, unsigned last, sMde_t *mde, int which, int limit)
{
  if (((last == SPACE) && (new == SPACE)) || (which >= limit)) return NO_ACTION;

  if ((new >= SPACE) && (new <= DEL)) {
    // regular character entered
    mde -> user_line [which] = (char) new;
    host_console_write (new);
    return ADD_ONE;    
  }
  // handle special keys

  switch (new) {
  case ESC:  return NO_ACTION;
  case DEL:
  case BS:
    if (which == 0) return NO_ACTION;
    host_printf ("\b \b");
    return SUB_ONE;    
    break;
  case CR:
    host_printf ("\n");
    mde -> user_line [which] = 0;
    return ADD_ONE;

  default:
    return NO_ACTION;
    break;
  }
}

void get_user_input ( sMde_t *mde)
{
  unsigned new_key;
  int inx, result;
  unsigned val ;

  int count;
  int take;

  char this, last = 0;

  inx = 0;
  while (CONTINUE_UNTIL_DONE) {
	//    while ( mde -> console_rx_ready () == 0) {
	//	}
    new_key = host_console_read ();
    result = hdeHandleNewKey (new_key, last, mde, inx, DIAG_SIZE_256);
    inx += result;
    if ((new_key == CR) || (new_key == LF)) break;
	last = new_key;
  }
  mde -> user_line [inx] = 0; // terminate the raw input line
}

void getuserline ( sMde_t *mde)
{
  host_printf ("CDE-%d >> ", mde->chip_id );
  
  get_user_input ( mde );
}

#define DBG(val)    host_printf ("step %08x\n\n", val)

void mde_entry (unsigned info )
{
  sMde_t mde;
  sMde_t *pThisMde;
  unsigned elapse_ms;
  MT_TIME start;
  MT_TIME done;
  unsigned retval;

  DBG(0x1000);
  setupNewMdeWithCommonInfo ( &mde );

  DBG(0x1001); 
  pThisMde = setupNewMdeWithLocalSpecificInfo ( &mde );
  DBG(0x1002);

  // do local chip specific init
  if ((retval = host_local_init ()) != 0) {
	host_printf ("\nWARNING: host_local_init returned status %08x\n", retval );
  }

  DBG(0x1003);

  // do common mde init
  mdeInit ( &mde );
  DBG(0x1004);
  // for debug and verifying the newly setup mde struct
  mdeShowInfo ( &mde );
  DBG(0x1005);
  // print banner 
  host_printf (mde_banner,  MDE_VER_MAJ, MDE_VER_MIN, MDE_DATE_CODE,  __DATE__, __TIME__,
			  mde.chip_name, mde.chip_id );
  DBG(0x1006);
  // read to go into CLI

  while (1) {
	getuserline (&mde);
	if (mde.user_line [0] == 'q') break;
	mde.current_command = mdeParseCmds ( &mde );
	host_printf ("\n");
	start.msec = host_get_elapsed_msec ();
	mde.current_command -> handler ( &mde );
	if (strchr (mde.user_line, '$') != 0) {
	  done.msec = host_get_elapsed_msec ();
	  elapse_ms = done.msec - start.msec;
	  host_printf ("\n\tLast command took %d ms to complete\n", elapse_ms);
	}
  }
  host_printf ("\nExit from CDE now (HALT)\n");
}
