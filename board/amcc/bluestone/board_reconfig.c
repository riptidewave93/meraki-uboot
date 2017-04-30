/*
 * (C) Copyright 2008
 * Stefan Roese, DENX Software Engineering, sr@denx.de.
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
 *
 */

#include <common.h>
#include <command.h>
#include <i2c.h>
#include <asm/io.h>
#include <nand.h>

#if !defined(CONFIG_SRAM_U_BOOT)
static int do_pll_reconfig(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
#if defined(CONFIG_PLL_RECONFIG_ADDR)
        u32 pll;

        if (argc < 2) {
                printf("Usage:\n%s\n", cmdtp->usage);
                return 1;
        }

        if (strcmp(argv[1], "1") == 0) {
                pll = CONFIG_CLK_800_200_200_100;
        } else if (strcmp(argv[1], "2") == 0) {
                pll = CONFIG_CLK_1000_166_250_83;
        } else if (strcmp(argv[1], "3") == 0) {
                pll = CONFIG_CLK_1000_250_250_83;
        } else if (strcmp(argv[1], "4") == 0) {
                pll = CONFIG_CLK_1200_200_300_100;
        } else if (strcmp(argv[1], "5") == 0) {
                pll = CONFIG_CLK_1200_200_200_100;
        } else if (strcmp(argv[1], "6") == 0) {
            	pll = CONFIG_CLK_667_166_166_83;
        }else {
                printf("Unsupported cpu clock - only option 1|2|3|4|5|6 is supported\n");
                return 1;
        }

#if !defined(CONFIG_NAND_U_BOOT)
        if (flash_sect_protect (0, CONFIG_PLL_RECONFIG_ADDR,
                CONFIG_PLL_RECONFIG_ADDR + CONFIG_PLL_RECONFIG_SIZE - 1)) {
                puts("Fail!!!\n");
                return 1;
        }
        if (flash_sect_erase (CONFIG_PLL_RECONFIG_ADDR,
               CONFIG_PLL_RECONFIG_ADDR + CONFIG_PLL_RECONFIG_SIZE - 1)) {
                puts("Fail!!!\n");
                return 1;
        }
        if (flash_write ((char *)&pll, CONFIG_PLL_RECONFIG_ADDR, sizeof(pll))) {
                printf("Fail!!!\n");
               return 1;
        }
        printf("Done. Please power-cycle/reset the board for the changes to take effect\n");
#else
        nand_info_t *nand;
        nand = &nand_info[nand_curr_device];
        int ret = 0;
    	ulong off;
    	size_t size;

    	off = 0;
    	size = NAND_ERASE_BLOCK_SIZE;
        ret = nand_read_skip_bad(nand, off, &size, (u_char *)CONFIG_PLL_RECONFIG_RAM_R);
        if (ret){
        	printf("do_pll_reconfig: nand_read_skip_bad %s\n", ret ? "ERROR" : "OK");
        	return 1;
        }

		(*(u32*)(CONFIG_PLL_RECONFIG_RAM_R + CONFIG_PLL_RECONFIG_OFF + CONFIG_SYS_NAND_BOOT_SPL_SIZE)) = pll;

        nand_erase_options_t opts;
		memset(&opts, 0, sizeof(opts));
		opts.offset = 0;
		opts.length = NAND_ERASE_BLOCK_SIZE;
		opts.jffs2  = 0;
		opts.quiet  = 1;
		ret = nand_erase_opts(nand, &opts);
        if (ret){
        	printf("do_pll_reconfig: nand_erase_opts %s\n", ret ? "ERROR" : "OK");
        	return 1;
        }

        ret = nand_write_skip_bad(nand, off, &size, (u_char *)CONFIG_PLL_RECONFIG_RAM_R);
        if (ret){
        	printf("do_pll_reconfig: nand_write_skip_bad %s\n", ret ? "ERROR" : "OK");
        	return 1;
        }

		 printf("Done. Please power-cycle/reset the board for the changes to take effect\n");

#endif // #if !defined(CONFIG_NAND_U_BOOT)

#else
        printf("Unsupported on this boot image\n");
#endif // #if defined(CONFIG_PLL_RECONFIG_ADDR)
        return 0;
}

U_BOOT_CMD(
        cpuclk, 2,      0,      do_pll_reconfig,
        "cpuclk - change cpu clock frequency\n",
        "<1|2|3|4|5|6> - cpu clock options \n"
        " Option 1: CPU=800MHz, PLB=200MHz, DDR=200MHz, OPB=100MHz\n"
        " Option 2: CPU=1000MHz, PLB=166MHz, DDR=250MHz, OPB=83MHz\n"
        " Option 3: CPU=1000MHz, PLB=250MHz, DDR=250MHz, OPB=83MHz\n"
        " Option 4: CPU=1200MHz, PLB=200MHz, DDR=300MHz, OPB=100MHz (DDR may not work!)\n"
        " Option 5: CPU=1200MHz, PLB=200MHz, DDR=200MHz, OPB=100MHz\n"
        " Option 6: CPU=667MHz, PLB=166MHz, DDR=166MHz, OPB=83MHz\n"
        );
#endif

static int do_sdram_config(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	uchar sdram_width = 0;

	if (argc != 2) {
                printf("Usage:\n%s\n", cmdtp->usage);
                return 1;
        }

        if (strcmp(argv[1], "32") == 0) {
                sdram_width = 0x32;
        } else if (strcmp(argv[1], "16") == 0) {
                sdram_width = 0x16;
	} else {
		printf("Option is not supported\n");
		return 1;
	}

        if (i2c_write(CONFIG_SDRAM_INFO_EEPROM_ADDR, 
		CONFIG_SDRAM16BIT_OFFSET, 1, &sdram_width, 1)) {
                printf("Failed to write to I2C EEPROM 0x%x\n", 
				CONFIG_SDRAM_INFO_EEPROM_ADDR);

		return 1;
	}
	
	printf("Done. Please power-cycle or reset "
		"the board for the changes to take effect\n");

	return 0;
}

U_BOOT_CMD(
        sdram_config, 2,      0,      do_sdram_config,
        "sdram_config - Configure SDRAM as 32-bit or 16-bit\n",
        "<32|16 - SDRAm bit width\n"
        );


