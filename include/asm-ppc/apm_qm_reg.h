/*
 * APM 86xxx QM Header File
 *
 * Copyright (c) 2011, Applied Micro Circuits Corporation
 * Author: Loc Ho <lho@apm.com>
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
 *
 */
#ifndef  _MB_QM_REG_H__ 
#define  _MB_QM_REG_H__ 

/*	Global Base Address	*/
#define QM_GLBL_DIAG_CSR_BASE_ADDR				0xddd807000ULL

/*    Address GLBL_DIAG_CSR  Registers */
#define QM_CFG_DIAG_SEL_ADDR                                         0x00000000
#define QM_CFG_DIAG_SEL_DEFAULT                                      0x00000000
#define QM_CFG_READ_BW_LAT_ADDR_MASK_ADDR                            0x00000004
#define QM_CFG_READ_BW_LAT_ADDR_MASK_DEFAULT                         0x00000000
#define QM_CFG_READ_BW_LAT_ADDR_PAT_ADDR                             0x00000008
#define QM_CFG_READ_BW_LAT_ADDR_PAT_DEFAULT                          0xffffffff
#define QM_CFG_WRITE_BW_LAT_ADDR_MASK_ADDR                           0x0000000c
#define QM_CFG_WRITE_BW_LAT_ADDR_MASK_DEFAULT                        0x00000000
#define QM_CFG_WRITE_BW_LAT_ADDR_PAT_ADDR                            0x00000010
#define QM_CFG_WRITE_BW_LAT_ADDR_PAT_DEFAULT                         0xffffffff
#define QM_CFG_DIAG_START_STOP_ADDR                                  0x00000014
#define QM_CFG_DIAG_START_STOP_DEFAULT                               0x0000001f
#define QM_CFG_BW_MSTR_STOP_CNT_ADDR                                 0x00000018
#define QM_CFG_BW_MSTR_STOP_CNT_DEFAULT                              0x00040004
#define QM_CFG_BW_SLV_STOP_CNT_ADDR                                  0x0000001c
#define QM_CFG_BW_SLV_STOP_CNT_DEFAULT                               0x00040004
#define QM_STS_READ_LATENCY_OUTPUT_ADDR                              0x00000020
#define QM_STS_READ_LATENCY_OUTPUT_DEFAULT                           0x00000000
#define QM_STS_AXI_MRD_BW_CLK_CNT_ADDR                               0x00000024
#define QM_STS_AXI_MRD_BW_CLK_CNT_DEFAULT                            0x00000000
#define QM_STS_AXI_MRD_BW_BYTE_CNT_ADDR                              0x00000028
#define QM_STS_AXI_MRD_BW_BYTE_CNT_DEFAULT                           0x00000000
#define QM_STS_AXI_MWR_BW_CLK_CNT_ADDR                               0x0000002c
#define QM_STS_AXI_MWR_BW_CLK_CNT_DEFAULT                            0x00000000
#define QM_STS_AXI_MWR_BW_BYTE_CNT_ADDR                              0x00000030
#define QM_STS_AXI_MWR_BW_BYTE_CNT_DEFAULT                           0x00000000
#define QM_STS_AXI_SRD_BW_CLK_CNT_ADDR                               0x00000034
#define QM_STS_AXI_SRD_BW_CLK_CNT_DEFAULT                            0x00000000
#define QM_STS_AXI_SRD_BW_BYTE_CNT_ADDR                              0x00000038
#define QM_STS_AXI_SRD_BW_BYTE_CNT_DEFAULT                           0x00000000
#define QM_STS_AXI_SWR_BW_CLK_CNT_ADDR                               0x0000003c
#define QM_STS_AXI_SWR_BW_CLK_CNT_DEFAULT                            0x00000000
#define QM_STS_AXI_SWR_BW_BYTE_CNT_ADDR                              0x00000040
#define QM_STS_AXI_SWR_BW_BYTE_CNT_DEFAULT                           0x00000000
#define QM_CFG_DBG_TRIG_CTRL_ADDR                                    0x00000044
#define QM_CFG_DBG_TRIG_CTRL_DEFAULT                                 0x00000000
#define QM_CFG_DBG_PAT_REG_0_ADDR                                    0x00000048
#define QM_CFG_DBG_PAT_REG_0_DEFAULT                                 0x00000000
#define QM_CFG_DBG_PAT_MASK_REG_0_ADDR                               0x0000004c
#define QM_CFG_DBG_PAT_MASK_REG_0_DEFAULT                            0x00000000
#define QM_CFG_DBG_PAT_REG_1_ADDR                                    0x00000050
#define QM_CFG_DBG_PAT_REG_1_DEFAULT                                 0x00000000
#define QM_CFG_DBG_PAT_MASK_REG_1_ADDR                               0x00000054
#define QM_CFG_DBG_PAT_MASK_REG_1_DEFAULT                            0x00000000
#define QM_DBG_TRIG_OUT_ADDR                                         0x00000058
#define QM_DBG_TRIG_OUT_DEFAULT                                      0x00000000
#define QM_DBG_ECC_INT_OUT_ADDR                                      0x0000005c
#define QM_DBG_ECC_INT_OUT_DEFAULT                                   0x00000000
#define QM_DBG_ECC_INT_OUTMASK_ADDR                                  0x00000060
#define QM_CFG_SECC_INT_THRSH_REG_ADDR                               0x00000064
#define QM_CFG_SECC_INT_THRSH_REG_DEFAULT                            0x000000fe
#define QM_CFG_MEM_ECC_BYPASS_ADDR                                   0x00000068
#define QM_CFG_MEM_ECC_BYPASS_DEFAULT                                0x00000000
#define QM_CFG_MEM_PWRDN_DIS_ADDR                                    0x0000006c
#define QM_CFG_MEM_PWRDN_DIS_DEFAULT                                 0x00000000
#define QM_CFG_MEM_RAM_SHUTDOWN_ADDR                                 0x00000070
#define QM_CFG_MEM_RAM_SHUTDOWN_DEFAULT                              0xffffffff
#define QM_BLOCK_MEM_RDY_ADDR                                        0x00000074
#define QM_BLOCK_MEM_RDY_DEFAULT                                     0xffffffff
#define QM_CFG_AXI_SLV_RD_TMO_ADDR                                   0x00000078
#define QM_CFG_AXI_SLV_RD_TMO_DEFAULT                                0x83ff83ff
#define QM_CFG_AXI_SLV_WR_TMO_ADDR                                   0x0000007c
#define QM_CFG_AXI_SLV_WR_TMO_DEFAULT                                0x83ff83ff
#define QM_STS_AXI_SLV_TMO_ADDR                                      0x00000080
#define QM_STS_AXI_SLV_TMO_DEFAULT                                   0x00000000
#define QM_CFG_AXI_SS_CSR_TMO_ADDR                                   0x00000084
#define QM_CFG_AXI_SS_CSR_TMO_DEFAULT                                0x02008000
#define QM_STS_READ_LATENCY_TOT_READ_REQS_ADDR                       0x0000008c
#define QM_STS_READ_LATENCY_TOT_READ_REQS_DEFAULT                    0x00000000

/*	Register CFG_DIAG_SEL	*/ 
/*	 Fields CFG_AXI_NON_AXI_MUX_SELECT	 */
#define QM_CFG_AXI_NON_AXI_MUX_SELECT_WIDTH                                   1
#define QM_CFG_AXI_NON_AXI_MUX_SELECT_SHIFT                                  11
#define QM_CFG_AXI_NON_AXI_MUX_SELECT_MASK                           0x00000800
#define QM_CFG_AXI_NON_AXI_MUX_SELECT_RD(src)        (((src) & 0x00000800)>>11)
#define QM_CFG_AXI_NON_AXI_MUX_SELECT_WR(src)   (((u32)(src)<<11) & 0x00000800)
#define QM_CFG_AXI_NON_AXI_MUX_SELECT_SET(dst,src) \
                      (((dst) & ~0x00000800) | (((u32)(src)<<11) & 0x00000800))
/*	 Fields CFG_MUX_SELECTOR	 */
#define QM_CFG_MUX_SELECTOR_WIDTH                                            11
#define QM_CFG_MUX_SELECTOR_SHIFT                                             0
#define QM_CFG_MUX_SELECTOR_MASK                                     0x000007ff
#define QM_CFG_MUX_SELECTOR_RD(src)                      (((src) & 0x000007ff))
#define QM_CFG_MUX_SELECTOR_WR(src)                 (((u32)(src)) & 0x000007ff)
#define QM_CFG_MUX_SELECTOR_SET(dst,src) \
                          (((dst) & ~0x000007ff) | (((u32)(src)) & 0x000007ff))

/*	Register CFG_READ_BW_LAT_ADDR_MASK	*/ 
/*	 Fields READ_ADDR_MASK	 */
#define QM_READ_ADDR_MASK_WIDTH                                              32
#define QM_READ_ADDR_MASK_SHIFT                                               0
#define QM_READ_ADDR_MASK_MASK                                       0xffffffff
#define QM_READ_ADDR_MASK_RD(src)                        (((src) & 0xffffffff))
#define QM_READ_ADDR_MASK_WR(src)                   (((u32)(src)) & 0xffffffff)
#define QM_READ_ADDR_MASK_SET(dst,src) \
                          (((dst) & ~0xffffffff) | (((u32)(src)) & 0xffffffff))

/*	Register CFG_READ_BW_LAT_ADDR_PAT	*/ 
/*	 Fields READ_ADDR_PAT	 */
#define QM_READ_ADDR_PAT_WIDTH                                               32
#define QM_READ_ADDR_PAT_SHIFT                                                0
#define QM_READ_ADDR_PAT_MASK                                        0xffffffff
#define QM_READ_ADDR_PAT_RD(src)                         (((src) & 0xffffffff))
#define QM_READ_ADDR_PAT_WR(src)                    (((u32)(src)) & 0xffffffff)
#define QM_READ_ADDR_PAT_SET(dst,src) \
                          (((dst) & ~0xffffffff) | (((u32)(src)) & 0xffffffff))

/*	Register CFG_WRITE_BW_LAT_ADDR_MASK	*/ 
/*	 Fields WRITE_ADDR_MASK	 */
#define QM_WRITE_ADDR_MASK_WIDTH                                             32
#define QM_WRITE_ADDR_MASK_SHIFT                                              0
#define QM_WRITE_ADDR_MASK_MASK                                      0xffffffff
#define QM_WRITE_ADDR_MASK_RD(src)                       (((src) & 0xffffffff))
#define QM_WRITE_ADDR_MASK_WR(src)                  (((u32)(src)) & 0xffffffff)
#define QM_WRITE_ADDR_MASK_SET(dst,src) \
                          (((dst) & ~0xffffffff) | (((u32)(src)) & 0xffffffff))

/*	Register CFG_WRITE_BW_LAT_ADDR_PAT	*/ 
/*	 Fields WRITE_ADDR_PAT	 */
#define QM_WRITE_ADDR_PAT_WIDTH                                              32
#define QM_WRITE_ADDR_PAT_SHIFT                                               0
#define QM_WRITE_ADDR_PAT_MASK                                       0xffffffff
#define QM_WRITE_ADDR_PAT_RD(src)                        (((src) & 0xffffffff))
#define QM_WRITE_ADDR_PAT_WR(src)                   (((u32)(src)) & 0xffffffff)
#define QM_WRITE_ADDR_PAT_SET(dst,src) \
                          (((dst) & ~0xffffffff) | (((u32)(src)) & 0xffffffff))

/*	Register CFG_DIAG_START_STOP	*/ 
/*	 Fields START_RD_LATENCY	 */
#define QM_START_RD_LATENCY_WIDTH                                             1
#define QM_START_RD_LATENCY_SHIFT                                             4
#define QM_START_RD_LATENCY_MASK                                     0x00000010
#define QM_START_RD_LATENCY_RD(src)                   (((src) & 0x00000010)>>4)
#define QM_START_RD_LATENCY_WR(src)              (((u32)(src)<<4) & 0x00000010)
#define QM_START_RD_LATENCY_SET(dst,src) \
                       (((dst) & ~0x00000010) | (((u32)(src)<<4) & 0x00000010))
/*	 Fields START_SRD_BW	 */
#define QM_START_SRD_BW_WIDTH                                                 1
#define QM_START_SRD_BW_SHIFT                                                 3
#define QM_START_SRD_BW_MASK                                         0x00000008
#define QM_START_SRD_BW_RD(src)                       (((src) & 0x00000008)>>3)
#define QM_START_SRD_BW_WR(src)                  (((u32)(src)<<3) & 0x00000008)
#define QM_START_SRD_BW_SET(dst,src) \
                       (((dst) & ~0x00000008) | (((u32)(src)<<3) & 0x00000008))
/*	 Fields START_MRD_BW	 */
#define QM_START_MRD_BW_WIDTH                                                 1
#define QM_START_MRD_BW_SHIFT                                                 2
#define QM_START_MRD_BW_MASK                                         0x00000004
#define QM_START_MRD_BW_RD(src)                       (((src) & 0x00000004)>>2)
#define QM_START_MRD_BW_WR(src)                  (((u32)(src)<<2) & 0x00000004)
#define QM_START_MRD_BW_SET(dst,src) \
                       (((dst) & ~0x00000004) | (((u32)(src)<<2) & 0x00000004))
/*	 Fields START_SWR_BW	 */
#define QM_START_SWR_BW_WIDTH                                                 1
#define QM_START_SWR_BW_SHIFT                                                 1
#define QM_START_SWR_BW_MASK                                         0x00000002
#define QM_START_SWR_BW_RD(src)                       (((src) & 0x00000002)>>1)
#define QM_START_SWR_BW_WR(src)                  (((u32)(src)<<1) & 0x00000002)
#define QM_START_SWR_BW_SET(dst,src) \
                       (((dst) & ~0x00000002) | (((u32)(src)<<1) & 0x00000002))
/*	 Fields START_MWR_BW	 */
#define QM_START_MWR_BW_WIDTH                                                 1
#define QM_START_MWR_BW_SHIFT                                                 0
#define QM_START_MWR_BW_MASK                                         0x00000001
#define QM_START_MWR_BW_RD(src)                          (((src) & 0x00000001))
#define QM_START_MWR_BW_WR(src)                     (((u32)(src)) & 0x00000001)
#define QM_START_MWR_BW_SET(dst,src) \
                          (((dst) & ~0x00000001) | (((u32)(src)) & 0x00000001))

/*	Register CFG_BW_MSTR_STOP_CNT	*/ 
/*	 Fields MSTR_STOP_RD_CNT	 */
#define QM_MSTR_STOP_RD_CNT_WIDTH                                            16
#define QM_MSTR_STOP_RD_CNT_SHIFT                                            16
#define QM_MSTR_STOP_RD_CNT_MASK                                     0xffff0000
#define QM_MSTR_STOP_RD_CNT_RD(src)                  (((src) & 0xffff0000)>>16)
#define QM_MSTR_STOP_RD_CNT_WR(src)             (((u32)(src)<<16) & 0xffff0000)
#define QM_MSTR_STOP_RD_CNT_SET(dst,src) \
                      (((dst) & ~0xffff0000) | (((u32)(src)<<16) & 0xffff0000))
/*	 Fields MSTR_STOP_WR_CNT	 */
#define QM_MSTR_STOP_WR_CNT_WIDTH                                            16
#define QM_MSTR_STOP_WR_CNT_SHIFT                                             0
#define QM_MSTR_STOP_WR_CNT_MASK                                     0x0000ffff
#define QM_MSTR_STOP_WR_CNT_RD(src)                      (((src) & 0x0000ffff))
#define QM_MSTR_STOP_WR_CNT_WR(src)                 (((u32)(src)) & 0x0000ffff)
#define QM_MSTR_STOP_WR_CNT_SET(dst,src) \
                          (((dst) & ~0x0000ffff) | (((u32)(src)) & 0x0000ffff))

/*	Register CFG_BW_SLV_STOP_CNT	*/ 
/*	 Fields SLV_STOP_RD_CNT	 */
#define QM_SLV_STOP_RD_CNT_WIDTH                                             16
#define QM_SLV_STOP_RD_CNT_SHIFT                                             16
#define QM_SLV_STOP_RD_CNT_MASK                                      0xffff0000
#define QM_SLV_STOP_RD_CNT_RD(src)                   (((src) & 0xffff0000)>>16)
#define QM_SLV_STOP_RD_CNT_WR(src)              (((u32)(src)<<16) & 0xffff0000)
#define QM_SLV_STOP_RD_CNT_SET(dst,src) \
                      (((dst) & ~0xffff0000) | (((u32)(src)<<16) & 0xffff0000))
/*	 Fields SLV_STOP_WR_CNT	 */
#define QM_SLV_STOP_WR_CNT_WIDTH                                             16
#define QM_SLV_STOP_WR_CNT_SHIFT                                              0
#define QM_SLV_STOP_WR_CNT_MASK                                      0x0000ffff
#define QM_SLV_STOP_WR_CNT_RD(src)                       (((src) & 0x0000ffff))
#define QM_SLV_STOP_WR_CNT_WR(src)                  (((u32)(src)) & 0x0000ffff)
#define QM_SLV_STOP_WR_CNT_SET(dst,src) \
                          (((dst) & ~0x0000ffff) | (((u32)(src)) & 0x0000ffff))

/*	Register STS_READ_LATENCY_OUTPUT	*/ 
/*	 Fields READ_MAX	 */
#define QM_READ_MAX_WIDTH                                                    10
#define QM_READ_MAX_SHIFT                                                    22
#define QM_READ_MAX_MASK                                             0xffc00000
#define QM_READ_MAX_RD(src)                          (((src) & 0xffc00000)>>22)
#define QM_READ_MAX_SET(dst,src) \
                      (((dst) & ~0xffc00000) | (((u32)(src)<<22) & 0xffc00000))
/*	 Fields READ_TOT	 */
#define QM_READ_TOT_WIDTH                                                    22
#define QM_READ_TOT_SHIFT                                                     0
#define QM_READ_TOT_MASK                                             0x003fffff
#define QM_READ_TOT_RD(src)                              (((src) & 0x003fffff))
#define QM_READ_TOT_SET(dst,src) \
                          (((dst) & ~0x003fffff) | (((u32)(src)) & 0x003fffff))

/*	Register STS_AXI_MRD_BW_CLK_CNT	*/ 
/*	 Fields MSTR_READ_BW_CLK_CNT	 */
#define QM_MSTR_READ_BW_CLK_CNT_WIDTH                                        32
#define QM_MSTR_READ_BW_CLK_CNT_SHIFT                                         0
#define QM_MSTR_READ_BW_CLK_CNT_MASK                                 0xffffffff
#define QM_MSTR_READ_BW_CLK_CNT_RD(src)                  (((src) & 0xffffffff))
#define QM_MSTR_READ_BW_CLK_CNT_SET(dst,src) \
                          (((dst) & ~0xffffffff) | (((u32)(src)) & 0xffffffff))

