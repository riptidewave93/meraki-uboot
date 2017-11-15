/*
 * (C) Copyright 2010-2011
 * Keyur Chudgar <kchudgar@amcc.com>
 * Prodyut Hazarika <phazarika@amcc.com>
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

#include <config.h>
#include <common.h>
#include <command.h>
#include <asm/processor.h>
#include <asm/io.h>
#include <asm/apm_ipp_csr.h>
#include <asm/apm_ipp_interface.h>
#include <asm/apm_scu.h>
#include <malloc.h>
#include <linux/err.h>

#ifdef CONFIG_MEDIA_U_BOOT 
extern int apm86xxx_ipp_read_file(char *f_name, u32 addr, u32 offset, u32 size);
#endif

#ifdef CONFIG_IPP_MESSAGING

int do_ipp_cmd (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int ret;
	ulong msg_data, msg_param;
	u32 data;
#ifdef CONFIG_MEDIA_U_BOOT
	char* e_file;
	u32 e_addr, e_offset, e_size;
#endif

	if (argc == 1) {
		cmd_usage(cmdtp);
		return 0;
	}

#ifdef CONFIG_MEDIA_U_BOOT
	if (strcmp(argv[1], "eread") == 0) {
		if (argc < 4) {
			printf("eread missing arguments\n");
			cmd_usage(cmdtp);
			return 0;
		}
		e_file = argv[2];
		e_addr = simple_strtoul (argv[3], NULL, 16);
		e_offset = 0x0;
		if(argv[4]!=NULL && 4<argc)
			e_offset = simple_strtoul (argv[4], NULL, 16);
		e_size = 0xFFFFFFFF;
		if(argv[5]!=NULL && 5<argc)
			e_size = simple_strtoul (argv[5], NULL, 16);
		printf(" Reading %s from offset=0x%x to addr=0x%x\n",e_file,e_offset,e_addr);
		ret = apm86xxx_ipp_read_file(e_file,e_addr,e_offset,e_size);
		if (ret)
			printf(" iPP eread failure\n");
		else
			printf(" iPP eread done\n");
#else
	if (strcmp(argv[1], "fwload") == 0) {	
		data = simple_strtoul (argv[2], NULL, 16);
		ret = apm86xxx_ipp_fwload(data); 
		if (ret)
			printf(" iPP Firmload load failure\n");
#endif
	} else if (strcmp(argv[1], "powerstate") == 0) {
#ifdef CONFIG_CMD_IPP_PWRMGMT
		if (argc < 3) {
			printf("powermode missing (doze, nap, sleep, deepsleep)\n");
			cmd_usage(cmdtp);
			return 0;
		}
		cpu_release(1, 1, NULL);
		return apm86xxx_set_powerstate(argv[2]);
#endif
	} else if (strcmp(argv[1], "sendmsg") == 0) {
		if (argc < 4) {
			printf("sendmsg parameter error\n");
			cmd_usage(cmdtp);
			return 0;
		}
		msg_data = simple_strtoul (argv[2], NULL, 16);
		msg_param = simple_strtoul (argv[3], NULL, 16);
		printf("Sending iPP Msg : data %08x param: %08x ... \n",
			(unsigned int)msg_data, (unsigned int)msg_param);
		ret = apm86xxx_write_ipp_msg(msg_data, msg_param); 
		if (ret) {
			printf(" iPP write timeout\n");
		} else {
			ret = apm86xxx_read_ipp_msg(&data);
			switch (ret) {
			case -ENAVAIL:
				printf("successful ... \n");
				return 0;
			case -ETIMEDOUT:
				printf("iPP Read Timeout\n");
				return ret;
			default:
				msg_param = IPP_DECODE_MSG_TYPE(data);
				if (msg_param == IPP_ERROR_MSG) {
					printf("iPP returns Error message ... code %x\n",
						(unsigned int)(IPP_DECODE_ERROR_MSG_CODE(data)));
				} else {
					printf("iPP returns %x\n", data);
				}
				return ret;
			}
		}
	} else if (strcmp(argv[1], "ping") == 0) {
		u32 data;

		/* Send FWLoad command to IRAM */
		data = IPP_ENCODE_PING_MSG();
		if ((ret = apm86xxx_write_ipp_msg(data,IPP_MSG_PARAM_UNUSED)) != 0) {
			printf("Failed to write iPP Ping Msg ... \n");
			return ret;
		}
		ret = apm86xxx_read_ipp_msg(&data);
		if (!ret && (data == IPP_ENCODE_OK_MSG())) {
			printf("iPP Ping Response ... OK\n");
			return 0;
		} else {
			printf("iPP Ping Timeout\n");
			return ret;
		}
	} else if (strcmp(argv[1], "mode") == 0) {
		u32 mode;
		int major, minor;

		mode = apm86xxx_ipp_getmode(&major, &minor);
		switch (mode) {
		case IPP_ROM_MODE_MASK:
			printf("iPP ROM Mode v(%d.%d)\n",
				major, minor);
			return 0;
		case IPP_RUNTIME_MODE_MASK:
			printf("iPP Runtime Mode v(%d.%d)\n",
				major, minor);
			return 0;
		case IPP_EXTBOOT_MODE_MASK:
			printf("iPP External Boot Mode v(%d.%d)\n",
				major, minor);
			return 0;
		default:
			printf("iPP Invalid Mode!\n");
			return -EINVAL;
		}
	} else if (strcmp(argv[1], "cpmregread") == 0) {	
		u8 regval;
		msg_data = simple_strtoul (argv[2], NULL, 16);
		if (msg_data > 0xff) {
			printf("CPM register offset must be between 0 to 0xFF\n");
			return -EINVAL;
		}
		ret = apm86xxx_read_cpmreg((u8)msg_data,&regval); 
		if (ret) {
			printf("CPM regread from offset 0x%x failed (%d)\n",
				(unsigned int)msg_data, ret);
		} else {
			printf("CPM register 0x%x : value 0x%x\n",
				(unsigned int)msg_data, 
				(unsigned int)regval);
		}
	} else if (strcmp(argv[1], "cpmregwrite") == 0) {	
		msg_data = simple_strtoul (argv[2], NULL, 16);
		msg_param = simple_strtoul (argv[2], NULL, 16);
		if (msg_data > 0xff) {
			printf("CPM register offset must be between 0 to 0xFF\n");
			return -EINVAL;
		}
		if (msg_param > 0xff) {
			printf("CPM register value must be between 0 to 0xFF\n");
			return -EINVAL;
		}
		ret = apm86xxx_write_cpmreg((u8)msg_data,(u8)msg_param); 
		if (ret) {
			printf("CPM regwrite to offset 0x%x failed (%d)\n",
				(unsigned int)msg_data, ret);
		} else {
			printf("CPM regwrite to offset 0x%x value 0x%x successful\n",
				(unsigned int)msg_data, 
				(unsigned int)msg_param);
		}
	}
	return 0;
}

