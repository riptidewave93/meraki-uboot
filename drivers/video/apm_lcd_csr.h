/*
 * APM 86xxx LCD Header File
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
#ifndef __APMDB9XXX_H__
#define __APMDB9XXX_H__

/* Global Base Address	*/
#define LCD_CSR_BASE_ADDR		0xddf440000ULL
	
/* Address LCD_CSR  Registers */		
#define START_ADDR0_ADDR                                              0x00001000
#define END_ADDR0_ADDR                                                0x00001004
#define START_ADDR1_ADDR                                              0x00001008
#define END_ADDR1_ADDR                                                0x0000100c
#define START_ADDR2_ADDR                                              0x00001010
#define END_ADDR2_ADDR                                                0x00001014
#define START_ADDR3_ADDR                                              0x00001018
#define END_ADDR3_ADDR                                                0x0000101c
#define START_ADDR4_ADDR                                              0x00001020
#define END_ADDR4_ADDR                                                0x00001024
#define START_ADDR5_ADDR                                              0x00001028
#define END_ADDR5_ADDR                                                0x0000102c
#define START_ADDR6_ADDR                                              0x00001030
#define END_ADDR6_ADDR                                                0x00001034
#define START_ADDR7_ADDR                                              0x00001038
#define END_ADDR7_ADDR                                                0x0000103c
#define START_ADDR8_ADDR                                              0x00001040
#define END_ADDR8_ADDR                                                0x00001044
#define START_ADDR9_ADDR                                              0x00001048
#define END_ADDR9_ADDR                                                0x0000104c
#define START_ADDR10_ADDR                                             0x00001050
#define END_ADDR10_ADDR                                               0x00001054
#define START_ADDR11_ADDR                                             0x00001058
#define END_ADDR11_ADDR                                               0x0000105c
#define START_ADDR12_ADDR                                             0x00001060
#define END_ADDR12_ADDR                                               0x00001064
#define START_ADDR13_ADDR                                             0x00001068
#define END_ADDR13_ADDR                                               0x0000106c
#define START_ADDR14_ADDR                                             0x00001070
#define END_ADDR14_ADDR                                               0x00001074
#define START_ADDR15_ADDR                                             0x00001078
#define END_ADDR15_ADDR                                               0x0000107c
#define START_ADDR16_ADDR                                             0x00001080
#define END_ADDR16_ADDR                                               0x00001084
#define CORE_ID_REG_ADDR                                              0x00001090
#define DEBUG_TEST_REG_ADDR                                           0x00001094
#define LCDC_CR1_ADDR                                                 0x00000000
#define LCDC_HTR_ADDR                                                 0x00000008
#define LCDC_VTR1_ADDR                                                0x0000000c
#define LCDC_VTR2_ADDR                                                0x00000010
#define LCDC_PCTR_ADDR                                                0x00000014
#define LCDC_ISR_ADDR                                                 0x00000018
#define LCDC_IMR_ADDR                                                 0x0000001c
#define LCDC_IVR_ADDR                                                 0x00000020
#define LCDC_ISCR_ADDR                                                0x00000024
#define LCDC_DBAR_ADDR                                                0x00000028
#define LCDC_DCAR_ADDR                                                0x0000002c
#define LCDC_DEAR_ADDR                                                0x00000030
#define LCDC_PWMFR_ADDR                                               0x00000034
#define LCDC_PWMDCR_ADDR                                              0x00000038
#define LCDC_DFBAR                                                    0x0000003c
#define LCDC_DFLAR                                                    0x00000040
#define LCDC_CIR_ADDR                                                 0x000001fc
#define LCDC_PALT_ADDR                                                0x00000200
#define LCDC_PALT_SIZE_ADDR                                           0x00000c00
#define LCDC_OWER_ADDR                                                0x00000600
#define LCDC_OXSER0_ADDR                                              0x00000604
#define LCDC_OYSER0_ADDR                                              0x00000608
#define LCDC_OWDBAR0_ADDR                                             0x0000060c
#define LCDC_OWDCAR0_ADDR                                             0x00000610
#define LCDC_OXSER1_ADDR                                              0x00000614
#define LCDC_OYSER1_ADDR                                              0x00000618
#define LCDC_OWDBAR1_ADDR                                             0x0000061c
#define LCDC_OWDCAR1_ADDR                                             0x00000620
#define LCDC_OXSER2_ADDR                                              0x00000624
#define LCDC_OYSER2_ADDR                                              0x00000628
#define LCDC_OWDBAR2_ADDR                                             0x0000062c
#define LCDC_OWDCAR2_ADDR                                             0x00000630
#define LCDC_OXSER3_ADDR                                              0x00000634
#define LCDC_OYSER3_ADDR                                              0x00000638
#define LCDC_OWDBAR3_ADDR                                             0x0000063c
#define LCDC_OWDCAR3_ADDR                                             0x00000640
#define LCDC_OXSER4_ADDR                                              0x00000644
#define LCDC_OYSER4_ADDR                                              0x00000648
#define LCDC_OWDBAR4_ADDR                                             0x0000064c
#define LCDC_OWDCAR4_ADDR                                             0x00000650
#define LCDC_OXSER5_ADDR                                              0x00000654
#define LCDC_OYSER5_ADDR                                              0x00000658
#define LCDC_OWDBAR5_ADDR                                             0x0000065c
#define LCDC_OWDCAR5_ADDR                                             0x00000660
#define LCDC_OXSER6_ADDR                                              0x00000664
#define LCDC_OYSER6_ADDR                                              0x00000668
#define LCDC_OWDBAR6_ADDR                                             0x0000066c
#define LCDC_OWDCAR6_ADDR                                             0x00000670
#define LCDC_OXSER7_ADDR                                              0x00000674
#define LCDC_OYSER7_ADDR                                              0x00000678
#define LCDC_OWDBAR7_ADDR                                             0x0000067c
#define LCDC_OWDCAR7_ADDR                                             0x00000680
#define LCDC_OXSER8_ADDR                                              0x00000684
#define LCDC_OYSER8_ADDR                                              0x00000688
#define LCDC_OWDBAR8_ADDR                                             0x0000068c
#define LCDC_OWDCAR8_ADDR                                             0x00000690
#define LCDC_OXSER9_ADDR                                              0x00000694
#define LCDC_OYSER9_ADDR                                              0x00000698
#define LCDC_OWDBAR9_ADDR                                             0x0000069c
#define LCDC_OWDCAR9_ADDR                                             0x000006a0
#define LCDC_OXSER10_ADDR                                             0x000006a4
#define LCDC_OYSER10_ADDR                                             0x000006a8
#define LCDC_OWDBAR10_ADDR                                            0x000006ac
#define LCDC_OWDCAR10_ADDR                                            0x000006b0
#define LCDC_OXSER11_ADDR                                             0x000006b4
#define LCDC_OYSER11_ADDR                                             0x000006b8
#define LCDC_OWDBAR11_ADDR                                            0x000006bc
#define LCDC_OWDCAR11_ADDR                                            0x000006c0
#define LCDC_OXSER12_ADDR                                             0x000006c4
#define LCDC_OYSER12_ADDR                                             0x000006c8
#define LCDC_OWDBAR12_ADDR                                            0x000006cc
#define LCDC_OWDCAR12_ADDR                                            0x000006d0
#define LCDC_OXSER13_ADDR                                             0x000006d4
#define LCDC_OYSER13_ADDR                                             0x000006d8
#define LCDC_OWDBAR13_ADDR                                            0x000006dc
#define LCDC_OWDCAR13_ADDR                                            0x000006e0
#define LCDC_OXSER14_ADDR                                             0x000006e4
#define LCDC_OYSER14_ADDR                                             0x000006e8
#define LCDC_OWDBAR14_ADDR                                            0x000006ec
#define LCDC_OWDCAR14_ADDR                                            0x000006f0
#define LCDC_OXSER15_ADDR                                             0x000006f4
#define LCDC_OYSER15_ADDR                                             0x000006f8
#define LCDC_OWDBAR15_ADDR                                            0x000006fc
#define LCDC_OWDCAR15_ADDR                                            0x00000700

/*	Register start_addr0	*/

/*	 Fields sa	 */
#define SA0_WIDTH                                                            28
#define SA0_SHIFT                                                             4
#define SA0_MASK                                                     0xfffffff0
#define SA0_RD(src)                                  (((src) & 0xfffffff0U)>>4)
#define SA0_WR(src)                             (((u32)(src)<<4) & 0xfffffff0U)
#define SA0_SET(dst,src) \
                     (((dst) & ~0xfffffff0U) | (((u32)(src)<<4) & 0xfffffff0U))


/*	 Fields ame	 */
#define AME0_WIDTH                                                            1
#define AME0_SHIFT                                                            0
#define AME0_MASK                                                    0x00000001
#define AME0_RD(src)                                    (((src) & 0x00000001U))
#define AME0_WR(src)                               (((u32)(src)) & 0x00000001U)
#define AME0_SET(dst,src) \
                        (((dst) & ~0x00000001U) | (((u32)(src)) & 0x00000001U))


/*	Register end_addr0	*/

/*	 Fields sa	 */
#define SA0_WIDTH                                                            28
#define SA0_SHIFT                                                             4
#define SA0_MASK                                                     0xfffffff0
#define SA0_RD(src)                                  (((src) & 0xfffffff0U)>>4)
#define SA0_WR(src)                             (((u32)(src)<<4) & 0xfffffff0U)
#define SA0_SET(dst,src) \
                     (((dst) & ~0xfffffff0U) | (((u32)(src)<<4) & 0xfffffff0U))


/*	 Fields msn	 */
#define MSN0_WIDTH                                                            4
#define MSN0_SHIFT                                                            0
#define MSN0_MASK                                                    0x0000000f
#define MSN0_RD(src)                                    (((src) & 0x0000000fU))
#define MSN0_WR(src)                               (((u32)(src)) & 0x0000000fU)
#define MSN0_SET(dst,src) \
                        (((dst) & ~0x0000000fU) | (((u32)(src)) & 0x0000000fU))


/*	Register start_addr1	*/

/*	 Fields sa	 */
#define SA1_WIDTH                                                            28
#define SA1_SHIFT                                                             4
#define SA1_MASK                                                     0xfffffff0
#define SA1_RD(src)                                  (((src) & 0xfffffff0U)>>4)
#define SA1_WR(src)                             (((u32)(src)<<4) & 0xfffffff0U)
#define SA1_SET(dst,src) \
                     (((dst) & ~0xfffffff0U) | (((u32)(src)<<4) & 0xfffffff0U))


/*	 Fields ame	 */
#define AME1_WIDTH                                                            1
#define AME1_SHIFT                                                            0
#define AME1_MASK                                                    0x00000001
#define AME1_RD(src)                                    (((src) & 0x00000001U))
#define AME1_WR(src)                               (((u32)(src)) & 0x00000001U)
#define AME1_SET(dst,src) \
                        (((dst) & ~0x00000001U) | (((u32)(src)) & 0x00000001U))


/*	Register end_addr1	*/

/*	 Fields sa	 */
#define SA1_WIDTH                                                            28
#define SA1_SHIFT                                                             4
#define SA1_MASK                                                     0xfffffff0
#define SA1_RD(src)                                  (((src) & 0xfffffff0U)>>4)
#define SA1_WR(src)                             (((u32)(src)<<4) & 0xfffffff0U)
#define SA1_SET(dst,src) \
                     (((dst) & ~0xfffffff0U) | (((u32)(src)<<4) & 0xfffffff0U))


/*	 Fields msn	 */
#define MSN1_WIDTH                                                            4
#define MSN1_SHIFT                                                            0
#define MSN1_MASK                                                    0x0000000f
#define MSN1_RD(src)                                    (((src) & 0x0000000fU))
#define MSN1_WR(src)                               (((u32)(src)) & 0x0000000fU)
#define MSN1_SET(dst,src) \
                        (((dst) & ~0x0000000fU) | (((u32)(src)) & 0x0000000fU))


/*	Register start_addr2	*/

/*	 Fields sa	 */
#define SA2_WIDTH                                                            28
#define SA2_SHIFT                                                             4
#define SA2_MASK                                                     0xfffffff0
#define SA2_RD(src)                                  (((src) & 0xfffffff0U)>>4)
#define SA2_WR(src)                             (((u32)(src)<<4) & 0xfffffff0U)
#define SA2_SET(dst,src) \
                     (((dst) & ~0xfffffff0U) | (((u32)(src)<<4) & 0xfffffff0U))


/*	 Fields ame	 */
#define AME2_WIDTH                                                            1
#define AME2_SHIFT                                                            0
#define AME2_MASK                                                    0x00000001
#define AME2_RD(src)                                    (((src) & 0x00000001U))
#define AME2_WR(src)                               (((u32)(src)) & 0x00000001U)
#define AME2_SET(dst,src) \
                        (((dst) & ~0x00000001U) | (((u32)(src)) & 0x00000001U))


/*	Register end_addr2	*/

/*	 Fields sa	 */
#define SA2_WIDTH                                                            28
#define SA2_SHIFT                                                             4
#define SA2_MASK                                                     0xfffffff0
#define SA2_RD(src)                                  (((src) & 0xfffffff0U)>>4)
#define SA2_WR(src)                             (((u32)(src)<<4) & 0xfffffff0U)
#define SA2_SET(dst,src) \
                     (((dst) & ~0xfffffff0U) | (((u32)(src)<<4) & 0xfffffff0U))


/*	 Fields msn	 */
#define MSN2_WIDTH                                                            4
#define MSN2_SHIFT                                                            0
#define MSN2_MASK                                                    0x0000000f
#define MSN2_RD(src)                                    (((src) & 0x0000000fU))
#define MSN2_WR(src)                               (((u32)(src)) & 0x0000000fU)
#define MSN2_SET(dst,src) \
                        (((dst) & ~0x0000000fU) | (((u32)(src)) & 0x0000000fU))


/*	Register start_addr3	*/

/*	 Fields sa	 */
#define SA3_WIDTH                                                            28
#define SA3_SHIFT                                                             4
#define SA3_MASK                                                     0xfffffff0
#define SA3_RD(src)                                  (((src) & 0xfffffff0U)>>4)
#define SA3_WR(src)                             (((u32)(src)<<4) & 0xfffffff0U)
#define SA3_SET(dst,src) \
                     (((dst) & ~0xfffffff0U) | (((u32)(src)<<4) & 0xfffffff0U))


/*	 Fields ame	 */
#define AME3_WIDTH                                                            1
#define AME3_SHIFT                                                            0
#define AME3_MASK                                                    0x00000001
#define AME3_RD(src)                                    (((src) & 0x00000001U))
#define AME3_WR(src)                               (((u32)(src)) & 0x00000001U)
#define AME3_SET(dst,src) \
                        (((dst) & ~0x00000001U) | (((u32)(src)) & 0x00000001U))


/*	Register end_addr3	*/

/*	 Fields sa	 */
#define SA3_WIDTH                                                            28
#define SA3_SHIFT                                                             4
#define SA3_MASK                                                     0xfffffff0
#define SA3_RD(src)                                  (((src) & 0xfffffff0U)>>4)
#define SA3_WR(src)                             (((u32)(src)<<4) & 0xfffffff0U)
#define SA3_SET(dst,src) \
                     (((dst) & ~0xfffffff0U) | (((u32)(src)<<4) & 0xfffffff0U))


/*	 Fields msn	 */
#define MSN3_WIDTH                                                            4
#define MSN3_SHIFT                                                            0
#define MSN3_MASK                                                    0x0000000f
#define MSN3_RD(src)                                    (((src) & 0x0000000fU))
#define MSN3_WR(src)                               (((u32)(src)) & 0x0000000fU)
#define MSN3_SET(dst,src) \
                        (((dst) & ~0x0000000fU) | (((u32)(src)) & 0x0000000fU))


/*	Register start_addr4	*/

/*	 Fields sa	 */
#define SA4_WIDTH                                                            28
#define SA4_SHIFT                                                             4
#define SA4_MASK                                                     0xfffffff0
#define SA4_RD(src)                                  (((src) & 0xfffffff0U)>>4)
#define SA4_WR(src)                             (((u32)(src)<<4) & 0xfffffff0U)
#define SA4_SET(dst,src) \
                     (((dst) & ~0xfffffff0U) | (((u32)(src)<<4) & 0xfffffff0U))


/*	 Fields ame	 */
#define AME4_WIDTH                                                            1
#define AME4_SHIFT                                                            0
#define AME4_MASK                                                    0x00000001
#define AME4_RD(src)                                    (((src) & 0x00000001U))
#define AME4_WR(src)                               (((u32)(src)) & 0x00000001U)
#define AME4_SET(dst,src) \
                        (((dst) & ~0x00000001U) | (((u32)(src)) & 0x00000001U))


/*	Register end_addr4	*/

/*	 Fields sa	 */
#define SA4_WIDTH                                                            28
#define SA4_SHIFT                                                             4
#define SA4_MASK                                                     0xfffffff0
#define SA4_RD(src)                                  (((src) & 0xfffffff0U)>>4)
#define SA4_WR(src)                             (((u32)(src)<<4) & 0xfffffff0U)
#define SA4_SET(dst,src) \
                     (((dst) & ~0xfffffff0U) | (((u32)(src)<<4) & 0xfffffff0U))


/*	 Fields msn	 */
#define MSN4_WIDTH                                                            4
#define MSN4_SHIFT                                                            0
#define MSN4_MASK                                                    0x0000000f
#define MSN4_RD(src)                                    (((src) & 0x0000000fU))
#define MSN4_WR(src)                               (((u32)(src)) & 0x0000000fU)
#define MSN4_SET(dst,src) \
                        (((dst) & ~0x0000000fU) | (((u32)(src)) & 0x0000000fU))


/*	Register start_addr5	*/

