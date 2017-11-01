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
#include <malloc.h>
#include <asm/fsl_pamu.h>

paace_t *primary;
paace_t *sec;

static inline int count_msb_zeroes(unsigned long val)
{
	return __builtin_clzl(val);
}

static inline int __ilog2_roundup_64(uint64_t val)
{
	if ((val - 1) >> 32)
		return 64 - (count_msb_zeroes((val - 1) >> 32));
	else
		return 32 - count_msb_zeroes((uint32_t)(val - 1));
}


static inline int count_lsb_zeroes(unsigned long val)
{
	return __builtin_ctzl(val);
}

static unsigned int map_addrspace_size_to_wse(uint64_t addrspace_size)
{
	/* window size is 2^(WSE+1) bytes */
	return count_lsb_zeroes(addrspace_size >> PAMU_PAGE_SHIFT) +
		PAMU_PAGE_SHIFT - 1;
}

static unsigned int map_subwindow_cnt_to_wce(uint32_t subwindow_cnt)
{
       /* window count is 2^(WCE+1) bytes */
	return count_lsb_zeroes(subwindow_cnt) - 1;
}

static void pamu_setup_default_xfer_to_host_ppaace(paace_t *ppaace)
{
	set_bf(ppaace->addr_bitfields, PAACE_AF_PT, PAACE_PT_PRIMARY);
	set_bf(ppaace->domain_attr.to_host.coherency_required, PAACE_DA_HOST_CR,
		PAACE_M_COHERENCE_REQ);
}

static void pamu_setup_default_xfer_to_host_spaace(paace_t *spaace)
{
	set_bf(spaace->addr_bitfields, PAACE_AF_PT, PAACE_PT_SECONDARY);
	set_bf(spaace->domain_attr.to_host.coherency_required, PAACE_DA_HOST_CR,
	       PAACE_M_COHERENCE_REQ);
}

/** Sets up PPAACE entry for specified liodn
 *
 * @param[in] liodn      Logical IO device number
 * @param[in] win_addr   starting address of DSA window
 * @param[in] win-size   size of DSA window
 * @param[in] omi        Operation mapping index -- if ~omi == 0 then omi
				not defined
 * @param[in] stashid    cache stash id for associated cpu -- if ~stashid == 0
				then stashid not defined
 * @param[in] snoopid    snoop id for hardware coherency -- if ~snoopid == 0
				then snoopid not defined
 * @param[in] subwin_cnt number of sub-windows
 *
 * @return Returns 0 upon success else error code < 0 returned
 */
static int pamu_config_ppaace(uint32_t liodn, uint64_t win_addr,
	uint64_t win_size, uint32_t omi,
	uint32_t snoopid, uint32_t stashid,
	uint32_t subwin_cnt)
{
	unsigned long fspi;
	paace_t *ppaace;

	if ((win_size & (win_size - 1)) || win_size < PAMU_PAGE_SIZE)
		return -1;

	if (win_addr & (win_size - 1))
		return -2;

	ppaace = &primary[liodn];

	/* window size is 2^(WSE+1) bytes */
	set_bf(ppaace->addr_bitfields, PPAACE_AF_WSE,
		map_addrspace_size_to_wse(win_size));

	pamu_setup_default_xfer_to_host_ppaace(ppaace);

	if (sizeof(phys_addr_t) > 4)
		ppaace->wbah = (u64)win_addr >> (PAMU_PAGE_SHIFT + 20);
	else
		ppaace->wbah = 0;

	set_bf(ppaace->addr_bitfields, PPAACE_AF_WBAL,
	       (win_addr >> PAMU_PAGE_SHIFT));

	/* set up operation mapping if it's configured */
	if (omi < OME_NUMBER_ENTRIES) {
		set_bf(ppaace->impl_attr, PAACE_IA_OTM, PAACE_OTM_INDEXED);
		ppaace->op_encode.index_ot.omi = omi;
	} else if (~omi != 0)
		return -3;

	/* configure stash id */
	if (~stashid != 0)
		set_bf(ppaace->impl_attr, PAACE_IA_CID, stashid);

	/* configure snoop id */
	if (~snoopid != 0)
		ppaace->domain_attr.to_host.snpid = snoopid;

	if (subwin_cnt) {
		/* We have a single SPAACE for 1 LIODN. So index would
			be always 0 */
		fspi = 0;

		/* window count is 2^(WCE+1) bytes */
		set_bf(ppaace->impl_attr, PAACE_IA_WCE,
		       map_subwindow_cnt_to_wce(subwin_cnt));
		set_bf(ppaace->addr_bitfields, PPAACE_AF_MW, 0x1);
		ppaace->fspi = fspi;
	} else
		set_bf(ppaace->addr_bitfields, PAACE_AF_AP, PAACE_AP_PERMS_ALL);

	asm volatile("sync" : : : "memory");
	/* Mark the ppace entry valid */
	ppaace->addr_bitfields |= PAACE_V_VALID;
	asm volatile("sync" : : : "memory");

	return 0;
}