/*	Register STS_AXI_MRD_BW_BYTE_CNT	*/ 
/*	 Fields MSTR_READ_BW_BYTE_CNT	 */
#define QM_MSTR_READ_BW_BYTE_CNT_WIDTH                                       32
#define QM_MSTR_READ_BW_BYTE_CNT_SHIFT                                        0
#define QM_MSTR_READ_BW_BYTE_CNT_MASK                                0xffffffff
#define QM_MSTR_READ_BW_BYTE_CNT_RD(src)                 (((src) & 0xffffffff))
#define QM_MSTR_READ_BW_BYTE_CNT_SET(dst,src) \
                          (((dst) & ~0xffffffff) | (((u32)(src)) & 0xffffffff))

/*	Register STS_AXI_MWR_BW_CLK_CNT	*/ 
/*	 Fields MSTR_WRITE_BW_CLK_CNT	 */
#define QM_MSTR_WRITE_BW_CLK_CNT_WIDTH                                       32
#define QM_MSTR_WRITE_BW_CLK_CNT_SHIFT                                        0
#define QM_MSTR_WRITE_BW_CLK_CNT_MASK                                0xffffffff
#define QM_MSTR_WRITE_BW_CLK_CNT_RD(src)                 (((src) & 0xffffffff))
#define QM_MSTR_WRITE_BW_CLK_CNT_SET(dst,src) \
                          (((dst) & ~0xffffffff) | (((u32)(src)) & 0xffffffff))

/*	Register STS_AXI_MWR_BW_BYTE_CNT	*/ 
/*	 Fields MSTR_WRITE_BW_BYTE_CNT	 */
#define QM_MSTR_WRITE_BW_BYTE_CNT_WIDTH                                      32
#define QM_MSTR_WRITE_BW_BYTE_CNT_SHIFT                                       0
#define QM_MSTR_WRITE_BW_BYTE_CNT_MASK                               0xffffffff
#define QM_MSTR_WRITE_BW_BYTE_CNT_RD(src)                (((src) & 0xffffffff))
#define QM_MSTR_WRITE_BW_BYTE_CNT_SET(dst,src) \
                          (((dst) & ~0xffffffff) | (((u32)(src)) & 0xffffffff))

/*	Register STS_AXI_SRD_BW_CLK_CNT	*/ 
/*	 Fields SLV_READ_BW_CLK_CNT	 */
#define QM_SLV_READ_BW_CLK_CNT_WIDTH                                         32
#define QM_SLV_READ_BW_CLK_CNT_SHIFT                                          0
#define QM_SLV_READ_BW_CLK_CNT_MASK                                  0xffffffff
#define QM_SLV_READ_BW_CLK_CNT_RD(src)                   (((src) & 0xffffffff))
#define QM_SLV_READ_BW_CLK_CNT_SET(dst,src) \
                          (((dst) & ~0xffffffff) | (((u32)(src)) & 0xffffffff))

/*	Register STS_AXI_SRD_BW_BYTE_CNT	*/ 
/*	 Fields SLV_READ_BW_BYTE_CNT	 */
#define QM_SLV_READ_BW_BYTE_CNT_WIDTH                                        32
#define QM_SLV_READ_BW_BYTE_CNT_SHIFT                                         0
#define QM_SLV_READ_BW_BYTE_CNT_MASK                                 0xffffffff
#define QM_SLV_READ_BW_BYTE_CNT_RD(src)                  (((src) & 0xffffffff))
#define QM_SLV_READ_BW_BYTE_CNT_SET(dst,src) \
                          (((dst) & ~0xffffffff) | (((u32)(src)) & 0xffffffff))

/*	Register STS_AXI_SWR_BW_CLK_CNT	*/ 
/*	 Fields SLV_WRITE_BW_CLK_CNT	 */
#define QM_SLV_WRITE_BW_CLK_CNT_WIDTH                                        32
#define QM_SLV_WRITE_BW_CLK_CNT_SHIFT                                         0
#define QM_SLV_WRITE_BW_CLK_CNT_MASK                                 0xffffffff
#define QM_SLV_WRITE_BW_CLK_CNT_RD(src)                  (((src) & 0xffffffff))
#define QM_SLV_WRITE_BW_CLK_CNT_SET(dst,src) \
                          (((dst) & ~0xffffffff) | (((u32)(src)) & 0xffffffff))

/*	Register STS_AXI_SWR_BW_BYTE_CNT	*/ 
/*	 Fields SLV_WRITE_BW_BYTE_CNT	 */
#define QM_SLV_WRITE_BW_BYTE_CNT_WIDTH                                       32
#define QM_SLV_WRITE_BW_BYTE_CNT_SHIFT                                        0
#define QM_SLV_WRITE_BW_BYTE_CNT_MASK                                0xffffffff
#define QM_SLV_WRITE_BW_BYTE_CNT_RD(src)                 (((src) & 0xffffffff))
#define QM_SLV_WRITE_BW_BYTE_CNT_SET(dst,src) \
                          (((dst) & ~0xffffffff) | (((u32)(src)) & 0xffffffff))

/*	Register CFG_DBG_TRIG_CTRL	*/ 
/*	 Fields TRIG_EN	 */
#define QM_TRIG_EN_WIDTH                                                      1
#define QM_TRIG_EN_SHIFT                                                      4
#define QM_TRIG_EN_MASK                                              0x00000010
#define QM_TRIG_EN_RD(src)                            (((src) & 0x00000010)>>4)
#define QM_TRIG_EN_WR(src)                       (((u32)(src)<<4) & 0x00000010)
#define QM_TRIG_EN_SET(dst,src) \
                       (((dst) & ~0x00000010) | (((u32)(src)<<4) & 0x00000010))
/*	 Fields AND_E	 */
#define QM_AND_E_WIDTH                                                        2
#define QM_AND_E_SHIFT                                                        2
#define QM_AND_E_MASK                                                0x0000000c
#define QM_AND_E_RD(src)                              (((src) & 0x0000000c)>>2)
#define QM_AND_E_WR(src)                         (((u32)(src)<<2) & 0x0000000c)
#define QM_AND_E_SET(dst,src) \
                       (((dst) & ~0x0000000c) | (((u32)(src)<<2) & 0x0000000c))
/*	 Fields OR_E	 */
#define QM_OR_E_WIDTH                                                         2
#define QM_OR_E_SHIFT                                                         0
#define QM_OR_E_MASK                                                 0x00000003
#define QM_OR_E_RD(src)                                  (((src) & 0x00000003))
#define QM_OR_E_WR(src)                             (((u32)(src)) & 0x00000003)
#define QM_OR_E_SET(dst,src) \
                          (((dst) & ~0x00000003) | (((u32)(src)) & 0x00000003))

/*	Register CFG_DBG_PAT_REG_0	*/ 
/*	 Fields PATTERN	 */
#define QM_PATTERN0_WIDTH                                                    32
#define QM_PATTERN0_SHIFT                                                     0
#define QM_PATTERN0_MASK                                             0xffffffff
#define QM_PATTERN0_RD(src)                              (((src) & 0xffffffff))
#define QM_PATTERN0_WR(src)                         (((u32)(src)) & 0xffffffff)
#define QM_PATTERN0_SET(dst,src) \
                          (((dst) & ~0xffffffff) | (((u32)(src)) & 0xffffffff))

/*	Register CFG_DBG_PAT_MASK_REG_0	*/ 
/*	 Fields PAT_MASK	 */
#define QM_PAT_MASK0_WIDTH                                                   32
#define QM_PAT_MASK0_SHIFT                                                    0
#define QM_PAT_MASK0_MASK                                            0xffffffff
#define QM_PAT_MASK0_RD(src)                             (((src) & 0xffffffff))
#define QM_PAT_MASK0_WR(src)                        (((u32)(src)) & 0xffffffff)
#define QM_PAT_MASK0_SET(dst,src) \
                          (((dst) & ~0xffffffff) | (((u32)(src)) & 0xffffffff))

/*	Register CFG_DBG_PAT_REG_1	*/ 
/*	 Fields PATTERN	 */
#define QM_PATTERN1_WIDTH                                                    32
#define QM_PATTERN1_SHIFT                                                     0
#define QM_PATTERN1_MASK                                             0xffffffff
#define QM_PATTERN1_RD(src)                              (((src) & 0xffffffff))
#define QM_PATTERN1_WR(src)                         (((u32)(src)) & 0xffffffff)
#define QM_PATTERN1_SET(dst,src) \
                          (((dst) & ~0xffffffff) | (((u32)(src)) & 0xffffffff))

/*	Register CFG_DBG_PAT_MASK_REG_1	*/ 
/*	 Fields PAT_MASK	 */
#define QM_PAT_MASK1_WIDTH                                                   32
#define QM_PAT_MASK1_SHIFT                                                    0
#define QM_PAT_MASK1_MASK                                            0xffffffff
#define QM_PAT_MASK1_RD(src)                             (((src) & 0xffffffff))
#define QM_PAT_MASK1_WR(src)                        (((u32)(src)) & 0xffffffff)
#define QM_PAT_MASK1_SET(dst,src) \
                          (((dst) & ~0xffffffff) | (((u32)(src)) & 0xffffffff))

/*	Register DBG_TRIG_OUT	*/ 
/*	 Fields DBG_OUT	 */
#define QM_DBG_OUT_WIDTH                                                     32
#define QM_DBG_OUT_SHIFT                                                      0
#define QM_DBG_OUT_MASK                                              0xffffffff
#define QM_DBG_OUT_RD(src)                               (((src) & 0xffffffff))
#define QM_DBG_OUT_SET(dst,src) \
                          (((dst) & ~0xffffffff) | (((u32)(src)) & 0xffffffff))

/*	Register DBG_ECC_INT_OUT	*/ 
/*	 Fields SLV_TMO	 */
#define QM_SLV_TMO_WIDTH                                                      1
#define QM_SLV_TMO_SHIFT                                                      3
#define QM_SLV_TMO_MASK                                              0x00000008
#define QM_SLV_TMO_RD(src)                            (((src) & 0x00000008)>>3)
#define QM_SLV_TMO_WR(src)                       (((u32)(src)<<3) & 0x00000008)
#define QM_SLV_TMO_SET(dst,src) \
                       (((dst) & ~0x00000008) | (((u32)(src)<<3) & 0x00000008))
/*	 Fields SECC_INT	 */
#define QM_SECC_INT_WIDTH                                                     1
#define QM_SECC_INT_SHIFT                                                     2
#define QM_SECC_INT_MASK                                             0x00000004
#define QM_SECC_INT_RD(src)                           (((src) & 0x00000004)>>2)
#define QM_SECC_INT_WR(src)                      (((u32)(src)<<2) & 0x00000004)
#define QM_SECC_INT_SET(dst,src) \
                       (((dst) & ~0x00000004) | (((u32)(src)<<2) & 0x00000004))
/*	 Fields DECC_INT	 */
#define QM_DECC_INT_WIDTH                                                     1
#define QM_DECC_INT_SHIFT                                                     1
#define QM_DECC_INT_MASK                                             0x00000002
#define QM_DECC_INT_RD(src)                           (((src) & 0x00000002)>>1)
#define QM_DECC_INT_WR(src)                      (((u32)(src)<<1) & 0x00000002)
#define QM_DECC_INT_SET(dst,src) \
                       (((dst) & ~0x00000002) | (((u32)(src)<<1) & 0x00000002))
/*	 Fields DBG_INT	 */
#define QM_DBG_INT_WIDTH                                                      1
#define QM_DBG_INT_SHIFT                                                      0
#define QM_DBG_INT_MASK                                              0x00000001
#define QM_DBG_INT_RD(src)                               (((src) & 0x00000001))
#define QM_DBG_INT_WR(src)                          (((u32)(src)) & 0x00000001)
#define QM_DBG_INT_SET(dst,src) \
                          (((dst) & ~0x00000001) | (((u32)(src)) & 0x00000001))

/*	Register DBG_ECC_INT_OUTMask	*/
/*    Mask Register Fields SLV_TMOMask    */
#define QM_SLV_TMOMASK_WIDTH                                                  1
#define QM_SLV_TMOMASK_SHIFT                                                  3
#define QM_SLV_TMOMASK_MASK                                          0x00000008
#define QM_SLV_TMOMASK_RD(src)                        (((src) & 0x00000008)>>3)
#define QM_SLV_TMOMASK_WR(src)                   (((u32)(src)<<3) & 0x00000008)
#define QM_SLV_TMOMASK_SET(dst,src) \
                       (((dst) & ~0x00000008) | (((u32)(src)<<3) & 0x00000008))
/*    Mask Register Fields SECC_INTMask    */
#define QM_SECC_INTMASK_WIDTH                                                 1
#define QM_SECC_INTMASK_SHIFT                                                 2
#define QM_SECC_INTMASK_MASK                                         0x00000004
#define QM_SECC_INTMASK_RD(src)                       (((src) & 0x00000004)>>2)
#define QM_SECC_INTMASK_WR(src)                  (((u32)(src)<<2) & 0x00000004)
#define QM_SECC_INTMASK_SET(dst,src) \
                       (((dst) & ~0x00000004) | (((u32)(src)<<2) & 0x00000004))
/*    Mask Register Fields DECC_INTMask    */
#define QM_DECC_INTMASK_WIDTH                                                 1
#define QM_DECC_INTMASK_SHIFT                                                 1
#define QM_DECC_INTMASK_MASK                                         0x00000002
#define QM_DECC_INTMASK_RD(src)                       (((src) & 0x00000002)>>1)
#define QM_DECC_INTMASK_WR(src)                  (((u32)(src)<<1) & 0x00000002)
#define QM_DECC_INTMASK_SET(dst,src) \
                       (((dst) & ~0x00000002) | (((u32)(src)<<1) & 0x00000002))
/*    Mask Register Fields DBG_INTMask    */
#define QM_DBG_INTMASK_WIDTH                                                  1
#define QM_DBG_INTMASK_SHIFT                                                  0
#define QM_DBG_INTMASK_MASK                                          0x00000001
#define QM_DBG_INTMASK_RD(src)                           (((src) & 0x00000001))
#define QM_DBG_INTMASK_WR(src)                      (((u32)(src)) & 0x00000001)
#define QM_DBG_INTMASK_SET(dst,src) \
                          (((dst) & ~0x00000001) | (((u32)(src)) & 0x00000001))

/*	Register CFG_SECC_INT_THRSH_REG	*/ 
/*	 Fields SECC_INT_THRSH	 */
#define QM_SECC_INT_THRSH_WIDTH                                               8
#define QM_SECC_INT_THRSH_SHIFT                                               0
#define QM_SECC_INT_THRSH_MASK                                       0x000000ff
#define QM_SECC_INT_THRSH_RD(src)                        (((src) & 0x000000ff))
#define QM_SECC_INT_THRSH_WR(src)                   (((u32)(src)) & 0x000000ff)
#define QM_SECC_INT_THRSH_SET(dst,src) \
                          (((dst) & ~0x000000ff) | (((u32)(src)) & 0x000000ff))

/*	Register CFG_MEM_ECC_BYPASS	*/ 
/*	 Fields CFG_ECC_BYPASS	 */
#define QM_CFG_ECC_BYPASS_WIDTH                                              16
#define QM_CFG_ECC_BYPASS_SHIFT                                               0
#define QM_CFG_ECC_BYPASS_MASK                                       0x0000ffff
#define QM_CFG_ECC_BYPASS_RD(src)                        (((src) & 0x0000ffff))
#define QM_CFG_ECC_BYPASS_WR(src)                   (((u32)(src)) & 0x0000ffff)
#define QM_CFG_ECC_BYPASS_SET(dst,src) \
                          (((dst) & ~0x0000ffff) | (((u32)(src)) & 0x0000ffff))

/*	Register CFG_MEM_PWRDN_DIS	*/ 
/*	 Fields CFG_ECC_PWRDN_DIS	 */
#define QM_CFG_ECC_PWRDN_DIS_WIDTH                                           16
#define QM_CFG_ECC_PWRDN_DIS_SHIFT                                           16
#define QM_CFG_ECC_PWRDN_DIS_MASK                                    0xffff0000
#define QM_CFG_ECC_PWRDN_DIS_RD(src)                 (((src) & 0xffff0000)>>16)
#define QM_CFG_ECC_PWRDN_DIS_WR(src)            (((u32)(src)<<16) & 0xffff0000)
#define QM_CFG_ECC_PWRDN_DIS_SET(dst,src) \
                      (((dst) & ~0xffff0000) | (((u32)(src)<<16) & 0xffff0000))
/*	 Fields CFG_PWRDN_DIS	 */
#define QM_CFG_PWRDN_DIS_WIDTH                                               16
#define QM_CFG_PWRDN_DIS_SHIFT                                                0
#define QM_CFG_PWRDN_DIS_MASK                                        0x0000ffff
#define QM_CFG_PWRDN_DIS_RD(src)                         (((src) & 0x0000ffff))
#define QM_CFG_PWRDN_DIS_WR(src)                    (((u32)(src)) & 0x0000ffff)
#define QM_CFG_PWRDN_DIS_SET(dst,src) \
                          (((dst) & ~0x0000ffff) | (((u32)(src)) & 0x0000ffff))

/*	Register CFG_MEM_RAM_SHUTDOWN	*/ 
/*	 Fields CFG_RAM_SHUTDOWN_EN	 */
#define QM_CFG_RAM_SHUTDOWN_EN_WIDTH                                         32
#define QM_CFG_RAM_SHUTDOWN_EN_SHIFT                                          0
#define QM_CFG_RAM_SHUTDOWN_EN_MASK                                  0xffffffff
#define QM_CFG_RAM_SHUTDOWN_EN_RD(src)                   (((src) & 0xffffffff))
#define QM_CFG_RAM_SHUTDOWN_EN_WR(src)              (((u32)(src)) & 0xffffffff)
#define QM_CFG_RAM_SHUTDOWN_EN_SET(dst,src) \
                          (((dst) & ~0xffffffff) | (((u32)(src)) & 0xffffffff))

/*	Register BLOCK_MEM_RDY	*/ 
/*	 Fields MEM_RDY	 */
#define QM_MEM_RDY_WIDTH                                                     32
#define QM_MEM_RDY_SHIFT                                                      0
#define QM_MEM_RDY_MASK                                              0xffffffff
#define QM_MEM_RDY_RD(src)                               (((src) & 0xffffffff))
#define QM_MEM_RDY_SET(dst,src) \
                          (((dst) & ~0xffffffff) | (((u32)(src)) & 0xffffffff))

/*	Register CFG_AXI_SLV_RD_TMO	*/ 
/*	 Fields SLV_ID_TMO_ENABLE	 */
#define QM_SLV_ID_TMO_ENABLE_WIDTH                                            1
#define QM_SLV_ID_TMO_ENABLE_SHIFT                                           31
#define QM_SLV_ID_TMO_ENABLE_MASK                                    0x80000000
#define QM_SLV_ID_TMO_ENABLE_RD(src)                 (((src) & 0x80000000)>>31)
#define QM_SLV_ID_TMO_ENABLE_WR(src)            (((u32)(src)<<31) & 0x80000000)
#define QM_SLV_ID_TMO_ENABLE_SET(dst,src) \
                      (((dst) & ~0x80000000) | (((u32)(src)<<31) & 0x80000000))
