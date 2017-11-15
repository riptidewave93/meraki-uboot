/**
 * Copyright (c) 2010 Applied Micro Circuits Corporation.
 * All rights reserved. Victor Gallardo<vgallard@apm.com>.
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
#include <ppc4xx.h>
#include <asm/processor.h>
#include <asm/io.h>
#include <asm/apm_scu.h>

struct cpufreq_info {
        u32 cpu_div[2];/* current cpu divisor */
        u32 plb2x_div;	/* current plb divisor */
        u32 pll;	/* PLL frequency is shared */
};

static struct cpufreq_info cpufreq;

struct cpufreq_data {
	u32 cpu_div;		/* cpu divisor */
	u32 plb2x_div[9];	/* plb2x divisor */
};

static struct cpufreq_data cpufreq_table[] = {
	{ 2, {2, 3, 4, 5, 6, 8, 10, 12, 16}},
	{ 3, {0, 3, 0, 0, 6, 0,  0, 12,  0}},
	{ 4, {2, 0, 4, 0, 6, 8, 10, 12, 16}},
	{ 5, {0, 0, 0, 5, 0, 0, 10,  0,  0}},
	{ 6, {0, 3, 0, 0, 6, 0,  0, 12,  0}},
	{ 8, {0, 0, 4, 0, 0, 8,  0, 12, 16}},
	{10, {0, 0, 0, 5, 0, 0, 10,  0,  0}},
	{12, {0, 0, 0, 0, 6, 0,  0, 12,  0}},
	{16, {0, 0, 0, 0, 0, 8,  0,  0, 16}},
	{20, {0, 0, 0, 0, 0, 0, 10,  0,  0}},
	{24, {0, 0, 0, 0, 0, 0,  0, 12,  0}},
	{32, {0, 0, 0, 0, 0, 0,  0,  0, 16}}
};

static int cpufreq_size = ARRAY_SIZE(cpufreq_table);
static int plbfreq_size = 9;

static u32 cpufreq_calc(u32 cpu_div)
{
	return (cpufreq.pll/cpu_div); /* Mhz */
}

static u32 plbfreq_calc(u32 plb2x_div)
{
	return (cpufreq.pll/(2*plb2x_div)); /* Mhz */
}

static u32 cpufreq_div(u32 cpu)
{
	return (cpufreq.pll/cpu); /* cpu divisor */
}

static u32 plbfreq_div(u32 plb)
{
	return (cpufreq.pll/(2*plb)); /* plb2x divisor */
}

static void cpufreq_print(void)
{
	int c, p;

	for (c = 0; c < cpufreq_size; c++) {
		for (p = 0; p < plbfreq_size; p++) {
			if (cpufreq_table[c].plb2x_div[p] == 0)
				continue; 

			printf("%4d-%d", cpufreq_calc(cpufreq_table[c].cpu_div),
				plbfreq_calc(cpufreq_table[c].plb2x_div[p]));

			if (cpufreq_table[c].cpu_div == cpufreq.cpu_div[0] &&
			    cpufreq_table[c].cpu_div == cpufreq.cpu_div[1] &&
			    cpufreq_table[c].plb2x_div[p] == cpufreq.plb2x_div)
				printf(" ***");
			else if (cpufreq_table[c].cpu_div == cpufreq.cpu_div[0] &&
			    cpufreq_table[c].plb2x_div[p] == cpufreq.plb2x_div)
				printf(" *0*");
			else if (cpufreq_table[c].cpu_div == cpufreq.cpu_div[1] &&
			    cpufreq_table[c].plb2x_div[p] == cpufreq.plb2x_div)
				printf(" *1*");
			
			printf("\n");
		}
	}
}

static int cpufreq_valid(u32 cpu_div, u32 plb2x_div)
{
	int c, p;

	for (c = 0; c < cpufreq_size; c++) {
		if (cpufreq_table[c].cpu_div != cpu_div)
			continue;

		for (p = 0; p < plbfreq_size; p++) {
			if (cpufreq_table[c].plb2x_div[p] == 0)
				continue;
		
			if (cpufreq_table[c].plb2x_div[p] == plb2x_div) {
				return 1;
			}
		}
	}

	return 0;
}

static int cpufreq_table_index(u32 cpu_div, u32 plb2x_div, int *ci, int *pi)
{
	int c, p;

	for (c = 0; c < cpufreq_size; c++) {
		if (cpufreq_table[c].cpu_div != cpu_div)
			continue;
			
		for (p = 0; p < plbfreq_size; p++) {
			if (cpufreq_table[c].plb2x_div[p] == plb2x_div) {
				*ci = c;
				*pi = p;
				return 0;
			}
		}
	}

	return -1;
}

