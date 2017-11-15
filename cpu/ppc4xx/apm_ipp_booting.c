/*
 * Copyright (c) 2010 Applied Micro Circuits Corporation.
 * All rights reserved. 
 *	Anup Patel <apatel@apm.com>
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
 */

#include <common.h>
#include <asm/apm_ipp_interface.h>
#include <asm/apm_ipp_csr.h>
#include <asm/mmu.h>
#include <asm/errno.h>
#define __iomem
#include <asm/apm_scu.h>
#include "apm_spd_ddr.h"

#define WRITE32(_addr, _value)	*((volatile u32*)(_addr))=(_value)
#define READ32(_addr)	*((volatile u32 *)(_addr))

#define clrbits(type, addr, clear) \
	out_##type((addr), in_##type(addr) & ~(clear))

#define setbits(type, addr, set) \
	out_##type((addr), in_##type(addr) | (set))

#define clrsetbits(type, addr, clear, set) \
	out_##type((addr), (in_##type(addr) & ~(clear)) | (set))

#define clrbits_be32(addr, clear) clrbits(be32, addr, clear)
#define setbits_be32(addr, set) setbits(be32, addr, set)
#define clrsetbits_be32(addr, clear, set) clrsetbits(be32, addr, clear, set)

#define HBF_RELAX(x)		{ \
	volatile u32 delay; \
	for (delay = 0; delay < (x*1000); delay++); \
	}
		
void ipp_mttlb1(unsigned long index, unsigned long value);
void ipp_mttlb2(unsigned long index, unsigned long value);
void ipp_mttlb3(unsigned long index, unsigned long value);
unsigned long ipp_mftlb1(unsigned long index);
unsigned long ipp_mftlb2(unsigned long index);
unsigned long ipp_mftlb3(unsigned long index);	

//static inline void sync(void)
//{
//	__asm__ __volatile__ ("sync" : : : "memory");
//}

static inline unsigned int in_be32(const volatile unsigned __iomem *addr)
{
	unsigned ret;

	__asm__ __volatile__("sync; lwz%U1%X1 %0,%1;\n"
			     "twi 0,%0,0;\n"
			     "isync" : "=r" (ret) : "m" (*addr));
	return ret;
}

static inline void out_be32(volatile unsigned __iomem *addr, int val)
{
	__asm__ __volatile__("sync; stw%U0%X0 %1,%0" : "=m" (*addr) : "r" (val));
}

static int ipp_add_tlb_entry(u64 phys_addr,
			 u32 virt_addr,
			 u32 tlb_word0_size_value,
			 u32 tlb_word2_i_value)
{
	int i;
	unsigned long tlb_word0_value;
	unsigned long tlb_word1_value;
	unsigned long tlb_word2_value;

	/* First, find the index of a TLB entry not being used */
	for (i = 0; i < PPC4XX_TLB_SIZE; i++) {
		tlb_word0_value = ipp_mftlb1(i);
		if ((tlb_word0_value & TLB_WORD0_V_MASK) == TLB_WORD0_V_DISABLE)
			break;
	}
	if (i >= PPC4XX_TLB_SIZE)
		return -1;

	/* Second, create the TLB entry */
	tlb_word0_value = TLB_WORD0_EPN_ENCODE(virt_addr) | TLB_WORD0_V_ENABLE |
		TLB_WORD0_TS_0 | tlb_word0_size_value;
	tlb_word1_value = TLB_WORD1_RPN_ENCODE((u32)phys_addr) |
		TLB_WORD1_ERPN_ENCODE(phys_addr >> 32);
	tlb_word2_value = TLB_WORD2_U0_DISABLE | TLB_WORD2_U1_DISABLE |
		TLB_WORD2_U2_DISABLE | TLB_WORD2_U3_DISABLE |
		TLB_WORD2_W_DISABLE | tlb_word2_i_value |
		TLB_WORD2_M_DISABLE | TLB_WORD2_G_DISABLE |
		TLB_WORD2_E_DISABLE | TLB_WORD2_UX_ENABLE |
		TLB_WORD2_UW_ENABLE | TLB_WORD2_UR_ENABLE |
		TLB_WORD2_SX_ENABLE | TLB_WORD2_SW_ENABLE |
		TLB_WORD2_SR_ENABLE;

	/* Wait for all memory accesses to complete */
	sync();

	/* Third, add the TLB entries */
	ipp_mttlb1(i, tlb_word0_value);
	ipp_mttlb2(i, tlb_word1_value);
	ipp_mttlb3(i, tlb_word2_value);

	/* Execute an ISYNC instruction so that the new TLB entry takes effect */
	asm("isync");

	return 0;
}