/*	 Fields SS_ID_TRK_ENABLE	 */
#define QM_SS_ID_TRK_ENABLE_WIDTH                                             1
#define QM_SS_ID_TRK_ENABLE_SHIFT                                            30
#define QM_SS_ID_TRK_ENABLE_MASK                                     0x40000000
#define QM_SS_ID_TRK_ENABLE_RD(src)                  (((src) & 0x40000000)>>30)
#define QM_SS_ID_TRK_ENABLE_WR(src)             (((u32)(src)<<30) & 0x40000000)
#define QM_SS_ID_TRK_ENABLE_SET(dst,src) \
                      (((dst) & ~0x40000000) | (((u32)(src)<<30) & 0x40000000))
/*	 Fields SLV_ID_TMO_VALUE	 */
#define QM_SLV_ID_TMO_VALUE_WIDTH                                            10
#define QM_SLV_ID_TMO_VALUE_SHIFT                                            16
#define QM_SLV_ID_TMO_VALUE_MASK                                     0x03ff0000
#define QM_SLV_ID_TMO_VALUE_RD(src)                  (((src) & 0x03ff0000)>>16)
#define QM_SLV_ID_TMO_VALUE_WR(src)             (((u32)(src)<<16) & 0x03ff0000)
#define QM_SLV_ID_TMO_VALUE_SET(dst,src) \
                      (((dst) & ~0x03ff0000) | (((u32)(src)<<16) & 0x03ff0000))
/*	 Fields SLV_RDY_TMO_ENABLE	 */
#define QM_SLV_RDY_TMO_ENABLE_WIDTH                                           1
#define QM_SLV_RDY_TMO_ENABLE_SHIFT                                          15
#define QM_SLV_RDY_TMO_ENABLE_MASK                                   0x00008000
#define QM_SLV_RDY_TMO_ENABLE_RD(src)                (((src) & 0x00008000)>>15)
#define QM_SLV_RDY_TMO_ENABLE_WR(src)           (((u32)(src)<<15) & 0x00008000)
#define QM_SLV_RDY_TMO_ENABLE_SET(dst,src) \
                      (((dst) & ~0x00008000) | (((u32)(src)<<15) & 0x00008000))
/*	 Fields SLV_RDY_TMO_VALUE	 */
#define QM_SLV_RDY_TMO_VALUE_WIDTH                                           10
#define QM_SLV_RDY_TMO_VALUE_SHIFT                                            0
#define QM_SLV_RDY_TMO_VALUE_MASK                                    0x000003ff
#define QM_SLV_RDY_TMO_VALUE_RD(src)                     (((src) & 0x000003ff))
#define QM_SLV_RDY_TMO_VALUE_WR(src)                (((u32)(src)) & 0x000003ff)
#define QM_SLV_RDY_TMO_VALUE_SET(dst,src) \
                          (((dst) & ~0x000003ff) | (((u32)(src)) & 0x000003ff))

/*	Register CFG_AXI_SLV_WR_TMO	*/ 
/*	 Fields SLV_ID_TMO_ENABLE	 */
#define QM_SLV_ID_TMO_ENABLE_F1_WIDTH                                         1
#define QM_SLV_ID_TMO_ENABLE_F1_SHIFT                                        31
#define QM_SLV_ID_TMO_ENABLE_F1_MASK                                 0x80000000
#define QM_SLV_ID_TMO_ENABLE_F1_RD(src)              (((src) & 0x80000000)>>31)
#define QM_SLV_ID_TMO_ENABLE_F1_WR(src)         (((u32)(src)<<31) & 0x80000000)
#define QM_SLV_ID_TMO_ENABLE_F1_SET(dst,src) \
                      (((dst) & ~0x80000000) | (((u32)(src)<<31) & 0x80000000))
/*	 Fields SS_ID_TRK_ENABLE	 */
#define QM_SS_ID_TRK_ENABLE_F1_WIDTH                                          1
#define QM_SS_ID_TRK_ENABLE_F1_SHIFT                                         30
#define QM_SS_ID_TRK_ENABLE_F1_MASK                                  0x40000000
#define QM_SS_ID_TRK_ENABLE_F1_RD(src)               (((src) & 0x40000000)>>30)
#define QM_SS_ID_TRK_ENABLE_F1_WR(src)          (((u32)(src)<<30) & 0x40000000)
#define QM_SS_ID_TRK_ENABLE_F1_SET(dst,src) \
                      (((dst) & ~0x40000000) | (((u32)(src)<<30) & 0x40000000))
/*	 Fields SLV_ID_TMO_VALUE	 */
#define QM_SLV_ID_TMO_VALUE_F1_WIDTH                                         10
#define QM_SLV_ID_TMO_VALUE_F1_SHIFT                                         16
#define QM_SLV_ID_TMO_VALUE_F1_MASK                                  0x03ff0000
#define QM_SLV_ID_TMO_VALUE_F1_RD(src)               (((src) & 0x03ff0000)>>16)
#define QM_SLV_ID_TMO_VALUE_F1_WR(src)          (((u32)(src)<<16) & 0x03ff0000)
#define QM_SLV_ID_TMO_VALUE_F1_SET(dst,src) \
                      (((dst) & ~0x03ff0000) | (((u32)(src)<<16) & 0x03ff0000))
/*	 Fields SLV_RDY_TMO_ENABLE	 */
#define QM_SLV_RDY_TMO_ENABLE_F1_WIDTH                                        1
#define QM_SLV_RDY_TMO_ENABLE_F1_SHIFT                                       15
#define QM_SLV_RDY_TMO_ENABLE_F1_MASK                                0x00008000
#define QM_SLV_RDY_TMO_ENABLE_F1_RD(src)             (((src) & 0x00008000)>>15)
#define QM_SLV_RDY_TMO_ENABLE_F1_WR(src)        (((u32)(src)<<15) & 0x00008000)
#define QM_SLV_RDY_TMO_ENABLE_F1_SET(dst,src) \
                      (((dst) & ~0x00008000) | (((u32)(src)<<15) & 0x00008000))
/*	 Fields SLV_RDY_TMO_VALUE	 */
#define QM_SLV_RDY_TMO_VALUE_F1_WIDTH                                        10
#define QM_SLV_RDY_TMO_VALUE_F1_SHIFT                                         0
#define QM_SLV_RDY_TMO_VALUE_F1_MASK                                 0x000003ff
#define QM_SLV_RDY_TMO_VALUE_F1_RD(src)                  (((src) & 0x000003ff))
#define QM_SLV_RDY_TMO_VALUE_F1_WR(src)             (((u32)(src)) & 0x000003ff)
#define QM_SLV_RDY_TMO_VALUE_F1_SET(dst,src) \
                          (((dst) & ~0x000003ff) | (((u32)(src)) & 0x000003ff))

/*	Register STS_AXI_SLV_TMO	*/ 
/*	 Fields SLV_ARID_TMO	 */
#define QM_SLV_ARID_TMO_WIDTH                                                 1
#define QM_SLV_ARID_TMO_SHIFT                                                 3
#define QM_SLV_ARID_TMO_MASK                                         0x00000008
#define QM_SLV_ARID_TMO_RD(src)                       (((src) & 0x00000008)>>3)
#define QM_SLV_ARID_TMO_SET(dst,src) \
                       (((dst) & ~0x00000008) | (((u32)(src)<<3) & 0x00000008))
/*	 Fields SLV_ARRDY_TMO	 */
#define QM_SLV_ARRDY_TMO_WIDTH                                                1
#define QM_SLV_ARRDY_TMO_SHIFT                                                2
#define QM_SLV_ARRDY_TMO_MASK                                        0x00000004
#define QM_SLV_ARRDY_TMO_RD(src)                      (((src) & 0x00000004)>>2)
#define QM_SLV_ARRDY_TMO_SET(dst,src) \
                       (((dst) & ~0x00000004) | (((u32)(src)<<2) & 0x00000004))
/*	 Fields SLV_AWID_TMO	 */
#define QM_SLV_AWID_TMO_WIDTH                                                 1
#define QM_SLV_AWID_TMO_SHIFT                                                 1
#define QM_SLV_AWID_TMO_MASK                                         0x00000002
#define QM_SLV_AWID_TMO_RD(src)                       (((src) & 0x00000002)>>1)
#define QM_SLV_AWID_TMO_SET(dst,src) \
                       (((dst) & ~0x00000002) | (((u32)(src)<<1) & 0x00000002))
/*	 Fields SLV_AWRDY_TMO	 */
#define QM_SLV_AWRDY_TMO_WIDTH                                                1
#define QM_SLV_AWRDY_TMO_SHIFT                                                0
#define QM_SLV_AWRDY_TMO_MASK                                        0x00000001
#define QM_SLV_AWRDY_TMO_RD(src)                         (((src) & 0x00000001))
#define QM_SLV_AWRDY_TMO_SET(dst,src) \
                          (((dst) & ~0x00000001) | (((u32)(src)) & 0x00000001))

/*	Register CFG_AXI_SS_CSR_TMO	*/ 
/*	 Fields CFG_CSR_TMO	 */
#define QM_CFG_CSR_TMO_WIDTH                                                 16
#define QM_CFG_CSR_TMO_SHIFT                                                 16
#define QM_CFG_CSR_TMO_MASK                                          0xffff0000
#define QM_CFG_CSR_TMO_RD(src)                       (((src) & 0xffff0000)>>16)
#define QM_CFG_CSR_TMO_WR(src)                  (((u32)(src)<<16) & 0xffff0000)
#define QM_CFG_CSR_TMO_SET(dst,src) \
                      (((dst) & ~0xffff0000) | (((u32)(src)<<16) & 0xffff0000))
/*	 Fields CSR_ERR_RESP_EN	 */
#define QM_CSR_ERR_RESP_EN_WIDTH                                              1
#define QM_CSR_ERR_RESP_EN_SHIFT                                             15
#define QM_CSR_ERR_RESP_EN_MASK                                      0x00008000
#define QM_CSR_ERR_RESP_EN_RD(src)                   (((src) & 0x00008000)>>15)
#define QM_CSR_ERR_RESP_EN_WR(src)              (((u32)(src)<<15) & 0x00008000)
#define QM_CSR_ERR_RESP_EN_SET(dst,src) \
                      (((dst) & ~0x00008000) | (((u32)(src)<<15) & 0x00008000))

/*	Register STS_READ_LATENCY_TOT_READ_REQS	*/ 
/*	 Fields TOT_READS	 */
#define QM_TOT_READS_WIDTH                                                   16
#define QM_TOT_READS_SHIFT                                                    0
#define QM_TOT_READS_MASK                                            0x0000ffff
#define QM_TOT_READS_RD(src)                             (((src) & 0x0000ffff))
#define QM_TOT_READS_SET(dst,src) \
                          (((dst) & ~0x0000ffff) | (((u32)(src)) & 0x0000ffff))

/*	Global Base Address	*/
#define QM_CSR_BASE_ADDR			0xddd800000ULL

/*    Address QM_CSR  Registers */
#define CSR_IPBRR_ADDR                                               0x00000000
#define CSR_IPBRR_DEFAULT                                            0x00000070
#define CSR_QM_CONFIG_ADDR                                           0x00000004
#define CSR_QM_CONFIG_DEFAULT                                        0x00000000
#define CSR_PBM_ADDR                                                 0x00000008
#define CSR_PBM_DEFAULT                                              0x00000000
#define CSR_PBM_BUF_WR_ADDR                                          0x0000000c
#define CSR_PBM_BUF_WR_DEFAULT                                       0x00000000
#define CSR_PBM_BUF_RD_ADDR                                          0x00000010
#define CSR_PBM_BUF_RD_DEFAULT                                       0x00000000
#define CSR_PBM_COAL_ADDR                                            0x00000014
#define CSR_PBM_COAL_DEFAULT                                         0x00000109
#define CSR_PBM_CTICK0_ADDR                                          0x00000018
#define CSR_PBM_CTICK0_DEFAULT                                       0x00000000
#define CSR_PBM_CTICK1_ADDR                                          0x0000001c
#define CSR_PBM_CTICK1_DEFAULT                                       0x00000000
#define CSR_THRESHOLD0_SET0_ADDR                                     0x00000020
#define CSR_THRESHOLD0_SET0_DEFAULT                                  0x00000000
#define CSR_THRESHOLD1_SET0_ADDR                                     0x00000024
#define CSR_THRESHOLD1_SET0_DEFAULT                                  0x00000000
#define CSR_THRESHOLD0_SET1_ADDR                                     0x00000028
#define CSR_THRESHOLD0_SET1_DEFAULT                                  0x00000000
#define CSR_THRESHOLD1_SET1_ADDR                                     0x0000002c
#define CSR_THRESHOLD1_SET1_DEFAULT                                  0x00000000
#define CSR_THRESHOLD0_SET2_ADDR                                     0x00000030
#define CSR_THRESHOLD0_SET2_DEFAULT                                  0x00000000
#define CSR_THRESHOLD1_SET2_ADDR                                     0x00000034
#define CSR_THRESHOLD1_SET2_DEFAULT                                  0x00000000
#define CSR_THRESHOLD0_SET3_ADDR                                     0x00000038
#define CSR_THRESHOLD0_SET3_DEFAULT                                  0x00000000
#define CSR_THRESHOLD1_SET3_ADDR                                     0x0000003c
#define CSR_THRESHOLD1_SET3_DEFAULT                                  0x00000000
#define CSR_THRESHOLD0_SET4_ADDR                                     0x00000040
#define CSR_THRESHOLD0_SET4_DEFAULT                                  0x00000000
#define CSR_THRESHOLD1_SET4_ADDR                                     0x00000044
#define CSR_THRESHOLD1_SET4_DEFAULT                                  0x00000000
#define CSR_THRESHOLD0_SET5_ADDR                                     0x00000048
#define CSR_THRESHOLD0_SET5_DEFAULT                                  0x00000000
#define CSR_THRESHOLD1_SET5_ADDR                                     0x0000004c
#define CSR_THRESHOLD1_SET5_DEFAULT                                  0x00000000
#define CSR_THRESHOLD0_SET6_ADDR                                     0x00000050
#define CSR_THRESHOLD0_SET6_DEFAULT                                  0x00000000
#define CSR_THRESHOLD1_SET6_ADDR                                     0x00000054
#define CSR_THRESHOLD1_SET6_DEFAULT                                  0x00000000
#define CSR_THRESHOLD0_SET7_ADDR                                     0x00000058
#define CSR_THRESHOLD0_SET7_DEFAULT                                  0x00000000
#define CSR_THRESHOLD1_SET7_ADDR                                     0x0000005c
#define CSR_THRESHOLD1_SET7_DEFAULT                                  0x00000000
#define CSR_HYSTERESIS_ADDR                                          0x00000060
#define CSR_HYSTERESIS_DEFAULT                                       0x00000000
#define CSR_QSTATE_ADDR                                              0x00000064
#define CSR_QSTATE_DEFAULT                                           0x00000000
#define CSR_QSTATE_WR_0_ADDR                                         0x00000068
#define CSR_QSTATE_WR_0_DEFAULT                                      0x00000000
#define CSR_QSTATE_WR_1_ADDR                                         0x0000006c
#define CSR_QSTATE_WR_1_DEFAULT                                      0x00000000
#define CSR_QSTATE_WR_2_ADDR                                         0x00000070
#define CSR_QSTATE_WR_2_DEFAULT                                      0x00000000
#define CSR_QSTATE_WR_3_ADDR                                         0x00000074
#define CSR_QSTATE_WR_3_DEFAULT                                      0x00000000
#define CSR_QSTATE_RD_0_ADDR                                         0x00000078
#define CSR_QSTATE_RD_0_DEFAULT                                      0x00000000
#define CSR_QSTATE_RD_1_ADDR                                         0x0000007c
#define CSR_QSTATE_RD_1_DEFAULT                                      0x00000000
#define CSR_QSTATE_RD_2_ADDR                                         0x00000080
#define CSR_QSTATE_RD_2_DEFAULT                                      0x00000000
#define CSR_QSTATE_RD_3_ADDR                                         0x00000084
#define CSR_QSTATE_RD_3_DEFAULT                                      0x00000000
#define CSR_CSTATE_WR_0_ADDR                                         0x00000088
#define CSR_CSTATE_WR_0_DEFAULT                                      0x00000000
#define CSR_CSTATE_WR_1_ADDR                                         0x0000008c
#define CSR_CSTATE_WR_1_DEFAULT                                      0x00000000
#define CSR_CSTATE_RD_0_ADDR                                         0x00000090
#define CSR_CSTATE_RD_0_DEFAULT                                      0x00000000
#define CSR_CSTATE_RD_1_ADDR                                         0x00000094
#define CSR_CSTATE_RD_1_DEFAULT                                      0x00000000
#define CSR_CU_TIMER_ADDR                                            0x00000098
#define CSR_CU_TIMER_DEFAULT                                         0x00007a11
#define CSR_PPC_MBOX_BASE_ADDR                                       0x0000009c
#define CSR_PPC_MBOX_BASE_DEFAULT                                    0x003ffffe
#define CSR_PPC_FPOOL_BASE_ADDR                                      0x000000a0
#define CSR_PPC_FPOOL_BASE_DEFAULT                                   0x003fffff
#define CSR_ENQ_BASE_A_ADDR                                          0x000000a4
#define CSR_ENQ_BASE_A_DEFAULT                                       0x00fffff4
#define CSR_ENQ_BASE_B_ADDR                                          0x000000a8
#define CSR_ENQ_BASE_B_DEFAULT                                       0x00fffff7
#define CSR_ENQ_STATUS_0_ADDR                                        0x000000ac
#define CSR_ENQ_STATUS_0_DEFAULT                                     0x00000000
#define CSR_ENQ_STATUS_1_ADDR                                        0x000000b0
#define CSR_ENQ_STATUS_1_DEFAULT                                     0x00000000
#define CSR_ENQ_STATUS_2_ADDR                                        0x000000b4
#define CSR_ENQ_STATUS_2_DEFAULT                                     0x00000000
#define CSR_ENQ_STATUS_3_ADDR                                        0x000000b8
#define CSR_ENQ_STATUS_3_DEFAULT                                     0x00000000
#define QM_INTERRUPT_ADDR                                            0x000000bc
#define QM_INTERRUPT_DEFAULT                                         0x00000000
#define QM_INTERRUPTMASK_ADDR                                        0x000000c0
#define CSR_PBM_ERRINF_ADDR                                          0x000000c4
#define CSR_PBM_ERRINF_DEFAULT                                       0x00000000
#define CSR_MSGRD_ERRINF_ADDR                                        0x000000c8
#define CSR_MSGRD_ERRINF_DEFAULT                                     0x00000000
#define CSR_QM_SAB_PPC0_ADDR                                         0x000000cc
#define CSR_QM_SAB_PPC0_DEFAULT                                      0x00000000
#define CSR_QM_SAB_PPC0MASK_ADDR                                     0x000000d0
#define CSR_QM_SAB_PPC1_ADDR                                         0x000000d4
#define CSR_QM_SAB_PPC1_DEFAULT                                      0x00000000
#define CSR_QM_SAB_PPC1MASK_ADDR                                     0x000000d8
#define CSR_QM_MBOX_NE_ADDR                                          0x000000dc
#define CSR_QM_MBOX_NE_DEFAULT                                       0x00000000
#define CSR_PPC_SAB0_ADDR                                            0x000000e4
#define CSR_PPC_SAB0_DEFAULT                                         0x00000000
#define CSR_PPC_SAB1_ADDR                                            0x000000e8
#define CSR_PPC_SAB1_DEFAULT                                         0x00000000
#define CSR_PPC_SAB2_ADDR                                            0x000000ec
#define CSR_PPC_SAB2_DEFAULT                                         0x00000000
#define CSR_PPC_SAB3_ADDR                                            0x000000f0
#define CSR_PPC_SAB3_DEFAULT                                         0x00000000
#define CSR_PPC_SAB4_ADDR                                            0x000000f4
#define CSR_PPC_SAB4_DEFAULT                                         0x00000000
#define CSR_PPC_SAB5_ADDR                                            0x000000f8
#define CSR_PPC_SAB5_DEFAULT                                         0x00000000
#define CSR_PPC_SAB6_ADDR                                            0x000000fc
#define CSR_PPC_SAB6_DEFAULT                                         0x00000000
#define CSR_PPC_SAB7_ADDR                                            0x00000100
#define CSR_PPC_SAB7_DEFAULT                                         0x00000000
#define CSR_PB_SLAVE6_ADDR                                           0x00000104
#define CSR_PB_SLAVE6_DEFAULT                                        0x00000000
#define CSR_PB_SLAVE7_ADDR                                           0x00000108
#define CSR_PB_SLAVE7_DEFAULT                                        0x00000000
#define CSR_QM_STATS_CFG_ADDR                                        0x00000110
#define CSR_QM_STATS_CFG_DEFAULT                                     0x00000000
#define CSR_ENQ_STATISTICS_ADDR                                      0x00000114
#define CSR_ENQ_STATISTICS_DEFAULT                                   0x00000000
#define CSR_DEQ_STATISTICS_ADDR                                      0x00000118
#define CSR_DEQ_STATISTICS_DEFAULT                                   0x00000000
#define CSR_FIFO_STATUS_ADDR                                         0x0000011c
#define CSR_FIFO_STATUS_DEFAULT                                      0x00000000
#define CSR_QM_RAM_MARGIN_ADDR                                       0x000003f8
#define CSR_QM_RAM_MARGIN_DEFAULT                                    0x00000000
#define CSR_QM_TESTINT_ADDR                                          0x000003fc
#define CSR_QM_TESTINT_DEFAULT                                       0x00000000

