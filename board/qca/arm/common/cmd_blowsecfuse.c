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
#include <command.h>
#include <image.h>
#include <nand.h>
#include <errno.h>
#include <asm/arch-qca-common/smem.h>
#include <asm/arch-qca-common/scm.h>
#include <linux/mtd/ubi.h>
#include <part.h>
#include "qca_common.h"

#if defined(CONFIG_ARCH_IPQ40xx)
#define KERNEL_AUTH_CMD			0x13
#elif defined(CONFIG_ARCH_IPQ806x)
#define KERNEL_AUTH_CMD			0x7
#endif

#define FUSEPROV_SUCCESS		0x0
#define FUSEPROV_INVALID_HASH		0x09
#define FUSEPROV_SECDAT_LOCK_BLOWN	0xB
#define TZ_BLOW_FUSE_SECDAT		0x20

int do_fuseipq(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	int ret;
	uint32_t fuse_status = 0;
	struct fuse_blow {
		uint32_t address;
		uint32_t status;
	} fuseip;

	if (argc != 2) {
		printf("No Arguments provided\n");
		printf("Command format: fuseipq <address>\n");
		return 1;
	}

	fuseip.address = simple_strtoul(argv[1], NULL, 16);
	fuseip.status = (uint32_t)&fuse_status;

	ret = scm_call(SCM_SVC_FUSE, TZ_BLOW_FUSE_SECDAT,
			&fuseip, sizeof(fuseip), NULL, 0);


	if (ret || fuse_status)
		printf("%s: Error in QFPROM write (%d, %d)\n",
			__func__, ret, fuse_status);

	if (fuse_status == FUSEPROV_SECDAT_LOCK_BLOWN)
		printf("Fuse already blown\n");
	else if (fuse_status == FUSEPROV_INVALID_HASH)
		printf("Invalid sec.dat\n");
	else if (fuse_status  != FUSEPROV_SUCCESS)
		printf("Failed to Blow fuses");
	else
		printf("Blow Success\n");

	return 0;
}

U_BOOT_CMD(fuseipq, 2, 0, do_fuseipq,
		"fuse QFPROM registers from memory\n",
		"fuseipq [address]  - Load fuse(s) and blows in the qfprom\n");

static int do_checksecboot(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int ret;
	char buf;

	ret = scm_call(SCM_SVC_FUSE, QFPROM_IS_AUTHENTICATE_CMD, NULL, 0,
		       &buf, sizeof(char));
	if (ret) {
		printf("error: %d\n", ret);
		return CMD_RET_FAILURE;
	}
	if (buf == 1)
		printf("secure boot enabled\n");
	else
		printf("secure boot not enabled\n");
	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(check_secboot, 1, 0, do_checksecboot,
	   "check secure boot enabled",
	   "check_secboot - prints if secure boot is enabled\n");

static int do_validate(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int ret;
	struct mbn {
		unsigned int addr;
		unsigned int size;
	} mbn;

	if (argc < 2)
		return CMD_RET_USAGE;

	mbn.addr = simple_strtoul(argv[1], NULL, 16);
	if (argc == 3)
		mbn.size = simple_strtoul(argv[2], NULL, 16);
	else
		mbn.size = 0;
	ret = scm_call(SCM_SVC_BOOT, KERNEL_AUTH_CMD, &mbn, sizeof(struct mbn), NULL, 0);
	if (ret) {
		printf("Validate error: %d\n", ret);
		return CMD_RET_FAILURE;
	}

	printf("Valid image\n");
	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(validate, 3, 0, do_validate,
	   "Validate signed mbn file",
	   "validate <addr> [size] - check signed mbn file @ given addr");
