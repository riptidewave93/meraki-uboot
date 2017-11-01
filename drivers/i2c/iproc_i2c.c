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

#include <common.h>
#include <config.h>
#include <i2c.h>
#include <asm/io.h>

#include "iproc_i2c_regs.h"
#include "iproc_i2c_defs.h"
#include "iproc_i2c.h"
#include "errno.h"



/* local debug macro */
#undef IPROC_I2C_DBG

#undef debug
#ifdef IPROC_I2C_DBG
#define debug(fmt,args...)	printf (fmt ,##args)
#else
#define debug(fmt,args...)
#endif /* I2C_DEBUG */

static smb_clk_freq_t  smb_clk_speed;
static int  i2c_init_done = 0;

// WNC Steve 2013/08/08
extern u8 i2cWord;
extern u8 i2cByte; // send/recv byte
// WNC

// WNC Steve 2013/08/23
u8 i2cdata[8];
// WNC

#if (defined(CONFIG_NS_PLUS) || defined(CONFIG_HELIX4)) 
#define SHADOW_CPY_BUFFER 0x70000000
#define IPROC_SMBUS_BASE_ADDR   (0x18038000)
//#define IPROC_SMBUS_BASE_ADDR   (0x1803B000)
#else
#define SHADOW_CPY_BUFFER 0x89000000
#define IPROC_SMBUS_BASE_ADDR   (0x18009000)
#endif

/* Function to read a value from specified register. */
static unsigned int iproc_i2c_reg_read(unsigned long reg_addr)
{
    return( __raw_readl((void *)(IPROC_SMBUS_BASE_ADDR + reg_addr)) );
}

/* Function to write a value ('val') in to a specified register. */
static int iproc_i2c_reg_write(unsigned long reg_addr, unsigned int val)
{
    __raw_writel(val, (void *)(IPROC_SMBUS_BASE_ADDR + reg_addr));
    return (0);
}

static int iproc_dump_i2c_regs(void)
{
    unsigned int regval;

    debug("\n----------------------------------------------\n");
    debug("%s: Dumping SMBus registers... \n", __func__);

    regval = iproc_i2c_reg_read(CCB_SMB_CFG_REG);
    debug("CCB_SMB_CFG_REG=0x%08X\n", regval);

    regval = iproc_i2c_reg_read(CCB_SMB_TIMGCFG_REG);
    debug("CCB_SMB_TIMGCFG_REG=0x%08X\n", regval);

    regval = iproc_i2c_reg_read(CCB_SMB_ADDR_REG);
    debug("CCB_SMB_ADDR_REG=0x%08X\n", regval);

    regval = iproc_i2c_reg_read(CCB_SMB_MSTRFIFOCTL_REG);
    debug("CCB_SMB_MSTRFIFOCTL_REG=0x%08X\n", regval);

    regval = iproc_i2c_reg_read(CCB_SMB_SLVFIFOCTL_REG);
    debug("CCB_SMB_SLVFIFOCTL_REG=0x%08X\n", regval);

    regval = iproc_i2c_reg_read(CCB_SMB_BITBANGCTL_REG);
    debug("CCB_SMB_BITBANGCTL_REG=0x%08X\n", regval);

    regval = iproc_i2c_reg_read(CCB_SMB_MSTRCMD_REG);
    debug("CCB_SMB_MSTRCMD_REG=0x%08X\n", regval);

    regval = iproc_i2c_reg_read(CCB_SMB_SLVCMD_REG);
    debug("CCB_SMB_SLVCMD_REG=0x%08X\n", regval);

    regval = iproc_i2c_reg_read(CCB_SMB_EVTEN_REG);
    debug("CCB_SMB_EVTEN_REG=0x%08X\n", regval);

    regval = iproc_i2c_reg_read(CCB_SMB_EVTSTS_REG);
    debug("CCB_SMB_EVTSTS_REG=0x%08X\n", regval);

    regval = iproc_i2c_reg_read(CCB_SMB_MSTRDATAWR_REG);
    debug("CCB_SMB_MSTRDATAWR_REG=0x%08X\n", regval);

    regval = iproc_i2c_reg_read(CCB_SMB_MSTRDATARD_REG);
    debug("CCB_SMB_MSTRDATARD_REG=0x%08X\n", regval);

    regval = iproc_i2c_reg_read(CCB_SMB_SLVDATAWR_REG);
    debug("CCB_SMB_SLVDATAWR_REG=0x%08X\n", regval);

    regval = iproc_i2c_reg_read(CCB_SMB_SLVDATARD_REG);
    debug("CCB_SMB_SLVDATARD_REG=0x%08X\n", regval);

    debug("----------------------------------------------\n\n");
    return(0);
}