/*	Register csr_ipbrr	*/ 
/*	 Fields revno	 */
#define REVNO_F14_WIDTH                                                       2
#define REVNO_F14_SHIFT                                                       8
#define REVNO_F14_MASK                                               0x00000300
#define REVNO_F14_RD(src)                             (((src) & 0x00000300)>>8)
#define REVNO_F14_SET(dst,src) \
                       (((dst) & ~0x00000300) | (((u32)(src)<<8) & 0x00000300))
/*	 Fields busid	 */
#define BUSID_F12_WIDTH                                                       3
#define BUSID_F12_SHIFT                                                       5
#define BUSID_F12_MASK                                               0x000000e0
#define BUSID_F12_RD(src)                             (((src) & 0x000000e0)>>5)
#define BUSID_F12_SET(dst,src) \
                       (((dst) & ~0x000000e0) | (((u32)(src)<<5) & 0x000000e0))
/*	 Fields deviceid	 */
#define DEVICEID_F16_WIDTH                                                    5
#define DEVICEID_F16_SHIFT                                                    0
#define DEVICEID_F16_MASK                                            0x0000001f
#define DEVICEID_F16_RD(src)                             (((src) & 0x0000001f))
#define DEVICEID_F16_SET(dst,src) \
                          (((dst) & ~0x0000001f) | (((u32)(src)) & 0x0000001f))

/*	Register csr_qm_config	*/ 
/*	 Fields enable	 */
#define ENABLE_F6_WIDTH                                                       1
#define ENABLE_F6_SHIFT                                                      31
#define ENABLE_F6_MASK                                               0x80000000
#define ENABLE_F6_RD(src)                            (((src) & 0x80000000)>>31)
#define ENABLE_F6_WR(src)                       (((u32)(src)<<31) & 0x80000000)
#define ENABLE_F6_SET(dst,src) \
                      (((dst) & ~0x80000000) | (((u32)(src)<<31) & 0x80000000))
/*	 Fields error_queue_enable	 */
#define ERROR_QUEUE_ENABLE_F2_WIDTH                                           1
#define ERROR_QUEUE_ENABLE_F2_SHIFT                                          30
#define ERROR_QUEUE_ENABLE_F2_MASK                                   0x40000000
#define ERROR_QUEUE_ENABLE_F2_RD(src)                (((src) & 0x40000000)>>30)
#define ERROR_QUEUE_ENABLE_F2_WR(src)           (((u32)(src)<<30) & 0x40000000)
#define ERROR_QUEUE_ENABLE_F2_SET(dst,src) \
                      (((dst) & ~0x40000000) | (((u32)(src)<<30) & 0x40000000))
/*	 Fields error_qid	 */
#define ERROR_QID_F2_WIDTH                                                    8
#define ERROR_QID_F2_SHIFT                                                    0
#define ERROR_QID_F2_MASK                                            0x000000ff
#define ERROR_QID_F2_RD(src)                             (((src) & 0x000000ff))
#define ERROR_QID_F2_WR(src)                        (((u32)(src)) & 0x000000ff)
#define ERROR_QID_F2_SET(dst,src) \
                          (((dst) & ~0x000000ff) | (((u32)(src)) & 0x000000ff))

/*	Register csr_pbm	*/ 
/*	 Fields slvid_pbn	 */
#define SLVID_PBN_F2_WIDTH                                                   10
#define SLVID_PBN_F2_SHIFT                                                    0
#define SLVID_PBN_F2_MASK                                            0x000003ff
#define SLVID_PBN_F2_RD(src)                             (((src) & 0x000003ff))
#define SLVID_PBN_F2_WR(src)                        (((u32)(src)) & 0x000003ff)
#define SLVID_PBN_F2_SET(dst,src) \
                          (((dst) & ~0x000003ff) | (((u32)(src)) & 0x000003ff))

/*	Register csr_pbm_buf_wr	*/ 
/*	 Fields overwrite	 */
#define OVERWRITE_F4_WIDTH                                                    1
#define OVERWRITE_F4_SHIFT                                                   31
#define OVERWRITE_F4_MASK                                            0x80000000
#define OVERWRITE_F4_RD(src)                         (((src) & 0x80000000)>>31)
#define OVERWRITE_F4_WR(src)                    (((u32)(src)<<31) & 0x80000000)
#define OVERWRITE_F4_SET(dst,src) \
                      (((dst) & ~0x80000000) | (((u32)(src)<<31) & 0x80000000))
/*	 Fields in_service_clr	 */
#define IN_SERVICE_CLR_F2_WIDTH                                               1
#define IN_SERVICE_CLR_F2_SHIFT                                              18
#define IN_SERVICE_CLR_F2_MASK                                       0x00040000
#define IN_SERVICE_CLR_F2_RD(src)                    (((src) & 0x00040000)>>18)
#define IN_SERVICE_CLR_F2_WR(src)               (((u32)(src)<<18) & 0x00040000)
#define IN_SERVICE_CLR_F2_SET(dst,src) \
                      (((dst) & ~0x00040000) | (((u32)(src)<<18) & 0x00040000))
/*	 Fields data	 */
#define DATA_F4_WIDTH                                                        18
#define DATA_F4_SHIFT                                                         0
#define DATA_F4_MASK                                                 0x0003ffff
#define DATA_F4_RD(src)                                  (((src) & 0x0003ffff))
#define DATA_F4_WR(src)                             (((u32)(src)) & 0x0003ffff)
#define DATA_F4_SET(dst,src) \
                          (((dst) & ~0x0003ffff) | (((u32)(src)) & 0x0003ffff))

/*	Register csr_pbm_buf_rd	*/ 
/*	 Fields pb_in_service	 */
#define PB_IN_SERVICE_F2_WIDTH                                                1
#define PB_IN_SERVICE_F2_SHIFT                                               18
#define PB_IN_SERVICE_F2_MASK                                        0x00040000
#define PB_IN_SERVICE_F2_RD(src)                     (((src) & 0x00040000)>>18)
#define PB_IN_SERVICE_F2_SET(dst,src) \
                      (((dst) & ~0x00040000) | (((u32)(src)<<18) & 0x00040000))
/*	 Fields slot_num	 */
#define SLOT_NUM_F2_WIDTH                                                     3
#define SLOT_NUM_F2_SHIFT                                                    15
#define SLOT_NUM_F2_MASK                                             0x00038000
#define SLOT_NUM_F2_RD(src)                          (((src) & 0x00038000)>>15)
#define SLOT_NUM_F2_SET(dst,src) \
                      (((dst) & ~0x00038000) | (((u32)(src)<<15) & 0x00038000))
/*	 Fields prefetch_buf_en	 */
#define PREFETCH_BUF_EN_F2_WIDTH                                              1
#define PREFETCH_BUF_EN_F2_SHIFT                                             14
#define PREFETCH_BUF_EN_F2_MASK                                      0x00004000
#define PREFETCH_BUF_EN_F2_RD(src)                   (((src) & 0x00004000)>>14)
#define PREFETCH_BUF_EN_F2_SET(dst,src) \
                      (((dst) & ~0x00004000) | (((u32)(src)<<14) & 0x00004000))
/*	 Fields is_free_pool	 */
#define IS_FREE_POOL_F2_WIDTH                                                 1
#define IS_FREE_POOL_F2_SHIFT                                                13
#define IS_FREE_POOL_F2_MASK                                         0x00002000
#define IS_FREE_POOL_F2_RD(src)                      (((src) & 0x00002000)>>13)
#define IS_FREE_POOL_F2_SET(dst,src) \
                      (((dst) & ~0x00002000) | (((u32)(src)<<13) & 0x00002000))
/*	 Fields tlvq	 */
#define TLVQ_F2_WIDTH                                                         1
#define TLVQ_F2_SHIFT                                                        12
#define TLVQ_F2_MASK                                                 0x00001000
#define TLVQ_F2_RD(src)                              (((src) & 0x00001000)>>12)
#define TLVQ_F2_SET(dst,src) \
                      (((dst) & ~0x00001000) | (((u32)(src)<<12) & 0x00001000))
/*	 Fields corresponding_qnum	 */
#define CORRESPONDING_QNUM_F2_WIDTH                                           8
#define CORRESPONDING_QNUM_F2_SHIFT                                           4
#define CORRESPONDING_QNUM_F2_MASK                                   0x00000ff0
#define CORRESPONDING_QNUM_F2_RD(src)                 (((src) & 0x00000ff0)>>4)
#define CORRESPONDING_QNUM_F2_SET(dst,src) \
                       (((dst) & ~0x00000ff0) | (((u32)(src)<<4) & 0x00000ff0))
/*	 Fields num_msgs_in_buf	 */
#define NUM_MSGS_IN_BUF_F2_WIDTH                                              4
#define NUM_MSGS_IN_BUF_F2_SHIFT                                              0
#define NUM_MSGS_IN_BUF_F2_MASK                                      0x0000000f
#define NUM_MSGS_IN_BUF_F2_RD(src)                       (((src) & 0x0000000f))
#define NUM_MSGS_IN_BUF_F2_SET(dst,src) \
                          (((dst) & ~0x0000000f) | (((u32)(src)) & 0x0000000f))

/*	Register csr_pbm_coal	*/ 
/*	 Fields qne_ctick_sel	 */
#define QNE_CTICK_SEL_WIDTH                                                   3
#define QNE_CTICK_SEL_SHIFT                                                  28
#define QNE_CTICK_SEL_MASK                                           0x70000000
#define QNE_CTICK_SEL_RD(src)                        (((src) & 0x70000000)>>28)
#define QNE_CTICK_SEL_WR(src)                   (((u32)(src)<<28) & 0x70000000)
#define QNE_CTICK_SEL_SET(dst,src) \
                      (((dst) & ~0x70000000) | (((u32)(src)<<28) & 0x70000000))
/*	 Fields observe_ctick_7	 */
#define OBSERVE_CTICK_7_WIDTH                                                 1
#define OBSERVE_CTICK_7_SHIFT                                                27
#define OBSERVE_CTICK_7_MASK                                         0x08000000
#define OBSERVE_CTICK_7_RD(src)                      (((src) & 0x08000000)>>27)
#define OBSERVE_CTICK_7_SET(dst,src) \
                      (((dst) & ~0x08000000) | (((u32)(src)<<27) & 0x08000000))
/*	 Fields count_range	 */
#define COUNT_RANGE_WIDTH                                                    16
#define COUNT_RANGE_SHIFT                                                     0
#define COUNT_RANGE_MASK                                             0x0000ffff
#define COUNT_RANGE_RD(src)                              (((src) & 0x0000ffff))
#define COUNT_RANGE_WR(src)                         (((u32)(src)) & 0x0000ffff)
#define COUNT_RANGE_SET(dst,src) \
                          (((dst) & ~0x0000ffff) | (((u32)(src)) & 0x0000ffff))

/*	Register csr_pbm_ctick0	*/ 
/*	 Fields mbox0	 */
#define MBOX00_WIDTH                                                          3
#define MBOX00_SHIFT                                                         28
#define MBOX00_MASK                                                  0x70000000
#define MBOX00_RD(src)                               (((src) & 0x70000000)>>28)
#define MBOX00_WR(src)                          (((u32)(src)<<28) & 0x70000000)
#define MBOX00_SET(dst,src) \
                      (((dst) & ~0x70000000) | (((u32)(src)<<28) & 0x70000000))
/*	 Fields mbox1	 */
#define MBOX10_WIDTH                                                          3
#define MBOX10_SHIFT                                                         24
#define MBOX10_MASK                                                  0x07000000
#define MBOX10_RD(src)                               (((src) & 0x07000000)>>24)
#define MBOX10_WR(src)                          (((u32)(src)<<24) & 0x07000000)
#define MBOX10_SET(dst,src) \
                      (((dst) & ~0x07000000) | (((u32)(src)<<24) & 0x07000000))
/*	 Fields mbox2	 */
#define MBOX20_WIDTH                                                          3
#define MBOX20_SHIFT                                                         20
#define MBOX20_MASK                                                  0x00700000
#define MBOX20_RD(src)                               (((src) & 0x00700000)>>20)
#define MBOX20_WR(src)                          (((u32)(src)<<20) & 0x00700000)
#define MBOX20_SET(dst,src) \
                      (((dst) & ~0x00700000) | (((u32)(src)<<20) & 0x00700000))
/*	 Fields mbox3	 */
#define MBOX30_WIDTH                                                          3
#define MBOX30_SHIFT                                                         16
#define MBOX30_MASK                                                  0x00070000
#define MBOX30_RD(src)                               (((src) & 0x00070000)>>16)
#define MBOX30_WR(src)                          (((u32)(src)<<16) & 0x00070000)
#define MBOX30_SET(dst,src) \
                      (((dst) & ~0x00070000) | (((u32)(src)<<16) & 0x00070000))
/*	 Fields mbox4	 */
#define MBOX40_WIDTH                                                          3
#define MBOX40_SHIFT                                                         12
#define MBOX40_MASK                                                  0x00007000
#define MBOX40_RD(src)                               (((src) & 0x00007000)>>12)
#define MBOX40_WR(src)                          (((u32)(src)<<12) & 0x00007000)
#define MBOX40_SET(dst,src) \
                      (((dst) & ~0x00007000) | (((u32)(src)<<12) & 0x00007000))
/*	 Fields mbox5	 */
#define MBOX50_WIDTH                                                          3
#define MBOX50_SHIFT                                                          8
#define MBOX50_MASK                                                  0x00000700
#define MBOX50_RD(src)                                (((src) & 0x00000700)>>8)
#define MBOX50_WR(src)                           (((u32)(src)<<8) & 0x00000700)
#define MBOX50_SET(dst,src) \
                       (((dst) & ~0x00000700) | (((u32)(src)<<8) & 0x00000700))
/*	 Fields mbox6	 */
#define MBOX60_WIDTH                                                          3
#define MBOX60_SHIFT                                                          4
#define MBOX60_MASK                                                  0x00000070
#define MBOX60_RD(src)                                (((src) & 0x00000070)>>4)
#define MBOX60_WR(src)                           (((u32)(src)<<4) & 0x00000070)
#define MBOX60_SET(dst,src) \
                       (((dst) & ~0x00000070) | (((u32)(src)<<4) & 0x00000070))
/*	 Fields mbox7	 */
#define MBOX70_WIDTH                                                          3
#define MBOX70_SHIFT                                                          0
#define MBOX70_MASK                                                  0x00000007
#define MBOX70_RD(src)                                   (((src) & 0x00000007))
#define MBOX70_WR(src)                              (((u32)(src)) & 0x00000007)
#define MBOX70_SET(dst,src) \
                          (((dst) & ~0x00000007) | (((u32)(src)) & 0x00000007))

/*	Register csr_pbm_ctick1	*/ 
/*	 Fields mbox8	 */
#define MBOX81_WIDTH                                                          3
#define MBOX81_SHIFT                                                         28
#define MBOX81_MASK                                                  0x70000000
#define MBOX81_RD(src)                               (((src) & 0x70000000)>>28)
#define MBOX81_WR(src)                          (((u32)(src)<<28) & 0x70000000)
#define MBOX81_SET(dst,src) \
                      (((dst) & ~0x70000000) | (((u32)(src)<<28) & 0x70000000))
/*	 Fields mbox9	 */
#define MBOX91_WIDTH                                                          3
#define MBOX91_SHIFT                                                         24
#define MBOX91_MASK                                                  0x07000000
#define MBOX91_RD(src)                               (((src) & 0x07000000)>>24)
#define MBOX91_WR(src)                          (((u32)(src)<<24) & 0x07000000)
#define MBOX91_SET(dst,src) \
                      (((dst) & ~0x07000000) | (((u32)(src)<<24) & 0x07000000))
/*	 Fields mbox10	 */
#define MBOX101_WIDTH                                                         3
#define MBOX101_SHIFT                                                        20
#define MBOX101_MASK                                                 0x00700000
#define MBOX101_RD(src)                              (((src) & 0x00700000)>>20)
#define MBOX101_WR(src)                         (((u32)(src)<<20) & 0x00700000)
#define MBOX101_SET(dst,src) \
                      (((dst) & ~0x00700000) | (((u32)(src)<<20) & 0x00700000))
