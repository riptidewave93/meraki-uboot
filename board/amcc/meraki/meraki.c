/**
 * Meraki - Senao Cloud Managed Router board
 *
 * Based on APM86xxx Eval Board 
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
#include <asm/apm_ipp_csr.h>
#include <asm/apm_scu.h>
#if defined(CONFIG_OF_LIBFDT) && defined(CONFIG_OF_BOARD_SETUP)
#include <libfdt.h>
#include <libfdt_env.h>
#include <fdt_support.h>
#endif


DECLARE_GLOBAL_DATA_PTR;

/* This is the SPD for the MT8JTF25664AZ-1G4D1 */ 
const unsigned char fake_spd[]={0x92,0x10,0x0B,0x02,0x03,0x19,0x00,0x01,0x03,0x52, /* 0 */    
				0x01,0x08,0x0C,0x00,0x7E,0x00,0x69,0x78,0x69,0x30, /* 10 */   
				0x69,0x11,0x20,0x89,0x00,0x05,0x3C,0x3C,0x00,0xF0, /* 20 */   
				0x82,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 30 */   
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 40 */   
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 50 */   
				0x0F,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 60 */   
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 70 */   
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 80 */   
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 90 */   
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 100 */  
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x2C,0x00, /* 110 */  
				0x00,0x00,0x00,0x00,0x00,0x00,0x8B,0xB7,0x38,0x4a, /* 120 */  
				0x54,0x46,0x32,0x35,0x36,0x36,0x34,0x41,0x20,0x5a, /* 130 */  
				0x2d,0x31,0x47,0x34,0x44,0x31,0x44,0x31,0x80,0x2C, /* 140 */  
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 150 */  
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 160 */  
				0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF, /* 170 */  
				0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, /* 180 */  
				0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, /* 190 */  
				0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, /* 200 */  
				0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, /* 210 */  
				0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, /* 220 */  
				0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, /* 230 */  
				0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, /* 240 */  
				0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}; /* 250 */

int compare_to_true(char *str);
char *remove_l_w_space(char *in_str);
char *remove_t_w_space(char *in_str);
int get_console_port(void);
void init_switch_gpio(void);

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
	reg |= 0x30000000;	//0x20000000; //
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


#if defined(CONFIG_HW_BRINGUP) || defined(CONFIG_ISS)

	/* Remove TLB entry of boot EBC mapping */
        remove_tlb(CONFIG_SYS_BOOT_BASE_ADDR, 16 << 20);

	/* Add TLB entry for 0xfc00.0000 -> 0xf.fc00.0000 */
	program_tlb(0xFFC000000ULL, CONFIG_SYS_FLASH_BASE,
			CONFIG_SYS_FLASH_SIZE, TLB_WORD2_I_ENABLE|SA_G);
#endif

	init_switch_gpio();

	return 0;
}

int checkboard(void)
{
	char *s = getenv("serial#");

	printf("Board: Fullerene-2 - Meraki Fullerene Cloud Managed Router");
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
	
#if defined(CONFIG_4xx) && defined(CONFIG_MP)
	/* Setup Shared Memory if any */
	if (gd->shmem_size) {
		phy  -= gd->shmem_size;
		val[0] = cpu_to_fdt32((u32) (phy >> 32));
		val[1] = cpu_to_fdt32((u32) phy);
		val[2] = cpu_to_fdt32(gd->shmem_size);
		do_fixup_by_compat(blob, "apm,apm86xxx-shm", "reg", 
				val, sizeof(val), 0);
	}
#endif
	
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
		phy += (iocm_size / 2) * 0/*CPUID*/;	
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
#if !defined(CONFIG_DDR_U_BOOT)
	phys_size_t dram_size = gd->ram_size; 
#endif

#if defined(CONFIG_HW_BRINGUP) && !defined(CONFIG_HW_DDR)
	return;
#endif

	get_sys_info(&bd_info);
	printf(" at %ldMHz", bd_info.freqDDR/1000000*2);	
	
#if defined(CONFIG_DDR_U_BOOT)
	return;
#endif

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
        //program_tlb(0, CONFIG_SYS_SDRAM_BASE, dram_size, AC_R|AC_W|AC_X);
        program_tlb(0, CONFIG_SYS_SDRAM_BASE, dram_size, AC_R|AC_W|AC_X|SA_G|SA_I);
#endif /* CONFIG_MP */
#endif /* defined(CONFIG_PAL) || defined(CONFIG_NOT_COHERENT_CACHE) */
#endif /* !defined(CONFIG_MEDIA_U_BOOT) && !defined(CONFIG_NAND_U_BOOT) */
		return;
	}
}