/*
 * Function to ensure that the previous transaction was completed before
 * initiating a new transaction. It can also be used in polling mode to
 * check status of completion of a command
 */
static int iproc_i2c_startbusy_wait(void)
{
    unsigned int regval;

    regval = iproc_i2c_reg_read(CCB_SMB_MSTRCMD_REG);

    /* Check if an operation is in progress. During probe it won't be.
     * But when shutdown/remove was called we want to make sure that
     * the transaction in progress completed
     */
    if (regval & CCB_SMB_MSTRSTARTBUSYCMD_MASK) {
        unsigned int i = 0;

        do {
               udelay(1000); /* Wait for 1 msec */
               i++;
               regval = iproc_i2c_reg_read(CCB_SMB_MSTRCMD_REG);

          /* If start-busy bit cleared, exit the loop */
        } while ((regval & CCB_SMB_MSTRSTARTBUSYCMD_MASK) &&
                 (i < IPROC_SMB_MAX_RETRIES));

        if (i >= IPROC_SMB_MAX_RETRIES) {
            printf("%s: START_BUSY bit didn't clear, exiting\n",
                   __func__);;
            return -ETIMEDOUT;
        }
    }
    return 0;
}

/*
 * This function copies data to SMBus's Tx FIFO. Valid for write transactions
 * only
 *
 * base_addr: Mapped address of this SMBus instance
 * dev_addr:  SMBus (I2C) device address. We are assuming 7-bit addresses
 *            initially
 * info:   Data to copy in to Tx FIFO. For read commands, the size should be
 *         set to zero by the caller
 *
 */
static void iproc_i2c_write_trans_data(unsigned short dev_addr,
                                       struct iproc_xact_info *info)
{
    unsigned int regval;
    unsigned int i;
    unsigned int num_data_bytes = 0;

#ifdef IPROC_I2C_DBG
    debug("\n%s: dev_addr=0x%X, cmd_valid=%d, cmd=0x%02x, size=%u\n", 
                __func__, dev_addr, info->cmd_valid, info->command, info->size);
#endif /* IPROC_I2C_DBG */

    /* Write SMBus device address first */
    /* Note, we are assuming 7-bit addresses for now. For 10-bit addresses,
     * we may have one more write to send the upper 3 bits of 10-bit addr
     */
// WNC Steve 2013/08/22
    if (info->smb_proto == SMBUS_PROT_RECV_BYTE)
        iproc_i2c_reg_write(CCB_SMB_MSTRDATAWR_REG, dev_addr | 0x1);
    else
        iproc_i2c_reg_write(CCB_SMB_MSTRDATAWR_REG, dev_addr);
// WNC

    /* If the protocol needs command code, copy it */
// WNC Steve 2013/08/22
    if (info->smb_proto != SMBUS_PROT_SEND_BYTE)
// WNC
    if (info->cmd_valid) {
        iproc_i2c_reg_write(CCB_SMB_MSTRDATAWR_REG, info->command);
    }

    /* Depending on the SMBus protocol, we need to write additional transaction
     * data in to Tx FIFO. Refer to section 5.5 of SMBus spec for sequence for a
     * transaction
     */
    switch (info->smb_proto) {

        case SMBUS_PROT_RECV_BYTE:
            /* No additional data to be written */
            num_data_bytes = 0;
        break;

        case SMBUS_PROT_SEND_BYTE:
            num_data_bytes = info->size;
        break;

        case SMBUS_PROT_RD_BYTE:
        case SMBUS_PROT_RD_WORD:
        case SMBUS_PROT_BLK_RD:
            /* Write slave address with R/W~ set (bit #0) */
            iproc_i2c_reg_write(CCB_SMB_MSTRDATAWR_REG, dev_addr | 0x1);
            num_data_bytes = 0;
        break;

        case SMBUS_PROT_WR_BYTE:
        case SMBUS_PROT_WR_WORD:
            /* No additional bytes to be written. Data portion is written in the
             * 'for' loop below
             */
            num_data_bytes = info->size;
        break;

        case SMBUS_PROT_BLK_WR:
            /* 3rd byte is byte count */
            iproc_i2c_reg_write(CCB_SMB_MSTRDATAWR_REG, info->size);
            num_data_bytes = info->size;
        break;

        default:
        break;

    }

