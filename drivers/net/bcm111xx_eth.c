/*****************************************************************************
* Copyright 2006 - 2011 Broadcom Corporation.  All rights reserved.
*
* Unless you and Broadcom execute a separate written software license
* agreement governing use of this software, this software is licensed to you
* under the terms of the GNU General Public License version 2, available at
* http://www.broadcom.com/licenses/GPLv2.php (the "GPL").
*
* Notwithstanding the above, under no circumstances may you combine this
* software in any way with any other Broadcom software provided under a
* license other than the GPL, without Broadcom's express prior written
* consent.
*****************************************************************************/
//#define BCM111XX_ETH_DEBUG

#ifdef BCM111XX_ETH_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif

#include <common.h>
#include <malloc.h>
#include <net.h>
#include <config.h>
//#include <stdint.h>

#include <asm/arch/csp/ethHw.h>
#include <asm/arch/csp/ethHw_dma.h>

#ifdef CONFIG_BCMHANA_ETH
#include <asm/arch/csp/gpiomux.h>
#include <asm/arch/csp/chipregHw_def.h>
#include <asm/arch/csp/chal_ccu_esub_inline.h>
#include <asm/arch/csp/chal_gpio_inline.h>

#define GPIO_PHY_RST0     86
#define GPIO_PHY_RST1     85

#define BCM111XX_ETH_DEV_NAME          "bcm11160_eth"
#define ETH_DMA_CONTROLLER     0     // DMA0

#else
#define BCM111XX_ETH_DEV_NAME          "bcm11107_eth"
#define ETH_DMA_CONTROLLER     1     // DMA1

#endif

#define BCM_NET_MODULE_DESCRIPTION    "Broadcom BCM111XX Ethernet driver"
#define BCM_NET_MODULE_VERSION        "0.1"

#define ETH_DMA_CH_RX         1     // PTM
#define ETH_DMA_CH_TX         0     // MTP

#define ETH_DMA_BURST_SIZE    8
#define ETH_DMA_BLOCK_SIZE    256

/* Memory for RX buffers and RX DMA descriptors. */
#define RX_BUF_SIZE        2048
#define RX_BUF_NUM         8
#define RX_DESC_NUM        RX_BUF_NUM

/* Use 2nd half of reserved uncache RAM (2MB) for Rx buffer */
#define RX_BUF_BASE        (CONFIG_PHYS_SDRAM_1 + 0x100000)
#define RX_DESC_BASE       (RX_BUF_BASE + (RX_BUF_NUM * RX_BUF_SIZE))

#define RX_BUF(i)          (uint8_t *)(RX_BUF_BASE + ((i) * RX_BUF_SIZE))
#define RX_DESC(i)         (ETHHW_DMA_DESC *)(RX_DESC_BASE + ((i) * sizeof( ETHHW_DMA_DESC )))
#define RX_FLUSH_CACHE()

/* Memory for TX buffers and TX DMA descriptors.
 * For every Tx buffer, there must be two descriptors (one for config and one
 * for data) and one config buffer (for config information)
 */
#define TX_BUF_SIZE        2048
#define TX_BUF_NUM         1
#define TX_DESC_NUM        (TX_BUF_NUM * 2)  // Require one config and one data descriptor for every data buffer
#define CFG_BUF_SIZE       8
#define CFG_BUF_NUM        TX_BUF_NUM

/* Starting from RX_BUF_BASE + 128KB for Tx buffer */
#define TX_BUF_BASE        (RX_BUF_BASE + 0x20000)
#define TX_DESC_BASE       (TX_BUF_BASE + (TX_BUF_NUM * TX_BUF_SIZE))
#define CFG_BUF_BASE       (TX_DESC_BASE + (TX_DESC_NUM * sizeof( ETHHW_DMA_DESC )))

#define TX_BUF(i)          (uint8_t *)(TX_BUF_BASE + ((i) * TX_BUF_SIZE))
#define TX_DESC(i)         (ETHHW_DMA_DESC *)(TX_DESC_BASE + ((i) * sizeof( ETHHW_DMA_DESC )))
#define CFG_BUF(i)         (uint8_t *)(CFG_BUF_BASE + ((i) * CFG_BUF_SIZE))