static void ipp_program_tlb_addr(u64 phys_addr,
			     u32 virt_addr,
			     u32 mem_size,
			     u32 tlb_word2_i_value)
{
	int rc;
	int tlb_i;

	tlb_i = tlb_word2_i_value;
	while (mem_size != 0) {
		rc = 0;
		/* Add the TLB entries in to map the region. */
		if (((phys_addr & TLB_256MB_ALIGN_MASK) == phys_addr) &&
		    (mem_size >= TLB_256MB_SIZE)) {
			/* Add a 256MB TLB entry */
			if ((rc = ipp_add_tlb_entry(phys_addr, virt_addr,
					TLB_WORD0_SIZE_256MB, tlb_i)) == 0) {
				mem_size -= TLB_256MB_SIZE;
				phys_addr += TLB_256MB_SIZE;
				virt_addr += TLB_256MB_SIZE;
			}
		} else if (((phys_addr & TLB_16MB_ALIGN_MASK) == phys_addr) &&
			   (mem_size >= TLB_16MB_SIZE)) {
			/* Add a 16MB TLB entry */
			if ((rc = ipp_add_tlb_entry(phys_addr, virt_addr,
					TLB_WORD0_SIZE_16MB, tlb_i)) == 0) {
				mem_size -= TLB_16MB_SIZE;
				phys_addr += TLB_16MB_SIZE;
				virt_addr += TLB_16MB_SIZE;
			}
		} else if (((phys_addr & TLB_1MB_ALIGN_MASK) == phys_addr) &&
			   (mem_size >= TLB_1MB_SIZE)) {
			/* Add a 1MB TLB entry */
			if ((rc = ipp_add_tlb_entry(phys_addr, virt_addr,
					TLB_WORD0_SIZE_1MB, tlb_i)) == 0) {
				mem_size -= TLB_1MB_SIZE;
				phys_addr += TLB_1MB_SIZE;
				virt_addr += TLB_1MB_SIZE;
			}
		} else if (((phys_addr & TLB_256KB_ALIGN_MASK) == phys_addr) &&
			   (mem_size >= TLB_256KB_SIZE)) {
			/* Add a 256KB TLB entry */
			if ((rc = ipp_add_tlb_entry(phys_addr, virt_addr,
					TLB_WORD0_SIZE_256KB, tlb_i)) == 0) {
				mem_size -= TLB_256KB_SIZE;
				phys_addr += TLB_256KB_SIZE;
				virt_addr += TLB_256KB_SIZE;
			}
		} else if (((phys_addr & TLB_64KB_ALIGN_MASK) == phys_addr) &&
			   (mem_size >= TLB_64KB_SIZE)) {
			/* Add a 64KB TLB entry */
			if ((rc = ipp_add_tlb_entry(phys_addr, virt_addr,
					TLB_WORD0_SIZE_64KB, tlb_i)) == 0) {
				mem_size -= TLB_64KB_SIZE;
				phys_addr += TLB_64KB_SIZE;
				virt_addr += TLB_64KB_SIZE;
			}
		} else if (((phys_addr & TLB_16KB_ALIGN_MASK) == phys_addr) &&
			   (mem_size >= TLB_16KB_SIZE)) {
			/* Add a 16KB TLB entry */
			if ((rc = ipp_add_tlb_entry(phys_addr, virt_addr,
					TLB_WORD0_SIZE_16KB, tlb_i)) == 0) {
				mem_size -= TLB_16KB_SIZE;
				phys_addr += TLB_16KB_SIZE;
				virt_addr += TLB_16KB_SIZE;
			}
		} else if (((phys_addr & TLB_4KB_ALIGN_MASK) == phys_addr) &&
			   (mem_size >= TLB_4KB_SIZE)) {
			/* Add a 4KB TLB entry */
			if ((rc = ipp_add_tlb_entry(phys_addr, virt_addr,
					TLB_WORD0_SIZE_4KB, tlb_i)) == 0) {
				mem_size -= TLB_4KB_SIZE;
				phys_addr += TLB_4KB_SIZE;
				virt_addr += TLB_4KB_SIZE;
			}
		} else if (((phys_addr & TLB_1KB_ALIGN_MASK) == phys_addr) &&
			   (mem_size >= TLB_1KB_SIZE)) {
			/* Add a 1KB TLB entry */
			if ((rc = ipp_add_tlb_entry(phys_addr, virt_addr,
					TLB_WORD0_SIZE_1KB, tlb_i)) == 0) {
				mem_size -= TLB_1KB_SIZE;
				phys_addr += TLB_1KB_SIZE;
				virt_addr += TLB_1KB_SIZE;
			}
		} else {
			/* ERROR: no TLB size exists. Not possible */			
		}

		if (rc != 0) {
			/* ERROR: no TLB entries available. Not possible */
		}
	}
}

