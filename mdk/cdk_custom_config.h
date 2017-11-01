#ifndef _CDK_CUSTOM_CONFIG_H
#define _CDK_CUSTOM_CONFIG_H

#ifndef CDK_CONFIG_INCLUDE_CHIP_DEFAULT
#define CDK_CONFIG_INCLUDE_CHIP_DEFAULT 0
#endif

#ifndef CDK_CONFIG_INCLUDE_CHIP_SYMBOLS
#define CDK_CONFIG_INCLUDE_CHIP_SYMBOLS CFG_MDK_DEBUG
#endif

#ifndef PHY_CONFIG_INCLUDE_CHIP_SYMBOLS
#define PHY_CONFIG_INCLUDE_CHIP_SYMBOLS 0
#endif

#ifndef CDK_CONFIG_INCLUDE_ASSERT
#define CDK_CONFIG_INCLUDE_ASSERT       CFG_MDK_DEBUG
#endif

#ifndef CDK_CONFIG_INCLUDE_DEBUG
#define CDK_CONFIG_INCLUDE_DEBUG        CFG_MDK_DEBUG
#endif

#ifndef CDK_CONFIG_INCLUDE_DYN_CONFIG
#define CDK_CONFIG_INCLUDE_DYN_CONFIG   CFG_MDK_DEBUG
#endif

#ifndef CDK_CONFIG_DEFINE_DMA_ADDR_T
#define CDK_CONFIG_DEFINE_DMA_ADDR_T 0
#endif

#define CDK_CONFIG_DEFINE_UINT8_T       0
#define CDK_CONFIG_DEFINE_UINT16_T      0
#define CDK_CONFIG_DEFINE_UINT32_T      0
#define CDK_CONFIG_DEFINE_SIZE_T        0

#define CDK_ABORT                       abort
#define CDK_PRINTF                      printf
#define CDK_VPRINTF                     vprintf
#define CDK_SPRINTF                     sprintf
#define CDK_VSPRINTF                    vsprintf
/*#define CDK_ATOI                        atoi*/
#define CDK_STRNCHR                     strnchr
#define CDK_STRCPY                      strcpy
#define CDK_STRNCPY                     strncpy
#define CDK_STRLEN                      strlen
#define CDK_STRCHR                      strchr
#define CDK_STRRCHR                     strrchr
#define CDK_STRCMP                      strcmp
#define CDK_MEMCMP                      memcmp
#define CDK_MEMSET                      memset
#define CDK_MEMCPY                      memcpy
#define CDK_STRUPR                      strupr
/*#define CDK_TOUPPER                     toupper*/
#define CDK_STRCAT                      strcat

/* Include external C definitions */
#if 0
#include "lib_types.h"
#include "lib_string.h"
#include "lib_printf.h"
#else
#include <common.h>
#include "../include/linux/string.h"
#include "../include/exports.h"
#endif

extern void abort(void);

#endif /* _CDK_CUSTOM_CONFIG_H */
