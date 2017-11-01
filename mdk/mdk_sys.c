
#include <common.h>
#include "../include/linux/string.h"
#include "../include/exports.h"
#include "../include/linux/time.h"
#include "../include/net.h"

/* 
 * Avoid double definitions when mixing CDK 
 * and system header files.
 */
#define CDK_CONFIG_DEFINE_UINT8_T       0
#define CDK_CONFIG_DEFINE_UINT16_T      0
#define CDK_CONFIG_DEFINE_UINT32_T      0
#define CDK_CONFIG_DEFINE_PRIu32        0
#define CDK_CONFIG_DEFINE_PRIx32        0
#define CDK_CONFIG_DEFINE_SIZE_T        0
#define CDK_CONFIG_DEFINE_DMA_ADDR_T    0

#include "mdk_sys.h"
#include "soc_srab_regfile.h"

#include <cdk/cdk_device.h>
#include <cdk/cdk_assert.h>
#include "cdk/cdk_shell.h"

#ifdef CDK_CONFIG_ARCH_ROBO_INSTALLED
/* Robo probing support */
#include <cdk/arch/robo_chip.h>
#endif

#include <bmd/bmd.h>
#include <bmd/bmd_device.h>
#include <bmd/bmd_phy_ctrl.h>

#define SWAP16(x) ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8))

/*
 * Actual devices in our system
 */
typedef struct mdk_sys_dev_s {
    int unit;
    uint32_t regbase;
} mdk_sys_dev_t;

static mdk_sys_dev_t _sys_devs[CDK_CONFIG_MAX_UNITS];

/*
 * MDK network driver
 */

/*  *********************************************************************
    *  switch register access
    ********************************************************************* */

#define readl(reg)              (*(volatile uint32_t *)(reg))
#define writel(reg, val)        (*(volatile uint32_t *)(reg) = (uint32_t)(val))

static
void switch_interface_reset(void)
{
    uint32_t timeout;

    /* Wait for switch initialization complete */
    timeout = 10000;
    while (!(readl(CFG_R_SW_IF_MEMADDR) & CFG_F_sw_init_done_MASK)) {
        /*YIELD;*/
        if (!timeout--) {
            printf("switch_interface_reset: timeout sw_init_done\n");
            break;
        }
    }

    /* Set the SRAU reset bit */
    writel(CFG_R_SRAB_CMDSTAT_MEMADDR, CFG_F_sra_rst_MASK);
    /* Wait for it to auto-clear */
    timeout = 10000;
    while (readl(CFG_R_SRAB_CMDSTAT_MEMADDR) & CFG_F_sra_rst_MASK) {
        /*YIELD;*/
        if (!timeout--) {
            printf("switch_interface_reset: timeout sra_rst\n");
            return;
        }
    }
}

static
void switch_request_grant(void)
{
    uint32_t regval;
    uint32_t timeout = 10000;

    regval = readl(CFG_R_SW_IF_MEMADDR);
    regval |= CFG_F_rcareq_MASK;
    writel(CFG_R_SW_IF_MEMADDR, regval);
    while (!(readl(CFG_R_SW_IF_MEMADDR) & CFG_F_rcagnt_MASK)) {
        /* YIELD; */
        if (!timeout--) {
            printf("switch_request_grant: timeout\n");
            return;
        }
    }
}

static
void switch_release_grant(void)
{
    uint32_t regval;

    regval = readl(CFG_R_SW_IF_MEMADDR);
    regval &= ~CFG_F_rcareq_MASK;
    writel(CFG_R_SW_IF_MEMADDR, regval);
}

