/*
 * APM APM86xxx DDR2/3 controller code
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
#undef DEBUG
#undef DEBUG_REGMOD
#undef FORCE_VALIDATION_SETTINGS


#include <common.h>
#include <command.h>
#include <ppc4xx.h>
#include <i2c.h>
#include <asm/io.h>
#include <asm/processor.h>
#include <asm/mmu.h>
#include <asm/cache.h>
#include <asm/mp.h>
#include <asm/apm_ipp_csr.h>
#include <asm/apm_scu.h>
#include "config.h"
#include "apm_spd_ddr.h"

#if defined(CONFIG_APM86XXX)

/*
 * This value tunes the timing parameter stability
 * If the increase is no substantial, simple round
 * up is used.
 * 0 - no change
 * 1 - 100% increase in value
 * 2 - 50% increase in value
 * 4 - 25% increase in value
 * 5 - 20% increase in value
 */
/* At 2 I could get 800, 1066 and 1333 to calibrate */
#if defined(CONFIG_APM86XXX_DDR_AUTOCALIBRATION)
#define MEMC_STABILITY_FACTOR		2
#else
#define MEMC_STABILITY_FACTOR		0
#endif

#if defined(FORCE_VALIDATION_SETTINGS)
#undef MEMC_STABILITY_FACTOR
#define MEMC_STABILITY_FACTOR		0

#if (CONFIG_SYS_DDR_FREQ == 1600000000)
#define DDR1600
#endif
#if (CONFIG_SYS_DDR_FREQ == 1333333333)
#define DDR1333
#endif
#if (CONFIG_SYS_DDR_FREQ == 1066666666)
#define DDR1066
#endif
#if (CONFIG_SYS_DDR_FREQ == 800000000)
#define DDR800
#endif

#endif /* FORCE_VALIDATION_SETTINGS */

#define MEMC_DIV		2


#define SPD_DELAY(x)		{ \
	volatile u32 delay; \
	for (delay = 0; delay < (x*1000); delay++); \
	}


#define CONFIG_SYS_ENABLE_DDR3

#undef SPD_DEBUG
#if !defined(CONFIG_MEDIA_SPL)
#if defined(SPD_DEBUG)
#define SPD_DBG_PRINT(x, ...)		printf((x), ##__VA_ARGS__)
#else
#define SPD_DBG_PRINT(x, ...)      
#endif
#else
#define SPD_DBG_PRINT(x, ...)      
#endif

#if defined(DEBUG)
#define DUMP_SPD_INFO		print_spd_info
#else
#define DUMP_SDRAM_REG(...)
#define DUMP_SPD_INFO(...)
#endif

#if defined(CONFIG_MEDIA_SPL) || defined(CONFIG_NAND_SPL)
#define udelay(x)               { \
        volatile u32 delay; \
        for (delay = 0; delay < 1000; delay++); \
        }
#endif
void apm86xxx_ddr_regdump(void);
unsigned int ddr_compute_timing_params(struct apm_ddr_info_s *);

#if defined(CONFIG_SYS_SIMULATE_SPD_EEPROM)
extern const unsigned char fake_spd[256];
#endif

#if defined(CONFIG_ISS)
/* This is the SPD for the MT9JSF12872AZ-1G4F1 */
unsigned char fake_spd[]={0x92,0x10,0x0B,0x02,0x02,0x11,0x00,0x01,0x0B,0x52, /* 0 */
                          0x01,0x08,0x0c,0x00,0x7e,0x00,0x69,0x78,0x69,0x30, /* 10 */
                          0x69,0x11,0x20,0x89,0x70,0x03,0x3c,0x3c,0x00,0xf0, /* 20 */
                          0x82,0x05,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 30 */
                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 40 */
                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 50 */
                          0x0f,0x01,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 60 */
                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 70 */
                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 80 */
                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 90 */
                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 100 */
                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x2c,0x00, /* 110 */
                          0x00,0x00,0x00,0x00,0x00,0x00,0x86,0x59,0x39,0x4a, /* 120 */
                          0x53,0x46,0x31,0x32,0x38,0x37,0x32,0x41,0x5a,0x2d, /* 130 */
                          0x31,0x47,0x34,0x46,0x31,0x66,0x46,0x31,0x80,0x2c, /* 140 */
                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 150 */
                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 160 */
                          0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff, /* 170 */
                          0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, /* 180 */
                          0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, /* 190 */
                          0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, /* 200 */
                          0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, /* 210 */
                          0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, /* 220 */
                          0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, /* 230 */
                          0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, /* 240 */
                          0xff,0xff,0xff,0xff,0xff,0xff};		     /* 250 */
#endif

#if !defined(CONFIG_FIXED_DDR_INIT) || defined(CONFIG_SYS_SIMULATE_SPD_EEPROM)
static int apm86xxx_is_32bit(void)
{
	int err;
	u32 data = 0;

	err = apm86xxx_read_scu_reg(SCU_SOC_EFUSE_ADDR, &data);

	/* Extract the SW key of the EFUSE */
	switch (EFUSE0_RD(data) & 0xF) {
	case 0x02:	
		return 1;	/* Diamondback is 32-bit DDR */
	}
	return 0;
}
#endif

u32 memc_regmod(u32 addr, u32 data, u8 bl, u8 bh)
{
	u32 reg, mask;

	mask = ~(0xffffffff << (32 - bl));
	data &= mask;
	data = data << (31 - bh);
	
	/*Preparing register mask*/
        mask = ~(0xffffffff << (32 - bl));
        mask &= (0xffffffff << (31 - bh));
        mask = ~(mask);

	mfsdram(addr, reg);
#if defined(DEBUG_REGMOD)
	debug("%s: reg 0x%x is 0x%x set 0x%x from %d to %d\n", __func__, addr, reg, data, bl, bh);
#endif
	reg &= mask;
	reg |= data;
	mtsdram(addr, reg);

#if defined(DEBUG_REGMOD)
	debug("%s: setting 0x%x to 0x%x\n", __func__, addr, reg);
#endif
	return 0;
}

u32 memc_regread (u32 addr, int bl, int bh) 
{
        u32 mask = 0;
        int i;
        u32 regval;
#if defined(DEBUG_REGMOD)
	debug("%s: @ 0x%08x,%d,%d(", __func__, addr,bl,bh);
#endif

        mfsdram(addr, regval);
#if defined(DEBUG_REGMOD)
        debug("@ 0x%08x: 0x%08x)",addr,regval);
#endif
        for(i=0; i<(bh - bl + 1); i++) {
                mask |= (1 << i) ;
        }
        regval = regval >> (31 - bh);
        regval &= mask;
#if defined(DEBUG_REGMOD)
        debug(" = %x (mask = %08x)",regval,mask);
#endif
        return regval;
}

/* simplified accessors */
#define memc_get(addr, data)       memc_regread(addr, data ## _BITL, \
                                         data ## _BITH)

#define memc_mod(addr, data) 	   memc_regmod(addr, data, data ## _BITL, \
                                         data ## _BITH)


#if defined(CONFIG_DDR_SPD)
/*
 * Board-specific Platform code can reimplement spd_ddr_init_hang () if needed
 */
void __spd_ddr_init_hang(void)
{
	while(1);
}

void spd_ddr_init_hang(void)
	__attribute__ ((weak, alias("__spd_ddr_init_hang")));

#if !defined(CONFIG_SYS_SIMULATE_SPD_EEPROM)
unsigned char spd_read(unsigned char chip, unsigned int addr)
{
	unsigned char data[2] = { 0, 0 };

	if (0 != i2c_read(chip, addr, 1, data, 1)) {
		debug("spd_read(0x%02X, 0x%02X) failed\n", chip, addr);
		return -1;
	}
	debug("spd_read(0x%02X, 0x%02X) data 0x%02X\n", chip, addr, data[0]);
	return data[0];
}
#endif

#if defined(DEBUG)
void print_spd_info(unsigned char *spd)
{
	unsigned char *ptr;
	unsigned char i;
	ptr = ((ddr3_spd_eeprom_t *)spd)->mpart;
	debug("DIMM Vendor : 0x%X%X\n", ((ddr3_spd_eeprom_t *)spd)->dmid_lsb,
					((ddr3_spd_eeprom_t *)spd)->dmid_msb);
	debug("DIMM Part : ");
	debug("%c", *ptr++);
	for (i = 0; i<18; i++)
		debug("%c", *ptr++);
	debug("\n");
}
#endif

static unsigned int get_spd(generic_spd_eeprom_t *spd, unsigned char i2c_address)
{
	unsigned char dimm_type = SPD_MEMTYPE_DDR3;

#if defined(CONFIG_ISS) || defined(CONFIG_SYS_SIMULATE_SPD_EEPROM)
	debug("* DIMM SPD simulated, copying from fake_spd\n");
        memcpy(spd, fake_spd, sizeof(ddr3_spd_eeprom_t));
#else
	debug("* Using real DIMM SPD.\n");
	I2C_SET_BUS(CONFIG_SYS_SPD_BUS_NUM);
	i2c_init(CONFIG_SYS_I2C_SPEED, i2c_address);

	/* Check for DIMM type before reading*/
	dimm_type = spd_read(i2c_address, 2); 
	if (dimm_type == SPD_MEMTYPE_DDR3) {
		i2c_read(i2c_address, 0, 1, (uchar *) spd, 
			sizeof(ddr3_spd_eeprom_t));
	} else if (dimm_type == SPD_MEMTYPE_DDR2) {
		i2c_read(i2c_address, 0, 1, (uchar *) spd, 
			sizeof(ddr2_spd_eeprom_t));
		debug("DDR 2 unsupported, hanging....\n");
		spd_ddr_init_hang();
	} else {
		debug("Unsupported DRAM, hanging....\n");
		spd_ddr_init_hang();
	}
#endif
	return dimm_type;
}

/*
 * DDR3 releated compute dimm param functions
 */
static unsigned long long ddr3_compute_ranksize(const ddr3_spd_eeprom_t *spd)
{
        unsigned long long bsize;

        int nbit_sdram_cap_bsize = 0;
        int nbit_primary_bus_width = 0;
        int nbit_sdram_width = 0;

        if ((spd->density_banks & 0xf) < 7)
                nbit_sdram_cap_bsize = (spd->density_banks & 0xf) + 28;
        if ((spd->bus_width & 0x7) < 4)
                nbit_primary_bus_width = (spd->bus_width & 0x7) + 3;
        if ((spd->organization & 0x7) < 4)
                nbit_sdram_width = (spd->organization & 0x7) + 2;

        bsize = 1ULL << (nbit_sdram_cap_bsize - 3
                    + nbit_primary_bus_width - nbit_sdram_width);

        debug("DDR: DDR III rank density = 0x%16llx\n", bsize);

        return bsize;
}

/*
 * ddr3_compute_dimm_parameters for DDR3 SPD
 *
 * Compute DIMM parameters based upon the SPD information in spd.
 * Writes the results to the dimm_params_t structure pointed by pdimm.
 *
 */