/*	 Fields mbox11	 */
#define MBOX111_WIDTH                                                         3
#define MBOX111_SHIFT                                                        16
#define MBOX111_MASK                                                 0x00070000
#define MBOX111_RD(src)                              (((src) & 0x00070000)>>16)
#define MBOX111_WR(src)                         (((u32)(src)<<16) & 0x00070000)
#define MBOX111_SET(dst,src) \
                      (((dst) & ~0x00070000) | (((u32)(src)<<16) & 0x00070000))
/*	 Fields mbox12	 */
#define MBOX121_WIDTH                                                         3
#define MBOX121_SHIFT                                                        12
#define MBOX121_MASK                                                 0x00007000
#define MBOX121_RD(src)                              (((src) & 0x00007000)>>12)
#define MBOX121_WR(src)                         (((u32)(src)<<12) & 0x00007000)
#define MBOX121_SET(dst,src) \
                      (((dst) & ~0x00007000) | (((u32)(src)<<12) & 0x00007000))
/*	 Fields mbox13	 */
#define MBOX131_WIDTH                                                         3
#define MBOX131_SHIFT                                                         8
#define MBOX131_MASK                                                 0x00000700
#define MBOX131_RD(src)                               (((src) & 0x00000700)>>8)
#define MBOX131_WR(src)                          (((u32)(src)<<8) & 0x00000700)
#define MBOX131_SET(dst,src) \
                       (((dst) & ~0x00000700) | (((u32)(src)<<8) & 0x00000700))
/*	 Fields mbox14	 */
#define MBOX141_WIDTH                                                         3
#define MBOX141_SHIFT                                                         4
#define MBOX141_MASK                                                 0x00000070
#define MBOX141_RD(src)                               (((src) & 0x00000070)>>4)
#define MBOX141_WR(src)                          (((u32)(src)<<4) & 0x00000070)
#define MBOX141_SET(dst,src) \
                       (((dst) & ~0x00000070) | (((u32)(src)<<4) & 0x00000070))
/*	 Fields mbox15	 */
#define MBOX151_WIDTH                                                         3
#define MBOX151_SHIFT                                                         0
#define MBOX151_MASK                                                 0x00000007
#define MBOX151_RD(src)                                  (((src) & 0x00000007))
#define MBOX151_WR(src)                             (((u32)(src)) & 0x00000007)
#define MBOX151_SET(dst,src) \
                          (((dst) & ~0x00000007) | (((u32)(src)) & 0x00000007))

/*	Register csr_threshold0_set0	*/ 
/*	 Fields thr0	 */
#define THR00_WIDTH                                                          18
#define THR00_SHIFT                                                           0
#define THR00_MASK                                                   0x0003ffff
#define THR00_RD(src)                                    (((src) & 0x0003ffff))
#define THR00_WR(src)                               (((u32)(src)) & 0x0003ffff)
#define THR00_SET(dst,src) \
                          (((dst) & ~0x0003ffff) | (((u32)(src)) & 0x0003ffff))

/*	Register csr_threshold1_set0	*/ 
/*	 Fields thr1	 */
#define THR10_WIDTH                                                          18
#define THR10_SHIFT                                                           0
#define THR10_MASK                                                   0x0003ffff
#define THR10_RD(src)                                    (((src) & 0x0003ffff))
#define THR10_WR(src)                               (((u32)(src)) & 0x0003ffff)
#define THR10_SET(dst,src) \
                          (((dst) & ~0x0003ffff) | (((u32)(src)) & 0x0003ffff))

/*	Register csr_threshold0_set1	*/ 
/*	 Fields thr0	 */
#define THR01_WIDTH                                                          18
#define THR01_SHIFT                                                           0
#define THR01_MASK                                                   0x0003ffff
#define THR01_RD(src)                                    (((src) & 0x0003ffff))
#define THR01_WR(src)                               (((u32)(src)) & 0x0003ffff)
#define THR01_SET(dst,src) \
                          (((dst) & ~0x0003ffff) | (((u32)(src)) & 0x0003ffff))

/*	Register csr_threshold1_set1	*/ 
/*	 Fields thr1	 */
#define THR11_WIDTH                                                          18
#define THR11_SHIFT                                                           0
#define THR11_MASK                                                   0x0003ffff
#define THR11_RD(src)                                    (((src) & 0x0003ffff))
#define THR11_WR(src)                               (((u32)(src)) & 0x0003ffff)
#define THR11_SET(dst,src) \
                          (((dst) & ~0x0003ffff) | (((u32)(src)) & 0x0003ffff))

/*	Register csr_threshold0_set2	*/ 
/*	 Fields thr0	 */
#define THR02_WIDTH                                                          18
#define THR02_SHIFT                                                           0
#define THR02_MASK                                                   0x0003ffff
#define THR02_RD(src)                                    (((src) & 0x0003ffff))
#define THR02_WR(src)                               (((u32)(src)) & 0x0003ffff)
#define THR02_SET(dst,src) \
                          (((dst) & ~0x0003ffff) | (((u32)(src)) & 0x0003ffff))

/*	Register csr_threshold1_set2	*/ 
/*	 Fields thr1	 */
#define THR12_WIDTH                                                          18
#define THR12_SHIFT                                                           0
#define THR12_MASK                                                   0x0003ffff
#define THR12_RD(src)                                    (((src) & 0x0003ffff))
#define THR12_WR(src)                               (((u32)(src)) & 0x0003ffff)
#define THR12_SET(dst,src) \
                          (((dst) & ~0x0003ffff) | (((u32)(src)) & 0x0003ffff))

/*	Register csr_threshold0_set3	*/ 
/*	 Fields thr0	 */
#define THR03_WIDTH                                                          18
#define THR03_SHIFT                                                           0
#define THR03_MASK                                                   0x0003ffff
#define THR03_RD(src)                                    (((src) & 0x0003ffff))
#define THR03_WR(src)                               (((u32)(src)) & 0x0003ffff)
#define THR03_SET(dst,src) \
                          (((dst) & ~0x0003ffff) | (((u32)(src)) & 0x0003ffff))

/*	Register csr_threshold1_set3	*/ 
/*	 Fields thr1	 */
#define THR13_WIDTH                                                          18
#define THR13_SHIFT                                                           0
#define THR13_MASK                                                   0x0003ffff
#define THR13_RD(src)                                    (((src) & 0x0003ffff))
#define THR13_WR(src)                               (((u32)(src)) & 0x0003ffff)
#define THR13_SET(dst,src) \
                          (((dst) & ~0x0003ffff) | (((u32)(src)) & 0x0003ffff))

/*	Register csr_threshold0_set4	*/ 
/*	 Fields thr0	 */
#define THR04_WIDTH                                                          18
#define THR04_SHIFT                                                           0
#define THR04_MASK                                                   0x0003ffff
#define THR04_RD(src)                                    (((src) & 0x0003ffff))
#define THR04_WR(src)                               (((u32)(src)) & 0x0003ffff)
#define THR04_SET(dst,src) \
                          (((dst) & ~0x0003ffff) | (((u32)(src)) & 0x0003ffff))

/*	Register csr_threshold1_set4	*/ 
/*	 Fields thr1	 */
#define THR14_WIDTH                                                          18
#define THR14_SHIFT                                                           0
#define THR14_MASK                                                   0x0003ffff
#define THR14_RD(src)                                    (((src) & 0x0003ffff))
#define THR14_WR(src)                               (((u32)(src)) & 0x0003ffff)
#define THR14_SET(dst,src) \
                          (((dst) & ~0x0003ffff) | (((u32)(src)) & 0x0003ffff))

/*	Register csr_threshold0_set5	*/ 
/*	 Fields thr0	 */
#define THR05_WIDTH                                                          18
#define THR05_SHIFT                                                           0
#define THR05_MASK                                                   0x0003ffff
#define THR05_RD(src)                                    (((src) & 0x0003ffff))
#define THR05_WR(src)                               (((u32)(src)) & 0x0003ffff)
#define THR05_SET(dst,src) \
                          (((dst) & ~0x0003ffff) | (((u32)(src)) & 0x0003ffff))

/*	Register csr_threshold1_set5	*/ 
/*	 Fields thr1	 */
#define THR15_WIDTH                                                          18
#define THR15_SHIFT                                                           0
#define THR15_MASK                                                   0x0003ffff
#define THR15_RD(src)                                    (((src) & 0x0003ffff))
#define THR15_WR(src)                               (((u32)(src)) & 0x0003ffff)
#define THR15_SET(dst,src) \
                          (((dst) & ~0x0003ffff) | (((u32)(src)) & 0x0003ffff))

/*	Register csr_threshold0_set6	*/ 
/*	 Fields thr0	 */
#define THR06_WIDTH                                                          18
#define THR06_SHIFT                                                           0
#define THR06_MASK                                                   0x0003ffff
#define THR06_RD(src)                                    (((src) & 0x0003ffff))
#define THR06_WR(src)                               (((u32)(src)) & 0x0003ffff)
#define THR06_SET(dst,src) \
                          (((dst) & ~0x0003ffff) | (((u32)(src)) & 0x0003ffff))

/*	Register csr_threshold1_set6	*/ 
/*	 Fields thr1	 */
#define THR16_WIDTH                                                          18
#define THR16_SHIFT                                                           0
#define THR16_MASK                                                   0x0003ffff
#define THR16_RD(src)                                    (((src) & 0x0003ffff))
#define THR16_WR(src)                               (((u32)(src)) & 0x0003ffff)
#define THR16_SET(dst,src) \
                          (((dst) & ~0x0003ffff) | (((u32)(src)) & 0x0003ffff))

/*	Register csr_threshold0_set7	*/ 
/*	 Fields thr0	 */
#define THR07_WIDTH                                                          18
#define THR07_SHIFT                                                           0
#define THR07_MASK                                                   0x0003ffff
#define THR07_RD(src)                                    (((src) & 0x0003ffff))
#define THR07_WR(src)                               (((u32)(src)) & 0x0003ffff)
#define THR07_SET(dst,src) \
                          (((dst) & ~0x0003ffff) | (((u32)(src)) & 0x0003ffff))

/*	Register csr_threshold1_set7	*/ 
/*	 Fields thr1	 */
#define THR17_WIDTH                                                          18
#define THR17_SHIFT                                                           0
#define THR17_MASK                                                   0x0003ffff
#define THR17_RD(src)                                    (((src) & 0x0003ffff))
#define THR17_WR(src)                               (((u32)(src)) & 0x0003ffff)
#define THR17_SET(dst,src) \
                          (((dst) & ~0x0003ffff) | (((u32)(src)) & 0x0003ffff))

/*	Register csr_hysteresis	*/ 
/*	 Fields set0_hyst	 */
#define SET0_HYST_WIDTH                                                       4
#define SET0_HYST_SHIFT                                                      28
#define SET0_HYST_MASK                                               0xf0000000
#define SET0_HYST_RD(src)                            (((src) & 0xf0000000)>>28)
#define SET0_HYST_WR(src)                       (((u32)(src)<<28) & 0xf0000000)
#define SET0_HYST_SET(dst,src) \
                      (((dst) & ~0xf0000000) | (((u32)(src)<<28) & 0xf0000000))
/*	 Fields set1_hyst	 */
#define SET1_HYST_WIDTH                                                       4
#define SET1_HYST_SHIFT                                                      24
#define SET1_HYST_MASK                                               0x0f000000
#define SET1_HYST_RD(src)                            (((src) & 0x0f000000)>>24)
#define SET1_HYST_WR(src)                       (((u32)(src)<<24) & 0x0f000000)
#define SET1_HYST_SET(dst,src) \
                      (((dst) & ~0x0f000000) | (((u32)(src)<<24) & 0x0f000000))
/*	 Fields set2_hyst	 */
#define SET2_HYST_WIDTH                                                       4
#define SET2_HYST_SHIFT                                                      20
#define SET2_HYST_MASK                                               0x00f00000
#define SET2_HYST_RD(src)                            (((src) & 0x00f00000)>>20)
#define SET2_HYST_WR(src)                       (((u32)(src)<<20) & 0x00f00000)
#define SET2_HYST_SET(dst,src) \
                      (((dst) & ~0x00f00000) | (((u32)(src)<<20) & 0x00f00000))
/*	 Fields set3_hyst	 */
#define SET3_HYST_WIDTH                                                       4
#define SET3_HYST_SHIFT                                                      16
#define SET3_HYST_MASK                                               0x000f0000
#define SET3_HYST_RD(src)                            (((src) & 0x000f0000)>>16)
#define SET3_HYST_WR(src)                       (((u32)(src)<<16) & 0x000f0000)
#define SET3_HYST_SET(dst,src) \
                      (((dst) & ~0x000f0000) | (((u32)(src)<<16) & 0x000f0000))
/*	 Fields set4_hyst	 */
#define SET4_HYST_WIDTH                                                       4
#define SET4_HYST_SHIFT                                                      12
#define SET4_HYST_MASK                                               0x0000f000
#define SET4_HYST_RD(src)                            (((src) & 0x0000f000)>>12)
#define SET4_HYST_WR(src)                       (((u32)(src)<<12) & 0x0000f000)
#define SET4_HYST_SET(dst,src) \
                      (((dst) & ~0x0000f000) | (((u32)(src)<<12) & 0x0000f000))
/*	 Fields set5_hyst	 */
#define SET5_HYST_WIDTH                                                       4
#define SET5_HYST_SHIFT                                                       8
#define SET5_HYST_MASK                                               0x00000f00
#define SET5_HYST_RD(src)                             (((src) & 0x00000f00)>>8)
#define SET5_HYST_WR(src)                        (((u32)(src)<<8) & 0x00000f00)
#define SET5_HYST_SET(dst,src) \
                       (((dst) & ~0x00000f00) | (((u32)(src)<<8) & 0x00000f00))
/*	 Fields set6_hyst	 */
#define SET6_HYST_WIDTH                                                       4
#define SET6_HYST_SHIFT                                                       4
#define SET6_HYST_MASK                                               0x000000f0
#define SET6_HYST_RD(src)                             (((src) & 0x000000f0)>>4)
#define SET6_HYST_WR(src)                        (((u32)(src)<<4) & 0x000000f0)
#define SET6_HYST_SET(dst,src) \
                       (((dst) & ~0x000000f0) | (((u32)(src)<<4) & 0x000000f0))
/*	 Fields set7_hyst	 */
#define SET7_HYST_WIDTH                                                       4
#define SET7_HYST_SHIFT                                                       0
#define SET7_HYST_MASK                                               0x0000000f
#define SET7_HYST_RD(src)                                (((src) & 0x0000000f))
#define SET7_HYST_WR(src)                           (((u32)(src)) & 0x0000000f)
#define SET7_HYST_SET(dst,src) \
                          (((dst) & ~0x0000000f) | (((u32)(src)) & 0x0000000f))

/*	Register csr_qstate	*/ 
/*	 Fields qnumber	 */
#define QNUMBER_F2_WIDTH                                                      8
#define QNUMBER_F2_SHIFT                                                      0
#define QNUMBER_F2_MASK                                              0x000000ff
#define QNUMBER_F2_RD(src)                               (((src) & 0x000000ff))
#define QNUMBER_F2_WR(src)                          (((u32)(src)) & 0x000000ff)
#define QNUMBER_F2_SET(dst,src) \
                          (((dst) & ~0x000000ff) | (((u32)(src)) & 0x000000ff))

/*	Register csr_qstate_wr_0	*/ 
/*	 Fields data	 */
#define DATA0_F8_WIDTH                                                       32
#define DATA0_F8_SHIFT                                                        0
#define DATA0_F8_MASK                                                0xffffffff
#define DATA0_F8_RD(src)                                 (((src) & 0xffffffff))
#define DATA0_F8_WR(src)                            (((u32)(src)) & 0xffffffff)
#define DATA0_F8_SET(dst,src) \
                          (((dst) & ~0xffffffff) | (((u32)(src)) & 0xffffffff))

/*	Register csr_qstate_wr_1	*/ 
/*	 Fields data	 */
#define DATA1_F8_WIDTH                                                       32
#define DATA1_F8_SHIFT                                                        0
#define DATA1_F8_MASK                                                0xffffffff
#define DATA1_F8_RD(src)                                 (((src) & 0xffffffff))
#define DATA1_F8_WR(src)                            (((u32)(src)) & 0xffffffff)
#define DATA1_F8_SET(dst,src) \
                          (((dst) & ~0xffffffff) | (((u32)(src)) & 0xffffffff))

/*	Register csr_qstate_wr_2	*/ 
/*	 Fields data	 */
#define DATA2_F8_WIDTH                                                       32
#define DATA2_F8_SHIFT                                                        0
#define DATA2_F8_MASK                                                0xffffffff
#define DATA2_F8_RD(src)                                 (((src) & 0xffffffff))
#define DATA2_F8_WR(src)                            (((u32)(src)) & 0xffffffff)
#define DATA2_F8_SET(dst,src) \
                          (((dst) & ~0xffffffff) | (((u32)(src)) & 0xffffffff))

/*	Register csr_qstate_wr_3	*/ 
/*	 Fields data	 */
#define DATA3_F4_WIDTH                                                       32
#define DATA3_F4_SHIFT                                                        0
#define DATA3_F4_MASK                                                0xffffffff
#define DATA3_F4_RD(src)                                 (((src) & 0xffffffff))
#define DATA3_F4_WR(src)                            (((u32)(src)) & 0xffffffff)
#define DATA3_F4_SET(dst,src) \
                          (((dst) & ~0xffffffff) | (((u32)(src)) & 0xffffffff))

/*	Register csr_qstate_rd_0	*/ 
/*	 Fields data	 */
#define DATA0_F9_WIDTH                                                       32
#define DATA0_F9_SHIFT                                                        0
#define DATA0_F9_MASK                                                0xffffffff
#define DATA0_F9_RD(src)                                 (((src) & 0xffffffff))
#define DATA0_F9_SET(dst,src) \
                          (((dst) & ~0xffffffff) | (((u32)(src)) & 0xffffffff))

/*	Register csr_qstate_rd_1	*/ 
/*	 Fields data	 */
#define DATA1_F9_WIDTH                                                       32
#define DATA1_F9_SHIFT                                                        0
#define DATA1_F9_MASK                                                0xffffffff
#define DATA1_F9_RD(src)                                 (((src) & 0xffffffff))
#define DATA1_F9_SET(dst,src) \
                          (((dst) & ~0xffffffff) | (((u32)(src)) & 0xffffffff))

/*	Register csr_qstate_rd_2	*/ 
/*	 Fields data	 */
#define DATA2_F9_WIDTH                                                       32
#define DATA2_F9_SHIFT                                                        0
#define DATA2_F9_MASK                                                0xffffffff
#define DATA2_F9_RD(src)                                 (((src) & 0xffffffff))
#define DATA2_F9_SET(dst,src) \
                          (((dst) & ~0xffffffff) | (((u32)(src)) & 0xffffffff))

