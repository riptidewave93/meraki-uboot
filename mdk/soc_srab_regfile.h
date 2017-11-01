#ifndef _SOC_SRAB_REG_FILE_H
#define _SOC_SRAB_REG_FILE_H

#include <asm/arch/iproc_regs.h>

/* ---------------------------------------------------------------------------------------- */
/*                                                                                          */
/* Register: CFG_R_SRAB_CMDSTAT                                                             */
/* Description: Switch Register Access Bridge command status                                */
/* Width: 32 bits                                                                           */
/* Type: RW                                                                                 */
/* Reg Index: 0xc802c                                                                       */
/*                                                                                          */
/* ---------------------------------------------------------------------------------------- */
#define CFG_R_SRAB_CMDSTAT_MEMADDR                          (IPROC_CCB_SRAU_REG_BASE | 0x2c)
#define CFG_R_SRAB_CMDSTAT_SEL                                                          0x2c
#define CFG_R_SRAB_CMDSTAT_SIZE                                                           32
#define CFG_R_SRAB_CMDSTAT_MASK                                                   0xffffffff
#define CFG_R_SRAB_CMDSTAT_INIT                                                          0x0
#define CFG_R_SRAB_CMDSTAT_RWMODE                                                         RW

/* ---------------------------------------------------------------------------------------- */
/*                                                                                          */
/* Register: CFG_R_SRAB_WDH                                                                 */
/* Description: "Switch register access ,write data, high order word"                       */
/* Width: 32 bits                                                                           */
/* Type: RW                                                                                 */
/* Reg Index: 0xc8030                                                                       */
/*                                                                                          */
/* ---------------------------------------------------------------------------------------- */
#define CFG_R_SRAB_WDH_MEMADDR                              (IPROC_CCB_SRAU_REG_BASE | 0x30)
#define CFG_R_SRAB_WDH_SEL                                                              0x30
#define CFG_R_SRAB_WDH_SIZE                                                               32
#define CFG_R_SRAB_WDH_MASK                                                       0xffffffff
#define CFG_R_SRAB_WDH_INIT                                                              0x0
#define CFG_R_SRAB_WDH_RWMODE                                                             RW

/* ---------------------------------------------------------------------------------------- */
/*                                                                                          */
/* Register: CFG_R_SRAB_WDL                                                                 */
/* Description: "Switch register access ,write data, low order word"                        */
/* Width: 32 bits                                                                           */
/* Type: RW                                                                                 */
/* Reg Index: 0xc8034                                                                       */
/*                                                                                          */
/* ---------------------------------------------------------------------------------------- */
#define CFG_R_SRAB_WDL_MEMADDR                              (IPROC_CCB_SRAU_REG_BASE | 0x34)
#define CFG_R_SRAB_WDL_SEL                                                              0x34
#define CFG_R_SRAB_WDL_SIZE                                                               32
#define CFG_R_SRAB_WDL_MASK                                                       0xffffffff
#define CFG_R_SRAB_WDL_INIT                                                              0x0
#define CFG_R_SRAB_WDL_RWMODE                                                             RW

/* ---------------------------------------------------------------------------------------- */
/*                                                                                          */
/* Register: CFG_R_SRAB_RDH                                                                 */
/* Description: "Switch register access read data, high order word"                         */
/* Width: 32 bits                                                                           */
/* Type: RW                                                                                 */
/* Reg Index: 0xc8038                                                                       */
/*                                                                                          */
/* ---------------------------------------------------------------------------------------- */
#define CFG_R_SRAB_RDH_MEMADDR                              (IPROC_CCB_SRAU_REG_BASE | 0x38)
#define CFG_R_SRAB_RDH_SEL                                                              0x38
#define CFG_R_SRAB_RDH_SIZE                                                               32
#define CFG_R_SRAB_RDH_MASK                                                       0xffffffff
#define CFG_R_SRAB_RDH_INIT                                                              0x0
#define CFG_R_SRAB_RDH_RWMODE                                                             RW

/* ---------------------------------------------------------------------------------------- */
/*                                                                                          */
/* Register: CFG_R_SRAB_RDL                                                                 */
/* Description: "Switch register access read data, low order word"                          */
/* Width: 32 bits                                                                           */
/* Type: RW                                                                                 */
/* Reg Index: 0xc803c                                                                       */
/*                                                                                          */
/* ---------------------------------------------------------------------------------------- */
#define CFG_R_SRAB_RDL_MEMADDR                              (IPROC_CCB_SRAU_REG_BASE | 0x3c)
#define CFG_R_SRAB_RDL_SEL                                                              0x3c
#define CFG_R_SRAB_RDL_SIZE                                                               32
#define CFG_R_SRAB_RDL_MASK                                                       0xffffffff
#define CFG_R_SRAB_RDL_INIT                                                              0x0
#define CFG_R_SRAB_RDL_RWMODE                                                             RW

