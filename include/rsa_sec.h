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

#ifndef __RSA_SEC_H
#define __RSA_SEC_H

#include <common.h>
#include <jr.h>

struct pk_in_params {
	uint8_t *e;
	uint32_t e_siz;
	uint8_t *n;
	uint32_t n_siz;
	uint8_t *a;
	uint32_t a_siz;
	uint8_t *b;
	uint32_t b_siz;
};

struct rsa_context {
	struct pk_in_params pkin;
	uint32_t rsa_desc[64];
	struct result op;
};

int rsa_public_verif_sec(unsigned char *sign, uint8_t *to, uint8_t *rsa_pub_key,
			int klen, struct rsa_context *ctx, struct jobring *jr);

#endif
