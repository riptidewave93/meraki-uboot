/*
 * Meraki specific commands, Kevin Paul Herbert
 *
 * Copyright (C) 2011 Meraki, Inc.
 */

#include <common.h>
#include <sha1.h>
#include <asm/immap_85xx.h>

DECLARE_GLOBAL_DATA_PTR;

/* SHA-1 checksum functions */

#define SHA1_CHECKSUM_MAGIC 0x8e73ed8a

struct sha1_checksum_data {
    uint32_t magic;
    int32_t header_len;
    int32_t data_len;
    unsigned char data_sha1[20];
};

static void print_sha1 (unsigned char *sum)
{
    int i;

    for (i = 0; i < 20; i++)
      printf("%02x", sum[i]);
}

static int check_partition_sha1 (uint32_t data)
{
    struct sha1_checksum_data *header = (struct sha1_checksum_data *)data;
    uint32_t image_start;
    uint32_t image_end;
    unsigned char calculated_sha1[20];

    if (header->magic != SHA1_CHECKSUM_MAGIC) {
        printf("Unexpected magic number %x\n", header->magic);
        return 1;
    }

    image_start = data + header->header_len;
    image_end = image_start + header->data_len;

    if ((image_start < gd->bd->bi_memstart) ||
	(image_start >= (gd->bd->bi_memstart + gd->bd->bi_memsize))) {
      printf("Image start %08x is outside of memory\n", image_start);
        return 1;
    }

    if ((image_end < gd->bd->bi_memstart) ||
	(image_end >= (gd->bd->bi_memstart + gd->bd->bi_memsize))) {
        printf("Image end %08x is outside of memory\n", image_end);
        return 1;
    }

    sha1_csum((unsigned char *)image_start, header->data_len, calculated_sha1);

    if (memcmp(header->data_sha1, calculated_sha1, 20) != 0) {

      printf("Checksum mismatch, expected sha1 = ");
      print_sha1(header->data_sha1);
      printf(" calcluated sha1 = ");
      print_sha1(calculated_sha1);
      printf("\n");

      return 1;
    }

    return 0;
}

int do_meraki(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    char *cmd;
    uint32_t addr;
    char *endptr;

    /* For now, only command is meraki checkpart addr */
    if (argc == 2 && !strcmp("rst_sw", argv[1])) {
        ccsr_gpio_t *pgpio = (void *)(CONFIG_SYS_MPC85xx_GPIO_ADDR);
        return pgpio->gpdat & 0x8;
    }

    if (argc != 3) {
      cmd_usage(cmdtp);
      return 1;
    }

    cmd = argv[1];

    if (strcmp(cmd, "checkpart") != 0)
      goto usage;

    addr = simple_strtoul(argv[2], &endptr, 16);
    if (endptr[0] != 0)
      goto usage;

    return check_partition_sha1(addr);

usage:

    cmd_usage(cmdtp);
    return 1;
}

U_BOOT_CMD(meraki, CONFIG_SYS_MAXARGS, 1, do_meraki,
	   "Meraki sub-system",
	   "checkpart addr - Check the validity of a loaded image");