/*	Register csr_qstate_rd_3	*/ 
/*	 Fields data	 */
#define DATA3_F5_WIDTH                                                       32
#define DATA3_F5_SHIFT                                                        0
#define DATA3_F5_MASK                                                0xffffffff
#define DATA3_F5_RD(src)                                 (((src) & 0xffffffff))
#define DATA3_F5_SET(dst,src) \
                          (((dst) & ~0xffffffff) | (((u32)(src)) & 0xffffffff))

/*	Register csr_cstate_wr_0	*/ 
/*	 Fields data	 */
#define DATA0_F10_WIDTH                                                      15
#define DATA0_F10_SHIFT                                                       0
#define DATA0_F10_MASK                                               0x00007fff
#define DATA0_F10_RD(src)                                (((src) & 0x00007fff))
#define DATA0_F10_WR(src)                           (((u32)(src)) & 0x00007fff)
#define DATA0_F10_SET(dst,src) \
                          (((dst) & ~0x00007fff) | (((u32)(src)) & 0x00007fff))

/*	Register csr_cstate_wr_1	*/ 
/*	 Fields data	 */
#define DATA1_F10_WIDTH                                                      32
#define DATA1_F10_SHIFT                                                       0
#define DATA1_F10_MASK                                               0xffffffff
#define DATA1_F10_RD(src)                                (((src) & 0xffffffff))
#define DATA1_F10_WR(src)                           (((u32)(src)) & 0xffffffff)
#define DATA1_F10_SET(dst,src) \
                          (((dst) & ~0xffffffff) | (((u32)(src)) & 0xffffffff))

/*	Register csr_cstate_rd_0	*/ 
/*	 Fields data	 */
#define DATA0_F11_WIDTH                                                      15
#define DATA0_F11_SHIFT                                                       0
#define DATA0_F11_MASK                                               0x00007fff
#define DATA0_F11_RD(src)                                (((src) & 0x00007fff))
#define DATA0_F11_SET(dst,src) \
                          (((dst) & ~0x00007fff) | (((u32)(src)) & 0x00007fff))

/*	Register csr_cstate_rd_1	*/ 
/*	 Fields data	 */
#define DATA1_F11_WIDTH                                                      32
#define DATA1_F11_SHIFT                                                       0
#define DATA1_F11_MASK                                               0xffffffff
#define DATA1_F11_RD(src)                                (((src) & 0xffffffff))
#define DATA1_F11_SET(dst,src) \
                          (((dst) & ~0xffffffff) | (((u32)(src)) & 0xffffffff))

/*	Register csr_cu_timer	*/ 
/*	 Fields init	 */
#define INIT_F2_WIDTH                                                        20
#define INIT_F2_SHIFT                                                         0
#define INIT_F2_MASK                                                 0x000fffff
#define INIT_F2_RD(src)                                  (((src) & 0x000fffff))
#define INIT_F2_WR(src)                             (((u32)(src)) & 0x000fffff)
#define INIT_F2_SET(dst,src) \
                          (((dst) & ~0x000fffff) | (((u32)(src)) & 0x000fffff))

/*	Register csr_ppc_mbox_base	*/ 
/*	 Fields coherent	 */
#define COHERENT_F4_WIDTH                                                     1
#define COHERENT_F4_SHIFT                                                    31
#define COHERENT_F4_MASK                                             0x80000000
#define COHERENT_F4_RD(src)                          (((src) & 0x80000000)>>31)
#define COHERENT_F4_WR(src)                     (((u32)(src)<<31) & 0x80000000)
#define COHERENT_F4_SET(dst,src) \
                      (((dst) & ~0x80000000) | (((u32)(src)<<31) & 0x80000000))
/*	 Fields addr	 */
#define ADDR_F12_WIDTH                                                       24
#define ADDR_F12_SHIFT                                                        0
#define ADDR_F12_MASK                                                0x00ffffff
#define ADDR_F12_RD(src)                                 (((src) & 0x00ffffff))
#define ADDR_F12_WR(src)                            (((u32)(src)) & 0x00ffffff)
#define ADDR_F12_SET(dst,src) \
                          (((dst) & ~0x00ffffff) | (((u32)(src)) & 0x00ffffff))

/*	Register csr_ppc_fpool_base	*/ 
/*	 Fields coherent	 */
#define COHERENT_F5_WIDTH                                                     1
#define COHERENT_F5_SHIFT                                                    31
#define COHERENT_F5_MASK                                             0x80000000
#define COHERENT_F5_RD(src)                          (((src) & 0x80000000)>>31)
#define COHERENT_F5_WR(src)                     (((u32)(src)<<31) & 0x80000000)
#define COHERENT_F5_SET(dst,src) \
                      (((dst) & ~0x80000000) | (((u32)(src)<<31) & 0x80000000))
/*	 Fields addr	 */
#define ADDR_F13_WIDTH                                                       24
#define ADDR_F13_SHIFT                                                        0
#define ADDR_F13_MASK                                                0x00ffffff
#define ADDR_F13_RD(src)                                 (((src) & 0x00ffffff))
#define ADDR_F13_WR(src)                            (((u32)(src)) & 0x00ffffff)
#define ADDR_F13_SET(dst,src) \
                          (((dst) & ~0x00ffffff) | (((u32)(src)) & 0x00ffffff))

/*	Register csr_enq_base_a	*/ 
/*	 Fields coherent	 */
#define COHERENT_F6_WIDTH                                                     1
#define COHERENT_F6_SHIFT                                                    31
#define COHERENT_F6_MASK                                             0x80000000
#define COHERENT_F6_RD(src)                          (((src) & 0x80000000)>>31)
#define COHERENT_F6_WR(src)                     (((u32)(src)<<31) & 0x80000000)
#define COHERENT_F6_SET(dst,src) \
                      (((dst) & ~0x80000000) | (((u32)(src)<<31) & 0x80000000))
/*	 Fields addr	 */
#define ADDR_F14_WIDTH                                                       26
#define ADDR_F14_SHIFT                                                        0
#define ADDR_F14_MASK                                                0x03ffffff
#define ADDR_F14_RD(src)                                 (((src) & 0x03ffffff))
#define ADDR_F14_WR(src)                            (((u32)(src)) & 0x03ffffff)
#define ADDR_F14_SET(dst,src) \
                          (((dst) & ~0x03ffffff) | (((u32)(src)) & 0x03ffffff))

/*	Register csr_enq_base_b	*/ 
/*	 Fields coherent	 */
#define COHERENT_F7_WIDTH                                                     1
#define COHERENT_F7_SHIFT                                                    31
#define COHERENT_F7_MASK                                             0x80000000
#define COHERENT_F7_RD(src)                          (((src) & 0x80000000)>>31)
#define COHERENT_F7_WR(src)                     (((u32)(src)<<31) & 0x80000000)
#define COHERENT_F7_SET(dst,src) \
                      (((dst) & ~0x80000000) | (((u32)(src)<<31) & 0x80000000))
/*	 Fields addr	 */
#define ADDR_F15_WIDTH                                                       26
#define ADDR_F15_SHIFT                                                        0
#define ADDR_F15_MASK                                                0x03ffffff
#define ADDR_F15_RD(src)                                 (((src) & 0x03ffffff))
#define ADDR_F15_WR(src)                            (((u32)(src)) & 0x03ffffff)
#define ADDR_F15_SET(dst,src) \
                          (((dst) & ~0x03ffffff) | (((u32)(src)) & 0x03ffffff))

/*	Register csr_enq_status_0	*/ 
/*	 Fields mbox_0	 */
#define MBOX_00_F2_WIDTH                                                      8
#define MBOX_00_F2_SHIFT                                                     24
#define MBOX_00_F2_MASK                                              0xff000000
#define MBOX_00_F2_RD(src)                           (((src) & 0xff000000)>>24)
#define MBOX_00_F2_SET(dst,src) \
                      (((dst) & ~0xff000000) | (((u32)(src)<<24) & 0xff000000))
/*	 Fields mbox_1	 */
#define MBOX_10_F2_WIDTH                                                      8
#define MBOX_10_F2_SHIFT                                                     16
#define MBOX_10_F2_MASK                                              0x00ff0000
#define MBOX_10_F2_RD(src)                           (((src) & 0x00ff0000)>>16)
#define MBOX_10_F2_SET(dst,src) \
                      (((dst) & ~0x00ff0000) | (((u32)(src)<<16) & 0x00ff0000))
/*	 Fields mbox_2	 */
#define MBOX_20_F2_WIDTH                                                      8
#define MBOX_20_F2_SHIFT                                                      8
#define MBOX_20_F2_MASK                                              0x0000ff00
#define MBOX_20_F2_RD(src)                            (((src) & 0x0000ff00)>>8)
#define MBOX_20_F2_SET(dst,src) \
                       (((dst) & ~0x0000ff00) | (((u32)(src)<<8) & 0x0000ff00))
/*	 Fields mbox_3	 */
#define MBOX_30_F2_WIDTH                                                      8
#define MBOX_30_F2_SHIFT                                                      0
#define MBOX_30_F2_MASK                                              0x000000ff
#define MBOX_30_F2_RD(src)                               (((src) & 0x000000ff))
#define MBOX_30_F2_SET(dst,src) \
                          (((dst) & ~0x000000ff) | (((u32)(src)) & 0x000000ff))

/*	Register csr_enq_status_1	*/ 
/*	 Fields mbox_4	 */
#define MBOX_41_F2_WIDTH                                                      8
#define MBOX_41_F2_SHIFT                                                     24
#define MBOX_41_F2_MASK                                              0xff000000
#define MBOX_41_F2_RD(src)                           (((src) & 0xff000000)>>24)
#define MBOX_41_F2_SET(dst,src) \
                      (((dst) & ~0xff000000) | (((u32)(src)<<24) & 0xff000000))
/*	 Fields mbox_5	 */
#define MBOX_51_F2_WIDTH                                                      8
#define MBOX_51_F2_SHIFT                                                     16
#define MBOX_51_F2_MASK                                              0x00ff0000
#define MBOX_51_F2_RD(src)                           (((src) & 0x00ff0000)>>16)
#define MBOX_51_F2_SET(dst,src) \
                      (((dst) & ~0x00ff0000) | (((u32)(src)<<16) & 0x00ff0000))
/*	 Fields mbox_6	 */
#define MBOX_61_F2_WIDTH                                                      8
#define MBOX_61_F2_SHIFT                                                      8
#define MBOX_61_F2_MASK                                              0x0000ff00
#define MBOX_61_F2_RD(src)                            (((src) & 0x0000ff00)>>8)
#define MBOX_61_F2_SET(dst,src) \
                       (((dst) & ~0x0000ff00) | (((u32)(src)<<8) & 0x0000ff00))
/*	 Fields mbox_7	 */
#define MBOX_71_F2_WIDTH                                                      8
#define MBOX_71_F2_SHIFT                                                      0
#define MBOX_71_F2_MASK                                              0x000000ff
#define MBOX_71_F2_RD(src)                               (((src) & 0x000000ff))
#define MBOX_71_F2_SET(dst,src) \
                          (((dst) & ~0x000000ff) | (((u32)(src)) & 0x000000ff))

/*	Register csr_enq_status_2	*/ 
/*	 Fields mbox_8	 */
#define MBOX_82_F2_WIDTH                                                      8
#define MBOX_82_F2_SHIFT                                                     24
#define MBOX_82_F2_MASK                                              0xff000000
#define MBOX_82_F2_RD(src)                           (((src) & 0xff000000)>>24)
#define MBOX_82_F2_SET(dst,src) \
                      (((dst) & ~0xff000000) | (((u32)(src)<<24) & 0xff000000))
/*	 Fields mbox_9	 */
#define MBOX_92_F2_WIDTH                                                      8
#define MBOX_92_F2_SHIFT                                                     16
#define MBOX_92_F2_MASK                                              0x00ff0000
#define MBOX_92_F2_RD(src)                           (((src) & 0x00ff0000)>>16)
#define MBOX_92_F2_SET(dst,src) \
                      (((dst) & ~0x00ff0000) | (((u32)(src)<<16) & 0x00ff0000))
/*	 Fields mbox_10	 */
#define MBOX_102_F2_WIDTH                                                     8
#define MBOX_102_F2_SHIFT                                                     8
#define MBOX_102_F2_MASK                                             0x0000ff00
#define MBOX_102_F2_RD(src)                           (((src) & 0x0000ff00)>>8)
#define MBOX_102_F2_SET(dst,src) \
                       (((dst) & ~0x0000ff00) | (((u32)(src)<<8) & 0x0000ff00))
/*	 Fields mbox_11	 */
#define MBOX_112_F2_WIDTH                                                     8
#define MBOX_112_F2_SHIFT                                                     0
#define MBOX_112_F2_MASK                                             0x000000ff
#define MBOX_112_F2_RD(src)                              (((src) & 0x000000ff))
#define MBOX_112_F2_SET(dst,src) \
                          (((dst) & ~0x000000ff) | (((u32)(src)) & 0x000000ff))

/*	Register csr_enq_status_3	*/ 
/*	 Fields mbox_12	 */
#define MBOX_123_F2_WIDTH                                                     8
#define MBOX_123_F2_SHIFT                                                    24
#define MBOX_123_F2_MASK                                             0xff000000
#define MBOX_123_F2_RD(src)                          (((src) & 0xff000000)>>24)
#define MBOX_123_F2_SET(dst,src) \
                      (((dst) & ~0xff000000) | (((u32)(src)<<24) & 0xff000000))
/*	 Fields mbox_13	 */
#define MBOX_133_F2_WIDTH                                                     8
#define MBOX_133_F2_SHIFT                                                    16
#define MBOX_133_F2_MASK                                             0x00ff0000
#define MBOX_133_F2_RD(src)                          (((src) & 0x00ff0000)>>16)
#define MBOX_133_F2_SET(dst,src) \
                      (((dst) & ~0x00ff0000) | (((u32)(src)<<16) & 0x00ff0000))
/*	 Fields mbox_14	 */
#define MBOX_143_F2_WIDTH                                                     8
#define MBOX_143_F2_SHIFT                                                     8
#define MBOX_143_F2_MASK                                             0x0000ff00
#define MBOX_143_F2_RD(src)                           (((src) & 0x0000ff00)>>8)
#define MBOX_143_F2_SET(dst,src) \
                       (((dst) & ~0x0000ff00) | (((u32)(src)<<8) & 0x0000ff00))
/*	 Fields mbox_15	 */
#define MBOX_153_F2_WIDTH                                                     8
#define MBOX_153_F2_SHIFT                                                     0
#define MBOX_153_F2_MASK                                             0x000000ff
#define MBOX_153_F2_RD(src)                              (((src) & 0x000000ff))
#define MBOX_153_F2_SET(dst,src) \
                          (((dst) & ~0x000000ff) | (((u32)(src)) & 0x000000ff))

/*	Register qm_INT	*/ 
/*	 Fields queue_not_empty	 */
#define QUEUE_NOT_EMPTY_WIDTH                                                 1
#define QUEUE_NOT_EMPTY_SHIFT                                                31
#define QUEUE_NOT_EMPTY_MASK                                         0x80000000
#define QUEUE_NOT_EMPTY_RD(src)                      (((src) & 0x80000000)>>31)
#define QUEUE_NOT_EMPTY_WR(src)                 (((u32)(src)<<31) & 0x80000000)
#define QUEUE_NOT_EMPTY_SET(dst,src) \
                      (((dst) & ~0x80000000) | (((u32)(src)<<31) & 0x80000000))
/*	 Fields interrupting_csr	 */
#define INTERRUPTING_CSR_WIDTH                                                8
#define INTERRUPTING_CSR_SHIFT                                               16
#define INTERRUPTING_CSR_MASK                                        0x00ff0000
#define INTERRUPTING_CSR_RD(src)                     (((src) & 0x00ff0000)>>16)
#define INTERRUPTING_CSR_WR(src)                (((u32)(src)<<16) & 0x00ff0000)
#define INTERRUPTING_CSR_SET(dst,src) \
                      (((dst) & ~0x00ff0000) | (((u32)(src)<<16) & 0x00ff0000))
/*	 Fields qpcore_acr_error	 */
#define QPCORE_ACR_ERROR_F2_WIDTH                                             1
#define QPCORE_ACR_ERROR_F2_SHIFT                                             2
#define QPCORE_ACR_ERROR_F2_MASK                                     0x00000004
#define QPCORE_ACR_ERROR_F2_RD(src)                   (((src) & 0x00000004)>>2)
#define QPCORE_ACR_ERROR_F2_WR(src)              (((u32)(src)<<2) & 0x00000004)
#define QPCORE_ACR_ERROR_F2_SET(dst,src) \
                       (((dst) & ~0x00000004) | (((u32)(src)<<2) & 0x00000004))
/*	 Fields deq_axi_error	 */
#define DEQ_AXI_ERROR_F2_WIDTH                                                1
#define DEQ_AXI_ERROR_F2_SHIFT                                                1
#define DEQ_AXI_ERROR_F2_MASK                                        0x00000002
#define DEQ_AXI_ERROR_F2_RD(src)                      (((src) & 0x00000002)>>1)
#define DEQ_AXI_ERROR_F2_WR(src)                 (((u32)(src)<<1) & 0x00000002)
#define DEQ_AXI_ERROR_F2_SET(dst,src) \
                       (((dst) & ~0x00000002) | (((u32)(src)<<1) & 0x00000002))
/*	 Fields pbm_dec_error	 */
#define PBM_DEC_ERROR_F2_WIDTH                                                1
#define PBM_DEC_ERROR_F2_SHIFT                                                0
#define PBM_DEC_ERROR_F2_MASK                                        0x00000001
#define PBM_DEC_ERROR_F2_RD(src)                         (((src) & 0x00000001))
#define PBM_DEC_ERROR_F2_WR(src)                    (((u32)(src)) & 0x00000001)
#define PBM_DEC_ERROR_F2_SET(dst,src) \
                          (((dst) & ~0x00000001) | (((u32)(src)) & 0x00000001))

/*	Register qm_interruptMask	*/
/*    Mask Register Fields queue_not_emptyMask    */
#define QUEUE_NOT_EMPTYMASK_WIDTH                                             1
#define QUEUE_NOT_EMPTYMASK_SHIFT                                            31
#define QUEUE_NOT_EMPTYMASK_MASK                                     0x80000000
#define QUEUE_NOT_EMPTYMASK_RD(src)                  (((src) & 0x80000000)>>31)
#define QUEUE_NOT_EMPTYMASK_WR(src)             (((u32)(src)<<31) & 0x80000000)
#define QUEUE_NOT_EMPTYMASK_SET(dst,src) \
                      (((dst) & ~0x80000000) | (((u32)(src)<<31) & 0x80000000))