unsigned int ddr3_compute_dimm_parameters(const ddr3_spd_eeprom_t *spd,
                             dimm_params_t *pdimm,
                             unsigned int dimm_number)
{
        unsigned int retval;
        unsigned int mtb_ps;
        unsigned int i;

        if (spd->mem_type) {
                if (spd->mem_type != SPD_MEMTYPE_DDR3) {
                        debug("DIMM %u: is not a DDR3 SPD.\n", 
                        	dimm_number);
                        return 1;
                }
        } else {
                memset(pdimm, 0, sizeof(dimm_params_t));
                return 1;
        }

        retval = ddr3_spd_check(spd);
        if (retval) {
                debug("DIMM %u: failed checksum\n", dimm_number);
                return 2;
        }

       /*
         * The part name in ASCII in the SPD EEPROM is not null terminated.
         * Guarantee null termination here by presetting all bytes to 0
         * and copying the part name in ASCII from the SPD onto it
         */
        for (i = 0; i < sizeof(pdimm->mpart) - 1; i++) 
        	pdimm->mpart[i] = spd->mpart[i];
        pdimm->mpart[i-1] = 0;

        /* DIMM organization parameters */
        pdimm->n_ranks = ((spd->organization >> 3) & 0x7) + 1;
        pdimm->rank_density = ddr3_compute_ranksize(spd);
        pdimm->capacity = pdimm->n_ranks * pdimm->rank_density;
	debug("DIMM Capacity (%d ranks): total 0x%llx\n", 
		pdimm->n_ranks, pdimm->capacity);
        pdimm->primary_sdram_width = 1 << (3 + (spd->bus_width & 0x7));
        if ((spd->bus_width >> 3) & 0x3)
                pdimm->ec_sdram_width = 8;
        else
                pdimm->ec_sdram_width = 0;
        pdimm->data_width = pdimm->primary_sdram_width
                          + pdimm->ec_sdram_width;

        switch (spd->module_type & 0xf) {
        case 0x01:      /* RDIMM */
        case 0x05:      /* Mini-RDIMM */
		debug("Register DIMM detected: Type 0x%X\n", spd->module_type & 0xf);
                pdimm->registered_dimm = 1; /* register buffered */
                break;

        case 0x02:      /* UDIMM */
        case 0x03:      /* SO-DIMM */
        case 0x04:      /* Micro-DIMM */
        case 0x06:      /* Mini-UDIMM */
		debug("Unbuffered DIMM detected: Type 0x%X\n", spd->module_type & 0xf);
                pdimm->registered_dimm = 0;     /* unbuffered */
                break;

        default:
                debug("Unknown dimm_type 0x%02X\n", spd->module_type);
                return 1;
        }

        /* SDRAM device parameters */
        pdimm->n_row_addr = ((spd->addressing >> 3) & 0x7) + 12;
        pdimm->n_col_addr = (spd->addressing & 0x7) + 9;
        pdimm->n_banks_per_sdram_device = 8 << ((spd->density_banks >> 4) & 0x7);
	debug("Addressing: Row: %d Col: %d Bank: %d\n", 
		pdimm->n_row_addr, pdimm->n_col_addr, 
		pdimm->n_banks_per_sdram_device);

        /*
         * The SPD spec has not the ECC bit,
         * We consider the DIMM as ECC capability
         * when the extension bus exist
         */
        if (pdimm->ec_sdram_width)
                pdimm->edc_config = 0x02;
        else
                pdimm->edc_config = 0x00;

        /*
         * The SPD spec has not the burst length byte
         * but DDR3 spec has nature BL8 and BC4,
         * BL8 -bit3, BC4 -bit2
         */
        pdimm->burst_lengths_bitmask = 0x0c;
        pdimm->row_density = __ilog2(pdimm->rank_density);

        /* MTB - medium timebase
         * The unit in the SPD spec is ns,
         * We convert it to ps.
         * eg: MTB = 0.125ns (125ps)
         */
        mtb_ps = (spd->mtb_dividend * 1000) /spd->mtb_divisor;
        pdimm->mtb_ps = mtb_ps;
	debug("MTB(ps): %d\n", mtb_ps);

        /*
         * sdram minimum cycle time
         * we assume the MTB is 0.125ns
         * eg:
         * tCK_min=15 MTB (1.875ns) ->DDR3-1066
         *        =12 MTB (1.5ns) ->DDR3-1333
         *        =10 MTB (1.25ns) ->DDR3-1600
         */
        pdimm->tCKmin_X_ps = spd->tCK_min * mtb_ps;
	debug("CK min(ps): %d\n", pdimm->tCKmin_X_ps);

        /*
         * CAS latency supported
         * bit4 - CL4
         * bit5 - CL5
         * bit18 - CL18
         */
        pdimm->caslat_X  = ((spd->caslat_msb << 8) | spd->caslat_lsb) << 4;
	debug("SPD CAS Latency: 0x%4x\n", pdimm->caslat_X);
	debug("CAS: 0x%x\n", pdimm->caslat_X);

        /*
         * min CAS latency time
         * eg: tAA_min =
         * DDR3-800D    100 MTB (12.5ns)
         * DDR3-1066F   105 MTB (13.125ns)
         * DDR3-1333H   108 MTB (13.5ns)
         * DDR3-1600H   90 MTB (11.25ns)
         */
        pdimm->tAA_ps = spd->tAA_min * mtb_ps;
	debug("AA min(ps): %d\n", pdimm->tAA_ps);

        /*
         * min write recovery time
         * eg:
         * tWR_min = 120 MTB (15ns) -> all speed grades.
         */
        pdimm->tWR_ps = spd->tWR_min * mtb_ps;
	debug("WR min(ps): %d\n", pdimm->tWR_ps);

        /*
         * min RAS to CAS delay time
         * eg: tRCD_min =
         * DDR3-800     100 MTB (12.5ns)
         * DDR3-1066F   105 MTB (13.125ns)
         * DDR3-1333H   108 MTB (13.5ns)
         * DDR3-1600H   90 MTB (11.25)
         */
        pdimm->tRCD_ps = spd->tRCD_min * mtb_ps;
	debug("RCD min(ps): %d\n", pdimm->tRCD_ps);

        /*
         * min row active to row active delay time
         * eg: tRRD_min =
         * DDR3-800(1KB page)   80 MTB (10ns)
         * DDR3-1333(1KB page)  48 MTB (6ns)
         */
        pdimm->tRRD_ps = spd->tRRD_min * mtb_ps;
	debug("RRD min(ps): %d\n", pdimm->tRRD_ps);

        /*
         * min row precharge delay time
         * eg: tRP_min =
         * DDR3-800D    100 MTB (12.5ns)
         * DDR3-1066F   105 MTB (13.125ns)
         * DDR3-1333H   108 MTB (13.5ns)
         * DDR3-1600H   90 MTB (11.25ns)
         */
        pdimm->tRP_ps = spd->tRP_min * mtb_ps;
	debug("RP min(ps): %d\n", pdimm->tRP_ps);

        /* min active to precharge delay time
         * eg: tRAS_min =
         * DDR3-800D    300 MTB (37.5ns)
         * DDR3-1066F   300 MTB (37.5ns)
         * DDR3-1333H   288 MTB (36ns)
         * DDR3-1600H   280 MTB (35ns)
         */
        pdimm->tRAS_ps = (((spd->tRAS_tRC_ext & 0xf) << 8) | spd->tRAS_min_lsb)
                        * mtb_ps;
	debug("RAS min(ps): %d\n", pdimm->tRAS_ps);
        /*
         * min active to actice/refresh delay time
         * eg: tRC_min =
         * DDR3-800D    400 MTB (50ns)
         * DDR3-1066F   405 MTB (50.625ns)
         * DDR3-1333H   396 MTB (49.5ns)
         * DDR3-1600H   370 MTB (46.25ns)
         */
        pdimm->tRC_ps = (((spd->tRAS_tRC_ext & 0xf0) << 4) | spd->tRC_min_lsb)
                        * mtb_ps;
	debug("RC min(ps): %d\n", pdimm->tRC_ps);
        /*
         * min refresh recovery delay time
         * eg: tRFC_min =
         * 512Mb        720 MTB (90ns)
         * 1Gb          880 MTB (110ns)
         * 2Gb          1280 MTB (160ns)
         */
        pdimm->tRFC_ps = ((spd->tRFC_min_msb << 8) | spd->tRFC_min_lsb)
                        * mtb_ps;
	debug("RFC min(ps): %d\n", pdimm->tRFC_ps);

        /*
         * min internal write to read command delay time
         * eg: tWTR_min = 40 MTB (7.5ns) - all speed bins.
         * tWRT is at least 4 mclk independent of operating freq.
         */
        pdimm->tWTR_ps = spd->tWTR_min * mtb_ps;
	debug("WTR (ps): %d\n", pdimm->tWTR_ps);

        /*
         * min internal read to precharge command delay time
         * eg: tRTP_min = 40 MTB (7.5ns) - all speed bins.
         * tRTP is at least 4 mclk independent of operating freq.
         */
        pdimm->tRTP_ps = spd->tRTP_min * mtb_ps;
	debug("RTP (ps): %d\n", pdimm->tRTP_ps);

        /*
         * Average periodic refresh interval
         * tREFI = 7.8 us at normal temperature range
         *       = 3.9 us at ext temperature range
         */
        pdimm->refresh_rate_ps = 7800000;

        /*
         * min four active window delay time
         * eg: tFAW_min =
         * DDR3-800(1KB page)   320 MTB (40ns)
         * DDR3-1066(1KB page)  300 MTB (37.5ns)
         * DDR3-1333(1KB page)  240 MTB (30ns)
         * DDR3-1600(1KB page)  240 MTB (30ns)
         */
        pdimm->tFAW_ps = (((spd->tFAW_msb & 0xf) << 8) | spd->tFAW_min)
                        * mtb_ps;
	debug("FAW (ps): %d\n", pdimm->tFAW_ps);

        /*
         * We need check the address mirror for unbuffered DIMM
         * If SPD indicate the address map mirror, The DDR controller
         * need care it.
         */
        if ((spd->module_type == SPD_MODULETYPE_UDIMM) ||
            (spd->module_type == SPD_MODULETYPE_SODIMM) ||
            (spd->module_type == SPD_MODULETYPE_MICRODIMM) ||
            (spd->module_type == SPD_MODULETYPE_MINIUDIMM))
                pdimm->mirrored_dimm = spd->mod_section.unbuffered.addr_mapping & 0x1;

        return 0;
}

void compute_dimm_info(struct apm_ddr_info_s *ddr_info)
{
	unsigned char iic0_dimm_addr[] = SPD_EEPROM_ADDRESS;
	/*
	 * Get spd information from the dimm.
	 */
	ddr_info->dimm_addr[0][0] = iic0_dimm_addr[0];
	get_spd(ddr_info->spd_info[0], ddr_info->dimm_addr[0][0]);
#if defined(DEBUG)
	DUMP_SPD_INFO((unsigned char *)ddr_info->spd_info[0]);
#endif

	if (ddr3_compute_dimm_parameters(ddr_info->spd_info[0], 
		ddr_info->dimm_params[0], 0)) {
		SPD_DBG_PRINT("Incorrect DIMM parameters\n");
		hang();
	}

}
#endif

void reset_ddr_controller(void)
{
	int poll = 1000000;
	unsigned int done = 0;

	/* Release soft reset, start initialization */
	memc_regmod(DDRC0_00, 1, 31, 31);

	/* Wait for done bit */	
	while (poll) {
		/* 
		* 0x1 is normal operation
		* 0x0 is init phase
		* 0x2 power down mode
		* 0x3 self refresh
		*/
		mfsdram(DDRC0_DBG, done);
		if ((done == 0x2) || (done == 0x3)) {
			SPD_DBG_PRINT("DRAM: incorrect mode for DDR calibration %d\n", done);
			while(1);
		}
	
		if (done == 0x1)
			break;

		poll--;

		/* Wait sometime */
		SPD_DELAY(1);
	}
	
	if (poll == 0) {
		debug("Unable to initialize DDR controller\n");
		while(1);
	}
}

#if defined(CONFIG_DDR_SPD)
#if 0
static unsigned long get_tcyc(unsigned char reg)
{
	/*
	 * Byte 9, et al: Cycle time for CAS Latency=X, is split into two
	 * nibbles: the higher order nibble (bits 4-7) designates the cycle time
	 * to a granularity of 1ns; the value presented by the lower order
	 * nibble (bits 0-3) has a granularity of .1ns and is added to the value
	 * designated by the higher nibble. In addition, four lines of the lower
	 * order nibble are assigned to support +.25, +.33, +.66, and +.75.
	 */

	unsigned char subfield_b = reg & 0x0F;

	switch (subfield_b & 0x0F) {
	case 0x0:
	case 0x1:
	case 0x2:
	case 0x3:
	case 0x4:
	case 0x5:
	case 0x6:
	case 0x7:
	case 0x8:
	case 0x9:
		return 1000 * (reg >> 4) + 100 * subfield_b;
	case 0xA:
		return 1000 * (reg >> 4) + 250;
	case 0xB:
		return 1000 * (reg >> 4) + 333;
	case 0xC:
		return 1000 * (reg >> 4) + 667;
	case 0xD:
		return 1000 * (reg >> 4) + 750;
	}
	return 0;
}

/**
 * For the memory DIMMs installed, this routine verifies that
 * frequency previously calculated is supported.
 */
static int check_frequency(unsigned long *dimm_ranks,
			    unsigned char const iic0_dimm_addr[],
			    unsigned long num_dimm_banks,
			    unsigned long sdram_freq)
{
	unsigned long dimm_num;
	unsigned long cycle_time = 0;
	unsigned long calc_cycle_time;

	/*
	 * calc_cycle_time is calculated from DDR frequency set by board/chip
	 * and is expressed in picoseconds to match the way DIMM cycle time is
	 * calculated below.
	 */
	calc_cycle_time = MULDIV64(ONE_BILLION, 1000, sdram_freq);

	for (dimm_num = 0; dimm_num < num_dimm_banks; dimm_num++) {
		if (dimm_ranks[dimm_num]) {
			cycle_time =
			    get_tcyc(spd_read(iic0_dimm_addr[dimm_num], 9));
			debug("cycle_time: %lu ps\n", cycle_time);

			if (cycle_time > (calc_cycle_time + 10)) {
				/*
				 * the provided sdram cycle_time is too small
				 * for the available DIMM cycle_time. The
				 * additional 10ps is here to accept a small
				 * uncertainty.
				 */
				debug("ERROR: DRAM DIMM detected with "
				       "cycle_time %d ps in slot %d \n while "
				       "calculated cycle time is %d ps.\n",
				       (unsigned int)cycle_time,
				       (unsigned int)dimm_num,
				       (unsigned int)calc_cycle_time);
				debug("Replace the DIMM, or change DDR "
				       "frequency via strapping bits.\n\n");
				spd_ddr_init_hang();
			}
		}
	}
	return (1000000/cycle_time);
}

/**
 * This routine gets size information for the installed memory
 * DIMMs.
 */
static void get_dimm_size(unsigned long dimm_ranks[],
			  unsigned char const iic0_dimm_addr[],
			  unsigned long num_dimm_banks,
			  unsigned long *const rows,
			  unsigned long *const banks,
			  unsigned long *const cols,
			  unsigned long *const width)
{
	unsigned long dimm_num;

	*rows = 0;
	*banks = 0;
	*cols = 0;
	*width = 0;
	for (dimm_num = 0; dimm_num < num_dimm_banks; dimm_num++) {
		unsigned long t;
		if (!dimm_ranks[dimm_num])
			continue;
		
		/* Rows */
		t = spd_read(iic0_dimm_addr[dimm_num], 3);
		if (0 == *rows) {
			*rows = t;
		} else if (t != *rows) {
			debug("ERROR: DRAM DIMM modules do not all "
				"have the same number of rows.\n\n");
			spd_ddr_init_hang();
		}
		/* Banks */
		t = spd_read(iic0_dimm_addr[dimm_num], 17);
		if (0 == *banks) {
			*banks = t;
		} else if (t != *banks) {
			debug("ERROR: DRAM DIMM modules do not all "
				"have the same number of banks.\n\n");
			spd_ddr_init_hang();
		}
		/* Columns */
		t = spd_read(iic0_dimm_addr[dimm_num], 4);
		if (0 == *cols) {
			*cols = t;
		} else if (t != *cols) {
			debug("ERROR: DRAM DIMM modules do not all "
				"have the same number of columns.\n\n");
			spd_ddr_init_hang();
		}
		/* Data width */
		t = spd_read(iic0_dimm_addr[dimm_num], 6);
		if (0 == *width) {
			*width = t;
		} else if (t != *width) {
			debug("ERROR: DRAM DIMM modules do not all "
				"have the same data width.\n\n");
			spd_ddr_init_hang();
		}
	}
	debug("Number of rows: %lu\n", *rows);
	debug("Number of columns: %lu\n", *cols);
	debug("Number of banks: %lu\n", *banks);
	debug("Data width: %lu\n", *width);
	if (*rows > 14) {
		debug("ERROR: DRAM DIMM modules have %lu address rows.\n",
			*rows);
		debug("Only modules with 14 or fewer rows are supported.\n\n");
		spd_ddr_init_hang();
	}
	if (4 != *banks && 8 != *banks) {
		debug("ERROR: DRAM DIMM modules have %lu banks.\n", *banks);
		debug("Only modules with 4 or 8 banks are supported.\n\n");
		spd_ddr_init_hang();
	}
	if (*cols > 12) {
		debug("ERROR: DRAM DIMM modules have %lu address columns.\n",
			*cols);
		debug("Only modules with 12 or fewer columns are "
			"supported.\n\n");
		spd_ddr_init_hang();
	}
	if (32 != *width && 40 != *width && 64 != *width && 72 != *width) {
		debug("ERROR: DRAM DIMM modules have a width of %lu bit.\n",
			*width);
		debug("Only modules with widths of 32, 40, 64, and 72 bits "
			"are supported.\n\n");
		spd_ddr_init_hang();
	}
}

