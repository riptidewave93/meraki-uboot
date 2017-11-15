/*
 * SPD SDRAM detection code supports APM cpu's with DDR2/3 controller
 *
  * Copyright (c) 2010 Applied Micro Circuits Corporation.
 * All rights reserved. Feng Kan <fkan@apm.com>.
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
 *
 */
#ifndef __APM_SPD_DDR_H__
#define __APM_SPD_DDR_H__

#include <ddr_spd.h>

#define CONFIG_SYS_ENABLE_DDR3
#ifdef CONFIG_SYS_ENABLE_DDR3
typedef ddr3_spd_eeprom_t generic_spd_eeprom_t;
#else
#error "Only DDR3 is supported"
#endif

#define CONFIG_SYS_NUM_DDR_CTLRS 1
#define CONFIG_SYS_DIMM_SLOTS_PER_CTLR 1
#define CONFIG_SYS_NO_OF_DIMMS CONFIG_SYS_NUM_DDR_CTLRS*CONFIG_SYS_DIMM_SLOTS_PER_CTLR

#define APM_UMC_BLOCK_MQ	0 /* UMC MQ CSR block */
#define APM_UMC_BLOCK_NWL	1 /* UMC NWL CSR block */
#define APM_UMC_OFFSET_NWL	0x2000
#define APM_UMC_BASE		APM_CSR_UMC0_BASE_LO

#define MEM_DDR			1 /**< DDR1 memory */
#define MEM_DDR1		2 /**< DDR1 memory */
#define MEM_DDR2		4 /**< DDR2 memory */
#define MEM_DDR3		8 /**< DDR3 memory */

/*
 * SDRAM Controller
 */
#define MEMC_BID				0x00
#define MEMRANGE                                0x04
#define MEMQ_ARB				0x08
#define DDRC0_00                                0x10
#define DDRC0_01                                0x14
#define DDRC0_02                                0x18
#define DDRC0_03                                0x1C
#define DDRC0_04                                0x20
#define DDRC0_05                                0x24
#define DDRC0_06                                0x28
#define DDRC0_07                                0x2C
#define DDRC0_08                                0x30
#define DDRC0_09                                0x34
#define DDRC0_10                                0x38
#define DDRC0_11                                0x3C
#define DDRC0_12                                0x40
#define DDRC0_13                                0x44
#define DDRC0_15                                0x4C
#define DDRC0_16                                0x50
#define DDRC0_17                                0x54
#define DDRC0_18                                0x58
#define DDRC0_18_1                              0x5C
#define DDRC0_19                                0x60
#define DDRC0_20                                0x64
#define DDRC0_21                                0x68
#define DDRC0_22                                0x6C
#define DDRC0_23                                0x70
#define DDRC0_31                                0x90
#define DDRC0_31_1                              0x94
#define DDRC0_32                                0x98
#define DDRC0_33                                0x9C
#define DDRC0_35                                0xA4
#define PHY_1					0xB0
#define PHY_CTRL_SLAVE				0xB4
#define PHY_RD_DQS_SLAVE_R0_S0			0xB8
#define DDRC0_DBG                               0x3A8
#define DDRC0_INTERRUPT				0x3F4
#define MEM_ECC_BYPASS				0x668
#define DDRC0_37                                0x524
#define MEMQ_MISC				0x4b0
#define MEM_RAM_SHUTDOWN			0x670
#define DDR_IO_CONTROL				0x458
#define PHY_WR_DATA_SLAVE_R0_S0			0x300
#define PHY_WR_DQS_SLAVE_R0_S0			0x14c
#define PHY_2					0x1DC
#define PHY_DQ_OFFSET_0				0x390
#define PHY_DQ_OFFSET_1				0x394
#define PHY_DQ_OFFSET_2				0x398
#define PHY_3_R0_S0				0x1e0
#define PHY_FIFO_WE_SLAVE_R0_S0			0x270

#define DDRC0_ECC_1BEAT				0x04
#define DDRC0_ECC_OFF				0x00

