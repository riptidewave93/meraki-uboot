#include <common.h>
#include <configs/iproc_board.h>
#include <iproc_gpio.h>
#include <iproc_gpio_cfg.h>
#include <search.h>

#define LOADER_MAGIC 0x4d495053

static struct tag *params;
static ulong atags_addr = 0x100;

static void setup_start_tag ()
{
   params = (struct tag *)atags_addr;;

   params->hdr.tag = ATAG_CORE;
   params->hdr.size = tag_size (tag_core);

   params->u.core.flags = 0;
   params->u.core.pagesize = 1 << 12;
   params->u.core.rootdev = 0;

   params = tag_next (params);
}

static void setup_end_tag()
{
	params->hdr.tag = ATAG_NONE;
	params->hdr.size = 0;
}

static void setup_commandline_tag(char *commandline)
{
   char *p;

   if (!commandline)
      return;

   /* eat leading white space */
   for (p = commandline; *p == ' '; p++);

   /* skip non-existent command lines so the kernel will still
    * use its default command line.
    */
   if (*p == '\0')
      return;

   params->hdr.tag = ATAG_CMDLINE;
   params->hdr.size =
      (sizeof (struct tag_header) + strlen (p) + 1 + 4) >> 2;

   strcpy (params->u.cmdline.cmdline, p);

   params = tag_next (params);
}

struct meraki_image_header {
    u_int32_t magic;    /* Every sufficiently advanced technology... */
    u_int32_t load_addr;   /* Address to load binary */
    u_int32_t size;     /* Size of binary */
    u_int32_t entry;    /* Entry point */
    u_int32_t crc;      /* CRC32 of image */
    u_int32_t reserved[3]; /* Reserved */
};

int do_bootbk (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
   ulong addr;
   ulong hdr_crc, calc_crc, length;
   struct meraki_image_header *hdr;
   char bootcmd[10];
   char cmdline[128];
   void (*kernel_entry)(int zero, int arch, uint params);
   unsigned long r2=0;
   unsigned long machid;

   /* Validate arguments */
   if (argc != 3)
      return CMD_RET_USAGE;

   addr = simple_strtoul (argv[1], NULL, 16);
   hdr = (struct meraki_image_header *)addr;

   /* Validate arguments */
   if (hdr->magic != LOADER_MAGIC) {
      puts("Bad Bootkernel magic!");
      return CMD_RET_FAILURE;
   }

   /* save crc and then zero it out */
   hdr_crc = hdr->crc;
   hdr->crc = 0;

   /* recalculate image crc */
   length = hdr->size + sizeof(struct meraki_image_header);
   calc_crc = crc32_wd (0, (const uchar *) addr, length, CHUNKSZ_CRC32);

   /* match?? */
   if (calc_crc != hdr_crc) {
      puts("Bad CRC!");
      return CMD_RET_FAILURE;
   }

   addr += sizeof(struct meraki_image_header);
   snprintf(bootcmd, sizeof(bootcmd), "go %x", addr);

   /* Pass kernel command line using ATAGS */
   setup_start_tag();
   snprintf(cmdline, sizeof(cmdline), "loader=u-boot part=%s",argv[2]);
   setup_commandline_tag(cmdline);
   setup_end_tag();

   kernel_entry = (void (*)(int, int, uint))addr;
   machid = CONFIG_MACH_TYPE;
   r2 = atags_addr;
   kernel_entry(0, machid, r2);

   run_command (bootcmd, 0);

   return CMD_RET_SUCCESS;
}

U_BOOT_CMD(
	bootbk, 3, 0, do_bootbk,
	"Boot Meraki bootkernel",
	"bootbk <address> <partition-name>"
);

static void led_clock_word(int cki, int sdi, uint16_t word)
{
}

static void __led_set_rgb(int red, int green, int blue, int cki, int lei, int sdi)
{
}

static void led_set_rgb(int red, int green, int blue)
{
	__led_set_rgb(red, green, blue, 0, 1, 2);
}

void venom_setup(void)
{
}

void pre_command_hook(void)
{
	run_command_list (MERAKI_AUTOBOOT_CMD, -1, 0);
	led_set_rgb(255, 0, 0);
}

#ifndef CONFIG_CMD_NET
void
NetReceive(uchar *inpkt, int len)
{
}
uchar *NetRxPackets[4];
int eth_getenv_enetaddr(char *name, uchar *enetaddr)
{
   return 0;
}

int eth_register(struct eth_device *dev)
{
   return 0;
}
#endif
char *getenv(const char *name)
{ return NULL; }

ulong getenv_ulong(const char *name, int base, ulong default_val)
{ return default_val; }

int setenv(const char *varname, const char *varvalue)
{ return 1; }

int env_check_apply(const char *name, const char *oldval,
			const char *newval, int flag)
{ return 1; }

ulong load_addr = CONFIG_SYS_LOAD_ADDR;	/* Default Load Address */

int himport_r(struct hsearch_data *htab,
		const char *env, size_t size, const char sep, int flag,
		int nvars, char * const vars[], int do_apply)
{ return 0; }
