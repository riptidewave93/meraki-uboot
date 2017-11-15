/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright © 2007 Atheros Communications, Inc.,  All Rights Reserved.
 */

/*
 * Manage the atheros ethernet PHY.
 *
 * All definitions in this file are operating system independent!
 */

#include <asm-ppc/types.h>
#include <config.h>
#include <common.h>
#include <net.h>
#include <asm/processor.h>
#include <asm/errno.h>
#include <asm/io.h>
#include <asm/cache.h>
#include <asm/mmu.h>
#include <commproc.h>
/*#include <ppc4xx.h>*/
/*#include <ppc4xx_enet.h>*/
#include <miiphy.h>
#include <malloc.h>
/*#include <asm/ppc4xx-intvec.h>*/
/*#include <board.h>*/
#include "athrs27_phy.h"
#include "../../cpu/ppc4xx/apm_enet_access.h"
#include "../../cpu/ppc4xx/apm_enet_mac.h"
#include "../../cpu/ppc4xx/apm_enet_csr.h"

#if 0
extern int emac4xx_miiphy_read (char *devname, unsigned char addr,
								unsigned char reg, unsigned short *value);
extern int emac4xx_miiphy_write (char *devname, unsigned char addr,
								 unsigned char reg, unsigned short value);
#else
#if defined(CONFIG_MII) || defined(CONFIG_CMD_MII)
extern int apm_eth_miiphy_read(char *devname, unsigned char addr, 
								unsigned char reg, unsigned short *value);
extern int apm_eth_miiphy_write(char *devname, unsigned char addr, 
								unsigned char reg, unsigned short value);
#endif
#endif

/* define DEBUG for debugging output (obviously ;-)) */


#define DEBUG
#define SHOW_INFO
#define CONFIG_ATHRS16_VERIFY

//#define LOW_DBG

#ifdef DEBUG
#define dev_dbg(format, arg...) printf(format , ## arg)
#else
#define dev_dbg(format, arg...) do {} while(0)
#endif /* DEBUG */

#ifdef LOW_DBG
#define low_dbg(format, arg...) printf(format , ## arg)
#else
#define low_dbg(format, arg...) do {} while(0)
#endif /* DEBUG */

#define err(format, arg...) printf(format , ## arg)
#ifdef SHOW_INFO
#define info(format, arg...) printf(format , ## arg)
#else
#define info(format, arg...) do {} while(0)
#endif /*DEBUG*/

/* PHY selections and access functions */
//#define S16_VER10
//#define FULL_FEATURE
#define CONFIG_PORT0_AS_SWITCH
typedef enum {
    PHY_SRCPORT_INFO, 
    PHY_PORTINFO_SIZE,
} PHY_CAP_TYPE;

typedef enum {
    PHY_SRCPORT_NONE,
    PHY_SRCPORT_VLANTAG, 
    PHY_SRCPORT_TRAILER,
} PHY_SRCPORT_TYPE;

#define ATHR_LAN_PORT_VLAN          1
#define ATHR_WAN_PORT_VLAN          2


/*depend on connection between cpu mac and s16 mac*/
#if defined (CONFIG_PORT0_AS_SWITCH)
#define ENET_UNIT_LAN 0  
#define ENET_UNIT_WAN 1
#define CFG_BOARD_AP96 1
#else
#define ENET_UNIT_LAN 1  
#define ENET_UNIT_WAN 0
#define CFG_BOARD_PB45 1
#endif

#define TRUE    1
#define FALSE   0

#define ATHR_PHY0_ADDR   0x0
#define ATHR_PHY1_ADDR   0x1
#define ATHR_PHY2_ADDR   0x2
#define ATHR_PHY3_ADDR   0x3
#define ATHR_PHY4_ADDR   0x4
#define ATHR_IND_PHY 4


#define MODULE_NAME "ATHRS16"

struct eth_device *device;		//after init, can use command ar from device get from dev in init

/*
 * Track per-PHY port information.
 */
typedef struct {
    BOOL   isEnetPort;       /* normal enet port */
    BOOL   isPhyAlive;       /* last known state of link */
    int    ethUnit;          /* MAC associated with this phy port */
    uint32_t phyBase;
    uint32_t phyAddr;          /* PHY registers associated with this phy port */
    uint32_t VLANTableSetting; /* Value to be written to VLAN table */
} athrPhyInfo_t;

