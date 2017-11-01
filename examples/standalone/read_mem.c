/*
 * (C) Copyright 2000
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

#include <common.h>
#include <exports.h>

void init_timeouts() ;

int hello_world (int argc, char * const argv[])
{
	int i;

	//unsigned int start=0, end=0xffffffff;
	unsigned int start=0x00000000, end=0xffffffff;
	//unsigned int start=0x18029000, end=0xffffffff;
	//unsigned int start=0x18010000, end=0xffffffff;
	//unsigned int start=0x1802d000, end=0xffffffff;
	//unsigned int start=0x18137000, end=0xffffffff;
	volatile unsigned int *ptr, c, *wg;

	#define MAX_COUNT		0x10000
	volatile count=MAX_COUNT;

	/* Print the ABI version */
	app_startup(argv);
	printf ("Example expects ABI version %d\n", XF_VERSION);
	printf ("Actual U-Boot ABI version %d\n", (int)get_version());

	printf ("Hello World\n");

	printf ("argc = %d\n", argc);

	init_timeouts();

	for (i=0; i<=argc; ++i) {
		printf ("argv[%d] = \"%s\"\n",
			i,
			argv[i] ? argv[i] : "<NULL>");
	}


	wg=0x18000080;

	for (ptr=start; ptr< end; ptr) {
		if (ptr == 0x18010400) {
			ptr=0x18011000;
			printf("0x%08x :  skip\n", ptr); continue;
		} else if (ptr == 0x180121fc) {
			ptr=0x18012200;
			printf("0x%08x :  skip\n", ptr); continue;
		} else if (ptr == 0x180131fc) {
			ptr=0x18013200;
			printf("0x%08x :  skip\n", ptr); continue;
		} else if (ptr == 0x180141fc) {
			ptr=0x18014200;
			printf("0x%08x :  skip\n", ptr); continue;
		} else if (ptr == 0x18020100) {
			ptr=0x18021000;
			printf("0x%08x :  skip\n", ptr); continue;
		} else if (ptr == 0x18023000) {
			ptr=0x18024000;
			printf("0x%08x :  skip\n", ptr); continue;
		} else if (ptr == 0x1802a000) {
			ptr=0x1802b000;
			printf("0x%08x :  skip\n", ptr); continue;
		} else if (ptr == 0x1802c000) {
			ptr=0x1802d000;
			printf("0x%08x :  skip\n", ptr); continue;
		} 
		//printf("0x%08x : %08x\n", ptr, c);
		//printf("0x%08x :", ptr);
		if (--count == 0){
			printf("0x%08x:\n", ptr);
			count = MAX_COUNT;
		} 
		//*wg=0x01000000;
		
		c=*ptr;
		//printf(" %08x\n", c);
		//ptr+=1000;
		//ptr+=400;
		ptr++;
	}
		

	printf ("Hit any key to exit ... ");
	while (!tstc())
		;
	/* consume input */
	(void) getc();

	printf ("\n\n");
	return (0);
}