/*    Mask Register Fields interrupting_csrMask    */
#define INTERRUPTING_CSRMASK_WIDTH                                            8
#define INTERRUPTING_CSRMASK_SHIFT                                           16
#define INTERRUPTING_CSRMASK_MASK                                    0x00ff0000
#define INTERRUPTING_CSRMASK_RD(src)                 (((src) & 0x00ff0000)>>16)
#define INTERRUPTING_CSRMASK_WR(src)            (((u32)(src)<<16) & 0x00ff0000)
#define INTERRUPTING_CSRMASK_SET(dst,src) \
                      (((dst) & ~0x00ff0000) | (((u32)(src)<<16) & 0x00ff0000))
/*    Mask Register Fields qpcore_acr_errorMask    */
#define QPCORE_ACR_ERRORMASK_F2_WIDTH                                         1
#define QPCORE_ACR_ERRORMASK_F2_SHIFT                                         2
#define QPCORE_ACR_ERRORMASK_F2_MASK                                 0x00000004
#define QPCORE_ACR_ERRORMASK_F2_RD(src)               (((src) & 0x00000004)>>2)
#define QPCORE_ACR_ERRORMASK_F2_WR(src)          (((u32)(src)<<2) & 0x00000004)
#define QPCORE_ACR_ERRORMASK_F2_SET(dst,src) \
                       (((dst) & ~0x00000004) | (((u32)(src)<<2) & 0x00000004))
/*    Mask Register Fields deq_axi_errorMask    */
#define DEQ_AXI_ERRORMASK_F2_WIDTH                                            1
#define DEQ_AXI_ERRORMASK_F2_SHIFT                                            1
#define DEQ_AXI_ERRORMASK_F2_MASK                                    0x00000002
#define DEQ_AXI_ERRORMASK_F2_RD(src)                  (((src) & 0x00000002)>>1)
#define DEQ_AXI_ERRORMASK_F2_WR(src)             (((u32)(src)<<1) & 0x00000002)
#define DEQ_AXI_ERRORMASK_F2_SET(dst,src) \
                       (((dst) & ~0x00000002) | (((u32)(src)<<1) & 0x00000002))
/*    Mask Register Fields pbm_dec_errorMask    */
#define PBM_DEC_ERRORMASK_F2_WIDTH                                            1
#define PBM_DEC_ERRORMASK_F2_SHIFT                                            0
#define PBM_DEC_ERRORMASK_F2_MASK                                    0x00000001
#define PBM_DEC_ERRORMASK_F2_RD(src)                     (((src) & 0x00000001))
#define PBM_DEC_ERRORMASK_F2_WR(src)                (((u32)(src)) & 0x00000001)
#define PBM_DEC_ERRORMASK_F2_SET(dst,src) \
                          (((dst) & ~0x00000001) | (((u32)(src)) & 0x00000001))

/*	Register csr_pbm_errinf	*/ 
/*	 Fields pbm_config_err	 */
#define PBM_CONFIG_ERR_WIDTH                                                  1
#define PBM_CONFIG_ERR_SHIFT                                                 17
#define PBM_CONFIG_ERR_MASK                                          0x00020000
#define PBM_CONFIG_ERR_RD(src)                       (((src) & 0x00020000)>>17)
#define PBM_CONFIG_ERR_SET(dst,src) \
                      (((dst) & ~0x00020000) | (((u32)(src)<<17) & 0x00020000))
/*	 Fields pbm_decrement_err	 */
#define PBM_DECREMENT_ERR_F2_WIDTH                                            1
#define PBM_DECREMENT_ERR_F2_SHIFT                                           16
#define PBM_DECREMENT_ERR_F2_MASK                                    0x00010000
#define PBM_DECREMENT_ERR_F2_RD(src)                 (((src) & 0x00010000)>>16)
#define PBM_DECREMENT_ERR_F2_SET(dst,src) \
                      (((dst) & ~0x00010000) | (((u32)(src)<<16) & 0x00010000))
/*	 Fields slave_id	 */
#define SLAVE_ID_F2_WIDTH                                                     4
#define SLAVE_ID_F2_SHIFT                                                     6
#define SLAVE_ID_F2_MASK                                             0x000003c0
#define SLAVE_ID_F2_RD(src)                           (((src) & 0x000003c0)>>6)
#define SLAVE_ID_F2_SET(dst,src) \
                       (((dst) & ~0x000003c0) | (((u32)(src)<<6) & 0x000003c0))
/*	 Fields PB_number	 */
#define PB_NUMBER_F2_WIDTH                                                    6
#define PB_NUMBER_F2_SHIFT                                                    0
#define PB_NUMBER_F2_MASK                                            0x0000003f
#define PB_NUMBER_F2_RD(src)                             (((src) & 0x0000003f))
#define PB_NUMBER_F2_SET(dst,src) \
                          (((dst) & ~0x0000003f) | (((u32)(src)) & 0x0000003f))

/*	Register csr_msgrd_errinf	*/ 
/*	 Fields qpcore_was_disabled	 */
#define QPCORE_WAS_DISABLED_F2_WIDTH                                          1
#define QPCORE_WAS_DISABLED_F2_SHIFT                                         27
#define QPCORE_WAS_DISABLED_F2_MASK                                  0x08000000
#define QPCORE_WAS_DISABLED_F2_RD(src)               (((src) & 0x08000000)>>27)
#define QPCORE_WAS_DISABLED_F2_SET(dst,src) \
                      (((dst) & ~0x08000000) | (((u32)(src)<<27) & 0x08000000))
/*	 Fields drop_code	 */
#define DROP_CODE_F2_WIDTH                                                    3
#define DROP_CODE_F2_SHIFT                                                   24
#define DROP_CODE_F2_MASK                                            0x07000000
#define DROP_CODE_F2_RD(src)                         (((src) & 0x07000000)>>24)
#define DROP_CODE_F2_SET(dst,src) \
                      (((dst) & ~0x07000000) | (((u32)(src)<<24) & 0x07000000))
/*	 Fields acr_qid	 */
#define ACR_QID_F2_WIDTH                                                      8
#define ACR_QID_F2_SHIFT                                                     16
#define ACR_QID_F2_MASK                                              0x00ff0000
#define ACR_QID_F2_RD(src)                           (((src) & 0x00ff0000)>>16)
#define ACR_QID_F2_SET(dst,src) \
                      (((dst) & ~0x00ff0000) | (((u32)(src)<<16) & 0x00ff0000))
/*	 Fields response_code	 */
#define RESPONSE_CODE_F2_WIDTH                                                2
#define RESPONSE_CODE_F2_SHIFT                                                8
#define RESPONSE_CODE_F2_MASK                                        0x00000300
#define RESPONSE_CODE_F2_RD(src)                      (((src) & 0x00000300)>>8)
#define RESPONSE_CODE_F2_SET(dst,src) \
                       (((dst) & ~0x00000300) | (((u32)(src)<<8) & 0x00000300))
/*	 Fields qid	 */
#define QID_F2_WIDTH                                                          8
#define QID_F2_SHIFT                                                          0
#define QID_F2_MASK                                                  0x000000ff
#define QID_F2_RD(src)                                   (((src) & 0x000000ff))
#define QID_F2_SET(dst,src) \
                          (((dst) & ~0x000000ff) | (((u32)(src)) & 0x000000ff))

/*	Register csr_qm_sab_ppc0	*/ 
/*	 Fields interrupting_csr	 */
#define INTERRUPTING_CSR0_WIDTH                                               8
#define INTERRUPTING_CSR0_SHIFT                                               0
#define INTERRUPTING_CSR0_MASK                                       0x000000ff
#define INTERRUPTING_CSR0_RD(src)                        (((src) & 0x000000ff))
#define INTERRUPTING_CSR0_WR(src)                   (((u32)(src)) & 0x000000ff)
#define INTERRUPTING_CSR0_SET(dst,src) \
                          (((dst) & ~0x000000ff) | (((u32)(src)) & 0x000000ff))

/*	Register csr_qm_sab_ppc0Mask	*/
/*    Mask Register Fields interrupting_csrMask    */
#define INTERRUPTING_CSRMASK_F1_WIDTH                                         8
#define INTERRUPTING_CSRMASK_F1_SHIFT                                         0
#define INTERRUPTING_CSRMASK_F1_MASK                                 0x000000ff
#define INTERRUPTING_CSRMASK_F1_RD(src)                  (((src) & 0x000000ff))
#define INTERRUPTING_CSRMASK_F1_WR(src)             (((u32)(src)) & 0x000000ff)
#define INTERRUPTING_CSRMASK_F1_SET(dst,src) \
                          (((dst) & ~0x000000ff) | (((u32)(src)) & 0x000000ff))

/*	Register csr_qm_sab_ppc1	*/ 
/*	 Fields interrupting_csr	 */
#define INTERRUPTING_CSR1_WIDTH                                               8
#define INTERRUPTING_CSR1_SHIFT                                               0
#define INTERRUPTING_CSR1_MASK                                       0x000000ff
#define INTERRUPTING_CSR1_RD(src)                        (((src) & 0x000000ff))
#define INTERRUPTING_CSR1_WR(src)                   (((u32)(src)) & 0x000000ff)
#define INTERRUPTING_CSR1_SET(dst,src) \
                          (((dst) & ~0x000000ff) | (((u32)(src)) & 0x000000ff))

/*	Register csr_qm_sab_ppc1Mask	*/
/*    Mask Register Fields interrupting_csrMask    */
#define INTERRUPTING_CSRMASK_F2_WIDTH                                         8
#define INTERRUPTING_CSRMASK_F2_SHIFT                                         0
#define INTERRUPTING_CSRMASK_F2_MASK                                 0x000000ff
#define INTERRUPTING_CSRMASK_F2_RD(src)                  (((src) & 0x000000ff))
#define INTERRUPTING_CSRMASK_F2_WR(src)             (((u32)(src)) & 0x000000ff)
#define INTERRUPTING_CSRMASK_F2_SET(dst,src) \
                          (((dst) & ~0x000000ff) | (((u32)(src)) & 0x000000ff))

/*	Register csr_qm_mbox_ne	*/ 
/*	 Fields mbox_0_not_empty	 */
#define MBOX_0_NOT_EMPTY_WIDTH                                                1
#define MBOX_0_NOT_EMPTY_SHIFT                                               31
#define MBOX_0_NOT_EMPTY_MASK                                        0x80000000
#define MBOX_0_NOT_EMPTY_RD(src)                     (((src) & 0x80000000)>>31)
#define MBOX_0_NOT_EMPTY_SET(dst,src) \
                      (((dst) & ~0x80000000) | (((u32)(src)<<31) & 0x80000000))
/*	 Fields mbox_1_not_empty	 */
#define MBOX_1_NOT_EMPTY_WIDTH                                                1
#define MBOX_1_NOT_EMPTY_SHIFT                                               30
#define MBOX_1_NOT_EMPTY_MASK                                        0x40000000
#define MBOX_1_NOT_EMPTY_RD(src)                     (((src) & 0x40000000)>>30)
#define MBOX_1_NOT_EMPTY_SET(dst,src) \
                      (((dst) & ~0x40000000) | (((u32)(src)<<30) & 0x40000000))
/*	 Fields mbox_2_not_empty	 */
#define MBOX_2_NOT_EMPTY_WIDTH                                                1
#define MBOX_2_NOT_EMPTY_SHIFT                                               29
#define MBOX_2_NOT_EMPTY_MASK                                        0x20000000
#define MBOX_2_NOT_EMPTY_RD(src)                     (((src) & 0x20000000)>>29)
#define MBOX_2_NOT_EMPTY_SET(dst,src) \
                      (((dst) & ~0x20000000) | (((u32)(src)<<29) & 0x20000000))
/*	 Fields mbox_3_not_empty	 */
#define MBOX_3_NOT_EMPTY_WIDTH                                                1
#define MBOX_3_NOT_EMPTY_SHIFT                                               28
#define MBOX_3_NOT_EMPTY_MASK                                        0x10000000
#define MBOX_3_NOT_EMPTY_RD(src)                     (((src) & 0x10000000)>>28)
#define MBOX_3_NOT_EMPTY_SET(dst,src) \
                      (((dst) & ~0x10000000) | (((u32)(src)<<28) & 0x10000000))
/*	 Fields mbox_4_not_empty	 */
#define MBOX_4_NOT_EMPTY_WIDTH                                                1
#define MBOX_4_NOT_EMPTY_SHIFT                                               27
#define MBOX_4_NOT_EMPTY_MASK                                        0x08000000
#define MBOX_4_NOT_EMPTY_RD(src)                     (((src) & 0x08000000)>>27)
#define MBOX_4_NOT_EMPTY_SET(dst,src) \
                      (((dst) & ~0x08000000) | (((u32)(src)<<27) & 0x08000000))
/*	 Fields mbox_5_not_empty	 */
#define MBOX_5_NOT_EMPTY_WIDTH                                                1
#define MBOX_5_NOT_EMPTY_SHIFT                                               26
#define MBOX_5_NOT_EMPTY_MASK                                        0x04000000
#define MBOX_5_NOT_EMPTY_RD(src)                     (((src) & 0x04000000)>>26)
#define MBOX_5_NOT_EMPTY_SET(dst,src) \
                      (((dst) & ~0x04000000) | (((u32)(src)<<26) & 0x04000000))
/*	 Fields mbox_6_not_empty	 */
#define MBOX_6_NOT_EMPTY_WIDTH                                                1
#define MBOX_6_NOT_EMPTY_SHIFT                                               25
#define MBOX_6_NOT_EMPTY_MASK                                        0x02000000
#define MBOX_6_NOT_EMPTY_RD(src)                     (((src) & 0x02000000)>>25)
#define MBOX_6_NOT_EMPTY_SET(dst,src) \
                      (((dst) & ~0x02000000) | (((u32)(src)<<25) & 0x02000000))
/*	 Fields mbox_7_not_empty	 */
#define MBOX_7_NOT_EMPTY_WIDTH                                                1
#define MBOX_7_NOT_EMPTY_SHIFT                                               24
#define MBOX_7_NOT_EMPTY_MASK                                        0x01000000
#define MBOX_7_NOT_EMPTY_RD(src)                     (((src) & 0x01000000)>>24)
#define MBOX_7_NOT_EMPTY_SET(dst,src) \
                      (((dst) & ~0x01000000) | (((u32)(src)<<24) & 0x01000000))
/*	 Fields mbox_8_not_empty	 */
#define MBOX_8_NOT_EMPTY_WIDTH                                                1
#define MBOX_8_NOT_EMPTY_SHIFT                                               23
#define MBOX_8_NOT_EMPTY_MASK                                        0x00800000
#define MBOX_8_NOT_EMPTY_RD(src)                     (((src) & 0x00800000)>>23)
#define MBOX_8_NOT_EMPTY_SET(dst,src) \
                      (((dst) & ~0x00800000) | (((u32)(src)<<23) & 0x00800000))
/*	 Fields mbox_9_not_empty	 */
#define MBOX_9_NOT_EMPTY_WIDTH                                                1
#define MBOX_9_NOT_EMPTY_SHIFT                                               22
#define MBOX_9_NOT_EMPTY_MASK                                        0x00400000
#define MBOX_9_NOT_EMPTY_RD(src)                     (((src) & 0x00400000)>>22)
#define MBOX_9_NOT_EMPTY_SET(dst,src) \
                      (((dst) & ~0x00400000) | (((u32)(src)<<22) & 0x00400000))
/*	 Fields mbox_10_not_empty	 */
#define MBOX_10_NOT_EMPTY_WIDTH                                               1
#define MBOX_10_NOT_EMPTY_SHIFT                                              21
#define MBOX_10_NOT_EMPTY_MASK                                       0x00200000
#define MBOX_10_NOT_EMPTY_RD(src)                    (((src) & 0x00200000)>>21)
#define MBOX_10_NOT_EMPTY_SET(dst,src) \
                      (((dst) & ~0x00200000) | (((u32)(src)<<21) & 0x00200000))
/*	 Fields mbox_11_not_empty	 */
#define MBOX_11_NOT_EMPTY_WIDTH                                               1
#define MBOX_11_NOT_EMPTY_SHIFT                                              20
#define MBOX_11_NOT_EMPTY_MASK                                       0x00100000
#define MBOX_11_NOT_EMPTY_RD(src)                    (((src) & 0x00100000)>>20)
#define MBOX_11_NOT_EMPTY_SET(dst,src) \
                      (((dst) & ~0x00100000) | (((u32)(src)<<20) & 0x00100000))
/*	 Fields mbox_12_not_empty	 */
#define MBOX_12_NOT_EMPTY_WIDTH                                               1
#define MBOX_12_NOT_EMPTY_SHIFT                                              19
#define MBOX_12_NOT_EMPTY_MASK                                       0x00080000
#define MBOX_12_NOT_EMPTY_RD(src)                    (((src) & 0x00080000)>>19)
#define MBOX_12_NOT_EMPTY_SET(dst,src) \
                      (((dst) & ~0x00080000) | (((u32)(src)<<19) & 0x00080000))
/*	 Fields mbox_13_not_empty	 */
#define MBOX_13_NOT_EMPTY_WIDTH                                               1
#define MBOX_13_NOT_EMPTY_SHIFT                                              18
#define MBOX_13_NOT_EMPTY_MASK                                       0x00040000
#define MBOX_13_NOT_EMPTY_RD(src)                    (((src) & 0x00040000)>>18)
#define MBOX_13_NOT_EMPTY_SET(dst,src) \
                      (((dst) & ~0x00040000) | (((u32)(src)<<18) & 0x00040000))
/*	 Fields mbox_14_not_empty	 */
#define MBOX_14_NOT_EMPTY_WIDTH                                               1
#define MBOX_14_NOT_EMPTY_SHIFT                                              17
#define MBOX_14_NOT_EMPTY_MASK                                       0x00020000
#define MBOX_14_NOT_EMPTY_RD(src)                    (((src) & 0x00020000)>>17)
#define MBOX_14_NOT_EMPTY_SET(dst,src) \
                      (((dst) & ~0x00020000) | (((u32)(src)<<17) & 0x00020000))
/*	 Fields mbox_15_not_empty	 */
#define MBOX_15_NOT_EMPTY_WIDTH                                               1
#define MBOX_15_NOT_EMPTY_SHIFT                                              16
#define MBOX_15_NOT_EMPTY_MASK                                       0x00010000
#define MBOX_15_NOT_EMPTY_RD(src)                    (((src) & 0x00010000)>>16)
#define MBOX_15_NOT_EMPTY_SET(dst,src) \
                      (((dst) & ~0x00010000) | (((u32)(src)<<16) & 0x00010000))

/*	Register csr_ppc_sab0	*/ 
/*	 Fields queue_critical	 */
#define QUEUE_CRITICAL0_WIDTH                                                32
#define QUEUE_CRITICAL0_SHIFT                                                 0
#define QUEUE_CRITICAL0_MASK                                         0xffffffff
#define QUEUE_CRITICAL0_RD(src)                          (((src) & 0xffffffff))
#define QUEUE_CRITICAL0_SET(dst,src) \
                          (((dst) & ~0xffffffff) | (((u32)(src)) & 0xffffffff))

