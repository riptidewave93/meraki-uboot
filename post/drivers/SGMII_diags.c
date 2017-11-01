/*
 * Copyright (C) 2013, Broadcom Corporation. All Rights Reserved.
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */


#include <common.h>
#include <post.h>
#include <net.h>


#if CONFIG_POST & CONFIG_SYS_POST_SGMII
DECLARE_GLOBAL_DATA_PTR;

extern IPaddr_t    NetPingIP;

static char localIPName[16]="ipaddr";
static char serverIPName[16]="serverip";

#define XMK_STR(x)	#x
#define MK_STR(x)	XMK_STR(x)


static const char serverIP[] = {MK_STR(CONFIG_SERVERIP)};


int sgmii_post_test (int flags)
{
#if defined(CONFIG_NS_PLUS)
    IPaddr_t  ip;
    int status=0;
	char *var;
	int port;

    if ( (post_get_board_diags_type()&( BCM953025K | BCM953025K_REV2 |BCM958625HR ))==0)
	{
		printf("\nThis diag is not supported on this platform\n ");
		return 2;
	}


	post_log("\nSGMII diags starts  !!!");

	/* get port5 config */
	var = getenv(localIPName);
	if (var == NULL)
	{
		post_log("\nLocal board ip address not defined");
		post_log("\nAdd ipaddr in the env\n");
		return -1;
	}
	var = getenv(serverIPName);
	if (var == NULL)
	{
		post_log("\nserver ip address not defined");
		post_log("\nadd serverip in the env\n");
		return -1;
	}

	//SVKs have two SFPs
	if ( (post_get_board_diags_type() & BCM958625HR ) == 0  )
	{
		var = getenv("port4cfg");
		if ((var == NULL ) || ( strncmp(var,"sgmii",5)!=0) )
		{
			post_log("\nport4cfg needs to be set to sgmii");
			return -1;
		}
	}
	//25 HR has only one SFP
	var = getenv("port5cfg");
	if ((var == NULL ) || ( strncmp(var,"sgmii",5)!=0) )
	{
		post_log("\nport5cfg needs to be set to sgmii");
		return -1;
	}

    if ( (flags & POST_AUTO) !=  POST_AUTO )
    {
    	if ( (post_get_board_diags_type() & BCM958625HR ) == 0  )
    	{
			post_log("\nmake sure the host pc connected is configured to IP: %s",var );
			post_getConfirmation("\nReady? (Y/N)");
			post_log("\nSelect SGMII port to test:0 - port 4; 1 - port 5");
			post_log("\nPort 4 is bottom SFP/Port5 is the top SFP\n");
			port = post_getUserInput("Port (0/1)?:");
    	}
    }

    NetPingIP = string_to_ip(serverIP);

    if (NetLoop(PING) < 0)
    {
        post_log("Testfailed; host %s is not alive.\n", var);
        status = -1;
    }
    else
    {
        post_log("host %s is alive, test passed \n", var);
    }

    return status;
#else
    post_log("\nNot available on NS platform !!!");
    return -1;
#endif
}

#endif /* CONFIG_POST & CONFIG_SYS_POST_SGMII */
