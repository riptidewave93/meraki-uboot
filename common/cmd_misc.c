/*
 * (C) Copyright 2001
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
 * Misc functions
 */
#include <common.h>
#include <command.h>
#ifdef CONFIG_MERAKI
#define CONFIG_SYS_MPA_GPIO_CSR_BASE		0xdd8b0000	/* MPA_GPIO_CS */
#define CONFIG_SYS_MPA_GPIO_BASE		CONFIG_SYS_MPA_GPIO_CSR_BASE 

#define DEBUG_MPA_REGMOD 1
#define mpa_gpio_wr(reg, d)	\
  do { *(volatile unsigned int *)(CONFIG_SYS_MPA_GPIO_BASE + reg) = d; } while (0)
#define mpa_gpio_rd(reg, d)	\
  do { d = *(volatile unsigned int *)(CONFIG_SYS_MPA_GPIO_BASE+ reg); } while (0)
 
u32 mpa_gpio_regmod(u32 addr, u32 data, u8 bl, u8 bh)
{
	u32 reg, mask;

	mask = ~(0xffffffff << (32 - bl));
	data &= mask;
	data = data << (31 - bh);
	
	/*Preparing register mask*/
        mask = ~(0xffffffff << (32 - bl));
        mask &= (0xffffffff << (31 - bh));
        mask = ~(mask);

	 mpa_gpio_rd(addr, reg);
#if defined(DEBUG_MPA_REGMOD)
	printf("%s: reg 0x%x is 0x%x set 0x%x from %d to %d\n", __func__, addr, reg, data, bl, bh);
#endif
	reg &= mask;
	reg |= data;
	 mpa_gpio_wr(addr, reg);

#if defined(DEBUG_MPA_REGMOD)
	printf("%s: setting 0x%x to 0x%x\n", __func__, addr, reg);
#endif
	return 0;
}

u32 mpa_gpio_regread (u32 addr, int bl, int bh) 
{
	u32 mask = 0;
	int i;
  u32 regval = 0;    
#if defined(DEBUG_MPA_REGMOD)
	printf("%s: @ 0x%08x,%d,%d(", __func__, addr,bl,bh);
#endif

         mpa_gpio_rd(addr, regval);
#if defined(DEBUG_MPA_REGMOD)
       printf("@ 0x%08x: 0x%08x)",addr,regval);
#endif
        for(i=0; i<(bh - bl + 1); i++) {
                mask |= (1 << i) ;
        }
        regval = regval >> (31 - bh);
        regval &= mask;
#if defined(DEBUG_MPA_REGMOD)
        printf(" = %x (mask = %08x)",regval,mask);
#endif
        return regval;
}
#endif
int do_sleep (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	ulong start = get_timer(0);
	ulong delay;

	if (argc != 2) {
		cmd_usage(cmdtp);
		return 1;
	}

	delay = simple_strtoul(argv[1], NULL, 10) * CONFIG_SYS_HZ;

	while (get_timer(start) < delay) {
		if (ctrlc ()) {
			return (-1);
		}
		udelay (100);
	}

	return 0;
}
#ifdef CONFIG_MERAKI
int do_regread(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	u32 addr;
	u32 regval;
	int bl,bh;
	if (argc != 4) {
		cmd_usage(cmdtp);
		return 1;
	}

	addr = simple_strtoul(argv[1], NULL, 16);
	bl   = simple_strtoul(argv[2], NULL, 10);
	bh   = simple_strtoul(argv[3], NULL, 10);  
	regval = mpa_gpio_regread(addr,bl,bh);
	printf("\n\n");	
	return 0;
}

int do_regmod (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	u32 addr;	
	u8 bl=0,bh=0;
	u32 data = 0;

	if (argc != 5) {
		cmd_usage(cmdtp);
		return 1;
	}

	addr = simple_strtoul(argv[1], NULL, 16);
	data = simple_strtoul(argv[2], NULL, 16);
	bl   = simple_strtoul(argv[3], NULL, 10);
	bh   = simple_strtoul(argv[4], NULL, 10);
  mpa_gpio_regmod(addr,data,bl, bh);

  printf("\n\n");
	return 0;
}
U_BOOT_CMD(
	 regmod,    5,    1,     do_regmod,
	"Write GPIO Controller Register",
	"N\n"
	"    - regmod <addr>  <val> <bl> <bh>"
);

U_BOOT_CMD(
	regread ,    4,    1,     do_regread,
	"Read GPIO Controller Register",
	"N\n"
	"    - regread <addr> <bl>  <bh>"
);
#endif
U_BOOT_CMD(
	sleep ,    2,    1,     do_sleep,
	"delay execution for some time",
	"N\n"
	"    - delay execution for N seconds (N is _decimal_ !!!)"
);