struct serial_device *default_serial_console(void)
{
                return &serial0_device;
}

void board_reset(void)
{
	printf("Resetting System ...\n");
	mtspr(SPRN_DBCR0, 0x30000000);	
}

#if 0
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
#endif

/* GPIO controller */
#if defined(CONFIG_APM86XXX_GPIO)
//#define DEBUG_GPIO
#ifdef DEBUG_GPIO
#define gpio_dbg(format, arg...) do {printf("GPIO: " format , ## arg); } while(0)
#else
#define gpio_dbg(fmt,args...)
#endif

typedef enum gpio_select {		//GPIO-FL					//GPIO-DS
	GPIO_ALT0,	/* 000 */		//0: all pins are EBUS		//0: GPIO-DS
	GPIO_ALT1,	/* 001 */		//1: all pins are GPIO-FL	//1: All pin0-4 Trace, IRQ
	GPIO_ALT2	/* 010 */		//2: SDIO pin (???)			//2: Pin strap
} gpio_select_t;
typedef enum gpio_type_select	{ GPIO_DS, GPIO_FL, GPIO_LED } gpio_type_select_t;
typedef enum gpio_driver		{ GPIO_DIS, GPIO_IN, GPIO_OUT, GPIO_BI } gpio_driver_t;
typedef enum gpio_out			{ GPIO_OUT_0, GPIO_OUT_1, GPIO_OUT_NO_CHG } gpio_out_t;

#define CONFIG_SYS_SLIMPRO_CSR_BASE CONFIG_SYS_SCU_BASE		/*CONFIG_SYS_CSR_BASE + 0xB0000*/
#define MPA_GPIO_BASE_ADDR			CONFIG_SYS_SLIMPRO_CSR_BASE

#define MPA_GPIO					(MPA_GPIO_BASE_ADDR | 0x00000A94)		
#define MPA_GPIO_IN					(MPA_GPIO_BASE_ADDR | 0x00000A98)		
#define MPA_GPIO_DS_IOCTL0			(MPA_GPIO_BASE_ADDR | 0x00000B70)		
#define MPA_GPIO_DS_IOCTL1			(MPA_GPIO_BASE_ADDR | 0x00000B74)		
#define MPA_GPIO_DS_IOCTL2			(MPA_GPIO_BASE_ADDR | 0x00000B78)		
#define MPA_GPIO_DS_IOCTL3			(MPA_GPIO_BASE_ADDR | 0x00000B7C)		
#define MPA_DIAG					(MPA_GPIO_BASE_ADDR | 0x00000B20)	
	#define MPA_DIAG_IRQ_SEL_MASK		0xFF800000
	#define MPA_DIAG_GPIO_SEL_MASK		0x00000300

#define GPIO_DS_OUT_VAL(gpio)		(0x0 << (gpio*8+1))			//direction for GPIO-LED
#define GPIO_DS_IN_VAL(gpio)		(0x0 << (gpio*8+1))			//no pull-up, no smitt trigger
#define GPIO_DS_DIS_VAL(gpio)		(0x1 << (gpio*8+1))	
#define GPIO_DS_MASK_VAL(gpio)		(0x0000001E << (gpio*8))	
#define GPIO_DS_VAL(gpio)			(0x00000001 << gpio)		//direction for GPIO-DS //bit order unknown???