/*
 * Per-PHY information, indexed by PHY unit number.
 */
static athrPhyInfo_t athrPhyInfo[] = {
    {TRUE,   /* phy port 0 -- LAN port 0 */
     FALSE,
     ENET_UNIT_LAN,
     0,
     ATHR_PHY0_ADDR,
     ATHR_LAN_PORT_VLAN
    },

    {TRUE,   /* phy port 1 -- LAN port 1 */
     FALSE,
     ENET_UNIT_LAN,
     0,
     ATHR_PHY1_ADDR,
     ATHR_LAN_PORT_VLAN
    },

    {TRUE,   /* phy port 2 -- LAN port 2 */
     FALSE,
     ENET_UNIT_LAN,
     0,
     ATHR_PHY2_ADDR, 
     ATHR_LAN_PORT_VLAN
    },

    {TRUE,   /* phy port 3 -- LAN port 3 */
     FALSE,
     ENET_UNIT_LAN,
     0,
     ATHR_PHY3_ADDR, 
     ATHR_LAN_PORT_VLAN
    },

    {TRUE,   /* phy port 4 -- WAN port or LAN port 4 */
     FALSE,
     ENET_UNIT_WAN,
     0,
     ATHR_PHY4_ADDR, 
     ATHR_LAN_PORT_VLAN   /* Send to all ports */
    },
    
    {FALSE,  /* phy port 5 -- CPU port (no RJ45 connector) */
     TRUE,
     ENET_UNIT_LAN,
     0,
     0x00, 
     ATHR_LAN_PORT_VLAN    /* Send to all ports */
    },
};

#define phy_reg_read        apm_eth_miiphy_read
#define phy_reg_write       apm_eth_miiphy_write

#define ATHR_FULL_DLX		0x00000040
#define ATHR_HALF_DLX		0x00000000
#define ATHR_SPEED_MASK		0x00000003
#define ATHR_CPU_PORT		0
static uint8_t athr16_init_flag = 0;
//static cmd_resp_t cmd_resp;
//static int wait_flag = 0;

//#define ATHR_PHY_MAX (sizeof(ipPhyInfo) / sizeof(ipPhyInfo[0]))
#define ATHR_PHY_MAX 5

/* Range of valid PHY IDs is [MIN..MAX] */
#define ATHR_ID_MIN 0
#define ATHR_ID_MAX (ATHR_PHY_MAX-1)

/* Convenience macros to access myPhyInfo */
#define ATHR_IS_ENET_PORT(phyUnit) (athrPhyInfo[phyUnit].isEnetPort)
#define ATHR_IS_PHY_ALIVE(phyUnit) (athrPhyInfo[phyUnit].isPhyAlive)
#define ATHR_ETHUNIT(phyUnit) (athrPhyInfo[phyUnit].ethUnit)
#define ATHR_PHYBASE(phyUnit) (athrPhyInfo[phyUnit].phyBase)
#define ATHR_PHYADDR(phyUnit) (athrPhyInfo[phyUnit].phyAddr)
#define ATHR_VLAN_TABLE_SETTING(phyUnit) (athrPhyInfo[phyUnit].VLANTableSetting)


#define ATHR_IS_ETHUNIT(phyUnit, ethUnit) \
            (ATHR_IS_ENET_PORT(phyUnit) &&        \
            ATHR_ETHUNIT(phyUnit) == (ethUnit))

#define ATHR_IS_WAN_PORT(phyUnit) (!(ATHR_ETHUNIT(phyUnit)==ENET_UNIT_LAN))
            

static inline void mdelay(int n)
{
	u32 ms = n;

	while (ms--)
		udelay(1000);
}

int emac4xx_athr16_read(char *devname, unsigned char addr,
							  unsigned char reg, unsigned short *value)
{
	dev_dbg("%s addr 0x%x reg 0x%x \n",__FUNCTION__,addr,reg);

	switch(reg) {
	case PHY_BMCR:
		*value = PHY_BMCR_DPLX | PHY_BMCR_1000_MBPS;
		break;
	case PHY_BMSR:
		*value = PHY_BMSR_AUTN_COMP | PHY_BMSR_LS | PHY_BMSR_EXT;
		break;
	case PHY_PHYIDR1:
		*value = 0x004D;
		break;
	case PHY_PHYIDR2:
		*value = 0xD041;
		break;
	case PHY_1000BTSR:
		*value = PHY_1000BTSR_LRS | PHY_1000BTSR_RRS | PHY_1000BTSR_1000FD;
		break;
	case PHY_MIPSCR:
		*value = PHY_SSR_1000_MBPS | PHY_SSR_FD;
		break;
	case PHY_EXSR:
		*value = PHY_EXSR_1000TF;
		break;
	default:
		*value = 0;
		break;
	}
	return 0;
}