/*	 Fields sa	 */
#define SA5_WIDTH                                                            28
#define SA5_SHIFT                                                             4
#define SA5_MASK                                                     0xfffffff0
#define SA5_RD(src)                                  (((src) & 0xfffffff0U)>>4)
#define SA5_WR(src)                             (((u32)(src)<<4) & 0xfffffff0U)
#define SA5_SET(dst,src) \
                     (((dst) & ~0xfffffff0U) | (((u32)(src)<<4) & 0xfffffff0U))


/*	 Fields ame	 */
#define AME5_WIDTH                                                            1
#define AME5_SHIFT                                                            0
#define AME5_MASK                                                    0x00000001
#define AME5_RD(src)                                    (((src) & 0x00000001U))
#define AME5_WR(src)                               (((u32)(src)) & 0x00000001U)
#define AME5_SET(dst,src) \
                        (((dst) & ~0x00000001U) | (((u32)(src)) & 0x00000001U))


/*	Register end_addr5	*/

/*	 Fields sa	 */
#define SA5_WIDTH                                                            28
#define SA5_SHIFT                                                             4
#define SA5_MASK                                                     0xfffffff0
#define SA5_RD(src)                                  (((src) & 0xfffffff0U)>>4)
#define SA5_WR(src)                             (((u32)(src)<<4) & 0xfffffff0U)
#define SA5_SET(dst,src) \
                     (((dst) & ~0xfffffff0U) | (((u32)(src)<<4) & 0xfffffff0U))


/*	 Fields msn	 */
#define MSN5_WIDTH                                                            4
#define MSN5_SHIFT                                                            0
#define MSN5_MASK                                                    0x0000000f
#define MSN5_RD(src)                                    (((src) & 0x0000000fU))
#define MSN5_WR(src)                               (((u32)(src)) & 0x0000000fU)
#define MSN5_SET(dst,src) \
                        (((dst) & ~0x0000000fU) | (((u32)(src)) & 0x0000000fU))


/*	Register start_addr6	*/

/*	 Fields sa	 */
#define SA6_WIDTH                                                            28
#define SA6_SHIFT                                                             4
#define SA6_MASK                                                     0xfffffff0
#define SA6_RD(src)                                  (((src) & 0xfffffff0U)>>4)
#define SA6_WR(src)                             (((u32)(src)<<4) & 0xfffffff0U)
#define SA6_SET(dst,src) \
                     (((dst) & ~0xfffffff0U) | (((u32)(src)<<4) & 0xfffffff0U))


/*	 Fields ame	 */
#define AME6_WIDTH                                                            1
#define AME6_SHIFT                                                            0
#define AME6_MASK                                                    0x00000001
#define AME6_RD(src)                                    (((src) & 0x00000001U))
#define AME6_WR(src)                               (((u32)(src)) & 0x00000001U)
#define AME6_SET(dst,src) \
                        (((dst) & ~0x00000001U) | (((u32)(src)) & 0x00000001U))


/*	Register end_addr6	*/

/*	 Fields sa	 */
#define SA6_WIDTH                                                            28
#define SA6_SHIFT                                                             4
#define SA6_MASK                                                     0xfffffff0
#define SA6_RD(src)                                  (((src) & 0xfffffff0U)>>4)
#define SA6_WR(src)                             (((u32)(src)<<4) & 0xfffffff0U)
#define SA6_SET(dst,src) \
                     (((dst) & ~0xfffffff0U) | (((u32)(src)<<4) & 0xfffffff0U))


/*	 Fields msn	 */
#define MSN6_WIDTH                                                            4
#define MSN6_SHIFT                                                            0
#define MSN6_MASK                                                    0x0000000f
#define MSN6_RD(src)                                    (((src) & 0x0000000fU))
#define MSN6_WR(src)                               (((u32)(src)) & 0x0000000fU)
#define MSN6_SET(dst,src) \
                        (((dst) & ~0x0000000fU) | (((u32)(src)) & 0x0000000fU))


/*	Register start_addr7	*/

/*	 Fields sa	 */
#define SA7_WIDTH                                                            28
#define SA7_SHIFT                                                             4
#define SA7_MASK                                                     0xfffffff0
#define SA7_RD(src)                                  (((src) & 0xfffffff0U)>>4)
#define SA7_WR(src)                             (((u32)(src)<<4) & 0xfffffff0U)
#define SA7_SET(dst,src) \
                     (((dst) & ~0xfffffff0U) | (((u32)(src)<<4) & 0xfffffff0U))


/*	 Fields ame	 */
#define AME7_WIDTH                                                            1
#define AME7_SHIFT                                                            0
#define AME7_MASK                                                    0x00000001
#define AME7_RD(src)                                    (((src) & 0x00000001U))
#define AME7_WR(src)                               (((u32)(src)) & 0x00000001U)
#define AME7_SET(dst,src) \
                        (((dst) & ~0x00000001U) | (((u32)(src)) & 0x00000001U))


/*	Register end_addr7	*/

/*	 Fields sa	 */
#define SA7_WIDTH                                                            28
#define SA7_SHIFT                                                             4
#define SA7_MASK                                                     0xfffffff0
#define SA7_RD(src)                                  (((src) & 0xfffffff0U)>>4)
#define SA7_WR(src)                             (((u32)(src)<<4) & 0xfffffff0U)
#define SA7_SET(dst,src) \
                     (((dst) & ~0xfffffff0U) | (((u32)(src)<<4) & 0xfffffff0U))


/*	 Fields msn	 */
#define MSN7_WIDTH                                                            4
#define MSN7_SHIFT                                                            0
#define MSN7_MASK                                                    0x0000000f
#define MSN7_RD(src)                                    (((src) & 0x0000000fU))
#define MSN7_WR(src)                               (((u32)(src)) & 0x0000000fU)
#define MSN7_SET(dst,src) \
                        (((dst) & ~0x0000000fU) | (((u32)(src)) & 0x0000000fU))


/*	Register start_addr8	*/

/*	 Fields sa	 */
#define SA8_WIDTH                                                            28
#define SA8_SHIFT                                                             4
#define SA8_MASK                                                     0xfffffff0
#define SA8_RD(src)                                  (((src) & 0xfffffff0U)>>4)
#define SA8_WR(src)                             (((u32)(src)<<4) & 0xfffffff0U)
#define SA8_SET(dst,src) \
                     (((dst) & ~0xfffffff0U) | (((u32)(src)<<4) & 0xfffffff0U))


/*	 Fields ame	 */
#define AME8_WIDTH                                                            1
#define AME8_SHIFT                                                            0
#define AME8_MASK                                                    0x00000001
#define AME8_RD(src)                                    (((src) & 0x00000001U))
#define AME8_WR(src)                               (((u32)(src)) & 0x00000001U)
#define AME8_SET(dst,src) \
                        (((dst) & ~0x00000001U) | (((u32)(src)) & 0x00000001U))


/*	Register end_addr8	*/

/*	 Fields sa	 */
#define SA8_WIDTH                                                            28
#define SA8_SHIFT                                                             4
#define SA8_MASK                                                     0xfffffff0
#define SA8_RD(src)                                  (((src) & 0xfffffff0U)>>4)
#define SA8_WR(src)                             (((u32)(src)<<4) & 0xfffffff0U)
#define SA8_SET(dst,src) \
                     (((dst) & ~0xfffffff0U) | (((u32)(src)<<4) & 0xfffffff0U))


/*	 Fields msn	 */
#define MSN8_WIDTH                                                            4
#define MSN8_SHIFT                                                            0
#define MSN8_MASK                                                    0x0000000f
#define MSN8_RD(src)                                    (((src) & 0x0000000fU))
#define MSN8_WR(src)                               (((u32)(src)) & 0x0000000fU)
#define MSN8_SET(dst,src) \
                        (((dst) & ~0x0000000fU) | (((u32)(src)) & 0x0000000fU))


/*	Register start_addr9	*/

/*	 Fields sa	 */
#define SA9_WIDTH                                                            28
#define SA9_SHIFT                                                             4
#define SA9_MASK                                                     0xfffffff0
#define SA9_RD(src)                                  (((src) & 0xfffffff0U)>>4)
#define SA9_WR(src)                             (((u32)(src)<<4) & 0xfffffff0U)
#define SA9_SET(dst,src) \
                     (((dst) & ~0xfffffff0U) | (((u32)(src)<<4) & 0xfffffff0U))


/*	 Fields ame	 */
#define AME9_WIDTH                                                            1
#define AME9_SHIFT                                                            0
#define AME9_MASK                                                    0x00000001
#define AME9_RD(src)                                    (((src) & 0x00000001U))
#define AME9_WR(src)                               (((u32)(src)) & 0x00000001U)
#define AME9_SET(dst,src) \
                        (((dst) & ~0x00000001U) | (((u32)(src)) & 0x00000001U))


/*	Register end_addr9	*/

/*	 Fields sa	 */
#define SA9_WIDTH                                                            28
#define SA9_SHIFT                                                             4
#define SA9_MASK                                                     0xfffffff0
#define SA9_RD(src)                                  (((src) & 0xfffffff0U)>>4)
#define SA9_WR(src)                             (((u32)(src)<<4) & 0xfffffff0U)
#define SA9_SET(dst,src) \
                     (((dst) & ~0xfffffff0U) | (((u32)(src)<<4) & 0xfffffff0U))


/*	 Fields msn	 */
#define MSN9_WIDTH                                                            4
#define MSN9_SHIFT                                                            0
#define MSN9_MASK                                                    0x0000000f
#define MSN9_RD(src)                                    (((src) & 0x0000000fU))
#define MSN9_WR(src)                               (((u32)(src)) & 0x0000000fU)
#define MSN9_SET(dst,src) \
                        (((dst) & ~0x0000000fU) | (((u32)(src)) & 0x0000000fU))


/*	Register start_addr10	*/

/*	 Fields sa	 */
#define SA0_WIDTH                                                            28
#define SA0_SHIFT                                                             4
#define SA0_MASK                                                     0xfffffff0
#define SA0_RD(src)                                  (((src) & 0xfffffff0U)>>4)
#define SA0_WR(src)                             (((u32)(src)<<4) & 0xfffffff0U)
#define SA0_SET(dst,src) \
                     (((dst) & ~0xfffffff0U) | (((u32)(src)<<4) & 0xfffffff0U))


/*	 Fields ame	 */
#define AME0_WIDTH                                                            1
#define AME0_SHIFT                                                            0
#define AME0_MASK                                                    0x00000001
#define AME0_RD(src)                                    (((src) & 0x00000001U))
#define AME0_WR(src)                               (((u32)(src)) & 0x00000001U)
#define AME0_SET(dst,src) \
                        (((dst) & ~0x00000001U) | (((u32)(src)) & 0x00000001U))


/*	Register end_addr10	*/

/*	 Fields sa	 */
#define SA0_WIDTH                                                            28
#define SA0_SHIFT                                                             4
#define SA0_MASK                                                     0xfffffff0
#define SA0_RD(src)                                  (((src) & 0xfffffff0U)>>4)
#define SA0_WR(src)                             (((u32)(src)<<4) & 0xfffffff0U)
#define SA0_SET(dst,src) \
                     (((dst) & ~0xfffffff0U) | (((u32)(src)<<4) & 0xfffffff0U))


/*	 Fields msn	 */
#define MSN0_WIDTH                                                            4
#define MSN0_SHIFT                                                            0
#define MSN0_MASK                                                    0x0000000f
#define MSN0_RD(src)                                    (((src) & 0x0000000fU))
#define MSN0_WR(src)                               (((u32)(src)) & 0x0000000fU)
#define MSN0_SET(dst,src) \
                        (((dst) & ~0x0000000fU) | (((u32)(src)) & 0x0000000fU))


/*	Register start_addr11	*/

/*	 Fields sa	 */
#define SA1_WIDTH                                                            28
#define SA1_SHIFT                                                             4
#define SA1_MASK                                                     0xfffffff0
#define SA1_RD(src)                                  (((src) & 0xfffffff0U)>>4)
#define SA1_WR(src)                             (((u32)(src)<<4) & 0xfffffff0U)
#define SA1_SET(dst,src) \
                     (((dst) & ~0xfffffff0U) | (((u32)(src)<<4) & 0xfffffff0U))


/*	 Fields ame	 */
#define AME1_WIDTH                                                            1
#define AME1_SHIFT                                                            0
#define AME1_MASK                                                    0x00000001
#define AME1_RD(src)                                    (((src) & 0x00000001U))
#define AME1_WR(src)                               (((u32)(src)) & 0x00000001U)
#define AME1_SET(dst,src) \
                        (((dst) & ~0x00000001U) | (((u32)(src)) & 0x00000001U))


/*	Register end_addr11	*/

/*	 Fields sa	 */
#define SA1_WIDTH                                                            28
#define SA1_SHIFT                                                             4
#define SA1_MASK                                                     0xfffffff0
#define SA1_RD(src)                                  (((src) & 0xfffffff0U)>>4)
#define SA1_WR(src)                             (((u32)(src)<<4) & 0xfffffff0U)
#define SA1_SET(dst,src) \
                     (((dst) & ~0xfffffff0U) | (((u32)(src)<<4) & 0xfffffff0U))


/*	 Fields msn	 */
#define MSN1_WIDTH                                                            4
#define MSN1_SHIFT                                                            0
#define MSN1_MASK                                                    0x0000000f
#define MSN1_RD(src)                                    (((src) & 0x0000000fU))
#define MSN1_WR(src)                               (((u32)(src)) & 0x0000000fU)
#define MSN1_SET(dst,src) \
                        (((dst) & ~0x0000000fU) | (((u32)(src)) & 0x0000000fU))


/*	Register start_addr12	*/

/*	 Fields sa	 */
#define SA2_WIDTH                                                            28
#define SA2_SHIFT                                                             4
#define SA2_MASK                                                     0xfffffff0
#define SA2_RD(src)                                  (((src) & 0xfffffff0U)>>4)
#define SA2_WR(src)                             (((u32)(src)<<4) & 0xfffffff0U)
#define SA2_SET(dst,src) \
                     (((dst) & ~0xfffffff0U) | (((u32)(src)<<4) & 0xfffffff0U))


/*	 Fields ame	 */
#define AME2_WIDTH                                                            1
#define AME2_SHIFT                                                            0
#define AME2_MASK                                                    0x00000001
#define AME2_RD(src)                                    (((src) & 0x00000001U))
#define AME2_WR(src)                               (((u32)(src)) & 0x00000001U)
#define AME2_SET(dst,src) \
                        (((dst) & ~0x00000001U) | (((u32)(src)) & 0x00000001U))


/*	Register end_addr12	*/

/*	 Fields sa	 */
#define SA2_WIDTH                                                            28
#define SA2_SHIFT                                                             4
#define SA2_MASK                                                     0xfffffff0
#define SA2_RD(src)                                  (((src) & 0xfffffff0U)>>4)
#define SA2_WR(src)                             (((u32)(src)<<4) & 0xfffffff0U)
#define SA2_SET(dst,src) \
                     (((dst) & ~0xfffffff0U) | (((u32)(src)<<4) & 0xfffffff0U))


/*	 Fields msn	 */
#define MSN2_WIDTH                                                            4
#define MSN2_SHIFT                                                            0
#define MSN2_MASK                                                    0x0000000f
#define MSN2_RD(src)                                    (((src) & 0x0000000fU))
#define MSN2_WR(src)                               (((u32)(src)) & 0x0000000fU)
#define MSN2_SET(dst,src) \
                        (((dst) & ~0x0000000fU) | (((u32)(src)) & 0x0000000fU))


/*	Register start_addr13	*/

/*	 Fields sa	 */
#define SA3_WIDTH                                                            28
#define SA3_SHIFT                                                             4
#define SA3_MASK                                                     0xfffffff0
#define SA3_RD(src)                                  (((src) & 0xfffffff0U)>>4)
#define SA3_WR(src)                             (((u32)(src)<<4) & 0xfffffff0U)
#define SA3_SET(dst,src) \
                     (((dst) & ~0xfffffff0U) | (((u32)(src)<<4) & 0xfffffff0U))


/*	 Fields ame	 */
#define AME3_WIDTH                                                            1
#define AME3_SHIFT                                                            0
#define AME3_MASK                                                    0x00000001
#define AME3_RD(src)                                    (((src) & 0x00000001U))
#define AME3_WR(src)                               (((u32)(src)) & 0x00000001U)
#define AME3_SET(dst,src) \
                        (((dst) & ~0x00000001U) | (((u32)(src)) & 0x00000001U))


/*	Register end_addr13	*/

/*	 Fields sa	 */
#define SA3_WIDTH                                                            28
#define SA3_SHIFT                                                             4
#define SA3_MASK                                                     0xfffffff0
#define SA3_RD(src)                                  (((src) & 0xfffffff0U)>>4)
#define SA3_WR(src)                             (((u32)(src)<<4) & 0xfffffff0U)
#define SA3_SET(dst,src) \
                     (((dst) & ~0xfffffff0U) | (((u32)(src)<<4) & 0xfffffff0U))


/*	 Fields msn	 */
#define MSN3_WIDTH                                                            4
#define MSN3_SHIFT                                                            0
#define MSN3_MASK                                                    0x0000000f
#define MSN3_RD(src)                                    (((src) & 0x0000000fU))
#define MSN3_WR(src)                               (((u32)(src)) & 0x0000000fU)
#define MSN3_SET(dst,src) \
                        (((dst) & ~0x0000000fU) | (((u32)(src)) & 0x0000000fU))


/*	Register start_addr14	*/

/*	 Fields sa	 */
#define SA4_WIDTH                                                            28
#define SA4_SHIFT                                                             4
#define SA4_MASK                                                     0xfffffff0
#define SA4_RD(src)                                  (((src) & 0xfffffff0U)>>4)
#define SA4_WR(src)                             (((u32)(src)<<4) & 0xfffffff0U)
#define SA4_SET(dst,src) \
                     (((dst) & ~0xfffffff0U) | (((u32)(src)<<4) & 0xfffffff0U))