static int pamu_config_spaace(uint32_t liodn,
	uint64_t subwin_size, uint64_t subwin_addr, uint64_t size,
	uint32_t omi, uint32_t snoopid, uint32_t stashid)
{
	paace_t *paace;
	unsigned long fspi;
	/* Align start addr of subwin to subwindoe size */
	uint64_t sec_addr = subwin_addr & ~(subwin_size - 1);
	uint64_t end_addr = subwin_addr + size;
	int size_shift = __ilog2_u64(subwin_size);
	uint64_t win_size = 0;
	uint32_t index, swse;

	/* Recalculate the size */
	size = end_addr - sec_addr;

	if (!subwin_size)
		return -1;

	while (sec_addr < end_addr) {
#ifdef DEBUG
		printf("sec_addr < end_addr is %llx < %llx\n", sec_addr,
			end_addr);
#endif
		paace = &primary[liodn];
		if (!paace)
			return -1;

		fspi = paace->fspi;

		/* Calculating the win_size here as if we map in index 0,
			paace entry woudl need to  be programmed for SWSE */
		win_size = end_addr - sec_addr;
		win_size = 1 << __ilog2_roundup_64(win_size);

		if (win_size > subwin_size)
			win_size = subwin_size;
		else if (win_size < PAMU_PAGE_SIZE)
			win_size = PAMU_PAGE_SIZE;

#ifdef DEBUG
		printf("win_size is %x\n", win_size);
#endif

		swse = map_addrspace_size_to_wse(win_size);
		index = fspi +  (sec_addr >> size_shift);

		if (index == 0) {
			set_bf(paace->win_bitfields, PAACE_WIN_SWSE, swse);
			set_bf(paace->addr_bitfields, PAACE_AF_AP,
				PAACE_AP_PERMS_ALL);
			sec_addr += subwin_size;
			continue;
		}

		paace = sec + index - 1;

#ifdef DEBUG
		printf("SPAACT:Writing at location %p, index %d\n", paace,
			index);
#endif

		pamu_setup_default_xfer_to_host_spaace(paace);
		set_bf(paace->addr_bitfields, SPAACE_AF_LIODN, liodn);
		set_bf(paace->addr_bitfields, PAACE_AF_AP, PAACE_AP_PERMS_ALL);

		/* configure snoop id */
		if (~snoopid != 0)
			paace->domain_attr.to_host.snpid = snoopid;

		if (paace->addr_bitfields & PAACE_V_VALID) {
#ifdef DEBUG
			printf("Reached overlap condition\n");
			printf("%d < %d\n", get_bf(paace->win_bitfields,
				PAACE_WIN_SWSE), swse);
#endif
			if (get_bf(paace->win_bitfields, PAACE_WIN_SWSE) < swse)
				set_bf(paace->win_bitfields, PAACE_WIN_SWSE,
					swse);
		} else
			set_bf(paace->win_bitfields, PAACE_WIN_SWSE, swse);

		paace->addr_bitfields |= PAACE_V_VALID;
		sec_addr += subwin_size;
	}

	return 0;
}

int pamu_init(void)
{
	ccsr_pamu_t *regs = (ccsr_pamu_t *)CONFIG_SYS_PAMU_ADDR;

	primary = memalign(PAMU_TABLE_ALIGNMENT,
				 sizeof(paace_t) * NUM_PPAACT_ENTRIES);
	if (!primary)
		return -1;
	memset(primary, 0,  sizeof(paace_t) * NUM_PPAACT_ENTRIES);

	sec = memalign(PAMU_TABLE_ALIGNMENT,
			 sizeof(paace_t) * NUM_SPAACT_ENTRIES);
	if (!sec)
		return -1;
	memset(sec, 0,  sizeof(paace_t) * NUM_SPAACT_ENTRIES);

	uint64_t pamu = virt_to_phys((void *)primary);
#ifdef DEBUG
	printf("PAACT on address location  %llx\n", pamu);
#endif

	out_be32(&regs->ppbah, pamu >> 32);
	out_be32(&regs->ppbal, (uint32_t)pamu);

	pamu = (uint64_t)virt_to_phys((void *)(primary + NUM_PPAACT_ENTRIES));
	out_be32(&regs->pplah, pamu >> 32);
	out_be32(&regs->pplal, (uint32_t)pamu);

	pamu = 0;

	if (sec != NULL) {
		pamu = (uint64_t)virt_to_phys((void *)sec);
#ifdef DEBUG
		printf("SPAACT on location  %llx\n", pamu);
#endif
		out_be32(&regs->spbah, pamu << 32);
		out_be32(&regs->spbal, (uint32_t)pamu);
		pamu = (uint64_t)virt_to_phys((void *)(sec +
			NUM_SPAACT_ENTRIES));
		out_be32(&regs->splah, pamu >> 32);
		out_be32(&regs->splal, (uint32_t)pamu);
	}
	asm volatile("sync" : : : "memory");
	return 0;
}