int emac4xx_athr16_write (char *devname, unsigned char addr,
								unsigned char reg, unsigned short value)
{
	dev_dbg("%s addr 0x%x reg 0x%x \n",__FUNCTION__,addr,reg);
	return 0;
}

int athrs16_header_write(struct eth_device *dev, uint32_t dev_id, uint32_t reg_addr, uint8_t *reg_data, uint32_t len)
{
    athrs16_reg_write(dev, reg_addr, *(uint32_t *)reg_data);
    return 0;
}

int athrs16_header_read(struct eth_device *dev, uint32_t dev_id, uint32_t reg_addr, uint8_t *reg_data, uint32_t len)
{    
	  uint32_t data;
    
    data = athrs16_reg_read(dev, reg_addr);
    *(uint32_t*)reg_data = data;
    return 0;
}

int athrs16_phy_write(struct eth_device *dev, u32 phy_addr, u32 reg, u16 data)
{
	low_dbg("%s, phy_addr: 0x%x, reg: 0x%x,data: 0x%x\n",__FUNCTION__,phy_addr,reg,data);
    phy_reg_write(dev->name, phy_addr, reg, data);
    return 0;
}

int athrs16_phy_read(struct eth_device *dev, u32 phy_addr, u32 reg, u16 *data)
{    
    phy_reg_read(dev->name, phy_addr, reg, data);
	low_dbg("%s, phy_addr: 0x%x, reg: 0x%x,data: 0x%x\n",__FUNCTION__,phy_addr,reg,*data);
    return 0;
}

void phy_mode_setup(struct eth_device *dev) 
{
#ifdef S16_VER10
    info("phy_mode_setup\n");

    /*work around for phy4 rgmii mode*/
    athrs16_phy_write(dev, ATHR_PHYADDR(ATHR_IND_PHY), 29, 18);     
    athrs16_phy_write(dev, ATHR_PHYADDR(ATHR_IND_PHY), 30, 0x480c);    
#if defined (CONFIG_ETH_PHY_TX_RX_DELAY)
    /*rx delay*/ 
    athrs16_phy_write(dev, ATHR_PHYADDR(ATHR_IND_PHY), 29, 0);     
    athrs16_phy_write(dev, ATHR_PHYADDR(ATHR_IND_PHY), 30, 0x824e);  

    /*tx delay*/ 
    athrs16_phy_write(dev, ATHR_PHYADDR(ATHR_IND_PHY), 29, 5);     
    athrs16_phy_write(dev, ATHR_PHYADDR(ATHR_IND_PHY), 30, 0x3d47);    
#endif /*CONFIG_ETH_PHY_TX_RX_DELAY*/
#endif
}

/* test : if no header support in boot loader */
/* Used to access internal registers (not PHY's registers) */
void athrs16_mdio_write(struct eth_device *dev, uint32_t reg_addr, uint32_t reg_val)
{
    uint32_t reg_word_addr;
    uint32_t phy_addr;
    uint16_t phy_val;
    uint8_t phy_reg;
	low_dbg("%s\n",__FUNCTION__);
#if defined(DEBUG)
	printf("reg 0x%x value: 0x%x\n",reg_addr,reg_val);
#endif /*DEBUG*/  
    /* change reg_addr to 16-bit word address, 32-bit aligned */
    reg_word_addr = (reg_addr & 0xfffffffc) >> 1;

    /* configure register high address */
    phy_addr = 0x18;
    phy_reg = 0x0;
    phy_val = (uint16_t) ((reg_word_addr >> 8) & 0x1ff);  /* bit16-8 of reg address */
    athrs16_phy_write(dev, phy_addr, phy_reg, phy_val);

    /* For some registers such as ARL and VLAN, since they include BUSY bit */
    /* in lower address, we should write the higher 16-bit register then the */
    /* lower one */

    /* read register in higher address */
    reg_word_addr++;
    phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
    phy_reg = (uint8_t) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
    phy_val = (uint16_t) ((reg_val >> 16) & 0xffff);
    athrs16_phy_write(dev, phy_addr, phy_reg, phy_val);

    /* write register in lower address */
    reg_word_addr--;
    phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
    phy_reg = (uint8_t) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
    phy_val = (uint16_t) (reg_val & 0xffff);
    athrs16_phy_write(dev, phy_addr, phy_reg, phy_val);
}