/* bit range helper defines */
#define DDRC0_ECC_OFF_BITL			0
#define DDRC0_ECC_OFF_BITH			2
#define DDRC0_ECC_1BEAT_BITL			DDRC0_ECC_OFF_BITL
#define DDRC0_ECC_1BEAT_BITH			DDRC0_ECC_OFF_BITH
#define DDRC0_RMODE_BITL			6
#define DDRC0_RMODE_BITH			7
#define DDRC0_ODTBLK_BITL			10
#define DDRC0_ODTBLK_BITH			11
#define DDRC0_ENRANKS_BITL			12
#define DDRC0_ENRANKS_BITH			15
#define DDRC0_DQWDTH_BITL			28
#define DDRC0_DQWDTH_BITH			29
#define DDRC1_tREFI_BITL			20
#define DDRC1_tREFI_BITH			31
#define DDRC5_tRFCMIN_BITL			16
#define DDRC5_tRFCMIN_BITH			23
#define DDRC5_tRC_BITL				26
#define DDRC5_tRC_BITH				31
#define DDRC6_tRASMIN_BITL			3
#define DDRC6_tRASMIN_BITH			7
#define DDRC6_tRASMAX_BITL			10
#define DDRC6_tRASMAX_BITH			15
#define DDRC6_tFAWVAL_BITL			16
#define DDRC6_tFAWVAL_BITH			21
#define DDRC6_WR2PRE_BITL			27
#define DDRC6_WR2PRE_BITH			31
#define DDRC7_WRLAT_BITL			27
#define DDRC7_WRLAT_BITH			31
#define DDRC7_tRCD_BITL				0
#define DDRC7_tRCD_BITH				3
#define DDRC7_tRTP_BITL				4
#define DDRC7_tRTP_BITH				8
#define DDRC7_WR2RD_BITL			17
#define DDRC7_WR2RD_BITH			21
#define DDRC7_RD2WR_BITL			22
#define DDRC7_RD2WR_BITH			26
#define DDRC8_tRPMIN_BITL			16
#define DDRC8_tRPMIN_BITH			19
#define DDRC8_tRRDMIN_BITL			24
#define DDRC8_tRRDMIN_BITH			26
#define DDRC10_PRECKE_BITL			10
#define DDRC10_PRECKE_BITH			19
#define DDRC13_L2SIZZ_BITL			20
#define DDRC13_L2SIZZ_BITH			23
#define DDRC15_RADDR0_BITL			10
#define DDRC15_RADDR0_BITH			14
#define DDRC15_RADDR1_BITL			15
#define DDRC15_RADDR1_BITH			19
#define DDRC15_BADDR0_BITL			20
#define DDRC15_BADDR0_BITH			23
#define DDRC15_BADDR1_BITL			24
#define DDRC15_BADDR1_BITH			27
#define DDRC15_BADDR2_BITL			28
#define DDRC15_BADDR2_BITH			31
#define DDRC33_RSTN_BITL			4
#define DDRC33_RSTN_BITH			11

#define DDR_IOCTRL_ODTM_BITL			1
#define DDR_IOCTRL_ODTM_BITH			3
#define PHY1_WE2REDLY_BITL			4
#define PHY1_WE2REDLY_BITH			7
#define PHY1_WRLODT_BITL			15
#define PHY1_WRLODT_BITH			16
#define PHY1_RDLODT_BITL			17
#define PHY1_RDLODT_BITH			18
#define PHY1_DSLICE_BITL			23
#define PHY1_DSLICE_BITH			31
#define PHY2_WRRL_BITL				3
#define PHY2_WRRL_BITH				7
#define PHY2_DLYSEL_BITL			20
#define PHY2_DLYSEL_BITH			20

/** Types of memory sizes */
typedef enum {
	MB256,  /**< 256 MB */
	MB512,  /**< 512 MB */
	MB1024, /**< 1024 MB */
	MB2048, /**< 2048 MB */
	MB4096, /**< 4096 MB */
	MBSIZEMAX
} nwl_mem_size_e;

/** Types of memory speed grades */
typedef enum {
	MHZ133, /**< 133 MHz */
	MHZ167, /**< 167 MHz */
	MHZ200, /**< 200 MHz */
	MHZ267, /**< 267 MHz */
	MHZ333, /**< 333 MHz */
	MHZ400, /**< 400 MHz */
	MHZ533, /**< 533 MHz */
	MHZSPEEDMAX
} nwl_speed_grade_e;