static
uint64_t switch_reg_read(uint8_t page, uint8_t offset)
{
    uint64_t value = ~(uint64_t)0;
    uint32_t regval;
    uint32_t timeout = 1000;

    /* Assemble read command */
    switch_request_grant();
    regval = ((page << CFG_F_sra_page_R)
              | (offset << CFG_F_sra_offset_R)
              | CFG_F_sra_gordyn_MASK);
    writel(CFG_R_SRAB_CMDSTAT_MEMADDR, regval);

    /* Wait for command complete */
    while (readl(CFG_R_SRAB_CMDSTAT_MEMADDR) & CFG_F_sra_gordyn_MASK) {
        /*YIELD;*/
        if (!--timeout) {
            printf("switch_read: timeout\n");
            switch_interface_reset();
            break;
        }
    }
    if (timeout) {
        /* Didn't time out, read and return the value */
        value = (((uint64_t)readl(CFG_R_SRAB_RDH_MEMADDR)) << 32)
                        | readl(CFG_R_SRAB_RDL_MEMADDR);
    }

    switch_release_grant();
    return value;
}
	
static
void switch_reg_write(uint8_t page, uint8_t offset, uint64_t value)
{
    uint32_t regval;
    uint32_t timeout = 1000;

    switch_request_grant();
    /* Load the value to write */
    writel(CFG_R_SRAB_WDH_MEMADDR, (uint32_t)(value >> 32));
    writel(CFG_R_SRAB_WDL_MEMADDR, (uint32_t)(value));
    /* Issue the write command */
    regval = ((page << CFG_F_sra_page_R)
              | (offset << CFG_F_sra_offset_R)
              | CFG_F_sra_gordyn_MASK
              | CFG_F_sra_write_MASK);
    writel(CFG_R_SRAB_CMDSTAT_MEMADDR, regval);
    /* Wait for command complete */
    while (readl(CFG_R_SRAB_CMDSTAT_MEMADDR) & CFG_F_sra_gordyn_MASK) {
        /*YIELD;*/
        if (!--timeout) {
            printf("switch_write: timeout\n");
            switch_interface_reset();
            break;
        }
    }
    switch_release_grant();
}

static uint8_t rcv_data[2048];
static uint32_t rcv_len;

void mdk_rcv_handler(uint8_t *inpkt, uint32_t len)
{
    /* report the received length */
	rcv_len = len;
}

/* Only support the feature if CDK_DEV_ADDR_EXT_PHY_BUS_MDIO is defined in CDK */
#if defined(CDK_DEV_ADDR_EXT_PHY_BUS_MDIO)

/* ChipcommonB MDIO register set */
/* MII register definition */
#define  MII_MGMT                                                     0x18003000
#define  MII_MGMT_BASE                                                     0x000
#define  MII_MGMT_DATAMASK                                            0x000007ff
#define  MII_CMD_DATA                                                 0x18003004
#define  MII_CMD_DATA_BASE                                                 0x004
#define  MII_CMD_DATA_DATAMASK                                        0xffffffff
/* fields in MII_MGMT */
#define MII_MGMT_BYP_MASK		0x00000400
#define MII_MGMT_BYP_SHIFT	    10
#define MII_MGMT_EXP_MASK		0x00000200
#define MII_MGMT_EXP_SHIFT	    9
#define MII_MGMT_BSY_MASK		0x00000100
#define MII_MGMT_BSY_SHIFT	    8
#define MII_MGMT_PRE_MASK		0x00000080
#define MII_MGMT_PRE_SHIFT	    7
#define MII_MGMT_MDCDIV_MASK	0x0000007f
#define MII_MGMT_MDCDIV_SHIFT	0
/* fields in MII_CMD_DATA */
#define MII_CMD_DATA_SB_MASK		0xc0000000
#define MII_CMD_DATA_SB_SHIFT	    30
#define MII_CMD_DATA_OP_MASK		0x30000000
#define MII_CMD_DATA_OP_SHIFT	    28
#define MII_CMD_DATA_PA_MASK		0x0f800000
#define MII_CMD_DATA_PA_SHIFT	    23
#define MII_CMD_DATA_RA_MASK		0x007c0000
#define MII_CMD_DATA_RA_SHIFT	    18
#define MII_CMD_DATA_TA_MASK		0x00030000
#define MII_CMD_DATA_TA_SHIFT	    16
#define MII_CMD_DATA_DATA_MASK		0x0000ffff
#define MII_CMD_DATA_DATA_SHIFT	    0

#define MII_TRIES 100000
#define MII_POLL_USEC                     20