/*	 Fields ame	 */
#define AME4_WIDTH                                                            1
#define AME4_SHIFT                                                            0
#define AME4_MASK                                                    0x00000001
#define AME4_RD(src)                                    (((src) & 0x00000001U))
#define AME4_WR(src)                               (((u32)(src)) & 0x00000001U)
#define AME4_SET(dst,src) \
                        (((dst) & ~0x00000001U) | (((u32)(src)) & 0x00000001U))


/*	Register end_addr14	*/

/*	 Fields sa	 */
#define SA4_WIDTH                                                            28
#define SA4_SHIFT                                                             4
#define SA4_MASK                                                     0xfffffff0
#define SA4_RD(src)                                  (((src) & 0xfffffff0U)>>4)
#define SA4_WR(src)                             (((u32)(src)<<4) & 0xfffffff0U)
#define SA4_SET(dst,src) \
                     (((dst) & ~0xfffffff0U) | (((u32)(src)<<4) & 0xfffffff0U))


/*	 Fields msn	 */
#define MSN4_WIDTH                                                            4
#define MSN4_SHIFT                                                            0
#define MSN4_MASK                                                    0x0000000f
#define MSN4_RD(src)                                    (((src) & 0x0000000fU))
#define MSN4_WR(src)                               (((u32)(src)) & 0x0000000fU)
#define MSN4_SET(dst,src) \
                        (((dst) & ~0x0000000fU) | (((u32)(src)) & 0x0000000fU))


/*	Register start_addr15	*/

/*	 Fields sa	 */
#define SA5_WIDTH                                                            28
#define SA5_SHIFT                                                             4
#define SA5_MASK                                                     0xfffffff0
#define SA5_RD(src)                                  (((src) & 0xfffffff0U)>>4)
#define SA5_WR(src)                             (((u32)(src)<<4) & 0xfffffff0U)
#define SA5_SET(dst,src) \
                     (((dst) & ~0xfffffff0U) | (((u32)(src)<<4) & 0xfffffff0U))


/*	 Fields ame	 */
#define AME5_WIDTH                                                            1
#define AME5_SHIFT                                                            0
#define AME5_MASK                                                    0x00000001
#define AME5_RD(src)                                    (((src) & 0x00000001U))
#define AME5_WR(src)                               (((u32)(src)) & 0x00000001U)
#define AME5_SET(dst,src) \
                        (((dst) & ~0x00000001U) | (((u32)(src)) & 0x00000001U))


/*	Register end_addr15	*/

/*	 Fields sa	 */
#define SA5_WIDTH                                                            28
#define SA5_SHIFT                                                             4
#define SA5_MASK                                                     0xfffffff0
#define SA5_RD(src)                                  (((src) & 0xfffffff0U)>>4)
#define SA5_WR(src)                             (((u32)(src)<<4) & 0xfffffff0U)
#define SA5_SET(dst,src) \
                     (((dst) & ~0xfffffff0U) | (((u32)(src)<<4) & 0xfffffff0U))


/*	 Fields msn	 */
#define MSN5_WIDTH                                                            4
#define MSN5_SHIFT                                                            0
#define MSN5_MASK                                                    0x0000000f
#define MSN5_RD(src)                                    (((src) & 0x0000000fU))
#define MSN5_WR(src)                               (((u32)(src)) & 0x0000000fU)
#define MSN5_SET(dst,src) \
                        (((dst) & ~0x0000000fU) | (((u32)(src)) & 0x0000000fU))


/*	Register start_addr16	*/

/*	 Fields sa	 */
#define SA6_WIDTH                                                            28
#define SA6_SHIFT                                                             4
#define SA6_MASK                                                     0xfffffff0
#define SA6_RD(src)                                  (((src) & 0xfffffff0U)>>4)
#define SA6_WR(src)                             (((u32)(src)<<4) & 0xfffffff0U)
#define SA6_SET(dst,src) \
                     (((dst) & ~0xfffffff0U) | (((u32)(src)<<4) & 0xfffffff0U))


/*	 Fields ame	 */
#define AME6_WIDTH                                                            1
#define AME6_SHIFT                                                            0
#define AME6_MASK                                                    0x00000001
#define AME6_RD(src)                                    (((src) & 0x00000001U))
#define AME6_WR(src)                               (((u32)(src)) & 0x00000001U)
#define AME6_SET(dst,src) \
                        (((dst) & ~0x00000001U) | (((u32)(src)) & 0x00000001U))


/*	Register end_addr16	*/

/*	 Fields sa	 */
#define SA6_WIDTH                                                            28
#define SA6_SHIFT                                                             4
#define SA6_MASK                                                     0xfffffff0
#define SA6_RD(src)                                  (((src) & 0xfffffff0U)>>4)
#define SA6_WR(src)                             (((u32)(src)<<4) & 0xfffffff0U)
#define SA6_SET(dst,src) \
                     (((dst) & ~0xfffffff0U) | (((u32)(src)<<4) & 0xfffffff0U))


/*	 Fields msn	 */
#define MSN6_WIDTH                                                            4
#define MSN6_SHIFT                                                            0
#define MSN6_MASK                                                    0x0000000f
#define MSN6_RD(src)                                    (((src) & 0x0000000fU))
#define MSN6_WR(src)                               (((u32)(src)) & 0x0000000fU)
#define MSN6_SET(dst,src) \
                        (((dst) & ~0x0000000fU) | (((u32)(src)) & 0x0000000fU))


/*	Register core_id_reg	*/

/*	 Fields core_id	 */
#define CORE_ID_WIDTH                                                        32
#define CORE_ID_SHIFT                                                         0
#define CORE_ID_MASK                                                 0xffffffff
#define CORE_ID_RD(src)                                 (((src) & 0xffffffffU))
#define CORE_ID_WR(src)                            (((u32)(src)) & 0xffffffffU)
#define CORE_ID_SET(dst,src) \
                        (((dst) & ~0xffffffffU) | (((u32)(src)) & 0xffffffffU))


/*	Register debug_test_reg	*/

/*	 Fields sw_int	 */
#define SW_INT_WIDTH                                                          1
#define SW_INT_SHIFT                                                          0
#define SW_INT_MASK                                                  0x00000001
#define SW_INT_RD(src)                                  (((src) & 0x00000001U))
#define SW_INT_WR(src)                             (((u32)(src)) & 0x00000001U)
#define SW_INT_SET(dst,src) \
                        (((dst) & ~0x00000001U) | (((u32)(src)) & 0x00000001U))


/*	Register lcdc_cr1	*/

/*	 Fields dfe	 */
#define DFE1_WIDTH                                                            1
#define DFE1_SHIFT                                                           23
#define DFE1_MASK                                                    0x00800000
#define DFE1_RD(src)                                (((src) & 0x00800000U)>>23)
#define DFE1_WR(src)                           (((u32)(src)<<23) & 0x00800000U)
#define DFE1_SET(dst,src) \
                    (((dst) & ~0x00800000U) | (((u32)(src)<<23) & 0x00800000U))


/*	 Fields dfb	 */
#define DFB1_WIDTH                                                            1
#define DFB1_SHIFT                                                           22
#define DFB1_MASK                                                    0x00400000
#define DFB1_RD(src)                                (((src) & 0x00400000U)>>22)
#define DFB1_WR(src)                           (((u32)(src)<<22) & 0x00400000U)
#define DFB1_SET(dst,src) \
                    (((dst) & ~0x00400000U) | (((u32)(src)<<22) & 0x00400000U))


/*	 Fields dfr	 */
#define DFR1_WIDTH                                                            1
#define DFR1_SHIFT                                                           21
#define DFR1_MASK                                                    0x00200000
#define DFR1_RD(src)                                (((src) & 0x00200000U)>>21)
#define DFR1_WR(src)                           (((u32)(src)<<21) & 0x00200000U)
#define DFR1_SET(dst,src) \
                    (((dst) & ~0x00200000U) | (((u32)(src)<<21) & 0x00200000U))


/*	 Fields dee	 */
#define DEE1_WIDTH                                                            1
#define DEE1_SHIFT                                                           20
#define DEE1_MASK                                                    0x00100000
#define DEE1_RD(src)                                (((src) & 0x00100000U)>>20)
#define DEE1_WR(src)                           (((u32)(src)<<20) & 0x00100000U)
#define DEE1_SET(dst,src) \
                    (((dst) & ~0x00100000U) | (((u32)(src)<<20) & 0x00100000U))


/*	 Fields fbp	 */
#define FBP1_WIDTH                                                            1
#define FBP1_SHIFT                                                           19
#define FBP1_MASK                                                    0x00080000
#define FBP1_RD(src)                                (((src) & 0x00080000U)>>19)
#define FBP1_WR(src)                           (((u32)(src)<<19) & 0x00080000U)
#define FBP1_SET(dst,src) \
                    (((dst) & ~0x00080000U) | (((u32)(src)<<19) & 0x00080000U))


/*	 Fields lps	 */
#define LPS1_WIDTH                                                            1
#define LPS1_SHIFT                                                           18
#define LPS1_MASK                                                    0x00040000
#define LPS1_RD(src)                                (((src) & 0x00040000U)>>18)
#define LPS1_WR(src)                           (((u32)(src)<<18) & 0x00040000U)
#define LPS1_SET(dst,src) \
                    (((dst) & ~0x00040000U) | (((u32)(src)<<18) & 0x00040000U))


/*	 Fields fdw	 */
#define FDW1_WIDTH                                                            2
#define FDW1_SHIFT                                                           16
#define FDW1_MASK                                                    0x00030000
#define FDW1_RD(src)                                (((src) & 0x00030000U)>>16)
#define FDW1_WR(src)                           (((u32)(src)<<16) & 0x00030000U)
#define FDW1_SET(dst,src) \
                    (((dst) & ~0x00030000U) | (((u32)(src)<<16) & 0x00030000U))


/*	 Fields pss	 */
#define PSS1_WIDTH                                                            1
#define PSS1_SHIFT                                                           15
#define PSS1_MASK                                                    0x00008000
#define PSS1_RD(src)                                (((src) & 0x00008000U)>>15)
#define PSS1_WR(src)                           (((u32)(src)<<15) & 0x00008000U)
#define PSS1_SET(dst,src) \
                    (((dst) & ~0x00008000U) | (((u32)(src)<<15) & 0x00008000U))


/*	 Fields ops	 */
#define OPS1_WIDTH                                                            3
#define OPS1_SHIFT                                                           12
#define OPS1_MASK                                                    0x00007000
#define OPS1_RD(src)                                (((src) & 0x00007000U)>>12)
#define OPS1_WR(src)                           (((u32)(src)<<12) & 0x00007000U)
#define OPS1_SET(dst,src) \
                    (((dst) & ~0x00007000U) | (((u32)(src)<<12) & 0x00007000U))


/*	 Fields vsp	 */
#define VSP1_WIDTH                                                            1
#define VSP1_SHIFT                                                           11
#define VSP1_MASK                                                    0x00000800
#define VSP1_RD(src)                                (((src) & 0x00000800U)>>11)
#define VSP1_WR(src)                           (((u32)(src)<<11) & 0x00000800U)
#define VSP1_SET(dst,src) \
                    (((dst) & ~0x00000800U) | (((u32)(src)<<11) & 0x00000800U))


/*	 Fields hsp	 */
#define HSP1_WIDTH                                                            1
#define HSP1_SHIFT                                                           10
#define HSP1_MASK                                                    0x00000400
#define HSP1_RD(src)                                (((src) & 0x00000400U)>>10)
#define HSP1_WR(src)                           (((u32)(src)<<10) & 0x00000400U)
#define HSP1_SET(dst,src) \
                    (((dst) & ~0x00000400U) | (((u32)(src)<<10) & 0x00000400U))


/*	 Fields pcp	 */
#define PCP1_WIDTH                                                            1
#define PCP1_SHIFT                                                            9
#define PCP1_MASK                                                    0x00000200
#define PCP1_RD(src)                                 (((src) & 0x00000200U)>>9)
#define PCP1_WR(src)                            (((u32)(src)<<9) & 0x00000200U)
#define PCP1_SET(dst,src) \
                     (((dst) & ~0x00000200U) | (((u32)(src)<<9) & 0x00000200U))


/*	 Fields dep	 */
#define DEP1_WIDTH                                                            1
#define DEP1_SHIFT                                                            8
#define DEP1_MASK                                                    0x00000100
#define DEP1_RD(src)                                 (((src) & 0x00000100U)>>8)
#define DEP1_WR(src)                            (((u32)(src)<<8) & 0x00000100U)
#define DEP1_SET(dst,src) \
                     (((dst) & ~0x00000100U) | (((u32)(src)<<8) & 0x00000100U))


/*	 Fields ebo	 */
#define EBO1_WIDTH                                                            1
#define EBO1_SHIFT                                                            7
#define EBO1_MASK                                                    0x00000080
#define EBO1_RD(src)                                 (((src) & 0x00000080U)>>7)
#define EBO1_WR(src)                            (((u32)(src)<<7) & 0x00000080U)
#define EBO1_SET(dst,src) \
                     (((dst) & ~0x00000080U) | (((u32)(src)<<7) & 0x00000080U))


/*	 Fields epo	 */
#define EPO1_WIDTH                                                            1
#define EPO1_SHIFT                                                            6
#define EPO1_MASK                                                    0x00000040
#define EPO1_RD(src)                                 (((src) & 0x00000040U)>>6)
#define EPO1_WR(src)                            (((u32)(src)<<6) & 0x00000040U)
#define EPO1_SET(dst,src) \
                     (((dst) & ~0x00000040U) | (((u32)(src)<<6) & 0x00000040U))


/*	 Fields rgb	 */
#define RGB1_WIDTH                                                            1
#define RGB1_SHIFT                                                            5
#define RGB1_MASK                                                    0x00000020
#define RGB1_RD(src)                                 (((src) & 0x00000020U)>>5)
#define RGB1_WR(src)                            (((u32)(src)<<5) & 0x00000020U)
#define RGB1_SET(dst,src) \
                     (((dst) & ~0x00000020U) | (((u32)(src)<<5) & 0x00000020U))


/*	 Fields bpp	 */
#define BPP1_WIDTH                                                            3
#define BPP1_SHIFT                                                            2
#define BPP1_MASK                                                    0x0000001c
#define BPP1_RD(src)                                 (((src) & 0x0000001cU)>>2)
#define BPP1_WR(src)                            (((u32)(src)<<2) & 0x0000001cU)
#define BPP1_SET(dst,src) \
                     (((dst) & ~0x0000001cU) | (((u32)(src)<<2) & 0x0000001cU))


/*	 Fields lpe	 */
#define LPE1_WIDTH                                                            1
#define LPE1_SHIFT                                                            1
#define LPE1_MASK                                                    0x00000002
#define LPE1_RD(src)                                 (((src) & 0x00000002U)>>1)
#define LPE1_WR(src)                            (((u32)(src)<<1) & 0x00000002U)
#define LPE1_SET(dst,src) \
                     (((dst) & ~0x00000002U) | (((u32)(src)<<1) & 0x00000002U))


/*	 Fields lce	 */
#define LCE1_WIDTH                                                            1
#define LCE1_SHIFT                                                            0
#define LCE1_MASK                                                    0x00000001
#define LCE1_RD(src)                                    (((src) & 0x00000001U))
#define LCE1_WR(src)                               (((u32)(src)) & 0x00000001U)
#define LCE1_SET(dst,src) \
                        (((dst) & ~0x00000001U) | (((u32)(src)) & 0x00000001U))


/*	Register lcdc_htr	*/

/*	 Fields hsw	 */
#define HSW_WIDTH                                                             8
#define HSW_SHIFT                                                            24
#define HSW_MASK                                                     0xff000000
#define HSW_RD(src)                                 (((src) & 0xff000000U)>>24)
#define HSW_WR(src)                            (((u32)(src)<<24) & 0xff000000U)
#define HSW_SET(dst,src) \
                    (((dst) & ~0xff000000U) | (((u32)(src)<<24) & 0xff000000U))


/*	 Fields hbp	 */
#define HBP_WIDTH                                                             8
#define HBP_SHIFT                                                            16
#define HBP_MASK                                                     0x00ff0000
#define HBP_RD(src)                                 (((src) & 0x00ff0000U)>>16)
#define HBP_WR(src)                            (((u32)(src)<<16) & 0x00ff0000U)
#define HBP_SET(dst,src) \
                    (((dst) & ~0x00ff0000U) | (((u32)(src)<<16) & 0x00ff0000U))


/*	 Fields ppl	 */
#define PPL_WIDTH                                                             8
#define PPL_SHIFT                                                             8
#define PPL_MASK                                                     0x0000ff00
#define PPL_RD(src)                                  (((src) & 0x0000ff00U)>>8)
#define PPL_WR(src)                             (((u32)(src)<<8) & 0x0000ff00U)
#define PPL_SET(dst,src) \
                     (((dst) & ~0x0000ff00U) | (((u32)(src)<<8) & 0x0000ff00U))


/*	 Fields hfp	 */
#define HFP_WIDTH                                                             8
#define HFP_SHIFT                                                             0
#define HFP_MASK                                                     0x000000ff
#define HFP_RD(src)                                     (((src) & 0x000000ffU))
#define HFP_WR(src)                                (((u32)(src)) & 0x000000ffU)
#define HFP_SET(dst,src) \
                        (((dst) & ~0x000000ffU) | (((u32)(src)) & 0x000000ffU))


/*	Register lcdc_vtr1	*/

/*	 Fields vbp	 */
#define VBP1_WIDTH                                                            8
#define VBP1_SHIFT                                                           16
#define VBP1_MASK                                                    0x00ff0000
#define VBP1_RD(src)                                (((src) & 0x00ff0000U)>>16)
#define VBP1_WR(src)                           (((u32)(src)<<16) & 0x00ff0000U)
#define VBP1_SET(dst,src) \
                    (((dst) & ~0x00ff0000U) | (((u32)(src)<<16) & 0x00ff0000U))


