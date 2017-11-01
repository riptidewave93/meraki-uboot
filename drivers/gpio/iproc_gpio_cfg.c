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

#include "iproc_gpio.h"
#include "iproc_gpio_cfg.h"

#define readl(addr) (*(volatile unsigned int*)(addr))
#define writel(b,addr) ((*(volatile unsigned int *) (addr)) = (b))
#define __raw_readl readl
#define __raw_writel writel


static regValuePair curRegValue[]=
{
	{NORTHSTAR_DMU_BASE + IPROC_GPIO_CCA_CTRL0,            0},
	{NORTHSTAR_DMU_BASE + IPROC_GPIO_CCA_DS2,              0},
	{NORTHSTAR_DMU_BASE + IPROC_GPIO_CCA_DS1,              0},
	{NORTHSTAR_DMU_BASE + IPROC_GPIO_CCA_DS0,              0},
	{NORTHSTAR_DMU_BASE + IPROC_GPIO_CCA_INPUT_DISABLE,    0},
	{NORTHSTAR_DMU_BASE + IPROC_GPIO_CCA_HYSTERESIS,       0},
	{NORTHSTAR_DMU_BASE + IPROC_GPIO_CCA_SLEWRATE,         0},
	{NORTHSTAR_DMU_BASE + IPROC_GPIO_CCA_PULL_UP,          0},
	{NORTHSTAR_DMU_BASE + IPROC_GPIO_CCA_PULL_DOWN,        0},

};

void iproc_gpio_saveCFGRegisters( void )
{
	int i, size;
	size = sizeof ( curRegValue )/ sizeof( regValuePair );
	for ( i = 0; i < size; i++ )
	{
		curRegValue[i].value = __raw_readl(curRegValue[i].regAddr);
	}
}
int iproc_gpio_restoreRegisters( void )
{
	int i, size;
	size = sizeof ( curRegValue )/ sizeof( regValuePair );
	for ( i = 0; i < size; i++ )
	{
		__raw_writel(curRegValue[i].value,curRegValue[i].regAddr);
		//if ( curRegValue[i].value !=  __raw_readl(curRegValue[i].regAddr))
		//{
		//	return -1;
		//}
	}
	return 0;
}
//set pin configuration: GPIO or iproc function
int iproc_gpio_set_config(iproc_gpio_chip *chip,
			      unsigned int off, unsigned int cfg)
{
    unsigned long aux_sel;
    unsigned int aux_sel_reg;
    unsigned int base;

    base = 0;
    if (chip->id == IPROC_GPIO_CCA_ID) {
        aux_sel_reg = IPROC_GPIO_CCA_CTRL0;
        base = (unsigned int)chip->dmu_ioaddr;
    }
    aux_sel = __raw_readl(base + aux_sel_reg);

    switch (cfg) {
        case IPROC_GPIO_GENERAL:
            if (chip->id == IPROC_GPIO_CCA_ID) {
                aux_sel |= (1 << off);
            } else {
                aux_sel &= ~(1 << off);
            }
            break;            
        case IPROC_GPIO_AUX_FUN:
            if (chip->id == IPROC_GPIO_CCA_ID) {
                aux_sel &= ~(1 << off);
            } else {
                aux_sel |= (1 << off);
            }
            break;
        default:
            return -1;
    }
    __raw_writel(aux_sel, base + aux_sel_reg);

    return 0;
}

//Get pin configuration: GPIO or iproc
unsigned iproc_gpio_get_config(iproc_gpio_chip *chip,
				   unsigned int off)
{
    unsigned long aux_sel;
    unsigned int aux_sel_reg;
    unsigned int base;

    base = 0;
    if (chip->id == IPROC_GPIO_CCA_ID) {
        aux_sel_reg = IPROC_GPIO_CCA_CTRL0;
        base = (unsigned int)chip->dmu_ioaddr;
    }
    aux_sel = __raw_readl(base + aux_sel_reg);

    if (aux_sel & (1 << off)) {
        if (chip->id == IPROC_GPIO_CCA_ID) {
            return IPROC_GPIO_GENERAL;
        } else {
            return IPROC_GPIO_AUX_FUN;
        }
    }
    else {
        if (chip->id == IPROC_GPIO_CCA_ID) {
            return IPROC_GPIO_AUX_FUN;
        } else {
            return IPROC_GPIO_GENERAL;
        }        
    }        
}


