#if CFG_MDK_DEBUG == 1
#include <common.h>
#include <command.h>

/* 
 * Avoid double definitions when mixing CDK 
 * and system header files.
 */
#define CDK_CONFIG_DEFINE_UINT8_T       0
#define CDK_CONFIG_DEFINE_UINT16_T      0
#define CDK_CONFIG_DEFINE_UINT32_T      0
#define CDK_CONFIG_DEFINE_PRIu32        0
#define CDK_CONFIG_DEFINE_PRIx32        0
#define CDK_CONFIG_DEFINE_SIZE_T        0
#define CDK_CONFIG_DEFINE_DMA_ADDR_T    0

#include "mdk_sys.h"

#include "cdk/cdk_shell.h"
#include "cdk/shell/shcmd_debug.h"

#include <cdk/arch/robo_cmds.h>

#include "bmd/shell/shcmd_reset.h"
#include "bmd/shell/shcmd_switching_init.h"
#include "bmd/shell/shcmd_port_mode.h"
#include "bmd/shell/shcmd_tx.h"
#include "bmd/shell/shcmd_rx.h"
#include "bmd/shell/bmd_cmds.h"

#if CFG_MDK_DIAG
#include <bmddiag/shell/diag_cmds.h>
#endif

static char* cdk_gets(const char *prompt, char *str, int maxlen)
{
    str[0] = '\0';
    readline_into_buffer(prompt, str, 0);
    return str;
}

int do_mdk (cmd_tbl_t *cmdtp, int flag, int argc, char * argv[])
{
    if (argc < 2) {
	    /* without sub-commands */
		return cmd_usage(cmdtp);
    }

    if (!strcmp(argv[1],"probe")){
        return mdk_sys_probe();
    } else if (!strcmp(argv[1],"shell")){
        /* Initialize shell */
        cdk_shell_init();

        /* Add CDK commands */
        cdk_shell_add_robo_core_cmds();

        /* Add BMD commands */
        bmd_shell_add_bmd_cmds();

#if CFG_MDK_DIAG
        bmddiag_shell_add_diag_cmds();
#endif

        /* Start CDK shell */
        return cdk_shell("MDK", cdk_gets);
    } else if (!strcmp(argv[1],"swinit")){
        return bmd_shcmd_switching_init(argc, argv);
	}

    return cmd_usage(cmdtp);
}

U_BOOT_CMD(
	mdk,	3,	1,	do_mdk,
	"mdk commands:",
	"mdk probe\n"
	"    - Probe for MDK switch devices\n"
	"mdk shell\n"
	"    - Enter CDK shell. Type 'quit' or 'exit' to return to the u-boot shell.\n"
	"mdk swinit\n"
	BMD_SHCMD_SWITCHING_INIT_HELP
	"\n"
);

#endif