/*	 Fields vfp	 */
#define VFP1_WIDTH                                                            8
#define VFP1_SHIFT                                                            8
#define VFP1_MASK                                                    0x0000ff00
#define VFP1_RD(src)                                 (((src) & 0x0000ff00U)>>8)
#define VFP1_WR(src)                            (((u32)(src)<<8) & 0x0000ff00U)
#define VFP1_SET(dst,src) \
                     (((dst) & ~0x0000ff00U) | (((u32)(src)<<8) & 0x0000ff00U))


/*	 Fields vsw	 */
#define VSW1_WIDTH                                                            8
#define VSW1_SHIFT                                                            0
#define VSW1_MASK                                                    0x000000ff
#define VSW1_RD(src)                                    (((src) & 0x000000ffU))
#define VSW1_WR(src)                               (((u32)(src)) & 0x000000ffU)
#define VSW1_SET(dst,src) \
                        (((dst) & ~0x000000ffU) | (((u32)(src)) & 0x000000ffU))


/*	Register lcdc_vtr2	*/

/*	 Fields lpp	 */
#define LPP2_WIDTH                                                           12
#define LPP2_SHIFT                                                            0
#define LPP2_MASK                                                    0x00000fff
#define LPP2_RD(src)                                    (((src) & 0x00000fffU))
#define LPP2_WR(src)                               (((u32)(src)) & 0x00000fffU)
#define LPP2_SET(dst,src) \
                        (((dst) & ~0x00000fffU) | (((u32)(src)) & 0x00000fffU))


/*	Register lcdc_pctr	*/

/*	 Fields pci	 */
#define PCI_WIDTH                                                             1
#define PCI_SHIFT                                                             9
#define PCI_MASK                                                     0x00000200
#define PCI_RD(src)                                  (((src) & 0x00000200U)>>9)
#define PCI_WR(src)                             (((u32)(src)<<9) & 0x00000200U)
#define PCI_SET(dst,src) \
                     (((dst) & ~0x00000200U) | (((u32)(src)<<9) & 0x00000200U))


/*	 Fields pcb	 */
#define PCB_WIDTH                                                             1
#define PCB_SHIFT                                                             8
#define PCB_MASK                                                     0x00000100
#define PCB_RD(src)                                  (((src) & 0x00000100U)>>8)
#define PCB_WR(src)                             (((u32)(src)<<8) & 0x00000100U)
#define PCB_SET(dst,src) \
                     (((dst) & ~0x00000100U) | (((u32)(src)<<8) & 0x00000100U))


/*	 Fields pcd	 */
#define PCD_WIDTH                                                             8
#define PCD_SHIFT                                                             0
#define PCD_MASK                                                     0x000000ff
#define PCD_RD(src)                                     (((src) & 0x000000ffU))
#define PCD_WR(src)                                (((u32)(src)) & 0x000000ffU)
#define PCD_SET(dst,src) \
                        (((dst) & ~0x000000ffU) | (((u32)(src)) & 0x000000ffU))


/*	Register lcdc_isr	*/

/*	 Fields ars	 */
#define ARS_WIDTH                                                             1
#define ARS_SHIFT                                                            11
#define ARS_MASK                                                     0x00000800
#define ARS_RD(src)                                 (((src) & 0x00000800U)>>11)
#define ARS_SET(dst,src) \
                    (((dst) & ~0x00000800U) | (((u32)(src)<<11) & 0x00000800U))


/*	 Fields ari	 */
#define ARI_WIDTH                                                             1
#define ARI_SHIFT                                                            10
#define ARI_MASK                                                     0x00000400
#define ARI_RD(src)                                 (((src) & 0x00000400U)>>10)
#define ARI_SET(dst,src) \
                    (((dst) & ~0x00000400U) | (((u32)(src)<<10) & 0x00000400U))


/*	 Fields abl	 */
#define ABL_WIDTH                                                             1
#define ABL_SHIFT                                                             9
#define ABL_MASK                                                     0x00000200
#define ABL_RD(src)                                  (((src) & 0x00000200U)>>9)
#define ABL_SET(dst,src) \
                     (((dst) & ~0x00000200U) | (((u32)(src)<<9) & 0x00000200U))


/*	 Fields ldd	 */
#define LDD_WIDTH                                                             1
#define LDD_SHIFT                                                             8
#define LDD_MASK                                                     0x00000100
#define LDD_RD(src)                                  (((src) & 0x00000100U)>>8)
#define LDD_SET(dst,src) \
                     (((dst) & ~0x00000100U) | (((u32)(src)<<8) & 0x00000100U))


/*	 Fields bau	 */
#define BAU_WIDTH                                                             1
#define BAU_SHIFT                                                             7
#define BAU_MASK                                                     0x00000080
#define BAU_RD(src)                                  (((src) & 0x00000080U)>>7)
#define BAU_SET(dst,src) \
                     (((dst) & ~0x00000080U) | (((u32)(src)<<7) & 0x00000080U))


/*	 Fields vct	 */
#define VCT_WIDTH                                                             1
#define VCT_SHIFT                                                             6
#define VCT_MASK                                                     0x00000040
#define VCT_RD(src)                                  (((src) & 0x00000040U)>>6)
#define VCT_SET(dst,src) \
                     (((dst) & ~0x00000040U) | (((u32)(src)<<6) & 0x00000040U))


/*	 Fields mbe	 */
#define MBE_WIDTH                                                             1
#define MBE_SHIFT                                                             5
#define MBE_MASK                                                     0x00000020
#define MBE_RD(src)                                  (((src) & 0x00000020U)>>5)
#define MBE_SET(dst,src) \
                     (((dst) & ~0x00000020U) | (((u32)(src)<<5) & 0x00000020U))


/*	 Fields fer	 */
#define FER_WIDTH                                                             1
#define FER_SHIFT                                                             4
#define FER_MASK                                                     0x00000010
#define FER_RD(src)                                  (((src) & 0x00000010U)>>4)
#define FER_SET(dst,src) \
                     (((dst) & ~0x00000010U) | (((u32)(src)<<4) & 0x00000010U))


/*	 Fields ifo	 */
#define IFO_WIDTH                                                             1
#define IFO_SHIFT                                                             3
#define IFO_MASK                                                     0x00000008
#define IFO_RD(src)                                  (((src) & 0x00000008U)>>3)
#define IFO_SET(dst,src) \
                     (((dst) & ~0x00000008U) | (((u32)(src)<<3) & 0x00000008U))


/*	 Fields ifu	 */
#define IFU_WIDTH                                                             1
#define IFU_SHIFT                                                             2
#define IFU_MASK                                                     0x00000004
#define IFU_RD(src)                                  (((src) & 0x00000004U)>>2)
#define IFU_SET(dst,src) \
                     (((dst) & ~0x00000004U) | (((u32)(src)<<2) & 0x00000004U))


/*	 Fields ofo	 */
#define OFO_WIDTH                                                             1
#define OFO_SHIFT                                                             1
#define OFO_MASK                                                     0x00000002
#define OFO_RD(src)                                  (((src) & 0x00000002U)>>1)
#define OFO_SET(dst,src) \
                     (((dst) & ~0x00000002U) | (((u32)(src)<<1) & 0x00000002U))


/*	 Fields ofu	 */
#define OFU_WIDTH                                                             1
#define OFU_SHIFT                                                             0
#define OFU_MASK                                                     0x00000001
#define OFU_RD(src)                                     (((src) & 0x00000001U))
#define OFU_SET(dst,src) \
                        (((dst) & ~0x00000001U) | (((u32)(src)) & 0x00000001U))


/*	Register lcdc_imr	*/

/*	 Fields arsm	 */
#define ARSM_WIDTH                                                            1
#define ARSM_SHIFT                                                           11
#define ARSM_MASK                                                    0x00000800
#define ARSM_RD(src)                                (((src) & 0x00000800U)>>11)
#define ARSM_WR(src)                           (((u32)(src)<<11) & 0x00000800U)
#define ARSM_SET(dst,src) \
                    (((dst) & ~0x00000800U) | (((u32)(src)<<11) & 0x00000800U))


/*	 Fields arim	 */
#define ARIM_WIDTH                                                            1
#define ARIM_SHIFT                                                           10
#define ARIM_MASK                                                    0x00000400
#define ARIM_RD(src)                                (((src) & 0x00000400U)>>10)
#define ARIM_WR(src)                           (((u32)(src)<<10) & 0x00000400U)
#define ARIM_SET(dst,src) \
                    (((dst) & ~0x00000400U) | (((u32)(src)<<10) & 0x00000400U))


/*	 Fields ablm	 */
#define ABLM_WIDTH                                                            1
#define ABLM_SHIFT                                                            9
#define ABLM_MASK                                                    0x00000200
#define ABLM_RD(src)                                 (((src) & 0x00000200U)>>9)
#define ABLM_WR(src)                            (((u32)(src)<<9) & 0x00000200U)
#define ABLM_SET(dst,src) \
                     (((dst) & ~0x00000200U) | (((u32)(src)<<9) & 0x00000200U))


/*	 Fields lddm	 */
#define LDDM_WIDTH                                                            1
#define LDDM_SHIFT                                                            8
#define LDDM_MASK                                                    0x00000100
#define LDDM_RD(src)                                 (((src) & 0x00000100U)>>8)
#define LDDM_WR(src)                            (((u32)(src)<<8) & 0x00000100U)
#define LDDM_SET(dst,src) \
                     (((dst) & ~0x00000100U) | (((u32)(src)<<8) & 0x00000100U))


/*	 Fields baum	 */
#define BAUM_WIDTH                                                            1
#define BAUM_SHIFT                                                            7
#define BAUM_MASK                                                    0x00000080
#define BAUM_RD(src)                                 (((src) & 0x00000080U)>>7)
#define BAUM_WR(src)                            (((u32)(src)<<7) & 0x00000080U)
#define BAUM_SET(dst,src) \
                     (((dst) & ~0x00000080U) | (((u32)(src)<<7) & 0x00000080U))


/*	 Fields vctm	 */
#define VCTM_WIDTH                                                            1
#define VCTM_SHIFT                                                            6
#define VCTM_MASK                                                    0x00000040
#define VCTM_RD(src)                                 (((src) & 0x00000040U)>>6)
#define VCTM_WR(src)                            (((u32)(src)<<6) & 0x00000040U)
#define VCTM_SET(dst,src) \
                     (((dst) & ~0x00000040U) | (((u32)(src)<<6) & 0x00000040U))


/*	 Fields mbem	 */
#define MBEM_WIDTH                                                            1
#define MBEM_SHIFT                                                            5
#define MBEM_MASK                                                    0x00000020
#define MBEM_RD(src)                                 (((src) & 0x00000020U)>>5)
#define MBEM_WR(src)                            (((u32)(src)<<5) & 0x00000020U)
#define MBEM_SET(dst,src) \
                     (((dst) & ~0x00000020U) | (((u32)(src)<<5) & 0x00000020U))


/*	 Fields ferm	 */
#define FERM_WIDTH                                                            1
#define FERM_SHIFT                                                            4
#define FERM_MASK                                                    0x00000010
#define FERM_RD(src)                                 (((src) & 0x00000010U)>>4)
#define FERM_WR(src)                            (((u32)(src)<<4) & 0x00000010U)
#define FERM_SET(dst,src) \
                     (((dst) & ~0x00000010U) | (((u32)(src)<<4) & 0x00000010U))


/*	 Fields ifom	 */
#define IFOM_WIDTH                                                            1
#define IFOM_SHIFT                                                            3
#define IFOM_MASK                                                    0x00000008
#define IFOM_RD(src)                                 (((src) & 0x00000008U)>>3)
#define IFOM_WR(src)                            (((u32)(src)<<3) & 0x00000008U)
#define IFOM_SET(dst,src) \
                     (((dst) & ~0x00000008U) | (((u32)(src)<<3) & 0x00000008U))


/*	 Fields ifum	 */
#define IFUM_WIDTH                                                            1
#define IFUM_SHIFT                                                            2
#define IFUM_MASK                                                    0x00000004
#define IFUM_RD(src)                                 (((src) & 0x00000004U)>>2)
#define IFUM_WR(src)                            (((u32)(src)<<2) & 0x00000004U)
#define IFUM_SET(dst,src) \
                     (((dst) & ~0x00000004U) | (((u32)(src)<<2) & 0x00000004U))


/*	 Fields ofom	 */
#define OFOM_WIDTH                                                            1
#define OFOM_SHIFT                                                            1
#define OFOM_MASK                                                    0x00000002
#define OFOM_RD(src)                                 (((src) & 0x00000002U)>>1)
#define OFOM_WR(src)                            (((u32)(src)<<1) & 0x00000002U)
#define OFOM_SET(dst,src) \
                     (((dst) & ~0x00000002U) | (((u32)(src)<<1) & 0x00000002U))


/*	 Fields ofum	 */
#define OFUM_WIDTH                                                            1
#define OFUM_SHIFT                                                            0
#define OFUM_MASK                                                    0x00000001
#define OFUM_RD(src)                                    (((src) & 0x00000001U))
#define OFUM_WR(src)                               (((u32)(src)) & 0x00000001U)
#define OFUM_SET(dst,src) \
                        (((dst) & ~0x00000001U) | (((u32)(src)) & 0x00000001U))


/*	Register lcdc_ivr	*/

/*	 Fields arsv	 */
#define ARSV_WIDTH                                                            1
#define ARSV_SHIFT                                                           11
#define ARSV_MASK                                                    0x00000800
#define ARSV_RD(src)                                (((src) & 0x00000800U)>>11)
#define ARSV_SET(dst,src) \
                    (((dst) & ~0x00000800U) | (((u32)(src)<<11) & 0x00000800U))


/*	 Fields ariv	 */
#define ARIV_WIDTH                                                            1
#define ARIV_SHIFT                                                           10
#define ARIV_MASK                                                    0x00000400
#define ARIV_RD(src)                                (((src) & 0x00000400U)>>10)
#define ARIV_SET(dst,src) \
                    (((dst) & ~0x00000400U) | (((u32)(src)<<10) & 0x00000400U))


/*	 Fields ablv	 */
#define ABLV_WIDTH                                                            1
#define ABLV_SHIFT                                                            9
#define ABLV_MASK                                                    0x00000200
#define ABLV_RD(src)                                 (((src) & 0x00000200U)>>9)
#define ABLV_SET(dst,src) \
                     (((dst) & ~0x00000200U) | (((u32)(src)<<9) & 0x00000200U))


/*	 Fields lddv	 */
#define LDDV_WIDTH                                                            1
#define LDDV_SHIFT                                                            8
#define LDDV_MASK                                                    0x00000100
#define LDDV_RD(src)                                 (((src) & 0x00000100U)>>8)
#define LDDV_SET(dst,src) \
                     (((dst) & ~0x00000100U) | (((u32)(src)<<8) & 0x00000100U))


/*	 Fields bauv	 */
#define BAUV_WIDTH                                                            1
#define BAUV_SHIFT                                                            7
#define BAUV_MASK                                                    0x00000080
#define BAUV_RD(src)                                 (((src) & 0x00000080U)>>7)
#define BAUV_SET(dst,src) \
                     (((dst) & ~0x00000080U) | (((u32)(src)<<7) & 0x00000080U))


/*	 Fields vctv	 */
#define VCTV_WIDTH                                                            1
#define VCTV_SHIFT                                                            6
#define VCTV_MASK                                                    0x00000040
#define VCTV_RD(src)                                 (((src) & 0x00000040U)>>6)
#define VCTV_SET(dst,src) \
                     (((dst) & ~0x00000040U) | (((u32)(src)<<6) & 0x00000040U))


/*	 Fields mbev	 */
#define MBEV_WIDTH                                                            1
#define MBEV_SHIFT                                                            5
#define MBEV_MASK                                                    0x00000020
#define MBEV_RD(src)                                 (((src) & 0x00000020U)>>5)
#define MBEV_SET(dst,src) \
                     (((dst) & ~0x00000020U) | (((u32)(src)<<5) & 0x00000020U))


/*	 Fields ferv	 */
#define FERV_WIDTH                                                            1
#define FERV_SHIFT                                                            4
#define FERV_MASK                                                    0x00000010
#define FERV_RD(src)                                 (((src) & 0x00000010U)>>4)
#define FERV_SET(dst,src) \
                     (((dst) & ~0x00000010U) | (((u32)(src)<<4) & 0x00000010U))


/*	 Fields ifov	 */
#define IFOV_WIDTH                                                            1
#define IFOV_SHIFT                                                            3
#define IFOV_MASK                                                    0x00000008
#define IFOV_RD(src)                                 (((src) & 0x00000008U)>>3)
#define IFOV_SET(dst,src) \
                     (((dst) & ~0x00000008U) | (((u32)(src)<<3) & 0x00000008U))


/*	 Fields ifuv	 */
#define IFUV_WIDTH                                                            1
#define IFUV_SHIFT                                                            2
#define IFUV_MASK                                                    0x00000004
#define IFUV_RD(src)                                 (((src) & 0x00000004U)>>2)
#define IFUV_SET(dst,src) \
                     (((dst) & ~0x00000004U) | (((u32)(src)<<2) & 0x00000004U))


/*	 Fields ofov	 */
#define OFOV_WIDTH                                                            1
#define OFOV_SHIFT                                                            1
#define OFOV_MASK                                                    0x00000002
#define OFOV_RD(src)                                 (((src) & 0x00000002U)>>1)
#define OFOV_SET(dst,src) \
                     (((dst) & ~0x00000002U) | (((u32)(src)<<1) & 0x00000002U))


/*	 Fields ofuv	 */
#define OFUV_WIDTH                                                            1
#define OFUV_SHIFT                                                            0
#define OFUV_MASK                                                    0x00000001
#define OFUV_RD(src)                                    (((src) & 0x00000001U))
#define OFUV_SET(dst,src) \
                        (((dst) & ~0x00000001U) | (((u32)(src)) & 0x00000001U))


/*	Register lcdc_iscr	*/