int iproc_gpio_setpull_updown(iproc_gpio_chip *chip,
			    unsigned int off, iproc_gpio_pull_t pull)
{
    unsigned int base;
    base = 0;

    if (chip->id == IPROC_GPIO_CCA_ID) {
        unsigned long pull_up, pull_down;

        base = (unsigned int)chip->dmu_ioaddr;

        pull_up = __raw_readl(base + IPROC_GPIO_CCA_PULL_UP);
        pull_down = __raw_readl(base + IPROC_GPIO_CCA_PULL_DOWN);

        switch (pull) {
            case IPROC_GPIO_PULL_UP:
                pull_up |= (1 << off);
                pull_down &= ~(1 << off);
                break;
            case IPROC_GPIO_PULL_DOWN:
                pull_up &= ~(1 << off);
                pull_down |= (1 << off);
                break;
            case IPROC_GPIO_PULL_NONE:
                pull_up &= ~(1 << off);
                pull_down &= ~(1 << off);
                break;
            default:
                return -1;
        }
        __raw_writel(pull_up, base + IPROC_GPIO_CCA_PULL_UP);
        __raw_writel(pull_down, base + IPROC_GPIO_CCA_PULL_DOWN);

    }
    return 0;
}


iproc_gpio_pull_t iproc_gpio_getpull_updown(iproc_gpio_chip *chip,
					unsigned int off)
{

    unsigned int base;
    base = 0;

    if (chip->id == IPROC_GPIO_CCA_ID) {
        unsigned long pull_up, pull_down;

        base = (unsigned int)chip->dmu_ioaddr;

        pull_up = __raw_readl(base + IPROC_GPIO_CCA_PULL_UP);
        pull_down = __raw_readl(base + IPROC_GPIO_CCA_PULL_DOWN);
        pull_up &= 1 << off;
        pull_down &= 1 << off;

        if (pull_up ^ pull_down) {
            if (pull_up) {
                return IPROC_GPIO_PULL_UP;
            } else {
                return IPROC_GPIO_PULL_DOWN;
            }                
        } else if(!pull_up) {
            return IPROC_GPIO_PULL_NONE;
        }
    }
    return IPROC_GPIO_PULL_NONE;
}
int iproc_gpio_setHyeteresis(iproc_gpio_chip *chip,
			    unsigned int off, HysteresisEnable enableDisable)
{
    unsigned int base;
    base = 0;

        unsigned long hysteresis;

        base = (unsigned int)chip->dmu_ioaddr;

        hysteresis = __raw_readl(base + IPROC_GPIO_CCA_HYSTERESIS);

        switch (enableDisable) {
            case ENABLE:
            	hysteresis |= (1 << off);
                break;
            case DISABLE:
            	hysteresis &= ~(1 << off);
                break;
            default:
                return -1;
        }
        __raw_writel(hysteresis, base + IPROC_GPIO_CCA_HYSTERESIS);

    return 0;
}


HysteresisEnable iproc_gpio_getHyeteresis(iproc_gpio_chip *chip,
					unsigned int off)
{

	unsigned long hysteresis;
    unsigned int base;
    base = 0;

        base = (unsigned int)chip->dmu_ioaddr;

        hysteresis = __raw_readl(base + IPROC_GPIO_CCA_HYSTERESIS);

        hysteresis &= 1 << off;

        if (hysteresis)
        {
            return ENABLE;
        }
        else
        {
            return DISABLE;
        }
}
int iproc_gpio_setSlewrate(iproc_gpio_chip *chip,
			    unsigned int off, SlewRateCfg type)
{
    unsigned int base;
    unsigned long slewRate;
    base = 0;

        base = (unsigned int)chip->dmu_ioaddr;

        slewRate = __raw_readl(base + IPROC_GPIO_CCA_SLEWRATE);

        switch (type) {
            case SLEWED_EDGE:
            	slewRate |= (1 << off);
                break;
            case FAST_EDGE:
            	slewRate &= ~(1 << off);
                break;
            default:
                return -1;
        }
        __raw_writel(slewRate, base + IPROC_GPIO_CCA_SLEWRATE);

    return 0;
}


SlewRateCfg iproc_gpio_getSlewrate(iproc_gpio_chip *chip,
					unsigned int off)
{

    unsigned int base;
    unsigned long slewRate;
    base = 0;


        base = (unsigned int)chip->dmu_ioaddr;

        slewRate = __raw_readl(base + IPROC_GPIO_CCA_SLEWRATE);

        slewRate &= 1 << off;

        if (slewRate)
        {
            return SLEWED_EDGE;
        }
        else
        {
            return FAST_EDGE;
        }
}