static int
ccb_mii_read(int phy_addr, int reg_off)
{
    int i;
    uint32_t ctrl = 0;

    ctrl = (0xcd | MII_MGMT_EXP_MASK);
    writel(MII_MGMT, ctrl);

    for (i = 0; i < MII_TRIES; i++) {
        ctrl = readl(MII_MGMT);
        if (!(ctrl & MII_MGMT_BSY_MASK)) {
	    break;
	}
	udelay(MII_POLL_USEC);
    }
    if (i >= MII_TRIES) {
	printf("MDIO ERROR: %s: BUSY stuck: ctrl=0x%x, count=%d\n", __FUNCTION__, ctrl, i);
	return CDK_E_IO;
    }

    ctrl = ((1 << MII_CMD_DATA_SB_SHIFT) & MII_CMD_DATA_SB_MASK) |
	    ((2 << MII_CMD_DATA_OP_SHIFT) & MII_CMD_DATA_OP_MASK) |
	    ((phy_addr << MII_CMD_DATA_PA_SHIFT) & MII_CMD_DATA_PA_MASK) |
	    ((reg_off << MII_CMD_DATA_RA_SHIFT) & MII_CMD_DATA_RA_MASK) |
	    ((2 << MII_CMD_DATA_TA_SHIFT) & MII_CMD_DATA_TA_MASK);
    writel(MII_CMD_DATA, ctrl);


    for (i = 0; i < MII_TRIES; i++) {
        ctrl = readl(MII_MGMT);
	if (!(ctrl & MII_MGMT_BSY_MASK)) {
	    break;
	}
	udelay(MII_POLL_USEC);
    }
    if (i >= MII_TRIES) {
	printf("MDIO ERROR: \n%s: BUSY stuck: ctrl=0x%x, count=%d\n", __FUNCTION__, ctrl, i);
	return CDK_E_IO;
    }

    ctrl = readl(MII_CMD_DATA);

    return (ctrl & 0xffff);
}

static int
ccb_mii_write(int phy_addr, int reg_off, uint16_t data)
{
    int i;
    uint32_t ctrl = 0;

    ctrl = (0xcd | MII_MGMT_EXP_MASK);
    writel(MII_MGMT, ctrl);

    for (i = 0; i < MII_TRIES; i++) {
        ctrl = readl(MII_MGMT);
	if (!(ctrl & MII_MGMT_BSY_MASK)) {
	    break;
	}
	udelay(MII_POLL_USEC);
    }
    if (i >= MII_TRIES) {
	printf("\n%s: BUSY stuck: ctrl=0x%x, count=%d\n", __FUNCTION__, ctrl, i);
	return CDK_E_IO;
    }

    ctrl = ((1 << MII_CMD_DATA_SB_SHIFT) & MII_CMD_DATA_SB_MASK) |
	    ((1 << MII_CMD_DATA_OP_SHIFT) & MII_CMD_DATA_OP_MASK) |
	    ((phy_addr << MII_CMD_DATA_PA_SHIFT) & MII_CMD_DATA_PA_MASK) |
	    ((reg_off << MII_CMD_DATA_RA_SHIFT) & MII_CMD_DATA_RA_MASK) |
	    ((2 << MII_CMD_DATA_TA_SHIFT) & MII_CMD_DATA_TA_MASK) |
	    ((data << MII_CMD_DATA_DATA_SHIFT) & MII_CMD_DATA_DATA_MASK);
    writel(MII_CMD_DATA, ctrl);


    for (i = 0; i < MII_TRIES; i++) {
        ctrl = readl(MII_MGMT);
	if (!(ctrl & MII_MGMT_BSY_MASK)) {
	    break;
	}
	udelay(MII_POLL_USEC);
    }
    if (i >= MII_TRIES) {
	printf("\n%s: BUSY stuck: ctrl=0x%x, count=%d\n", __FUNCTION__, ctrl, i);
	return CDK_E_IO;
    }

    return CDK_E_NONE;
}
#endif