uint32_t athrs16_mdio_read(struct eth_device *dev, uint32_t reg_addr) 
{
    uint32_t reg_word_addr;
    uint32_t phy_addr, retval;
    uint16_t phy_val;
    uint8_t phy_reg;
	uint16_t tmp_val, reg_val;
	low_dbg("%s\n",__FUNCTION__);

    /* change reg_addr to 16-bit word address, 32-bit aligned */
    reg_word_addr = (reg_addr & 0xfffffffc) >> 1;

    /* configure register high address */
    phy_addr = 0x18;
    phy_reg = 0x0;
    phy_val = (uint16_t) ((reg_word_addr >> 8) & 0x1ff);  /* bit16-8 of reg address */
    athrs16_phy_write(dev, phy_addr, phy_reg, phy_val);

    /* For some registers such as MIBs, since it is read/clear, we should */
    /* read the lower 16-bit register then the higher one */

    /* read register in lower address */
    phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
    phy_reg = (uint8_t) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
    athrs16_phy_read(dev, phy_addr, phy_reg, &reg_val);

    /* read register in higher address */
    reg_word_addr++;
    phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
    phy_reg = (uint8_t) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
	athrs16_phy_read(dev, phy_addr, phy_reg, &tmp_val);

    retval = reg_val | (tmp_val << 16);
#if defined(DEBUG)
	printf("reg 0x%x value: 0x%x\n",reg_addr,retval);
#endif /*DEBUG*/  
	return retval;   
}

void header_config_prepare(struct eth_device *dev)
{
#ifdef HEADER_EN
    info("%s: set port0 header enable\n", __func__);
    athrs16_mdio_write(dev, 0x104, athrs16_mdio_read(dev, 0x104)|0x800);
#endif

#ifdef HEADER_REG_CONF
    info("%s: set cpuport enable\n", __func__);
    athrs16_mdio_write(dev, 0x78, athrs16_mdio_read(dev, 0x78)|0x100);
#endif   
    info("%s: set port0 enable \n", __func__);
    athrs16_mdio_write(dev, 0x100, 0x7e);
    
    info("%s: set power on strip:0x012E1bea \n", __func__);
    athrs16_mdio_write(dev, 0x8, 0x012E1bea);
}

/*
void athrs16_mdio_dump(struct eth_device *dev){
	athrs16_reg_read(dev,ATHR_PWS_REG);
	athrs16_reg_read(dev,ATHR_GLOBAL_FW_CTRL1);
	athrs16_reg_read(dev,ATHR_PORT0_PAD_MODE_CTRL);
	athrs16_reg_read(dev,ATHR_PORT6_PAD_MODE_CTRL);
	athrs16_reg_read(dev,ATHR_PORT0_STATUS);
}
*/

/************************************************************************/
/* init speed of CPU port,												*/
/*	speed = 10/100/1000													*/
/*	cpu_port_num = 0/5/6												*/
/*	duplex = ATHR_FULL_DLX/ATHR_HALF_DLX				 				*/
/************************************************************************/
void athrs16_speed_init(struct eth_device *dev, int cpu_port_num,u32 speed, u32 duplex)
{
	uint32_t val32;
	uint32_t reg = 0;
	dev_dbg("%s\n",__FUNCTION__);

	switch (cpu_port_num) 
	{
		case 0:
			reg = ATHR_PORT0_STATUS;
			break;
		case 5:
			reg = ATHR_PORT5_STATUS;
			break;
		case 6:
			reg = ATHR_PORT6_STATUS;
			break;
		default:
			printf("Invalid Port Number, only ports 0..6 are supported\n");
			break;
	}
	val32 = athrs16_reg_read(dev,reg);
	val32 &= ~ATHR_SPEED_MASK;
	dev_dbg("config value for speed and duplex: 0x%x\n",val32);
	val32 |= ((speed == 1000) ? 0x02 : ((speed == 100) ? 0x01 : 0x00));
	dev_dbg("config value for speed and duplex: 0x%x\n",val32);
	val32 |= duplex;
	dev_dbg("config value for speed and duplex: 0x%x\n",val32);
	athrs16_reg_write(dev, reg, val32);
}

