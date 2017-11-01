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

#include <common.h>
#include <rsa_sec.h>
#include <jobdesc.h>
#include <fsl_secboot_err.h>

void rsa_done(uint32_t desc, uint32_t status, void *arg)
{
	struct result *x = arg;
	x->status = status;
#ifdef DEBUG
	x->err = caam_jr_strstatus(x->outstr, status);
#else
	if (status)
		x->err = -1;
#endif

	x->done = 1;
}

/* This functionw ould return teh status returned by SEC .
 * If non zero , means there was some error reported by SEC */
int rsa_public_verif_sec(unsigned char *sign, uint8_t *to, uint8_t *rsa_pub_key,
			int klen, struct rsa_context *ctx, struct jobring *jr)
{
	unsigned long long timeval;
	unsigned long long timeout;
	int ret = 0;
	memset(ctx, 0, sizeof(struct rsa_context));

	ctx->pkin.a = sign;
	ctx->pkin.a_siz = klen;
	ctx->pkin.n = rsa_pub_key;
	ctx->pkin.n_siz = klen;
	ctx->pkin.e = rsa_pub_key + klen;
	ctx->pkin.e_siz = klen;

	inline_cnstr_jobdesc_pkha_rsaexp(ctx->rsa_desc,
			      &ctx->pkin, to,
			      klen);

	ret = jr_enqueue(jr, ctx->rsa_desc, rsa_done, &ctx->op);
	if (ret)
		return ret;

	timeval = get_ticks();
	timeout = usec2ticks(CONFIG_SEC_DEQ_TIMEOUT);

	while (ctx->op.done != 1) {
		if (jr_dequeue(jr))
			return ERROR_ESBC_SEC_DEQ;

		if ((get_ticks() - timeval) > timeout) {
			printf("SEC Dequeue timed out\n");
			return ERROR_ESBC_SEC_DEQ_TO;
		}
	}

	if (ctx->op.err < 0)
		return ctx->op.status;

	return 0;
}
