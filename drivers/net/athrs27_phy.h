#ifndef _ATHRS16_PHY_H
#define _ATHRS16_PHY_H


/****************************/
/* Global control Registers */
/****************************/
#define ATHR_PWS_REG					0x0010
#define ATHR_GLOBAL_FW_CTRL1			0x0624
#define ATHR_PORT0_PAD_MODE_CTRL		0x0004
#define ATHR_PORT6_PAD_MODE_CTRL		0x000C
#define ATHR_PORT0_STATUS				0x007C
#define ATHR_PORT1_STATUS				0x0080
#define ATHR_PORT2_STATUS				0x0084
#define ATHR_PORT3_STATUS				0x0088
#define ATHR_PORT4_STATUS				0x008C
#define ATHR_PORT5_STATUS				0x0090
#define ATHR_PORT6_STATUS				0x0094

#define ATHR_DBG_PORT_ADDR				0x001D
#define ATHR_DBG_PORT_DATA				0x001E


/*****************/
/* PHY Registers */
/*****************/
#define ATHR_PHY_CONTROL                 0
#define ATHR_PHY_STATUS                  1
#define ATHR_PHY_ID1                     2
#define ATHR_PHY_ID2                     3
#define ATHR_AUTONEG_ADVERT              4
#define ATHR_LINK_PARTNER_ABILITY        5
#define ATHR_AUTONEG_EXPANSION           6
#define ATHR_NEXT_PAGE_TRANSMIT          7
#define ATHR_LINK_PARTNER_NEXT_PAGE      8
#define ATHR_1000BASET_CONTROL           9
#define ATHR_1000BASET_STATUS            10
#define ATHR_PHY_SPEC_CONTROL            16
#define ATHR_PHY_SPEC_STATUS             17
#define ATHR_DEBUG_PORT_ADDRESS          29
#define ATHR_DEBUG_PORT_DATA             30

/* ATHR_PHY_CONTROL fields */
#define ATHR_CTRL_SOFTWARE_RESET                    0x8000
#define ATHR_CTRL_SPEED_LSB                         0x2000
#define ATHR_CTRL_AUTONEGOTIATION_ENABLE            0x1000
#define ATHR_CTRL_RESTART_AUTONEGOTIATION           0x0200
#define ATHR_CTRL_SPEED_FULL_DUPLEX                 0x0100
#define ATHR_CTRL_SPEED_MSB                         0x0040

#define ATHR_RESET_DONE(phy_control)                   \
    (((phy_control) & (ATHR_CTRL_SOFTWARE_RESET)) == 0)
    
/* Phy status fields */
#define ATHR_STATUS_AUTO_NEG_DONE                   0x0020

#define ATHR_AUTONEG_DONE(ip_phy_status)                   \
    (((ip_phy_status) &                                  \
        (ATHR_STATUS_AUTO_NEG_DONE)) ==                    \
        (ATHR_STATUS_AUTO_NEG_DONE))

/* Link Partner ability */
#define ATHR_LINK_100BASETX_FULL_DUPLEX       0x0100
#define ATHR_LINK_100BASETX                   0x0080
#define ATHR_LINK_10BASETX_FULL_DUPLEX        0x0040
#define ATHR_LINK_10BASETX                    0x0020

/* Advertisement register. */
#define ATHR_ADVERTISE_NEXT_PAGE              0x8000
#define ATHR_ADVERTISE_ASYM_PAUSE             0x0800
#define ATHR_ADVERTISE_PAUSE                  0x0400
#define ATHR_ADVERTISE_100FULL                0x0100
#define ATHR_ADVERTISE_100HALF                0x0080  
#define ATHR_ADVERTISE_10FULL                 0x0040  
#define ATHR_ADVERTISE_10HALF                 0x0020  

#define ATHR_ADVERTISE_ALL (ATHR_ADVERTISE_ASYM_PAUSE | ATHR_ADVERTISE_PAUSE | \
                            ATHR_ADVERTISE_10HALF | ATHR_ADVERTISE_10FULL | \
                            ATHR_ADVERTISE_100HALF | ATHR_ADVERTISE_100FULL)
                       
/* 1000BASET_CONTROL */
#define ATHR_ADVERTISE_1000FULL               0x0200

/* Phy Specific status fields */
#define ATHER_STATUS_LINK_MASK                0xC000
#define ATHER_STATUS_LINK_SHIFT               14
#define ATHER_STATUS_FULL_DEPLEX              0x2000
#define ATHR_STATUS_LINK_PASS                 0x0400 
#define ATHR_STATUS_RESOVLED                  0x0800

/*phy debug port  register */
#define ATHER_DEBUG_SERDES_REG                5

/* Serdes debug fields */
#define ATHER_SERDES_BEACON                   0x0100

#ifndef BOOL
#define BOOL    int
#endif

/*add feature define here*/
//#define HEADER_REG_CONF
#define FULL_FEATURE
/*end of feature define*/



#ifdef HEADER_REG_CONF
#define HEADER_EN
#endif

#define HEADER_LEN   2
#define VID_LEN   2
#define HEADER_MAX_DATA  10

typedef enum {
    NORMAL_PACKET, 
    RESERVED0,
    MIB_1ST, 
    RESERVED1,
    RESERVED2,
    READ_WRITE_REG,
    READ_WRITE_REG_ACK,
    RESERVED3				        
} AT_HEADER_TYPE;

typedef struct {
    uint16_t    version0; 
    uint16_t    priority;  
    uint16_t    type ;     
    uint16_t    broadcast; 
    uint16_t    from_cpu;  
    uint16_t    version1; 
    uint16_t    port_num;  
}at_header_t;

typedef struct {
    uint64_t    reg_addr;	
    uint64_t    reserved0;	
    uint64_t    cmd_len;		
    uint64_t    reserved1; 	
    uint64_t    cmd; 
    uint64_t    check_code;	
    uint64_t    seq_num;  
}reg_cmd_t;

typedef struct {
    uint8_t data[HEADER_MAX_DATA];
    uint8_t len;
    volatile uint32_t seq;
} cmd_resp_t;

BOOL athrs16_phy_is_link_alive(struct eth_device *dev, int phyUnit);
uint32_t athrs16_reg_read(struct eth_device *dev, uint32_t reg_addr);
void athrs16_reg_write(struct eth_device *dev, uint32_t reg_addr, uint32_t reg_val);
void athrs16_reg_init(struct eth_device *dev);
void athrs16_speed_init(struct eth_device *dev, int cpu_port_num,u32 speed, u32 duplex);
int athrs16_phy_is_up(int unit);
int athrs16_phy_is_fdx(struct eth_device *dev, int ethUnit);
int athrs16_phy_speed(int unit);
BOOL athrs16_phy_setup(struct eth_device *dev, int unit);
void athrs16_mdio_write(struct eth_device *dev, uint32_t reg_addr, uint32_t reg_val);
uint32_t athrs16_mdio_read(struct eth_device *dev, uint32_t reg_addr);
#endif