/*	Register csr_ppc_sab1	*/ 
/*	 Fields queue_critical	 */
#define QUEUE_CRITICAL1_WIDTH                                                32
#define QUEUE_CRITICAL1_SHIFT                                                 0
#define QUEUE_CRITICAL1_MASK                                         0xffffffff
#define QUEUE_CRITICAL1_RD(src)                          (((src) & 0xffffffff))
#define QUEUE_CRITICAL1_SET(dst,src) \
                          (((dst) & ~0xffffffff) | (((u32)(src)) & 0xffffffff))

/*	Register csr_ppc_sab2	*/ 
/*	 Fields queue_critical	 */
#define QUEUE_CRITICAL2_WIDTH                                                32
#define QUEUE_CRITICAL2_SHIFT                                                 0
#define QUEUE_CRITICAL2_MASK                                         0xffffffff
#define QUEUE_CRITICAL2_RD(src)                          (((src) & 0xffffffff))
#define QUEUE_CRITICAL2_SET(dst,src) \
                          (((dst) & ~0xffffffff) | (((u32)(src)) & 0xffffffff))

/*	Register csr_ppc_sab3	*/ 
/*	 Fields queue_critical	 */
#define QUEUE_CRITICAL3_WIDTH                                                32
#define QUEUE_CRITICAL3_SHIFT                                                 0
#define QUEUE_CRITICAL3_MASK                                         0xffffffff
#define QUEUE_CRITICAL3_RD(src)                          (((src) & 0xffffffff))
#define QUEUE_CRITICAL3_SET(dst,src) \
                          (((dst) & ~0xffffffff) | (((u32)(src)) & 0xffffffff))

/*	Register csr_ppc_sab4	*/ 
/*	 Fields queue_critical	 */
#define QUEUE_CRITICAL4_WIDTH                                                32
#define QUEUE_CRITICAL4_SHIFT                                                 0
#define QUEUE_CRITICAL4_MASK                                         0xffffffff
#define QUEUE_CRITICAL4_RD(src)                          (((src) & 0xffffffff))
#define QUEUE_CRITICAL4_SET(dst,src) \
                          (((dst) & ~0xffffffff) | (((u32)(src)) & 0xffffffff))

/*	Register csr_ppc_sab5	*/ 
/*	 Fields queue_critical	 */
#define QUEUE_CRITICAL5_WIDTH                                                32
#define QUEUE_CRITICAL5_SHIFT                                                 0
#define QUEUE_CRITICAL5_MASK                                         0xffffffff
#define QUEUE_CRITICAL5_RD(src)                          (((src) & 0xffffffff))
#define QUEUE_CRITICAL5_SET(dst,src) \
                          (((dst) & ~0xffffffff) | (((u32)(src)) & 0xffffffff))

/*	Register csr_ppc_sab6	*/ 
/*	 Fields queue_critical	 */
#define QUEUE_CRITICAL6_WIDTH                                                32
#define QUEUE_CRITICAL6_SHIFT                                                 0
#define QUEUE_CRITICAL6_MASK                                         0xffffffff
#define QUEUE_CRITICAL6_RD(src)                          (((src) & 0xffffffff))
#define QUEUE_CRITICAL6_SET(dst,src) \
                          (((dst) & ~0xffffffff) | (((u32)(src)) & 0xffffffff))

/*	Register csr_ppc_sab7	*/ 
/*	 Fields queue_critical	 */
#define QUEUE_CRITICAL7_WIDTH                                                32
#define QUEUE_CRITICAL7_SHIFT                                                 0
#define QUEUE_CRITICAL7_MASK                                         0xffffffff
#define QUEUE_CRITICAL7_RD(src)                          (((src) & 0xffffffff))
#define QUEUE_CRITICAL7_SET(dst,src) \
                          (((dst) & ~0xffffffff) | (((u32)(src)) & 0xffffffff))

/*	Register csr_pb_slave6	*/ 
/*	 Fields base_addr	 */
#define BASE_ADDR6_F2_WIDTH                                                  24
#define BASE_ADDR6_F2_SHIFT                                                   0
#define BASE_ADDR6_F2_MASK                                           0x00ffffff
#define BASE_ADDR6_F2_RD(src)                            (((src) & 0x00ffffff))
#define BASE_ADDR6_F2_WR(src)                       (((u32)(src)) & 0x00ffffff)
#define BASE_ADDR6_F2_SET(dst,src) \
                          (((dst) & ~0x00ffffff) | (((u32)(src)) & 0x00ffffff))

/*	Register csr_pb_slave7	*/ 
/*	 Fields base_addr	 */
#define BASE_ADDR7_F2_WIDTH                                                  24
#define BASE_ADDR7_F2_SHIFT                                                   0
#define BASE_ADDR7_F2_MASK                                           0x00ffffff
#define BASE_ADDR7_F2_RD(src)                            (((src) & 0x00ffffff))
#define BASE_ADDR7_F2_WR(src)                       (((u32)(src)) & 0x00ffffff)
#define BASE_ADDR7_F2_SET(dst,src) \
                          (((dst) & ~0x00ffffff) | (((u32)(src)) & 0x00ffffff))

/*	Register csr_qm_stats_cfg	*/ 
/*	 Fields qid_enq_counter	 */
#define QID_ENQ_COUNTER_F2_WIDTH                                              8
#define QID_ENQ_COUNTER_F2_SHIFT                                             24
#define QID_ENQ_COUNTER_F2_MASK                                      0xff000000
#define QID_ENQ_COUNTER_F2_RD(src)                   (((src) & 0xff000000)>>24)
#define QID_ENQ_COUNTER_F2_WR(src)              (((u32)(src)<<24) & 0xff000000)
#define QID_ENQ_COUNTER_F2_SET(dst,src) \
                      (((dst) & ~0xff000000) | (((u32)(src)<<24) & 0xff000000))
/*	 Fields qid_deq_counter	 */
#define QID_DEQ_COUNTER_F2_WIDTH                                              8
#define QID_DEQ_COUNTER_F2_SHIFT                                             16
#define QID_DEQ_COUNTER_F2_MASK                                      0x00ff0000
#define QID_DEQ_COUNTER_F2_RD(src)                   (((src) & 0x00ff0000)>>16)
#define QID_DEQ_COUNTER_F2_WR(src)              (((u32)(src)<<16) & 0x00ff0000)
#define QID_DEQ_COUNTER_F2_SET(dst,src) \
                      (((dst) & ~0x00ff0000) | (((u32)(src)<<16) & 0x00ff0000))

/*	Register csr_enq_statistics	*/ 
/*	 Fields enq_count	 */
#define ENQ_COUNT_F2_WIDTH                                                   32
#define ENQ_COUNT_F2_SHIFT                                                    0
#define ENQ_COUNT_F2_MASK                                            0xffffffff
#define ENQ_COUNT_F2_RD(src)                             (((src) & 0xffffffff))
#define ENQ_COUNT_F2_SET(dst,src) \
                          (((dst) & ~0xffffffff) | (((u32)(src)) & 0xffffffff))

/*	Register csr_deq_statistics	*/ 
/*	 Fields deq_count	 */
#define DEQ_COUNT_F2_WIDTH                                                   32
#define DEQ_COUNT_F2_SHIFT                                                    0
#define DEQ_COUNT_F2_MASK                                            0xffffffff
#define DEQ_COUNT_F2_RD(src)                             (((src) & 0xffffffff))
#define DEQ_COUNT_F2_SET(dst,src) \
                          (((dst) & ~0xffffffff) | (((u32)(src)) & 0xffffffff))

/*	Register csr_fifo_status	*/ 
/*	 Fields csr_epoch_overrun	 */
#define CSR_EPOCH_OVERRUN_WIDTH                                               1
#define CSR_EPOCH_OVERRUN_SHIFT                                              31
#define CSR_EPOCH_OVERRUN_MASK                                       0x80000000
#define CSR_EPOCH_OVERRUN_RD(src)                    (((src) & 0x80000000)>>31)
#define CSR_EPOCH_OVERRUN_SET(dst,src) \
                      (((dst) & ~0x80000000) | (((u32)(src)<<31) & 0x80000000))
/*	 Fields csr_qpcore_active_process	 */
#define CSR_QPCORE_ACTIVE_PROCESS_F2_WIDTH                                    5
#define CSR_QPCORE_ACTIVE_PROCESS_F2_SHIFT                                   22
#define CSR_QPCORE_ACTIVE_PROCESS_F2_MASK                            0x07c00000
#define CSR_QPCORE_ACTIVE_PROCESS_F2_RD(src)         (((src) & 0x07c00000)>>22)
#define CSR_QPCORE_ACTIVE_PROCESS_F2_SET(dst,src) \
                      (((dst) & ~0x07c00000) | (((u32)(src)<<22) & 0x07c00000))
/*	 Fields csr_msgwr_outstanding_req	 */
#define CSR_MSGWR_OUTSTANDING_REQ_F2_WIDTH                                    3
#define CSR_MSGWR_OUTSTANDING_REQ_F2_SHIFT                                   19
#define CSR_MSGWR_OUTSTANDING_REQ_F2_MASK                            0x00380000
#define CSR_MSGWR_OUTSTANDING_REQ_F2_RD(src)         (((src) & 0x00380000)>>19)
#define CSR_MSGWR_OUTSTANDING_REQ_F2_SET(dst,src) \
                      (((dst) & ~0x00380000) | (((u32)(src)<<19) & 0x00380000))
/*	 Fields csr_msgrd_fifo_stat	 */
#define CSR_MSGRD_FIFO_STAT_F2_WIDTH                                          7
#define CSR_MSGRD_FIFO_STAT_F2_SHIFT                                         12
#define CSR_MSGRD_FIFO_STAT_F2_MASK                                  0x0007f000
#define CSR_MSGRD_FIFO_STAT_F2_RD(src)               (((src) & 0x0007f000)>>12)
#define CSR_MSGRD_FIFO_STAT_F2_SET(dst,src) \
                      (((dst) & ~0x0007f000) | (((u32)(src)<<12) & 0x0007f000))
/*	 Fields csr_acr_mstr_cmd_level	 */
#define CSR_ACR_MSTR_CMD_LEVEL_F2_WIDTH                                       4
#define CSR_ACR_MSTR_CMD_LEVEL_F2_SHIFT                                       8
#define CSR_ACR_MSTR_CMD_LEVEL_F2_MASK                               0x00000f00
#define CSR_ACR_MSTR_CMD_LEVEL_F2_RD(src)             (((src) & 0x00000f00)>>8)
#define CSR_ACR_MSTR_CMD_LEVEL_F2_SET(dst,src) \
                       (((dst) & ~0x00000f00) | (((u32)(src)<<8) & 0x00000f00))
/*	 Fields csr_acr_allocated	 */
#define CSR_ACR_ALLOCATED_F2_WIDTH                                            8
#define CSR_ACR_ALLOCATED_F2_SHIFT                                            0
#define CSR_ACR_ALLOCATED_F2_MASK                                    0x000000ff
#define CSR_ACR_ALLOCATED_F2_RD(src)                     (((src) & 0x000000ff))
#define CSR_ACR_ALLOCATED_F2_SET(dst,src) \
                          (((dst) & ~0x000000ff) | (((u32)(src)) & 0x000000ff))

/*	Register csr_qm_ram_margin	*/ 
/*	 Fields qstate_rmea	 */
#define QSTATE_RMEA_WIDTH                                                     1
#define QSTATE_RMEA_SHIFT                                                    13
#define QSTATE_RMEA_MASK                                             0x00002000
#define QSTATE_RMEA_RD(src)                          (((src) & 0x00002000)>>13)
#define QSTATE_RMEA_WR(src)                     (((u32)(src)<<13) & 0x00002000)
#define QSTATE_RMEA_SET(dst,src) \
                      (((dst) & ~0x00002000) | (((u32)(src)<<13) & 0x00002000))
/*	 Fields qstate_rmeb	 */
#define QSTATE_RMEB_WIDTH                                                     1
#define QSTATE_RMEB_SHIFT                                                    12
#define QSTATE_RMEB_MASK                                             0x00001000
#define QSTATE_RMEB_RD(src)                          (((src) & 0x00001000)>>12)
#define QSTATE_RMEB_WR(src)                     (((u32)(src)<<12) & 0x00001000)
#define QSTATE_RMEB_SET(dst,src) \
                      (((dst) & ~0x00001000) | (((u32)(src)<<12) & 0x00001000))
/*	 Fields qstate_rma	 */
#define QSTATE_RMA_WIDTH                                                      2
#define QSTATE_RMA_SHIFT                                                     10
#define QSTATE_RMA_MASK                                              0x00000c00
#define QSTATE_RMA_RD(src)                           (((src) & 0x00000c00)>>10)
#define QSTATE_RMA_WR(src)                      (((u32)(src)<<10) & 0x00000c00)
#define QSTATE_RMA_SET(dst,src) \
                      (((dst) & ~0x00000c00) | (((u32)(src)<<10) & 0x00000c00))
/*	 Fields qstate_rmb	 */
#define QSTATE_RMB_WIDTH                                                      2
#define QSTATE_RMB_SHIFT                                                      8
#define QSTATE_RMB_MASK                                              0x00000300
#define QSTATE_RMB_RD(src)                            (((src) & 0x00000300)>>8)
#define QSTATE_RMB_WR(src)                       (((u32)(src)<<8) & 0x00000300)
#define QSTATE_RMB_SET(dst,src) \
                       (((dst) & ~0x00000300) | (((u32)(src)<<8) & 0x00000300))
/*	 Fields cstate_rmea	 */
#define CSTATE_RMEA_WIDTH                                                     1
#define CSTATE_RMEA_SHIFT                                                     5
#define CSTATE_RMEA_MASK                                             0x00000020
#define CSTATE_RMEA_RD(src)                           (((src) & 0x00000020)>>5)
#define CSTATE_RMEA_WR(src)                      (((u32)(src)<<5) & 0x00000020)
#define CSTATE_RMEA_SET(dst,src) \
                       (((dst) & ~0x00000020) | (((u32)(src)<<5) & 0x00000020))
/*	 Fields cstate_rmeb	 */
#define CSTATE_RMEB_WIDTH                                                     1
#define CSTATE_RMEB_SHIFT                                                     4
#define CSTATE_RMEB_MASK                                             0x00000010
#define CSTATE_RMEB_RD(src)                           (((src) & 0x00000010)>>4)
#define CSTATE_RMEB_WR(src)                      (((u32)(src)<<4) & 0x00000010)
#define CSTATE_RMEB_SET(dst,src) \
                       (((dst) & ~0x00000010) | (((u32)(src)<<4) & 0x00000010))
/*	 Fields cstate_rma	 */
#define CSTATE_RMA_WIDTH                                                      2
#define CSTATE_RMA_SHIFT                                                      2
#define CSTATE_RMA_MASK                                              0x0000000c
#define CSTATE_RMA_RD(src)                            (((src) & 0x0000000c)>>2)
#define CSTATE_RMA_WR(src)                       (((u32)(src)<<2) & 0x0000000c)
#define CSTATE_RMA_SET(dst,src) \
                       (((dst) & ~0x0000000c) | (((u32)(src)<<2) & 0x0000000c))
/*	 Fields cstate_rmb	 */
#define CSTATE_RMB_WIDTH                                                      2
#define CSTATE_RMB_SHIFT                                                      0
#define CSTATE_RMB_MASK                                              0x00000003
#define CSTATE_RMB_RD(src)                               (((src) & 0x00000003))
#define CSTATE_RMB_WR(src)                          (((u32)(src)) & 0x00000003)
#define CSTATE_RMB_SET(dst,src) \
                          (((dst) & ~0x00000003) | (((u32)(src)) & 0x00000003))

/*	Register csr_qm_testint	*/ 
/*	 Fields mbox	 */
#define MBOX_WIDTH                                                           16
#define MBOX_SHIFT                                                           16
#define MBOX_MASK                                                    0xffff0000
#define MBOX_RD(src)                                 (((src) & 0xffff0000)>>16)
#define MBOX_WR(src)                            (((u32)(src)<<16) & 0xffff0000)
#define MBOX_SET(dst,src) \
                      (((dst) & ~0xffff0000) | (((u32)(src)<<16) & 0xffff0000))
/*	 Fields qm_int	 */
#define QM_INT_WIDTH                                                          1
#define QM_INT_SHIFT                                                         15
#define QM_INT_MASK                                                  0x00008000
#define QM_INT_RD(src)                               (((src) & 0x00008000)>>15)
#define QM_INT_WR(src)                          (((u32)(src)<<15) & 0x00008000)
#define QM_INT_SET(dst,src) \
                      (((dst) & ~0x00008000) | (((u32)(src)<<15) & 0x00008000))
/*	 Fields sab_ppc0	 */
#define SAB_PPC0_WIDTH                                                        1
#define SAB_PPC0_SHIFT                                                       13
#define SAB_PPC0_MASK                                                0x00002000
#define SAB_PPC0_RD(src)                             (((src) & 0x00002000)>>13)
#define SAB_PPC0_WR(src)                        (((u32)(src)<<13) & 0x00002000)
#define SAB_PPC0_SET(dst,src) \
                      (((dst) & ~0x00002000) | (((u32)(src)<<13) & 0x00002000))
/*	 Fields sab_ppc1	 */
#define SAB_PPC1_WIDTH                                                        1
#define SAB_PPC1_SHIFT                                                       12
#define SAB_PPC1_MASK                                                0x00001000
#define SAB_PPC1_RD(src)                             (((src) & 0x00001000)>>12)
#define SAB_PPC1_WR(src)                        (((u32)(src)<<12) & 0x00001000)
#define SAB_PPC1_SET(dst,src) \
                      (((dst) & ~0x00001000) | (((u32)(src)<<12) & 0x00001000))
/*	 Fields ecc_sec	 */
#define ECC_SEC_WIDTH                                                         2
#define ECC_SEC_SHIFT                                                        10
#define ECC_SEC_MASK                                                 0x00000c00
#define ECC_SEC_RD(src)                              (((src) & 0x00000c00)>>10)
#define ECC_SEC_WR(src)                         (((u32)(src)<<10) & 0x00000c00)
#define ECC_SEC_SET(dst,src) \
                      (((dst) & ~0x00000c00) | (((u32)(src)<<10) & 0x00000c00))
/*	 Fields ecc_ded	 */
#define ECC_DED_WIDTH                                                         2
#define ECC_DED_SHIFT                                                         8
#define ECC_DED_MASK                                                 0x00000300
#define ECC_DED_RD(src)                               (((src) & 0x00000300)>>8)
#define ECC_DED_WR(src)                          (((u32)(src)<<8) & 0x00000300)
#define ECC_DED_SET(dst,src) \
                       (((dst) & ~0x00000300) | (((u32)(src)<<8) & 0x00000300))
	


#endif  