/* ---------------------------------------------------------------------------------------- */
/*                                                                                          */
/* Register: CFG_R_SW_IF                                                                    */
/* Description: Switch interface and control                                                */
/* Width: 32 bits                                                                           */
/* Type: RW                                                                                 */
/* Reg Index: 0xc8040                                                                       */
/*                                                                                          */
/* ---------------------------------------------------------------------------------------- */
#define CFG_R_SW_IF_MEMADDR                                 (IPROC_CCB_SRAU_REG_BASE | 0x40)
#define CFG_R_SW_IF_SEL                                                                 0x40
#define CFG_R_SW_IF_SIZE                                                                  32
#define CFG_R_SW_IF_MASK                                                          0xffffffff
#define CFG_R_SW_IF_INIT                                                                 0x0
#define CFG_R_SW_IF_RWMODE                                                                RW


/* ---------------------------------------------------------------------------------------- */
/*                                                                                          */
/* Field: CFG_F_rcareq                                                                      */
/* Description: switch arbiter access request                                               */
/* Width: 1 bits                                                                            */
/* Type: RW                                                                                 */
/* Bit Mappings: [3:3]                                                                      */
/*                                                                                          */
/* ---------------------------------------------------------------------------------------- */
#define CFG_F_rcareq_R                                                                     3
#define CFG_F_rcareq_L                                                                     3
#define CFG_F_rcareq_SIZE                                                                  1
#define CFG_F_rcareq_MASK                                                         0x00000008
#define CFG_F_rcareq_RANGE                                                               3:3
#define CFG_F_rcareq_INIT                                                         0x00000000
#define CFG_F_rcareq_RWMODE                                                               RW

#define CFG_F_rcareq_GET(x) \
( \
    (((x) & CFG_F_rcareq_MASK) >> \
            CFG_F_rcareq_R) \
)

/* ---------------------------------------------------------------------------------------- */
/*                                                                                          */
/* Field: CFG_F_rcagnt                                                                      */
/* Description: switch arbiter grant                                                        */
/* Width: 1 bits                                                                            */
/* Type: RO                                                                                 */
/* Bit Mappings: [4:4]                                                                      */
/*                                                                                          */
/* ---------------------------------------------------------------------------------------- */
#define CFG_F_rcagnt_R                                                                     4
#define CFG_F_rcagnt_L                                                                     4
#define CFG_F_rcagnt_SIZE                                                                  1
#define CFG_F_rcagnt_MASK                                                         0x00000010
#define CFG_F_rcagnt_RANGE                                                               4:4
#define CFG_F_rcagnt_INIT                                                         0x00000000
#define CFG_F_rcagnt_RWMODE                                                               RO

#define CFG_F_rcagnt_GET(x) \
( \
    (((x) & CFG_F_rcagnt_MASK) >> \
            CFG_F_rcagnt_R) \
)

/* ---------------------------------------------------------------------------------------- */
/*                                                                                          */
/* Field: CFG_F_sra_page                                                                    */
/* Description: page to switch register                                                     */
/* Width: 8 bits                                                                            */
/* Type: RW                                                                                 */
/* Bit Mappings: [31:24]                                                                    */
/*                                                                                          */
/* ---------------------------------------------------------------------------------------- */
#define CFG_F_sra_page_R                                                                  24
#define CFG_F_sra_page_L                                                                  31
#define CFG_F_sra_page_SIZE                                                                8
#define CFG_F_sra_page_MASK                                                       0xff000000
#define CFG_F_sra_page_RANGE                                                           31:24
#define CFG_F_sra_page_INIT                                                       0x00000000
#define CFG_F_sra_page_RWMODE                                                             RW

#define CFG_F_sra_page_GET(x) \
( \
    (((x) & CFG_F_sra_page_MASK) >> \
            CFG_F_sra_page_R) \
)

/* ---------------------------------------------------------------------------------------- */
/*                                                                                          */
/* Field: CFG_F_sra_offset                                                                  */
/* Description: offset to switch register in page                                           */
/* Width: 8 bits                                                                            */
/* Type: RW                                                                                 */
/* Bit Mappings: [23:16]                                                                    */
/*                                                                                          */
/* ---------------------------------------------------------------------------------------- */
#define CFG_F_sra_offset_R                                                                16
#define CFG_F_sra_offset_L                                                                23
#define CFG_F_sra_offset_SIZE                                                              8
#define CFG_F_sra_offset_MASK                                                     0x00ff0000
#define CFG_F_sra_offset_RANGE                                                         23:16
#define CFG_F_sra_offset_INIT                                                     0x00000000
#define CFG_F_sra_offset_RWMODE                                                           RW

