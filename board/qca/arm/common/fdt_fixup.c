/*
 * Copyright (c) 2015-2017 The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <common.h>
#include <asm/arch-qca-common/smem.h>
#include <asm/arch-qca-common/scm.h>
#include <jffs2/load_kernel.h>
#include <fdtdec.h>

DECLARE_GLOBAL_DATA_PTR;

/*
 * Don't have this as a '.bss' variable. The '.bss' and '.rel.dyn'
 * sections seem to overlap.
 *
 * $ arm-none-linux-gnueabi-objdump -h u-boot
 * . . .
 *  8 .rel.dyn      00004ba8  40630b0c  40630b0c  00038b0c  2**2
 *                  CONTENTS, ALLOC, LOAD, READONLY, DATA
 *  9 .bss          0000559c  40630b0c  40630b0c  00000000  2**3
 *                  ALLOC
 * . . .
 *
 * board_early_init_f() initializes this variable, resulting in one
 * of the relocation entries present in '.rel.dyn' section getting
 * corrupted. Hence, when relocate_code()'s 'fixrel' executes, it
 * patches a wrong address, which incorrectly modifies some global
 * variable resulting in a crash.
 *
 * Moral of the story: Global variables that are written before
 * relocate_code() gets executed cannot be in '.bss'
 */

#ifdef CONFIG_OF_BOARD_SETUP

extern int fdt_node_set_part_info(void *blob, int parent_offset,
                                  struct mtd_device *dev);

struct flash_node_info {
	const char *compat;	/* compatible string */
	int type;		/* mtd flash type */
	int idx;		/* flash index */
};

int ipq_fdt_fixup_spi_nor_params(void *blob)
{
	int nodeoff, ret;
	qca_smem_flash_info_t sfi;
	uint32_t val;

	/* Get flash parameters from smem */
	smem_get_boot_flash(&sfi.flash_type,
				&sfi.flash_index,
				&sfi.flash_chip_select,
				&sfi.flash_block_size,
				&sfi.flash_density);
	nodeoff = fdt_node_offset_by_compatible(blob, -1, "n25q128a11");

	if (nodeoff < 0) {
		printf("fdt-fixup: unable to find compatible node\n");
		return nodeoff;
	}

	val = cpu_to_fdt32(sfi.flash_block_size);
	ret = fdt_setprop(blob, nodeoff, "sector-size",
			&val, sizeof(uint32_t));
	if (ret) {
		printf("fdt-fixup: unable to set sector size(%d)\n", ret);
		return -1;
	}

	if (sfi.flash_density != 0) {
		val = cpu_to_fdt32(sfi.flash_density);
		ret = fdt_setprop(blob, nodeoff, "density",
				&val, sizeof(uint32_t));
		if (ret) {
			printf("fdt-fixup: unable to set density(%d)\n", ret);
			return -1;
		}
	}

	return 0;
}

void ipq_fdt_fixup_version(void *blob)
{
	int nodeoff, ret;
	char ver[OEM_VERSION_STRING_LENGTH + VERSION_STRING_LENGTH + 1];

	nodeoff = fdt_path_offset(blob, "/");

	if (nodeoff < 0) {
		debug("fdt-fixup: fdt fixup unable to find root node\n");
		return;
	}

	if (!smem_get_build_version(ver, sizeof(ver), BOOT_VERSION)) {
		debug("BOOT Build Version:  %s\n", ver);
		ret = fdt_setprop(blob, nodeoff, "boot_version",
				ver, strlen(ver));
		if (ret)
			debug("fdt-fixup: unable to set Boot version(%d)\n", ret);
	}

	if (!smem_get_build_version(ver, sizeof(ver), TZ_VERSION)) {
		debug("TZ Build Version:  %s\n", ver);
		ret = fdt_setprop(blob, nodeoff, "tz_version",
				ver, strlen(ver));
		if (ret)
			debug("fdt-fixup: unable to set TZ version(%d)\n", ret);
	}

#ifdef RPM_VERSION
	if (!smem_get_build_version(ver, sizeof(ver), RPM_VERSION)) {
		debug("RPM Build Version:  %s\n", ver);
		ret = fdt_setprop(blob, nodeoff, "rpm_version",
				ver, strlen(ver));
		if (ret)
			debug("fdt-fixup: unable to set RPM version(%d)\n", ret);
	}
#endif /* RPM_VERSION */
}

void ipq_fdt_fixup_mtdparts(void *blob, struct flash_node_info *ni)
{
	struct mtd_device *dev;
	char *parts;
	int noff;

	parts = getenv("mtdparts");
	if (!parts)
		return;

	if (mtdparts_init() != 0)
		return;

	for (; ni->compat; ni++) {
		noff = fdt_node_offset_by_compatible(blob, -1, ni->compat);
		while (noff != -FDT_ERR_NOTFOUND) {
			dev = device_find(ni->type, ni->idx);
			if (dev) {
				if (fdt_node_set_part_info(blob, noff, dev))
					return; /* return on error */
			}

			/* Jump to next flash node */
			noff = fdt_node_offset_by_compatible(blob, noff,
							     ni->compat);
		}
	}
}

