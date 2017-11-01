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

#ifndef __PLAT_GPIO_CFG_H
#define __PLAT_GPIO_CFG_H


typedef unsigned int iproc_gpio_drvstr_t;

#define IPROC_GPIO_GENERAL  1
#define IPROC_GPIO_AUX_FUN  0

/* Define values for the pull-{up,down} available for each gpio pin.
 *
 * These values control the state of the weak pull-{up,down} resistors.
 */
typedef enum gpioPullCfg
{
	IPROC_GPIO_PULL_NONE,
	IPROC_GPIO_PULL_DOWN,
	IPROC_GPIO_PULL_UP,
}iproc_gpio_pull_t;
typedef enum config
{
	DISABLE,
	ENABLE,
}HysteresisEnable;
typedef enum slewRate
{
	FAST_EDGE,
	SLEWED_EDGE,
}SlewRateCfg;
typedef enum driveStrength
{
    d_2mA,
    d_4mA,
    d_6mA,
    d_8mA,
    d_10mA,
    d_12mA,
    d_14mA,
    d_16mA,
}driveStrengthConfig;

typedef struct regValue {
	unsigned long regAddr;
	unsigned long value;
}regValuePair;

typedef enum inputDisable
{
	INPUT_ENABLE,
	INPUT_DISABLE
}InputDisableCfg;

/* internal gpio functions */
extern int iproc_gpio_setpull_updown(iproc_gpio_chip *chip,
			    unsigned int off, iproc_gpio_pull_t pull);

extern iproc_gpio_pull_t iproc_gpio_getpull_updown(iproc_gpio_chip *chip,
					unsigned int off);

extern int iproc_gpio_set_config(iproc_gpio_chip *chip,
			      unsigned int off, unsigned int cfg);
			      
unsigned iproc_gpio_get_config(iproc_gpio_chip *chip,
				   unsigned int off);
int iproc_gpio_setHyeteresis(iproc_gpio_chip *chip,
			    unsigned int off, HysteresisEnable enableDisable);
HysteresisEnable iproc_gpio_getHyeteresis(iproc_gpio_chip *chip,
					unsigned int off);
int iproc_gpio_setSlewrate(iproc_gpio_chip *chip,
			    unsigned int off, SlewRateCfg type);
SlewRateCfg iproc_gpio_getSlewrate(iproc_gpio_chip *chip,
					unsigned int off);
int iproc_gpio_setDriveStrength(iproc_gpio_chip *chip,
			    unsigned int off, driveStrengthConfig ds);
driveStrengthConfig iproc_gpio_getDriveStrength(iproc_gpio_chip *chip,
					unsigned int off);
int iproc_gpio_restoreRegisters( void );
void iproc_gpio_saveCFGRegisters( void );
InputDisableCfg iproc_gpio_getInputdisable(iproc_gpio_chip *chip,
					unsigned int off);
int iproc_gpio_setInputdisable (iproc_gpio_chip *chip,
			    unsigned int off, InputDisableCfg enableDisable);
#endif				   