#define GPIO_LED_MAX				5			/* pin 0-4 */
#define GPIO_FL_MAX					26			/* pin 0-25 */
#define GPIO_DS_MAX					16			/* pin 0-15 */

u32 gpio_in32(u32 addr)
{
	u32 data;

	udelay(100);
	data = in_be32((void *) addr);
	gpio_dbg("%s: line %d addr %08x value %08x\n", __FUNCTION__, __LINE__, addr, data);
	return data;
}

void gpio_out32(u32 addr, u32 val)
{
	gpio_dbg("%s: line %d addr %08x value %08x\n", __FUNCTION__, __LINE__, addr, val);
	out_be32((void *) addr, val);
	udelay(100);
}

#if 0
/*
* Set GPIO-LED
*   @pin: GPIO0-4
*	 @in-out: GPIO_OUT, GPIO_DIS: powerdown
*/
void gpio_led_config(int pin, int in_out)
{
	u32 val;
	u32 offs = 0;
	int pin_offset = pin%4;

	//set register for GPIO0-3 and GPIO4
	if (pin < 4) {
		offs = MPA_GPIO_LED_IOCTL0;
	}else	{
		offs = MPA_GPIO_LED_IOCTL1;
	}

	//set direction
	val = gpio_in32(offs);
	val &= ~GPIO_LED_MASK_VAL(pin_offset);
	if(in_out == GPIO_OUT) {
		val |= GPIO_LED_OUT_VAL(pin_offset);
	}else if(in_out == GPIO_IN) {
		val |= GPIO_DS_IN_VAL(pin_offset);
	}else {
		val |= GPIO_LED_DIS_VAL(pin_offset);
	}
	gpio_out32(offs, val);
	gpio_dbg("%s: line %d MPA_GPIO_LED_IOCTL %08x value %08x\n", __FUNCTION__, __LINE__, offs, gpio_in32(offs));

	return;	
}

/*
* Write GPIO-LED
*		@pin: 0-4
*		@val: 0-1
*/
void gpio_led_write_bit(int pin, int val)
{
	if (pin >= GPIO_LED_MAX) {
		pin = GPIO_LED_MAX-1;
		gpio_dbg("%s: line %d out of range\n", __FUNCTION__, __LINE__);
	}

	if (val) {
		gpio_out32(MPA_LED, gpio_in32(MPA_LED) |  GPIO_LED_VAL(pin));
	}else {
		gpio_out32(MPA_LED, gpio_in32(MPA_LED) &  ~GPIO_LED_VAL(pin));
	}
	gpio_dbg("%s: line %d MPA_LED %08x value %08x\n", __FUNCTION__, __LINE__, MPA_LED, gpio_in32(MPA_LED));
}

int gpio_led_read_in_bit(int pin)
{
	if (pin >= GPIO_LED_MAX) {
		pin = GPIO_LED_MAX-1;
		gpio_dbg("%s: line %d out of range\n", __FUNCTION__, __LINE__);
	}

	return ((gpio_in32(MPA_LED) & GPIO_LED_VAL(pin)) ? 1 : 0);
}

/*
* Save GPIO-LED direction and value at current
*/
void gpio_led_save_config(gpio_led_reg_list_t *gpio_led_reg_list) 
{
	gpio_led_reg_list->mpa_gpio_led_cfg[0] = gpio_in32(MPA_GPIO_LED_IOCTL0);
	gpio_led_reg_list->mpa_gpio_led_cfg[1] = gpio_in32(MPA_GPIO_LED_IOCTL1);
	gpio_led_reg_list->mpa_gpio_led_val = gpio_in32(MPA_LED);
}