/**
 * Only 1.8V modules are supported.  This routine verifies this.
 */
static void check_voltage_type(unsigned long dimm_ranks[],
			       unsigned char const iic0_dimm_addr[],
			       unsigned long num_dimm_banks)
{
	unsigned long dimm_num;
	unsigned long voltage_type;

	for (dimm_num = 0; dimm_num < num_dimm_banks; dimm_num++) {
		if (!dimm_ranks[dimm_num])
			continue;
			
		voltage_type = spd_read(iic0_dimm_addr[dimm_num], 8);
		if (0x05 != voltage_type) {	/* 1.8V for DDR2 */
			debug("ERROR: Slot %lu provides 1.8V for DDR2 "
				"DIMMs.\n", dimm_num);
			switch (voltage_type) {
			case 0x00:
				debug("This DIMM is 5.0 Volt/TTL.\n");
				break;
			case 0x01:
				debug("This DIMM is LVTTL.\n");
				break;
			case 0x02:
				debug("This DIMM is 1.5 Volt.\n");
				break;
			case 0x03:
				debug("This DIMM is 3.3 Volt/TTL.\n");
				break;
			case 0x04:
				debug("This DIMM is 2.5 Volt.\n");
				break;
			default:
				debug("This DIMM is an unknown "
					"voltage.\n");
				break;
			}
			debug("Replace it with a 1.8V DDR2 DIMM.\n\n");
			spd_ddr_init_hang();
		}
	}
}
#endif /* not define CONFIG_ISS */

/*
 * SPD releated functions
 */

#if !defined(CONFIG_FIXED_DDR_INIT) || defined(CONFIG_SYS_SIMULATE_SPD_EEPROM)
/*
 * Round mclk_ps to nearest 10 ps in memory controller code.
 *
 * If an imprecise data rate is too high due to rounding error
 * propagation, compute a suitably rounded mclk_ps to compute
 * a working memory controller configuration.
 */
unsigned int get_memory_clk_period_ps(void)
{
        unsigned int mclk_ps,freqDDR;
#if !defined(CONFIG_MEDIA_SPL)
#if defined(CONFIG_ISS)
	freqDDR = 1066666666;
#else
	PPC4xx_SYS_INFO board_cfg;

	get_sys_info(&board_cfg);
	freqDDR = board_cfg.freqDDR;
#endif
#else
	if (apm86xxx_get_freq(APM86xxx_DDR, &freqDDR)) {
		return 0;
	}
#endif

	/* mclk_ps x 10 = 10,000,000,000,000 / freqDDR. We accomplish this with 32-bit operations */
        mclk_ps = (10000000000000ULL / freqDDR);

	debug("DDR Frequency (ps): %d rounded %d freq is %d\n", (mclk_ps + 5) / 10, mclk_ps, freqDDR);
        /* Round to nearest 10 ps */
        return (mclk_ps + 5) / 10;
}

static inline unsigned int rdiv(unsigned int a, unsigned int b)
{
	unsigned int temp;

	temp = (a * 10) / b;

	if ((temp % 10) != 0)
		temp = temp + 10;

	return temp / 10;
}

unsigned int time_ps_to_clk(unsigned int time_ps)
{
        unsigned int mclk_ps,freqDDR;
#if !defined(CONFIG_MEDIA_SPL)
#if defined(CONFIG_ISS)
	freqDDR = 1066666666;
#else
	PPC4xx_SYS_INFO board_cfg;

	get_sys_info(&board_cfg);
	freqDDR = board_cfg.freqDDR;
#endif
#else
	if (apm86xxx_get_freq(APM86xxx_DDR, &freqDDR)) {
		return 0;
	}
#endif
	/* mclk_ps = 2,000,000,000,000 / freqDDR. We accomplish this with 32-bit operations */
        mclk_ps = (1000000000000ULL / freqDDR);
 	time_ps = time_ps * 10 / mclk_ps;

	if ((time_ps % 10) == 0)
		return time_ps / 10;
	else
		return (time_ps + 10) / 10;
}

unsigned int mclk_to_picos(unsigned int mclk)
{
        return get_memory_clk_period_ps() * mclk;
}

/* 
 * compute the CAS write latency according to DDR3 spec
 * CWL = 5 if tCK >= 2.5ns
 *       6 if 2.5ns > tCK >= 1.875ns
 *       7 if 1.875ns > tCK >= 1.5ns
 *       8 if 1.5ns > tCK >= 1.25ns
 */
static inline unsigned int compute_cas_write_latency(void)
{
        unsigned int cwl;
        const unsigned int mclk_ps = get_memory_clk_period_ps();

        if (mclk_ps >= 2500)
                cwl = 5;
        else if (mclk_ps >= 1875)
                cwl = 6;
        else if (mclk_ps >= 1500)
                cwl = 7;
        else if (mclk_ps >= 1250)
                cwl = 8;
        else
                cwl = 8;

#if defined(CONFIG_APM86XXX_DDR_AUTOCALIBRATION)
	if (cwl < 8)
		cwl = 8;
#endif
        return cwl;
}

unsigned int compute_cas_latency_ddr3(const dimm_params_t *dimm_params,
                         timing_params_t *outpdimm,
                         unsigned int number_of_dimms)
{
        unsigned int i;
        unsigned int tAAmin_ps = 0;
        unsigned int tCKmin_X_ps = 0;
        unsigned int common_caslat;
        unsigned int caslat_actual;
        unsigned int retry = 16;
        unsigned int tmp;
        const unsigned int mclk_ps = get_memory_clk_period_ps();

        /* compute the common CAS latency supported between slots */
        tmp = dimm_params[0].caslat_X;
        for (i = 1; i < CONFIG_SYS_NO_OF_DIMMS; i++)
                 tmp &= dimm_params[i].caslat_X;
        common_caslat = tmp;

        /* compute the max tAAmin tCKmin between slots */
        for (i = 0; i < CONFIG_SYS_NO_OF_DIMMS; i++) {
                tAAmin_ps = max(tAAmin_ps, dimm_params[i].tAA_ps);
                tCKmin_X_ps = max(tCKmin_X_ps, dimm_params[i].tCKmin_X_ps);
        }
        /* validate if the memory clk is in the range of dimms */
        if (mclk_ps < tCKmin_X_ps) {
                debug("The DIMM max tCKmin is %d ps,"
                        "doesn't support the MCLK cycle %d ps\n",
                        tCKmin_X_ps, mclk_ps);
                return 1;
        }
        /* determine the acutal cas latency */
        caslat_actual = (tAAmin_ps + mclk_ps - 1) / mclk_ps;
        /* check if the dimms support the CAS latency */
        while (!(common_caslat & (1 << caslat_actual)) && retry > 0) {
                caslat_actual++;
                retry--;
        }
        /* once the caculation of caslat_actual is completed
         * we must verify that this CAS latency value does not
         * exceed tAAmax, which is 20 ns for all DDR3 speed grades
         */
        if (caslat_actual * mclk_ps > 20000) {
                debug("The choosen cas latency %d is too large\n",
                        caslat_actual);
                return 1;
        }
        outpdimm->lowest_common_SPD_caslat = caslat_actual;

#if defined(CONFIG_APM86XXX_DDR_AUTOCALIBRATION)
	if (outpdimm->lowest_common_SPD_caslat < 8) {
		debug ("Forcing CAS latency to 8 from original %d\n",
				outpdimm->lowest_common_SPD_caslat);
		outpdimm->lowest_common_SPD_caslat = 8;
	}
#endif
	debug("Lowest common CAS latency: %d\n", caslat_actual);

        return 0;
}

/*
 * ddr_compute_timing_params
 *
 * This will derive the timing parameters from dimm parameters
 */
