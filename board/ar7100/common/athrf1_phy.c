/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright c 2003 Atheros Communications, Inc.,  All Rights Reserved.
 */

/*
 * Manage the atheros ethernet PHY.
 *
 * All definitions in this file are operating system independent!
 */
#ifdef __BDI
#include "bdi.h"
#endif

#include <config.h>
#include <linux/types.h>
#include <common.h>
#include <miiphy.h>
#include "phy.h"
#include "ar7100_soc.h"

#include "athr_phy.h"
#define MODULE_NAME "ATHRF1E"

#define mdelay(_x)         udelay((_x)*1000)

typedef struct {
  int              is_enet_port;
  int              mac_unit;
  unsigned int     phy_addr;
}athr_phy_t;

athr_phy_t phy_info[] = {
#ifdef CFG_BOARD_PB47
    {is_enet_port: 1,
     mac_unit    : 1,
     phy_addr    : 0x00}
#else
    {is_enet_port: 1,
     mac_unit    : 0,
     phy_addr    : 0x00},
    {is_enet_port: 1,
     mac_unit    : 1,
     phy_addr    : 0x01}
#endif
};

/* Add by Ken */
static uint16_t
ag7100_mii_read(uint32_t phybase, uint16_t phyaddr, uint16_t reg)
{
    uint16_t val;

    phy_reg_read(phybase, phyaddr, reg, &val);
    return val;
}

static void
ag7100_mii_write(uint32_t phybase, uint16_t phyaddr, uint16_t reg, uint32_t val)
{
    phy_reg_write(phybase, phyaddr, reg, val);
}
//end of Ken

static athr_phy_t *
athr_phy_find(int unit)
{
    int i;
    athr_phy_t *phy;

    for(i = 0; i < sizeof(phy_info)/sizeof(athr_phy_t); i++) {
        phy = &phy_info[i];
        
        if (phy->is_enet_port && (phy->mac_unit == unit)) 
            return phy;
    }
    
    return NULL;
}

int
athr_phy_setup(int unit)
{
    athr_phy_t *phy = athr_phy_find(unit);
    uint16_t  phyHwStatus;
    uint16_t  timeout;

    if (!phy) {
        printf(MODULE_NAME": \nNo phy found for unit %d\n", unit);
        return;
    }
    
     /*
     * After the phy is reset, it takes a little while before
     * it can respond properly.
     */

    phy_reg_write(unit, phy->phy_addr, ATHR_AUTONEG_ADVERT,
                  ATHR_ADVERTISE_ALL);

    phy_reg_write(unit, phy->phy_addr, ATHR_1000BASET_CONTROL,
                  ATHR_ADVERTISE_1000FULL);

    /* delay rx_clk */
    phy_reg_write(unit, phy->phy_addr, 0x1D, 0x0);
#if 0	//Alex:20100806
    phy_reg_write(unit, phy->phy_addr, 0x1E, 0x34E); /* 0x24E - no delay, 0x34E - 2ns delay */
//Alex:20100806 - {
#else
    phy_reg_write(unit, phy->phy_addr, 0x1E, 0x24E); /* 0x24E - no delay, 0x34E - 2ns delay */
#endif
//Alex:20100806 - }
    /* delay tx_clk */
    phy_reg_write(unit, phy->phy_addr, 0x1D, 0x5);
#if 0	//Alex:20100806
    phy_reg_write(unit, phy->phy_addr, 0x1E, 0x3C47); /* 0x3C47 - no delay, 0x3D47 - 1.5ns delay */
//Alex:20100806 - {
#else
    phy_reg_write(unit, phy->phy_addr, 0x1E, 0x3D47); /* 0x3C47 - no delay, 0x3D47 - 1.5ns delay */
#endif
//Alex:20100806 - }

    /* Reset PHYs*/
    phy_reg_write(unit, phy->phy_addr, ATHR_PHY_CONTROL,
                  ATHR_CTRL_AUTONEGOTIATION_ENABLE 
                  | ATHR_CTRL_SOFTWARE_RESET);
    //KK
    mdelay(500);

    /*
     * Wait up to 3 seconds for ALL associated PHYs to finish
     * autonegotiation.  The only way we get out of here sooner is
     * if ALL PHYs are connected AND finish autonegotiation.
     */
    for (timeout=20; timeout; mdelay(150), timeout--) {
        phy_reg_read(unit, phy->phy_addr, ATHR_PHY_CONTROL, &phyHwStatus);

        if (!ATHR_RESET_DONE(phyHwStatus))
            continue;

        phy_reg_read(unit, phy->phy_addr, ATHR_PHY_STATUS, &phyHwStatus);
        if (ATHR_AUTONEG_DONE(phyHwStatus)) {
                printf(MODULE_NAME": Port %d, Neg Success\n", unit);
                break;
        }
    }
    if (timeout == 0)
        printf(MODULE_NAME": Port %d, Negotiation timeout\n", unit);

    printf(MODULE_NAME": unit %d phy addr %x ", unit, phy->phy_addr);
    printf(MODULE_NAME": reg0 %x\n", ag7100_mii_read(0, phy->phy_addr, 0));
}

int
athr_phy_is_up(int unit)
{
    int status;
    athr_phy_t *phy = athr_phy_find(unit);

    if (!phy) 
        return 0;
    status = ag7100_mii_read(0, phy->phy_addr, ATHR_PHY_SPEC_STATUS);

    if (status & ATHR_STATUS_LINK_PASS)
        return 1;

    return 0;
}

int
athr_phy_is_fdx(int unit)
{
    int status;
    athr_phy_t *phy = athr_phy_find(unit);
    int ii = 200;

    if (!phy) 
        return 0;
    do {
    status = ag7100_mii_read(0, phy->phy_addr, ATHR_PHY_SPEC_STATUS);
	     //KK
             mdelay(10);
    } while((!(status & ATHR_STATUS_RESOVLED)) && --ii);
    
    //status = !(!(status & ATHER_STATUS_FULL_DEPLEX));
    if (status & ATHER_STATUS_FULL_DEPLEX)
        status = FULL;
    else
        status = HALF;
    
    return (status);
}
int
athr_phy_speed(int unit)
{
    int status;
    athr_phy_t *phy = athr_phy_find(unit);
    int ii = 200;

    if (!phy) 
        return 0;
    do {
        status = ag7100_mii_read(0, phy->phy_addr, ATHR_PHY_SPEC_STATUS);
	    mdelay(10);
    }while((!(status & ATHR_STATUS_RESOVLED)) && --ii);

    status = ((status & ATHER_STATUS_LINK_MASK) >> ATHER_STATUS_LINK_SHIFT);

    switch(status) {
    case 0:
        return _10BASET;
    case 1:
        return _100BASET;
    case 2:
        return _1000BASET;
    default:
        printf(MODULE_NAME": Unkown speed read!\n");
    }
    return -1;
}