void gpio_led_restore_config(gpio_led_reg_list_t *gpio_led_reg_list) 
{
	gpio_out32(MPA_GPIO_LED_IOCTL0, gpio_led_reg_list->mpa_gpio_led_cfg[0]);
	gpio_out32(MPA_GPIO_LED_IOCTL1, gpio_led_reg_list->mpa_gpio_led_cfg[1]);
	gpio_out32(MPA_LED, gpio_led_reg_list->mpa_gpio_led_val);
}


/*
* Set GPIO-FL
*   @pin: GPIO0-25
*	 @in-out: GPIO_IN, GPIO_OUT
*	 @gpio_alt: 0: all pins are EBUS
*				1: all pins are GPIO-FL
*				2: SDIO pin (???)
*/
void gpio_fl_config(int pin, int in_out, int gpio_alt)
{
	u32 val;

	switch (gpio_alt) {
case GPIO_ALT0:
	val = gpio_in32(ENABLE_INTF);
	gpio_out32(ENABLE_INTF, (val & ~0x00000007) | 0x00000000);
	break;		
case GPIO_ALT1:
	val = gpio_in32(ENABLE_INTF);
	gpio_out32(ENABLE_INTF, (val & ~0x00000007) | 0x00000002);
	break;
case GPIO_ALT2:
	gpio_dbg("Not support\n");
	break;
	}
	gpio_dbg("%s: line %d ENABLE_INTF %08x value %08x\n", __FUNCTION__, __LINE__, ENABLE_INTF, gpio_in32(ENABLE_INTF));

	if(gpio_alt == GPIO_ALT1) {		//GPIO-FL
		if(pin >= GPIO_FL_MAX) {
			pin = GPIO_FL_MAX - 1;
			gpio_dbg("%s: line %d out of range\n", __FUNCTION__, __LINE__);
		}

		/* Set direction*/
		val = gpio_in32(GPIO_SWPORTA_DDR);
		if(in_out == GPIO_OUT) {
			val |= GPIO_FL_VAL(pin);
		}else{
			val &= ~GPIO_FL_VAL(pin);
		}
		gpio_out32(GPIO_SWPORTA_DDR, val);
		gpio_dbg("%s: line %d GPIO_SWPORTA_DDR %08x value %08x\n", __FUNCTION__, __LINE__, GPIO_SWPORTA_DDR, gpio_in32(GPIO_SWPORTA_DDR));
	}

	return;
}

/*
* Write GPIO-FL
*   @pin: GPIO0-25
*	 @val: 0-1
*/
void gpio_fl_write_bit(int pin, int val)
{
	if(pin >= GPIO_FL_MAX) {
		pin = GPIO_FL_MAX - 1;
		gpio_dbg("%s: line %d out of range\n", __FUNCTION__, __LINE__);
	}

	if (val){
		gpio_out32(GPIO_SWPORTA_DR, 
			gpio_in32(GPIO_SWPORTA_DR) | GPIO_FL_VAL(pin));
	}else{
		gpio_out32(GPIO_SWPORTA_DR, 
			gpio_in32(GPIO_SWPORTA_DR) & ~GPIO_FL_VAL(pin));
	}
	gpio_dbg("%s: line %d GPIO_SWPORTA_DR %08x value %08x\n", __FUNCTION__, __LINE__, GPIO_SWPORTA_DR, gpio_in32(GPIO_SWPORTA_DR));
}

int gpio_fl_read_in_bit(int pin)
{
	if(pin >= GPIO_FL_MAX) {
		pin = GPIO_FL_MAX - 1;
		gpio_dbg("%s: line %d out of range\n", __FUNCTION__, __LINE__);
	}

	return (gpio_in32(GPIO_EXT_PORTA) & GPIO_FL_VAL(pin) ? 1 : 0);
}


/*
* Save GPIO-FL direction and value at current
*/
void gpio_fl_save_config(gpio_fl_reg_list_t *gpio_fl_reg_list) 
{
	gpio_fl_reg_list->gpio_swporta_ddr_cfg = gpio_in32(GPIO_SWPORTA_DDR);
	gpio_fl_reg_list->gpio_swporta_dr_val  = gpio_in32(GPIO_SWPORTA_DR); 
}