/*
 * Program one (or multiple) TLB entries for one memory region
 *
 * Common usage for boards with SDRAM DIMM modules to dynamically
 * configure the TLB's for the SDRAM
 */
void ipp_program_tlb(u64 phys_addr, u32 virt_addr, u32 size, 
			u32 tlb_word2_i_value)
{
	/* Call the routine to add in the tlb entries for the memory regions */
	ipp_program_tlb_addr(phys_addr, virt_addr, size, tlb_word2_i_value);
}

/*
 * This DDR2 setup code can dynamically setup TLB entries for the DDR2 memory
 * region. Right now the cache should still be disabled in U-Boot because of
 * the EMAC driver, that need it's buffer descriptor to be located in non cached
 * memory.
 *
 * If at some time this restriction doesn't apply anymore, just define
 * CONFIG_4xx_DCACHE in the board config file and this code should setup
 * everything correctly.
 */
#if defined(CONFIG_4xx_DCACHE)
#define MY_TLB_WORD2_I_ENABLE TLB_WORD2_M_ENABLE	/* enable caching on SDRAM */
#else
#define MY_TLB_WORD2_I_ENABLE TLB_WORD2_I_ENABLE 	/* disable caching on SDRAM */
#endif

#if defined CONFIG_MEDIA_SPL
#define DBG_PRINT(x, y)
#define DBG_PRINT_FS(x, y, z)
#else
#define DBG_PRINT_FS printf
#if 0
#define DEBUG_PRINT
#endif
#ifdef DEBUG_PRINT
#define DBG_PRINT	printf
#else
#define DBG_PRINT(x, y)
#endif
#endif

#if defined CONFIG_MEDIA_SPL
char *strcpy(char *dst, const char *src)
{
	char *rt_dst = dst;
	while((*dst++ = *src++) != '\0');

	return rt_dst;
}
#endif