u32 athrs16_speed_duplex(struct eth_device *dev, int cpu_port_num)
{
	u32 reg = 0;

	switch (cpu_port_num) 
	{
	case 0:
		reg = ATHR_PORT0_STATUS;
		break;
	case 5:
		reg = ATHR_PORT5_STATUS;
		break;
	case 6:
		reg = ATHR_PORT6_STATUS;
		break;
	default:
		printf("Invalid Port Number, only ports 0..6 are supported\n");
		break;
	}
	return athrs16_reg_read(dev,reg);
}

void athrs16_reg_init(struct eth_device *dev)
{
	info("%s: in\n", __FUNCTION__);

	device = dev;

	//int phyaddr;
    /* if using header for register configuration, we have to     */
    /* configure s16 register after frame transmission is enabled */
    if (athr16_init_flag)
        return;
    //header_config_prepare();
	dev_dbg("%s\n",__FUNCTION__);

    /* Power on strip mode setup */
    athrs16_reg_write(dev, ATHR_PWS_REG, 0x40000000);
    
	/* Configure MAC0 as RGMII mode */
	athrs16_reg_write(dev, ATHR_GLOBAL_FW_CTRL1, 0x007f7f7f);
	athrs16_reg_write(dev, ATHR_PORT0_PAD_MODE_CTRL, 0x07600000);
	athrs16_reg_write(dev, ATHR_PORT6_PAD_MODE_CTRL, 0x01000000);
	athrs16_reg_write(dev, ATHR_PORT0_STATUS, 0x7e); /* initial speed is 1000 */
	//athrs16_speed_init(dev,ATHR_CPU_PORT,1000,ATHR_FULL_DLX);

	/* Enable link on 4 LAN ports */
#if defined(CONFIG_LAN_PORTS_ENABLE)
	athrs16_reg_write(dev, ATHR_PORT1_STATUS, 0x200);
	athrs16_reg_write(dev, ATHR_PORT2_STATUS, 0x200);
	athrs16_reg_write(dev, ATHR_PORT3_STATUS, 0x200);
	athrs16_reg_write(dev, ATHR_PORT4_STATUS, 0x200);
	/* Phy link mode enable.enable mac auto-neg with phy		*/
  	athrs16_reg_write(dev, ATHR_PORT6_STATUS, 0x200);
   
		
#endif /*CONFIG_LAN_PORTS_ENABLE*/

#if 0 /* This is not necessary for Port 5 of AR8327 */
#if 0
	/* Configure PHY4 RGMII mode */
	/* Debug register offset 0x12[3]=1 (Select RGMII interface with MAC) */
	athrs16_phy_write(dev,4,ATHR_DBG_PORT_ADDR,0x12);
	athrs16_phy_write(dev,4,ATHR_DBG_PORT_DATA,0x4c0c);
#endif 

#if defined (CONFIG_ETH_PHY_TX_RX_DELAY)
	/* Debug register offset 0x0[15]=1 (RGMII RX clock delay enable) */
	athrs16_phy_write(dev,4,ATHR_DBG_PORT_ADDR,0x00);
	athrs16_phy_write(dev,4,ATHR_DBG_PORT_DATA,0x82ee);

	/* Debug register offset 0x5[8]=1 (RGMII TX clock delay enable) */
	athrs16_phy_write(dev,4,ATHR_DBG_PORT_ADDR,0x05);
	athrs16_phy_write(dev,4,ATHR_DBG_PORT_DATA,0x3d46);
	/* End of Configure PHY4 RGMII mode */
#endif
#endif

#if 0
#if defined(CONFIG_VLAN_8021Q) || defined(CONFIG_VLAN_8021Q_MODULE)        
#ifdef HEADER_EN        
    athrs16_reg_write(dev, 0x104, 0x6804);
#else
    athrs16_reg_write(dev, 0x104, 0x6004);
#endif

    athrs16_reg_write(dev, 0x204, 0x6004);
    athrs16_reg_write(dev, 0x304, 0x6004);
    athrs16_reg_write(dev, 0x404, 0x6004);
    athrs16_reg_write(dev, 0x504, 0x6004);    
    athrs16_reg_write(dev, 0x604, 0x6004);    
#else
#ifdef HEADER_EN        
    athrs16_reg_write(dev, 0x104, 0x4804);
#else
    athrs16_reg_write(dev, 0x104, 0x4004);
#endif /* HEADER_EN */
#endif /* CONFIG_VLAN_8021Q */
#endif /* 0 */

#if 0
    info("athrs16_reg_init complete.\n");
#if defined (DEBUG)
	//athrs16_mdio_dump(dev);
#endif /*DEBUG*/
    athr16_init_flag = 1;
    //phy_mode_setup(dev); 
#endif /* 0 */
}