void gpio_fl_restore_config(gpio_fl_reg_list_t *gpio_fl_reg_list) 
{
	gpio_out32(GPIO_SWPORTA_DR, gpio_fl_reg_list->gpio_swporta_dr_val);
	gpio_out32(GPIO_SWPORTA_DDR, gpio_fl_reg_list->gpio_swporta_ddr_cfg);
}
#endif


/*
* Set GPIO-DS
*   @pin: GPIO0-15
*	 @in-out: GPIO_IN, GPIO_OUT
*	 @gpio_alt: 0: GPIO_DS
*              1: trace out if pin0-4, IRQ out if pin7, IRQ in if pin8-15 IRQ
*	            2: pin strap (not settings)
*/
void gpio_ds_config(int pin, int in_out, int gpio_alt)
{
	u32 val, offs=0;
	u32 gpio_sel_val, irq_sel_val;
	int pin_offset = pin%4;

	/* read gpio_sel bits current */
	val = gpio_in32(MPA_DIAG);
	gpio_sel_val = (val >> 8)  & 0x03;
	irq_sel_val  = (val >> 23) & 0x1FF;

	/* Set GPIO-DS type */
	switch (gpio_alt) {
case GPIO_ALT0:
	if (pin < 5) {
		gpio_sel_val = 0x0;			/* all GPIO-DS */
	}else {
		if(gpio_sel_val != 0x01)	/* not trace setting */
			gpio_sel_val = 0x00;	
		if(pin >= 7)
			irq_sel_val &= ~(0x001 << (pin-7)) ;
	}
	break;		
case GPIO_ALT1:
	if (pin < 5) {
		gpio_sel_val = 0x01;		/* pin0-4 trace */
	}else {
		if(gpio_sel_val != 0x01) 	/* not trace setting */
			gpio_sel_val = 0x00;	
		irq_sel_val |= (0x001 << (pin-7)) ;
	}
	break;		
case GPIO_ALT2:
	printf("Not support\n");
	return;
	}

	/* set configuration for GPIO */
	switch(pin/4) {
case 0: offs = MPA_GPIO_DS_IOCTL0; break;
case 1: offs = MPA_GPIO_DS_IOCTL1; break;
case 2: offs = MPA_GPIO_DS_IOCTL2; break;
case 3: offs = MPA_GPIO_DS_IOCTL3; break;
	}

	val = gpio_in32(offs);
	val &= ~GPIO_DS_MASK_VAL(pin_offset);
	if(in_out == GPIO_OUT) {
		val |= GPIO_DS_OUT_VAL(pin_offset);
	}else if(in_out == GPIO_IN) {
		val |= GPIO_DS_IN_VAL(pin_offset);
	}else {	
		val |= GPIO_DS_DIS_VAL(pin_offset);
	}
	gpio_out32(offs, val);	
	gpio_dbg("%s: line %d MPA_GPIO_DS_IOCTL %08x value %08x\n", __FUNCTION__, __LINE__, offs, gpio_in32(offs));

	/* Set direction*/
	val = gpio_in32(MPA_GPIO);
	if(in_out == GPIO_OUT) {
		val |= (0x00000001 << (pin + 16));
	}else{
		val &= ~(0x00000001 << (pin + 16));
	}
	gpio_out32(MPA_GPIO, val);
	gpio_dbg("%s: line %d MPA_GPIO %08x value %08x\n", __FUNCTION__, __LINE__, MPA_GPIO, gpio_in32(MPA_GPIO));

	gpio_out32(MPA_DIAG, 
		(gpio_in32(MPA_DIAG) & ~(MPA_DIAG_GPIO_SEL_MASK | MPA_DIAG_IRQ_SEL_MASK))
		| ((gpio_sel_val << 8) | (irq_sel_val << 23)));
	gpio_dbg("%s: line %d MPA_DIAG %08x value %08x\n", __FUNCTION__, __LINE__, MPA_DIAG, gpio_in32(MPA_DIAG));
	return;
}

