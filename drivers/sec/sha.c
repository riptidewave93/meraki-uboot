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
#include <jobdesc.h>
#include <sha.h>
#include <fsl_secboot_err.h>

void sha_done(uint32_t desc, uint32_t status, void *arg)
{
	struct result *x = arg;
	x->status = status;
	x->err = caam_jr_strstatus(x->outstr, status);
	x->done = 1;
}

/* This function initializes the sha-256 context */
void sha_init(struct sha_ctx *ctx, struct jobring *jr)
{
	memset(ctx, 0, sizeof(struct sha_ctx));
	ctx->jr = jr;
}

void sha_update(struct sha_ctx *ctx, u8 *buffer, u32 length)
{
	dma_addr_t addr = virt_to_phys((void *)buffer);
#ifdef CONFIG_PHYS_64BIT
	ctx->sg_tbl[ctx->sg_num].addr_hi = addr >> 32;
#else
	ctx->sg_tbl[ctx->sg_num].addr_hi = 0x0;
#endif
	ctx->sg_tbl[ctx->sg_num].addr_lo = addr;
	ctx->sg_tbl[ctx->sg_num].length = length;
	ctx->sg_num++;
}

int sha_final(struct sha_ctx *ctx)
{
	int ret = 0, i = 0;
	ctx->sg_tbl[ctx->sg_num - 1].final = 1;
	uint32_t len = 0;

	for (i = 0; i < ctx->sg_num; i++)
		len +=  ctx->sg_tbl[i].length;

	inline_cnstr_jobdesc_sha256(ctx->sha_desc,
			   (uint8_t *)ctx->sg_tbl, len, ctx->hash);

	ret = jr_enqueue(ctx->jr, ctx->sha_desc, sha_done, &ctx->op);
	if (ret)
		return ret;

	return ret;
}


/* This  function  completes the calulation of the sha-256 */
int sha_digest(struct sha_ctx *ctx, uint8_t *digest)
{
	unsigned long long timeval = get_ticks();
	unsigned long long timeout = usec2ticks(CONFIG_SEC_DEQ_TIMEOUT);
	int ret = 0;

	while (ctx->op.done != 1) {
		ret = jr_dequeue(ctx->jr);
		if (ret)
			return ret;

		if ((get_ticks() - timeval) > timeout) {
			printf("SEC Dequeue timed out\n");
			return ERROR_ESBC_SEC_DEQ_TO;
		}
	}

	if (ctx->op.err < 0)
		return ctx->op.status;

	memcpy(digest, ctx->hash, sizeof(ctx->hash));

	return 0;
}