/*	 Fields vsc	 */
#define VSC_WIDTH                                                             3
#define VSC_SHIFT                                                             0
#define VSC_MASK                                                     0x00000007
#define VSC_RD(src)                                     (((src) & 0x00000007U))
#define VSC_WR(src)                                (((u32)(src)) & 0x00000007U)
#define VSC_SET(dst,src) \
                        (((dst) & ~0x00000007U) | (((u32)(src)) & 0x00000007U))


/*	Register lcdc_dbar	*/

/*	 Fields dbar	 */
#define DBAR_WIDTH                                                           32
#define DBAR_SHIFT                                                            0
#define DBAR_MASK                                                    0xffffffff
#define DBAR_RD(src)                                    (((src) & 0xffffffffU))
#define DBAR_WR(src)                               (((u32)(src)) & 0xffffffffU)
#define DBAR_SET(dst,src) \
                        (((dst) & ~0xffffffffU) | (((u32)(src)) & 0xffffffffU))


/*	Register lcdc_dcar	*/

/*	 Fields dcar	 */
#define DCAR_WIDTH                                                           32
#define DCAR_SHIFT                                                            0
#define DCAR_MASK                                                    0xffffffff
#define DCAR_RD(src)                                    (((src) & 0xffffffffU))
#define DCAR_SET(dst,src) \
                        (((dst) & ~0xffffffffU) | (((u32)(src)) & 0xffffffffU))


/*	Register lcdc_dear	*/

/*	 Fields dear	 */
#define DEAR_WIDTH                                                           32
#define DEAR_SHIFT                                                            0
#define DEAR_MASK                                                    0xffffffff
#define DEAR_RD(src)                                    (((src) & 0xffffffffU))
#define DEAR_WR(src)                               (((u32)(src)) & 0xffffffffU)
#define DEAR_SET(dst,src) \
                        (((dst) & ~0xffffffffU) | (((u32)(src)) & 0xffffffffU))


/*	Register lcdc_pwmfr	*/

/*	 Fields pwmfci	 */
#define PWMFCI_WIDTH                                                          1
#define PWMFCI_SHIFT                                                         21
#define PWMFCI_MASK                                                  0x00200000
#define PWMFCI_RD(src)                              (((src) & 0x00200000U)>>21)
#define PWMFCI_WR(src)                         (((u32)(src)<<21) & 0x00200000U)
#define PWMFCI_SET(dst,src) \
                    (((dst) & ~0x00200000U) | (((u32)(src)<<21) & 0x00200000U))


/*	 Fields pwmfce	 */
#define PWMFCE_WIDTH                                                          1
#define PWMFCE_SHIFT                                                         20
#define PWMFCE_MASK                                                  0x00100000
#define PWMFCE_RD(src)                              (((src) & 0x00100000U)>>20)
#define PWMFCE_WR(src)                         (((u32)(src)<<20) & 0x00100000U)
#define PWMFCE_SET(dst,src) \
                    (((dst) & ~0x00100000U) | (((u32)(src)<<20) & 0x00100000U))


/*	 Fields pwmfcd	 */
#define PWMFCD_WIDTH                                                         20
#define PWMFCD_SHIFT                                                          0
#define PWMFCD_MASK                                                  0x000fffff
#define PWMFCD_RD(src)                                  (((src) & 0x000fffffU))
#define PWMFCD_WR(src)                             (((u32)(src)) & 0x000fffffU)
#define PWMFCD_SET(dst,src) \
                        (((dst) & ~0x000fffffU) | (((u32)(src)) & 0x000fffffU))


/*	Register lcdc_pwmdcr	*/

/*	 Fields pwmdcr	 */
#define PWMDCR_WIDTH                                                          8
#define PWMDCR_SHIFT                                                          0
#define PWMDCR_MASK                                                  0x000000ff
#define PWMDCR_RD(src)                                  (((src) & 0x000000ffU))
#define PWMDCR_WR(src)                             (((u32)(src)) & 0x000000ffU)
#define PWMDCR_SET(dst,src) \
                        (((dst) & ~0x000000ffU) | (((u32)(src)) & 0x000000ffU))


/*	Register lcdc_cir	*/

/*	 Fields cir_mn	 */
#define CIR_MN_WIDTH                                                          8
#define CIR_MN_SHIFT                                                         16
#define CIR_MN_MASK                                                  0x00ff0000
#define CIR_MN_RD(src)                              (((src) & 0x00ff0000U)>>16)
#define CIR_MN_SET(dst,src) \
                    (((dst) & ~0x00ff0000U) | (((u32)(src)<<16) & 0x00ff0000U))


/*	 Fields cir_bw	 */
#define CIR_BW_WIDTH                                                          4
#define CIR_BW_SHIFT                                                         12
#define CIR_BW_MASK                                                  0x0000f000
#define CIR_BW_RD(src)                              (((src) & 0x0000f000U)>>12)
#define CIR_BW_SET(dst,src) \
                    (((dst) & ~0x0000f000U) | (((u32)(src)<<12) & 0x0000f000U))


/*	 Fields cir_bi	 */
#define CIR_BI_WIDTH                                                          4
#define CIR_BI_SHIFT                                                          8
#define CIR_BI_MASK                                                  0x00000f00
#define CIR_BI_RD(src)                               (((src) & 0x00000f00U)>>8)
#define CIR_BI_SET(dst,src) \
                     (((dst) & ~0x00000f00U) | (((u32)(src)<<8) & 0x00000f00U))


/*	 Fields cir_rev	 */
#define CIR_REV_WIDTH                                                         8
#define CIR_REV_SHIFT                                                         0
#define CIR_REV_MASK                                                 0x000000ff
#define CIR_REV_RD(src)                                 (((src) & 0x000000ffU))
#define CIR_REV_SET(dst,src) \
                        (((dst) & ~0x000000ffU) | (((u32)(src)) & 0x000000ffU))


/*	Register lcdc_palt	*/

/*	 Fields blue_0	 */
#define BLUE_0_WIDTH                                                          5
#define BLUE_0_SHIFT                                                         27
#define BLUE_0_MASK                                                  0xf8000000
#define BLUE_0_RD(src)                              (((src) & 0xf8000000U)>>27)
#define BLUE_0_SET(dst,src) \
                    (((dst) & ~0xf8000000U) | (((u32)(src)<<27) & 0xf8000000U))


/*	 Fields green_0	 */
#define GREEN_0_WIDTH                                                         6
#define GREEN_0_SHIFT                                                        21
#define GREEN_0_MASK                                                 0x07e00000
#define GREEN_0_RD(src)                             (((src) & 0x07e00000U)>>21)
#define GREEN_0_SET(dst,src) \
                    (((dst) & ~0x07e00000U) | (((u32)(src)<<21) & 0x07e00000U))


/*	 Fields red_0	 */
#define RED_0_WIDTH                                                           5
#define RED_0_SHIFT                                                          16
#define RED_0_MASK                                                   0x001f0000
#define RED_0_RD(src)                               (((src) & 0x001f0000U)>>16)
#define RED_0_SET(dst,src) \
                    (((dst) & ~0x001f0000U) | (((u32)(src)<<16) & 0x001f0000U))


/*	 Fields blue_1	 */
#define BLUE_1_WIDTH                                                          5
#define BLUE_1_SHIFT                                                         11
#define BLUE_1_MASK                                                  0x0000f800
#define BLUE_1_RD(src)                              (((src) & 0x0000f800U)>>11)
#define BLUE_1_SET(dst,src) \
                    (((dst) & ~0x0000f800U) | (((u32)(src)<<11) & 0x0000f800U))


/*	 Fields green_1	 */
#define GREEN_1_WIDTH                                                         6
#define GREEN_1_SHIFT                                                         5
#define GREEN_1_MASK                                                 0x000007e0
#define GREEN_1_RD(src)                              (((src) & 0x000007e0U)>>5)
#define GREEN_1_SET(dst,src) \
                     (((dst) & ~0x000007e0U) | (((u32)(src)<<5) & 0x000007e0U))


/*	 Fields red_1	 */
#define RED_1_WIDTH                                                           5
#define RED_1_SHIFT                                                           0
#define RED_1_MASK                                                   0x0000001f
#define RED_1_RD(src)                                   (((src) & 0x0000001fU))
#define RED_1_SET(dst,src) \
                        (((dst) & ~0x0000001fU) | (((u32)(src)) & 0x0000001fU))


/*	Register lcdc_palt_size	*/

/*	 Fields size	 */
#define SIZE_WIDTH                                                            1
#define SIZE_SHIFT                                                            9
#define SIZE_MASK                                                    0x00000200
#define SIZE_RD(src)                                 (((src) & 0x00000200U)>>9)
#define SIZE_SET(dst,src) \
                     (((dst) & ~0x00000200U) | (((u32)(src)<<9) & 0x00000200U))


/*	Register lcdc_ower	*/

/*	 Fields owe_15	 */
#define OWE_15_WIDTH                                                          1
#define OWE_15_SHIFT                                                         15
#define OWE_15_MASK                                                  0x00008000
#define OWE_15_RD(src)                              (((src) & 0x00008000U)>>15)
#define OWE_15_WR(src)                         (((u32)(src)<<15) & 0x00008000U)
#define OWE_15_SET(dst,src) \
                    (((dst) & ~0x00008000U) | (((u32)(src)<<15) & 0x00008000U))


/*	 Fields owe_14	 */
#define OWE_14_WIDTH                                                          1
#define OWE_14_SHIFT                                                         14
#define OWE_14_MASK                                                  0x00004000
#define OWE_14_RD(src)                              (((src) & 0x00004000U)>>14)
#define OWE_14_WR(src)                         (((u32)(src)<<14) & 0x00004000U)
#define OWE_14_SET(dst,src) \
                    (((dst) & ~0x00004000U) | (((u32)(src)<<14) & 0x00004000U))


/*	 Fields owe_13	 */
#define OWE_13_WIDTH                                                          1
#define OWE_13_SHIFT                                                         13
#define OWE_13_MASK                                                  0x00002000
#define OWE_13_RD(src)                              (((src) & 0x00002000U)>>13)
#define OWE_13_WR(src)                         (((u32)(src)<<13) & 0x00002000U)
#define OWE_13_SET(dst,src) \
                    (((dst) & ~0x00002000U) | (((u32)(src)<<13) & 0x00002000U))


/*	 Fields owe_12	 */
#define OWE_12_WIDTH                                                          1
#define OWE_12_SHIFT                                                         12
#define OWE_12_MASK                                                  0x00001000
#define OWE_12_RD(src)                              (((src) & 0x00001000U)>>12)
#define OWE_12_WR(src)                         (((u32)(src)<<12) & 0x00001000U)
#define OWE_12_SET(dst,src) \
                    (((dst) & ~0x00001000U) | (((u32)(src)<<12) & 0x00001000U))


/*	 Fields owe_11	 */
#define OWE_11_WIDTH                                                          1
#define OWE_11_SHIFT                                                         11
#define OWE_11_MASK                                                  0x00000800
#define OWE_11_RD(src)                              (((src) & 0x00000800U)>>11)
#define OWE_11_WR(src)                         (((u32)(src)<<11) & 0x00000800U)
#define OWE_11_SET(dst,src) \
                    (((dst) & ~0x00000800U) | (((u32)(src)<<11) & 0x00000800U))


/*	 Fields owe_10	 */
#define OWE_10_WIDTH                                                          1
#define OWE_10_SHIFT                                                         10
#define OWE_10_MASK                                                  0x00000400
#define OWE_10_RD(src)                              (((src) & 0x00000400U)>>10)
#define OWE_10_WR(src)                         (((u32)(src)<<10) & 0x00000400U)
#define OWE_10_SET(dst,src) \
                    (((dst) & ~0x00000400U) | (((u32)(src)<<10) & 0x00000400U))


/*	 Fields owe_09	 */
#define OWE_09_WIDTH                                                          1
#define OWE_09_SHIFT                                                          9
#define OWE_09_MASK                                                  0x00000200
#define OWE_09_RD(src)                              (((src) & 0x00000200U)>>09)
#define OWE_09_WR(src)                         (((u32)(src)<<09) & 0x00000200U)
#define OWE_09_SET(dst,src) \
                    (((dst) & ~0x00000200U) | (((u32)(src)<<09) & 0x00000200U))


/*	 Fields owe_08	 */
#define OWE_08_WIDTH                                                          1
#define OWE_08_SHIFT                                                          8
#define OWE_08_MASK                                                  0x00000100
#define OWE_08_RD(src)                              (((src) & 0x00000100U)>>08)
#define OWE_08_WR(src)                         (((u32)(src)<<08) & 0x00000100U)
#define OWE_08_SET(dst,src) \
                    (((dst) & ~0x00000100U) | (((u32)(src)<<08) & 0x00000100U))


/*	 Fields owe_07	 */
#define OWE_07_WIDTH                                                          1
#define OWE_07_SHIFT                                                          7
#define OWE_07_MASK                                                  0x00000080
#define OWE_07_RD(src)                              (((src) & 0x00000080U)>>07)
#define OWE_07_WR(src)                         (((u32)(src)<<07) & 0x00000080U)
#define OWE_07_SET(dst,src) \
                    (((dst) & ~0x00000080U) | (((u32)(src)<<07) & 0x00000080U))


/*	 Fields owe_06	 */
#define OWE_06_WIDTH                                                          1
#define OWE_06_SHIFT                                                          6
#define OWE_06_MASK                                                  0x00000040
#define OWE_06_RD(src)                              (((src) & 0x00000040U)>>06)
#define OWE_06_WR(src)                         (((u32)(src)<<06) & 0x00000040U)
#define OWE_06_SET(dst,src) \
                    (((dst) & ~0x00000040U) | (((u32)(src)<<06) & 0x00000040U))


/*	 Fields owe_05	 */
#define OWE_05_WIDTH                                                          1
#define OWE_05_SHIFT                                                          5
#define OWE_05_MASK                                                  0x00000020
#define OWE_05_RD(src)                              (((src) & 0x00000020U)>>05)
#define OWE_05_WR(src)                         (((u32)(src)<<05) & 0x00000020U)
#define OWE_05_SET(dst,src) \
                    (((dst) & ~0x00000020U) | (((u32)(src)<<05) & 0x00000020U))


/*	 Fields owe_04	 */
#define OWE_04_WIDTH                                                          1
#define OWE_04_SHIFT                                                          4
#define OWE_04_MASK                                                  0x00000010
#define OWE_04_RD(src)                              (((src) & 0x00000010U)>>04)
#define OWE_04_WR(src)                         (((u32)(src)<<04) & 0x00000010U)
#define OWE_04_SET(dst,src) \
                    (((dst) & ~0x00000010U) | (((u32)(src)<<04) & 0x00000010U))


/*	 Fields owe_03	 */
#define OWE_03_WIDTH                                                          1
#define OWE_03_SHIFT                                                          3
#define OWE_03_MASK                                                  0x00000008
#define OWE_03_RD(src)                              (((src) & 0x00000008U)>>03)
#define OWE_03_WR(src)                         (((u32)(src)<<03) & 0x00000008U)
#define OWE_03_SET(dst,src) \
                    (((dst) & ~0x00000008U) | (((u32)(src)<<03) & 0x00000008U))


/*	 Fields owe_02	 */
#define OWE_02_WIDTH                                                          1
#define OWE_02_SHIFT                                                          2
#define OWE_02_MASK                                                  0x00000004
#define OWE_02_RD(src)                              (((src) & 0x00000004U)>>02)
#define OWE_02_WR(src)                         (((u32)(src)<<02) & 0x00000004U)
#define OWE_02_SET(dst,src) \
                    (((dst) & ~0x00000004U) | (((u32)(src)<<02) & 0x00000004U))


/*	 Fields owe_01	 */
#define OWE_01_WIDTH                                                          1
#define OWE_01_SHIFT                                                          1
#define OWE_01_MASK                                                  0x00000002
#define OWE_01_RD(src)                              (((src) & 0x00000002U)>>01)
#define OWE_01_WR(src)                         (((u32)(src)<<01) & 0x00000002U)
#define OWE_01_SET(dst,src) \
                    (((dst) & ~0x00000002U) | (((u32)(src)<<01) & 0x00000002U))


/*	 Fields owe_00	 */
#define OWE_00_WIDTH                                                          1
#define OWE_00_SHIFT                                                          0
#define OWE_00_MASK                                                  0x00000001
#define OWE_00_RD(src)                                  (((src) & 0x00000001U))
#define OWE_00_WR(src)                             (((u32)(src)) & 0x00000001U)
#define OWE_00_SET(dst,src) \
                        (((dst) & ~0x00000001U) | (((u32)(src)) & 0x00000001U))


/*	Register lcdc_oxser0	*/

/*	 Fields ow_x_start	 */
#define OW_X_START0_WIDTH                                                    11
#define OW_X_START0_SHIFT                                                    16
#define OW_X_START0_MASK                                             0x07ff0000
#define OW_X_START0_RD(src)                         (((src) & 0x07ff0000U)>>16)
#define OW_X_START0_WR(src)                    (((u32)(src)<<16) & 0x07ff0000U)
#define OW_X_START0_SET(dst,src) \
                    (((dst) & ~0x07ff0000U) | (((u32)(src)<<16) & 0x07ff0000U))


/*	 Fields ow_x_end	 */
#define OW_X_END0_WIDTH                                                      11
#define OW_X_END0_SHIFT                                                       0
#define OW_X_END0_MASK                                               0x000007ff
#define OW_X_END0_RD(src)                               (((src) & 0x000007ffU))
#define OW_X_END0_WR(src)                          (((u32)(src)) & 0x000007ffU)
#define OW_X_END0_SET(dst,src) \
                        (((dst) & ~0x000007ffU) | (((u32)(src)) & 0x000007ffU))


/*	Register lcdc_oyser0	*/

