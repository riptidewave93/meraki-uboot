
/*
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef _PPC4xx_ISRAM_H_
#define _PPC4xx_ISRAM_H_

/*
 * Internal SRAM
 */
#if defined(CONFIG_440EPX) || defined(CONFIG_440GRX)
#define ISRAM0_DCR_BASE 0x380
#else
#define ISRAM0_DCR_BASE 0x020
#endif
#define ISRAM0_SB0CR	(ISRAM0_DCR_BASE+0x00)	/* SRAM bank config 0*/
#define ISRAM0_SB1CR	(ISRAM0_DCR_BASE+0x01)	/* SRAM bank config 1*/
#define ISRAM0_SB2CR	(ISRAM0_DCR_BASE+0x02)	/* SRAM bank config 2*/
#define ISRAM0_SB3CR	(ISRAM0_DCR_BASE+0x03)	/* SRAM bank config 3*/
#define ISRAM0_BEAR	(ISRAM0_DCR_BASE+0x04)	/* SRAM bus error addr reg */
#define ISRAM0_BESR0	(ISRAM0_DCR_BASE+0x05)	/* SRAM bus error status reg 0 */
#define ISRAM0_BESR1	(ISRAM0_DCR_BASE+0x06)	/* SRAM bus error status reg 1 */
#define ISRAM0_PMEG	(ISRAM0_DCR_BASE+0x07)	/* SRAM power management */
#define ISRAM0_CID	(ISRAM0_DCR_BASE+0x08)	/* SRAM bus core id reg */
#define ISRAM0_REVID	(ISRAM0_DCR_BASE+0x09)	/* SRAM bus revision id reg */
#define ISRAM0_DPC	(ISRAM0_DCR_BASE+0x0a)	/* SRAM data parity check reg */

#if defined(CONFIG_460EX) || defined(CONFIG_460GT)
#define ISRAM1_DCR_BASE 0x0B0
#define ISRAM1_SB0CR	(ISRAM1_DCR_BASE+0x00)	/* SRAM1 bank config 0*/
#define ISRAM1_BEAR	(ISRAM1_DCR_BASE+0x04)	/* SRAM1 bus error addr reg */
#define ISRAM1_BESR0	(ISRAM1_DCR_BASE+0x05)	/* SRAM1 bus error status reg 0 */
#define ISRAM1_BESR1	(ISRAM1_DCR_BASE+0x06)	/* SRAM1 bus error status reg 1 */
#define ISRAM1_PMEG	(ISRAM1_DCR_BASE+0x07)	/* SRAM1 power management */
#define ISRAM1_CID	(ISRAM1_DCR_BASE+0x08)	/* SRAM1 bus core id reg */
#define ISRAM1_REVID	(ISRAM1_DCR_BASE+0x09)	/* SRAM1 bus revision id reg */
#define ISRAM1_DPC	(ISRAM1_DCR_BASE+0x0a)	/* SRAM1 data parity check reg */
#endif /* CONFIG_460EX || CONFIG_460GT */

/*
 * L2 Cache
 */
#if defined (CONFIG_440GX) || \
    defined(CONFIG_440SP) || defined(CONFIG_440SPE) || \
    defined(CONFIG_460EX) || defined(CONFIG_460GT) || \
    defined(CONFIG_460SX)
#define L2_CACHE_BASE	0x030
#define L2_CACHE_CFG	(L2_CACHE_BASE+0x00)	/* L2 Cache Config      */
#define L2_CACHE_CMD	(L2_CACHE_BASE+0x01)	/* L2 Cache Command     */
#define L2_CACHE_ADDR	(L2_CACHE_BASE+0x02)	/* L2 Cache Address     */
#define L2_CACHE_DATA	(L2_CACHE_BASE+0x03)	/* L2 Cache Data        */
#define L2_CACHE_STAT	(L2_CACHE_BASE+0x04)	/* L2 Cache Status      */
#define L2_CACHE_CVER	(L2_CACHE_BASE+0x05)	/* L2 Cache Revision ID */
#define L2_CACHE_SNP0	(L2_CACHE_BASE+0x06)	/* L2 Cache Snoop reg 0 */
#define L2_CACHE_SNP1	(L2_CACHE_BASE+0x07)	/* L2 Cache Snoop reg 1 */
#endif /* CONFIG_440GX */

#if defined(CONFIG_APM86XXX)
#define L2_CACHE_BASE	0x000
#define L2_CACHE_ADDR	(L2_CACHE_BASE+0x00)	/* L2 Cache Address     */
#define L2_CACHE_DATA	(L2_CACHE_BASE+0x01)	/* L2 Cache Data        */
#define L2_CACHE_L2CR0		0x00	/* L2 Cache Config 0 */
#define L2_CACHE_L2CR1		0x01	/* L2 Cache Config 1 */
#define L2_CACHE_L2CR2		0x02	/* L2 Cache Config 2 */
#define L2_CACHE_L2CR3		0x03	/* L2 Cache Config 3 */
#define L2_CACHE_L2ERAPR 	0x08	/* L2 Extended Real Address Prefix Register */
#define L2_CACHE_L2SLVERAPR 	0x09	/* L2 Slave Extended Real Address Prefix Register */
#define L2_CACHE_L2FAMAR 	0x0A	/* L2 Fixed Address Mode Address Register */
#define L2_CACHE_L2FER0		0x0E	/* L2 Force Error register 0 */
#define L2_CACHE_L2FER1		0x0F	/* L2 Force Error Register 1 */
#define L2_CACHE_L2MCSR		0x10	/* L2 Machine Check Status Register */
#define L2_CACHE_L2MCRER	0x11	/* L2 Machine Check Rpt Enable Register */
#define L2_CACHE_L2DBSR		0x18	/* L2 Debug Status Register */
#define L2_CACHE_L2DBCR		0x19	/* L2 Debug Control Register */
#define L2_CACHE_L2SLVAC0	0x1c	/* L2 Debug Control Register */
#define L2_CACHE_L2SNPAC0	0x1e	/* L2 Debug Control Register */
#define L2_CACHE_L2SNPAC1	0x1f	/* L2 Debug Control Register */
#endif

#endif /* _PPC4xx_ISRAM_H_ */
