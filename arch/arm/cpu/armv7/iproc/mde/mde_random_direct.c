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
    *  Filename: mde_random.c
    *
    *  Function:
    *
    *  Author:  Chung Kwong (C.K.) Der (ckder@broadcom.com)
    *
 */

//// Include non-broadcom headers

#include <tinymt32.h>

#include <mde_common_headers.h>

#include <tinymt32.c>


///// random test support
unsigned mdeRandomFill ( sMde_t *mde )
{
  // Usage: rf <address><count><seed>

  char discard [DIAG_SIZE_SMALL];
  unsigned address, count_bytes;
  unsigned count_ints;
  unsigned *pfill;
  unsigned seed;
  tinymt32_t tinymt;

  int binx; /// block index
  
  mdeScanf (mde, "%s%x%x%x", discard, &address, &count_bytes, &seed);

  address &= ALIGN_32BITS;
  count_ints = count_bytes / 4;

  pfill = (unsigned*) address;

  if ( seed == 0 ) {
	seed = mde -> get_local_random_seed ( mde );
  } 

  host_printf ("Filling memory at %08x with random data (size %08x INTS) seed %08x\n",
				 pfill, count_ints, seed);

  // initialize tiny
  tinymt.mat1 = 0;
  tinymt.mat2 = 0;
  tinymt.tmat = 0;

  tinymt32_init(&tinymt, seed);


  for ( binx = 0; binx < count_ints; binx ++ ) {
	*pfill++ = tinymt32_generate_uint32(&tinymt);
  }

  host_printf ("Done fill with random data (seed %08x)\n", seed);

  return DIAG_RET_OK;
}

unsigned mdeRandomVerify ( sMde_t *mde )
{
#define ERROR_LIMIT 16

  // Usage: mv <address><count><data><incrementer>
  char discard [DIAG_SIZE_SMALL];
  unsigned address, count_bytes;
  unsigned count_ints;
  unsigned retval;
  unsigned actual_read;
  unsigned expected;
  unsigned *pread;
  unsigned seed;
  unsigned error = 0;
  int binx; /// block index
  tinymt32_t tinymt;
  
  mdeScanf (mde, "%s%x%x%x", discard, &address, &count_bytes, &seed);
  address &= ALIGN_32BITS;
  pread = (unsigned*)address;

  count_ints = count_bytes/4;
  if ( seed == 0 ) {
	seed = mde -> get_local_random_seed ( mde );
  } 

  host_printf ("Verifying memory at %08x with random data (size %08x INTS) seed %08x\n",
				 pread, count_ints, seed );

  // initialize tiny
  tinymt.mat1 = 0;
  tinymt.mat2 = 0;
  tinymt.tmat = 0;

  tinymt32_init(&tinymt, seed);

  for ( binx = 0; binx < count_ints; binx ++, pread ++ ) {
	expected = tinymt32_generate_uint32(&tinymt);
	actual_read = *pread;
	if ( actual_read == expected) contine;
	host_printf (" ** Failed @address %08x: expected %08x read %08x XOR %08x\n",
				 pread , expected , actual_read, (expected ^ actual_read));
	if (error ++ > ERROR_LIMIT) {
	  host_printf ("\nToo many errors (%d) abort now\n", error );
	  return DIAG_RET_BAD;
	}
  }
  return DIAG_RET_OK;
}