/*	 Fields ow_y_start	 */
#define OW_Y_START0_WIDTH                                                    11
#define OW_Y_START0_SHIFT                                                    16
#define OW_Y_START0_MASK                                             0x07ff0000
#define OW_Y_START0_RD(src)                         (((src) & 0x07ff0000U)>>16)
#define OW_Y_START0_WR(src)                    (((u32)(src)<<16) & 0x07ff0000U)
#define OW_Y_START0_SET(dst,src) \
                    (((dst) & ~0x07ff0000U) | (((u32)(src)<<16) & 0x07ff0000U))


/*	 Fields ow_y_end	 */
#define OW_Y_END0_WIDTH                                                      11
#define OW_Y_END0_SHIFT                                                       0
#define OW_Y_END0_MASK                                               0x000007ff
#define OW_Y_END0_RD(src)                               (((src) & 0x000007ffU))
#define OW_Y_END0_WR(src)                          (((u32)(src)) & 0x000007ffU)
#define OW_Y_END0_SET(dst,src) \
                        (((dst) & ~0x000007ffU) | (((u32)(src)) & 0x000007ffU))


/*	Register lcdc_owdbar0	*/

/*	 Fields ow_dbar	 */
#define OW_DBAR0_WIDTH                                                       32
#define OW_DBAR0_SHIFT                                                        0
#define OW_DBAR0_MASK                                                0xffffffff
#define OW_DBAR0_RD(src)                                (((src) & 0xffffffffU))
#define OW_DBAR0_WR(src)                           (((u32)(src)) & 0xffffffffU)
#define OW_DBAR0_SET(dst,src) \
                        (((dst) & ~0xffffffffU) | (((u32)(src)) & 0xffffffffU))


/*	Register lcdc_owdcar0	*/

/*	 Fields ow_dcar	 */
#define OW_DCAR0_WIDTH                                                       32
#define OW_DCAR0_SHIFT                                                        0
#define OW_DCAR0_MASK                                                0xffffffff
#define OW_DCAR0_RD(src)                                (((src) & 0xffffffffU))
#define OW_DCAR0_WR(src)                           (((u32)(src)) & 0xffffffffU)
#define OW_DCAR0_SET(dst,src) \
                        (((dst) & ~0xffffffffU) | (((u32)(src)) & 0xffffffffU))


/*	Register lcdc_oxser1	*/

/*	 Fields ow_x_start	 */
#define OW_X_START1_WIDTH                                                    11
#define OW_X_START1_SHIFT                                                    16
#define OW_X_START1_MASK                                             0x07ff0000
#define OW_X_START1_RD(src)                         (((src) & 0x07ff0000U)>>16)
#define OW_X_START1_WR(src)                    (((u32)(src)<<16) & 0x07ff0000U)
#define OW_X_START1_SET(dst,src) \
                    (((dst) & ~0x07ff0000U) | (((u32)(src)<<16) & 0x07ff0000U))


/*	 Fields ow_x_end	 */
#define OW_X_END1_WIDTH                                                      11
#define OW_X_END1_SHIFT                                                       0
#define OW_X_END1_MASK                                               0x000007ff
#define OW_X_END1_RD(src)                               (((src) & 0x000007ffU))
#define OW_X_END1_WR(src)                          (((u32)(src)) & 0x000007ffU)
#define OW_X_END1_SET(dst,src) \
                        (((dst) & ~0x000007ffU) | (((u32)(src)) & 0x000007ffU))


/*	Register lcdc_oyser1	*/

/*	 Fields ow_y_start	 */
#define OW_Y_START1_WIDTH                                                    11
#define OW_Y_START1_SHIFT                                                    16
#define OW_Y_START1_MASK                                             0x07ff0000
#define OW_Y_START1_RD(src)                         (((src) & 0x07ff0000U)>>16)
#define OW_Y_START1_WR(src)                    (((u32)(src)<<16) & 0x07ff0000U)
#define OW_Y_START1_SET(dst,src) \
                    (((dst) & ~0x07ff0000U) | (((u32)(src)<<16) & 0x07ff0000U))


/*	 Fields ow_y_end	 */
#define OW_Y_END1_WIDTH                                                      11
#define OW_Y_END1_SHIFT                                                       0
#define OW_Y_END1_MASK                                               0x000007ff
#define OW_Y_END1_RD(src)                               (((src) & 0x000007ffU))
#define OW_Y_END1_WR(src)                          (((u32)(src)) & 0x000007ffU)
#define OW_Y_END1_SET(dst,src) \
                        (((dst) & ~0x000007ffU) | (((u32)(src)) & 0x000007ffU))


/*	Register lcdc_owdbar1	*/

/*	 Fields OW_DBAR	 */
#define OW_DBAR1_WIDTH                                                       32
#define OW_DBAR1_SHIFT                                                        0
#define OW_DBAR1_MASK                                                0xffffffff
#define OW_DBAR1_RD(src)                                (((src) & 0xffffffffU))
#define OW_DBAR1_WR(src)                           (((u32)(src)) & 0xffffffffU)
#define OW_DBAR1_SET(dst,src) \
                        (((dst) & ~0xffffffffU) | (((u32)(src)) & 0xffffffffU))


/*	Register lcdc_owdcar1	*/

/*	 Fields OW_DCAR	 */
#define OW_DCAR1_WIDTH                                                       32
#define OW_DCAR1_SHIFT                                                        0
#define OW_DCAR1_MASK                                                0xffffffff
#define OW_DCAR1_RD(src)                                (((src) & 0xffffffffU))
#define OW_DCAR1_WR(src)                           (((u32)(src)) & 0xffffffffU)
#define OW_DCAR1_SET(dst,src) \
                        (((dst) & ~0xffffffffU) | (((u32)(src)) & 0xffffffffU))


/*	Register lcdc_oxser2	*/

/*	 Fields ow_x_start	 */
#define OW_X_START2_WIDTH                                                    11
#define OW_X_START2_SHIFT                                                    16
#define OW_X_START2_MASK                                             0x07ff0000
#define OW_X_START2_RD(src)                         (((src) & 0x07ff0000U)>>16)
#define OW_X_START2_WR(src)                    (((u32)(src)<<16) & 0x07ff0000U)
#define OW_X_START2_SET(dst,src) \
                    (((dst) & ~0x07ff0000U) | (((u32)(src)<<16) & 0x07ff0000U))


/*	 Fields ow_x_end	 */
#define OW_X_END2_WIDTH                                                      11
#define OW_X_END2_SHIFT                                                       0
#define OW_X_END2_MASK                                               0x000007ff
#define OW_X_END2_RD(src)                               (((src) & 0x000007ffU))
#define OW_X_END2_WR(src)                          (((u32)(src)) & 0x000007ffU)
#define OW_X_END2_SET(dst,src) \
                        (((dst) & ~0x000007ffU) | (((u32)(src)) & 0x000007ffU))


/*	Register lcdc_oyser2	*/

/*	 Fields ow_y_start	 */
#define OW_Y_START2_WIDTH                                                    11
#define OW_Y_START2_SHIFT                                                    16
#define OW_Y_START2_MASK                                             0x07ff0000
#define OW_Y_START2_RD(src)                         (((src) & 0x07ff0000U)>>16)
#define OW_Y_START2_WR(src)                    (((u32)(src)<<16) & 0x07ff0000U)
#define OW_Y_START2_SET(dst,src) \
                    (((dst) & ~0x07ff0000U) | (((u32)(src)<<16) & 0x07ff0000U))


/*	 Fields ow_y_end	 */
#define OW_Y_END2_WIDTH                                                      11
#define OW_Y_END2_SHIFT                                                       0
#define OW_Y_END2_MASK                                               0x000007ff
#define OW_Y_END2_RD(src)                               (((src) & 0x000007ffU))
#define OW_Y_END2_WR(src)                          (((u32)(src)) & 0x000007ffU)
#define OW_Y_END2_SET(dst,src) \
                        (((dst) & ~0x000007ffU) | (((u32)(src)) & 0x000007ffU))


/*	Register lcdc_owdbar2	*/

/*	 Fields OW_DBAR	 */
#define OW_DBAR2_WIDTH                                                       32
#define OW_DBAR2_SHIFT                                                        0
#define OW_DBAR2_MASK                                                0xffffffff
#define OW_DBAR2_RD(src)                                (((src) & 0xffffffffU))
#define OW_DBAR2_WR(src)                           (((u32)(src)) & 0xffffffffU)
#define OW_DBAR2_SET(dst,src) \
                        (((dst) & ~0xffffffffU) | (((u32)(src)) & 0xffffffffU))


/*	Register lcdc_owdcar2	*/

/*	 Fields OW_DCAR	 */
#define OW_DCAR2_WIDTH                                                       32
#define OW_DCAR2_SHIFT                                                        0
#define OW_DCAR2_MASK                                                0xffffffff
#define OW_DCAR2_RD(src)                                (((src) & 0xffffffffU))
#define OW_DCAR2_WR(src)                           (((u32)(src)) & 0xffffffffU)
#define OW_DCAR2_SET(dst,src) \
                        (((dst) & ~0xffffffffU) | (((u32)(src)) & 0xffffffffU))


/*	Register lcdc_oxser3	*/

/*	 Fields ow_x_start	 */
#define OW_X_START3_WIDTH                                                    11
#define OW_X_START3_SHIFT                                                    16
#define OW_X_START3_MASK                                             0x07ff0000
#define OW_X_START3_RD(src)                         (((src) & 0x07ff0000U)>>16)
#define OW_X_START3_WR(src)                    (((u32)(src)<<16) & 0x07ff0000U)
#define OW_X_START3_SET(dst,src) \
                    (((dst) & ~0x07ff0000U) | (((u32)(src)<<16) & 0x07ff0000U))


/*	 Fields ow_x_end	 */
#define OW_X_END3_WIDTH                                                      11
#define OW_X_END3_SHIFT                                                       0
#define OW_X_END3_MASK                                               0x000007ff
#define OW_X_END3_RD(src)                               (((src) & 0x000007ffU))
#define OW_X_END3_WR(src)                          (((u32)(src)) & 0x000007ffU)
#define OW_X_END3_SET(dst,src) \
                        (((dst) & ~0x000007ffU) | (((u32)(src)) & 0x000007ffU))


/*	Register lcdc_oyser3	*/

/*	 Fields ow_y_start	 */
#define OW_Y_START3_WIDTH                                                    11
#define OW_Y_START3_SHIFT                                                    16
#define OW_Y_START3_MASK                                             0x07ff0000
#define OW_Y_START3_RD(src)                         (((src) & 0x07ff0000U)>>16)
#define OW_Y_START3_WR(src)                    (((u32)(src)<<16) & 0x07ff0000U)
#define OW_Y_START3_SET(dst,src) \
                    (((dst) & ~0x07ff0000U) | (((u32)(src)<<16) & 0x07ff0000U))


/*	 Fields ow_y_end	 */
#define OW_Y_END3_WIDTH                                                      11
#define OW_Y_END3_SHIFT                                                       0
#define OW_Y_END3_MASK                                               0x000007ff
#define OW_Y_END3_RD(src)                               (((src) & 0x000007ffU))
#define OW_Y_END3_WR(src)                          (((u32)(src)) & 0x000007ffU)
#define OW_Y_END3_SET(dst,src) \
                        (((dst) & ~0x000007ffU) | (((u32)(src)) & 0x000007ffU))


/*	Register lcdc_owdbar3	*/

/*	 Fields OW_DBAR	 */
#define OW_DBAR3_WIDTH                                                       32
#define OW_DBAR3_SHIFT                                                        0
#define OW_DBAR3_MASK                                                0xffffffff
#define OW_DBAR3_RD(src)                                (((src) & 0xffffffffU))
#define OW_DBAR3_WR(src)                           (((u32)(src)) & 0xffffffffU)
#define OW_DBAR3_SET(dst,src) \
                        (((dst) & ~0xffffffffU) | (((u32)(src)) & 0xffffffffU))


/*	Register lcdc_owdcar3	*/

/*	 Fields OW_DCAR	 */
#define OW_DCAR3_WIDTH                                                       32
#define OW_DCAR3_SHIFT                                                        0
#define OW_DCAR3_MASK                                                0xffffffff
#define OW_DCAR3_RD(src)                                (((src) & 0xffffffffU))
#define OW_DCAR3_WR(src)                           (((u32)(src)) & 0xffffffffU)
#define OW_DCAR3_SET(dst,src) \
                        (((dst) & ~0xffffffffU) | (((u32)(src)) & 0xffffffffU))


/*	Register lcdc_oxser4	*/

/*	 Fields ow_x_start	 */
#define OW_X_START4_WIDTH                                                    11
#define OW_X_START4_SHIFT                                                    16
#define OW_X_START4_MASK                                             0x07ff0000
#define OW_X_START4_RD(src)                         (((src) & 0x07ff0000U)>>16)
#define OW_X_START4_WR(src)                    (((u32)(src)<<16) & 0x07ff0000U)
#define OW_X_START4_SET(dst,src) \
                    (((dst) & ~0x07ff0000U) | (((u32)(src)<<16) & 0x07ff0000U))


/*	 Fields ow_x_end	 */
#define OW_X_END4_WIDTH                                                      11
#define OW_X_END4_SHIFT                                                       0
#define OW_X_END4_MASK                                               0x000007ff
#define OW_X_END4_RD(src)                               (((src) & 0x000007ffU))
#define OW_X_END4_WR(src)                          (((u32)(src)) & 0x000007ffU)
#define OW_X_END4_SET(dst,src) \
                        (((dst) & ~0x000007ffU) | (((u32)(src)) & 0x000007ffU))


/*	Register lcdc_oyser4	*/

/*	 Fields ow_y_start	 */
#define OW_Y_START4_WIDTH                                                    11
#define OW_Y_START4_SHIFT                                                    16
#define OW_Y_START4_MASK                                             0x07ff0000
#define OW_Y_START4_RD(src)                         (((src) & 0x07ff0000U)>>16)
#define OW_Y_START4_WR(src)                    (((u32)(src)<<16) & 0x07ff0000U)
#define OW_Y_START4_SET(dst,src) \
                    (((dst) & ~0x07ff0000U) | (((u32)(src)<<16) & 0x07ff0000U))


/*	 Fields ow_y_end	 */
#define OW_Y_END4_WIDTH                                                      11
#define OW_Y_END4_SHIFT                                                       0
#define OW_Y_END4_MASK                                               0x000007ff
#define OW_Y_END4_RD(src)                               (((src) & 0x000007ffU))
#define OW_Y_END4_WR(src)                          (((u32)(src)) & 0x000007ffU)
#define OW_Y_END4_SET(dst,src) \
                        (((dst) & ~0x000007ffU) | (((u32)(src)) & 0x000007ffU))


/*	Register lcdc_owdbar4	*/

/*	 Fields OW_DBAR	 */
#define OW_DBAR4_WIDTH                                                       32
#define OW_DBAR4_SHIFT                                                        0
#define OW_DBAR4_MASK                                                0xffffffff
#define OW_DBAR4_RD(src)                                (((src) & 0xffffffffU))
#define OW_DBAR4_WR(src)                           (((u32)(src)) & 0xffffffffU)
#define OW_DBAR4_SET(dst,src) \
                        (((dst) & ~0xffffffffU) | (((u32)(src)) & 0xffffffffU))


/*	Register lcdc_owdcar4	*/

/*	 Fields OW_DCAR	 */
#define OW_DCAR4_WIDTH                                                       32
#define OW_DCAR4_SHIFT                                                        0
#define OW_DCAR4_MASK                                                0xffffffff
#define OW_DCAR4_RD(src)                                (((src) & 0xffffffffU))
#define OW_DCAR4_WR(src)                           (((u32)(src)) & 0xffffffffU)
#define OW_DCAR4_SET(dst,src) \
                        (((dst) & ~0xffffffffU) | (((u32)(src)) & 0xffffffffU))


/*	Register lcdc_oxser5	*/

/*	 Fields ow_x_start	 */
#define OW_X_START5_WIDTH                                                    11
#define OW_X_START5_SHIFT                                                    16
#define OW_X_START5_MASK                                             0x07ff0000
#define OW_X_START5_RD(src)                         (((src) & 0x07ff0000U)>>16)
#define OW_X_START5_WR(src)                    (((u32)(src)<<16) & 0x07ff0000U)
#define OW_X_START5_SET(dst,src) \
                    (((dst) & ~0x07ff0000U) | (((u32)(src)<<16) & 0x07ff0000U))


/*	 Fields ow_x_end	 */
#define OW_X_END5_WIDTH                                                      11
#define OW_X_END5_SHIFT                                                       0
#define OW_X_END5_MASK                                               0x000007ff
#define OW_X_END5_RD(src)                               (((src) & 0x000007ffU))
#define OW_X_END5_WR(src)                          (((u32)(src)) & 0x000007ffU)
#define OW_X_END5_SET(dst,src) \
                        (((dst) & ~0x000007ffU) | (((u32)(src)) & 0x000007ffU))


/*	Register lcdc_oyser5	*/

/*	 Fields ow_y_start	 */
#define OW_Y_START5_WIDTH                                                    11
#define OW_Y_START5_SHIFT                                                    16
#define OW_Y_START5_MASK                                             0x07ff0000
#define OW_Y_START5_RD(src)                         (((src) & 0x07ff0000U)>>16)
#define OW_Y_START5_WR(src)                    (((u32)(src)<<16) & 0x07ff0000U)
#define OW_Y_START5_SET(dst,src) \
                    (((dst) & ~0x07ff0000U) | (((u32)(src)<<16) & 0x07ff0000U))


/*	 Fields ow_y_end	 */
#define OW_Y_END5_WIDTH                                                      11
#define OW_Y_END5_SHIFT                                                       0
#define OW_Y_END5_MASK                                               0x000007ff
#define OW_Y_END5_RD(src)                               (((src) & 0x000007ffU))
#define OW_Y_END5_WR(src)                          (((u32)(src)) & 0x000007ffU)
#define OW_Y_END5_SET(dst,src) \
                        (((dst) & ~0x000007ffU) | (((u32)(src)) & 0x000007ffU))