/*
* Write GPIO-DS
*   @pin: GPIO0-15
*	 @val: 0-1
*/
void gpio_ds_write_bit(int pin, int val)
{
	if(pin >= GPIO_DS_MAX) {
		pin = GPIO_DS_MAX - 1;
		gpio_dbg("%s: line %d out of range\n", __FUNCTION__, __LINE__);
	}

	if (val){
		gpio_out32(MPA_GPIO, 
			gpio_in32(MPA_GPIO) | GPIO_DS_VAL(pin));
	}else{
		gpio_out32(MPA_GPIO, 
			gpio_in32(MPA_GPIO) & ~GPIO_DS_VAL(pin));
	}
	gpio_dbg("%s: line %d MPA_GPIO %08x value %08x\n", __FUNCTION__, __LINE__, MPA_GPIO, gpio_in32(MPA_GPIO));
}

int gpio_ds_read_in_bit(int pin)
{
	if(pin >= GPIO_DS_MAX) {
		pin = GPIO_DS_MAX - 1;
		gpio_dbg("%s: line %d out of range\n", __FUNCTION__, __LINE__);
	}

	u32 data = gpio_in32(MPA_GPIO_IN);
	gpio_dbg("%s: line %d MPA_GPIO_IN %08x value %08x\n", __FUNCTION__, __LINE__, MPA_GPIO_IN, data);
	return ((data & GPIO_DS_VAL(pin)) ? 1 : 0);
}

#if 0
/*
* Save GPIO-DS direction and value at current
*/
void gpio_ds_save_config(gpio_ds_reg_list_t *gpio_ds_reg_list) 
{
	gpio_ds_reg_list->mpa_gpio_ds_cfg[0] = gpio_in32(MPA_GPIO_DS_IOCTL0);
	gpio_ds_reg_list->mpa_gpio_ds_cfg[1] = gpio_in32(MPA_GPIO_DS_IOCTL1);
	gpio_ds_reg_list->mpa_gpio_ds_cfg[2] = gpio_in32(MPA_GPIO_DS_IOCTL2);
	gpio_ds_reg_list->mpa_gpio_ds_cfg[3] = gpio_in32(MPA_GPIO_DS_IOCTL3);
	gpio_ds_reg_list->mpa_gpio_ds_val    = gpio_in32(MPA_GPIO); 
}

void gpio_ds_restore_config(gpio_ds_reg_list_t *gpio_ds_reg_list) 
{
	gpio_out32(MPA_GPIO_DS_IOCTL0, gpio_ds_reg_list->mpa_gpio_ds_cfg[0]);
	gpio_out32(MPA_GPIO_DS_IOCTL1, gpio_ds_reg_list->mpa_gpio_ds_cfg[1]);
	gpio_out32(MPA_GPIO_DS_IOCTL2, gpio_ds_reg_list->mpa_gpio_ds_cfg[2]);
	gpio_out32(MPA_GPIO_DS_IOCTL3, gpio_ds_reg_list->mpa_gpio_ds_cfg[3]);
	gpio_out32(MPA_GPIO, gpio_ds_reg_list->mpa_gpio_ds_val);
}


/* Only some 4xx variants support alternate functions on the GPIO's */
void gpio_config(int gpio_type, int pin, int in_out, int gpio_alt)
{
	switch(gpio_type) {
case GPIO_DS:
	gpio_ds_config(pin, in_out, gpio_alt);
	break;
case GPIO_FL:
	gpio_fl_config(pin, in_out, gpio_alt);
	break;
case GPIO_LED:
	gpio_led_config(pin, in_out);
	break;
	}

	return;		
}