/******************************************************************************
*
* athrs16_phy_is_link_alive - test to see if the specified link is alive
*
* RETURNS:
*    TRUE  --> link is alive
*    FALSE --> link is down
*/
int athrs16_phy_is_link_alive(struct eth_device *dev, int phyUnit)
{
    uint16_t phyHwStatus;
//    uint32_t phyBase;
    uint32_t phyAddr;

    //phyBase = ATHR_PHYBASE(phyUnit);
    phyAddr = ATHR_PHYADDR(phyUnit);

    athrs16_phy_read(dev, phyAddr, ATHR_PHY_SPEC_STATUS, &phyHwStatus);

    if (phyHwStatus & ATHR_STATUS_LINK_PASS)
        return TRUE;

    return FALSE;
}

/******************************************************************************
*
* athrs16_phy_setup - reset and setup the PHY associated with
* the specified MAC unit number.
*
* Resets the associated PHY port.
*
* RETURNS:
*    TRUE  --> associated PHY is alive
*    FALSE --> no LINKs on this ethernet unit
*/

BOOL
athrs16_phy_setup(struct eth_device *dev, int ethUnit)
{
    int       phyUnit;
    uint16_t  phyHwStatus;
    uint16_t  timeout;
    int       liveLinks = 0;
    //uint32_t  phyBase = 0;
    BOOL      foundPhy = FALSE;
    uint32_t  phyAddr = 0;
    

    /* See if there's any configuration data for this enet */
    /* start auto negogiation on each phy */
    for (phyUnit=0; phyUnit < ATHR_PHY_MAX; phyUnit++) {
        if (!ATHR_IS_ETHUNIT(phyUnit, ethUnit)) {
            continue;
        }

        foundPhy = TRUE;
        //phyBase = ATHR_PHYBASE(phyUnit);
        phyAddr = ATHR_PHYADDR(phyUnit);
        
        athrs16_phy_write(dev, phyAddr, ATHR_AUTONEG_ADVERT,
                      ATHR_ADVERTISE_ALL);

        athrs16_phy_write(dev, phyAddr, ATHR_1000BASET_CONTROL,
                      ATHR_ADVERTISE_1000FULL);

        /* Reset PHYs*/
        athrs16_phy_write(dev, phyAddr, ATHR_PHY_CONTROL,
                      ATHR_CTRL_AUTONEGOTIATION_ENABLE 
                      | ATHR_CTRL_SOFTWARE_RESET);

    }

    if (!foundPhy) {
        return FALSE; /* No PHY's configured for this ethUnit */
    }

    /*
     * After the phy is reset, it takes a little while before
     * it can respond properly.
     */
    mdelay(1000);


    /*
     * Wait up to 3 seconds for ALL associated PHYs to finish
     * autonegotiation.  The only way we get out of here sooner is
     * if ALL PHYs are connected AND finish autonegotiation.
     */
    for (phyUnit=0; (phyUnit < ATHR_PHY_MAX) /*&& (timeout > 0) */; phyUnit++) {
        if (!ATHR_IS_ETHUNIT(phyUnit, ethUnit)) {
            continue;
        }

        timeout=20;
        for (;;) {
            athrs16_phy_read(dev, phyAddr, ATHR_PHY_CONTROL, &phyHwStatus);

            if (ATHR_RESET_DONE(phyHwStatus)) {
                dev_dbg("Port %d, Neg Success\n", phyUnit);
                break;
            }
            if (timeout == 0) {
                dev_dbg("Port %d, Negogiation timeout\n", phyUnit);
                break;
            }
            if (--timeout == 0) {
                dev_dbg("Port %d, Negogiation timeout\n", phyUnit);
                break;
            }

            mdelay(150);
        }
    }

    /*
     * All PHYs have had adequate time to autonegotiate.
     * Now initialize software status.
     *
     * It's possible that some ports may take a bit longer
     * to autonegotiate; but we can't wait forever.  They'll
     * get noticed by mv_phyCheckStatusChange during regular
     * polling activities.
     */
    for (phyUnit=0; phyUnit < ATHR_PHY_MAX; phyUnit++) {
        if (!ATHR_IS_ETHUNIT(phyUnit, ethUnit)) {
            continue;
        }

        if (athrs16_phy_is_link_alive(dev, phyUnit)) {
            liveLinks++;
            ATHR_IS_PHY_ALIVE(phyUnit) = TRUE;
        } else {
            ATHR_IS_PHY_ALIVE(phyUnit) = FALSE;
        }
#ifdef DEBUG
		u16 status=0;
		athrs16_phy_read(dev, ATHR_PHYADDR(phyUnit), ATHR_PHY_SPEC_STATUS, &status);
        dev_dbg("eth%d: Phy Specific Status=%4.4x\n", ethUnit, status);
#endif /*DEBUG*/
    }
    return (liveLinks > 0);
}