#define TX_FLUSH_CACHE()

static const char banner[] = BCM_NET_MODULE_DESCRIPTION " " BCM_NET_MODULE_VERSION "\n";

#ifdef BCM111XX_ETH_DEBUG
static int bcm111xx_eth_info( void );

static int bcm111xx_eth_info( void )
{
   printf( "\n" );
   printf( "Ethernet Console\n" );
   printf( "================\n" );
   printf( "\n" );
   printf( __FILE__ " built on " __DATE__ " at " __TIME__ "\n" );
   printf( "\n" );
   printf( "Buffer              Address     Num   Size\n" );
   printf( "--------------    ----------   ----   ----\n" );
   printf( "Rx Data           0x%08x   %4i   %4i\n", (int)RX_BUF( 0 ), RX_BUF_NUM, RX_BUF_SIZE );
   printf( "Rx Descriptors    0x%08x   %4i\n", (int)RX_DESC( 0 ), RX_DESC_NUM );
   printf( "Tx Data           0x%08x   %4i   %4i\n", (int)TX_BUF( 0 ), TX_BUF_NUM, TX_BUF_SIZE );
   printf( "Tx Descriptors    0x%08x   %4i\n", (int)TX_DESC( 0 ), TX_DESC_NUM  );
   printf( "Config Data       0x%08x   %4i   %4i\n", (int)CFG_BUF( 0 ), CFG_BUF_NUM, CFG_BUF_SIZE );
   printf( "\n" );
   printf( "DMA Config     Value\n" );
   printf( "----------     -----\n" );
   printf( "Controller      %4i\n", ETH_DMA_CONTROLLER );
   printf( "Rx Channel      %4i\n", ETH_DMA_CH_RX );
   printf( "Tx Channel      %4i\n", ETH_DMA_CH_TX );
   printf( "Burst Size      %4i\n", ETH_DMA_BURST_SIZE );
   printf( "Block Size      %4i\n", ETH_DMA_BLOCK_SIZE );
   printf( "\n" );

   return 0;
}

static void txDump(int index, int len)
{
   uint8_t *bufp;
   int i;

   bufp = (uint8_t *) TX_BUF(index);

   debug("Tx Buf: %d, %d\n", index, len);
   for (i = 0; i < len; i++) {
      debug("%02X ", bufp[i]);
   }
   debug("\n");
}
#endif