/*	Register lcdc_owdbar5	*/

/*	 Fields OW_DBAR	 */
#define OW_DBAR5_WIDTH                                                       32
#define OW_DBAR5_SHIFT                                                        0
#define OW_DBAR5_MASK                                                0xffffffff
#define OW_DBAR5_RD(src)                                (((src) & 0xffffffffU))
#define OW_DBAR5_WR(src)                           (((u32)(src)) & 0xffffffffU)
#define OW_DBAR5_SET(dst,src) \
                        (((dst) & ~0xffffffffU) | (((u32)(src)) & 0xffffffffU))


/*	Register lcdc_owdcar5	*/

/*	 Fields OW_DCAR	 */
#define OW_DCAR5_WIDTH                                                       32
#define OW_DCAR5_SHIFT                                                        0
#define OW_DCAR5_MASK                                                0xffffffff
#define OW_DCAR5_RD(src)                                (((src) & 0xffffffffU))
#define OW_DCAR5_WR(src)                           (((u32)(src)) & 0xffffffffU)
#define OW_DCAR5_SET(dst,src) \
                        (((dst) & ~0xffffffffU) | (((u32)(src)) & 0xffffffffU))


/*	Register lcdc_oxser6	*/

/*	 Fields ow_x_start	 */
#define OW_X_START6_WIDTH                                                    11
#define OW_X_START6_SHIFT                                                    16
#define OW_X_START6_MASK                                             0x07ff0000
#define OW_X_START6_RD(src)                         (((src) & 0x07ff0000U)>>16)
#define OW_X_START6_WR(src)                    (((u32)(src)<<16) & 0x07ff0000U)
#define OW_X_START6_SET(dst,src) \
                    (((dst) & ~0x07ff0000U) | (((u32)(src)<<16) & 0x07ff0000U))


/*	 Fields ow_x_end	 */
#define OW_X_END6_WIDTH                                                      11
#define OW_X_END6_SHIFT                                                       0
#define OW_X_END6_MASK                                               0x000007ff
#define OW_X_END6_RD(src)                               (((src) & 0x000007ffU))
#define OW_X_END6_WR(src)                          (((u32)(src)) & 0x000007ffU)
#define OW_X_END6_SET(dst,src) \
                        (((dst) & ~0x000007ffU) | (((u32)(src)) & 0x000007ffU))


/*	Register lcdc_oyser6	*/

/*	 Fields ow_y_start	 */
#define OW_Y_START6_WIDTH                                                    11
#define OW_Y_START6_SHIFT                                                    16
#define OW_Y_START6_MASK                                             0x07ff0000
#define OW_Y_START6_RD(src)                         (((src) & 0x07ff0000U)>>16)
#define OW_Y_START6_WR(src)                    (((u32)(src)<<16) & 0x07ff0000U)
#define OW_Y_START6_SET(dst,src) \
                    (((dst) & ~0x07ff0000U) | (((u32)(src)<<16) & 0x07ff0000U))


/*	 Fields ow_y_end	 */
#define OW_Y_END6_WIDTH                                                      11
#define OW_Y_END6_SHIFT                                                       0
#define OW_Y_END6_MASK                                               0x000007ff
#define OW_Y_END6_RD(src)                               (((src) & 0x000007ffU))
#define OW_Y_END6_WR(src)                          (((u32)(src)) & 0x000007ffU)
#define OW_Y_END6_SET(dst,src) \
                        (((dst) & ~0x000007ffU) | (((u32)(src)) & 0x000007ffU))


/*	Register lcdc_owdbar6	*/

/*	 Fields OW_DBAR	 */
#define OW_DBAR6_WIDTH                                                       32
#define OW_DBAR6_SHIFT                                                        0
#define OW_DBAR6_MASK                                                0xffffffff
#define OW_DBAR6_RD(src)                                (((src) & 0xffffffffU))
#define OW_DBAR6_WR(src)                           (((u32)(src)) & 0xffffffffU)
#define OW_DBAR6_SET(dst,src) \
                        (((dst) & ~0xffffffffU) | (((u32)(src)) & 0xffffffffU))


/*	Register lcdc_owdcar6	*/

/*	 Fields OW_DCAR	 */
#define OW_DCAR6_WIDTH                                                       32
#define OW_DCAR6_SHIFT                                                        0
#define OW_DCAR6_MASK                                                0xffffffff
#define OW_DCAR6_RD(src)                                (((src) & 0xffffffffU))
#define OW_DCAR6_WR(src)                           (((u32)(src)) & 0xffffffffU)
#define OW_DCAR6_SET(dst,src) \
                        (((dst) & ~0xffffffffU) | (((u32)(src)) & 0xffffffffU))


/*	Register lcdc_oxser7	*/

/*	 Fields ow_x_start	 */
#define OW_X_START7_WIDTH                                                    11
#define OW_X_START7_SHIFT                                                    16
#define OW_X_START7_MASK                                             0x07ff0000
#define OW_X_START7_RD(src)                         (((src) & 0x07ff0000U)>>16)
#define OW_X_START7_WR(src)                    (((u32)(src)<<16) & 0x07ff0000U)
#define OW_X_START7_SET(dst,src) \
                    (((dst) & ~0x07ff0000U) | (((u32)(src)<<16) & 0x07ff0000U))


/*	 Fields ow_x_end	 */
#define OW_X_END7_WIDTH                                                      11
#define OW_X_END7_SHIFT                                                       0
#define OW_X_END7_MASK                                               0x000007ff
#define OW_X_END7_RD(src)                               (((src) & 0x000007ffU))
#define OW_X_END7_WR(src)                          (((u32)(src)) & 0x000007ffU)
#define OW_X_END7_SET(dst,src) \
                        (((dst) & ~0x000007ffU) | (((u32)(src)) & 0x000007ffU))


/*	Register lcdc_oyser7	*/

/*	 Fields ow_y_start	 */
#define OW_Y_START7_WIDTH                                                    11
#define OW_Y_START7_SHIFT                                                    16
#define OW_Y_START7_MASK                                             0x07ff0000
#define OW_Y_START7_RD(src)                         (((src) & 0x07ff0000U)>>16)
#define OW_Y_START7_WR(src)                    (((u32)(src)<<16) & 0x07ff0000U)
#define OW_Y_START7_SET(dst,src) \
                    (((dst) & ~0x07ff0000U) | (((u32)(src)<<16) & 0x07ff0000U))


/*	 Fields ow_y_end	 */
#define OW_Y_END7_WIDTH                                                      11
#define OW_Y_END7_SHIFT                                                       0
#define OW_Y_END7_MASK                                               0x000007ff
#define OW_Y_END7_RD(src)                               (((src) & 0x000007ffU))
#define OW_Y_END7_WR(src)                          (((u32)(src)) & 0x000007ffU)
#define OW_Y_END7_SET(dst,src) \
                        (((dst) & ~0x000007ffU) | (((u32)(src)) & 0x000007ffU))


/*	Register lcdc_owdbar7	*/

/*	 Fields OW_DBAR	 */
#define OW_DBAR7_WIDTH                                                       32
#define OW_DBAR7_SHIFT                                                        0
#define OW_DBAR7_MASK                                                0xffffffff
#define OW_DBAR7_RD(src)                                (((src) & 0xffffffffU))
#define OW_DBAR7_WR(src)                           (((u32)(src)) & 0xffffffffU)
#define OW_DBAR7_SET(dst,src) \
                        (((dst) & ~0xffffffffU) | (((u32)(src)) & 0xffffffffU))


/*	Register lcdc_owdcar7	*/

/*	 Fields OW_DCAR	 */
#define OW_DCAR7_WIDTH                                                       32
#define OW_DCAR7_SHIFT                                                        0
#define OW_DCAR7_MASK                                                0xffffffff
#define OW_DCAR7_RD(src)                                (((src) & 0xffffffffU))
#define OW_DCAR7_WR(src)                           (((u32)(src)) & 0xffffffffU)
#define OW_DCAR7_SET(dst,src) \
                        (((dst) & ~0xffffffffU) | (((u32)(src)) & 0xffffffffU))


/*	Register lcdc_oxser8	*/

/*	 Fields ow_x_start	 */
#define OW_X_START8_WIDTH                                                    11
#define OW_X_START8_SHIFT                                                    16
#define OW_X_START8_MASK                                             0x07ff0000
#define OW_X_START8_RD(src)                         (((src) & 0x07ff0000U)>>16)
#define OW_X_START8_WR(src)                    (((u32)(src)<<16) & 0x07ff0000U)
#define OW_X_START8_SET(dst,src) \
                    (((dst) & ~0x07ff0000U) | (((u32)(src)<<16) & 0x07ff0000U))


/*	 Fields ow_x_end	 */
#define OW_X_END8_WIDTH                                                      11
#define OW_X_END8_SHIFT                                                       0
#define OW_X_END8_MASK                                               0x000007ff
#define OW_X_END8_RD(src)                               (((src) & 0x000007ffU))
#define OW_X_END8_WR(src)                          (((u32)(src)) & 0x000007ffU)
#define OW_X_END8_SET(dst,src) \
                        (((dst) & ~0x000007ffU) | (((u32)(src)) & 0x000007ffU))


/*	Register lcdc_oyser8	*/

/*	 Fields ow_y_start	 */
#define OW_Y_START8_WIDTH                                                    11
#define OW_Y_START8_SHIFT                                                    16
#define OW_Y_START8_MASK                                             0x07ff0000
#define OW_Y_START8_RD(src)                         (((src) & 0x07ff0000U)>>16)
#define OW_Y_START8_WR(src)                    (((u32)(src)<<16) & 0x07ff0000U)
#define OW_Y_START8_SET(dst,src) \
                    (((dst) & ~0x07ff0000U) | (((u32)(src)<<16) & 0x07ff0000U))


/*	 Fields ow_y_end	 */
#define OW_Y_END8_WIDTH                                                      11
#define OW_Y_END8_SHIFT                                                       0
#define OW_Y_END8_MASK                                               0x000007ff
#define OW_Y_END8_RD(src)                               (((src) & 0x000007ffU))
#define OW_Y_END8_WR(src)                          (((u32)(src)) & 0x000007ffU)
#define OW_Y_END8_SET(dst,src) \
                        (((dst) & ~0x000007ffU) | (((u32)(src)) & 0x000007ffU))


/*	Register lcdc_owdbar8	*/

/*	 Fields ow_dbar	 */
#define OW_DBAR8_WIDTH                                                       32
#define OW_DBAR8_SHIFT                                                        0
#define OW_DBAR8_MASK                                                0xffffffff
#define OW_DBAR8_RD(src)                                (((src) & 0xffffffffU))
#define OW_DBAR8_WR(src)                           (((u32)(src)) & 0xffffffffU)
#define OW_DBAR8_SET(dst,src) \
                        (((dst) & ~0xffffffffU) | (((u32)(src)) & 0xffffffffU))


/*	Register lcdc_owdcar8	*/

/*	 Fields OW_DCAR	 */
#define OW_DCAR8_WIDTH                                                       32
#define OW_DCAR8_SHIFT                                                        0
#define OW_DCAR8_MASK                                                0xffffffff
#define OW_DCAR8_RD(src)                                (((src) & 0xffffffffU))
#define OW_DCAR8_WR(src)                           (((u32)(src)) & 0xffffffffU)
#define OW_DCAR8_SET(dst,src) \
                        (((dst) & ~0xffffffffU) | (((u32)(src)) & 0xffffffffU))


/*	Register lcdc_oxser9	*/

/*	 Fields ow_x_start	 */
#define OW_X_START9_WIDTH                                                    11
#define OW_X_START9_SHIFT                                                    16
#define OW_X_START9_MASK                                             0x07ff0000
#define OW_X_START9_RD(src)                         (((src) & 0x07ff0000U)>>16)
#define OW_X_START9_WR(src)                    (((u32)(src)<<16) & 0x07ff0000U)
#define OW_X_START9_SET(dst,src) \
                    (((dst) & ~0x07ff0000U) | (((u32)(src)<<16) & 0x07ff0000U))


/*	 Fields ow_x_end	 */
#define OW_X_END9_WIDTH                                                      11
#define OW_X_END9_SHIFT                                                       0
#define OW_X_END9_MASK                                               0x000007ff
#define OW_X_END9_RD(src)                               (((src) & 0x000007ffU))
#define OW_X_END9_WR(src)                          (((u32)(src)) & 0x000007ffU)
#define OW_X_END9_SET(dst,src) \
                        (((dst) & ~0x000007ffU) | (((u32)(src)) & 0x000007ffU))


/*	Register lcdc_oyser9	*/

/*	 Fields ow_y_start	 */
#define OW_Y_START9_WIDTH                                                    11
#define OW_Y_START9_SHIFT                                                    16
#define OW_Y_START9_MASK                                             0x07ff0000
#define OW_Y_START9_RD(src)                         (((src) & 0x07ff0000U)>>16)
#define OW_Y_START9_WR(src)                    (((u32)(src)<<16) & 0x07ff0000U)
#define OW_Y_START9_SET(dst,src) \
                    (((dst) & ~0x07ff0000U) | (((u32)(src)<<16) & 0x07ff0000U))


/*	 Fields ow_y_end	 */
#define OW_Y_END9_WIDTH                                                      11
#define OW_Y_END9_SHIFT                                                       0
#define OW_Y_END9_MASK                                               0x000007ff
#define OW_Y_END9_RD(src)                               (((src) & 0x000007ffU))
#define OW_Y_END9_WR(src)                          (((u32)(src)) & 0x000007ffU)
#define OW_Y_END9_SET(dst,src) \
                        (((dst) & ~0x000007ffU) | (((u32)(src)) & 0x000007ffU))


/*	Register lcdc_owdbar9	*/

/*	 Fields OW_DBAR	 */
#define OW_DBAR9_WIDTH                                                       32
#define OW_DBAR9_SHIFT                                                        0
#define OW_DBAR9_MASK                                                0xffffffff
#define OW_DBAR9_RD(src)                                (((src) & 0xffffffffU))
#define OW_DBAR9_WR(src)                           (((u32)(src)) & 0xffffffffU)
#define OW_DBAR9_SET(dst,src) \
                        (((dst) & ~0xffffffffU) | (((u32)(src)) & 0xffffffffU))


/*	Register lcdc_owdcar9	*/

/*	 Fields OW_DCAR	 */
#define OW_DCAR9_WIDTH                                                       32
#define OW_DCAR9_SHIFT                                                        0
#define OW_DCAR9_MASK                                                0xffffffff
#define OW_DCAR9_RD(src)                                (((src) & 0xffffffffU))
#define OW_DCAR9_WR(src)                           (((u32)(src)) & 0xffffffffU)
#define OW_DCAR9_SET(dst,src) \
                        (((dst) & ~0xffffffffU) | (((u32)(src)) & 0xffffffffU))


/*	Register lcdc_oxser10	*/

/*	 Fields ow_x_start	 */
#define OW_X_START0_WIDTH                                                    11
#define OW_X_START0_SHIFT                                                    16
#define OW_X_START0_MASK                                             0x07ff0000
#define OW_X_START0_RD(src)                         (((src) & 0x07ff0000U)>>16)
#define OW_X_START0_WR(src)                    (((u32)(src)<<16) & 0x07ff0000U)
#define OW_X_START0_SET(dst,src) \
                    (((dst) & ~0x07ff0000U) | (((u32)(src)<<16) & 0x07ff0000U))


/*	 Fields ow_x_end	 */
#define OW_X_END0_WIDTH                                                      11
#define OW_X_END0_SHIFT                                                       0
#define OW_X_END0_MASK                                               0x000007ff
#define OW_X_END0_RD(src)                               (((src) & 0x000007ffU))
#define OW_X_END0_WR(src)                          (((u32)(src)) & 0x000007ffU)
#define OW_X_END0_SET(dst,src) \
                        (((dst) & ~0x000007ffU) | (((u32)(src)) & 0x000007ffU))


/*	Register lcdc_oyser10	*/

/*	 Fields ow_y_start	 */
#define OW_Y_START0_WIDTH                                                    11
#define OW_Y_START0_SHIFT                                                    16
#define OW_Y_START0_MASK                                             0x07ff0000
#define OW_Y_START0_RD(src)                         (((src) & 0x07ff0000U)>>16)
#define OW_Y_START0_WR(src)                    (((u32)(src)<<16) & 0x07ff0000U)
#define OW_Y_START0_SET(dst,src) \
                    (((dst) & ~0x07ff0000U) | (((u32)(src)<<16) & 0x07ff0000U))


/*	 Fields ow_y_end	 */
#define OW_Y_END0_WIDTH                                                      11
#define OW_Y_END0_SHIFT                                                       0
#define OW_Y_END0_MASK                                               0x000007ff
#define OW_Y_END0_RD(src)                               (((src) & 0x000007ffU))
#define OW_Y_END0_WR(src)                          (((u32)(src)) & 0x000007ffU)
#define OW_Y_END0_SET(dst,src) \
                        (((dst) & ~0x000007ffU) | (((u32)(src)) & 0x000007ffU))


/*	Register lcdc_owdbar10	*/

/*	 Fields OW_DBAR	 */
#define OW_DBAR0_WIDTH                                                       32
#define OW_DBAR0_SHIFT                                                        0
#define OW_DBAR0_MASK                                                0xffffffff
#define OW_DBAR0_RD(src)                                (((src) & 0xffffffffU))
#define OW_DBAR0_WR(src)                           (((u32)(src)) & 0xffffffffU)
#define OW_DBAR0_SET(dst,src) \
                        (((dst) & ~0xffffffffU) | (((u32)(src)) & 0xffffffffU))


/*	Register lcdc_owdcar10	*/