unsigned int ddr_compute_timing_params(struct apm_ddr_info_s *ddr_info)
{
        unsigned int i;

        unsigned int tCKmin_X_ps = 0;
        unsigned int tCKmax_ps = 0xFFFFFFFF;
        unsigned int tCKmax_max_ps = 0;
        unsigned int tRCD_ps = 0;
        unsigned int tRP_ps = 0;
        unsigned int tRAS_ps = 0;
        unsigned int tWR_ps = 0;
        unsigned int tWTR_ps = 0;
        unsigned int tRFC_ps = 0;
        unsigned int tRRD_ps = 0;
        unsigned int tRC_ps = 0;
        unsigned int refresh_rate_ps = 0;
        unsigned int tIS_ps = 0;
        unsigned int tIH_ps = 0;
        unsigned int tDS_ps = 0;
        unsigned int tDH_ps = 0;
        unsigned int tRTP_ps = 0;
        unsigned int tDQSQ_max_ps = 0;
        unsigned int tQHS_ps = 0;

        unsigned int temp1, temp2;
        unsigned int additive_latency = 0;
	struct dimm_params_s *dimm_params = NULL;
	timing_params_t *tp;
#if !defined(CONFIG_SYS_ENABLE_DDR3)
	const unsigned int mclk_ps = get_memory_clk_period_ps();
	unsigned int lowest_good_caslat = 0;
	unsigned int not_ok = 0;
#endif

        temp1 = 0;
        for (i = 0; i < CONFIG_SYS_NO_OF_DIMMS; i++) {
                /*
                 * If there are no ranks on this DIMM,
                 * it probably doesn't exist, so skip it.
                 */
                if (ddr_info->dimm_params[0][i].n_ranks == 0) {
                        temp1++;
                        continue;
                }
		dimm_params = (struct dimm_params_s *)&ddr_info->dimm_params[0][i];

                /*
                 * Find minimum tCKmax_ps to find fastest slow speed,
                 * i.e., this is the slowest the whole system can go.
                 */
                tCKmax_ps = min(tCKmax_ps, dimm_params->tCKmax_ps);

                /* Either find maximum value to determine slowest
                 * speed, delay, time, period, etc */
                tCKmin_X_ps = max(tCKmin_X_ps, dimm_params->tCKmin_X_ps);
                tCKmax_max_ps = max(tCKmax_max_ps, dimm_params->tCKmax_ps);
                tRCD_ps = max(tRCD_ps, dimm_params->tRCD_ps);
                tRP_ps = max(tRP_ps, dimm_params->tRP_ps);
                tRAS_ps = max(tRAS_ps, dimm_params->tRAS_ps);
                tWR_ps = max(tWR_ps, dimm_params->tWR_ps);
                tWTR_ps = max(tWTR_ps, dimm_params->tWTR_ps);
                tRFC_ps = max(tRFC_ps, dimm_params->tRFC_ps);
                tRRD_ps = max(tRRD_ps, dimm_params->tRRD_ps);
                tRC_ps = max(tRC_ps, dimm_params->tRC_ps);
                tIS_ps = max(tIS_ps, dimm_params->tIS_ps);
                tIH_ps = max(tIH_ps, dimm_params->tIH_ps);
                tDS_ps = max(tDS_ps, dimm_params->tDS_ps);
                tDH_ps = max(tDH_ps, dimm_params->tDH_ps);
                tRTP_ps = max(tRTP_ps, dimm_params->tRTP_ps);
                tQHS_ps = max(tQHS_ps, dimm_params->tQHS_ps);
                refresh_rate_ps = max(refresh_rate_ps,
                                      dimm_params->refresh_rate_ps);

                /*
                 * Find maximum tDQSQ_max_ps to find slowest.
                 *
                 * FIXME: is finding the slowest value the correct
                 * strategy for this parameter?
                 */
                tDQSQ_max_ps = max(tDQSQ_max_ps, dimm_params->tDQSQ_max_ps);
        }

	//tp = &ddr_info->timing_params[0];
        ddr_info->timing_params[0].ndimms_present = CONFIG_SYS_NO_OF_DIMMS - temp1;
	tp = ddr_info->timing_params;

        if (temp1 == CONFIG_SYS_NO_OF_DIMMS) {
                debug("no dimms this memory controller\n");
                return 0;
        }

        tp->tCKmin_X_ps = tCKmin_X_ps;
        tp->tCKmax_ps = tCKmax_ps;
        tp->tCKmax_max_ps = tCKmax_max_ps;
        tp->tRCD_ps = tRCD_ps;
        tp->tRP_ps = tRP_ps;
        tp->tRAS_ps = tRAS_ps;
        tp->tWR_ps = tWR_ps;
        tp->tWTR_ps = tWTR_ps;
        tp->tRFC_ps = tRFC_ps;
        tp->tRRD_ps = tRRD_ps;
        tp->tRC_ps = tRC_ps;
        tp->refresh_rate_ps = refresh_rate_ps;
        tp->tIS_ps = tIS_ps;
        tp->tIH_ps = tIH_ps;
        tp->tDS_ps = tDS_ps;
        tp->tDH_ps = tDH_ps;
        tp->tRTP_ps = tRTP_ps;
        tp->tDQSQ_max_ps = tDQSQ_max_ps;
        tp->tQHS_ps = tQHS_ps;

	debug("tCK: min %d max %d max max %d\n", 
		tCKmin_X_ps, tCKmax_ps, tCKmax_max_ps);
	debug("tRCD: %d tRP: %d tRAS: %d\n", tRCD_ps, tRP_ps, tRAS_ps);
	debug("tWR: %d tWTR: %d tRFC: %d\n", tWR_ps, tWTR_ps, tRFC_ps);
	debug("tRRD: %d tRC: %d tREFI: %d\n", tRRD_ps, tRC_ps, refresh_rate_ps);
	debug("tIS: %d tIH: %d tDS: %d tDH: %d\n", 
		tIS_ps, tIH_ps, tDS_ps, tDH_ps);
	debug("tRTP: %d tDQSQ: %d tQHS: %d\n", tRTP_ps, tDQSQ_max_ps, tQHS_ps);

        /* Determine common burst length for all DIMMs. */
        temp1 = 0xff;
        for (i = 0; i < CONFIG_SYS_NO_OF_DIMMS; i++) {
                if (ddr_info->dimm_params[0][i].n_ranks) {
                        temp1 &= ddr_info->dimm_params[0][i].burst_lengths_bitmask;
                }
        }
        tp->all_DIMMs_burst_lengths_bitmask = temp1;

        /* Determine if all DIMMs registered buffered. */
        temp1 = temp2 = 0;
        for (i = 0; i < CONFIG_SYS_NO_OF_DIMMS; i++) {
                if (ddr_info->dimm_params[0][i].n_ranks) {
                        if (ddr_info->dimm_params[0][i].registered_dimm)
                                temp1 = 1;
                        if (!ddr_info->dimm_params[0][i].registered_dimm)
                                temp2 = 1;
                }
        }

        tp->all_DIMMs_registered = 0;
        if (temp1 && !temp2) {
                tp->all_DIMMs_registered = 1;
        }

        tp->all_DIMMs_unbuffered = 0;
        if (!temp1 && temp2) {
                tp->all_DIMMs_unbuffered = 1;
        }

        /* CHECKME: */
        if (!tp->all_DIMMs_registered
            && !tp->all_DIMMs_unbuffered) {
                debug("ERROR:  Mix of registered buffered and unbuffered "
                                "DIMMs detected!\n");
        }

#if defined(CONFIG_SYS_ENABLE_DDR3)
        if (compute_cas_latency_ddr3(ddr_info->dimm_params[0], tp, tp->ndimms_present))
                return 1;
#else
        /*
         * Compute a CAS latency suitable for all DIMMs
         *
         * Strategy for SPD-defined latencies: compute only
         * CAS latency defined by all DIMMs.
         */

        /*
         * Step 1: find CAS latency common to all DIMMs using bitwise
         * operation.
         */
        temp1 = 0xFF;
        for (i = 0; i < CONFIG_SYS_NO_OF_DIMMS; i++) {
                if (dimm_params[i].n_ranks) {
                        temp2 = 0;
                        temp2 |= 1 << dimm_params[i].caslat_X;
                        temp2 |= 1 << dimm_params[i].caslat_X_minus_1;
                        temp2 |= 1 << dimm_params[i].caslat_X_minus_2;
                        /*
                         * FIXME: If there was no entry for X-2 (X-1) in
                         * the SPD, then caslat_X_minus_2
                         * (caslat_X_minus_1) contains either 255 or
                         * 0xFFFFFFFF because that's what the glorious
                         * __ilog2 function returns for an input of 0.
                         * On 32-bit PowerPC, left shift counts with bit
                         * 26 set (that the value of 255 or 0xFFFFFFFF
                         * will have), cause the destination register to
                         * be 0.  That is why this works.
                         */
                        temp1 &= temp2;
                }
        }
        /*
         * Step 2: check each common CAS latency against tCK of each
         * DIMM's SPD.
         */
        lowest_good_caslat = 0;
        temp2 = 0;
        while (temp1) {
                not_ok = 0;
                temp2 =  __ilog2(temp1);
                debug("checking common caslat = %u\n", temp2);

                /* Check if this CAS latency will work on all DIMMs at tCK. */
                for (i = 0; i < CONFIG_SYS_NO_OF_DIMMS; i++) {
                        if (!dimm_params[i].n_ranks) {
                                continue;
                        }
                        if (dimm_params[i].caslat_X == temp2) {
                                if (mclk_ps >= dimm_params[i].tCKmin_X_ps) {
                                        debug("CL = %u ok on DIMM %u at tCK=%u"
                                            " ps with its tCKmin_X_ps of %u\n",
                                               temp2, i, mclk_ps,
                                               dimm_params[i].tCKmin_X_ps);
                                        continue;
                                } else {
                                        not_ok++;
                                }
                        }

                        if (dimm_params[i].caslat_X_minus_1 == temp2) {
                                unsigned int tCKmin_X_minus_1_ps
                                        = dimm_params[i].tCKmin_X_minus_1_ps;
                                if (mclk_ps >= tCKmin_X_minus_1_ps) {
                                        debug("CL = %u ok on DIMM %u at "
                                                "tCK=%u ps with its "
                                                "tCKmin_X_minus_1_ps of %u\n",
                                               temp2, i, mclk_ps,
                                               tCKmin_X_minus_1_ps);
                                        continue;
                                } else {
                                        not_ok++;
                                }
                        }

                        if (dimm_params[i].caslat_X_minus_2 == temp2) {
                                unsigned int tCKmin_X_minus_2_ps
                                        = dimm_params[i].tCKmin_X_minus_2_ps;
                                if (mclk_ps >= tCKmin_X_minus_2_ps) {
                                        debug("CL = %u ok on DIMM %u at "
                                                "tCK=%u ps with its "
                                                "tCKmin_X_minus_2_ps of %u\n",
                                               temp2, i, mclk_ps,
                                               tCKmin_X_minus_2_ps);
                                        continue;
                                } else {
                                        not_ok++;
                                }
                        }
                }

                if (!not_ok) {
                        lowest_good_caslat = temp2;
                }

                temp1 &= ~(1 << temp2);
        }

        debug("lowest common SPD-defined CAS latency = %u\n",
               lowest_good_caslat);
        tp->lowest_common_SPD_caslat = lowest_good_caslat;


        /*
         * Compute a common 'de-rated' CAS latency.
         *
         * The strategy here is to find the *highest* dereated cas latency
         * with the assumption that all of the DIMMs will support a dereated
         * CAS latency higher than or equal to their lowest dereated value.
         */
        temp1 = 0;
        for (i = 0; i < CONFIG_SYS_NO_OF_DIMMS; i++) {
                temp1 = max(temp1, dimm_params[i].caslat_lowest_derated);
        }
        tp->highest_common_derated_caslat = temp1;
        debug("highest common dereated CAS latency = %u\n", temp1);
#endif

        /* Determine if all DIMMs ECC capable. */
        temp1 = 1;
        for (i = 0; i < CONFIG_SYS_NO_OF_DIMMS; i++) {
                if (dimm_params[i].n_ranks && dimm_params[i].edc_config != 2) {
                        temp1 = 0;
                        break;
                }
        }
        if (temp1) {
                debug("all DIMMs ECC capable\n");
        } else {
                debug("Warning: not all DIMMs ECC capable, cant enable ECC\n");
        }
        tp->all_DIMMs_ECC_capable = temp1;

        /*
         * Compute additive latency.
         *
         * For DDR1, additive latency should be 0.
         *
         * For DDR2, with ODT enabled, use "a value" less than ACTTORW,
         *      which comes from Trcd, and also note that:
         *          add_lat + caslat must be >= 4
         *
         * For DDR3, we use the AL=0
         *
         * When to use additive latency for DDR2:
         *
         * I. Because you are using CL=3 and need to do ODT on writes and
         *    want functionality.
         *    1. Are you going to use ODT? (Does your board not have
         *      additional termination circuitry for DQ, DQS, DQS_,
         *      DM, RDQS, RDQS_ for x4/x8 configs?)
         *    2. If so, is your lowest supported CL going to be 3?
         *    3. If so, then you must set AL=1 because
         *
         *       WL >= 3 for ODT on writes
         *       RL = AL + CL
         *       WL = RL - 1
         *       ->
         *       WL = AL + CL - 1
         *       AL + CL - 1 >= 3
         *       AL + CL >= 4
         *  QED
         *
         *  RL >= 3 for ODT on reads
         *  RL = AL + CL
         *
         *  Since CL aren't usually less than 2, AL=0 is a minimum,
         *  so the WL-derived AL should be the  -- FIXME?
         *
         * II. Because you are using auto-precharge globally and want to
         *     use additive latency (posted CAS) to get more bandwidth.
         *     1. Are you going to use auto-precharge mode globally?
         *
         *        Use addtivie latency and compute AL to be 1 cycle less than
         *        tRCD, i.e. the READ or WRITE command is in the cycle
         *        immediately following the ACTIVATE command..
         *
         * III. Because you feel like it or want to do some sort of
         *      degraded-performance experiment.
         *     1.  Do you just want to use additive latency because you feel
         *         like it?
         *
         * Validation:  AL is less than tRCD, and within the other
         * read-to-precharge constraints.
         */
	/*
         * The system will not use the global auto-precharge mode.
         * However, it uses the page mode, so we set AL=0
         */
        additive_latency = 0;


        /*
         * Validate additive latency
         * FIXME: move to somewhere else to validate
         *
         * AL <= tRCD(min)
         */
        if (mclk_to_picos(additive_latency) > tRCD_ps) {
                debug("Error: invalid additive latency exceeds tRCD(min).\n");
                return 1;
        }

        /*
         * RL = CL + AL;  RL >= 3 for ODT_RD_CFG to be enabled
         * WL = RL - 1;  WL >= 3 for ODT_WL_CFG to be enabled
         * ADD_LAT (the register) must be set to a value less
         * than ACTTORW if WL = 1, then AL must be set to 1
         * RD_TO_PRE (the register) must be set to a minimum
         * tRTP + AL if AL is nonzero
         */

        /*
         * Additive latency will be applied only if the memctl option to
         * use it.
         */
        tp->additive_latency = additive_latency;

        return 0;
}

static u32 stablize_value(u32 val, u32 max, u8 dec)
{
	u32 tmp = 0;
	u32 scale = MEMC_STABILITY_FACTOR;
	char *s;	

	if ((s = getenv("DDR_SF")) != NULL)
	{
		scale = simple_strtoul(s, NULL, 10);
#if 0
		printf("Scaling DDR timing value by factor %d\n", scale);
#endif
	}

	/*
	 * if scaling 0, means we don't change the value
	 */
	if (scale == 0)
		return val;

	/*
	 * Figure out scaling change
	 */
	tmp = val / scale;
	if (tmp == 0)
		tmp = 1;

	/*
	 * We are scaling down
	 */
	if (dec) {
		if (tmp > val)
			val = 0;
		else
			val -= tmp;

		if (val < max)
			val = max;

	} else {
		val = val + tmp;
		if (val > max)
			val = max;
	}
	return val;
}

/*
 * Compute the dimm params
 */