/** Types of memory configurations */
typedef enum {
	CFG64Mx4,     /**< 64MB x 4 type config for 256MB */
	CFG32Mx8,     /**< 32MB x 8 type config for 256MB */
	CFG16Mx16,    /**< 16MB x 16 type config for 256MB */
	CFG128Mx4=0,  /**< 128MB x 4 type config for 512MB */
	CFG64Mx8,     /**< 64MB x 8 type config for 512MB */
	CFG32Mx16,    /**< 32MB x 16 type config for 512MB */
	CFG256Mx4=0,  /**< 256MB x 4 type config for 1024MB */
	CFG128Mx8,    /**< 128MB x 8 type config for 1024MB */
	CFG64Mx16,    /**< 64MB x 16 type config for 1024MB */
	CFG512Mx4=0,  /**< 512MB x 4 type config for 2048MB */
	CFG256Mx8,    /**< 256MB x 8 type config for 2048MB */
	CFG128Mx16,   /**< 128MB x 16 type config for 2048MB */
	CFG1024Mx4=0, /**< 1024MB x 4 type config for 4096MB */
	CFG512Mx8,    /**< 512MB x 8 type config for 4096MB */
	CFG256Mx16,   /**< 256MB x 16 type config for 4096MB */
	CFGMAX = 2
} nwl_mem_config_e;

#define MAX_SPEED_CFG		7
#define MAX_SIZE		5
#define MAX_CONFIG		3

/* Default memory config */
#define DEF_MEM_TYPE		MEM_DDR2
#ifndef CONFIG_ISS
#define DEF_SPEED_GRD		MHZ400
#define DEF_MEM_SIZE		MB4096
#define DEF_MEM_CONFIG		CFG512Mx8
#else
#define DEF_SPEED_GRD		MHZ133
#define DEF_MEM_SIZE		MB512
#define DEF_MEM_CONFIG		CFG64Mx4
#endif

#ifndef TRUE
#define TRUE            1
#endif
#ifndef FALSE
#define FALSE           0
#endif

#define ONE_BILLION     1000000000
#define SIZE256MB       0x10000000
#define MPIC_FRR        0x1000
#define MULDIV64(m1, m2, d)     (u32)(((u64)(m1) * (u64)(m2)) / (u64)(d))

/* Parameters for DDR controller registers */
typedef struct memc_regs_s {
	unsigned int bid;
	unsigned int mem_range;
	unsigned int memq_arb;
	unsigned int reserved0;
	unsigned int reg0;
	unsigned int reg1;
	unsigned int reg2;
	unsigned int reg3;
	unsigned int reg4;
	unsigned int reg5;
	unsigned int reg6;
	unsigned int reg7;
	unsigned int reg8;
	unsigned int reg9;
	unsigned int reg10;
	unsigned int reg11;
	unsigned int reg12;
	unsigned int reg13;
	unsigned int reserve_14;
	unsigned int reg15;
	unsigned int reg16;
	unsigned int reg17;
	unsigned int reg18;
	unsigned int reg18_1;
	unsigned int reg19;
	unsigned int reserve_20;
	unsigned int reg21;
	unsigned int reg22;
	unsigned int reg23;
	unsigned int reserve_24;
	unsigned int reserve_25;
	unsigned int reserve_26;
	unsigned int reserve_27;
	unsigned int reserve_28;
	unsigned int reserve_29;
	unsigned int reserve_30;
	unsigned int reg31;
	unsigned int reg31_1;
	unsigned int reg32;
	unsigned int reg33;
	unsigned int reserve_34;
	unsigned int reg35;
	
} memc_regs_t;

