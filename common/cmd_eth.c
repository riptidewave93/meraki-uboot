/*
 * (C) Copyright 2010-2011
 * Mahesh Pujara  <mpujara@amcc.com>
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
 * Misc boot support
 */
#include <common.h>
#include <command.h>
#include "../cpu/ppc4xx/apm_enet_mac.h"

extern struct apm_data_priv gpriv;
extern int atoi(char *string);

int do_eth_util (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	u32 cmd , data, reg_offset = 0;
        u32 block_id = 0;

        if (argc < 4) {
		cmd_usage(cmdtp);
		return 0;
	}
        
        cmd = atoi(argv[1]);
        block_id = atoi(argv[2]);
        reg_offset = atoi(argv[3]);
                
        if ((cmd >= MAX_CMD) || (block_id >= BLOCK_ETH_MAX)) {
                cmd_usage(cmdtp);
		return 0;    
        }
        
        if ((cmd == WRITE_CMD) && (argc < 5)) {
                cmd_usage(cmdtp);
		return 0;      
        }
        
        if (block_id >= BLOCK_ETH_MAX) {
                printf("This is not yet supported \n ");
                return 0;
        }
        
        switch(cmd) {
        case READ_CMD:
                printf("Reading  block-id [%d], reg_offset[0X%08x] \n",
                       block_id, reg_offset);
                apm_enet_rd32(&gpriv, block_id, reg_offset, &data);
                printf("Read Data is : 0X%08X \n", data);
                break;
                
        case WRITE_CMD:
                data = atoi(argv[4]);
                printf("Writing block-id [%d], reg_offset[0X%08X], "
                       "data[0X%X] \n", block_id, reg_offset, data);
                apm_enet_wr32(&gpriv, block_id, reg_offset, data);
                break;
        default:
                printf("It should never reach here \n ");
        }
        
	return 0;
}

/* -------------------------------------------------------------------- */

U_BOOT_CMD(
	eth_util, 5, 1,	do_eth_util,
	"Mamba Ethernet util",
        " [1. Command op: 0: read, 1: write \n"
        "  2. Block id:   1: MAC \n"
        "                 2: STAT \n"
        "                 3: GLOBAL \n"
        "                 4: FFDIV \n"
        "                 5: MAC GLBL \n"
	"                 6: PTP     \n"
	"                 7: UNISEC  \n "
	"                 8: DIAG    \n"
	"                 9: ENET QMI Slave \n"
        "  3. reg_offset: Register to access \n"
        "  4. data: 32 bit data to write. Ignored in read.]\n"
        );