    /* Copy actual data from caller, next. In general, for reads, no data is
     * copied
     */
    for (i = 0; num_data_bytes; --num_data_bytes, i++) {

        /* For the last byte, set MASTER_WR_STATUS bit */
        regval = (num_data_bytes == 1) ? 
                                       info->data[i] | CCB_SMB_MSTRWRSTS_MASK :
                                       info->data[i];

        iproc_i2c_reg_write(CCB_SMB_MSTRDATAWR_REG, regval);
    }

    return;
}

static int iproc_i2c_data_send(unsigned short addr,
                               struct iproc_xact_info *info)
{
    int rc, retry=3;
    unsigned int regval;

    /* Make sure the previous transaction completed */
    rc = iproc_i2c_startbusy_wait();

    if (rc < 0) {
        printf("%s: Send: bus is busy, exiting\n", __func__);;
        return rc;
    }

    /* Write transaction bytes to Tx FIFO */
    iproc_i2c_write_trans_data(addr, info);

    /* Program master command register (0x30) with protocol type and set
     * start_busy_command bit to initiate the write transaction
     */
    regval = (info->smb_proto << CCB_SMB_MSTRSMBUSPROTO_SHIFT) |
              CCB_SMB_MSTRSTARTBUSYCMD_MASK;

    iproc_i2c_reg_write(CCB_SMB_MSTRCMD_REG, regval);

    /* Check for Master status */
    regval = iproc_i2c_reg_read(CCB_SMB_MSTRCMD_REG);
    while(regval & CCB_SMB_MSTRSTARTBUSYCMD_MASK)
    {
        udelay(100);
        if(retry-- <=0) break;
        regval = iproc_i2c_reg_read(CCB_SMB_MSTRCMD_REG);
    }        

    /* If start_busy bit cleared, check if there are any errors */
    if (!(regval & CCB_SMB_MSTRSTARTBUSYCMD_MASK)) {

        /* start_busy bit cleared, check master_status field now */
        regval &= CCB_SMB_MSTRSTS_MASK;
        regval >>= CCB_SMB_MSTRSTS_SHIFT;

        if (regval != MSTR_STS_XACT_SUCCESS) {
            /* Error We can flush Tx FIFO here */
            printf("%s: ERROR: Send: Error in transaction %u, exiting",
                   __func__, regval);
           return -EREMOTEIO;
        }
    }

    return(0);
}

