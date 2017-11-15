/*
 * (C) Copyright 2000-2009
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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


/*
 * Boot support
 */
#include <common.h>
#include <watchdog.h>
#include <command.h>
#include <image.h>
#include <malloc.h>
#include <u-boot/zlib.h>
#include <bzlib.h>
#include <environment.h>
#include <lmb.h>
#include <linux/ctype.h>
#include <asm/byteorder.h>

DECLARE_GLOBAL_DATA_PTR;

extern int gunzip (void *dst, int dstlen, unsigned char *src, unsigned long *lenp);
#ifndef CONFIG_SYS_BOOTM_LEN
#define CONFIG_SYS_BOOTM_LEN	0x800000	/* use 8MByte as default max gunzip size */
#endif

#ifdef CONFIG_BZIP2
extern void bz_internal_error(int);
#endif

#if defined(CONFIG_CMD_IMI)
//static int image_info (unsigned long addr);
#endif

#if defined(CONFIG_CMD_IMLS)
#include <flash.h>
extern flash_info_t flash_info[]; /* info for FLASH chips */
static int do_imls (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
#endif

#ifdef CONFIG_SILENT_CONSOLE
static void fixup_silent_linux (void);
#endif

static image_header_t *image_get_kernel (ulong img_addr, int verify);
#if defined(CONFIG_FIT)
static int fit_check_kernel (const void *fit, int os_noffset, int verify);
#endif

static void *boot_get_kernel (cmd_tbl_t *cmdtp, int flag,int argc, char *argv[],
		bootm_headers_t *images, ulong *os_data, ulong *os_len);
extern int do_reset (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);

/*
 *  Continue booting an OS image; caller already has:
 *  - copied image header to global variable `header'
 *  - checked header magic number, checksums (both header & image),
 *  - verified image architecture (PPC) and type (KERNEL or MULTI),
 *  - loaded (first part of) image to header load address,
 *  - disabled interrupts.
 */
typedef int boot_os_fn (int flag, int argc, char *argv[],
			bootm_headers_t *images); /* pointers to os/initrd/fdt */

#define CONFIG_BOOTM_LINUX 1
#define CONFIG_BOOTM_NETBSD 1
#define CONFIG_BOOTM_RTEMS 1

#ifdef CONFIG_BOOTM_LINUX
extern boot_os_fn do_bootm_linux;
#endif
#if defined(CONFIG_LYNXKDI)
static boot_os_fn do_bootm_lynxkdi;
extern void lynxkdi_boot (image_header_t *);
#endif
#if defined(CONFIG_INTEGRITY)
static boot_os_fn do_bootm_integrity;
#endif

extern ulong load_addr;
static bootm_headers_t images;		/* pointers to os/initrd/fdt images */

extern void __board_lmb_reserve(struct lmb *lmb);

//void board_lmb_reserve(struct lmb *lmb) __attribute__((weak, alias("__board_lmb_reserve")));

extern void __arch_lmb_reserve(struct lmb *lmb);

//void arch_lmb_reserve(struct lmb *lmb) __attribute__((weak, alias("__arch_lmb_reserve")));

#if defined(__ARM__)
  #define IH_INITRD_ARCH IH_ARCH_ARM
#elif defined(__avr32__)
  #define IH_INITRD_ARCH IH_ARCH_AVR32
#elif defined(__bfin__)
  #define IH_INITRD_ARCH IH_ARCH_BLACKFIN
#elif defined(__I386__)
  #define IH_INITRD_ARCH IH_ARCH_I386
#elif defined(__M68K__)
  #define IH_INITRD_ARCH IH_ARCH_M68K
#elif defined(__microblaze__)
  #define IH_INITRD_ARCH IH_ARCH_MICROBLAZE
#elif defined(__mips__)
  #define IH_INITRD_ARCH IH_ARCH_MIPS
#elif defined(__nios__)
  #define IH_INITRD_ARCH IH_ARCH_NIOS
#elif defined(__nios2__)
  #define IH_INITRD_ARCH IH_ARCH_NIOS2
#elif defined(__PPC__)
  #define IH_INITRD_ARCH IH_ARCH_PPC
#elif defined(__sh__)
  #define IH_INITRD_ARCH IH_ARCH_SH
#elif defined(__sparc__)
  #define IH_INITRD_ARCH IH_ARCH_SPARC
#else
# error Unknown CPU type
#endif

static int bootm_start(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	ulong		mem_start;
	phys_size_t	mem_size;
	void		*os_hdr;
	int		ret;

	memset ((void *)&images, 0, sizeof (images));
	images.verify = getenv_yesno ("verify");

	lmb_init(&images.lmb);

	mem_start = getenv_bootm_low();
	mem_size = getenv_bootm_size();

	lmb_add(&images.lmb, (phys_addr_t)mem_start, mem_size);

	__arch_lmb_reserve(&images.lmb);
	__board_lmb_reserve(&images.lmb);

	/* get kernel image header, start address and length */
	os_hdr = boot_get_kernel (cmdtp, flag, argc, argv,
			&images, &images.os.image_start, &images.os.image_len);
	if (images.os.image_len == 0) {
		puts ("ERROR: can't get kernel image!\n");
		return 1;
	}

	/* get image parameters */
	switch (genimg_get_format (os_hdr)) {
	case IMAGE_FORMAT_LEGACY:
		images.os.type = image_get_type (os_hdr);
		images.os.comp = image_get_comp (os_hdr);
		images.os.os = image_get_os (os_hdr);

		images.os.end = image_get_image_end (os_hdr);
		images.os.load = image_get_load (os_hdr);
		break;
#if defined(CONFIG_FIT)
	case IMAGE_FORMAT_FIT:
		if (fit_image_get_type (images.fit_hdr_os,
					images.fit_noffset_os, &images.os.type)) {
			puts ("Can't get image type!\n");
			show_boot_progress (-109);
			return 1;
		}

		if (fit_image_get_comp (images.fit_hdr_os,
					images.fit_noffset_os, &images.os.comp)) {
			puts ("Can't get image compression!\n");
			show_boot_progress (-110);
			return 1;
		}

		if (fit_image_get_os (images.fit_hdr_os,
					images.fit_noffset_os, &images.os.os)) {
			puts ("Can't get image OS!\n");
			show_boot_progress (-111);
			return 1;
		}

		images.os.end = fit_get_end (images.fit_hdr_os);

		if (fit_image_get_load (images.fit_hdr_os, images.fit_noffset_os,
					&images.os.load)) {
			puts ("Can't get image load address!\n");
			show_boot_progress (-112);
			return 1;
		}
		break;
#endif
	default:
		puts ("ERROR: unknown image format type!\n");
		return 1;
	}

	/* find kernel entry point */
	if (images.legacy_hdr_valid) {
		images.ep = image_get_ep (&images.legacy_hdr_os_copy);
#if defined(CONFIG_FIT)
	} else if (images.fit_uname_os) {
		ret = fit_image_get_entry (images.fit_hdr_os,
				images.fit_noffset_os, &images.ep);
		if (ret) {
			puts ("Can't get entry point property!\n");
			return 1;
		}
#endif
	} else {
		puts ("Could not find kernel entry point!\n");
		return 1;
	}

	if (images.os.os == IH_OS_LINUX) {
		/* find ramdisk */
		ret = boot_get_ramdisk (argc, argv, &images, IH_INITRD_ARCH,
				&images.rd_start, &images.rd_end);
		if (ret) {
			puts ("Ramdisk image is corrupt or invalid\n");
			return 1;
		}

#if defined(CONFIG_OF_LIBFDT)
#if defined(CONFIG_PPC) || defined(CONFIG_M68K) || defined(CONFIG_SPARC)
		/* find flattened device tree */
		ret = boot_get_fdt (flag, argc, argv, &images,
				    &images.ft_addr, &images.ft_len);
		if (ret) {
			puts ("Could not find a valid device tree\n");
			return 1;
		}

		//set_working_fdt_addr(images.ft_addr);
#endif
#endif
	}

	images.os.start = (ulong)os_hdr;
	images.state = BOOTM_STATE_START;

	return 0;
}

#define BOOTM_ERR_RESET		-1
#define BOOTM_ERR_OVERLAP	-2
#define BOOTM_ERR_UNIMPLEMENTED	-3
static int bootm_load_os(image_info_t os, ulong *load_end, int boot_progress)
{
	uint8_t comp = os.comp;
	ulong load = os.load;
	ulong blob_start = os.start;
//	ulong blob_end = os.end;
	ulong image_start = os.image_start;
	ulong image_len = os.image_len;
	uint unc_len = CONFIG_SYS_BOOTM_LEN;

	const char *type_name = genimg_get_type_name (os.type);

	switch (comp) {
	case IH_COMP_NONE:
		if (load == blob_start) {
			printf ("   XIP %s ... ", type_name);
		} else {
			printf ("   Loading %s ... ", type_name);

			if (load != image_start) {
				memmove_wd ((void *)load,
						(void *)image_start, image_len, CHUNKSZ);
			}
		}
		*load_end = load + image_len;
		puts("OK\n");
		break;
	case IH_COMP_GZIP:
		printf ("   Uncompressing %s ... ", type_name);
		if (gunzip ((void *)load, unc_len,
					(uchar *)image_start, &image_len) != 0) {
			puts ("GUNZIP: uncompress, out-of-mem or overwrite error "
				"- must RESET board to recover\n");
			if (boot_progress)
				show_boot_progress (-6);
			return BOOTM_ERR_RESET;
		}

		*load_end = load + image_len;
		break;
#ifdef CONFIG_BZIP2
	case IH_COMP_BZIP2:
		printf ("   Uncompressing %s ... ", type_name);
		/*
		 * If we've got less than 4 MB of malloc() space,
		 * use slower decompression algorithm which requires
		 * at most 2300 KB of memory.
		 */
		int i = BZ2_bzBuffToBuffDecompress ((char*)load,
					&unc_len, (char *)image_start, image_len,
					CONFIG_SYS_MALLOC_LEN < (4096 * 1024), 0);
		if (i != BZ_OK) {
			printf ("BUNZIP2: uncompress or overwrite error %d "
				"- must RESET board to recover\n", i);
			if (boot_progress)
				show_boot_progress (-6);
			return BOOTM_ERR_RESET;
		}

		*load_end = load + unc_len;
		break;
#endif /* CONFIG_BZIP2 */
	default:
		printf ("Unimplemented compression type %d\n", comp);
		return BOOTM_ERR_UNIMPLEMENTED;
	}
	printf("BSD kernel is loaded at Phy addr 0x%08lx to 0x%08lx\n", load, *load_end);
	//printf("FreeBSD kernel is loaded at Phy addr 0x%08lx to 0x%08lx\n", load, *load_end);

	return 0;
}

#if 0
static int bootm_start_standalone(ulong iflag, int argc, char *argv[])
{
	char  *s;
	int   (*appl)(int, char *[]);

	/* Don't start if "autostart" is set to "no" */
	if (((s = getenv("autostart")) != NULL) && (strcmp(s, "no") == 0)) {
		char buf[32];
		sprintf(buf, "%lX", images.os.image_len);
		setenv("filesize", buf);
		return 0;
	}
	appl = (int (*)(int, char *[]))ntohl(images.ep);
	(*appl)(argc-1, &argv[1]);

	return 0;
}
#endif

/*******************************************************************/
/* bootm - boot application image from image in memory */
/*******************************************************************/
//static int relocated = 0;

int do_bsdboot (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	ulong		iflag;
	ulong		load_end = 0;
	int		ret;
//	boot_os_fn	*boot_fn;

	if (bootm_start(cmdtp, flag, argc, argv))
		return 1;

	/*
	 * We have reached the point of no return: we are going to
	 * overwrite all exception vector code, so we cannot easily
	 * recover from any failures any more...
	 */
	iflag = disable_interrupts();

	ret = bootm_load_os(images.os, &load_end, 1);

	if (ret < 0) {
		if (ret == BOOTM_ERR_RESET)
			do_reset (cmdtp, flag, argc, argv);
		if (ret == BOOTM_ERR_OVERLAP) {
			if (images.legacy_hdr_valid) {
				if (image_get_type (&images.legacy_hdr_os_copy) == IH_TYPE_MULTI)
					puts ("WARNING: legacy format multi component "
						"image overwritten\n");
			} else {
				puts ("ERROR: new format image overwritten - "
					"must RESET the board to recover\n");
				show_boot_progress (-113);
				do_reset (cmdtp, flag, argc, argv);
			}
		}
		if (ret == BOOTM_ERR_UNIMPLEMENTED) {
			if (iflag)
				enable_interrupts();
			show_boot_progress (-7);
			return 1;
		}
	}

	__asm__ volatile  (
		"ba 0x00000000\n\t"
	);

	//enable_interrupts();

	return 0;
}

/**
 * image_get_kernel - verify legacy format kernel image
 * @img_addr: in RAM address of the legacy format image to be verified
 * @verify: data CRC verification flag
 *
 * image_get_kernel() verifies legacy image integrity and returns pointer to
 * legacy image header if image verification was completed successfully.
 *
 * returns:
 *     pointer to a legacy image header if valid image was found
 *     otherwise return NULL
 */
static image_header_t *image_get_kernel (ulong img_addr, int verify)
{
	image_header_t *hdr = (image_header_t *)img_addr;

	if (!image_check_magic(hdr)) {
		puts ("Bad Magic Number\n");
		show_boot_progress (-1);
		return NULL;
	}
	show_boot_progress (2);

	if (!image_check_hcrc (hdr)) {
		puts ("Bad Header Checksum\n");
		show_boot_progress (-2);
		return NULL;
	}

	show_boot_progress (3);
	image_print_contents (hdr);

	if (verify) {
		puts ("   Verifying Checksum ... ");
		if (!image_check_dcrc (hdr)) {
			printf ("Bad Data CRC\n");
			show_boot_progress (-3);
			return NULL;
		}
		puts ("OK\n");
	}
	show_boot_progress (4);

	if (!image_check_target_arch (hdr)) {
		printf ("Unsupported Architecture 0x%x\n", image_get_arch (hdr));
		show_boot_progress (-4);
		return NULL;
	}
	return hdr;
}

/**
 * fit_check_kernel - verify FIT format kernel subimage
 * @fit_hdr: pointer to the FIT image header
 * os_noffset: kernel subimage node offset within FIT image
 * @verify: data CRC verification flag
 *
 * fit_check_kernel() verifies integrity of the kernel subimage and from
 * specified FIT image.
 *
 * returns:
 *     1, on success
 *     0, on failure
 */
#if defined (CONFIG_FIT)
static int fit_check_kernel (const void *fit, int os_noffset, int verify)
{
	fit_image_print (fit, os_noffset, "   ");

	if (verify) {
		puts ("   Verifying Hash Integrity ... ");
		if (!fit_image_check_hashes (fit, os_noffset)) {
			puts ("Bad Data Hash\n");
			show_boot_progress (-104);
			return 0;
		}
		puts ("OK\n");
	}
	show_boot_progress (105);

	if (!fit_image_check_target_arch (fit, os_noffset)) {
		puts ("Unsupported Architecture\n");
		show_boot_progress (-105);
		return 0;
	}

	show_boot_progress (106);
	if (!fit_image_check_type (fit, os_noffset, IH_TYPE_KERNEL)) {
		puts ("Not a kernel image\n");
		show_boot_progress (-106);
		return 0;
	}

	show_boot_progress (107);
	return 1;
}
#endif /* CONFIG_FIT */

/**
 * boot_get_kernel - find kernel image
 * @os_data: pointer to a ulong variable, will hold os data start address
 * @os_len: pointer to a ulong variable, will hold os data length
 *
 * boot_get_kernel() tries to find a kernel image, verifies its integrity
 * and locates kernel data.
 *
 * returns:
 *     pointer to image header if valid image was found, plus kernel start
 *     address and length, otherwise NULL
 */
static void *boot_get_kernel (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[],
		bootm_headers_t *images, ulong *os_data, ulong *os_len)
{
	image_header_t	*hdr;
	ulong		img_addr;
#if defined(CONFIG_FIT)
	void		*fit_hdr;
	const char	*fit_uname_config = NULL;
	const char	*fit_uname_kernel = NULL;
	const void	*data;
	size_t		len;
	int		cfg_noffset;
	int		os_noffset;
#endif

	/* find out kernel image address */
	if (argc < 2) {
		img_addr = load_addr;
		debug ("*  kernel: default image load address = 0x%08lx\n",
				load_addr);
#if defined(CONFIG_FIT)
	} else if (fit_parse_conf (argv[1], load_addr, &img_addr,
							&fit_uname_config)) {
		debug ("*  kernel: config '%s' from image at 0x%08lx\n",
				fit_uname_config, img_addr);
	} else if (fit_parse_subimage (argv[1], load_addr, &img_addr,
							&fit_uname_kernel)) {
		debug ("*  kernel: subimage '%s' from image at 0x%08lx\n",
				fit_uname_kernel, img_addr);
#endif
	} else {
		img_addr = simple_strtoul(argv[1], NULL, 16);
		debug ("*  kernel: cmdline image address = 0x%08lx\n", img_addr);
	}

	show_boot_progress (1);

	/* copy from dataflash if needed */
	img_addr = genimg_get_image (img_addr);

	/* check image type, for FIT images get FIT kernel node */
	*os_data = *os_len = 0;
	switch (genimg_get_format ((void *)img_addr)) {
	case IMAGE_FORMAT_LEGACY:
		printf ("## Booting kernel from Legacy Image at %08lx ...\n",
				img_addr);
		hdr = image_get_kernel (img_addr, images->verify);
		if (!hdr)
			return NULL;
		show_boot_progress (5);

		/* get os_data and os_len */
		switch (image_get_type (hdr)) {
		case IH_TYPE_KERNEL:
			*os_data = image_get_data (hdr);
			*os_len = image_get_data_size (hdr);
			break;
		case IH_TYPE_MULTI:
			image_multi_getimg (hdr, 0, os_data, os_len);
			break;
		case IH_TYPE_STANDALONE:
			if (argc >2) {
				hdr->ih_load = htonl(simple_strtoul(argv[2], NULL, 16));
			}
			*os_data = image_get_data (hdr);
			*os_len = image_get_data_size (hdr);
			break;
		default:
			printf ("Wrong Image Type for %s command\n", cmdtp->name);
			show_boot_progress (-5);
			return NULL;
		}

		/*
		 * copy image header to allow for image overwrites during kernel
		 * decompression.
		 */
		memmove (&images->legacy_hdr_os_copy, hdr, sizeof(image_header_t));

		/* save pointer to image header */
		images->legacy_hdr_os = hdr;

		images->legacy_hdr_valid = 1;
		show_boot_progress (6);
		break;
#if defined(CONFIG_FIT)
	case IMAGE_FORMAT_FIT:
		fit_hdr = (void *)img_addr;
		printf ("## Booting kernel from FIT Image at %08lx ...\n",
				img_addr);

		if (!fit_check_format (fit_hdr)) {
			puts ("Bad FIT kernel image format!\n");
			show_boot_progress (-100);
			return NULL;
		}
		show_boot_progress (100);

		if (!fit_uname_kernel) {
			/*
			 * no kernel image node unit name, try to get config
			 * node first. If config unit node name is NULL
			 * fit_conf_get_node() will try to find default config node
			 */
			show_boot_progress (101);
			cfg_noffset = fit_conf_get_node (fit_hdr, fit_uname_config);
			if (cfg_noffset < 0) {
				show_boot_progress (-101);
				return NULL;
			}
			/* save configuration uname provided in the first
			 * bootm argument
			 */
			images->fit_uname_cfg = fdt_get_name (fit_hdr, cfg_noffset, NULL);
			printf ("   Using '%s' configuration\n", images->fit_uname_cfg);
			show_boot_progress (103);

			os_noffset = fit_conf_get_kernel_node (fit_hdr, cfg_noffset);
			fit_uname_kernel = fit_get_name (fit_hdr, os_noffset, NULL);
		} else {
			/* get kernel component image node offset */
			show_boot_progress (102);
			os_noffset = fit_image_get_node (fit_hdr, fit_uname_kernel);
		}
		if (os_noffset < 0) {
			show_boot_progress (-103);
			return NULL;
		}

		printf ("   Trying '%s' kernel subimage\n", fit_uname_kernel);

		show_boot_progress (104);
		if (!fit_check_kernel (fit_hdr, os_noffset, images->verify))
			return NULL;

		/* get kernel image data address and length */
		if (fit_image_get_data (fit_hdr, os_noffset, &data, &len)) {
			puts ("Could not find kernel subimage data!\n");
			show_boot_progress (-107);
			return NULL;
		}
		show_boot_progress (108);

		*os_len = len;
		*os_data = (ulong)data;
		images->fit_hdr_os = fit_hdr;
		images->fit_uname_os = fit_uname_kernel;
		images->fit_noffset_os = os_noffset;
		break;
#endif
	default:
		printf ("Wrong Image Format for %s command\n", cmdtp->name);
		show_boot_progress (-108);
		return NULL;
	}

	debug ("   kernel data at 0x%08lx, len = 0x%08lx (%ld)\n",
			*os_data, *os_len, *os_len);

	return (void *)img_addr;
}

U_BOOT_CMD(
	bootbsd,	CONFIG_SYS_MAXARGS,	1,	do_bsdboot,
	"boot application image from memory",
);