/* Parameters for a DDR2 dimm computed from the SPD */
typedef struct dimm_params_s {

        /* DIMM organization parameters */
        char mpart[19];         /* guaranteed null terminated */

        unsigned int n_ranks;
        unsigned long long rank_density;
        unsigned long long capacity;
        unsigned int data_width;
        unsigned int primary_sdram_width;
        unsigned int ec_sdram_width;
        unsigned int registered_dimm;

        /* SDRAM device parameters */
        unsigned int n_row_addr;
        unsigned int n_col_addr;
        unsigned int edc_config;        /* 0 = none, 1 = parity, 2 = ECC */
        unsigned int n_banks_per_sdram_device;
        unsigned int burst_lengths_bitmask;     /* BL=4 bit 2, BL=8 = bit 3 */
        unsigned int row_density;

        /* used in computing base address of DIMMs */
        unsigned long long base_address;
        /* mirrored DIMMs */
        unsigned int mirrored_dimm;     /* only for ddr3 */

        /* DIMM timing parameters */

        unsigned int mtb_ps;    /* medium timebase ps, only for ddr3 */
        unsigned int tAA_ps;    /* minimum CAS latency time, only for ddr3 */
        unsigned int tFAW_ps;   /* four active window delay, only for ddr3 */

        /*
         * SDRAM clock periods
         * The range for these are 1000-10000 so a short should be sufficient
         */
        unsigned int tCKmin_X_ps;
        unsigned int tCKmin_X_minus_1_ps;
        unsigned int tCKmin_X_minus_2_ps;
        unsigned int tCKmax_ps;
        
        /* SPD-defined CAS latencies */
        unsigned int caslat_X;
        unsigned int caslat_X_minus_1;
        unsigned int caslat_X_minus_2;
        
        unsigned int caslat_lowest_derated;     /* Derated CAS latency */
        
        /* basic timing parameters */
        unsigned int tRCD_ps;
        unsigned int tRP_ps;
        unsigned int tRAS_ps;

        unsigned int tWR_ps;    /* maximum = 63750 ps */
        unsigned int tWTR_ps;   /* maximum = 63750 ps */
        unsigned int tRFC_ps;   /* max = 255 ns + 256 ns + .75 ns
                                       = 511750 ps */

        unsigned int tRRD_ps;   /* maximum = 63750 ps */
        unsigned int tRC_ps;    /* maximum = 254 ns + .75 ns = 254750 ps */

        unsigned int refresh_rate_ps;

        /* DDR3 doesn't need these as below */
        unsigned int tIS_ps;    /* byte 32, spd->ca_setup */
        unsigned int tIH_ps;    /* byte 33, spd->ca_hold */
        unsigned int tDS_ps;    /* byte 34, spd->data_setup */
        unsigned int tDH_ps;    /* byte 35, spd->data_hold */
        unsigned int tRTP_ps;   /* byte 38, spd->trtp */
        unsigned int tDQSQ_max_ps;      /* byte 44, spd->tdqsq */
        unsigned int tQHS_ps;   /* byte 45, spd->tqhs */
} dimm_params_t;

typedef struct {
        /* parameters to constrict */

        unsigned int tCKmin_X_ps;
        unsigned int tCKmax_ps;
        unsigned int tCKmax_max_ps;
        unsigned int tRCD_ps;
        unsigned int tRP_ps;
        unsigned int tRAS_ps;

        unsigned int tWR_ps;    /* maximum = 63750 ps */
        unsigned int tWTR_ps;   /* maximum = 63750 ps */
        unsigned int tRFC_ps;   /* maximum = 255 ns + 256 ns + .75 ns
                                           = 511750 ps */

        unsigned int tRRD_ps;   /* maximum = 63750 ps */
        unsigned int tRC_ps;    /* maximum = 254 ns + .75 ns = 254750 ps */

        unsigned int refresh_rate_ps;

        unsigned int tIS_ps;    /* byte 32, spd->ca_setup */
        unsigned int tIH_ps;    /* byte 33, spd->ca_hold */
        unsigned int tDS_ps;    /* byte 34, spd->data_setup */
        unsigned int tDH_ps;    /* byte 35, spd->data_hold */
        unsigned int tRTP_ps;   /* byte 38, spd->trtp */
        unsigned int tDQSQ_max_ps;      /* byte 44, spd->tdqsq */
        unsigned int tQHS_ps;   /* byte 45, spd->tqhs */

        unsigned int ndimms_present;
        unsigned int lowest_common_SPD_caslat;
        unsigned int highest_common_derated_caslat;
        unsigned int additive_latency;
        unsigned int all_DIMMs_burst_lengths_bitmask;
        unsigned int all_DIMMs_registered;
        unsigned int all_DIMMs_unbuffered;
        unsigned int all_DIMMs_ECC_capable;

        unsigned long long total_mem;
        unsigned long long base_address;
} timing_params_t;

typedef struct apm_ddr_info_s {
        generic_spd_eeprom_t
          spd_info[CONFIG_SYS_NUM_DDR_CTLRS][CONFIG_SYS_DIMM_SLOTS_PER_CTLR];
        dimm_params_t
          dimm_params[CONFIG_SYS_NUM_DDR_CTLRS][CONFIG_SYS_DIMM_SLOTS_PER_CTLR];
        timing_params_t timing_params[CONFIG_SYS_NUM_DDR_CTLRS];
        memc_regs_t ddr_reg[CONFIG_SYS_NUM_DDR_CTLRS];
	unsigned char 
	  dimm_addr[CONFIG_SYS_NUM_DDR_CTLRS][CONFIG_SYS_DIMM_SLOTS_PER_CTLR];
} apm_ddr_info_t;



u32 memc_regmod(u32 addr, u32 data, u8 bl, u8 bh);
u32 memc_regread (u32 addr, int bl, int bh); 

#ifdef CONFIG_APM86XXX_DDR_AUTOCALIBRATION
int apm_sw_ddrcal(void);
#endif

#endif
