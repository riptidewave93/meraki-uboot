/*
 * Copyright 2012 Freescale Semiconductor, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __JOBDESC_H
#define __JOBDESC_H

#include <common.h>
#include <sha.h>
#include <rsa_sec.h>

void inline_cnstr_jobdesc_pkha_rsaexp(uint32_t *desc,
			      struct pk_in_params *pkin, uint8_t *out,
			      uint32_t out_siz);

void inline_cnstr_jobdesc_sha256(uint32_t *desc,
	uint8_t *msg, uint32_t msgsz, uint8_t *digest);
#endif