/*	 Fields OW_DCAR	 */
#define OW_DCAR0_WIDTH                                                       32
#define OW_DCAR0_SHIFT                                                        0
#define OW_DCAR0_MASK                                                0xffffffff
#define OW_DCAR0_RD(src)                                (((src) & 0xffffffffU))
#define OW_DCAR0_WR(src)                           (((u32)(src)) & 0xffffffffU)
#define OW_DCAR0_SET(dst,src) \
                        (((dst) & ~0xffffffffU) | (((u32)(src)) & 0xffffffffU))


/*	Register lcdc_oxser11	*/

/*	 Fields ow_x_start	 */
#define OW_X_START1_WIDTH                                                    11
#define OW_X_START1_SHIFT                                                    16
#define OW_X_START1_MASK                                             0x07ff0000
#define OW_X_START1_RD(src)                         (((src) & 0x07ff0000U)>>16)
#define OW_X_START1_WR(src)                    (((u32)(src)<<16) & 0x07ff0000U)
#define OW_X_START1_SET(dst,src) \
                    (((dst) & ~0x07ff0000U) | (((u32)(src)<<16) & 0x07ff0000U))


/*	 Fields ow_x_end	 */
#define OW_X_END1_WIDTH                                                      11
#define OW_X_END1_SHIFT                                                       0
#define OW_X_END1_MASK                                               0x000007ff
#define OW_X_END1_RD(src)                               (((src) & 0x000007ffU))
#define OW_X_END1_WR(src)                          (((u32)(src)) & 0x000007ffU)
#define OW_X_END1_SET(dst,src) \
                        (((dst) & ~0x000007ffU) | (((u32)(src)) & 0x000007ffU))


/*	Register lcdc_oyser11	*/

/*	 Fields ow_y_start	 */
#define OW_Y_START1_WIDTH                                                    11
#define OW_Y_START1_SHIFT                                                    16
#define OW_Y_START1_MASK                                             0x07ff0000
#define OW_Y_START1_RD(src)                         (((src) & 0x07ff0000U)>>16)
#define OW_Y_START1_WR(src)                    (((u32)(src)<<16) & 0x07ff0000U)
#define OW_Y_START1_SET(dst,src) \
                    (((dst) & ~0x07ff0000U) | (((u32)(src)<<16) & 0x07ff0000U))


/*	 Fields ow_y_end	 */
#define OW_Y_END1_WIDTH                                                      11
#define OW_Y_END1_SHIFT                                                       0
#define OW_Y_END1_MASK                                               0x000007ff
#define OW_Y_END1_RD(src)                               (((src) & 0x000007ffU))
#define OW_Y_END1_WR(src)                          (((u32)(src)) & 0x000007ffU)
#define OW_Y_END1_SET(dst,src) \
                        (((dst) & ~0x000007ffU) | (((u32)(src)) & 0x000007ffU))


/*	Register lcdc_owdbar11	*/

/*	 Fields OW_DBAR	 */
#define OW_DBAR1_WIDTH                                                       32
#define OW_DBAR1_SHIFT                                                        0
#define OW_DBAR1_MASK                                                0xffffffff
#define OW_DBAR1_RD(src)                                (((src) & 0xffffffffU))
#define OW_DBAR1_WR(src)                           (((u32)(src)) & 0xffffffffU)
#define OW_DBAR1_SET(dst,src) \
                        (((dst) & ~0xffffffffU) | (((u32)(src)) & 0xffffffffU))


/*	Register lcdc_owdcar11	*/

/*	 Fields OW_DCAR	 */
#define OW_DCAR1_WIDTH                                                       32
#define OW_DCAR1_SHIFT                                                        0
#define OW_DCAR1_MASK                                                0xffffffff
#define OW_DCAR1_RD(src)                                (((src) & 0xffffffffU))
#define OW_DCAR1_WR(src)                           (((u32)(src)) & 0xffffffffU)
#define OW_DCAR1_SET(dst,src) \
                        (((dst) & ~0xffffffffU) | (((u32)(src)) & 0xffffffffU))


/*	Register lcdc_oxser12	*/

/*	 Fields ow_x_start	 */
#define OW_X_START2_WIDTH                                                    11
#define OW_X_START2_SHIFT                                                    16
#define OW_X_START2_MASK                                             0x07ff0000
#define OW_X_START2_RD(src)                         (((src) & 0x07ff0000U)>>16)
#define OW_X_START2_WR(src)                    (((u32)(src)<<16) & 0x07ff0000U)
#define OW_X_START2_SET(dst,src) \
                    (((dst) & ~0x07ff0000U) | (((u32)(src)<<16) & 0x07ff0000U))


/*	 Fields ow_x_end	 */
#define OW_X_END2_WIDTH                                                      11
#define OW_X_END2_SHIFT                                                       0
#define OW_X_END2_MASK                                               0x000007ff
#define OW_X_END2_RD(src)                               (((src) & 0x000007ffU))
#define OW_X_END2_WR(src)                          (((u32)(src)) & 0x000007ffU)
#define OW_X_END2_SET(dst,src) \
                        (((dst) & ~0x000007ffU) | (((u32)(src)) & 0x000007ffU))


/*	Register lcdc_oyser12	*/

/*	 Fields ow_y_start	 */
#define OW_Y_START2_WIDTH                                                    11
#define OW_Y_START2_SHIFT                                                    16
#define OW_Y_START2_MASK                                             0x07ff0000
#define OW_Y_START2_RD(src)                         (((src) & 0x07ff0000U)>>16)
#define OW_Y_START2_WR(src)                    (((u32)(src)<<16) & 0x07ff0000U)
#define OW_Y_START2_SET(dst,src) \
                    (((dst) & ~0x07ff0000U) | (((u32)(src)<<16) & 0x07ff0000U))


/*	 Fields ow_y_end	 */
#define OW_Y_END2_WIDTH                                                      11
#define OW_Y_END2_SHIFT                                                       0
#define OW_Y_END2_MASK                                               0x000007ff
#define OW_Y_END2_RD(src)                               (((src) & 0x000007ffU))
#define OW_Y_END2_WR(src)                          (((u32)(src)) & 0x000007ffU)
#define OW_Y_END2_SET(dst,src) \
                        (((dst) & ~0x000007ffU) | (((u32)(src)) & 0x000007ffU))


/*	Register lcdc_owdbar12	*/

/*	 Fields OW_DBAR	 */
#define OW_DBAR2_WIDTH                                                       32
#define OW_DBAR2_SHIFT                                                        0
#define OW_DBAR2_MASK                                                0xffffffff
#define OW_DBAR2_RD(src)                                (((src) & 0xffffffffU))
#define OW_DBAR2_WR(src)                           (((u32)(src)) & 0xffffffffU)
#define OW_DBAR2_SET(dst,src) \
                        (((dst) & ~0xffffffffU) | (((u32)(src)) & 0xffffffffU))


/*	Register lcdc_owdcar12	*/

/*	 Fields OW_DCAR	 */
#define OW_DCAR2_WIDTH                                                       32
#define OW_DCAR2_SHIFT                                                        0
#define OW_DCAR2_MASK                                                0xffffffff
#define OW_DCAR2_RD(src)                                (((src) & 0xffffffffU))
#define OW_DCAR2_WR(src)                           (((u32)(src)) & 0xffffffffU)
#define OW_DCAR2_SET(dst,src) \
                        (((dst) & ~0xffffffffU) | (((u32)(src)) & 0xffffffffU))


/*	Register lcdc_oxser13	*/

/*	 Fields ow_x_start	 */
#define OW_X_START3_WIDTH                                                    11
#define OW_X_START3_SHIFT                                                    16
#define OW_X_START3_MASK                                             0x07ff0000
#define OW_X_START3_RD(src)                         (((src) & 0x07ff0000U)>>16)
#define OW_X_START3_WR(src)                    (((u32)(src)<<16) & 0x07ff0000U)
#define OW_X_START3_SET(dst,src) \
                    (((dst) & ~0x07ff0000U) | (((u32)(src)<<16) & 0x07ff0000U))


/*	 Fields ow_x_end	 */
#define OW_X_END3_WIDTH                                                      11
#define OW_X_END3_SHIFT                                                       0
#define OW_X_END3_MASK                                               0x000007ff
#define OW_X_END3_RD(src)                               (((src) & 0x000007ffU))
#define OW_X_END3_WR(src)                          (((u32)(src)) & 0x000007ffU)
#define OW_X_END3_SET(dst,src) \
                        (((dst) & ~0x000007ffU) | (((u32)(src)) & 0x000007ffU))


/*	Register lcdc_oyser13	*/

/*	 Fields ow_y_start	 */
#define OW_Y_START3_WIDTH                                                    11
#define OW_Y_START3_SHIFT                                                    16
#define OW_Y_START3_MASK                                             0x07ff0000
#define OW_Y_START3_RD(src)                         (((src) & 0x07ff0000U)>>16)
#define OW_Y_START3_WR(src)                    (((u32)(src)<<16) & 0x07ff0000U)
#define OW_Y_START3_SET(dst,src) \
                    (((dst) & ~0x07ff0000U) | (((u32)(src)<<16) & 0x07ff0000U))


/*	 Fields ow_y_end	 */
#define OW_Y_END3_WIDTH                                                      11
#define OW_Y_END3_SHIFT                                                       0
#define OW_Y_END3_MASK                                               0x000007ff
#define OW_Y_END3_RD(src)                               (((src) & 0x000007ffU))
#define OW_Y_END3_WR(src)                          (((u32)(src)) & 0x000007ffU)
#define OW_Y_END3_SET(dst,src) \
                        (((dst) & ~0x000007ffU) | (((u32)(src)) & 0x000007ffU))


/*	Register lcdc_owdbar13	*/

/*	 Fields OW_DBAR	 */
#define OW_DBAR3_WIDTH                                                       32
#define OW_DBAR3_SHIFT                                                        0
#define OW_DBAR3_MASK                                                0xffffffff
#define OW_DBAR3_RD(src)                                (((src) & 0xffffffffU))
#define OW_DBAR3_WR(src)                           (((u32)(src)) & 0xffffffffU)
#define OW_DBAR3_SET(dst,src) \
                        (((dst) & ~0xffffffffU) | (((u32)(src)) & 0xffffffffU))


/*	Register lcdc_owdcar13	*/

/*	 Fields OW_DCAR	 */
#define OW_DCAR3_WIDTH                                                       32
#define OW_DCAR3_SHIFT                                                        0
#define OW_DCAR3_MASK                                                0xffffffff
#define OW_DCAR3_RD(src)                                (((src) & 0xffffffffU))
#define OW_DCAR3_WR(src)                           (((u32)(src)) & 0xffffffffU)
#define OW_DCAR3_SET(dst,src) \
                        (((dst) & ~0xffffffffU) | (((u32)(src)) & 0xffffffffU))


/*	Register lcdc_oxser14	*/

/*	 Fields ow_x_start	 */
#define OW_X_START4_WIDTH                                                    11
#define OW_X_START4_SHIFT                                                    16
#define OW_X_START4_MASK                                             0x07ff0000
#define OW_X_START4_RD(src)                         (((src) & 0x07ff0000U)>>16)
#define OW_X_START4_WR(src)                    (((u32)(src)<<16) & 0x07ff0000U)
#define OW_X_START4_SET(dst,src) \
                    (((dst) & ~0x07ff0000U) | (((u32)(src)<<16) & 0x07ff0000U))


/*	 Fields ow_x_end	 */
#define OW_X_END4_WIDTH                                                      11
#define OW_X_END4_SHIFT                                                       0
#define OW_X_END4_MASK                                               0x000007ff
#define OW_X_END4_RD(src)                               (((src) & 0x000007ffU))
#define OW_X_END4_WR(src)                          (((u32)(src)) & 0x000007ffU)
#define OW_X_END4_SET(dst,src) \
                        (((dst) & ~0x000007ffU) | (((u32)(src)) & 0x000007ffU))


/*	Register lcdc_oyser14	*/

/*	 Fields ow_y_start	 */
#define OW_Y_START4_WIDTH                                                    11
#define OW_Y_START4_SHIFT                                                    16
#define OW_Y_START4_MASK                                             0x07ff0000
#define OW_Y_START4_RD(src)                         (((src) & 0x07ff0000U)>>16)
#define OW_Y_START4_WR(src)                    (((u32)(src)<<16) & 0x07ff0000U)
#define OW_Y_START4_SET(dst,src) \
                    (((dst) & ~0x07ff0000U) | (((u32)(src)<<16) & 0x07ff0000U))


/*	 Fields ow_y_end	 */
#define OW_Y_END4_WIDTH                                                      11
#define OW_Y_END4_SHIFT                                                       0
#define OW_Y_END4_MASK                                               0x000007ff
#define OW_Y_END4_RD(src)                               (((src) & 0x000007ffU))
#define OW_Y_END4_WR(src)                          (((u32)(src)) & 0x000007ffU)
#define OW_Y_END4_SET(dst,src) \
                        (((dst) & ~0x000007ffU) | (((u32)(src)) & 0x000007ffU))


/*	Register lcdc_owdbar14	*/

/*	 Fields OW_DBAR	 */
#define OW_DBAR4_WIDTH                                                       32
#define OW_DBAR4_SHIFT                                                        0
#define OW_DBAR4_MASK                                                0xffffffff
#define OW_DBAR4_RD(src)                                (((src) & 0xffffffffU))
#define OW_DBAR4_WR(src)                           (((u32)(src)) & 0xffffffffU)
#define OW_DBAR4_SET(dst,src) \
                        (((dst) & ~0xffffffffU) | (((u32)(src)) & 0xffffffffU))


/*	Register lcdc_owdcar14	*/

/*	 Fields OW_DCAR	 */
#define OW_DCAR4_WIDTH                                                       32
#define OW_DCAR4_SHIFT                                                        0
#define OW_DCAR4_MASK                                                0xffffffff
#define OW_DCAR4_RD(src)                                (((src) & 0xffffffffU))
#define OW_DCAR4_WR(src)                           (((u32)(src)) & 0xffffffffU)
#define OW_DCAR4_SET(dst,src) \
                        (((dst) & ~0xffffffffU) | (((u32)(src)) & 0xffffffffU))


/*	Register lcdc_oxser15	*/

/*	 Fields ow_x_start	 */
#define OW_X_START5_WIDTH                                                    11
#define OW_X_START5_SHIFT                                                    16
#define OW_X_START5_MASK                                             0x07ff0000
#define OW_X_START5_RD(src)                         (((src) & 0x07ff0000U)>>16)
#define OW_X_START5_WR(src)                    (((u32)(src)<<16) & 0x07ff0000U)
#define OW_X_START5_SET(dst,src) \
                    (((dst) & ~0x07ff0000U) | (((u32)(src)<<16) & 0x07ff0000U))


/*	 Fields ow_x_end	 */
#define OW_X_END5_WIDTH                                                      11
#define OW_X_END5_SHIFT                                                       0
#define OW_X_END5_MASK                                               0x000007ff
#define OW_X_END5_RD(src)                               (((src) & 0x000007ffU))
#define OW_X_END5_WR(src)                          (((u32)(src)) & 0x000007ffU)
#define OW_X_END5_SET(dst,src) \
                        (((dst) & ~0x000007ffU) | (((u32)(src)) & 0x000007ffU))


/*	Register lcdc_oyser15	*/

/*	 Fields ow_y_start	 */
#define OW_Y_START5_WIDTH                                                    11
#define OW_Y_START5_SHIFT                                                    16
#define OW_Y_START5_MASK                                             0x07ff0000
#define OW_Y_START5_RD(src)                         (((src) & 0x07ff0000U)>>16)
#define OW_Y_START5_WR(src)                    (((u32)(src)<<16) & 0x07ff0000U)
#define OW_Y_START5_SET(dst,src) \
                    (((dst) & ~0x07ff0000U) | (((u32)(src)<<16) & 0x07ff0000U))


/*	 Fields ow_y_end	 */
#define OW_Y_END5_WIDTH                                                      11
#define OW_Y_END5_SHIFT                                                       0
#define OW_Y_END5_MASK                                               0x000007ff
#define OW_Y_END5_RD(src)                               (((src) & 0x000007ffU))
#define OW_Y_END5_WR(src)                          (((u32)(src)) & 0x000007ffU)
#define OW_Y_END5_SET(dst,src) \
                        (((dst) & ~0x000007ffU) | (((u32)(src)) & 0x000007ffU))


/*	Register lcdc_owdbar15	*/

/*	 Fields ow_dbar	 */
#define OW_DBAR5_WIDTH                                                       32
#define OW_DBAR5_SHIFT                                                        0
#define OW_DBAR5_MASK                                                0xffffffff
#define OW_DBAR5_RD(src)                                (((src) & 0xffffffffU))
#define OW_DBAR5_WR(src)                           (((u32)(src)) & 0xffffffffU)
#define OW_DBAR5_SET(dst,src) \
                        (((dst) & ~0xffffffffU) | (((u32)(src)) & 0xffffffffU))


/*	Register lcdc_owdcar15	*/

/*	 Fields ow_dcar	 */
#define OW_DCAR5_WIDTH                                                       32
#define OW_DCAR5_SHIFT                                                        0
#define OW_DCAR5_MASK                                                0xffffffff
#define OW_DCAR5_RD(src)                                (((src) & 0xffffffffU))
#define OW_DCAR5_WR(src)                           (((u32)(src)) & 0xffffffffU)
#define OW_DCAR5_SET(dst,src) \
                        (((dst) & ~0xffffffffU) | (((u32)(src)) & 0xffffffffU))

/* Manual address as these are missing from the LCD header file */
#define FBE_MASK	(ARS_MASK << 1)
#define FNC_MASK	(ARS_MASK << 2)
#define FLC_MASK	(ARS_MASK << 3)

/* Added Manually : LCD RAM_SHUTDOWN_ADDR is defined in mpic_csr.h   */
/* This is because LCD, MPIC and SDU share the Global Diag registers */
/* The RAM is only used by LCD block, not by MPIC and SDU block      */
#define LCD_CFG_MEM_RAM_SHUTDOWN_ADDR   0x00000070

#endif