#ifdef CONFIG_BCMHANA_ETH
static int ethPhyStart( void )
{
   uint32_t reg32;

   /* Enable Ethernet clock */
   chal_ccu_esub_pll_init();

   chal_gpio_init();

   /* Take PORT0 PHY out of reset */
   gpiomux_request(GPIO_PHY_RST0, chipregHw_PIN_FUNCTION_ALT04, "ephy0_rst");
   chal_gpio_set_mode( GPIO_PHY_RST0, CHAL_GPIO_DIR_OUTPUT );
   chal_gpio_set_bit( GPIO_PHY_RST0, 0 );
   chal_gpio_set_bit( GPIO_PHY_RST0, 1 );

   /* Set MDIO address */
   ETHHW_REG_SET( ETHHW_REG32( MM_IO_BASE_ESW + 0x00380 ), 0 );

   /* Disable the RGMII transmit timing delay on the external PHY0 */
   ethHw_miiSet( 0, ETHHW_MII_FLAGS_EXT, 0x1c, 0x0c00 );
   ethHw_miiGet( 0, ETHHW_MII_FLAGS_EXT, 0x1c, &reg32 );
   reg32 |= 0x8000; /* write enable */
   reg32 &= ~0x0200; /* turn off bit 9 to disable RGMII Tx delay */
   ethHw_miiSet( 0, ETHHW_MII_FLAGS_EXT, 0x1c, reg32 );

   /* Enable out of band signaling on the external PHY0 */
   ethHw_miiSet( 0, ETHHW_MII_FLAGS_EXT, 0x18, 0x7007 );
   ethHw_miiGet( 0, ETHHW_MII_FLAGS_EXT, 0x18, &reg32 );
   reg32 |= 0x8000; /* write enable */
   reg32 &= ~0x0020; /* turn off bit 5 for outband signaling */
   ethHw_miiSet( 0, ETHHW_MII_FLAGS_EXT, 0x18, reg32 );

   /* Take PORT1 PHY out of reset */
   gpiomux_request(GPIO_PHY_RST1, chipregHw_PIN_FUNCTION_ALT04, "ephy1_rst");
   chal_gpio_set_mode( GPIO_PHY_RST1, CHAL_GPIO_DIR_OUTPUT );
   chal_gpio_set_bit( GPIO_PHY_RST1, 0 );
   chal_gpio_set_bit( GPIO_PHY_RST1, 1 );

   /* Set MDIO address */
   ETHHW_REG_SET( ETHHW_REG32( MM_IO_BASE_ESW + 0x00388 ), 1 );

   /* Disable the RGMII transmit timing delay on the external PHY1 */
   ethHw_miiSet( 1, ETHHW_MII_FLAGS_EXT, 0x1c, 0x0c00 );
   ethHw_miiGet( 1, ETHHW_MII_FLAGS_EXT, 0x1c, &reg32 );
   reg32 |= 0x8000; /* write enable */
   reg32 &= ~0x0200; /* turn off bit 9 to disable RGMII Tx delay */
   ethHw_miiSet( 1, ETHHW_MII_FLAGS_EXT, 0x1c, reg32 );

   /* Enable out of band signaling on the external PHY1 */
   ethHw_miiSet( 1, ETHHW_MII_FLAGS_EXT, 0x18, 0x7007 );
   ethHw_miiGet( 1, ETHHW_MII_FLAGS_EXT, 0x18, &reg32 );
   reg32 |= 0x8000; /* write enable */
   reg32 &= ~0x0020; /* turn off bit 5 for outband signaling */
   ethHw_miiSet( 1, ETHHW_MII_FLAGS_EXT, 0x18, reg32 );

   return 0;
}
#endif

static int txPacketAdd(int index, size_t len, uint8_t * tx_buf)
{
   uint8_t *bufp;
   ETHHW_DMA_DESC *descp;
   uint64_t *cfgp;

   /* Fill SA and DA */
   memcpy((uint8_t *) TX_BUF(index), tx_buf, 12);
   /* add empty brcmTag */
   memset((uint8_t *) TX_BUF(index) + 12, 0, 4);
   /* Fill other data */
   memcpy((uint8_t *) TX_BUF(index) + 16, tx_buf + 12, len - 12);

   /* Add length of BCM tag */
   len += sizeof(uint32_t);   // Account for addition of Broadcom tag
   
   /* The Ethernet packet has to be >= 64 bytes required by switch 
    * padding it with zeros
    */
   if (len < 64)
   {
      memset((uint8_t *) TX_BUF(index) + len, 0, 64 - len);
      len = 64;
   }

   /* Add 4 bytes for Ethernet FCS/CRC */
   len += 4;

   /* Fill config data 
    * (there is a one-to-one mapping of config data and buffer data) 
    */
   bufp = (uint8_t *) TX_BUF(index);
   cfgp = (uint64_t *) CFG_BUF(index);

   *cfgp = ETHHW_DMA_CFG_EOP_MASK | ETHHW_DMA_CFG_OFFSET( bufp, len );

   /* Setup descriptor */
   bufp = (uint8_t *) TX_BUF(index);
   cfgp = (uint64_t *) CFG_BUF(index);
   descp = (ETHHW_DMA_DESC *) TX_DESC(2 * index);

   // Tx config descriptor
   // The config descriptor allows transfer to be purely 64-bit
   // transactions, spanning an arbitrary number of descriptors,
   // so information must be provided to define offsets and EOP
   ETHHW_DMA_DESC_CREATE( 
      descp,
      cfgp, 
      ETHHW_DMA_MTP_FIFO_ADDR_CFG,
      descp,
      ETHHW_DMA_MTP_CTL_LO, 
      ETHHW_DMA_MTP_TRANSACTION_SIZE( cfgp, sizeof( uint64_t ) ) 
      );

   descp++;

   // Tx data descriptor, only one data buffer will be added
   ETHHW_DMA_DESC_CREATE_NEXT( 
      descp,
      bufp, 
      ETHHW_DMA_MTP_FIFO_ADDR_DATA,
      NULL,
      ETHHW_DMA_MTP_CTL_LO, 
      ETHHW_DMA_MTP_TRANSACTION_SIZE( bufp, len) 
      );

   //Flush data, config, and descriptors to external memory
   TX_FLUSH_CACHE();

#ifdef BCM111XX_ETH_DEBUG
   txDump(index, len);
#endif
   return 0;
}