static int cpufreq_step(int cc, int nc, int cp)
{
	int c;

	if (cc < nc) {
		for (c = (cc + 1); c <= nc; c++) {
			if (cpufreq_table[c].plb2x_div[cp] == 0)
				continue;

			apm86xxx_set_cpu_divisor2(0, cpufreq_table[c].cpu_div);
			apm86xxx_set_cpu_divisor2(1, cpufreq_table[c].cpu_div);

			printf("%4d-%d\n",
				cpufreq_calc(cpufreq_table[c].cpu_div),
				plbfreq_calc(cpufreq_table[c].plb2x_div[cp]));
		}
	} else if (cc > nc) {
		for (c = (cc - 1); c >= nc; c--) {
			if (cpufreq_table[c].plb2x_div[cp] == 0)
				continue;

			apm86xxx_set_cpu_divisor2(0, cpufreq_table[c].cpu_div);
			apm86xxx_set_cpu_divisor2(1, cpufreq_table[c].cpu_div);

			printf("%4d-%d\n",
				cpufreq_calc(cpufreq_table[c].cpu_div),
				plbfreq_calc(cpufreq_table[c].plb2x_div[cp]));
		}
	}

	return 0;
}

static int plbfreq_step(int cp, int np)
{
	int p;

	if (cp < np) {
		for (p = (cp + 1); p <= np; p++) {
			if (cpufreq_table[0].plb2x_div[p] == 0)
				continue;

			apm86xxx_set_plb2x_divisor2(
				cpufreq_table[0].plb2x_div[p]);

			printf("%4d-%d\n",
				cpufreq_calc(cpufreq_table[0].cpu_div),
				plbfreq_calc(cpufreq_table[0].plb2x_div[p]));
		}
	} else if (cp > np) {
		for (p = (cp - 1); p >= np; p--) {
			if (cpufreq_table[0].plb2x_div[p] == 0)
				continue;

			apm86xxx_set_plb2x_divisor2(
				cpufreq_table[0].plb2x_div[p]);

			printf("%4d-%d\n",
				cpufreq_calc(cpufreq_table[0].cpu_div),
				plbfreq_calc(cpufreq_table[0].plb2x_div[p]));
		}
	}
	
	return 0;
}

static int cpufreq_scale(u32 cpu_div, u32 plb2x_div)
{
	int cc = 0;
	int cp = 0;
	int nc = 0;
	int np = 0;

	/* find out where we are currently at */
	cpufreq_table_index(cpufreq.cpu_div[0], cpufreq.plb2x_div, &cc, &cp);

	printf("%4d-%d\n",
		cpufreq_calc(cpufreq_table[cc].cpu_div),
		plbfreq_calc(cpufreq_table[cc].plb2x_div[cp]));

	/* find out where we we want to go */
	cpufreq_table_index(cpu_div, plb2x_div, &nc, &np);

	if (cp != np) {
		cpufreq_step(cc, 0, cp);
		cc = 0;
		plbfreq_step(cp, np);
	}

	cpufreq_step(cc, nc, np);

	return 0;
}

static int cpm_div_test(void)
{
	u32 i;
	u32 div;
	int ret;

	div = 0;
	ret = apm86xxx_get_plb2x_divisor2(&div);
	printf("plb2x_div = %d\n", div);

	if (ret)
		return 0;

	for (i=0; i < 2; i++) {
		div = 0;
		ret = apm86xxx_get_cpu_divisor2(i, &div);
		printf("cpu_div[%d] = %d\n", i, div);
		if (ret)
			return 0;
	}

	return 1;
}

static void extract_cpu_plb(char * input, u32 * cpu, u32 * plb)
{
	char * end;

	*cpu = simple_strtoul(input, &end, 10);

	if (*end == '-') {
		end++;
		*plb = simple_strtoul(end, NULL, 10);
	}
}

static int do_cpufreq(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int ret;
        u32 freq, div;
	u32 plb, cpu;
	u32 plb2x_div, cpu_div;

        ret = apm86xxx_get_freq(APM86xxx_CPU_PLL, &freq);
	if (ret) {
		printf("Failed to get CPU PLL\n");
		return -1;
	}

	cpufreq.pll = (freq / (1000*1000));

	apm86xxx_get_cpu_divisor2(0, &div);
	cpufreq.cpu_div[0] = div;

	apm86xxx_get_cpu_divisor2(1, &div);
	cpufreq.cpu_div[1] = div;

	apm86xxx_get_plb2x_divisor2(&div);
	cpufreq.plb2x_div = div;


	if (argc < 2) {
		printf("Available Frequencies (CPU-PLB):\n");
		cpufreq_print();
		return 0;
	}

	if (mfspr(SPRN_PIR) != CORE0_ID) {
		printf("Updating Frequency can only be done by CPU0\n");
		return -1;
	}

	if (strcmp(argv[1], "div_test") == 0) {
		int i = 0;
		while (cpm_div_test()) {
			printf("count = %d\n", i++);
		}

		return 0;
	}

	cpu = 0;
	cpu_div = 0;
	plb = 0;
	plb2x_div = 0;

	extract_cpu_plb(argv[1], &cpu, &plb);

	cpu_div = cpufreq_div(cpu);
	plb2x_div = plbfreq_div(plb);

	if (cpufreq_valid(cpu_div, plb2x_div) == 0) {
		printf("Invalid CPU-PLB Frequency\n");
		return -1;
	}

	return cpufreq_scale(cpu_div, plb2x_div);
}

U_BOOT_CMD(
	cpufreq,	2,	0,	do_cpufreq,
	"Program the CPU and PLB frequency",
	"           - list available CPU and PLB frequenccies\n"
	"cpufreq <cpu-plb>  - update CPU and PLB frequency\n"
	"cpufreq <div_test> - run div_test"
);
