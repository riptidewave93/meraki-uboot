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
/* debug/trace */
//#define BCMDBG
#define BCMDBG_ERR
#ifdef BCMDBG
#define	ET_ERROR(args) printf args
#define	ET_TRACE(args) printf args
#define BCMIPROC_ETH_DEBUG
#elif defined(BCMDBG_ERR)
#define	ET_ERROR(args) printf args
#define ET_TRACE(args)
#undef BCMIPROC_ETH_DEBUG
#else
#define	ET_ERROR(args)
#define	ET_TRACE(args)
#undef BCMIPROC_ETH_DEBUG
#endif /* BCMDBG */

#include <common.h>
#include <malloc.h>
#include <net.h>
#include <config.h>
#include <asm/arch/ethHw.h>
#include <asm/arch/ethHw_dma.h>

#define BCMIPROC_ETH_DEV_NAME          "bcmiproc_eth"

#define BCM_NET_MODULE_DESCRIPTION    "Broadcom BCM IPROC Ethernet driver"
#define BCM_NET_MODULE_VERSION        "0.1"

static const char banner[] = BCM_NET_MODULE_DESCRIPTION " " BCM_NET_MODULE_VERSION "\n";
extern int ethHw_miiphy_read(unsigned int const phyaddr, 
		   unsigned int const reg, unsigned short *const value);
extern int ethHw_miiphy_write(unsigned int const phyaddr, 
		   unsigned int const reg, unsigned short *const value);


/******************************************************************
 * u-boot net functions
 */
static int
bcmiproc_eth_send(struct eth_device *dev, volatile void *packet, int length)
{
	uint8_t *buf = (uint8_t *) packet;
	int rc;

	ET_TRACE(("%s enter\n", __FUNCTION__));

	/* load buf and start transmit */
	rc = ethHw_dmaTx( length, buf );
	if (rc) {
		ET_ERROR(("ERROR - Tx failed\n"));
		return rc;
	}

	rc = ethHw_dmaTxWait();
	if (rc) {
		ET_ERROR(("ERROR - no Tx notice\n"));
		return rc;
	}

	ET_TRACE(("%s exit rc(0x%x)\n", __FUNCTION__, rc));
	return rc;
}

static int
bcmiproc_eth_rcv(struct eth_device *dev)
{
	int rc;

	ET_TRACE(("%s enter\n", __FUNCTION__));

	rc = ethHw_dmaRx();

	ET_TRACE(("%s exit rc(0x%x)\n", __FUNCTION__, rc));
	return rc;
}


static int
bcmiproc_eth_write_hwaddr(struct eth_device* dev)
{
	int rc=0;

	ET_TRACE(("%s enter\n", __FUNCTION__));

	ET_TRACE(("%s Not going to change MAC address\n", __FUNCTION__));

	ET_TRACE(("%s exit rc(0x%x)\n", __FUNCTION__, rc));
	return rc;
}


static int
bcmiproc_eth_open( struct eth_device *dev, bd_t * bt )
{
	int rc=0;
	ET_TRACE(("%s enter\n", __FUNCTION__));

	/* Enable forwarding to internal port */
	ethHw_macEnableSet(ETHHW_PORT_INT, 1);

	/* enable tx and rx DMA */
	ethHw_dmaEnable(DMA_RX);
	ethHw_dmaEnable(DMA_TX);

#if (defined(CONFIG_HELIX4) || defined(CONFIG_HURRICANE2) || defined(CONFIG_KATANA2))
	/* Check which port is connected and take PORT0 with priority */
	if ( !ethHw_portLinkUp() ) {
		error("Ethernet external port not connected");
		return -1;
	}
	ethHw_checkPortSpeed();
#endif /* (defined(CONFIG_HELIX4) || defined(CONFIG_HURRICANE2) || defined(CONFIG_KATANA2)) */

	ET_TRACE(("%s exit rc(0x%x)\n", __FUNCTION__, rc));
	return rc;
}


static void
bcmiproc_eth_close(struct eth_device *dev)
{
	ET_TRACE(("%s enter\n", __FUNCTION__));

	/* disable DMA */
	ethHw_dmaDisable(DMA_RX);
	ethHw_dmaDisable(DMA_TX);

	/* Disable forwarding to internal port */
	ethHw_macEnableSet(ETHHW_PORT_INT, 0);

	ET_TRACE(("%s exit\n", __FUNCTION__));
}


int
bcmiproc_miiphy_read(char *devname, unsigned int const addr, 
   unsigned int const reg, unsigned short *const value)
{
	//printf("%s devname(%s) addr(0x%x) reg(0x%x)\n", __FUNCTION__, devname, addr, reg);
	return ethHw_miiphy_read(addr, reg, value);
}


int
bcmiproc_miiphy_write(char *devname, unsigned int const addr, 
   unsigned int const reg, unsigned short *const value)
{
	//printf("%s devname(%s) addr(0x%x) reg(0x%x)\n", __FUNCTION__, devname, addr, reg);
	return ethHw_miiphy_write(addr, reg, value);
}


int
bcmiproc_eth_register(u8 dev_num)
{
	struct eth_device *dev;
	int rc;

	ET_TRACE(("%s enter\n", __FUNCTION__));

	dev = (struct eth_device *) malloc(sizeof(struct eth_device));
	if (dev == NULL) {
		return -1;
	}
	memset(dev, 0, sizeof(*dev));
	sprintf(dev->name, "%s-%hu", BCMIPROC_ETH_DEV_NAME, dev_num);

	printf(banner);

	/* Initialization */
	ET_TRACE(("Ethernet initialization...\n"));
	rc = ethHw_Init();
	ET_TRACE(("Ethernet initialization %s (rc=%i)\n",
			(rc>=0) ? "successful" : "failed", rc));

	dev->iobase = 0;

	dev->init = bcmiproc_eth_open;
	dev->halt = bcmiproc_eth_close;
	dev->send = bcmiproc_eth_send;
	dev->recv = bcmiproc_eth_rcv;
	dev->write_hwaddr = bcmiproc_eth_write_hwaddr;

	eth_register(dev);

	ET_TRACE(("Ethernet Driver registered...\n"));

#ifdef CONFIG_CMD_MII
   miiphy_register(dev->name, bcmiproc_miiphy_read, bcmiproc_miiphy_write);
#endif

	ET_TRACE(("%s exit\n", __FUNCTION__));
	return 1;
}
