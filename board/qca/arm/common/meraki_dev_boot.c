#include <common.h>
DECLARE_GLOBAL_DATA_PTR;
#include <meraki_config.h>
#include <image.h>
#include <stdbool.h>

static void do_part_dev_boot(void)
{
   char cmd[128];
   uint8_t retry_count;

   /* Ensure that both part.dev and dev_upgrade_keepalive exist */
   if (run_command("ubi check dev_upgrade_keepalive", 0) != CMD_RET_SUCCESS) {
      return;
   }

   if (run_command("ubi check part.dev", 0) != CMD_RET_SUCCESS) {
      return;
   }

   /* Read the retry counter */
   sprintf(cmd, "ubi read 0x%x dev_upgrade_keepalive 1", (unsigned)&retry_count);
   if (run_command(cmd, 0) != CMD_RET_SUCCESS) {
      return;
   }

   /* sanity */
   if (retry_count > 5) {
      retry_count = 5;
   }

   printf("Upgrade retries: %d\n", retry_count);
   if (!retry_count) {
      printf("\nUpgrade retries 0!! Not booting part.dev\n");
      return;
   }

   /* Update the retry counter */
   retry_count--;
   sprintf(cmd, "ubi write 0x%x dev_upgrade_keepalive 1", (unsigned)&retry_count);
   if (run_command(cmd, 0) != CMD_RET_SUCCESS) {
      return;
   }

   printf("\n--------------------------\n");
   printf("------- part.dev  --------\n");
   printf("--------------------------\n");
   /* Boot it */
   setenv("part","part.dev");
   run_command("run boot_part",0);
}

static void do_update_uboot_keepalive(void)
{
   char cmd[128];
   uint8_t retry_count = 0x5;

   if (run_command("ubi check dev_uboot_keepalive", 0) != CMD_RET_SUCCESS) {
      return;
   }

   /* Update the Keepalive */
   sprintf(cmd, "ubi write 0x%x dev_uboot_keepalive 1", (unsigned)&retry_count);
   if (run_command(cmd, 0) != CMD_RET_SUCCESS) {
      printf("Failed to update dev U-boot Keepalive volume\n");
      return;
   }
}

static int do_meraki_qca_boot(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
   /* unsupported boards */
   switch(get_meraki_product_id()) {
      case MERAKI_BOARD_STINKBUG:
      case MERAKI_BOARD_LADYBUG:
      case MERAKI_BOARD_NOISY_CRICKET:
      case MERAKI_BOARD_YOWIE:
      case MERAKI_BOARD_BIGFOOT:
      case MERAKI_BOARD_SASQUATCH:
      case MERAKI_BOARD_WOOKIE:
         printf("Secure boot is not supported on this model\n");
         return 0;
      default:
         break;
   }

   do_update_uboot_keepalive();

   if ((run_command("ubi check bootcustom", 0) == CMD_RET_SUCCESS) &&
       (run_command("ubifsmount ubi0:bootcustom", 0) == CMD_RET_SUCCESS)) {

      run_command("run check_load_boot_scr", 0);
   }

   do_part_dev_boot();

   setenv("part","part.safe");
   run_command("run boot_signedpart",0);

   setenv("part","part.old");
   run_command("run boot_signedpart",0);

   run_command("reset",0);

   return 0;
}

U_BOOT_CMD(
	boot_meraki_qca,	1,	0,	do_meraki_qca_boot,
	"Meraki's QCA boot",
	NULL
);

static int do_meraki_bootcustom(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
   ulong addr;
   ulong size = -1;

   if (argc < 2) {
      addr = CONFIG_SYS_LOAD_ADDR;
   } else {
      addr = simple_strtoul(argv[1], NULL, 16);
   }

   size = getenv_ulong("filesize", 16, -1);
   run_command_list((const char *)addr, size, 0);

   return 0;
}

U_BOOT_CMD(
	bootcustom,	2,	0,	do_meraki_bootcustom,
	"Meraki's Bootcustom boot",
	NULL
);