void ipq_fdt_mem_rsvd_fixup(void *blob)
{
	u32 dload;
	int parentoff, nodeoff, ret, i;
	dload = htonl(DLOAD_DISABLE);

	/* Reserve only the TZ and SMEM memory region and free the rest */
	parentoff = fdt_path_offset(blob, rsvd_node);
	if (parentoff >= 0) {
		for (i = 0; del_node[i]; i++) {
			nodeoff = fdt_subnode_offset(blob, parentoff,
						     del_node[i]);
			if (nodeoff < 0) {
				debug("fdt-fixup: unable to findnode (%s)\n",
					del_node[i]);
				continue;
			}
			ret = fdt_del_node(blob, nodeoff);
			if (ret != 0)
				debug("fdt-fixup: unable to delete node (%s)\n",
					del_node[i]);
		}

		for (i = 0; add_node[i].nodename; i++) {
			nodeoff = fdt_add_subnode(blob, parentoff,
						  add_node[i].nodename);
			if (nodeoff < 0) {
				debug("fdt-fixup: unable to add subnode (%s)\n",
					add_node[i].nodename);
				continue;
			}
			ret = fdt_setprop(blob, nodeoff, "no-map", NULL, 0);
			if (ret != 0)
				debug("fdt-fixup: unable to set property\n");

			ret = fdt_setprop(blob, nodeoff, "reg",add_node[i].val,
					  sizeof(add_node[i].val));
			if (ret != 0)
				debug("fdt-fixup: unable to set property\n");
		}
	} else {
		debug("fdt-fixup: unable to find node \n");
	}

	/* Set the dload_status to DLOAD_DISABLE */
	nodeoff = fdt_path_offset(blob, "/soc/qca,scm_restart_reason");
	if (nodeoff < 0) {
		debug("fdt-fixup: unable to find compatible node\n");
		return;
	}
	ret = fdt_setprop(blob, nodeoff, "dload_status", &dload, sizeof(dload));
	if (ret != 0) {
		debug("fdt-fixup: unable to find compatible node\n");
		return;
	}
}

/*
 * For newer kernel that boot with device tree (3.14+), all of memory is
 * described in the /memory node, including areas that the kernel should not be
 * touching.
 *
 * By default, u-boot will walk the dram bank info and populate the /memory
 * node; here, overwrite this behavior so we describe all of memory instead.
 */
int ft_board_setup(void *blob, bd_t *bd)
{
	u64 memory_start = CONFIG_SYS_SDRAM_BASE;
	u64 memory_size = gd->ram_size;
	char *mtdparts = NULL;
	char parts_str[4096];
	qca_smem_flash_info_t *sfi = &qca_smem_flash_info;
	struct flash_node_info nodes[] = {
		{ "qcom,msm-nand", MTD_DEV_TYPE_NAND, 0 },
		{ "qcom,qcom_nand", MTD_DEV_TYPE_NAND, 0 },
		{ "qcom,ebi2-nandc-bam-v1.5.0", MTD_DEV_TYPE_NAND, 0 },
		{ "spinand,mt29f", MTD_DEV_TYPE_NAND, 1 },
		{ "n25q128a11", MTD_DEV_TYPE_NAND,
				CONFIG_IPQ_SPI_NOR_INFO_IDX },
		{ "s25fl256s1", MTD_DEV_TYPE_NAND, 1 },
		{ NULL, 0, -1 },	/* Terminator */
	};

	fdt_fixup_memory_banks(blob, &memory_start, &memory_size, 1);
	ipq_fdt_fixup_version(blob);
#ifndef CONFIG_QCA_APPSBL_DLOAD
	ipq_fdt_mem_rsvd_fixup(blob);
#endif
	if (sfi->flash_type == SMEM_BOOT_NAND_FLASH) {
		sprintf(parts_str, "mtdparts=nand0");
	} else if (sfi->flash_type == SMEM_BOOT_SPI_FLASH) {
		/* Patch NOR block size and density for
		 * generic probe case */
		ipq_fdt_fixup_spi_nor_params(blob);
		sprintf(parts_str, "mtdparts=" QCA_SPI_NOR_DEVICE);

		if (sfi->flash_secondary_type == SMEM_BOOT_NAND_FLASH)
			sprintf(parts_str,
				"mtdparts=nand0:64M@0(rootfs);nand1");
	}
	mtdparts = parts_str;
	if (mtdparts) {
		qca_smem_part_to_mtdparts(mtdparts);
		if (mtdparts[0] != '\0') {
			debug("mtdparts = %s\n", mtdparts);
			setenv("mtdparts", mtdparts);
		}

		debug("MTDIDS: %s\n", getenv("mtdids"));
		ipq_fdt_fixup_mtdparts(blob, nodes);
	}
	ipq_fdt_fixup_socinfo(blob);
	dcache_disable();
	fdt_fixup_ethernet(blob);
	ipq_fdt_fixup_usb_device_mode(blob);
	fdt_fixup_auto_restart(blob);

#ifdef CONFIG_QCA_MMC
	board_mmc_deinit();
#endif
	return 0;
}

#endif /* CONFIG_OF_BOARD_SETUP */