static int iproc_i2c_data_recv(unsigned short addr,
                               struct iproc_xact_info *info,
                               unsigned int *num_bytes_read)
{
    int rc, retry=3;
    unsigned int regval;

    /* Make sure the previous transaction completed */
    rc = iproc_i2c_startbusy_wait();

    if (rc < 0) {
        printf("%s: Receive: Bus is busy, exiting\n", __func__);;
        return rc;
    }

    /* Program all transaction bytes into master Tx FIFO */
    iproc_i2c_write_trans_data(addr, info);

    /* Program master command register (0x30) with protocol type and set
     * start_busy_command bit to initiate the write transaction
     */
    regval = (info->smb_proto << CCB_SMB_MSTRSMBUSPROTO_SHIFT) |
              CCB_SMB_MSTRSTARTBUSYCMD_MASK | info->size;

    iproc_i2c_reg_write(CCB_SMB_MSTRCMD_REG, regval);

    /* Check for Master status */
    regval = iproc_i2c_reg_read(CCB_SMB_MSTRCMD_REG);
    while(regval & CCB_SMB_MSTRSTARTBUSYCMD_MASK)
    {
        udelay(100);
        if(retry-- <=0) break;
        regval = iproc_i2c_reg_read(CCB_SMB_MSTRCMD_REG);
    } 

    /* If start_busy bit cleared, check if there are any errors */
    if (!(regval & CCB_SMB_MSTRSTARTBUSYCMD_MASK)) {

        /* start_busy bit cleared, check master_status field now */
        regval &= CCB_SMB_MSTRSTS_MASK;
        regval >>= CCB_SMB_MSTRSTS_SHIFT;

        if (regval != MSTR_STS_XACT_SUCCESS) {
            /* We can flush Tx FIFO here */
            printf("%s: Error in transaction %d, exiting",
                   __func__, regval);
           return -EREMOTEIO;
        }
    }

    /* Read received byte(s), after TX out address etc */
    regval = iproc_i2c_reg_read(CCB_SMB_MSTRDATARD_REG);

    /* For block read, protocol (hw) returns byte count, as the first byte */
    if (info->smb_proto == SMBUS_PROT_BLK_RD) {

        int i;

        *num_bytes_read = regval & CCB_SMB_MSTRRDDATA_MASK;

        /* Limit to reading a max of 32 bytes only; just a safeguard. If
         * # bytes read is a number > 32, check transaction set up, and contact
         * hw engg. Assumption: PEC is disabled 
         */
        for (i = 0; (i < *num_bytes_read) && (i < I2C_SMBUS_BLOCK_MAX); i++) {

            /* Read Rx FIFO for data bytes */
            regval = iproc_i2c_reg_read(CCB_SMB_MSTRDATARD_REG);
            info->data[i] = regval & CCB_SMB_MSTRRDDATA_MASK;
        }
    }
    else {
// WNC Steve 2013/08/23
        /* 1 Byte data */
        //*(info->data) = regval & CCB_SMB_MSTRRDDATA_MASK;
        //*num_bytes_read = 1;
    
        int i;

        info->data[0] = regval & CCB_SMB_MSTRRDDATA_MASK;
        i2cdata[0] = info->data[0];

        for (i=1; i<info->size; i++)
        {
            udelay(1000); // delay for the previous command finished
            
            regval = iproc_i2c_reg_read(CCB_SMB_MSTRDATARD_REG);
            info->data[i] = regval & CCB_SMB_MSTRRDDATA_MASK;
            i2cdata[i] = info->data[i];
        }
        *num_bytes_read = info->size;
// WNC
    }

    return(0);
}


/*
 * This function set clock frequency for SMBus block. As per hardware
 * engineering, the clock frequency can be changed dynamically.
 */
static int iproc_i2c_set_clk_freq(smb_clk_freq_t freq)
{
    unsigned int regval;
    unsigned int val;

    switch (freq) {

        case I2C_SPEED_100KHz:
            val = 0;
            break;

        case I2C_SPEED_400KHz:
            val = 1;
            break;

        default:
            return -EINVAL;
            break;
    }
    
    regval = iproc_i2c_reg_read(CCB_SMB_TIMGCFG_REG);

    SETREGFLDVAL(regval, val, CCB_SMB_TIMGCFG_MODE400_MASK, 
                 CCB_SMB_TIMGCFG_MODE400_SHIFT);

    iproc_i2c_reg_write(CCB_SMB_TIMGCFG_REG, regval);
    return(0);
}

void i2c_init (int speed, int slaveadd)
{
    unsigned int regval;

#ifdef IPROC_I2C_DBG
    debug("\n%s: Entering i2c_init\n", __func__);
#endif /* IPROC_I2C_DBG */

    /* Flush Tx, Rx FIFOs. Note we are setting the Rx FIFO threshold to 0.
     * May be OK since we are setting RX_EVENT and RX_FIFO_FULL interrupts
     */
    regval = CCB_SMB_MSTRRXFIFOFLSH_MASK | CCB_SMB_MSTRTXFIFOFLSH_MASK;
    iproc_i2c_reg_write(CCB_SMB_MSTRFIFOCTL_REG, regval);

    /* Enable SMbus block. Note, we are setting MASTER_RETRY_COUNT to zero
     * since there will be only one master
     */
    regval = CCB_SMB_CFG_SMBEN_MASK;
    iproc_i2c_reg_write(CCB_SMB_CFG_REG, regval);

    /* Wait a minimum of 50 Usec, as per SMB hw doc. But we wait longer */
    udelay(100);

    /* Set default clock frequency */
    iproc_i2c_set_clk_freq(I2C_SPEED_100KHz);

    /* Disable intrs */
    regval = 0x0;
    iproc_i2c_reg_write(CCB_SMB_EVTEN_REG, regval);

    /* Clear intrs (W1TC) */
    regval = iproc_i2c_reg_read(CCB_SMB_EVTSTS_REG);    
    iproc_i2c_reg_write(CCB_SMB_EVTSTS_REG, regval);

    i2c_init_done = 1;

#ifdef IPROC_I2C_DBG
    iproc_dump_i2c_regs();

    debug("%s: Init successful\n", __func__);
#endif /* IPROC_I2C_DBG */

    return(0);
}