int apm86xxx_ipp_read_file(char *f_name, u32 addr, u32 offset, u32 size)
{
	u32 ite,data,param,fword[IPP_BOOTACCESS_MAX_FILENAME_SIZE/4];
	char file_name[IPP_BOOTACCESS_MAX_FILENAME_SIZE];

	strcpy(file_name, f_name);
	file_name[IPP_BOOTACCESS_MAX_FILENAME_SIZE-1] = '\0';
	for(ite=0;ite<4;ite++) {
		fword[ite] = 0;
	}
	for(ite=0;ite<IPP_BOOTACCESS_MAX_FILENAME_SIZE;ite++) {
		fword[ite/4] |= file_name[ite] << ((ite%4)*8);
	}
	
	/* Word0: Access control information */
	if(offset==0 && size==0xFFFFFFFF) {
		param =	IPP_ENCODE_BOOTACCESS_WORD0(IPP_BOOTACCESS_DEVICE_DEFAULT,IPP_BOOTACCESS_OP_READFULL);
	} else {
		param =	IPP_ENCODE_BOOTACCESS_WORD0(IPP_BOOTACCESS_DEVICE_DEFAULT,IPP_BOOTACCESS_OP_READ);
	}
	DBG_PRINT("%x\n", param);
	data = IPP_ENCODE_DATA_MSG(IPP_BOOTACCESS_HDLR, 
					CTRL_BYTE_UNSED,
					IPP_DATA_MSG_CBYTE_START_BIT,
					0);
	apm86xxx_write_ipp_msg(data, param);

	DBG_PRINT("%x\n", fword[0]);
	/* Word1: File name (first four bytes) */
	param = fword[0];
	data = IPP_ENCODE_DATA_MSG(IPP_BOOTACCESS_HDLR, 
					CTRL_BYTE_UNSED, 
					IPP_DATA_MSG_CBYTE_DATA_BIT,
					1);
	apm86xxx_write_ipp_msg(data, param);

	DBG_PRINT("%x\n", fword[1]);
	/* Word2: File name (second four bytes) */
	param = fword[1];
	data = IPP_ENCODE_DATA_MSG(IPP_BOOTACCESS_HDLR, 
					CTRL_BYTE_UNSED, 
					IPP_DATA_MSG_CBYTE_DATA_BIT,
					2);
	apm86xxx_write_ipp_msg(data, param);

	DBG_PRINT("%x\n", fword[2]);
	/* Word3: File name (third four bytes) */
	param = fword[2];
	data = IPP_ENCODE_DATA_MSG(IPP_BOOTACCESS_HDLR, 
					CTRL_BYTE_UNSED, 
					IPP_DATA_MSG_CBYTE_DATA_BIT,
					3);
	apm86xxx_write_ipp_msg(data, param);

	DBG_PRINT("%x\n", fword[3]);
	/* Word4: File name (fourth four bytes) */
	param = fword[3];
	data = IPP_ENCODE_DATA_MSG(IPP_BOOTACCESS_HDLR, 
					CTRL_BYTE_UNSED, 
					IPP_DATA_MSG_CBYTE_DATA_BIT,
					4);
	apm86xxx_write_ipp_msg(data, param);

	DBG_PRINT("%x\n", offset);
	/* Word5: File offset (specifies where to start reading/writing in file) */
	param = offset;
	data = IPP_ENCODE_DATA_MSG(IPP_BOOTACCESS_HDLR, 
					CTRL_BYTE_UNSED, 
					IPP_DATA_MSG_CBYTE_DATA_BIT,
					5);
	apm86xxx_write_ipp_msg(data, param);

	DBG_PRINT("%x\n", size);
	/* Word6: File read size (specifies how much to read/write from file) */
	param = size;
	data = IPP_ENCODE_DATA_MSG(IPP_BOOTACCESS_HDLR, 
					CTRL_BYTE_UNSED, 
					IPP_DATA_MSG_CBYTE_DATA_BIT,
					6);
	apm86xxx_write_ipp_msg(data, param);

	DBG_PRINT("%x\n", IPP_ENCODE_BOOTACCESS_ADDRESS(0x0,addr));
	/* Word7: 16-byte aligned destination address */
	param = IPP_ENCODE_BOOTACCESS_ADDRESS(0x0,addr);
	data = IPP_ENCODE_DATA_MSG(IPP_BOOTACCESS_HDLR,
					CTRL_BYTE_UNSED,
					IPP_DATA_MSG_CBYTE_STOP_BIT,
					7);
	apm86xxx_write_ipp_msg(data, param);

	/* Keep polling till iPP responds OK */
	while(1) {
		if(!apm86xxx_read_ipp_msg(&data)) {
			if(IPP_DECODE_MSG_TYPE(data)==IPP_OK_MSG) {
				break;
			} else {
				return -ENAVAIL;
			}
		}
		HBF_RELAX(1000);
	}

	return 0;
}

void relocate_using_ipp(void)
{
	int ret;
	u32 dram_size;
	char *file_name = "u-boot.bin";

#ifdef CONFIG_MEDIA_SPL
	__attribute__((noreturn)) void (*uboot)(void);
#endif

//#ifdef CONFIG_MEDIA_SPL
//	volatile u32 t;
//	t = 0;
//	while(t==0);
//#endif

	/* Intialize DDR before relocating */
	extern phys_size_t initdram_int(int board_type);
	dram_size = initdram_int(0);
	if (dram_size <= 0) 
		while (1);

	/* Program TLB */		
#if defined(CONFIG_PAL) || defined(CONFIG_NOT_COHERENT_CACHE)
	ipp_program_tlb(0, CONFIG_SYS_SDRAM_BASE, dram_size, SA_I|SA_G|AC_R|AC_W|AC_X);
#else
	ipp_program_tlb(0, CONFIG_SYS_SDRAM_BASE, dram_size, SA_WL1|SA_M|AC_R|AC_W|AC_X);
#endif

	/* Ask iPP to relocate U-boot to DDR at given physical address */
	ret = apm86xxx_ipp_read_file(file_name, 0x01000000, 0, 0xFFFFFFFF);
	if(ret)
		while(1);

#ifdef CONFIG_MEDIA_SPL
	/*
	 * Jump to U-Boot image
	 */
#define CONFIG_SYS_NAND_U_BOOT_START 0x01000000 /* using same address as NUB U-boot */
	uboot = (void *) CONFIG_SYS_NAND_U_BOOT_START;
	(*uboot)();
#endif
}

void spd_ddr_init_hang(void)
{
	while (1);
}