/******************************************************************
 * u-boot net functions
 */
static int bcm111xx_eth_send(struct eth_device *dev, volatile void *packet, int length)
{
   uint8_t *buf = (uint8_t *) packet;
   int rc = 0;
   int i = 0;
   
   debug("bcm111xx net start Tx: %d\n", length);

   /* Always use 1st buffer since send is synchronous */
   txPacketAdd(0, length, buf);

   /* Start transmit */
   ethHw_dmaEnable( ETH_DMA_CONTROLLER, ETH_DMA_CH_TX, TX_DESC( 0 ) );

   while (!ETHHW_DMA_MTP_TRANSFER_DONE(TX_DESC(0))) 
   {
      udelay(100);
      debug(".");
      i++;
      if(i > 20)
      {
         error("\nbcm111xx ethernet Tx failure! Already retried 20 times\n");
         rc = -1;
         break;
      }
   }

   ethHw_dmaDisable( ETH_DMA_CONTROLLER, ETH_DMA_CH_TX );

   return rc;
}

static int bcm111xx_eth_rcv(struct eth_device *dev)
{
   uint8_t *buf = (uint8_t *) NetRxPackets[0];
   int rc = 0;
   volatile ETHHW_DMA_DESC *descp = RX_DESC( 0 );   // First Rx descriptor
   uint8_t *bufp;
   uint16_t rcvlen;
   uint32_t status;

   //debug("entering bcm111xx_eth_rcv\n");

   udelay(50);

   while(1)
   {
      /* Poll Rx queue to get a packet */
      if( ETHHW_DMA_PTM_TRANSFER_DONE( descp ) )
      {
         debug("recieved \n");
         bufp = (uint8_t *)descp->dar;

         status = descp->stat1;   // Get SSTATx
         //rcvlen = ((status >> 13) & 0x0001ffff);
         rcvlen = ETHHW_DMA_BUF_LEN(descp);
         
         if(( rcvlen == 0) || (rcvlen > RX_BUF_SIZE))
         {
            error("Wrong Rx packet size %d, drop it\n", rcvlen);
            ETHHW_DMA_DESC_RX_UPDATE( descp, descp->dar, RX_BUF_SIZE );
            break;
         }

         memcpy(buf, bufp, 12);
         /* skip brcmTag */
         memcpy(buf + 12, bufp + 16, rcvlen - 12 - 4);
         rcvlen -= 4;

#ifdef BCM111XX_ETH_DEBUG
         debug("Rx Buf: %d\n", rcvlen);
         int i;
         for (i = 0; i < 40; i++) {
            debug("%02X ", buf[i]);
         }
         debug("\n");
#endif
         /* A packet has been received, so forward to uboot network handler */
         NetReceive(buf, rcvlen);

         ETHHW_DMA_DESC_RX_UPDATE( descp, descp->dar, RX_BUF_SIZE );
      }
      else
      {
         //debug("Rx");
         descp = (ETHHW_DMA_DESC *)(descp)->lli;                   // Advance to next descriptor
         
         if(descp == RX_DESC( 0 ))
         {
            /* Tell caller that no packet was received when Rx queue was polled */
            rc = -1;
            //debug("\nNO Rx\n");
            break;
         }
      }
   }

   return rc;
}

static inline void ethMiiSet(int port, int addr, int data)
{
   //ethHw_miiSet(port, 0, (uint32_t) addr, (uint32_t) data);
}