int iproc_gpio_setDriveStrength(iproc_gpio_chip *chip,
			    unsigned int off, driveStrengthConfig ds)
{
    unsigned int base;
    base = 0;
    unsigned long ds0,ds1,ds2;



        base = (unsigned int)chip->dmu_ioaddr;

        ds2 = __raw_readl(base + IPROC_GPIO_CCA_DS2);
        ds1 = __raw_readl(base + IPROC_GPIO_CCA_DS1);
        ds0 = __raw_readl(base + IPROC_GPIO_CCA_DS0);


        switch (ds) {
            case d_2mA:
            	ds2 &= ~(1 << off);
            	ds1 &= ~(1 << off);
            	ds0 &= ~(1 << off);
                break;
            case d_4mA:
            	ds2 &= ~(1 << off);
            	ds1 &= ~(1 << off);
            	ds0 |= (1 << off);
                break;
            case d_6mA:
            	ds2 &= ~(1 << off);
            	ds1 |=  (1 << off);
            	ds0 &= ~(1 << off);
                break;
            case d_8mA:
            	ds2 &= ~(1 << off);
            	ds1 |=  (1 << off);
            	ds0 |=  (1 << off);
                break;
            case d_10mA:
            	ds2 |=  (1 << off);
            	ds1 &= ~(1 << off);
            	ds0 &= ~(1 << off);
                break;
            case d_12mA:
            	ds2 |=  (1 << off);
            	ds1 &= ~(1 << off);
            	ds0 |=  (1 << off);
                break;
            case d_14mA:
            	ds2 |=  (1 << off);
            	ds1 |=  (1 << off);
            	ds0 &=  ~(1 << off);
                break;
            case d_16mA:
            	ds2 |=  (1 << off);
            	ds1 |=  (1 << off);
            	ds0 |=  (1 << off);
                break;

            default:
                return -1;
        }
        __raw_writel(ds2, base + IPROC_GPIO_CCA_DS2);
        __raw_writel(ds1, base + IPROC_GPIO_CCA_DS1);
        __raw_writel(ds0, base + IPROC_GPIO_CCA_DS0);
    return 0;
}
driveStrengthConfig iproc_gpio_getDriveStrength(iproc_gpio_chip *chip,
					unsigned int offset)
{

    unsigned int base;
    base = 0;
    unsigned long ds0,ds1,ds2, ds;


        base = (unsigned int)chip->dmu_ioaddr;

        ds2 = __raw_readl(base + IPROC_GPIO_CCA_DS2);
        ds1 = __raw_readl(base + IPROC_GPIO_CCA_DS1);
        ds0 = __raw_readl(base + IPROC_GPIO_CCA_DS0);

        ds0 = (( 1 << offset ) & ds0 ) >> offset;
        ds1 = (( 1 << offset ) & ds1 ) >> offset;
        ds2 = (( 1 << offset ) & ds2 ) >> offset;
        ds = ((ds2  << 2 ) | (ds1 << 1 ) | ds0 );
        return ( (driveStrengthConfig)ds);

}
int iproc_gpio_setInputdisable (iproc_gpio_chip *chip,
			    unsigned int off, InputDisableCfg enableDisable)
{
    unsigned int base;
    base = 0;
    unsigned long inputDisabledCfg;


        base = (unsigned int)chip->dmu_ioaddr;

        inputDisabledCfg = __raw_readl(base + IPROC_GPIO_CCA_INPUT_DISABLE);

        switch (enableDisable) {
            case INPUT_DISABLE:
            	inputDisabledCfg |= (1 << off);
                break;
            case INPUT_ENABLE:
            	inputDisabledCfg &= ~(1 << off);
                break;
            default:
                return -1;
        }
        __raw_writel(inputDisabledCfg, base + IPROC_GPIO_CCA_INPUT_DISABLE);

    return 0;
}


InputDisableCfg iproc_gpio_getInputdisable(iproc_gpio_chip *chip,
					unsigned int off)
{

	unsigned long inputDisabledCfg;
    unsigned int base;
    base = 0;


        base = (unsigned int)chip->dmu_ioaddr;

        inputDisabledCfg = __raw_readl(base + IPROC_GPIO_CCA_INPUT_DISABLE);

        inputDisabledCfg &= 1 << off;

        if (inputDisabledCfg)
        {
            return INPUT_DISABLE;
        }
        else
        {
            return INPUT_ENABLE;
        }
}
