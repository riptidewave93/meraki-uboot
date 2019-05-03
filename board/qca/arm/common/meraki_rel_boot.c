#include <common.h>
DECLARE_GLOBAL_DATA_PTR;
#include <meraki_config.h>
#include <image.h>
#include <stdbool.h>
#include <asm/arch-qca-common/scm.h>
#include "sec_dat.h"  /* fuse file generated from qca-sectools */

static int decrement_ubi_retry_counter(const char *ubivol)
{
   char cmd[128];
   uint8_t count;

   /* Ensure that both part.new and upgrade_retries exist */
   sprintf(cmd, "ubi check %s", ubivol);
   if (run_command(cmd, 0) != CMD_RET_SUCCESS) {
      return CMD_RET_FAILURE;
   }

   /* Read the retry counter */
   sprintf(cmd, "ubi read 0x%x %s 1", (unsigned)&count, ubivol);
   if (run_command(cmd, 0) != CMD_RET_SUCCESS) {
      return CMD_RET_FAILURE;
   }

   /* sanity */
   if (count > 5) {
      count = 5;
   }

   printf("%s counter is: %d\n", ubivol, count);
   if (!count) {
      printf("\nUpgrade retries 0!! Not booting part.new\n");
      return CMD_RET_FAILURE;
   }

   /* Update the retry counter */
   count--;
   sprintf(cmd, "ubi write 0x%x %s 1", (unsigned)&count, ubivol);
   if (run_command(cmd, 0) != CMD_RET_SUCCESS) {
      return CMD_RET_FAILURE;
   }

   return CMD_RET_SUCCESS;
}

static void do_upgrade_boot(void)
{
   /* Ensure that both part.new and upgrade_retries exist */
   if (run_command("ubi check part.new", 0) != CMD_RET_SUCCESS) {
      return;
   }

   if (decrement_ubi_retry_counter("upgrade_retries") != CMD_RET_SUCCESS) {
      return;
   }

   /* Boot it */
   setenv("part","part.new");
   run_command("run boot_signedpart",0);
}

static void do_chainload_uboot(void)
{
   /* Ensure uboot.dev exists and keepalive valid */
   if (run_command("ubi check uboot.dev", 0) != CMD_RET_SUCCESS) {
      return;
   }

   if (decrement_ubi_retry_counter("dev_uboot_keepalive") != CMD_RET_SUCCESS) {
      return;
   }

   /* Boot it */
   run_command("run chainload_uboot", 0);
}

static bool devel_crt_valid(void)
{
   const void *dev_crt;

   if (run_command("ubi check dev.crt", 0) != CMD_RET_SUCCESS)
      return false;

   printf("Validating developer certificate\n");
   if (run_command("ubi read $loadaddr dev.crt; validate $loadaddr", 0) != CMD_RET_SUCCESS) {
      printf("Invalid developer certificate\n");
      return false;
   }

   printf("Valid signed developer certificate\n");
   dev_crt = (const void *)getenv_ulong("imgaddr", 16, 0);
   if (!dev_crt) {
      printf("Bad imgaddr %p\n", dev_crt);
      return false;
   }

   /* check this ID against node ID in the unlock */
   if (!meraki_check_unique_id(dev_crt)) {
      printf("Node serial number mismatch\n");
      return false;
   }

   return true;
}

static void force_secboot(void)
{
   int ret;
   char buf;
   char cmd[40];

   printf("\n\n");
   ret = scm_call(SCM_SVC_FUSE, QFPROM_IS_AUTHENTICATE_CMD, NULL, 0, &buf, sizeof(buf));
   if (ret != CMD_RET_SUCCESS || buf != 1) {
      if (ret != CMD_RET_SUCCESS)
         printf("Secure boot check error: %d\n", ret);
      else
         printf("Secure boot NOT enabled!\n");

      /* Blow fuses */
      printf("Blowing fuses...\n");
      snprintf(cmd, sizeof(cmd), "fuseipq %p", sec_dat);
      ret = run_command(cmd, 0);
      if (ret != CMD_RET_SUCCESS)
         printf("fuseipq returned %d\n", ret);

      mdelay(200);
      printf("Resetting now.\n\n");
      do_reset(NULL, 0, 0, NULL);
      printf("Power-off now.\n");
      hang();
   }
   printf("Secure boot enabled.\n\n");
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
         return 0;
      default:
         break;
   }

   /* Check 0: check/force secure boot on */
   force_secboot();

   /* Check 1: boot diagnostic */
   run_command("run check_boot_diag_img", 0);

   /* Check 2: boot part.new if upgrading */
   do_upgrade_boot();

   /* Check 3: if node specific unlock exists, chainload u-boot */
   if (devel_crt_valid()) {
      do_chainload_uboot();
   }

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