int bcm111xx_miiphy_read(char *devname, unsigned char const addr, 
   unsigned char const reg, unsigned short *const value)
{
   //ethHw_miiGet(PORT_LAN, 0, (uint32_t) addr, value);
   return 0;
}

int bcm111xx_miiphy_write(char *devname, unsigned char const addr, 
   unsigned char const reg, unsigned short *const value)
{
   //ethHw_miiSet(PORT_LAN, 0, (uint32_t) addr, (uint32_t) reg);
   return 0;
}

static int bcm111xx_eth_write_hwaddr(struct eth_device* dev)
{
   int rc;

   printf("\nMAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
      dev->enetaddr[0], dev->enetaddr[1], dev->enetaddr[2],
      dev->enetaddr[3], dev->enetaddr[4], dev->enetaddr[5]);

   rc = ethHw_arlEntrySet((char *)dev->enetaddr, 0, ETHHW_PORT_INT, 0, 1, 1);

   if (rc != ETHHW_RC_NONE) 
   {
      error("MAC set error!\n");
      return -1;
   }

   return 0;
}

static int bcm111xx_eth_open( struct eth_device *dev, bd_t * bt )
{
   uint32_t lanSpeed;
   ETHHW_DMA_DESC *descp = NULL;
   uint8_t *bufp;
   int extPort;
   int i;

   debug( "Enable BCM111XX Ethernet \n" );

   /* Set MAC address from env */
   if (bcm111xx_eth_write_hwaddr(dev) != 0) 
   {
      error("MAC set error when opening !\n");
      return -1;
   }

   /* Check which port is connected and take PORT0 with priority */
   if( ethHw_portLinkStatus(ETHHW_PORT_0) )
   {
      extPort = ETHHW_PORT_0;
   }
   else if( ethHw_portLinkStatus(ETHHW_PORT_1) )
   {
      extPort = ETHHW_PORT_1;
   }
   else
   {
      error(" None of Ethernet external ports are connected \n");
      return -1;
   }
   
   lanSpeed = ethHw_portSpeed(extPort);
   /* Configure internal port speed 
    * always 1Gb (switch->CPU) and follow external port speed (CPU->switch)
    */
   ethHw_impSpeedSet(lanSpeed, 1000);

   /* Enable forwarding to internal port */
   ethHw_impEnableSet(1);
   ethHw_macEnableSet(ETHHW_PORT_INT, 1, 1);

   int rx, tx;
   ethHw_impSpeedGet(&rx, &tx);
   debug("external port speed(%d), internal port speed: CPU->port(%d), port->CPU(%d)\n", 
      lanSpeed, rx, tx);

   /* Initialize RX DMA descriptors */
   for (i = 0; i < RX_BUF_NUM; i++) 
   {
      bufp = (uint8_t *) RX_BUF(i);
      descp = (ETHHW_DMA_DESC *) RX_DESC(i);

      // Rx config descriptor
      // The config descriptor allows transfer to be purely 64-bit
      // transactions, spanning an arbitrary number of descriptors,
      // so information must be provided to define offsets and EOP
      ETHHW_DMA_DESC_CREATE( 
         descp,
         ETHHW_DMA_PTM_FIFO_ADDR,
         bufp, 
         descp,
         ETHHW_DMA_PTM_CTL_LO, 
         RX_BUF_SIZE 
         );

      descp++;
   }
   // Wrap last descriptor back to beginning
   descp--;
   ETHHW_DMA_DESC_WRAP(descp, (uint32_t)RX_DESC(0));

   /* Init DMA for Ethernet */
   ethHw_dmaInit(ETH_DMA_CONTROLLER);
   ethHw_dmaDisable(ETH_DMA_CONTROLLER, ETH_DMA_CH_RX);
   ethHw_dmaDisable(ETH_DMA_CONTROLLER, ETH_DMA_CH_TX);

   /* Configure Rx DMA */
   ethHw_dmaRxConfig(ETH_DMA_BURST_SIZE, ETH_DMA_BLOCK_SIZE);
   ethHw_dmaConfig( ETH_DMA_CONTROLLER, ETH_DMA_CH_RX,
              ETHHW_DMA_PTM_CTL_HI, ETHHW_DMA_PTM_CTL_LO,
              ETHHW_DMA_PTM_CFG_HI, ETHHW_DMA_PTM_CFG_LO,
              ETHHW_DMA_PTM_SSTAT, 0 );

   // Configure Tx DMA
   ethHw_dmaTxConfig(ETH_DMA_BURST_SIZE);
   ethHw_dmaConfig( ETH_DMA_CONTROLLER, ETH_DMA_CH_TX,
              ETHHW_DMA_MTP_CTL_HI, ETHHW_DMA_MTP_CTL_LO,
              ETHHW_DMA_MTP_CFG_HI, ETHHW_DMA_MTP_CFG_LO,
              0, ETHHW_DMA_MTP_DSTAT );

   /* If RX DMA is not enabled, packet will be held in switch for a while, 
    * but eventually it will be dropped so that RX DMA should be enabled all the time
    */
   ethHw_dmaEnable(ETH_DMA_CONTROLLER, ETH_DMA_CH_RX, RX_DESC(0));

   debug( "Enable Ethernet Done \n" );

   return 0;
}