static int 
_read(void *dvc, uint32_t addr, uint8_t *data, uint32_t len)
{
    /*mdk_sys_dev_t *dev = (mdk_sys_dev_t *)dvc;*/
    int rv = CDK_E_NONE;
    uint8_t page;
    uint8_t offset;
    uint64_t regval;
    int regval32;
    uint8_t  *data8_ptr;

    if (addr == CDK_DEV_ADDR_ETH) {
        memset(rcv_data, 0, 1536);
        rcv_len= 0;
        NetRxPackets[0] = &rcv_data;

        eth_rx();

        memcpy(data, rcv_data, rcv_len);
        rv = rcv_len;
		if (!data) {
		    rv = CDK_E_IO;
		} else if (rv == 0) {
		    rv = CDK_E_TIMEOUT;
		}

		return rv;
    }

#if defined(CDK_DEV_ADDR_EXT_PHY_BUS_MDIO)
    if ((addr & 0xffff0000) == CDK_DEV_ADDR_EXT_PHY_BUS_MDIO) {
        regval32 = ccb_mii_read((addr & 0x0000ffff) >> 8, (addr & 0x000000ff));
        if (regval32 == CDK_E_IO) {
            return CDK_E_IO;
        }

        memcpy(data, &regval32, 4);

        return CDK_E_NONE;
    }
#endif

    page = (uint8_t)((addr >> 8) & 0xff);
    offset = (uint8_t)(addr & 0xff);
    regval = switch_reg_read(page, offset);

    data8_ptr = (uint8_t *)&regval;
    if (len > 4) {
        memcpy(data, data8_ptr, 8);
    } else {
        memcpy(data, data8_ptr, 4);
    }

    return rv;
}

static int
_write(void *dvc, uint32_t addr, const uint8_t *data, uint32_t len)
{
    /*mdk_sys_dev_t *dev = (mdk_sys_dev_t *)dvc;*/
    int rv = CDK_E_NONE;
    uint8_t page;
    uint8_t offset;
    uint64_t regval;
    int regval32;
    uint8_t  *data8_ptr;

    if (addr == CDK_DEV_ADDR_ETH) {
        rv = eth_send(data,len);

        return (rv < 0) ? CDK_E_IO : CDK_E_NONE;
    }

#if defined(CDK_DEV_ADDR_EXT_PHY_BUS_MDIO)
    if ((addr & 0xffff0000) == CDK_DEV_ADDR_EXT_PHY_BUS_MDIO) {
        memcpy(&regval32, data, 4);
        
        rv = ccb_mii_write((addr & 0x0000ffff) >> 8, (addr & 0x000000ff), (uint16_t)regval32);

        return rv;
    }
#endif

    page = (uint8_t)((addr >> 8) & 0xff);
    offset = (uint8_t)(addr & 0xff);

    regval = 0;
    data8_ptr = (uint8_t *)&regval;
    if (len > 4) {
        memcpy(data8_ptr, data, 8);
    } else {
        memcpy(data8_ptr, data, 4);
    }

    switch_reg_write(page, offset, regval);

    return CDK_E_NONE;
}


/*  *********************************************************************
    *  Probe functions for MDK devices
    ********************************************************************* */

#if !defined(SYS_BE_PIO) || !defined(SYS_BE_PACKET) || !defined(SYS_BE_OTHER)
#error PCI bus endian flags SYS_BE_PIO, SYS_BE_PACKET and SYS_BE_OTHER not defined
#endif
static uint32_t _bus_flags(void)
{
    uint32_t flags = CDK_DEV_MBUS_PCI;

#if SYS_BE_PIO == 1
    flags |= CDK_DEV_BE_PIO;
#endif
#if SYS_BE_PACKET == 1
    flags |= CDK_DEV_BE_PACKET;
#endif
#if SYS_BE_OTHER == 1
    flags |= CDK_DEV_BE_OTHER;
#endif
    return flags;
}

static int ndevs = 0;