void pamu_enable(void)
{
	setbits_be32((void *)CONFIG_SYS_PAMU_ADDR + PAMU_PCR_OFFSET,
			PAMU_PCR_PE);
	asm volatile("sync" : : : "memory");
}

void pamu_reset(void)
{
	ccsr_pamu_t *regs = (ccsr_pamu_t *)CONFIG_SYS_PAMU_ADDR;

	/* Clear PPAACT Base register */
	out_be32(&regs->ppbah, 0);
	out_be32(&regs->ppbal, 0);
	out_be32(&regs->pplah, 0);
	out_be32(&regs->pplal, 0);
	out_be32(&regs->spbah, 0);
	out_be32(&regs->spbal, 0);
	out_be32(&regs->splah, 0);
	out_be32(&regs->splal, 0);
	clrbits_be32((void *)CONFIG_SYS_PAMU_ADDR + PAMU_PCR_OFFSET,
			PAMU_PCR_PE);
	asm volatile("sync" : : : "memory");
}

void pamu_disable(void)
{
	clrbits_be32((void *)CONFIG_SYS_PAMU_ADDR + PAMU_PCR_OFFSET,
			PAMU_PCR_PE);
	asm volatile("sync" : : : "memory");
}

static uint64_t find_max(uint64_t arr[], int num)
{
	int i = 0;
	int max = 0;
	for (i = 1 ; i < num; i++)
		if (arr[max] < arr[i])
			max = i;

	return arr[max];
}

static uint64_t find_min(uint64_t arr[], int num)
{
	int i = 0;
	int min = 0;
	for (i = 1 ; i < num; i++)
		if (arr[min] > arr[i])
			min = i;

	return arr[min];
}

static uint32_t get_win_cnt(uint64_t size)
{
	uint32_t win_cnt = DEFAULT_NUM_SUBWINDOWS;

	while (win_cnt && (size/win_cnt) < PAMU_PAGE_SIZE)
		win_cnt >>= 1;

	return win_cnt;
}

int config_pamu(struct pamu_addr_tbl *tbl, int num_entries, uint32_t liodn)
{
	int i = 0;
	int ret = 0;
	uint32_t num_sec_windows = 0;
	uint32_t num_windows = 0;
	uint64_t min_addr, max_addr;
	uint64_t size;
	uint64_t subwin_size;
	int sizebit;

	min_addr = find_min(tbl->start_addr, num_entries);
	max_addr = find_max(tbl->end_addr, num_entries);
	size = max_addr - min_addr + 1;

	if (!size)
		return -1;

	sizebit = __ilog2_roundup_64(size);
	size = 1 << sizebit;
#ifdef DEBUG
	printf("min start_addr is %llx\n", min_addr);
	printf("max end_addr is %llx\n", max_addr);
	printf("size found is  %llx\n", size);
#endif

	if (size < PAMU_PAGE_SIZE)
		size = PAMU_PAGE_SIZE;

	while (1) {
		min_addr = min_addr & ~(size - 1);
		if (min_addr + size > max_addr)
			break;
		size <<= 1;
		if (!size)
			return -1;
	}
#ifdef DEBUG
	printf("PAACT :Base addr is %llx\n", min_addr);
	printf("PAACT : Size is %llx\n", size);
#endif
	num_windows = get_win_cnt(size);
	/* For a single window, no spaact entries are required
	 * sec_sub_window count = 0 */
	if (num_windows > 1)
		num_sec_windows = num_windows;
	else
		num_sec_windows = 0;

	ret = pamu_config_ppaace(liodn, min_addr,
			size , -1, -1, -1, num_sec_windows);

	if (ret < 0)
		return ret;

#ifdef DEBUG
	printf("configured ppace\n");
#endif

	if (num_sec_windows) {
		subwin_size = size >> count_lsb_zeroes(num_sec_windows);
#ifdef DEBUG
		printf("subwin_size is %llx\n", subwin_size);
#endif

		for (i = 0; i < num_entries; i++) {
			ret = pamu_config_spaace(liodn,
				subwin_size, tbl->start_addr[i] - min_addr,
				tbl->size[i], -1, -1, -1);

			if (ret < 0)
				return ret;
		}
	}
	return ret;
}
