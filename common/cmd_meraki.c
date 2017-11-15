/*
 * Meraki specific commands, Kevin Paul Herbert
 *
 * Copyright (C) 2011 Meraki, Inc.
 */

#include <common.h>
#include <sha1.h>

DECLARE_GLOBAL_DATA_PTR;
#define MERAKI_EXTRA_MAGIC  0xa1f0beef

enum meraki_extra_types {
    MERAKI_OFFSETS = 1,
};

struct meraki_extra {
    uint32_t magic;  //always MERAKI_EXTRA_MAGIC
    uint16_t length; //number of uint32s this extra part contains
    uint16_t type;   //enum meraki_extras
};

struct meraki_offsets {
    struct meraki_extra me_hdr;
    uint32_t image_off;
    uint32_t ramdisk_off;
    uint32_t fdt_offs[0];
};

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

static void handle_offsets(uint32_t data, struct meraki_offsets *offs)
{
    uint32_t image_off;
    uint32_t ramdisk_off;
    uint32_t fdt_off = -1;
    uint32_t fdt_index = -1;

    image_off = offs->image_off;
    ramdisk_off = offs->ramdisk_off;

    char *fdt_index_char = getenv("meraki_part_fdt_index");
    if (fdt_index_char) {
        fdt_index = simple_strtoul(fdt_index_char, NULL, 0);
    } else {
        fdt_index = 0;
        printf("no meraki_part_fdt_index env variable, using default of %d\n", fdt_index);
    }

    if (((uint8_t*)&offs->fdt_offs[fdt_index]) < (((uint8_t*)offs) + (offs->me_hdr.length << 2))) {
        fdt_off = offs->fdt_offs[fdt_index];
    } else {
        printf("fdt index %d does not exist in this part, not setting environment\n", fdt_index);
        return;
    }

    printf("setting meraki addr env fdt: 0x%x, kernel: 0x%x, ramdisk: 0x%x\n", (data + fdt_off), (data + image_off), (data + ramdisk_off));

    char numbuf[32];
    sprintf(numbuf, "0x%x", data + fdt_off);
    setenv("meraki_loadaddr_fdt", numbuf);
    sprintf(numbuf, "0x%x", data + image_off);
    setenv("meraki_loadaddr_kernel", numbuf);
    sprintf(numbuf, "0x%x", data + ramdisk_off);
    setenv("meraki_loadaddr_ramdisk", numbuf);
}

static int check_partition_sha1 (uint32_t data, int skip_hash)
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

    if (!skip_hash) {
        sha1_csum((unsigned char *)image_start, header->data_len, calculated_sha1);

        if (memcmp(header->data_sha1, calculated_sha1, 20) != 0) {

            printf("Checksum mismatch, expected sha1 = ");
            print_sha1(header->data_sha1);
            printf(" calcluated sha1 = ");
            print_sha1(calculated_sha1);
            printf("\n");

            return 1;
        }
    }

    //loop over all the extra data structures
    struct meraki_extra *me;
    for (me = (struct meraki_extra*)(header+1);
         me->magic == MERAKI_EXTRA_MAGIC;
         me = (struct meraki_extra*)(((uint8_t*)me) + (me->length << 2))) {

        switch(me->type) {
            case MERAKI_OFFSETS:
                handle_offsets(data, (struct meraki_offsets*)me);
                break;
            default:
                printf("Unrecognized meraki extra type: %d\n", me->type);
        }
    }

    return 0;
}

int do_meraki(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    char *cmd;
    uint32_t addr;
    int skip_hash = 0;
    char *endptr;

    /* For now, only command is meraki checkpart addr */

    if (argc < 3 || argc > 4) {
      cmd_usage(cmdtp);
      return 1;
    }

    cmd = argv[1];

    if (strcmp(cmd, "checkpart") != 0)
      goto usage;

    addr = simple_strtoul(argv[2], &endptr, 16);
    if (endptr[0] != 0)
      goto usage;

    if (argc == 4 && (strcmp(argv[3], "skiphash") == 0))
        skip_hash = 1;

    return check_partition_sha1(addr, skip_hash);

usage:

    cmd_usage(cmdtp);
    return 1;
}

U_BOOT_CMD(meraki, CONFIG_SYS_MAXARGS, 1, do_meraki,
	   "Meraki sub-system",
	   "checkpart addr [skiphash] - Check the validity of a loaded image and set the meraki_loadaddr_kernel, meraki_loadaddr_ramdisk, and meraki_loadaddr_fdt env variables.  If skiphash is specified, don't bother verifying, just set the environment variables");