int i2c_probe (uchar chip)
{

#ifdef IPROC_I2C_DBG
    debug("\n%s: Entering probe\n", __func__);
#endif /* IPROC_I2C_DBG */

    /* Init internal regs, disable intrs (and then clear intrs), set fifo
     * thresholds, etc.
     */
    if(! i2c_init_done)
        i2c_init(0, 0);

#ifdef IPROC_I2C_DBG
    iproc_dump_i2c_regs();

    debug("%s: probe successful\n", __func__);
#endif /* IPROC_I2C_DBG */

    return 0;
}

// WNC Steve 2013/08/29
static int i2c_recv_byte (u8 devaddr, u8 regoffset, u8 * value)
{
    int rc;
    struct iproc_xact_info info;
    unsigned int num_bytes_read = 0;

// e.g. devaddr=0100 0000(2)=0x40
    devaddr <<= 1;
// e.g. devaddr   = 1000 0000(2)
//      regoffset = addr + i     = 0x0

    info.cmd_valid = 1;
    info.command = (unsigned char)regoffset;
    info.data = value;
    info.size = 1;
    info.flags = 0;
    info.smb_proto = SMBUS_PROT_RECV_BYTE;

    /* Refer to i2c_smbus_read_byte for params passed. */
    rc = iproc_i2c_data_recv(devaddr, &info, &num_bytes_read);

    if (rc < 0) {
        printf("%s: %s error accessing device 0x%X", 
                    __func__, "Read", devaddr);
        return -EREMOTEIO;
    }

    return (0);
}
// WNC

static int i2c_read_byte (u8 devaddr, u8 regoffset, u8 * value)
{
    int rc;
    struct iproc_xact_info info;
    unsigned int num_bytes_read = 0;

    devaddr <<= 1;

    info.cmd_valid = 1;
    info.command = (unsigned char)regoffset;
    info.data = value;
    info.size = 1;
    info.flags = 0;
    info.smb_proto = SMBUS_PROT_RD_BYTE;

    /* Refer to i2c_smbus_read_byte for params passed. */
    rc = iproc_i2c_data_recv(devaddr, &info, &num_bytes_read);

    if (rc < 0) {
        printf("%s: %s error accessing device 0x%X", 
                    __func__, "Read", devaddr);
        return -EREMOTEIO;
    }

    return (0);
}

// WNC Steve 2013/08/09
static int i2c_read_word (u8 devaddr, u8 regoffset, u8 * value)
{
    int rc;
    struct iproc_xact_info info;
    unsigned int num_bytes_read = 0;

// e.g. devaddr=0100 0000(2)=0x40
    devaddr <<= 1;
// e.g. devaddr   = 1000 0000(2)
//      regoffset = addr + i     = 0x0

    info.cmd_valid = 1;
    info.command = (unsigned char)regoffset;
    info.data = value;
    info.size = 2;
    info.flags = 0;
    info.smb_proto = SMBUS_PROT_RD_WORD;

    /* Refer to i2c_smbus_read_byte for params passed. */
    rc = iproc_i2c_data_recv(devaddr, &info, &num_bytes_read);

    if (rc < 0) {
        printf("%s: %s error accessing device 0x%X", 
                    __func__, "Read", devaddr);
        return -EREMOTEIO;
    }

    return (0);
}
// WNC