void gpio_write_bit(int gpio_type, int pin, int val)
{
	switch(gpio_type) {
case GPIO_DS:
	gpio_ds_write_bit(pin, val);
	break;
case GPIO_FL:
	gpio_fl_write_bit(pin, val);
	break;
case GPIO_LED:
	gpio_led_write_bit(pin, val);
	break;
	}

	return;		
}

int gpio_read_in_bit(int gpio_type, int pin)
{
	int val = 0;

	switch(gpio_type) {
case GPIO_DS:
	val = gpio_ds_read_in_bit(pin);
	break;
case GPIO_FL:
	val = gpio_fl_read_in_bit(pin);
	break;
case GPIO_LED:
	val = gpio_led_read_in_bit(pin);
	break;
	}

	return val;		
}

void gpio_save_config(int gpio_type, gpio_reg_list_t *gpio_reg_list) 
{
	switch(gpio_type) {
case GPIO_DS:
	gpio_ds_save_config(gpio_reg_list->gpio_ds_reg);
	break;
case GPIO_FL:
	gpio_fl_save_config(gpio_reg_list->gpio_fl_reg);
	break;
case GPIO_LED:
	gpio_led_save_config(gpio_reg_list->gpio_led_reg);
	break;
	}
}

void gpio_restore_config(int gpio_type, gpio_reg_list_t *gpio_reg_list) 
{
	switch(gpio_type) {
case GPIO_DS:
	gpio_ds_restore_config(gpio_reg_list->gpio_ds_reg);
	break;
case GPIO_FL:
	gpio_fl_restore_config(gpio_reg_list->gpio_fl_reg);
	break;
case GPIO_LED:
	gpio_led_restore_config(gpio_reg_list->gpio_led_reg);
	break;
	}
}
#endif

#endif //defined(CONFIG_APM86XXX_GPIO)

#if defined(CONFIG_MISC_INIT_R) && defined(CONFIG_MERAKI)
int sw_current_dev(void)
{
	u32 sw_dev;

	sw_dev = gpio_in32(MPA_GPIO);
	if (sw_dev & GPIO_DS_VAL(14)) {
		printf("GPIO 14 enable\n");
		sw_dev = 0;
	}else{
		printf("GPIO 14 disable\n");
		sw_dev = 1;
	}

	return sw_dev;
}

void sw_select_dev(int sw_dev)
{
	int gpio_val = 0;	//disable all

	if ((sw_dev>1) || (sw_dev<0)) {
		printf("ERROR: Cannot set MDIO to device %d\n", sw_dev);
		return;
	}
	gpio_val = (sw_dev==0)?1:0;
	printf("GPIO 14 %s\n",(gpio_val==1)?"enable":"disable");
	
	/*set gpio for MDC clock out to Ethernet PHY1*/
	gpio_ds_write_bit(14, gpio_val);
	//gpio_ds_config(14, GPIO_OUT, GPIO_ALT0);
	udelay(10000);
}

int check_reset_boot (void)
{
	/* check reset switch for manufacturer boot */
	if (gpio_ds_read_in_bit(15) == 0)
		run_command("setenv netretry no ; run factory_boot", 1);

	return 0;
}

void init_switch_gpio(void)
{
	printf("Default enable MDIO for ETH0\n");

	/*set gpio for MDC clock out to Ethernet PHY1*/
	gpio_ds_write_bit(14, 1);				//enable SW0, disable SW1
	gpio_ds_config(14, GPIO_OUT, GPIO_ALT0);
}

int misc_init_r (void)
{

#if 0
	/*set gpio for MDC clock out to Ethernet PHY2 - select 1 PHY each times*/
	gpio_ds_write_bit(2, 0);				//disable
	gpio_ds_config(2, GPIO_OUT, GPIO_ALT0);

	/*set gpio for MDC clock out to Ethernet PHY Combo - select 1 PHY each times*/
	gpio_ds_write_bit(13, 0);				//disable
	gpio_ds_config(13, GPIO_OUT, GPIO_ALT0);
#endif

	return 0;
}
#endif