void setup_memc_registers(timing_params_t *tp, dimm_params_t *dp, memc_regs_t *mcp)
{
	unsigned int tmpval, stval;
	unsigned short mr, emr1, emr2, emr3=0;
	unsigned int write_latency, additive_latency, burst_len, cas_wr_latency;
	unsigned int read_latency;
	unsigned int cas_latency, tWR, write_recovery;
	unsigned int i;
	unsigned int *ptr;
	unsigned int max_col;

	/* First make a copy of the memc registers */
	ptr = (unsigned int *)mcp;
	for (i = 0; i < sizeof(memc_regs_t)/4; i++) {
		mfsdram(i*4, tmpval);
		*(ptr++) = tmpval;
		debug("reg %x val %08x\n",i*4,tmpval); 
	}

	/*
	 * Lets program the JEDEC registers first
	 */
	cas_wr_latency = compute_cas_write_latency();
	burst_len = 0;
	cas_latency = tp->lowest_common_SPD_caslat;
	cas_latency = (cas_latency >= 12)? (((cas_latency - 12) << 1) | 0x1):
					((cas_latency - 4) << 1); 
	additive_latency = 0;	

#if defined(CONFIG_APM86XXX_DDR_AUTOCALIBRATION)
	/*
	 * Write recovery calculation for MR0
	 */
	tWR = time_ps_to_clk(tp->tWR_ps);
	if ((tWR > 16) || (tWR < 5) || (tWR == 13)) {
		printf("\n tWR exceed normal range: %d\n", tWR); 
		hang();
	} else
		tWR = (tWR == 16) ? 0 : (tWR - 4);
	mr = mcp->reg12 & 0xF080;
	mr |= (cas_latency << 3) | burst_len | tWR << 9;	
#else
	tWR = time_ps_to_clk(tp->tWR_ps);
	mr = mcp->reg12 & 0xF080;
	mr |= (cas_latency << 3) | burst_len;	

#endif

	emr1 = (mcp->reg12 >> 16) & 0xFFE7;
	emr1 |= additive_latency << 3; 

	mcp->reg12 = mr | emr1 << 16;

	emr2 = mcp->reg11 & 0xFFC7;
	emr2 |= (cas_wr_latency - 5) << 3;
	
	mcp->reg11 = emr2 | emr3 << 16;

	debug("MR0: 0x%x\n", mcp->reg12 & 0xffff);
	debug("MR1: 0x%x\n", (mcp->reg12 >> 16) & 0xffff);
	debug("MR2: 0x%x\n", mcp->reg11 & 0xffff);
	debug("MR3: 0x%x\n", (mcp->reg11 >> 16) & 0xffff);


	/*	
	 * Extrac some of the values out of the JEDEC settings
	 */
	cas_latency = tp->lowest_common_SPD_caslat;
	read_latency = additive_latency + cas_latency;
	write_latency = cas_wr_latency + additive_latency;

	burst_len = mr & 0x3;
	burst_len = (mr < 2) ? 4 : 8;
	write_recovery = time_ps_to_clk(tp->tWR_ps);

	debug("cas_latency: %d\n", cas_latency);
	debug("cas_wr_latency: %d\n", cas_wr_latency);
	debug("additive_latency: %d\n", additive_latency);
	debug("write_latency: %d\n", write_latency);
	debug("read_latency: %d\n", read_latency);
	debug("burst_length: %d\n", burst_len);
	debug("write_recover (tWR): %d\n", write_recovery);

	
#if defined(CONFIG_DDR_ECC)
	mcp->reg0 = (mcp->reg0 & 0x1FFFFFFF) | 0x40000000;
#else
	mcp->reg0 = mcp->reg0 & 0x1FFFFFFF;
#endif

	tmpval = rdiv(time_ps_to_clk(tp->refresh_rate_ps),  32 * MEMC_DIV);
	debug("tREFI in clk: (n/2) 0x%x\n", tmpval);
	mcp->reg1 = (mcp->reg1 & 0xFFFFF000) | (tmpval & 0xFFF);

	tmpval = rdiv(time_ps_to_clk(tp->tRFC_ps), MEMC_DIV);
	stval = stablize_value(tmpval, 0xFF, 0);	
	debug("tRFCmin in clk: (n/2) 0x%x st val 0x%x\n", tmpval, stval);
	mcp->reg5 = (mcp->reg5 & 0xFFFF00FF) | ((stval & 0xFF) << 8);
		
	tmpval = rdiv(time_ps_to_clk(tp->tRC_ps), MEMC_DIV);
	stval = stablize_value(tmpval, 0x3F, 0);	
	debug("tRCmin in clk: (n/2) 0x%x st val 0x%x\n", tmpval, stval);
	mcp->reg5 = (mcp->reg5 & 0xFFFFFFC0) | (stval & 0x3F);

	/* tRAS_min */
	tmpval = time_ps_to_clk(tp->tRAS_ps);
	debug("tRASmin in clk: 0x%x ", tmpval);
	tmpval = rdiv((tmpval - 1), MEMC_DIV);
	stval = stablize_value(tmpval, 0x1F, 0);	
	debug("(n-1)/2 0x%x st val 0x%x\n", tmpval, stval);
	mcp->reg6 = (mcp->reg6 & 0xE0FFFFFF) | ((tmpval & 0x1F) << 24);

	/* tRAS_max */
	tmpval = rdiv(time_ps_to_clk(tp->refresh_rate_ps) * 9, 1024);
	tmpval = rdiv((tmpval - 1), MEMC_DIV);
	stval = stablize_value(tmpval, 0x1, 1);	
	debug("tRASmax in clk: (n-1)/2 0x%x st val 0x%x\n", tmpval, stval);
	mcp->reg6 = (mcp->reg6 & 0xFFC0FFFF) | ((tmpval & 0x3F) << 16);

	/* Write to Precharge */
	tmpval = time_ps_to_clk(tp->tWR_ps);
	tmpval = rdiv(((write_latency + burst_len / 2 + tmpval)), MEMC_DIV);
	stval = stablize_value(tmpval, 0x1F, 0);	
	debug("Write to precharge: (n-1)/2 0x%x st val 0x%x\n", tmpval, stval);
	mcp->reg6 = (mcp->reg6 & 0xFFFFFFE0) | stval;

	/* tFAW */
	tmpval = rdiv(time_ps_to_clk(dp->tFAW_ps), MEMC_DIV);
	stval = stablize_value(tmpval, 0x3F, 0);	
	debug("tFAWmin in clk: (n/2) 0x%x st val 0x%x\n", tmpval, stval);
	mcp->reg6 = (mcp->reg6 & 0xFFFF03FF) | ((stval & 0x3F) << 10);
	
	/* tRCD */
	tmpval = rdiv(time_ps_to_clk(tp->tRCD_ps), MEMC_DIV);
	stval = stablize_value(tmpval, 0xF, 0);	
	debug("tRCDmin in clk: (n/2) 0x%x st val 0x%x\n", tmpval, stval);
	mcp->reg7 = (mcp->reg7 & 0x0FFFFFFF) | ((stval & 0xF) << 28);

	/* tRTP */
	tmpval = time_ps_to_clk(tp->tRTP_ps);
	if (burst_len > 4) 
		tmpval = tmpval + 2;
	tmpval = rdiv((tmpval - 1), MEMC_DIV);
	stval = stablize_value(tmpval, 0x1F, 0);	
	debug("tRTPmin in clk: (n-1)/2 0x%x st val 0x%x\n", tmpval, stval);
	mcp->reg7 = (mcp->reg7 & 0xF07FFFFF) | ((stval & 0x1F) << 23);

	/* Write to Read */
	tmpval = time_ps_to_clk(tp->tWTR_ps);
	tmpval = rdiv((tmpval + burst_len / 2 + write_latency),  MEMC_DIV);
	stval = stablize_value(tmpval, 0x1F, 0);	
	debug("w2r in clk: (n/2) 0x%x st val 0x%x\n", tmpval, stval);
	mcp->reg7 = (mcp->reg7 & 0xFFFF83FF) | (stval << 10); 

	/* Read to Write */
	tmpval = rdiv((read_latency + burst_len / 2 + 2 - write_latency), MEMC_DIV);
	stval = stablize_value(tmpval, 0x1F, 0);
	debug("r2w in clk: (n/2) 0x%x st val 0x%x\n", tmpval, stval);
	mcp->reg7 = (mcp->reg7 & 0xFFFFFC1F) | ((stval & 0x1F) << 5);

	/* Write Latency AL + CL - 2 */
	tmpval = rdiv(additive_latency + cas_latency - 2, 2);
	stval = stablize_value(tmpval, 0x1F, 0);
	debug("wrlat in clk: (n/2) 0x%x st val 0x%x\n", tmpval, stval);
	mcp->reg7 = (mcp->reg7 & 0xFFFFFFE0) | (stval & 0x1F);
 		
	/* Precharge to activate */
	tmpval = rdiv(time_ps_to_clk(tp->tRP_ps), MEMC_DIV) + 1;
	stval = stablize_value(tmpval, 0xF, 0);
	debug("tRP in clk: (n/2)+1 0x%x st val 0x%x\n", tmpval, stval);
	mcp->reg8 = (mcp->reg8 & 0xFFFF0FFF) | ((stval & 0xF) << 12);

	/* Activate bank to bank */
	tmpval = rdiv(time_ps_to_clk(tp->tRRD_ps), MEMC_DIV);
	stval = stablize_value(tmpval, 0x7, 0);
	debug("tRRD in clk: (n/2) 0x%x st val 0x%x\n", tmpval, stval);
	mcp->reg8 = (mcp->reg8 & 0xFFFFFF1F) | ((stval & 0x7) << 5);

	/*
	 * Set Col address bit position
	 */
	tmpval = 0;
	
#if defined(CONFIG_DDR_32BIT)
	max_col = 14;
#else
	/* Diamondback variant is 32-bit DDR */
	max_col = apm86xxx_is_32bit() ? 14 : 13;
#endif		
	for (i = dp->n_col_addr; i < max_col; i++)
		tmpval = (tmpval >> 4) | 0xF0000000;
	mcp->reg16 = tmpval;
	debug("Col bits are %d, set reg16 to %x\n", dp->n_col_addr, mcp->reg16);

	/*
	 * Set Row address bit postion
	 * Starting at col + bank - 9 - 1
	 */
	tmpval = 0;
	for (i = dp->n_row_addr - 1; i < 15; i++)
		tmpval = (tmpval >> 4) | 0x0F000000;
	mcp->reg17 = tmpval;
#if defined(CONFIG_DDR_32BIT)
	tmpval = dp->n_col_addr - 8;
#else
	/* Diamondback variant is 32-bit DDR */
	if (apm86xxx_is_32bit())
		tmpval = dp->n_col_addr - 8;
	else		
		tmpval = dp->n_col_addr - 7;
#endif
	for (i = dp->n_row_addr - 1; i > 8; i--)
		tmpval |= tmpval << 4;
	mcp->reg17 |= tmpval;
	debug("Row bits are %d, set reg16 to %x\n", dp->n_row_addr, mcp->reg17);

	/*
	 * Set CS and Bank address bit position
	 * Bank starting position col - 2 - 1
	 * CS starting position col + bank + row - 9 - 1
	 */
#if defined(CONFIG_DDR_32BIT)
	tmpval = dp->n_col_addr - 4;
#else
	/* Diamondback variant is 32-bit DDR */
	if (apm86xxx_is_32bit())
		tmpval = dp->n_col_addr - 4;
	else
		tmpval = dp->n_col_addr - 3;
#endif	
	tmpval |= tmpval << 4;
	tmpval |= tmpval << 4;
	mcp->reg15 = tmpval;
	
#if defined(CONFIG_DDR_32BIT)
	tmpval = dp->n_row_addr + dp->n_col_addr - 8;
#else
	/* Diamondback variant is 32-bit DDR */
	if (apm86xxx_is_32bit())
		tmpval = dp->n_row_addr + dp->n_col_addr - 8;
	else
		tmpval = dp->n_row_addr + dp->n_col_addr -7;
#endif
	tmpval = (tmpval << 12) | (tmpval << 17);
	mcp->reg15 |= tmpval;
	debug("Bank and CS bits, reg15 to %x\n", mcp->reg15);
	
	/* Rddata_en = RL -1 */
	tmpval = read_latency - 1;
	debug("rddata_en : 0x%x\n", tmpval);
	mcp->reg19 = (mcp->reg19 & 0xFFFFFFE0) | (tmpval & 0x1F);

#if defined(DDR1600)
	mcp->reg0 = 0x02814204;
	mcp->reg1 = 0x44000061;
	mcp->reg5 = 0x20105825;
	mcp->reg6 = 0x1c2060d2;
	mcp->reg7 = 0xa403c904;
	mcp->reg8 = 0x0488a28a;
	mcp->reg10 = 0x00803007;
	mcp->reg12 = 0x00000070;
	mcp->reg19 = 0x0030400a;
#endif
#if defined(DDR1333)
	mcp->reg0 = 0x02814200;
	mcp->reg1 = 0x44000061;
	mcp->reg5 = 0x20104920;
	mcp->reg6 = 0x182050cf;
	mcp->reg7 = 0x8383bd04;
	mcp->reg8 = 0x0488828a;
	mcp->reg10 = 0x00803007;
	mcp->reg12 = 0x00000050;
#endif
#if defined(DDR1066)
	mcp->reg0 = 0x02814200;
	mcp->reg1 = 0x44000061;
	mcp->reg5 = 0x20103919;
	mcp->reg6 = 0x12203ccd;
	mcp->reg7 = 0x6283b504;
	mcp->reg8 = 0x0488626a;
	mcp->reg10 = 0x00803007;
	mcp->reg12 = 0x00000050;
	mcp->reg19 = 0x00304008;
#endif
#if defined(DDR800)
	mcp->reg0 = 0x02814200;
	mcp->reg1 = 0x44000061;
	mcp->reg5 = 0x20102c13;
	mcp->reg6 = 0x0e2030cb;
	mcp->reg7 = 0x5283ad04;
	mcp->reg8 = 0x0488524a;
	mcp->reg10 = 0x00803007;
	mcp->reg12 = 0x00000050;
	mcp->reg19 = 0x00304008;
#endif
}

void populate_memc_registers(struct apm_ddr_info_s *ddr_info)
{
	memc_regs_t mcp;
	timing_params_t *tp = &(ddr_info->timing_params[0]);
	dimm_params_t *dp = &(ddr_info->dimm_params[0][0]);
	u32 ddrspeed;

	apm86xxx_get_freq(APM86xxx_DDR, &ddrspeed);
	ddrspeed /= 500000;

	/* Setup registers */
	setup_memc_registers(tp, dp, &mcp);

	/* ECC MODE, number of ranks */
#if defined(CONFIG_DDR_32BIT)
        memc_regmod(DDRC0_00, 1, 28, 29);
#else
	/* Diamondback variant is 32-bit DDR */
        if (apm86xxx_is_32bit())
	        memc_regmod(DDRC0_00, 1, 28, 29);
#endif
	
	/* tREFI */
        mtsdram(DDRC0_01, mcp.reg1);
	debug("DDRC0_01: 0x%x\n", mcp.reg1);

	/* tMRD, tRFCMin, tRC */
        mtsdram(DDRC0_05, mcp.reg5);
	debug("DDRC0_05: 0x%x\n", mcp.reg5);

	/* tRASMin, tRASMax, tFAW, wr2pre */
        mtsdram(DDRC0_06, mcp.reg6);
	debug("DDRC0_06: 0x%x\n", mcp.reg6);

	/* tRTPMin, tRCDMin, tXP, tWTR */
        mtsdram(DDRC0_07, mcp.reg7);
	debug("DDRC0_07: 0x%x\n", mcp.reg7);

	/* wrlat */
	memc_regmod(DDRC0_07,0x4,26,31); 

	/* tRD and DDR mode */
        mtsdram(DDRC0_08, mcp.reg8);
	debug("DDRC0_08: 0x%x\n", mcp.reg8);

	/* pre_cke_x1024 */
	memc_regmod(DDRC0_10, 0x186, 10, 19);

	/* mr to emr3 */
        mtsdram(DDRC0_11, mcp.reg11);
	debug("DDRC0_11: 0x%x\n", mcp.reg11);
        mtsdram(DDRC0_12, mcp.reg12);
	debug("DDRC0_12: 0x%x\n", mcp.reg12);

	/* l2 optimization */
	memc_regmod(DDRC0_13, 0xc, 20, 23);

	/* disable the PLB to mailbox backpressure */
	memc_regmod(MEMQ_MISC, 1, 0,0);

	/* Set col row bank cs bits */
	mtsdram(DDRC0_15, mcp.reg15);
	mtsdram(DDRC0_16, mcp.reg16);
	mtsdram(DDRC0_17, mcp.reg17);

	/* rddata_en */
	mtsdram(DDRC0_19, mcp.reg19);

	/* Number of cycles to assert DRAM reset signal */
	memc_regmod(DDRC0_33, 0xF, 4, 11);

	memc_regmod(PHY_1, 0x3, 17, 18);

	/* Set to 4 or 5, 4 is more performance */
	if (ddrspeed > 1333)
		memc_regmod(PHY_1, 0x5, 4, 7);
	else
		memc_regmod(PHY_1, 0x4, 4, 7);

	/* rdc_we_to_re_delay */
	if (ddrspeed > 1333)
		memc_regmod(PHY_2, 0x5, 3, 7);
	else	
		memc_regmod(PHY_2, 0xF, 3, 7);


	/* Muti rank stability */
	memc_regmod(PHY_2, 1, 20, 20);

	
}
#endif /* CONFIG_FIXED_DDR_INIT */
#endif


