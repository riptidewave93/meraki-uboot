/*
 * Driver for NXP's 8 bit I2C gpio expanders (eg pca9670)
 */

#include <common.h>
#include <i2c.h>
#include <pca9670.h>

/* Default to an address that hopefully won't corrupt other i2c devices */
#ifndef CONFIG_SYS_I2C_PCA9670_ADDR
#define CONFIG_SYS_I2C_PCA9670_ADDR	0x20
#endif

/*Global data to hold current output value */
uint8_t pca_data = 0x81; /* Initilize with GPIO_SEL# and SHUTDOWN# are set to 1 (NOR active)*/


enum {
	PCA9670_CMD_DEVICE,
	PCA9670_CMD_OUTPUT,
	PCA9670_CMD_INPUT,
	PCA9670_CMD_RESET,
};

int pca9670_reload(uint8_t chip)
{
	uint8_t val;
	
	val  = (pca_data & 0x7F) ;

        return i2c_write(chip, val, 1, &val, 1);
}

/*
 * Set output value of IO pins in 'mask' to corresponding value in 'data'
 * 0 = low, 1 = high
 */
int pca9670_set_val(uint8_t chip, uint mask, uint data)
{
	uint8_t val;

	val = pca_data;
	val &= ~mask;
	val |= data;
	pca_data = val;
	return i2c_write(chip, val, 1, &val, 1);
}

/*
 * Read current logic level of all IO pins
 */
int pca9670_get_val(uint8_t chip)
{
	return (int)pca_data;
}

/*
 * Reset pca9670
 */
#define  pca9670_reset  i2c_write(0x0, 0x06, 1, 0x0, 0)

#ifdef CONFIG_CMD_PCA9670

cmd_tbl_t cmd_pca9670[] = {
	U_BOOT_CMD_MKENT(device, 3, 0, (void *)PCA9670_CMD_DEVICE, "", ""),
	U_BOOT_CMD_MKENT(output, 4, 0, (void *)PCA9670_CMD_OUTPUT, "", ""),
	U_BOOT_CMD_MKENT(input, 3, 0, (void *)PCA9670_CMD_INPUT, "", ""),
	U_BOOT_CMD_MKENT(reset, 3, 0, (void *)PCA9670_CMD_RESET, "", ""),
};


int do_pca9670(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	static uint8_t chip = CONFIG_SYS_I2C_PCA9670_ADDR;
	int val;
	ulong ul_arg2 = 0;
	ulong ul_arg3 = 0;
	cmd_tbl_t *c;

	c = find_cmd_tbl(argv[1], cmd_pca9670, ARRAY_SIZE(cmd_pca9670));

	/* All commands but "device" require 'maxargs' arguments */
	if (!c || !((argc == (c->maxargs)) ||
		(((int)c->cmd == PCA9670_CMD_DEVICE) &&
		 (argc == (c->maxargs - 1))))) {
		cmd_usage(cmdtp);
		return 1;
	}

	/* arg2 used as chip number or pin number */
	if (argc > 2)
		ul_arg2 = simple_strtoul(argv[2], NULL, 16);

	/* arg3 used as pin or invert value */
	if (argc > 3)
		ul_arg3 = simple_strtoul(argv[3], NULL, 16) & 0x1;

	switch ((int)c->cmd) {

	case PCA9670_CMD_DEVICE:
		if (argc == 3)
			chip = (uint8_t)ul_arg2;
		printf("Current device address: 0x%x\n", chip);
		return 0;
	case PCA9670_CMD_INPUT:
		val = (pca9670_get_val(chip) & (1 << ul_arg2)) != 0;

		printf("chip 0x%02x, pin 0x%lx = %d\n", chip, ul_arg2, val);
		return val;
	case PCA9670_CMD_OUTPUT:
		return pca9670_set_val(chip, (1 << ul_arg2),
					(ul_arg3 << ul_arg2));
	case PCA9670_CMD_RESET:
		pca9670_reset;
		return 0;
	default:
		/* We should never get here */
		return 1;
	}
}

U_BOOT_CMD(
	pca9670,	5,	1,	do_pca9670,
	"pca953x gpio access",
	"device [dev]\n"
	"	- show or set current device address\n"
	"pca953x output pin 0|1\n"
	"	- set pin as output and drive low or high\n"
	"pca953x intput pin\n"
	"	- set pin as input and read value"
	"pca953x reset\n"
        "       - reset device"
);

#endif /* CONFIG_CMD_PCA9670 */