static int
dev_create(uint32_t vendor_id, uint32_t dev_id, uint32_t rev, uint32_t model,
           uint32_t regbase)
{
    cdk_dev_id_t id;
    cdk_dev_vectors_t dv;
    char model_str[16];

    /* Clear model ID string */
    model_str[0] = 0;

    /* Set up ID structure */
    memset(&id, 0, sizeof(id));
    id.vendor_id = vendor_id;
    id.device_id = dev_id;
    id.revision = rev;
#ifdef CDK_DEV_HAS_CHIP_MODEL
    id.model = model;
    sprintf(model_str, "%04x:", model);
#endif

    /* Set up device vectors */
    memset(&dv, 0, sizeof(dv));
    dv.dvc = &_sys_devs[ndevs];
    dv.read = _read;
    dv.write = _write;

    /* Callback context is unit number */
    _sys_devs[ndevs].unit = ndevs;

    if (cdk_dev_create(&id, &dv, _bus_flags()) == CDK_E_NONE) {
        printf("Found MDK device: %04x:%04x:%s%02x\n", 
               id.vendor_id, id.device_id, model_str, id.revision);
        _sys_devs[ndevs].regbase = regbase;
        ndevs++;
    }

    return 0;
}

int
mdk_sys_probe(void)
{
    int unit;

    if (ndevs == 0) {
        /*mdk_sys_dev_t *dev = &_sys_devs[ndevs];*/
        /*int buflen = 1;*/

#ifdef CDK_DEV_HAS_CHIP_MODEL
        mdk_sys_dev_t *dev = &_sys_devs[ndevs];
        cdk_dev_id_t id;
#else
        uint16_t phyidl = 0, phyidh = 0;
        uint8_t rev = 0;
#endif


       /* Start ethernet interface */
       eth_halt();
       if (eth_init(NULL) < 0) {
           eth_halt();
		   printf("Ethernet interface open failed\n");
           return(-1);
       }

#ifdef CDK_DEV_HAS_CHIP_MODEL
        /* Perform extended probing */
        cdk_robo_probe(dev, &id, _read);

        dev_create(id.vendor_id, id.device_id, id.revision, id.model, 0);
#else
        /* Get ROBO device ID */
        _read(NULL, 0x1004, (uint8_t *)&phyidh, 2);
        _read(NULL, 0x1006, (uint8_t *)&phyidl, 2);

#if ENDIAN_BIG
        phyidh =  SWAP16(phyidh);
        phyidl =  SWAP16(phyidl);
#endif
        rev = phyidl & 0x000f;
        phyidl &= 0xfff0;

        dev_create(phyidh, phyidl, rev, 0, 0);
#endif

        if (ndevs == 0) {
            printf("Warning: no device probed.\n");
        }
    }

    for (unit = 0; unit < ndevs; unit++) {
        /* Attach devices after setting configuration */
        bmd_attach(unit);

#if BMD_CONFIG_INCLUDE_PHY == 1
        /* Install default PHY probing function */
        bmd_phy_probe_init(bmd_phy_probe_default, bmd_phy_drv_list);
#endif
    }

#if CFG_MDK_DEBUG == 1
    /* Set default MDK unit */
    cdk_shell_unit_set(0);
#endif

    return ndevs;
}

int
mdk_sys_netdev_init(void)
{
    /* CHECKME: not applicable */    
    return 0;
}

int
mdk_sys_init(void)
{
#if CFG_MDK_DEBUG == 0
    mdk_sys_netdev_init();
#endif

    return 0;
}

/*  *********************************************************************
    *  System functions required by BMD
    ********************************************************************* */
int
mdk_sys_udelay(uint32_t usec)
{
    udelay(usec);
    return 0;
}

void *
mdk_sys_dma_alloc_coherent(void *dvc, size_t size, dma_addr_t *baddr)
{
    void *laddr;
    laddr =  malloc(size);
    return laddr;
}

void
mdk_sys_dma_free_coherent(void *dvc, size_t size, void *laddr, dma_addr_t baddr)
{
    free(laddr);
}

/*  *********************************************************************
    *  CDK support functions
    ********************************************************************* */

/*
 * Abort function to properly hand CDK asserts
 */
void
abort(void)
{
    /* CHECKME */
}

