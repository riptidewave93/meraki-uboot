/**
 * APM86xxx Eval Board Level Main File
 *
 * Copyright (c) 2010 Applied Micro Circuits Corporation.
 * All rights reserved. Feng Kan <fkan@apm.com>.
 *
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
 *
 */
#undef DEBUG
#include <common.h>
#include <apm86xxx.h>
#include <ppc4xx.h>
#include <serial.h>
#include <i2c.h>
#include <apm86xxx.h>
#include <asm/io.h>
#include <asm/mmu.h>
#include <asm/processor.h>
#include <asm/ppc4xx-isram.h>
#ifdef CONFIG_PCI
#include <pci.h>
#include <asm/apm_pcie.h>
#endif
#ifdef CONFIG_MP
#include <asm/mp.h>
#endif
#include <asm/apm_ipp_csr.h>
#include <asm/apm_scu.h>
#if defined(CONFIG_OF_LIBFDT) && defined(CONFIG_OF_BOARD_SETUP)
#include <libfdt.h>
#include <libfdt_env.h>
#include <fdt_support.h>
#endif


DECLARE_GLOBAL_DATA_PTR;

#if defined(CONFIG_SYS_SIMULATE_SPD_EEPROM)
/* This is the SPD for the MT9JSF12872AZ-1G4F1 */
const unsigned char fake_spd[]={0x92,0x10,0x0B,0x02,0x02,0x11,0x00,0x01,0x0B,0x52, /* 0 */
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
int compare_to_true(char *str);
char *remove_l_w_space(char *in_str);
char *remove_t_w_space(char *in_str);
int get_console_port(void);

int l2fam_set(int param);
extern void apm_eth_shutdown(void);

void *lmb_va(phys_addr_t addr)
{
	return (void *) (unsigned long) (addr - gd->ram_phybase);
}

phys_addr_t lmb_pa(void *addr)
{	
	phys_addr_t temp = (unsigned long) addr;
	temp += gd->ram_phybase;
	return temp;
}

/**
 * UBOOT initiated board specific function calls
 */
int board_early_init_f(void)
{
	unsigned int reg;
        struct apm86xxx_reset_ctrl reset_ctrl;

#ifdef CONFIG_MP
	if (CPUID != CONFIG_SYS_MASTER_CPUID)
		return 0;
#endif

#ifdef CONFIG_MP
	/* In case of Core Reset, it doesn't need to re-init AHB and MPIC.*/
	if ((mfspr(DBSR) & 0x30000000) == 0x10000000)
		return 0;	/* Reset status bits DBSR[2-3] */
#endif

#ifdef  CONFIG_HW_BRINGUP
	out_le32((void *)CONFIG_SYS_EBC_BASE + 0x11c, 0x0000FFFF);
	out_le32((void *)CONFIG_SYS_EBC_BASE + 0x100, 0x00000007);
	out_le32((void *)CONFIG_SYS_EBC_BASE + 0xe0, 0x00725010);
	out_le32((void *)CONFIG_SYS_EBC_BASE + 0xf0, 0x0000037e);
	out_le32((void *)CONFIG_SYS_EBC_BASE + 0xd0, 0x00000040);
#endif

	/* Setup AHB ENABLE_INTF APB clock freq ratio*/
	reg = in32(CONFIG_SYS_CSR_BASE + 0x62090);
	reg &= 0xCFFFFFFF;
	reg |= 0x30000000;
	out32(CONFIG_SYS_CSR_BASE + 0x62090, reg);

	/* Scale APB clk divisor down from 100Mhz */
	do {
		u32 div, freq;
		apm86xxx_get_freq(APM86xxx_APB, &freq);
		if (freq >= 100000000)
		{
			div = apm86xxx_get_divisor(APM86xxx_APB);
			div++;
			if (div >= 0x1f)
				break;
			if (div & 1)
				div ++;
			reg = in32(CONFIG_SYS_CSR_BASE + 0xb00c0);
			reg &= 0xFFE0E0FF;
			reg |= ((div / 2) << 16) | (div << 8);
			out32(CONFIG_SYS_CSR_BASE + 0xb00c0, reg);
		} else
			break;
	} while (reg >= 100000000);

        /* Enable MPIC Block */
        memset(&reset_ctrl, 0, sizeof(reset_ctrl));
        reset_ctrl.reg_group = REG_GROUP_SRST_CLKEN;
        reset_ctrl.clken_mask = 0;
        reset_ctrl.csr_reset_mask = MPIC_F2_MASK;
        reset_ctrl.reset_mask = MPIC_MASK;
        reset_apm86xxx_block(&reset_ctrl);

	return 0;
}

#if !defined(CONFIG_MEDIA_SPL) && !defined(CONFIG_NAND_SPL)
int is_coherent(void)
{
	int i;
	u32 tlb_word0_value;
        u32 tlb_word2_value;
        u32 tlb_vaddr;

#ifdef CONFIG_NOT_COHERENT_CACHE
	return 0;
#endif

	/* Lets check DDR tlb to see if it M enabled */
        for (i=0; i<PPC4XX_TLB_SIZE; i++) {
                tlb_word0_value = mftlb1(i);
                tlb_word2_value = mftlb3(i);
                tlb_vaddr = TLB_WORD0_EPN_DECODE(tlb_word0_value);
                if (((tlb_word0_value & TLB_WORD0_V_MASK) == TLB_WORD0_V_ENABLE) &&
                    (tlb_vaddr == 0)) {
			if (tlb_word2_value & SA_M)
				return 1;
			else
				return 0;
		}
	}
	return 0;
}
#endif

#if !defined(CONFIG_MEDIA_SPL) && !defined(CONFIG_NAND_SPL) && \
	!defined(CONFIG_MEDIA_U_BOOT) && !defined(CONFIG_NAND_U_BOOT)

int misc_init_f(void)
{
	char *s;

	/*
	 * Initialize FAM configuration
	 */
	s = getenv("l2config");
	if (s != NULL)
	{
		switch (simple_strtoul(s, NULL, 10)) {
		case 1:
			l2fam_set(1);
			printf("WARNING: ECC off. Running with half L2 cache could impact performance\n");
			break;
		case 0: 
			l2fam_set(2);
			printf("WARNING: ECC off. Running with no L2 cache could result in incorrect behavior\n");
			break;
		}
                program_tlb(0x400000000ULL, 0x40000000, 0x40000, FAR|SA_I|SA_G|AC_R|AC_W|AC_X);
                program_tlb(0x400040000ULL, 0x40040000, 0x40000, SA_I|SA_G|AC_R|AC_W|AC_X);
	}

	return 0;
}
#endif

int board_early_init_r(void)
{
#ifdef DEBUG
	u32 reg;

	reg = (mfspr(SPRN_DBSR) >> 28) & 0x3;
	debug("Last reset was: ");
	if (reg == 0x3)
		debug("system\n");
	else if (reg == 0x2)
		debug("chip\n");
	else if (reg == 0x1)
		debug("core\n");
	else
		debug("external\n");
#endif

	/* Setup Mailbox TLB entries */
#if 0
	if (!is_coherent()) {
		program_tlb(0x3ffffd000ULL, CONFIG_SYS_MB_DDRC_BASE + 0x1000, 0x1000, SA_I|SA_G|AC_R|AC_W|AC_X);
		program_tlb(0x3ffffe000ULL, CONFIG_SYS_MB_DDRC_BASE + 0x2000, 0x1000, SA_I|SA_G|AC_R|AC_W|AC_X);
	} else {
		program_tlb(0x3ffffd000ULL, CONFIG_SYS_MB_DDRC_BASE + 0x1000, 0x1000, SA_WL1|SA_M|AC_R|AC_W|AC_X);
		program_tlb(0x3ffffe000ULL, CONFIG_SYS_MB_DDRC_BASE + 0x2000, 0x1000, SA_WL1|SA_M|AC_R|AC_W|AC_X);
	}
#else
		/* mailboxes non cacheable */
		program_tlb(0x3ffffd000ULL, CONFIG_SYS_MB_DDRC_BASE + 0x1000, 0x1000, SA_I|SA_G|AC_R|AC_W|AC_X);
		program_tlb(0x3ffffe000ULL, CONFIG_SYS_MB_DDRC_BASE + 0x2000, 0x1000, SA_I|SA_G|AC_R|AC_W|AC_X);
#endif

#ifdef CONFIG_MP
	gd->shmem = NULL;
#endif

#if defined(CONFIG_HW_BRINGUP) || defined(CONFIG_ISS)

	/* Remove TLB entry of boot EBC mapping */
        remove_tlb(CONFIG_SYS_BOOT_BASE_ADDR, 16 << 20);

	/* Add TLB entry for 0xfc00.0000 -> 0xf.fc00.0000 */
	program_tlb(0xFFC000000ULL, CONFIG_SYS_FLASH_BASE,
			CONFIG_SYS_FLASH_SIZE, TLB_WORD2_I_ENABLE|SA_G);
#endif

	return 0;
}

int checkboard(void)
{
	char *s = getenv("serial#");

#if defined(CONFIG_MAMBA_WAP)
	printf("Board: MAMBA WAP - AppliedMicro APM86xxx WAP Reference Board");
#else
	printf("Board: Serengeti - AppliedMicro APM86xxx Reference Board");
#endif
	if (s != NULL) {
		puts(", serial# ");
		puts(s);
	}
	putc('\n');

	return 0;
}

#if defined(CONFIG_OF_LIBFDT) && defined(CONFIG_OF_BOARD_SETUP)
#ifdef CONFIG_PCI
void fdt_apm_pcie_setup(void *blob)
{
	char s[10], *tk;
	char *pcie_mode;
	char pcieport[10];
	u32 port=0;
	char portname[3][40] = { "/hbf/pciex@500000000","/hbf/pciex@700000000","/hbf/pciex@A00000000" };

        /* Setup PCIE nodes */
	pcie_mode = getenv("pcie_mode");

	if (pcie_mode == NULL)
		return;

	strcpy(s, pcie_mode);
	tk = strtok(s, ":");

	memset(pcieport, 0, 10);
	
	for (port = 0; port < CONFIG_PCIE_MAX_PORTS; port++)
	{
		sprintf(pcieport, "pcie%d", port);

		if (port) 
			tk = strtok(NULL, ":");
		
		/* Del port is no token defined */
		if (tk == NULL) {
			fdt_del_node(blob,
				fdt_path_offset(blob, portname[port]));
			continue;
		}	
		
#ifdef CONFIG_MP
		/* Check if it belong to the right core */
		if ((getenv("AMP") != NULL) &&
			(CPUID != simple_strtol(getenv(pcieport), NULL, 10))) {
			fdt_del_node(blob,
				fdt_path_offset(blob, portname[port]));
			continue;
		}
#endif

		/* Configure RC/EP or remove */
		if (!strcmp(tk, "ep") || !strcmp(tk, "EP")) {
			fdt_find_and_setprop(blob, portname[port],
			"device_type", "pci-endpoint", sizeof("pci-endpoint"), 1);
		} else if (!strcmp(tk, "rc") || !strcmp(tk, "RC")) {
			fdt_find_and_setprop(blob, portname[port],
			"device_type", "pci", sizeof("pci"), 1);
		} else if (!strcmp(tk, "na") || !strcmp(tk, "NA")) {
			fdt_del_node(blob,
				fdt_path_offset(blob, portname[port]));
		} 

        }
}
#endif /* CONFIG_PCI */

int fdt_apm_fixup_memory(void *blob)
{
	phys_addr_t phy;
	u32 val[3];
	char *s;
	int err;
	u32 iocm_size;
	
	err = fdt_check_header(blob);
	if (err < 0) {
		printf("%s: %s\n", __FUNCTION__, fdt_strerror(err));
		return err;
	}
	
	/* Read the DDR size from Memory Queue controller */
	phy   = (~in32(CONFIG_SYS_DDRC_BASE + 0x00000004) + 1) & 0xFFFF;
	phy <<= 20;
	
	/* Setup Shared Memory if any */
	if (gd->shmem_size) {
		phy  -= gd->shmem_size;
		val[0] = cpu_to_fdt32((u32) (phy >> 32));
		val[1] = cpu_to_fdt32((u32) phy);
		val[2] = cpu_to_fdt32(gd->shmem_size);
		do_fixup_by_compat(blob, "apm,apm86xxx-shm", "reg", 
				val, sizeof(val), 0);
	}
	
	/* Setup IO coherent memory for security in UP/AMP IO coherent mode */
	s = getenv("iocmemory");
	if (s == NULL) {
		iocm_size = 0;
	} else {
		iocm_size = simple_strtoul(s, NULL, 10);
	}
	if (s && iocm_size < 16*1024*1024) {
		printf("\nWARNING: environment iocmemory size invalid");
	}
	iocm_size &= ~(4*1024);	/* Must be multiple of 4K */
	
	if (iocm_size > 0) {
		int off;
				
		phy -= iocm_size;
		phy += (iocm_size / 2) * CPUID;	
		iocm_size /= 2;			/* Half per CPU */
		iocm_size /= 2;			/* Half to security */		
		val[0] = cpu_to_fdt32((u32) (phy >> 32));
		val[1] = cpu_to_fdt32((u32) phy);
		val[2] = cpu_to_fdt32(iocm_size);
		do_fixup_by_compat(blob, "apm,apm86xxx-crypto", "dma-reg", 
				val, sizeof(val), 0);
	
		/* Fix up USB EHCI */
		phy += iocm_size;
		iocm_size /= 4;		/* Divide remaining for each USB */
		val[0] = cpu_to_fdt32((u32) (phy >> 32));
		val[1] = cpu_to_fdt32((u32) phy);
		val[2] = cpu_to_fdt32(iocm_size);
		
		off = fdt_node_offset_by_compatible(blob, -1, 
						"apm,usb-ehci-460ex");
		while (off != -FDT_ERR_NOTFOUND) {
			if (fdt_get_property(blob, off, "dma-reg", 0) != NULL) {
				fdt_setprop(blob, off, "dma-reg", val, sizeof(val));
				phy += iocm_size;
				val[0] = cpu_to_fdt32((u32) (phy >> 32));
				val[1] = cpu_to_fdt32((u32) phy);
			}
			off = fdt_node_offset_by_compatible(blob, off, "apm,usb-ehci-460ex");
		}
		
		/* Fix up USB OHCI */
		off = fdt_node_offset_by_compatible(blob, -1, 
						"apm,usb-ohci-460ex");
		while (off != -FDT_ERR_NOTFOUND) {
			if (fdt_get_property(blob, off, "dma-reg", 0) != NULL) {
				fdt_setprop(blob, off, "dma-reg", val, sizeof(val));
				phy += iocm_size;
				val[0] = cpu_to_fdt32((u32) (phy >> 32));
				val[1] = cpu_to_fdt32((u32) phy);
			}
			off = fdt_node_offset_by_compatible(blob, off, "apm,usb-ohci-460ex");
		}
	}
	return 0;
}

void ft_board_setup(void *blob, bd_t *bd)
{
	ft_cpu_setup(blob, bd);

	/* Set the physical boot CPU ID. This is requires for AMP. On SMP,
	   this will replace with the same value */
#ifdef CONFIG_MP
	fdt_set_boot_cpuid_phys(blob, CPUID);
#endif

#ifdef CONFIG_PCI
	fdt_apm_pcie_setup(blob);
#endif
#ifdef CONFIG_APM86xxx_ENET
	if (!is_coherent())
		apm_eth_shutdown();
#endif
		
	fdt_apm_fixup_memory(blob);
}
#endif /* defined(CONFIG_OF_LIBFDT) && defined(CONFIG_OF_BOARD_SETUP) */

/*
 * This is called early in the system initialization to grab memory
 * for the video controller.
 * Returns new address for monitor, after reserving video buffer memory
 *
 * Note that this is running from ROM, so no write access to global data.
 */
#ifdef CONFIG_VIDEO
ulong video_setmem(ulong addr)
{
        /* Allocate pages for the frame buffer. */
        addr -= VIDEO_COLS_MAX * VIDEO_ROWS_MAX * VIDEO_PIXEL_SIZE_MAX;  

        debug ("Reserving %dk for Video Framebuffer at: %08lx\n",
                (VIDEO_COLS_MAX * VIDEO_ROWS_MAX * VIDEO_PIXEL_SIZE_MAX) >> 10,
                addr);

        return addr;
}
#endif

int l2fam_set(int param)
{
	u32 val;
		
	/* Program the FAM Address for this CPU */
	mtdcr(L2_CACHE_ADDR, L2_CACHE_L2FAMAR);
	/* NOTE: This create a hole between the two non-shared L2
	         The first CPU will start at FAM address 0. The second
	         CPU will start at FAM address 0 + 256K. 
	 */ 
	mtdcr(L2_CACHE_DATA, 0x40000000 + ((mfspr(SPRN_PIR) * 256*1024) >> 4));	
	
	/* Program the FAM size */	
	mtdcr(L2_CACHE_ADDR, L2_CACHE_L2CR1);
	val = mfdcr(L2_CACHE_DATA);
	val &= ~0xC0000000;
	val |= (param & 0x3) << 30;
	val |= 0x20000000;		/* Make it public */
	mtdcr(L2_CACHE_DATA, val);
	if (param == 0)
		return 0;
	mtdcr(L2_CACHE_ADDR, L2_CACHE_L2CR0);
	val = mfdcr(L2_CACHE_DATA) | 0xc;
	mtdcr(L2_CACHE_DATA, val);
	return 0;
}

/* Currently support for dynamic FAM config is disabled */
#if 0 
static int is_on_off(const char *s)
{
	if (strcmp(s, "on") == 0) {
		return  1;
	} else if (strcmp(s, "off") == 0) {
		return  0;
	}
	return  -1;
}

static void l2cache_disable(void)
{
	u32 val;
	
	mtdcr(L2_CACHE_ADDR, L2_CACHE_L2CR0);
	val = mfdcr(L2_CACHE_DATA);
	val &= ~0x30000000;
	mtdcr(L2_CACHE_DATA, val);
}

static void l2cache_enable(void)
{
	u32 val;
	
	mtdcr(L2_CACHE_ADDR, L2_CACHE_L2CR0);
	val = mfdcr(L2_CACHE_DATA);

	/*
	 * Check if the L2 is enabled 
         * If already enabled, exit
         */
	if (val & 0x30000000)
		return;

	mtdcr(L2_CACHE_ADDR, L2_CACHE_L2CR2);
	val = mfdcr(L2_CACHE_DATA);
	val |= 0x80000000;		/* Enable snooping master */
	val &= ~0x10000000;		/* Select MESI */
	val |= 0x00008000;		/* Enable L1 coherency */
	mtdcr(L2_CACHE_DATA, val);
	__asm__ volatile ("sync");	/* msync */
	mtdcr(L2_CACHE_ADDR, L2_CACHE_L2CR0);
	val = mfdcr(L2_CACHE_DATA);
	val &= ~0x30000000;
	val |= 0x10000000;		/* Select 256K cache size */
	mtdcr(L2_CACHE_DATA, val);
	__asm__ volatile ("sync");
}

static int l2cache_status(void)
{
	u32 val;
	
	mtdcr(L2_CACHE_ADDR, L2_CACHE_L2CR0);
	val = mfdcr(L2_CACHE_DATA);
	return val & 0x30000000 ? 1 : 0;
}

int do_l2cache(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	switch (argc) {
	case 2:			/* on / off	*/
		switch (is_on_off(argv[1])) {
		case 0:	l2cache_disable();
			break;
		case 1:	l2cache_enable();
			break;
		}
		/* FALL TROUGH */
	case 1:			/* get status */
		printf ("L2 Cache is %s\n",
			l2cache_status() ? "ON" : "OFF");
		return 0;
	default:
		cmd_usage(cmdtp);
		return 1;
	}
	return  0;
}

static int do_l2fam(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	u32 famsize;
	
	switch (argc) {
	case 2:
	case 1:
	case 0:
		famsize = l2fam_set(argc);
		if (famsize == 0)
			printf("L2 FAM is OFF\n");
		else
			printf("L2 FAM is %dKB\n", famsize/1024);
		break;
	default:
		cmd_usage(cmdtp);
		return 1;
	}
	return  0;
}

U_BOOT_CMD(
	l2cache,   2,   1,     do_l2cache,
	"enable or disable L2 cache",
	"[on, off]\n"
	"    - enable or disable L2 cache"
);

U_BOOT_CMD(
	l2fam,   2,   1,     do_l2fam,
	"Select FAM size [0,1,2]",
	"0 - FAM is off\n"
	"1 - FAM is half\n"
	"2 - FAM is all of L2 cache"
);
#endif

/**
 *  pci_pre_init
 *
 *  This routine is called just prior to registering the hose and gives
 *  the board the opportunity to check things. Returning a value of zero
 *  indicates that things are bad & PCI initialization should be aborted.
 *
 *      Different boards may wish to customize the pci controller structure
 *      (add regions, override default access routines, etc) or perform
 *      certain pre-initialization actions.
 *
 */
#if defined(CONFIG_PCI)
int pci_pre_init(struct pci_controller * hose)
{
        return 1;
}
#endif

/**
 *  pci_target_init
 *
 *      The bootstrap configuration provides default settings for the pci
 *      inbound map (PIM). But the bootstrap config choices are limited and
 *      may not be sufficient for a given board.
 *
 */
#if defined(CONFIG_PCI) && defined(CONFIG_SYS_PCI_TARGET_INIT)
void pci_target_init(struct pci_controller * hose)
{
        u32 val;

        /* Program the board's subsystem id/vendor id */
        val = (((u32) CONFIG_SYS_PCI_SUBSYS_VENDORID) << 16) |
                CONFIG_SYS_PCI_SUBSYS_DEVICEID;
        pcie_csr_out32((u32) hose->cfg_addr, val);
}
#endif

#if defined(CONFIG_PCI) && defined(CONFIG_SYS_PCI_MASTER_INIT)
void pci_master_init(struct pci_controller *hose)
{
}
#endif /* defined(CONFIG_PCI) && defined(CONFIG_SYS_PCI_MASTER_INIT) */

#if defined(CONFIG_PCI) 
int pci_skip_dev(struct pci_controller *hose, pci_dev_t dev)
{
	/*
	 * Check if pci device should be skipped in configuration
	 */
	if (dev == PCI_BDF(hose->first_busno, 0, 0)) {
#if defined(CONFIG_PCI_CONFIG_HOST_BRIDGE) /* don't skip host bridge */
		/*
		 * Only skip configuration if "pciconfighost" is not set
		 */
		if (getenv("pciconfighost") == NULL)
			return 1;
		return 0;
#else
		return 1;
#endif
	} 
	return 1;
}
#endif

#if defined(CONFIG_4xx_DCACHE)
# define MY_TLB_WORD2_I_ENABLE TLB_WORD2_M_ENABLE	/* enable caching on SDRAM */
#else
# define MY_TLB_WORD2_I_ENABLE TLB_WORD2_I_ENABLE 	/* disable caching on SDRAM */
#endif


/**
 * Memory Layout
 * UP:      
 *    -------------------------------
 *    | IO Coherent Memory - unused |  8MB (set able)
 *    -------------------------------
 *    |    IO Coherent Memory       |  8MB (set able)
 *    -------------------------------
 *    |      OS Memory              |  size - 16MB
 *    -------------------------------
 *
 * SMP:      
 *    -------------------------
 *    |    OS Memory          |  size
 *    -------------------------
 *
 * AMP:      
 *    -------------------------------
 *    |    Shared Memory            |  16MB (set able)
 *    -------------------------------
 *    | IO Coherent Memory - CPU1   |  8MB (set able)
 *    -------------------------------
 *    | IO Coherent Memory - CPU0   |  8MB (set able)
 *    ------------------------------- 
 *    |      OS #2 Memory           |  size/2 - 16MB - 16MB
 *    ------------------------------- 
 *    |      OS #1 Memory           |  size/2
 *    ------------------------------- 
 */ 
void board_add_ram_info(int use_default)
{
	char *s;
	sys_info_t bd_info;
	phys_size_t dram_size = gd->ram_size; 
#ifdef CONFIG_MP
	u32  cpu_nr;
	phys_size_t dram_size_percore;
	u32  sm_size;
	u32  iocm_size;
#endif	

#if defined(CONFIG_HW_BRINGUP) && !defined(CONFIG_HW_DDR)
	return;
#endif

	get_sys_info(&bd_info);
	printf(" at %ldMHz", bd_info.freqDDR/1000000*2);	
	
	/* To support AMP, we reserve share memory for IPC */
        s = getenv ("AMP");
        if (s == NULL) {
#if !defined(CONFIG_MEDIA_U_BOOT) && !defined(CONFIG_NAND_U_BOOT)
#if defined(CONFIG_PAL) || defined(CONFIG_NOT_COHERENT_CACHE)
                program_tlb(0, CONFIG_SYS_SDRAM_BASE, dram_size, SA_I|SA_G|AC_R|AC_W|AC_X);
#else
#ifdef CONFIG_MP
		program_tlb(0, CONFIG_SYS_SDRAM_BASE, dram_size, SA_WL1|SA_M|AC_R|AC_W|AC_X);
#else
                program_tlb(0, CONFIG_SYS_SDRAM_BASE, dram_size, AC_R|AC_W|AC_X);
#endif
#endif
#endif
		return;
        }

#ifdef CONFIG_MP
	/* AMP code done here */

	/* Read the DDR size for non-master core */
	dram_size   = (~in32(CONFIG_SYS_DDRC_BASE + 0x00000004) + 1) & 0xFFFF;
	dram_size <<= 20;

	debug("\nDDR size is %llx", dram_size);

	/* Read total number of CPU's */
        cpu_nr = ((in_be32((void *) (CONFIG_SYS_MPIC_BASE + 0x1000)) >> 8)
                        & 0x1F) + 1;

        dram_size_percore = dram_size / cpu_nr;
        debug("\nAMP%d/%d: Physical Size 0x%08X DDR %llx Per Core %llx",
                CPUID+1, cpu_nr, (u32) (CPUID * dram_size_percore),
                dram_size, dram_size_percore);

	/* Reserve share memory size. Top of 4K is used for system resource */
	s = getenv("sharememory");
	if (s == NULL) {
		printf("WARNING: No sharememory ENV definied!!!");
		sm_size = 0;
	} else {
		sm_size = simple_strtoul(s, NULL, 10);
	}
	if (s && sm_size < 4*1024) {
		printf("\nWARNING: environment sharememory size invalid");
	}
	sm_size &= ~(4*1024);	/* Must be multiple of 4K */
	
	/* Reserve IO coherent memory */
	s = getenv("iocmemory");
	if (s == NULL) {
		iocm_size = 0;
	} else {
		iocm_size = simple_strtoul(s, NULL, 10);
	}
	if (s && iocm_size < 16*1024*1024) {
		printf("\nWARNING: environment iocmemory size invalid");
	}
	iocm_size &= ~(4*1024);	/* Must be multiple of 4K */
	
	if (CPUID == cpu_nr-1) {
		gd->ram_size = dram_size_percore - sm_size - iocm_size;
		/* Make size in 256MB chunk to avoid fragmented TLB */
		gd->ram_size /= 256*1024*1024;
		gd->ram_size *= 256*1024*1024;
	} else {
		gd->ram_size = dram_size_percore;
	}

	/* Put in ShMem TLB entry */
	if (sm_size != 0) {
		debug("\nProgramming shared memory entry");
		program_tlb(dram_size - sm_size, CONFIG_SYS_SHAREDMEM_BASE, 
			sm_size, SA_WL1|SA_M|AC_R|AC_W|AC_X);
		gd->shmem = (void *) CONFIG_SYS_SHAREDMEM_BASE;   

		/* Init spin lock */
		if (CPUID == CONFIG_SYS_MASTER_CPUID) 
			 *((u32 *) ((u8 *) gd->shmem)) = 0; 
	} else {
		gd->shmem = NULL;   
	}
	/* Put in main memory TLB for AMP */
        debug("\nAMP%d/%d: Physical Addr 0x%08X DDR %llx Per Core %llx",
                CPUID+1, cpu_nr, (u32) (CPUID * dram_size_percore),
                dram_size, dram_size_percore);
#if defined(CONFIG_PAL) || defined(CONFIG_NOT_COHERENT_CACHE)
	program_tlb(CPUID * dram_size_percore, CONFIG_SYS_SDRAM_BASE,
			gd->ram_size, AC_R|AC_W|AC_X|SA_I|SA_G);
#else
	program_tlb(CPUID * dram_size_percore, CONFIG_SYS_SDRAM_BASE,
		  	gd->ram_size, AC_R|AC_W|AC_X|SA_I|SA_G);
#endif

	gd->shmem_size = sm_size;
	
	/* Update physical start address for this core */
        gd->ram_phybase = CPUID * dram_size_percore;

	printf("\nAMP MODE: CPU %d RAM: ", CPUID);
	print_size (gd->ram_size, "");	
				
	printf("\nReserving top %dKB for shared memory: 0x%p " 
		"(PHY 0x%02X.%08X)", gd->shmem_size / 1024, gd->shmem,
		(u32) ((dram_size - gd->shmem_size) >> 32),
		(u32) (dram_size - gd->shmem_size));
	if (iocm_size) {
		phys_addr_t paddr;
		paddr = dram_size - gd->shmem_size - iocm_size +
				((iocm_size / 2) * CPUID);
		printf("\nReserving %dKB for io coherent memory " 
			"(PHY 0x%02X.%08X)", iocm_size / 2 / 1024, 
			(u32) (paddr >> 32), (u32) paddr);
	}
#endif
}

#ifdef CONFIG_MP
/* Override board_cpu_reset to set PPC1 release reset flag */
void board_cpu_reset(int nr)
{
	char *s;

	if (CPUID != CONFIG_SYS_MASTER_CPUID)
		return;
	s = getenv ("AMP");
	if (s && simple_strtoul(s, NULL, 10)) {
		/* For AMP mode, set CONFIG_SYS_PPC_RESET_CTRL_ADDR reg 
		   bit for AMP */
		volatile u32 *ppc1_resume = (volatile u32 *)
						(CONFIG_SYS_PPC_RESET_CTRL_ADDR);
		*ppc1_resume |= CONFIG_SYS_PPC1_RESET_RELEASE_MASK;
	}
}

/* Override board_cpu_release to set PPC1 release reset flag */
void board_cpu_release(int nr)
{
	board_cpu_reset(nr);
}
#endif

struct serial_device *default_serial_console(void)
{
#ifdef CONFIG_MP
        if (CPUID == 1) 
                return &serial1_device;
	else
#endif
                return &serial0_device;
}

void board_reset(void)
{
#ifdef CONFIG_MP
	if (getenv("AMP") != NULL) {
		printf("Resetting Core ...\n");
		mtspr(SPRN_DBCR0, 0x10000000);	
	} else {
		printf("Resetting System ...\n");
		mtspr(SPRN_DBCR0, 0x30000000);	
	}
#else
	printf("Resetting System ...\n");
	mtspr(SPRN_DBCR0, 0x30000000);	
#endif
}

int do_resetsys (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	printf("Resetting System ...\n");
	mtspr(SPRN_DBCR0, 0x30000000);	
	return 1;
}

U_BOOT_CMD(
        resetsys, 1, 0, do_resetsys,
        "Perform System RESET of the CPU",
        ""
);