void enable_ddr(void)
{
	struct apm86xxx_reset_ctrl reset_ctrl;
	struct apm86xxx_pll_ctrl ddr_pll;
	int i, rc;
	u32 val;
#if defined(CONFIG_APM86XXX_DDR_AUTOCALIBRATION)
	u32 ddrspeed;
	char *s;
#endif
        
	/*
	 * Only call this on non core reset
	 */
	if ((mfspr(SPRN_DBSR) & 0x30000000) == 0x10000000)
		return;

	if (mfspr(SPRN_PIR) != 0)
		return;

	/* 
	 * Setup DDR PLL 
	 * Assume output and NR divisor is 1
	 */
	ddr_pll.clkf_val = CONFIG_SYS_DDR_FREQ / CONFIG_SYS_CLK_FREQ - 1;
#if defined(CONFIG_APM86XXX_DDR_AUTOCALIBRATION)
	
	if ((s = getenv("DDR_SPEED")) != NULL) {
		ddrspeed = simple_strtoul(s, NULL, 10);
	} else 
		ddrspeed = CONFIG_SYS_DDR_FREQ / 1000000;

	debug("DDR_SPEED set to %dMHz\n", ddrspeed);

        if (ddrspeed == 800) {
                ddr_pll.clkf_val = 0x1f;
                ddr_pll.clkr_val = 0;
        }
        if (ddrspeed == 1066) {
                ddr_pll.clkf_val = 0x7f;
                ddr_pll.clkr_val = 2;
        }
        if (ddrspeed == 1333) {
                ddr_pll.clkf_val = 0x9f;
                ddr_pll.clkr_val = 0x2;
        }
        if (ddrspeed == 1600) {
                ddr_pll.clkf_val = 0x7f;
                ddr_pll.clkr_val = 0x1;
        }

#else
	ddr_pll.clkr_val = 0;
#endif
	ddr_pll.clkod_val = 0;
	ddr_pll.bwadj_val = 0x1f;

	/* Enable DDR PLL */
	disable_ddr_pll();
	enable_ddr_pll(&ddr_pll);

	/* Enable DDR Block */
	for (i = 0; i < sizeof(reset_ctrl); i++) 
		((unsigned char *) &reset_ctrl)[i] = 0;		
	reset_ctrl.reg_group = REG_GROUP_SRST_CLKEN;
	reset_ctrl.clken_mask = 0;
	reset_ctrl.csr_reset_mask = 0;
	reset_ctrl.reset_mask = 0;
        reset_ctrl.ram_shutdown_addr = (void __iomem *) (CONFIG_SYS_MB_DDRC_BASE+0x3e70);
        reset_ctrl.mem_rdy_mask = MEMC_F1_MASK;

        rc = reset_apm86xxx_block(&reset_ctrl);
        if (rc) {
                SPD_DBG_PRINT("MEMQ reset failed\n");
		return;
        }
        debug("MEMQ out of reset ... \n");
	debug("RAM Shutdown Addr: %x value: %x\n",
		(unsigned int)(reset_ctrl.ram_shutdown_addr),
		*((volatile u32 *)(reset_ctrl.ram_shutdown_addr)));

	/* Take SCU DDR divider out of reset */
	apm86xxx_read_scu_reg(SCU_SOC_SMS_CTL_ADDR,  &val);
	val |= 0x80;
	apm86xxx_write_scu_reg(SCU_SOC_SMS_CTL_ADDR, val);
	debug("SOC_SMS_CTL reg: %x\n", val);
}


int memc_wrlvl(u8 rank, u8 slice, u16 wrlvl) 
{
        u32 reg_wrlvl = PHY_3_R0_S0;
        int i;

        for(i=0;i<rank;i++) {
                reg_wrlvl += 0x4;
	}
        for(i=0;i<slice;i++) {
                reg_wrlvl += 0x10;
	}

        /* SET wrlvl_init_ratio */
	memc_regmod(reg_wrlvl, wrlvl, 2, 11);

        return 0;
}

int memc_wr_dqs_dly(u8 rank, u8 slice, u16 ratio) 
{
        u32 wr_dqs_slave = PHY_WR_DQS_SLAVE_R0_S0;
        int i;
        for(i=0;i<rank;i++)
                wr_dqs_slave += 0x4;
        for(i=0;i<slice;i++)
                        wr_dqs_slave += 0x10;

        /* SET RATIO */
        memc_regmod(wr_dqs_slave,ratio,22,31);

        return 0;
}

int memc_gatelvl(u8 rank, u8 slice, u16 gatelvl) {
        u32 reg_wrlvl = PHY_3_R0_S0;
        int i;
        for(i=0;i<rank;i++)
                reg_wrlvl += 0x4;
        for(i=0;i<slice;i++)
                reg_wrlvl += 0x10;

        /* SET gatelvl_init_ratio */
        memc_regmod(reg_wrlvl,gatelvl,14,23);

        return 0;
}


int memc_wr_data_dly(u8 rank, u8 slice,u16 ratio) {
        u32 wr_data_slave = PHY_WR_DATA_SLAVE_R0_S0;
        int i;
        for(i=0; i<rank; i++)
                wr_data_slave += 0x4;
        for(i=0; i<slice; i++)
                wr_data_slave += 0x10;

        /* SET RATIO */
        memc_regmod(wr_data_slave,ratio,22,31);

        return 0;
}

int memc_fifo_we_dly(u8 rank, u8 slice, u16 ratio) {
        u32 fifo_we_slave = PHY_FIFO_WE_SLAVE_R0_S0 ;
        int i;
        for(i=0;i<rank;i++)
                fifo_we_slave += 0x4;
        for(i=0;i<slice;i++)
                fifo_we_slave += 0x10;

        /* SET RATIO */
        memc_regmod(fifo_we_slave,ratio,22,31);

        return 0;
}

int memc_rd_dqs_dly(u8 rank, u8 slice, u16 ratio) {
        u32 rd_dqs_slave = PHY_RD_DQS_SLAVE_R0_S0;
        int i;
        for(i=0;i<rank;i++)
                rd_dqs_slave += 0x4;
        for(i=0;i<slice;i++)
                        rd_dqs_slave += 0x10;

        /* SET RATIO */
        memc_regmod(rd_dqs_slave,ratio,22,31);

        return 0;
}

int memc_dq_dqs_offset(u8 slice, u8 offset) {
        if(slice==0){
                memc_regmod(PHY_DQ_OFFSET_0,offset,25,31);
        } else if (slice==1) {
                memc_regmod(PHY_DQ_OFFSET_0,offset,17,23);
        } else if (slice==2) {
                memc_regmod(PHY_DQ_OFFSET_0,offset,9,15);
        } else if (slice==3) {
                memc_regmod(PHY_DQ_OFFSET_0,offset,1,7);
        } else if (slice==4) {
                memc_regmod(PHY_DQ_OFFSET_1,offset,25,31);
        } else if (slice==5) {
                memc_regmod(PHY_DQ_OFFSET_1,offset,17,23);
        } else if (slice==6) {
                memc_regmod(PHY_DQ_OFFSET_1,offset,9,15);
        } else if (slice==7) {
                memc_regmod(PHY_DQ_OFFSET_1,offset,1,7);
        } else if (slice==8) {
                memc_regmod(PHY_DQ_OFFSET_2,offset,25,31);
        }

        return 0;
}

int memc_wr_dqs_dly_rd(u8 rank, u8 slice) {
        u32 wr_dqs_slave = PHY_WR_DQS_SLAVE_R0_S0;
        int i,ratio;
        for(i=0;i<rank;i++)
                wr_dqs_slave += 0x4;
        for(i=0;i<slice;i++)
		wr_dqs_slave += 0x10;
        ratio = memc_regread(wr_dqs_slave,22,31);
        return ratio;
}

int memc_fifo_we_dly_rd(u8 rank, u8 slice) {
        u32 fifo_we_slave = PHY_FIFO_WE_SLAVE_R0_S0 ;
        int i;

        for(i=0;i<rank;i++)
                fifo_we_slave += 0x4;
        for(i=0;i<slice;i++)
                fifo_we_slave += 0x10;

        return memc_regread(fifo_we_slave,22,31);
}

int memc_rd_dqs_dly_rd(int rank, int slice) {
        u32 rd_dqs_slave = PHY_RD_DQS_SLAVE_R0_S0;
        int i;
        for(i=0;i<rank;i++)
                rd_dqs_slave += 0x4;
        for(i=0;i<slice;i++)
                        rd_dqs_slave += 0x10;

        return memc_regread(rd_dqs_slave,22,31);;
}

void memc_dimm_mr_wr(u8 mr_addr,u16 mr_data) {
	int i = 0;

        memc_regmod(DDRC0_09, mr_data, 0, 15);
        memc_regmod(DDRC0_09, mr_addr, 16, 17);

	/* Pulse signal for one clock period */
        memc_regmod(DDRC0_09, 1, 23, 23);
	i++;
	i++;
	i++;
        memc_regmod(DDRC0_09, 0, 23, 23);

}

#if 0
void check_timings(void)
{
	u32 val;
	
		/* Program the memory datasheet derived values */
	u32 clk_period = (100 / ((CONFIG_SYS_DDR_FREQ / 2) / 100000000LL));

	apm86xxx_get_freq(APM86xxx_DDR, &val);
	debug("apm86xx_get_freq(DDR) returned %d\n", val);
	debug("clk_period = 0x%08x\n", clk_period);
	/* t_rfc_nom_x32 */
	val = (DDR_DSVAL_tREFI/clk_period) / (2*32);
	debug("t_rfc_nom_x32 = 0x%08x\n", val/10);
	/* t_rfc_min */
	val = DIV_ROUND_UP((DDR_DSVAL_tRFC_MIN / clk_period), 2);
	debug("t_rfc_min = 0x%08x\n", val/10);
	/* t_rc */
	val = DIV_ROUND_UP((DDR_DSVAL_tRC_MIN / clk_period), 2);
	debug("t_rc = 0x%08x\n", val/10);		
	/* t_ras_min */
	val = DIV_ROUND_UP(((DDR_DSVAL_tRAS_MIN / clk_period) - 1), 2);
	debug("t_ras_min = 0x%08x\n", val/10);		
	/* t_ras_max */
	val = (9 * DDR_DSVAL_tREFI) / (clk_period * 1024);
	debug("t_ras_max = 0x%08x\n", val/10);
	/* t_faw */
	val = DIV_ROUND_UP((DDR_DSVAL_tFAW / clk_period), 2);
	debug("t_faw = 0x%08x\n", val/10);		
	/* wr2pre */
	val = DIV_ROUND_UP(((DDR_DSVAL_CWL + DDR_DSVAL_AL) + (DDR_DSVAL_BURSTLEN / 2) + (DDR_DSVAL_tWR / clk_period)) - 1, 2);
	debug("wr2pre = 0x%08x\n", val/10);		
	/* t_rcd */
	val = DIV_ROUND_UP((DDR_DSVAL_tRCD / clk_period), 2);
	debug("t_rcd = 0x%08x\n", val/10);
	/* rd2pre */
	val = DIV_ROUND_UP(((DDR_DSVAL_tRTP / clk_period) + ((DDR_DSVAL_BURSTLEN > 4) ? 2 : 0) - 1), 2);
	debug("rd2pre = 0x%08x\n", val/10);
	/* wr2rd */
	val = DIV_ROUND_UP(((DDR_DSVAL_CWL + DDR_DSVAL_AL) + (DDR_DSVAL_BURSTLEN / 2) + DDR_DSVAL_tWTR / clk_period), 2);
	debug("wr2rd = 0x%08x\n", val/10);
	/* rd2wr */
	val = DIV_ROUND_UP(((DDR_DSVAL_CL + DDR_DSVAL_AL) + (DDR_DSVAL_BURSTLEN / 2) + 2 - (DDR_DSVAL_CWL + DDR_DSVAL_AL)), 2);
	debug("rd2wr = 0x%08x\n", val/10);
	/* dfi_t_wrlat */
	val = DIV_ROUND_UP((DDR_DSVAL_CWL + DDR_DSVAL_AL) - 3, 2);
	debug("dfi_t_wrlat = 0x%08x\n", val/10);
	/* t_rp */
	val = DIV_ROUND_UP((DDR_DSVAL_tRP / clk_period), 2) + 1;
	debug("t_rp = 0x%08x\n", val/10);
	/* t_rrd */
	val = DIV_ROUND_UP((DDR_DSVAL_tRRD / clk_period), 2);
	debug("t_rrd = 0x%08x\n", val/10);
	/* dfi_t_rddata_en */
	val = (DDR_DSVAL_CL + DDR_DSVAL_AL) - 1;
	debug("dfi_t_rddata_en = 0x%08x\n", val/10);
}
#endif

