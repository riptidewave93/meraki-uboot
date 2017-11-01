/*
 * Portions Copyrighted by Freescale Semiconductor, Inc., 2010-2012
 */

#ifndef __FSL_SECBOOT_TYPES_H_
#define __FSL_SECBOOT_TYPES_H_

#include <linux/types.h>

#define WORD_SIZE 4

/* Minimum and maximum size of RSA signature length in bits */
#define KEY_SIZE       4096
#define KEY_SIZE_BYTES (KEY_SIZE/8)
#define KEY_SIZE_WORDS (KEY_SIZE_BYTES/(WORD_SIZE))

#endif