int i2c_read (uchar chip, uint addr, int alen, uchar * buffer, int len)
{
	int i;

	if (alen > 1) {
		printf ("I2C read: addr len %d not supported\n", alen);
		return 1;
	}

	if (addr + len > 256) {
		printf ("I2C read: address out of range\n");
		return 1;
	}

// WNC Steve 2013/08/23
    if (i2cWord)
    {
    	for (i = 0; i < len; i++) {
    		if (i2c_read_word (chip, addr + i, &buffer[i])) {
    			printf ("I2C read-word: I/O error\n");
    			i2c_init (CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
    			return 1;
    		}
    	}
    }
    else if (i2cByte)
    {
    	for (i = 0; i < len; i++) {
    		if (i2c_recv_byte (chip, addr + i, &buffer[i])) {
    			printf ("I2C read-word: I/O error\n");
    			i2c_init (CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
    			return 1;
    		}
    	}
    }
    else
    {
    	for (i = 0; i < len; i++) {
    		if (i2c_read_byte (chip, addr + i, &buffer[i])) {
    			printf ("I2C read: I/O error\n");
    			i2c_init (CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
    			return 1;
    		}
    	}
    }
// WNC

    memcpy((uchar*)SHADOW_CPY_BUFFER, buffer, len);

	return 0;
}

// WNC Steve 2013/08/29
// static int i2c_write_byte (u8 devaddr, u8 regoffset, u8 value)
static int i2c_write_byte (u8 devaddr, u8 regoffset, uint value)
// WNC
{
    int rc;
    struct iproc_xact_info info;
    unsigned int num_bytes_write = 0;

    devaddr <<= 1;

    info.cmd_valid = 1;
    info.command = (unsigned char)regoffset;
    info.data = &value; 
    info.size = 1;
    info.flags = 0;
    info.smb_proto = SMBUS_PROT_WR_BYTE;

    /* Refer to i2c_smbus_write_byte params passed. */
    rc = iproc_i2c_data_send(devaddr, &info);

    if (rc < 0) {
        printf("%s: %s error accessing device 0x%X", 
                    __func__, "Write", devaddr);
        return -EREMOTEIO;
    }

    return (0);
}

// WNC Steve 2013/08/23
static int i2c_send_byte (u8 devaddr, u8 regoffset, uint value)
{
    int rc;
    struct iproc_xact_info info;
    unsigned int num_bytes_write = 0;

    devaddr <<= 1;

    info.cmd_valid = 1;
    info.command = (unsigned char)regoffset;
    info.data = &value; 
    info.size = 1;
    info.flags = 0;
    info.smb_proto = SMBUS_PROT_SEND_BYTE;

    /* Refer to i2c_smbus_write_byte params passed. */
    rc = iproc_i2c_data_send(devaddr, &info);

    if (rc < 0) {
        printf("%s: %s error accessing device 0x%X", 
                    __func__, "Write", devaddr);
        return -EREMOTEIO;
    }

    return (0);
}

static int i2c_write_word (u8 devaddr, u8 regoffset, uint value)
{
    int rc;
    struct iproc_xact_info info;
    unsigned int num_bytes_write = 0;
    u8 val[2];

    devaddr <<= 1;

    val[0] = (value&0x0000ff00) >> 8;
    val[1] = (value&0x000000ff);

    info.cmd_valid = 1;
    info.command = (unsigned char)regoffset;
    //info.data = &value; 
    info.data = val; 
    info.size = 2;
    info.flags = 0;
    info.smb_proto = SMBUS_PROT_WR_WORD;

    /* Refer to i2c_smbus_write_byte params passed. */
    rc = iproc_i2c_data_send(devaddr, &info);

    if (rc < 0) {
        printf("%s: %s error accessing device 0x%X", 
                    __func__, "Write", devaddr);
        return -EREMOTEIO;
    }

    return (0);
}
// WNC

// WNC Steve 2013/08/07
//int i2c_write (uchar chip, uint addr, int alen, uchar * buffer, int len)
int i2c_write (uchar chip, uint addr, int alen, uint * buffer, int len)
// WNC
{
	int i;

	if (alen > 1) {
		printf ("I2C read: addr len %d not supported\n", alen);
		return 1;
	}

	if (addr + len > 256) {
		printf ("I2C read: address out of range\n");
		return 1;
	}

// WNC Steve 2013/08/23
    if (i2cWord)
    {
    	for (i = 0; i < len; i++) {
    		if (i2c_write_word (chip, addr + i, &buffer[i])) {
    			printf ("I2C read-word: I/O error\n");
    			i2c_init (CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
    			return 1;
    		}
    	}
    }
    else if (i2cByte)
    {
    	for (i = 0; i < len; i++) {
    		if (i2c_send_byte (chip, addr + i, buffer[i])) {
    			printf ("I2C read-word: I/O error\n");
    			i2c_init (CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
    			return 1;
    		}
    	}
    }
    else
    {
    	for (i = 0; i < len; i++) {
    		if (i2c_write_byte (chip, addr + i, buffer[i])) {
    			printf ("I2C read: I/O error\n");
    			i2c_init (CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
    			return 1;
    		}
    	}
    }
// WNC

	return 0;
}
