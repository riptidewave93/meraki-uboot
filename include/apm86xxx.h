/**
 * AppliedMicro SoC 86xxx Header File
 *
 * Copyright (c) 2010 Applied Micro Circuits Corporation.
 * All rights reserved.
 * Feng Kan <fkan@apm.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#ifndef __APM86XXX_H_
#define __APM86XXX_H_

/* Maximum PPC cores in the APM86xxx SoC */
#define CONFIG_MAX_CORES		2
#define CORE0_ID			0 	/**< PPC Core 0 ID */
#define CORE1_ID			1 	/**< PPC Core 1 ID */

#ifndef __ASSEMBLY__

void l1_init(void);
void l2_init(void);
void config_plb(void);
void enable_ip_config_reg(void);

#endif /* __ASSEMBLY__ */

#endif /* __ASM_APM_SCU_H_ */ 