int do_clks_cmd (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	char *s;
	PPC4xx_SYS_INFO sys_info;
	u32 cpu_id = mfspr(SPRN_PIR);

	get_sys_info(&sys_info);

	s = getenv ("AMP");
	if (s && simple_strtoul(s, NULL, 10)) {
		printf ("CPU%d at %lu MHz",
			cpu_id, 
			sys_info.freqProcessor[cpu_id] / 1000000);
	} else {
		printf ("CPU0 at %lu MHz CPU1 at %lu MHz",
			sys_info.freqProcessor[CORE0_ID] / 1000000,
			sys_info.freqProcessor[CORE1_ID] / 1000000);
	}
	printf (" PLB=%lu MHz\n",
		sys_info.freqPLB / 1000000);
	printf ("SOC=%lu AXI=%lu AHB=%lu IAHB=%lu APB=%lu EBC=%lu DDR=%lu\n",
		sys_info.freqSOC / 1000000,
		sys_info.freqAXI / 1000000,
		sys_info.freqAHB / 1000000,
		sys_info.freqIAHB / 1000000,
		sys_info.freqOPB / 1000000,
		sys_info.freqEBC / 1000000,
		sys_info.freqDDR / 1000000);

	u32 reg_data;
	reg_data = in_be32((volatile u32 *)(CONFIG_SYS_SCU_BASE + SCU_SOCPLL1_ADDR));
	printf("SCU_SOCPLL1_ADDR 0x%x = 0x%x\n",CONFIG_SYS_SCU_BASE + SCU_SOCPLL1_ADDR, reg_data); 
	reg_data = in_be32((volatile u32 *)(CONFIG_SYS_SCU_BASE + SCU_SOCDIV_ADDR));
	printf("SCU_SOCDIV_ADDR 0x%x = 0x%x\n",CONFIG_SYS_SCU_BASE + SCU_SOCDIV_ADDR, reg_data); 
	reg_data = in_be32((volatile u32 *)(CONFIG_SYS_SCU_BASE + SCU_SOCDIV5_ADDR));
	printf("SCU_SOCDIV5_ADDR 0x%x = 0x%x\n",CONFIG_SYS_SCU_BASE + SCU_SOCDIV5_ADDR, reg_data); 

	reg_data = in_be32((volatile u32 *)(CONFIG_SYS_SCU_BASE + SCU_SOCPLL4_ADDR));
	printf("SCU_SOCPLL4_ADDR 0x%x = 0x%x\n",CONFIG_SYS_SCU_BASE + SCU_SOCPLL4_ADDR, reg_data); 
	reg_data = in_be32((volatile u32 *)(CONFIG_SYS_SCU_BASE + SCU_SOCDIV3_ADDR));
	printf("SCU_SOCDIV3_ADDR 0x%x = 0x%x\n",CONFIG_SYS_SCU_BASE + SCU_SOCDIV3_ADDR, reg_data); 
	reg_data = in_be32((volatile u32 *)(CONFIG_SYS_SCU_BASE + SCU_SOCDIV4_ADDR));
	printf("SCU_SOCDIV4_ADDR 0x%x = 0x%x\n",CONFIG_SYS_SCU_BASE + SCU_SOCDIV4_ADDR, reg_data); 
	return 0;
}

/* -------------------------------------------------------------------- */

U_BOOT_CMD(
	ipp, 6, 0,	do_ipp_cmd,
	"iPP Commands",
#ifdef CONFIG_MEDIA_U_BOOT
	"eread <file> <addr> [offset] [size] - Read external file\n"
#else
	"fwload <addr>                           - Load iPP Firmware\n"
#endif
	"ipp ping                                - Send iPP ping\n"
#ifdef CONFIG_CMD_IPP_PWRMGMT
	"ipp powerstate <mode>                   - Set low power <mode>\n"
#endif
	"ipp sengmsg <msgdata> <msgparam>        - Send iPP message\n"
	"ipp cpmregread <offset>                 - Get CPM regval\n"
	"ipp cpmregwrite <offset> <val>          - Set CPM regval"
);

U_BOOT_CMD(
	showclks, 4, 0,	do_clks_cmd,
	"Show all Clocks",
	"showclks  - Display all Clock frequencies\n"
);

#endif /* CONFIG_IPP_MESSAGING */