#if defined(CONFIG_MAMBA_WAP) || defined(CONFIG_MERAKI)
void board_calc_ratio(void)
{
	int i/*,retval = 0*/;
	int index = 0;

	/* 10 bits xx_xxxx_xxxx */
	memc_regmod(PHY_CTRL_SLAVE, 0x80, 22, 31);

	for (i = 0; i < 9; i++) {
		memc_wrlvl(0, i, 0);
	}
	/* 7 bits xxx_xxxx */
	memc_dq_dqs_offset(0,MEMC_DQS2DQ_OFFSET);
	memc_dq_dqs_offset(1,MEMC_DQS2DQ_OFFSET);
	memc_dq_dqs_offset(2,MEMC_DQS2DQ_OFFSET);
	memc_dq_dqs_offset(3,MEMC_DQS2DQ_OFFSET);
	memc_dq_dqs_offset(4,MEMC_DQS2DQ_OFFSET);
	memc_dq_dqs_offset(5,MEMC_DQS2DQ_OFFSET);
	memc_dq_dqs_offset(6,MEMC_DQS2DQ_OFFSET);
	memc_dq_dqs_offset(7,MEMC_DQS2DQ_OFFSET);
	memc_dq_dqs_offset(8,MEMC_DQS2DQ_OFFSET);
	/* 10 bits xx_xxxx_xxxx */
	memc_wr_dqs_dly(0,0,0x59);
	memc_wr_dqs_dly(0,1,0x3e);
	memc_wr_dqs_dly(0,2,0x3c);
	memc_wr_dqs_dly(0,3,0x4d);
	memc_wr_dqs_dly(0,4,0x94);
	memc_wr_dqs_dly(0,5,0xA0);
	memc_wr_dqs_dly(0,6,0xA9);
	memc_wr_dqs_dly(0,7,0xAF);
	memc_wr_dqs_dly(0,8,0xAF);
	/* 10 bits xx_xxxx_xxxx */
	memc_wr_data_dly(0,0,0x99);
	memc_wr_data_dly(0,1,0x7e);
	memc_wr_data_dly(0,2,0x7c);
	memc_wr_data_dly(0,3,0x8d);
	memc_wr_data_dly(0,4,0xD4);
	memc_wr_data_dly(0,5,0xE0);
	memc_wr_data_dly(0,6,0xE9);
	memc_wr_data_dly(0,7,0xEF);
	memc_wr_data_dly(0,8,0xEF);
	/* 10 bits xx_xxxx_xxxx */
	memc_gatelvl(0,0,0x14D);
	memc_gatelvl(0,1,0x15B);
	memc_gatelvl(0,2,0x157);
	memc_gatelvl(0,3,0x16A);
	memc_gatelvl(0,4,0x17B);
	memc_gatelvl(0,5,0x16F);
	memc_gatelvl(0,6,0x18B);
	memc_gatelvl(0,7,0x185);
	memc_gatelvl(0,8,0x185);
	/* 10 bits xx_xxxx_xxxx */
	memc_fifo_we_dly(0,0,0x6e);
	memc_fifo_we_dly(0,1,0x62);
	memc_fifo_we_dly(0,2,0x67);
	memc_fifo_we_dly(0,3,0x6e);
	memc_fifo_we_dly(0,4,/*0xb0*/index);
	memc_fifo_we_dly(0,5,/*0xb0*/index);
	memc_fifo_we_dly(0,6,/*0xb0*/index);
	memc_fifo_we_dly(0,7,/*0xb0*/index);
	memc_fifo_we_dly(0,8,/*0xb0*/index);
	/* 10 bits xx_xxxx_xxxx */
	memc_rd_dqs_dly(0,0,0x27);
	memc_rd_dqs_dly(0,1,0x25);
	memc_rd_dqs_dly(0,2,0x29);
	memc_rd_dqs_dly(0,3,0x25);
	memc_rd_dqs_dly(0,4,0x43);
	memc_rd_dqs_dly(0,5,0x43);
	memc_rd_dqs_dly(0,6,0x43);
	memc_rd_dqs_dly(0,7,0x43);
	memc_rd_dqs_dly(0,8,0x43);

}
#else
void board_calc_ratio(void)
{
	int i;
	u32 rddata_en;

	mfsdram(DDRC0_19, rddata_en);
	memc_regmod(PHY_CTRL_SLAVE, 0x80, 22, 31);

	for (i = 0; i < 9; i++) {
		memc_wrlvl(0, i, 0);
	}

	memc_dq_dqs_offset(0,0x40);
	memc_dq_dqs_offset(1,0x40);
	memc_dq_dqs_offset(2,0x40);
	memc_dq_dqs_offset(3,0x40);
	memc_dq_dqs_offset(4,0x40);
	memc_dq_dqs_offset(5,0x40);
	memc_dq_dqs_offset(6,0x40);
	memc_dq_dqs_offset(7,0x40);
	memc_dq_dqs_offset(8,0x40);

	for (i = 0; i < 4; i++) {
		memc_wr_dqs_dly(i,0,0x3e);
		memc_wr_dqs_dly(i,1,0x55);
		memc_wr_dqs_dly(i,2,0x70);
		memc_wr_dqs_dly(i,3,0x7d);
		memc_wr_dqs_dly(i,4,0xb4);
		memc_wr_dqs_dly(i,5,0xcd);
		memc_wr_dqs_dly(i,6,0xda);
		memc_wr_dqs_dly(i,7,0xe9);
		memc_wr_dqs_dly(i,8,0x94);

		memc_wr_data_dly(i,0,0x7e);
		memc_wr_data_dly(i,1,0x95);
		memc_wr_data_dly(i,2,0xb0);
		memc_wr_data_dly(i,3,0xbd);
		memc_wr_data_dly(i,4,0xf4);
		memc_wr_data_dly(i,5,0x10d);
		memc_wr_data_dly(i,6,0x11a);
		memc_wr_data_dly(i,7,0x129);
		memc_wr_data_dly(i,8,0xd4);

		if ((rddata_en & 0x1f) == 0x09) {
			/* if rddata_en = 9 user below */
			memc_gatelvl(i,0,0x139);
			memc_gatelvl(i,1,0x14e);
			memc_gatelvl(i,2,0x15f);
			memc_gatelvl(i,3,0x17f);
			memc_gatelvl(i,4,0x1a5);
			memc_gatelvl(i,5,0x1b9);
			memc_gatelvl(i,6,0x1d8);
			memc_gatelvl(i,7,0x1f5);
			memc_gatelvl(i,8,0x193);

			/* if rddata_en = 9 user below */
			memc_fifo_we_dly(i,0,0xb8);
			memc_fifo_we_dly(i,1,0xc0);
			memc_fifo_we_dly(i,2,0xc8);
			memc_fifo_we_dly(i,3,0xd0);
			memc_fifo_we_dly(i,4,0xd8);
			memc_fifo_we_dly(i,5,0xe0);
			memc_fifo_we_dly(i,6,0xe8);
			memc_fifo_we_dly(i,7,0xf0);
			memc_fifo_we_dly(i,8,0xd4);
		} else {
			memc_gatelvl(i,0,0x79);
			memc_gatelvl(i,1,0x8e);
			memc_gatelvl(i,2,0x9f);
			memc_gatelvl(i,3,0xbf);
			memc_gatelvl(i,4,0xe6);
			memc_gatelvl(i,5,0xf9);
			memc_gatelvl(i,6,0x118);
			memc_gatelvl(i,7,0x138);
			memc_gatelvl(i,8,0xd3);

			memc_fifo_we_dly(i,0,0x180);
			memc_fifo_we_dly(i,1,0x188);
			memc_fifo_we_dly(i,2,0x190);
			memc_fifo_we_dly(i,3,0x198);
			memc_fifo_we_dly(i,4,0x1a0);
			memc_fifo_we_dly(i,5,0x1a8);
			memc_fifo_we_dly(i,6,0x1b0);
			memc_fifo_we_dly(i,7,0x1b8);
			memc_fifo_we_dly(i,8,0x19c);
		}

		memc_rd_dqs_dly(i,0,0x43);
		memc_rd_dqs_dly(i,1,0x43);
		memc_rd_dqs_dly(i,2,0x43);
		memc_rd_dqs_dly(i,3,0x43);
		memc_rd_dqs_dly(i,4,0x43);
		memc_rd_dqs_dly(i,5,0x43);
		memc_rd_dqs_dly(i,6,0x43);
		memc_rd_dqs_dly(i,7,0x43);
		memc_rd_dqs_dly(i,8,0x43);
	}
}
#endif

#if defined(CONFIG_FIXED_DDR_INIT)
phys_size_t initdram_int(int board_type)
{
	phys_size_t dram_size = CONFIG_SYS_MBYTES_SDRAM << 20; 
	u32 ddrspeed = CONFIG_SYS_DDR_FREQ / 1000000;
	
	/* U-Boot can only really handle 2GB */
	if (CONFIG_SYS_MBYTES_SDRAM > (CONFIG_MAX_MEM_MAPPED >> 20))
	    dram_size = CONFIG_MAX_MEM_MAPPED;
	
#if (defined(CONFIG_NAND_U_BOOT) && !defined(CONFIG_NAND_SPL)) || defined(CONFIG_DDR_U_BOOT)
	return dram_size;
#endif
#if defined(CONFIG_DDR_SPD)
	u8 i;
	struct apm_ddr_info_s ddr_info;
#endif

#if defined(CONFIG_HW_BRINGUP) && !defined(CONFIG_HW_DDR)
	/* This is for booting U-Boot using FAM */
	return 0x80000;
#endif

	u32 memc_inited;

	debug("\nEntering static initdram() for %dMB\n", CONFIG_SYS_MBYTES_SDRAM);
#if defined(CONFIG_NAND_SPL_S2) || defined(CONFIG_MEDIA_SPL) || !defined(CONFIG_MEDIA_U_BOOT)
	enable_ddr();
#if !defined(CONFIG_NAND_SPL_S2)
	apm86xxx_get_freq(APM86xxx_DDR, &ddrspeed);
	ddrspeed /= 500000;
#endif
#endif

#if defined(CONFIG_PAL)
	dram_size = 0x10000000;
	mtsdram(DDRC0_00, 0x020f3803);
#endif

	/* Check if the Memory Controller already initialized */
	mfsdram(MEMRANGE, memc_inited);

#if !defined(CONFIG_PAL)
	if (!memc_inited) {
#if !defined(CONFIG_ISS)
		/* Initialize drive control */
		if ((ddrspeed > 800) && (ddrspeed <= 1333))
			mtsdram(DDR_IO_CONTROL, 0x39000000);
		else
			mtsdram(DDR_IO_CONTROL, 0x5b400200);
		/* Clear shutdown bits */
		mtsdram(MEM_RAM_SHUTDOWN, 0);

#if defined(SIMPLE_FIXED_INIT)
		/* Initialize the JEDEC registers */
		mtsdram(DDRC0_00,);
		mtsdram(DDRC0_01,);
		mtsdram(DDRC0_05,);
		mtsdram(DDRC0_06,);
		mtsdram(DDRC0_07,);
		mtsdram(DDRC0_08,);
		mtsdram(DDRC0_10,);
		mtsdram(DDRC0_11,);
		mtsdram(DDRC0_12,);
		mtsdram(DDRC0_13,);
		mtsdram(DDRC0_19,);
		mtsdram(DDRC0_33,);

		mtsdram(PHY_1,);
		mtsdram(PHY_2,);
	
#elif defined(REWORK_FIXED_INIT)

		/* Initialize the JEDEC registers */
		/* Modified */
		memc_mod(DDR_IO_CONTROL, DDR_IOCTRL_ODTM);	/* set ODT mode */
		memc_mod(DDRC0_00, DDRC0_ENRANKS);	/* enable ranks */
		memc_mod(DDRC0_00, DDRC0_RMODE);	/* rmode */
		memc_mod(DDRC0_00, DDRC0_ODTBLK); 	/* wr_odt_block */
#if 0 /* FKAN remove old code, take too much space */
		memc_mod(DDRC0_01, DDRC1_tREFI);
		memc_mod(DDRC0_05, DDRC5_tRFCMIN);	/* trfc min */
		memc_mod(DDRC0_05, DDRC5_tRC);		/* t_rc */
		memc_mod(DDRC0_06, DDRC6_tRASMIN);	/* tRAS min */
		memc_mod(DDRC0_06, DDRC6_tRASMAX);	/* tRAS max */
		memc_mod(DDRC0_06, DDRC6_tFAWVAL);	/* tFAW Valid */
		memc_mod(DDRC0_06, DDRC6_WR2PRE);	/* wr2pre = wl+(bl/2)+ twr */
		memc_mod(DDRC0_07, DDRC7_WRLAT);	/* wrlat Time from write command to write data*/
		memc_mod(DDRC0_07, DDRC7_tRCD);		/* t_rcd */
		memc_mod(DDRC0_07, DDRC7_tRTP);		/* tRTP Minimum time from read to precharge */
		memc_mod(DDRC0_07, DDRC7_WR2RD);	/* wr2rd = wl+twtr + bl/2.*/
		memc_mod(DDRC0_07, DDRC7_RD2WR);	/* rd2wr = rl +bl/2 + 2 -wl*/
		memc_mod(DDRC0_08, DDRC8_tRPMIN);	/* t_rp tRP Minimum time */
		memc_mod(DDRC0_08, DDRC8_tRRDMIN);	/* t_rrd tRRD Minimum time */
		mtsdram(DDRC0_11, DDRC11_EMR);
		memc_mod(DDRC0_10, DDRC10_PRECKE);
		mtsdram(DDRC0_12, DDRC12_MR);

		memc_mod(PHY_1, PHY1_WRLODT);
		memc_mod(PHY_1, PHY1_RDLODT);
		memc_mod(PHY_1, PHY1_WE2REDLY);
		
                memc_mod(PHY_1, PHY1_DSLICE); 
		mtsdram(DDRC0_19, DDRC19_RDDATA);
#else
		mtsdram(DDRC0_01, DDRC1_VAL);
		mtsdram(DDRC0_05, DDRC5_VAL);
		mtsdram(DDRC0_06, DDRC6_VAL);
		mtsdram(DDRC0_07, DDRC7_VAL);
		mtsdram(DDRC0_08, DDRC8_VAL);
		mtsdram(DDRC0_10, DDRC10_VAL);
		mtsdram(DDRC0_11, DDRC11_VAL);
		mtsdram(DDRC0_12, DDRC12_VAL);
		mtsdram(DDRC0_18, DDRC18_VAL);
		mtsdram(DDRC0_18_1, DDRC18_1_VAL);
		mtsdram(DDRC0_19, DDRC19_VAL);
		mtsdram(DDRC0_23, DDRC23_VAL);
		mtsdram(PHY_1, PHY1_VAL);
#endif

		memc_mod(DDRC0_13, DDRC13_L2SIZZ);
                memc_mod(DDRC0_33, DDRC33_RSTN); 
		
		memc_mod(PHY_2, PHY2_WRRL);
		memc_mod(PHY_2, PHY2_DLYSEL);
                memc_mod(DDRC0_00, DDRC0_DQWDTH); 

		/* 64/32bit related */
		memc_mod(DDRC0_15, DDRC15_RADDR0);
		memc_mod(DDRC0_15, DDRC15_RADDR1);
		memc_mod(DDRC0_15, DDRC15_BADDR0);
		memc_mod(DDRC0_15, DDRC15_BADDR1);
		memc_mod(DDRC0_15, DDRC15_BADDR2);
                mtsdram(DDRC0_16, DDRC16_COLADDRS); 
                mtsdram(DDRC0_17, DDRC17_ROWADDRS); 

		/* Manual wear leveling */
		board_calc_ratio();
#elif defined(CONFIG_SYS_SIMULATE_SPD_EEPROM)

		/*
		 * Derive the dimm parameter from the SPD information
		 */
		compute_dimm_info(&ddr_info);

		/*
		 * Convert SPD info to timing parameters
		 */
		ddr_compute_timing_params(&ddr_info);

		/*
		 * Program DIMM info into MEMC
		 */
		populate_memc_registers(&ddr_info);
	
		board_calc_ratio();
#else	/* default FIXED_INIT */

		
		/* Initialize the JEDEC registers */
		memc_regmod(DDRC0_01, 0x51, 20, 31);

#if defined(CONFIG_HW_REV_14)
		/* Adjust IO drive strength */
		mtsdram(DDR_IO_CONTROL, 0x5d800000);

		/* Activate banks: one bank for now */
		memc_regmod(DDRC0_00, 0x1, 12, 15);

		/* High speed */
		memc_regmod(DDRC0_00, 0x2, 6, 7);
		
		/* Odt setting */
                memc_regmod(DDRC0_00, 0x0, 10, 11); 

                memc_regmod(DDRC0_05, 0x28, 16, 23); //trfc
                memc_regmod(DDRC0_05, 0x14, 26, 31); //t_rc

		memc_regmod(DDRC0_06, 0x10, 3, 7);
		memc_regmod(DDRC0_06, 0x1A, 10, 15);
                memc_regmod(DDRC0_06, 0xE, 16, 21); //t_faw
                memc_regmod(DDRC0_06, 0x15, 27, 31); //wr2pre = wl+(bl/2)+ twr

                memc_regmod(DDRC0_07, 0x8, 0, 3); //t_rcd
                memc_regmod(DDRC0_07, 0x5, 4, 8); //rd2pre trtp+2 for bl8, rtp for bl4.
                memc_regmod(DDRC0_07, 0x15, 17, 21); //wr2rd = wl+twtr + bl/2.
                memc_regmod(DDRC0_07, 0x8, 22, 26); //rd2wr = rl +bl/2 + 2 -wl
                memc_regmod(DDRC0_07, 0x4, 26, 31); //wrlat

                memc_regmod(DDRC0_08, 0x9, 16, 19); //t_rp
                memc_regmod(DDRC0_08, 0x6, 24, 26); //t_rrd

		/* rddata_en */
                memc_regmod(DDRC0_19, 0x8, 27, 31); 

		/* rdc_we_to_re_delay */
		memc_regmod(PHY_2, 0x5, 3, 7);
		
		/* Rank 0 delay for all */
		memc_regmod(PHY_2, 0x1, 20, 20);

		/* Set RD ODT local value */
		memc_regmod(PHY_1, 0x0, 17, 18);
		memc_regmod(PHY_1, 0x1, 15, 16);

		/* rdc_we_to_re_delay */
		memc_regmod(PHY_1, 0x5, 4, 7);

		/* emr */
		mtsdram(DDRC0_11, 0x00000010);

		/* mr */
		mtsdram(DDRC0_12, 0x00000050);

		/* l2 optimization */
		memc_regmod(DDRC0_13, 0xc, 20, 23);

                memc_regmod(DDRC0_33, 0xf, 4, 11); 
#if defined(CONFIG_DDR_32BIT)
                memc_regmod(DDRC0_00, 0x1, 28, 29); 

		/* Adjust bank row and col bits */
                memc_regmod(DDRC0_15, 0x00252666, 0, 31); 
                memc_regmod(DDRC0_16, 0xffff0000, 0, 31); 
                memc_regmod(DDRC0_17, 0x0F222222, 0, 31); 
	
		/* Disable phy slice 4 - 7 */
                memc_regmod(PHY_1, 0xF, 24, 31); 
#endif
#else
		memc_regmod(DDRC0_05,0x25,16,23); //trfc
		memc_regmod(DDRC0_05,0x11,26,31); //t_rc

		memc_regmod(DDRC0_06, 0x0d, 3, 7);
		memc_regmod(DDRC0_06, 0x2d, 10, 15);
		memc_regmod(DDRC0_06, 0xb, 16, 21); //t_faw
		memc_regmod(DDRC0_06, 0xc8, 27, 31); //wr2pre = wl+(bl/2)+ twr

		memc_regmod(DDRC0_07,0x5,0,3); //t_rcd
		memc_regmod(DDRC0_07,0x5,4,8); //rd2pre trtp+2 for bl8, rtp for bl4.
		memc_regmod(DDRC0_07,0xa,17,21); //wr2rd = wl+twtr + bl/2.
		memc_regmod(DDRC0_07,0x8,22,26); //rd2wr = rl +bl/2 + 2 -wl
		memc_regmod(DDRC0_07,0x4,26,31); //wrlat

		memc_regmod(DDRC0_08,0x3,16,19); //t_rp
		memc_regmod(DDRC0_08,0x3,24,26); //t_rrd

		/* rddata_en */
		mtsdram(DDRC0_19, 0x00304009);

		/* pre_cke_x1024 */
		memc_regmod(DDRC0_10, 0x186, 10, 19);

		/* emr */
		mtsdram(DDRC0_11, 0x00000010);

		/* mr */
		mtsdram(DDRC0_12, 0x00000050);

		/* l2 optimization */
		memc_regmod(DDRC0_13, 0xc, 20, 23);

		memc_regmod(PHY_1, 0x3, 17, 18);
		memc_regmod(PHY_1, 0x5, 4, 7);

		/* rdc_we_to_re_delay */
		memc_regmod(PHY_2, 0xF, 3, 7);

		/* Manual wear leveling */
		board_calc_ratio();
#endif
#endif
#if defined(CONFIG_APM86XXX_DDR_AUTOCALIBRATION)
		
		/* 
		 * SW calibration of R/W 
		 */
		apm_sw_ddrcal();

#else
		/* 
		* Soft reset the DDR controller and low speed mode
		* Need to put in reset first?
		*/
#if defined(CONFIG_DDR_ECC)
		memc_mod(DDRC0_00, DDRC0_ECC_1BEAT);
		mtsdram(MEM_ECC_BYPASS, 0);
#else
		memc_mod(DDRC0_00, DDRC0_ECC_OFF);
		mtsdram(MEM_ECC_BYPASS, 0xffffffff);
#endif

		/* Reset and wait for done bit */	
		reset_ddr_controller();

#endif /* AUTO CAL */
#endif /* Not ISS */

#if defined(CONFIG_DDR_SPD)
		/*
		 * Derive the dimm parameter from the SPD information
		 */
		compute_dimm_info(&ddr_info);

		/*
		 * Calculate the DRAM size
		 */
		dram_size = 0;
		for (i = 0; i < CONFIG_SYS_NO_OF_DIMMS; i++) {
			dram_size += ddr_info.dimm_params[0][i].capacity;	
		}
#endif
                /* Program DDR size into Memory Queue to be use by AMP */
		mtsdram(MEMRANGE, ~(dram_size-1) >> 20);
		debug("DRAM: Set memory range to 0x%x\n", (u32)(~(dram_size-1) >> 20));
	}
#endif
	apm86xxx_ddr_regdump();
	return dram_size;
}
#else
/*----------------------------------------------------------------------------+
 * initdram.  Initializes the APM Memory Queue and DDR SDRAM controller.
 * Note: This routine runs from flash with a stack set up in the chip's
 * sram space.  It is important that the routine does not require .sbss, .bss or
 * .data sections.  It also cannot call routines that require these sections.
 * WARNING: THIS IS THE FIXED INIT ROUTINE
 *----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Function:	 initdram
 * Description:  Configures SDRAM memory banks for DDR operation.
 *		 Auto Memory Configuration option reads the DDR SDRAM EEPROMs
 *		 via the IIC bus and then configures the DDR SDRAM memory
 *		 banks appropriately. If Auto Memory Configuration is
 *		 not used, it is assumed that no DIMM is plugged
 *----------------------------------------------------------------------------*/