#define CFG_F_sra_offset_GET(x) \
( \
    (((x) & CFG_F_sra_offset_MASK) >> \
            CFG_F_sra_offset_R) \
)

/* ---------------------------------------------------------------------------------------- */
/*                                                                                          */
/* Field: CFG_F_sra_gordyn                                                                  */
/* Description: "write 1 to initiate command, read 0 to indicate switch register access interface is ready for the next command" */
/* Width: 1 bits                                                                            */
/* Type: RW                                                                                 */
/* Bit Mappings: [0:0]                                                                      */
/*                                                                                          */
/* ---------------------------------------------------------------------------------------- */
#define CFG_F_sra_gordyn_R                                                                 0
#define CFG_F_sra_gordyn_L                                                                 0
#define CFG_F_sra_gordyn_SIZE                                                              1
#define CFG_F_sra_gordyn_MASK                                                     0x00000001
#define CFG_F_sra_gordyn_RANGE                                                           0:0
#define CFG_F_sra_gordyn_INIT                                                            0x0
#define CFG_F_sra_gordyn_RWMODE                                                           RW

#define CFG_F_sra_gordyn_GET(x) \
( \
    (((x) & CFG_F_sra_gordyn_MASK) >> \
            CFG_F_sra_gordyn_R) \
)

/* ---------------------------------------------------------------------------------------- */
/*                                                                                          */
/* Field: CFG_F_sra_write                                                                   */
/* Description: write access control: 1=write; 0=read                                       */
/* Width: 1 bits                                                                            */
/* Type: RW                                                                                 */
/* Bit Mappings: [1:1]                                                                      */
/*                                                                                          */
/* ---------------------------------------------------------------------------------------- */
#define CFG_F_sra_write_R                                                                  1
#define CFG_F_sra_write_L                                                                  1
#define CFG_F_sra_write_SIZE                                                               1
#define CFG_F_sra_write_MASK                                                      0x00000002
#define CFG_F_sra_write_RANGE                                                            1:1
#define CFG_F_sra_write_INIT                                                      0x00000000
#define CFG_F_sra_write_RWMODE                                                            RW

#define CFG_F_sra_write_GET(x) \
( \
    (((x) & CFG_F_sra_write_MASK) >> \
            CFG_F_sra_write_R) \
)

/* ---------------------------------------------------------------------------------------- */
/*                                                                                          */
/* Field: CFG_F_sw_init_done                                                                */
/* Description: switch initialization status                                                */
/* Width: 1 bits                                                                            */
/* Type: RO                                                                                 */
/* Bit Mappings: [6:6]                                                                      */
/*                                                                                          */
/* ---------------------------------------------------------------------------------------- */
#define CFG_F_sw_init_done_R                                                               6
#define CFG_F_sw_init_done_L                                                               6
#define CFG_F_sw_init_done_SIZE                                                            1
#define CFG_F_sw_init_done_MASK                                                   0x00000040
#define CFG_F_sw_init_done_RANGE                                                         6:6
#define CFG_F_sw_init_done_INIT                                                   0x00000000
#define CFG_F_sw_init_done_RWMODE                                                         RO

#define CFG_F_sw_init_done_GET(x) \
( \
    (((x) & CFG_F_sw_init_done_MASK) >> \
            CFG_F_sw_init_done_R) \
)

/* ---------------------------------------------------------------------------------------- */
/*                                                                                          */
/* Field: CFG_F_sra_rst                                                                     */
/* Description: "switch access interface reset, self clearing"                              */
/* Width: 1 bits                                                                            */
/* Type: RW                                                                                 */
/* Bit Mappings: [2:2]                                                                      */
/*                                                                                          */
/* ---------------------------------------------------------------------------------------- */
#define CFG_F_sra_rst_R                                                                    2
#define CFG_F_sra_rst_L                                                                    2
#define CFG_F_sra_rst_SIZE                                                                 1
#define CFG_F_sra_rst_MASK                                                        0x00000004
#define CFG_F_sra_rst_RANGE                                                              2:2
#define CFG_F_sra_rst_INIT                                                        0x00000000
#define CFG_F_sra_rst_RWMODE                                                              RW

#define CFG_F_sra_rst_GET(x) \
( \
    (((x) & CFG_F_sra_rst_MASK) >> \
            CFG_F_sra_rst_R) \
)

#endif