/******************************************************************************
*
* athrs16_phy_is_fdx - Determines whether the phy ports associated with the
* specified device are FULL or HALF duplex.
*
* RETURNS:
*    1 --> FULL
*    0 --> HALF
*/
int athrs16_phy_is_fdx(struct eth_device *dev, int ethUnit)
{
    int       phyUnit;
//    uint32_t  phyBase;
    uint32_t  phyAddr;
    uint16_t  phyHwStatus;
    int       ii = 200;

    if (ethUnit == ENET_UNIT_LAN)
        return TRUE;

    for (phyUnit=0; phyUnit < ATHR_PHY_MAX; phyUnit++) {
        if (!ATHR_IS_ETHUNIT(phyUnit, ethUnit)) {
            continue;
        }

        if (athrs16_phy_is_link_alive(dev, phyUnit)) {

            /*phyBase = ATHR_PHYBASE(phyUnit);*/
            phyAddr = ATHR_PHYADDR(phyUnit);

            do {
                athrs16_phy_read(dev, phyAddr, ATHR_PHY_SPEC_STATUS,&phyHwStatus);
				if(phyHwStatus & ATHR_STATUS_RESOVLED)
					break;
				mdelay(10);
            } while(--ii);
            
            if (phyHwStatus & ATHER_STATUS_FULL_DEPLEX)
                return TRUE;
        }
    }

    return FALSE;
}

uint32_t athrs16_reg_read(struct eth_device *dev, uint32_t reg_addr)
{
#ifndef HEADER_REG_CONF 
    return athrs16_mdio_read(dev, reg_addr);
#else
    uint8_t reg_data[4] = {0};
    athrs16_header_read_reg(reg_addr, 4, reg_data);
    return *((uint32_t *)reg_data);
#endif    
}

void
athrs16_reg_write(struct eth_device *dev, uint32_t reg_addr, uint32_t reg_val)
{
#ifndef HEADER_REG_CONF
    athrs16_mdio_write(dev, reg_addr, reg_val);
#else
    //athrs16_header_write_reg (reg_addr, 4, (uint8_t *)&reg_val);
#endif
#if defined(CONFIG_ATHRS16_VERIFY)
    uint32_t val32;
    val32 = athrs16_mdio_read(dev, reg_addr);
    if(val32 != reg_val) {
    	printf("%s: verify fail 0x%x expect 0x%x \n", __FUNCTION__, val32, reg_val);
    }
#endif
}

int
athr_ioctl(uint32_t *args, int cmd)
{
    info("EOPNOTSUPP\n");
    return -EOPNOTSUPP;

}