phys_size_t initdram_int(int board_type)
{
	phys_size_t dram_size = CONFIG_SYS_MBYTES_SDRAM << 20; 
	u32 memc_inited;
	u32 ddrspeed = CONFIG_SYS_DDR_FREQ / 1000000;
#if defined(CONFIG_DDR_SPD)
	u8 i;
	struct apm_ddr_info_s ddr_info;
#endif

	debug("\nEntering calibration initdram()\n");
#if defined(CONFIG_MEDIA_SPL) || !defined(CONFIG_MEDIA_U_BOOT)
	enable_ddr();
	apm86xxx_get_freq(APM86xxx_DDR, &ddrspeed);
	ddrspeed /= 500000;
#endif

	/* Check if the Memory Controller already initialized */
	mfsdram(MEMRANGE, memc_inited);

	if (!memc_inited) {
		/* Initialize drive control */
		if ((ddrspeed > 800) && (ddrspeed <= 1333))
			mtsdram(DDR_IO_CONTROL, 0x39000000);
		else
			mtsdram(DDR_IO_CONTROL, 0x5d800000);

		mtsdram(MEM_RAM_SHUTDOWN, 0);

		/* Soft reset PHY */
		memc_regmod(DDRC0_00, 0, 8, 8); 
		udelay(3);
		memc_regmod(DDRC0_00, 1, 8, 8);

		/* 00 Power Down, 0x1 Low Speed (<400Mhz), 0x2 HS (>400Mhz) */
		memc_regmod(DDRC0_00, 2, 6, 7);
		memc_regmod(DDRC0_00, 0, 9, 11);
	
		/* Activate banks: one bank for now */
		memc_regmod(DDRC0_00, 0x1, 12, 15);
		
		
#if defined(CONFIG_DDR_SPD)
		/*
		 * Derive the dimm parameter from the SPD information
		 */
		compute_dimm_info(&ddr_info);

		/*
		 * Convert SPD info to timing parameters
		 */
		ddr_compute_timing_params(&ddr_info);

		/*
		 * Program DIMM info into MEMC
		 */
		populate_memc_registers(&ddr_info);
#endif

#if !defined(CONFIG_DDR_ECC)
		/* Release soft reset, start initialization */
		mtsdram(MEM_ECC_BYPASS, 0xffffffff);
#else
		mtsdram(MEM_ECC_BYPASS, 0);

		/* Enable RMW optimization */
                memc_regmod(MEMQ_ARB,0x1,23,23);

		/* Clearing all pre-existing interrupts. */
                mtsdram(DDRC0_INTERRUPT,0xffff);

		/* Release soft reset, start initialization */
                memc_regmod(DDRC0_00, 0x4, 0, 3);

		/* Enable ECC Slice */
                memc_regmod(PHY_1,0x1,23,23);
#endif


#if defined(CONFIG_APM86XXX_DDR_AUTOCALIBRATION)
#if 0
		/* Take SCU DDR divider out of reset */
		apm86xxx_read_scu_reg(SCU_SOCDIV2_ADDR,  &val);
		val &= 0xFFFFE0FF;
		val |= 0x00001400;
		apm86xxx_write_scu_reg(SCU_SOCDIV2_ADDR, val);
#endif

		/* 
		 * SW calibration of R/W 
		 */
		apm_sw_ddrcal();

#else
#if defined(CONFIG_MERAKI)
                memc_mod(DDRC0_00, DDRC0_DQWDTH); 
		/* 64/32bit related */
                memc_mod(PHY_1, PHY1_DSLICE); 
		memc_mod(DDRC0_15, DDRC15_RADDR0);
		memc_mod(DDRC0_15, DDRC15_RADDR1);
		memc_mod(DDRC0_15, DDRC15_BADDR0);
		memc_mod(DDRC0_15, DDRC15_BADDR1);
		memc_mod(DDRC0_15, DDRC15_BADDR2);
                mtsdram(DDRC0_16, DDRC16_COLADDRS); 
                mtsdram(DDRC0_17, DDRC17_ROWADDRS); 
#endif

		/* Manual init values */
		board_calc_ratio();

		reset_ddr_controller();
#endif

#if defined(CONFIG_DDR_SPD)
		/*
		 * Calculate the DRAM size
		 */
		dram_size = 0;
		for (i = 0; i < CONFIG_SYS_NO_OF_DIMMS; i++) {
			dram_size += ddr_info.dimm_params[0][i].capacity;	
		}
#if defined(CONFIG_DDR_32BIT)
		dram_size /= 2;
#else
		/* Diamondback variant is 32-bit DDR */
		if (apm86xxx_is_32bit())
			dram_size /= 2;		
#endif
#endif
                /* Program DDR size into Memory Queue to be use by AMP */
		mtsdram(MEMRANGE, ~(dram_size-1) >> 20);
		debug("DRAM: Set memory range to 0x%x\n", (u32)(~(dram_size-1) >> 20));
	}

	apm86xxx_ddr_regdump();
	return dram_size;
}
#endif /* CONFIG_FIXED_INIT */

phys_size_t initdram(int board_type)
{
	return initdram_int(board_type);
}


void apm86xxx_ddr_regdump(void) {
#if defined(DEBUG)  
  u32 regval;
  u32 index = 0;
  
  /* just dump them all */
  for (index = 0x00; index < 0x678; index += 4) {
    mfsdram(index, regval);

    if ((index >= 0x544) && (index <= 0x5fc))
	  continue;

    if ((index >= 0x620) && (index <= 0x640))
	  continue;

    if ((index >= 0x74) && (index <= 0x8c))
	  continue;
    
    switch(index) {
      case 0:
      case 0x64:
      case 0xa0:
      case 0xa8:
      case 0xac:
      case 0x12c:
      case 0x39c:
      case 0x3a0:
      case 0x3a4:
      case 0x3a8:
      case 0x3ac:
      case 0x3f0:
      case 0x3f4:
      case 0x3fc:
      case 0x400:
      case 0x40c:
      case 0x414:
      case 0x41c:
      case 0x420:
      case 0x424:
      case 0x42c:
      case 0x434:
      case 0x438:
      case 0x43c:
      case 0x444:
      case 0x44c:
      case 0x450:
      case 0x454:
      case 0x464:
      case 0x484:
      case 0x488:
      case 0x48c:
      case 0x4a0:
      case 0x4a8:
      case 0x4b4:
      case 0x4b8:
      case 0x4bc:
      case 0x4c0:
      case 0x4c4:
      case 0x4c8:
      case 0x500:
      case 0x514:
      case 0x518:
      case 0x51c:
      case 0x520:
      case 0x658:
      case 0x65c:

	  break;
      default:
	  /* use this for basic debug (for logging -- scrolls screen heavily) */
	  debug("DDR Reg offset_%03x = 0x%08x\n", 0x800+index, regval);
	  /* use this to generate a BDI script base */
	  /* debug("WM32 0xEFFFF%03x 0x%08x\n", 0x800+index, regval); */
    }
  }
#else
  return;
#endif
}
#endif /* CONFIG_APM86XXX */