static void bcm111xx_eth_close(struct eth_device *dev)
{
   debug("entering bcm111xx_eth_close!\n");

   ethHw_dmaDisable(ETH_DMA_CONTROLLER, ETH_DMA_CH_RX);
   ethHw_dmaDisable(ETH_DMA_CONTROLLER, ETH_DMA_CH_TX);

   /* Disable forwarding to internal port */
   ethHw_macEnableSet(ETHHW_PORT_INT, 0, 0);
   ethHw_impEnableSet(0);

   debug("bcmring_net_close!\n");
}

int bcm111xx_eth_register(u8 dev_num)
{
   struct eth_device *dev;
   int rc;

   dev = (struct eth_device *) malloc(sizeof(struct eth_device));
   if (dev == NULL) {
      return -1;
   }

   printf(banner);

   memset(dev, 0, sizeof(*dev));
   sprintf(dev->name, "%s-%hu", BCM111XX_ETH_DEV_NAME, dev_num);
   
   /* Initialization */
   debug( "Ethernet initialization ..." );
   
#ifdef CONFIG_BCMHANA_ETH
   rc = ethPhyStart();
   if( rc == ETHHW_RC_NONE )
   {
      debug( "PHY successful\n" );
   }
   else
   {
      error( "PHY failed\n" );
      return -1;
   }
#endif

   rc = ethHw_Init();
   debug("Ethernet initialization %s (rc=%i)\n",
          ETHHW_RC_SUCCESS(rc) ? "successful" : "failed", rc);

   /* Disable forwarding to internal port (this must be done before
    * forwarding is enabled on the external ports)
    */
   ethHw_macEnableSet(ETHHW_PORT_INT, 0, 0);
   /* Disable internal port */
   ethHw_impEnableSet(0);

   /* STP not used so put external ports in forwarding state */
   /* TODO:  If STP support is required, this state control will need 
    * to be moved to the STP application
    */
   ethHw_stpStateSet(ETHHW_PORT_0, ETHHW_STP_STATE_FORWARDING);
   ethHw_stpStateSet(ETHHW_PORT_1, ETHHW_STP_STATE_FORWARDING);

   dev->iobase = 0;

   dev->init = bcm111xx_eth_open;
   dev->halt = bcm111xx_eth_close;
   dev->send = bcm111xx_eth_send;
   dev->recv = bcm111xx_eth_rcv;
   dev->write_hwaddr = bcm111xx_eth_write_hwaddr;

   eth_register(dev);

#ifdef CONFIG_CMD_MII
   miiphy_register(dev->name, bcm111xx_mii_read, bcm111xx_mii_write);
#endif

   debug( "Basic ethernet functionality initialized\n" );

#ifdef BCM111XX_ETH_DEBUG
   bcm111xx_eth_info();
#endif

   return 1;
}