void init_timeouts() {

#define DDR_S1_IDM_ERROR_LOG_CONTROL                        0x18108900
#define DDR_IDM_ERROR_LOG_CONTROL_VAL                       0x330
#define DDR_S2_IDM_ERROR_LOG_CONTROL                        0x18109900
#define AXI_PCIE_S0_IDM_ERROR_LOG_CONTROL             0x1810A900
#define AXI_PCIE_S1_IDM_ERROR_LOG_CONTROL             0x1810B900
#define AXI_PCIE_S2_IDM_ERROR_LOG_CONTROL             0x1810C900
#define ROM_S0_IDM_ERROR_LOG_CONTROL                0x1810D900
#define NAND_IDM_ERROR_LOG_CONTROL                          0x1811A900
#define QSPI_IDM_ERROR_LOG_CONTROL                          0x1811B900
#define A9JAG_S0_IDM_ERROR_LOG_CONTROL                      0x1811C900
#define APBX_IDM_IDM_ERROR_LOG_CONTROL                      0x18121900
#define AXIIC_DS_0_IDM_IDM_ERROR_LOG_CONTROL        0x18132900
#define AXIIC_DS_1_IDM_IDM_ERROR_LOG_CONTROL        0x18133900
#define AXIIC_DS_2_IDM_IDM_ERROR_LOG_CONTROL        0x18134900
#define AXIIC_DS_3_IDM_IDM_ERROR_LOG_CONTROL        0x18135900
#define AXIIC_DS_4_IDM_IDM_ERROR_LOG_CONTROL        0x18136900
#define ARMCORE_S1_IDM_ERROR_LOG_CONTROL              0x18106900
#define ARMCORE_S0_IDM_ERROR_LOG_CONTROL              0x18107900

#define W_REG(o, a, v) *a=(unsigned int)v
#define uint32 unsigned int

    W_REG(osh, (uint32 *)ARMCORE_S1_IDM_ERROR_LOG_CONTROL, DDR_IDM_ERROR_LOG_CONTROL_VAL);
    W_REG(osh, (uint32 *)ARMCORE_S0_IDM_ERROR_LOG_CONTROL, DDR_IDM_ERROR_LOG_CONTROL_VAL);
    W_REG(osh, (uint32 *)DDR_S1_IDM_ERROR_LOG_CONTROL, DDR_IDM_ERROR_LOG_CONTROL_VAL);
    W_REG(osh, (uint32 *)DDR_S2_IDM_ERROR_LOG_CONTROL, DDR_IDM_ERROR_LOG_CONTROL_VAL);
    W_REG(osh, (uint32 *)AXI_PCIE_S0_IDM_ERROR_LOG_CONTROL, DDR_IDM_ERROR_LOG_CONTROL_VAL);
    W_REG(osh, (uint32 *)AXI_PCIE_S1_IDM_ERROR_LOG_CONTROL, DDR_IDM_ERROR_LOG_CONTROL_VAL);
    W_REG(osh, (uint32 *)AXI_PCIE_S2_IDM_ERROR_LOG_CONTROL, DDR_IDM_ERROR_LOG_CONTROL_VAL);
    W_REG(osh, (uint32 *)ROM_S0_IDM_ERROR_LOG_CONTROL, DDR_IDM_ERROR_LOG_CONTROL_VAL);
    W_REG(osh, (uint32 *)NAND_IDM_ERROR_LOG_CONTROL, DDR_IDM_ERROR_LOG_CONTROL_VAL);
    W_REG(osh, (uint32 *)QSPI_IDM_ERROR_LOG_CONTROL, DDR_IDM_ERROR_LOG_CONTROL_VAL);
    W_REG(osh, (uint32 *)A9JAG_S0_IDM_ERROR_LOG_CONTROL, DDR_IDM_ERROR_LOG_CONTROL_VAL);
    W_REG(osh, (uint32 *)APBX_IDM_IDM_ERROR_LOG_CONTROL, DDR_IDM_ERROR_LOG_CONTROL_VAL);
    W_REG(osh, (uint32 *)AXIIC_DS_0_IDM_IDM_ERROR_LOG_CONTROL, DDR_IDM_ERROR_LOG_CONTROL_VAL);
    W_REG(osh, (uint32 *)AXIIC_DS_1_IDM_IDM_ERROR_LOG_CONTROL, DDR_IDM_ERROR_LOG_CONTROL_VAL);
    W_REG(osh, (uint32 *)AXIIC_DS_2_IDM_IDM_ERROR_LOG_CONTROL, DDR_IDM_ERROR_LOG_CONTROL_VAL);
    W_REG(osh, (uint32 *)AXIIC_DS_3_IDM_IDM_ERROR_LOG_CONTROL, DDR_IDM_ERROR_LOG_CONTROL_VAL);
    W_REG(osh, (uint32 *)AXIIC_DS_4_IDM_IDM_ERROR_LOG_CONTROL, DDR_IDM_ERROR_LOG_CONTROL_VAL);


}