extern struct apm_data_priv gpriv;
/* ---------------------------------------------------------------- */
int do_ar (cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	struct apm_data_priv *priv = &gpriv;

	u32 phy_addr;
	u16 data_val;
	uint32_t addr;
	uint32_t data;
	int		rcode = 0;

	if (argc < 2) {
		cmd_usage(cmdtp);
		return 1;
	}

	/*
	 * check info/read/write.
	 */
	if (argv[1][0] == 'r') {
		if(argc < 2) {
			cmd_usage(cmdtp);
			return 1;
		}
		addr = (uint32_t)simple_strtoul( argv[2], NULL, 16);
		data = athrs16_reg_read (device, addr);
		printf("Read %08X:%08X\n", addr, data);
	} else if (argv[1][0] == 'w') {
		if(argc < 3) {
			cmd_usage(cmdtp);
			return 1;
		}
		addr = (uint32_t)simple_strtoul( argv[2], NULL, 16);
		data = (uint32_t)simple_strtoul( argv[3], NULL, 16);
		athrs16_reg_write (device, addr, data);
		printf("Write %08X:%08X\n", addr, data);
	} else if (argv[1][0] == 'v') {
		if(argc < 3) {
			cmd_usage(cmdtp);
			return 1;
		}
		addr = (uint32_t)simple_strtoul( argv[2], NULL, 16);
		data = (uint32_t)simple_strtoul( argv[3], NULL, 16);
		athrs16_reg_write (device, addr, data);
		uint32_t data_v = athrs16_reg_read (device, addr);
		if(data != data_v) {
			printf("Write verify fail %08X:%08X expect %08X\n", addr, data_v, data);
		} else {
			printf("Write verify pass %08X:%08X expect %08X\n", addr, data_v, data);
		}
	} else if (argv[1][0] == 'c') {
		if(argc < 2) {
			cmd_usage(cmdtp);
			return 1;
		}
		data = (uint32_t)simple_strtoul( argv[2], NULL, 16);
		/* Adjust MDC clock frequency */
		uint32_t value;
		apm_enet_rd32(priv, BLOCK_ETH_MAC, MII_MGMT_CONFIG_ADDR, &value);
		value &= ~MGMT_CLOCK_SEL_MASK;
		value |= MGMT_CLOCK_SEL_WR(data);
		apm_enet_wr32(priv, BLOCK_ETH_MAC, MII_MGMT_CONFIG_ADDR, value);

		apm_enet_rd32(priv, BLOCK_ETH_MAC, MII_MGMT_CONFIG_ADDR, &value);
		printf("MDC clock div 0x%x \n", value);
	} else if (argv[1][0] == 'i') {
		if(argc < 2) {
			cmd_usage(cmdtp);
			return 1;
		}
		phy_addr = (uint32_t)simple_strtoul( argv[2], NULL, 16);
		addr = (uint32_t)simple_strtoul( argv[3], NULL, 16);
		
		athrs16_phy_read(device, phy_addr, addr, &data_val);

		printf("Read %08X.%08X = 0x%04x\n", phy_addr, addr, data_val);
	} else if (argv[1][0] == 'o') {
		if(argc < 2) {
			cmd_usage(cmdtp);
			return 1;
		}
		phy_addr = (uint32_t)simple_strtoul( argv[2], NULL, 16);
		addr = (uint32_t)simple_strtoul( argv[3], NULL, 16);
		data_val = (u16)simple_strtoul( argv[4], NULL, 16);
		athrs16_phy_write(device, phy_addr, addr, data_val);

		printf("Write %08X.%08X 0x%04x\n", phy_addr, addr, data_val);
	} else {
		cmd_usage(cmdtp);
		return 1;
	}

	return rcode;
}

/***************************************************/
U_BOOT_CMD(
	ar,	5,	0,	do_ar,
	"SMI utility commands",
	"device                     - list available devices\n"
	"ar device <devname>           - set current device\n"
	"ar info   <addr>              - display SMI PHY info\n"
	"ar read   <reg>			   - read  SMI PHY <addr> register <reg>\n"
	"ar write  <reg> <data>		   - write SMI PHY <addr> register <reg>\n"
	"ar dump   <addr> <reg>        - pretty-print <addr> <reg> (0-5 only)\n"
	"ar c	   <data>			   - divide MDC\n"
	"ar in     <phy_addr> <reg>        - mii read <reg>\n"
	"ar out    <phy_addr> <reg> <data> - mii write <reg>\n"
	"Addr and/or reg may be ranges, e.g. 2-7."
);
