/**
 * APM APM86xxx SlimPRO Interface Header
 *
 * Copyright (c) 2010, Applied Micro Circuits Corporation
 * Author: Anup Patel <apatel@apm.com>
 * Author: Prodyut Hazarika <phazarika@apm.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 */
#ifndef __IPP_INTERFACE_H__
#define __IPP_INTERFACE_H__

/** 
 * @name iPP to PPC message structure and message encoding/decoding macros 
 * 
 * @code
 * Bit Definitions in IBM convention
 * 0       4             8          16        24      31  
 * ----------------------------------------------------- 
 * MsgType | Hdlr/SubCmd | CtrlByte | P0/Rsvd | P1/Rsvd |
 * ----------------------------------------------------- 
 * Software Defined message between iPP<->PPC
 * Scratch1 Reg used by PPC1 and Scratch2 Reg by PPC2
 * to send additional 4 bytes to iPP - encoded per cmd  
 * MsgType: Upper Nibble in MSB identifies command type           
 * Hdlr/SubCmd: Lower Nibble in MDB identify subcmd or handler ID 
 * Control Byte: Control Byte - Must not be used for command data 
 * P0: Byte for Command Data - encoded per message type           
 * P1: LSB byte for Command Data - encoded per message type
 * @endcode
 */
/*@{*/
#define IPP_MSG_TYPE_MASK			0xF0000000U
#define IPP_MSG_TYPE_SHIFT			28
#define IPP_MSG_CONTROL_BYTE_MASK		0x00FF0000U
#define IPP_MSG_CONTROL_BYTE_SHIFT		16
#define IPP_DATA_MSG_HNDL_MASK 			0x0F000000U
#define IPP_DATA_MSG_HNDL_SHIFT 		24	
#define IPP_DATA_MSG_CTRL_BYTE_MASK 		0x0000FF00U
#define IPP_DATA_MSG_CTRL_BYTE_SHIFT 		8
#define IPP_DATA_MSG_POS_MASK			0x000000FFU
#define IPP_DATA_MSG_POS_SHIFT			0
#define IPP_ADDR_MSG_HNDL_MASK 			0x0F000000U
#define IPP_ADDR_MSG_HNDL_SHIFT 		24	
#define IPP_ADDR_MSG_UPPER_ADDR_MASK		0x0000000FU
#define IPP_ADDR_MSG_UPPER_ADDR_SHIFT		0
#define IPP_USER_MSG_HNDL_MASK 			0x0F000000U
#define IPP_USER_MSG_HNDL_SHIFT 		24	
#define IPP_USER_MSG_P0_MASK 			0x0000FF00U
#define IPP_USER_MSG_P0_SHIFT 			8
#define IPP_USER_MSG_P1_MASK 			0x000000FFU
#define IPP_USER_MSG_P1_SHIFT 			0
#define IPP_ERROR_MSG_CODE_MASK			0x0000FFFFU
#define IPP_ERROR_MSG_CODE_SHIFT		0	
#define IPP_FWLOAD_MSG_MODE_MASK		0x0F000000U
#define IPP_FWLOAD_MSG_MODE_SHIFT		24
#define IPP_DBGMSG_TYPE_MASK			0x0F000000U
#define IPP_DBGMSG_TYPE_SHIFT			24
#define IPP_DBGMSG_P0_MASK 			0x0000FF00U
#define IPP_DBGMSG_P0_SHIFT 			8
#define IPP_DBGMSG_P1_MASK 			0x000000FFU
#define IPP_DBGMSG_P1_SHIFT 			0
#define IPP_ENCODE_OK_MSG() 			(IPP_OK_MSG << IPP_MSG_TYPE_SHIFT)
#define IPP_ENCODE_PING_MSG() 			(IPP_PING_MSG << IPP_MSG_TYPE_SHIFT)
#define IPP_ENCODE_RESET_MSG() 			(IPP_RESET_MSG << IPP_MSG_TYPE_SHIFT)
#define IPP_ENCODE_DATA_MSG(hndl,cb,dcb,pos)	((IPP_DATA_MSG << IPP_MSG_TYPE_SHIFT) | \
						((hndl << IPP_DATA_MSG_HNDL_SHIFT) & IPP_DATA_MSG_HNDL_MASK) | \
						((cb << IPP_MSG_CONTROL_BYTE_SHIFT) & IPP_MSG_CONTROL_BYTE_MASK) | \
						((dcb << IPP_DATA_MSG_CTRL_BYTE_SHIFT) & IPP_DATA_MSG_CTRL_BYTE_MASK) | \
						((pos << IPP_DATA_MSG_POS_SHIFT) & IPP_DATA_MSG_POS_MASK))
#define IPP_ENCODE_ADDR_MSG(hndl,cb,ua)		((IPP_ADDR_MSG << IPP_MSG_TYPE_SHIFT) | \
						((hndl << IPP_ADDR_MSG_HNDL_SHIFT) & IPP_ADDR_MSG_HNDL_MASK) | \
						((cb << IPP_MSG_CONTROL_BYTE_SHIFT) & IPP_MSG_CONTROL_BYTE_MASK) | \
						((ua << IPP_ADDR_MSG_UPPER_ADDR_SHIFT) & IPP_ADDR_MSG_UPPER_ADDR_MASK))
#define IPP_ENCODE_USER_MSG(hndl,cb,p0,p1)	((IPP_USER_MSG << IPP_MSG_TYPE_SHIFT) | \
						((hndl << IPP_USER_MSG_HNDL_SHIFT) & IPP_USER_MSG_HNDL_MASK) | \
						((cb << IPP_MSG_CONTROL_BYTE_SHIFT) & IPP_MSG_CONTROL_BYTE_MASK) | \
						((p0 << IPP_USER_MSG_P0_SHIFT) & IPP_USER_MSG_P0_MASK) | \
						(p1 & IPP_USER_MSG_P1_MASK))
#define IPP_ENCODE_ERROR_MSG(errcode) 		((IPP_ERROR_MSG << IPP_MSG_TYPE_SHIFT)  | \
						(errcode & IPP_ERROR_MSG_CODE_MASK))
#define IPP_ENCODE_FWLOAD_MSG(fw_mode)		((IPP_FWLOAD_MSG << IPP_MSG_TYPE_SHIFT) | \
						((fw_mode << IPP_FWLOAD_MSG_MODE_SHIFT) & IPP_FWLOAD_MSG_MODE_MASK)) 
#define IPP_ENCODE_DEBUG_MSG(type,cb,p0,p1)	((IPP_DEBUG_MSG << IPP_MSG_TYPE_SHIFT) | \
						((type << IPP_DBGMSG_TYPE_SHIFT) & IPP_DBGMSG_TYPE_MASK) | \
						((cb << IPP_MSG_CONTROL_BYTE_SHIFT) & IPP_MSG_CONTROL_BYTE_MASK) | \
						((p0 << IPP_DBGMSG_P0_SHIFT) & IPP_DBGMSG_P0_MASK) | \
						((p1 << IPP_DBGMSG_P1_SHIFT) & IPP_DBGMSG_P1_MASK))
#define IPP_DECODE_MSG_TYPE(data)		((data & IPP_MSG_TYPE_MASK) >> IPP_MSG_TYPE_SHIFT)
#define IPP_DECODE_MSG_CONTROL_BYTE(cb)		((cb & IPP_MSG_CONTROL_BYTE_MASK) >> IPP_MSG_CONTROL_BYTE_SHIFT) 
#define IPP_DECODE_DATA_MSG_HNDL(data) 		((data & IPP_DATA_MSG_HNDL_MASK) >> IPP_DATA_MSG_HNDL_SHIFT)
#define IPP_DECODE_DATA_MSG_CTRL_BYTE(data) 	((data & IPP_DATA_MSG_CTRL_BYTE_MASK) >> IPP_DATA_MSG_CTRL_BYTE_SHIFT)
#define IPP_DECODE_DATA_MSG_POS(data) 		((data & IPP_DATA_MSG_POS_MASK) >> IPP_DATA_MSG_POS_SHIFT)
#define IPP_DECODE_ADDR_MSG_HNDL(data) 		((data & IPP_ADDR_MSG_HNDL_MASK) >> IPP_ADDR_MSG_HNDL_SHIFT)
#define IPP_DECODE_ADDR_MSG_UPPER_ADDR(data)	((data & IPP_ADDR_MSG_UPPER_ADDR_MASK) >> IPP_ADDR_MSG_UPPER_ADDR_SHIFT)
#define IPP_DECODE_USER_MSG_HNDL(data) 		((data & IPP_USER_MSG_HNDL_MASK) >> IPP_USER_MSG_HNDL_SHIFT)
#define IPP_DECODE_USER_MSG_P0(data) 		((data & IPP_USER_MSG_P0_MASK) >> IPP_USER_MSG_P0_SHIFT)
#define IPP_DECODE_USER_MSG_P1(data) 		(data & IPP_USER_MSG_P1_MASK)
#define IPP_DECODE_ERROR_MSG_CODE(data)		(data & IPP_ERROR_MSG_CODE_MASK)
#define IPP_DECODE_FWLOAD_MSG_MODE(data)	((data & IPP_FWLOAD_MSG_MODE_MASK) >> IPP_FWLOAD_MSG_MODE_SHIFT)
#define IPP_DECODE_DBGMSG_TYPE(data) 		((data & IPP_DBGMSG_TYPE_MASK) >> IPP_DBGMSG_TYPE_SHIFT)
#define IPP_DECODE_DBGMSG_P0(data) 		((data & IPP_DBGMSG_P0_MASK) >> IPP_DBGMSG_P0_SHIFT)
#define IPP_DECODE_DBGMSG_P1(data) 		(data & IPP_DBGMSG_P1_MASK)
#define IPP_MSG_PARAM_UNUSED			0
/*@}*/

/**
 * @name iPP Message Control Byte Definition
 * @code
 * Bit Definitions in IBM convention          
 * 0   1          7  
 * ---------------- 
 * URG | Reserved | 
 * ---------------- 
 * Bit Definitions (S/W Defined)                                             
 * Bit (0) - URG: Urgent msg: Send ACK after processing msg (in ISR context)
 * Bit (1-7) - Reserved for future and customer use                     
 * @endcode
 */
/*@{*/
#define IPP_MSG_CONTROL_URG_BIT		0x80	
#define CTRL_BYTE_UNSED			0
/*@}*/

/**
 * @name iPP Data Message Control Byte Definition (Only used for Data Message)
 * @code
 * Bit Definitions in IBM convention          
 * 0     1      2          3          4            7  
 * ------------------------------------------------ 
 * Start | Stop | Pos/Data | Reserved | Unused/Pos |
 * ------------------------------------------------ 
 * Bit Definitions (S/W Defined)                                             
 * Bit (0) - Start: Flag for start of Data
 * Bit (1) - Stop: Flag for end of Data
 * Bit (2) - Pos/Data: Flag to indicate whether next byte used for position or data
 *           Flag to indicate whether next byte used for position or data
 *           SCRATCH1/2 not available for sending message from iPP to PPC, hence 
 *           last byte is used for sending data in case of iPP to PPC data Msg
 * Bit 3 - Reserved for future and customer use                     
 * Bit 4:7 - Valid only if bit 2 (Pos/Data) is 1 
 *           Indicates position of data that will be sent via next byte                      
 *           Used only for iPP to PPC Data msg, not for PPC to iPP Data msg
 * @endcode
 */
/*@{*/
#define DATA_MSG_CTRL_BYTE_UNSED	0
#define IPP_DATA_MSG_CBYTE_START_BIT	0x80	
#define IPP_DATA_MSG_CBYTE_STOP_BIT	0x40	
#define IPP_DATA_MSG_CBYTE_DATA_BIT	0x20	
#define IPP_DATA_MSG_CBYTE_POS_MASK	0x0F
/*@}*/

/** 
 * @name iPP Global Configuration: Common Settings
 * \n
 * Maximum number of variables is 255. \n
 * One byte parameter can be send in message byte - additional 4 byte 
 * sent using SCRATCH1/2 \n
 * Configured using IPP_CONFIG_HDLR from PPC. The maximum value for iPP 
 * Config Var is 1 byte (255) \n 
 * Config parameters can get additional data (4 bytes) using SCRATCH1 and 
 * SCRATCH2 registers \n
 * Protocol: CPU0 always writes to SCRATCH1 and CPU1 uses SCRATCH2 for 
 * sending additional 4 bytes of data \n
 */
/*@{*/
#define IPP_MAX_CONFIG_VAR	(0xFF)
#define IPP_CONFIG_VAL_MASK	(0xFF)
/*@}*/

/** 
 * @name iPP Global Configuration: Deep Sleep Timer Variable (in secs)
 * \n
 * Sleep Timer tells the time after which iPP cuts of 
 * SoC power if both PPC cores are powered down \n       
 * 0: Disable timer 
 */
/*@{*/
#define IPP_DEEPSLEEP_TIMER_VAR		1
#define IPP_DEEPSLEEP_TIMER_DISABLE	0
/*@}*/

/** 
 * @name iPP Global Configuration: CPM Sleep Control Mode
 * \n 
 *  1:Local 
 *  0:Global 
 */
/*@{*/
#define IPP_SLEEP_CTRL_MODE_VAR		2 
#define IPP_LOCAL_SLEEP_MODE		1
#define IPP_GLOBAL_SLEEP_MODE		0
/*@}*/

/** 
 * @name iPP Global Configuration: QMLite_init Variable
 * \n 
 *  1:Start Init 
 */
/*@{*/
#define IPP_QMLITE_INIT_VAR		3
#define IPP_QMLITE_INIT_START		1
/*@}*/

/** 
 * @name iPP Global Configuration: iPP Wakeup EventMap Bitmask Variable 
 */
/*@{*/
#define IPP_WAKEUP_EVENTMAP_VAR		4
#define IPP_WAKEUP_ETHERNET_EVENT	0x01
#define IPP_WAKEUP_USB_EVENT		0x02
#define IPP_WAKEUP_GPIO_EVENT		0x04
#define IPP_WAKEUP_RTC_EVENT		0x08
#define IPP_WAKEUP_SW_EVENT		0x10
/*@}*/

/** 
 * @name iPP Global Configuration: OCM offset of ipp_pwr_state_t power state structure 
 */
/*@{*/
#define IPP_PWRSTATE_OCM_OFFSET_VAR	5
/*@}*/

/** 
 * @name iPP Global Configuration: DRAM offset of ipp_pwr_state_t power state structure 
 */
/*@{*/
#define IPP_PWRSTATE_IPPDRAM_OFFSET_VAR	6
/*@}*/

/** 
 * @name iPP Global Configuration: Sleep Timer Variable (in secs) 
* \n 
  * Sleep Timer tells the time after which iPP cuts of
 * SoC clocks if both PPC cores are clock gated
 * 0: Disable timer 
 */
/*@{*/
#define IPP_SLEEP_TIMER_VAR		7
#define IPP_SLEEP_TIMER_DISABLE		0	
/*@}*/

/** 
 * @name iPP Global Configuration: Send IPv4 address
 * \n 
 *  Notify IPv4 Address from PPC to iPP
 */
/*@{*/
#define IPP_SEND_IPv4_ADDR_VAR		8	
/*@}*/

/** 
 * @name iPP Global Configuration: Mask of online PPC cores
 */
/*@{*/
#define IPP_ONLINE_PPC_CORES_MASK_VAR    9
/*@}*/

/** 
 * @name iPP Global Configuration: iPP GPIO Config Variable
* \n 
  * Configure iPP GPIO as iPP Input (Wakeup), iPP Output,
  * PPC Input or PPC Output,
  * GPIO Mask is passed using SCRATCH1/2 
 */
/*@{*/
#define IPP_GPIO_CONFIG_VAR		10
#define GPIO_TYPE_WAKEUP_IPP_INPUT	1
#define GPIO_TYPE_IPP_OUTPUT		2	
#define GPIO_TYPE_PPC_INPUT		3
#define GPIO_TYPE_PPC_OUTPUT		4
/*@}*/

/** 
 * @name iPP Global Configuration: iPP Disable GPIO shutdown Variable
* \n 
  * Disable GPIO and LED shutdown during deep sleep 
 */
/*@{*/
#define IPP_DISABLE_GPIO_SHUTDOWN	11
#define DISABLE_GPIO_SHUTDOWN		1
#define ENABLE_GPIO_SHUTDOWN		0
/*@}*/

/** 
 * @name iPP Global Configuration: Wakeuptime LED Mask Variable
* \n 
  * LED to light up to measure deep sleep entry/exit times
 */
/*@{*/
#define IPP_LED_MEASURE_MASK		12
/*@}*/


/* End of iPP Global Configuration Variables */

/** 
 * @name iPP Scratch PAD Register Definitions 
 * \n 
 * ScratchPAD register is used to notify PPC about iPP capabilities
 * It also stores iPP Software major/minor versions, along with bits
 * indicating whether an IP block has been already initialized (eg. QM/DDR)
 * These bits can be used by PPC boot code to skip that IP initialization
 * Definitions in IBM convention.
 * @code
 * 0  4      6        8     9          16                              31 
 * ---------------------------------------------------------------------- 
 * |ID| Mode | Rsvd   |TPIM |PPCstarted| Reserved                       | 
 * ---------------------------------------------------------------------- 
 * Bit Definitions (S/W Defined)                                          
 * Bit (0-3)   - ID: Magic Identifier                                     
 * Bit (4-5)   - Mode: iPP whether in ROM, Runtime or External code       
 * Bit (6-7)   - Rsvd: Reserved for future
 * Bit (8)     - TPIM: Whether iPP is running Secure boot                   
 * Bit (9)     - PPCStarted: Whether iPP has release PPC reset              
 * Bit (10-23) - Reserved: For future use                                 
 * Bit (24-27) - Major Version number                                     
 * Bit (28-31) - Minor Version number                                     
 * @endcode
 */
/*@{*/
/** Scratch ID mask */
#define IPP_SCRATCH_ID_MASK		0xF0000000
/** Scratch ID magic value */
#define IPP_SCRATCH_ID_MAGIC		0xA0000000
/** Scratch ID shift */
#define IPP_SCRATCH_ID_SHIFT		28
/** Mode mask */
#define IPP_MODE_MASK			0x0C000000
/** Mode value ROM Code */
#define IPP_ROM_MODE_MASK		0x04000000
/** Mode value RUNTIME Code */
#define IPP_RUNTIME_MODE_MASK		0x08000000
/** Mode value EXTERNAL Code */
#define IPP_EXTBOOT_MODE_MASK		0x0C000000
/** Mode shift */
#define IPP_MODE_SHIFT			26
/** TPIM mask */
#define IPP_TPIM_ENABLE_MASK		0x00800000
/** TPIM shift */
#define IPP_TPIM_SHIFT			23
/** PPCStarted mask */
#define IPP_PPC_STARTED_MASK		0x00400000
/** PPCStarted shift */
#define IPP_PPC_STARTED_SHIFT		22
/** Major Version mask */
#define IPP_MAJOR_VER_MASK		0x000000f0
/** Major Version shift */
#define IPP_MAJOR_VER_SHIFT		4
/** Minor Version mask */
#define IPP_MINOR_VER_MASK		0x0000000f
/** Minor Version shift */
#define IPP_MINOR_VER_SHIFT		0
/*@}*/

/** 
 * @name Offset of members of ipp_pwr_state_t structure
 * \n 
 * WARNING: If you change order of ipp_pwr_state_t
 * make sure to update the defines for the offsets
 * Assembly code depends on the defines for offsets
 * and bootcode will stop working. BE VERY CAREFUL
 * when changing/adding to ipp_pwr_state_t \n 
 * NOTE: For use in assembly code by PPC bootloader
 */
/*@{*/
/** PowerON flag offset */
#define POWERON_FLAG_OFFSET	0
/** Resume Program Counter offset */
#define RESUME_PC_OFFSET	4
/** TLB Word0 offset */
#define TLB_WORD0_OFFSET	8
/** TLB Word1 offset */
#define TLB_WORD1_OFFSET	12
/** TLB Word2 offset */
#define TLB_WORD2_OFFSET	16
/*@}*/

/** The upper 16 bits of ipp_pwr_state_t.poweron_flag holds magic word if power state structure is valid.
 * The iPP BootCode clears this magic on cold boot. Only iPP writes to poweron_flag, PowerPC only reads it.
 * If structure is invalid, PowerPC resumes normal boot. The Lower 16 bits contain flag to restore state 
 * without doing normal boot
 */
/*@{*/
/** Power State Indentification Magic */
#define IPP_PWR_STATE_MAGIC	(0xA7CC0000)
/** Power State Restore Flag Value */
#define RESTORE_FLAG		(0x1)
/** Power State DDR Magic Values
 *    These magic values are store in DDR at
 *    apm86xxx_resume_deepsleep + 4 + (4*n)
 */
#define IPP_DDR_RESTORE_MAGIC0	(0xA5A5FCFC)
#define IPP_DDR_RESTORE_MAGIC1	(0xAAAA5555)
#define IPP_DDR_RESTORE_MAGIC2	(0xFFFFA5A5)
#define IPP_DDR_RESTORE_MAGIC3	(0x0F0FF0F0)
/*@}*/

/** 
 * Misc Defines
 */
/*@{*/
#define MAX_PPC_CORES		2 	/**< Maximum number of PPC cores */
#define MAX_PPC_CORES_MASK	3 	/**< BitMask for max PPC cores */
#define PPC_CORE0		0 	/**< PPC Core 0 ID */
#define PPC_CORE1		1 	/**< PPC Core 1 ID */
/*@}*/

/**
 * @name IPP_PWRSTATE_IN_XXX
 * @note Location of Shared ipp_pwr_state_t power state structure 
 * 	 ipp_pwr_state_t can be in DSRAM (deep sleep RAM) or in OCM      
 */
/*@{*/
#define IPP_PWRSTATE_IN_OCM		1
#define IPP_PWRSTATE_IN_DSRAM		2
/*@}*/

/** 
 * @name iPP SCU_PPCx_RESUME_DS Register Definition
 * \n 
 * SCU_PPCx_RESUME_DS register used to notify the location for the ipp_pwr_state_t 
 * structure. It can be either in iPP DRAM or OCM. It also has bit to indicate
 * that PPC1 should break out of the spin loop in initial bootup (AMP mode)
 * SCU_PPCx_RESUME_DS is modified by iPP only when iPP gets a message from PPC
 * Some bits in SCU_PPCx_RESUME_DS are set by PPC bootcode (eg. C1Rst bit)
 * Definitions in IBM convention.
 * @code
 * 0      2      3       16             31 
 * --------------------------------------
 * |PSLoc |C1Rst | Rsvd   | OCMPSOffset | 
 * --------------------------------------
 * Bit Definitions (S/W Defined)                                          
 * Bit (0-1)  - PSLoc: Power State Location (OCM or iPP DRAM) - set by iPP
 * Bit (2)  - C1Rst: Release Core1 Reset (used in AMP boot) - set by PPC
 * Bit (3-15) - Rsvd: For future use                                 
 * Bit (16-31) - OCMPSOffset: OCM Power State Offset - set by iPP
 * @endcode
 */
/*@{*/
/** Power State mask */
#define IPP_PWR_STATE_MASK		0xC0000000
/** Power State shift */
#define IPP_PWR_STATE_SHIFT		30
/** Mode value OCM Power State Code */
#define IPP_PWR_STATE_OCM_MODE_MASK	(IPP_PWRSTATE_IN_OCM<<IPP_PWR_STATE_SHIFT)
/** Mode value iPP DRAM Power State Code */
#define IPP_PWR_STATE_DSRAM_MODE_MASK	(IPP_PWRSTATE_IN_DSRAM<<IPP_PWR_STATE_SHIFT)
/** PPC1 Reset release mask - set by PPC bootcode */
#define PPC1_RELEASE_RESET_MASK		0x20000000
/** PPC1 Reset release shift - set by PPC bootcode */
#define PPC1_RELEASE_RESET_SHIFT	29
/*@}*/

/** 
 * @name iPP bootaccess helper defines and macros
 */
/*@{*/
#define IPP_BOOTACCESS_WORD0_DEVICE_MASK	 0xF0000000
#define IPP_BOOTACCESS_WORD0_DEVICE_SHIFT	 28
#define IPP_BOOTACCESS_WORD0_OPERATION_MASK	 0x0F000000
#define IPP_BOOTACCESS_WORD0_OPERATION_SHIFT	 24
#define IPP_BOOTACCESS_WORD0_RESERVED_MASK	 0x000FFFFF
#define IPP_BOOTACCESS_WORD0_RESERVED_SHIFT	 0
#define IPP_ENCODE_BOOTACCESS_WORD0(dev,op)	 (((dev << IPP_BOOTACCESS_WORD0_DEVICE_SHIFT) & IPP_BOOTACCESS_WORD0_DEVICE_MASK) | \
						 ((op << IPP_BOOTACCESS_WORD0_OPERATION_SHIFT) & IPP_BOOTACCESS_WORD0_OPERATION_MASK))
#define IPP_ENCODE_BOOTACCESS_ADDRESS(erpn,addr) (((erpn << 28) & 0xF0000000) | \
						 ((addr >> 4) & 0x0FFFFFFF))
#define IPP_DECODE_BOOTACCESS_DEVICE(word0)	 ((word0 & IPP_BOOTACCESS_WORD0_DEVICE_MASK) >> IPP_BOOTACCESS_WORD0_DEVICE_SHIFT)
#define IPP_DECODE_BOOTACCESS_OPERATION(word0)	 ((word0 & IPP_BOOTACCESS_WORD0_OPERATION_MASK) >> IPP_BOOTACCESS_WORD0_OPERATION_SHIFT)
#define IPP_DECODE_BOOTACCESS_ERPN(data)	 ((data & 0xF0000000) >> 28)
#define IPP_DECODE_BOOTACCESS_ADDRESS(data)	 ((data & 0x0FFFFFFF) << 4)
/** Maximum size of filename to be used by boot access */
#define IPP_BOOTACCESS_MAX_FILENAME_SIZE	 16
/*@}*/

/** 
 * @name iPP NET_DATA helper defines and macros
 */
/*@{*/
#define IPP_NETDATA_CTRL_BYTE0_MASK			0xFF000000
#define IPP_NETDATA_CTRL_BYTE0_SHIFT			24
#define IPP_NETDATA_CTRL_BYTE1_MASK			0x00FF0000
#define IPP_NETDATA_CTRL_BYTE1_SHIFT			16
#define IPP_NETDATA_CTRL_BYTE2_MASK			0x0000FF00
#define IPP_NETDATA_CTRL_BYTE2_SHIFT			8		
#define IPP_NETDATA_CTRL_BYTE3_MASK			0x000000FF
#define IPP_ENCODE_NETDATA_CTRL_WORD(b0,b1,b2,b3)	(((b0 << IPP_NETDATA_CTRL_BYTE0_SHIFT) & IPP_NETDATA_CTRL_BYTE0_MASK) | \
							((b1 << IPP_NETDATA_CTRL_BYTE1_SHIFT) & IPP_NETDATA_CTRL_BYTE1_MASK) | \
							((b2 << IPP_NETDATA_CTRL_BYTE2_SHIFT) & IPP_NETDATA_CTRL_BYTE2_MASK) | \
							(b3 & IPP_NETDATA_CTRL_BYTE3_MASK))
#define IPP_DECODE_NETDATA_CTRL_BYTE0(word)		((word & IPP_NETDATA_CTRL_BYTE0_MASK) >> IPP_NETDATA_CTRL_BYTE0_SHIFT)
#define IPP_DECODE_NETDATA_CTRL_BYTE1(word)		((word & IPP_NETDATA_CTRL_BYTE1_MASK) >> IPP_NETDATA_CTRL_BYTE1_SHIFT)
#define IPP_DECODE_NETDATA_CTRL_BYTE2(word)		((word & IPP_NETDATA_CTRL_BYTE2_MASK) >> IPP_NETDATA_CTRL_BYTE2_SHIFT)
#define IPP_DECODE_NETDATA_CTRL_BYTE3(word)		((word & IPP_NETDATA_CTRL_BYTE3_MASK) >> IPP_NETDATA_CTRL_BYTE3_SHIFT)

#define IPP_NETDATA_CTRL_UNUSED				0

/* IPP_NETDATA Control Byte 1 Types */
#define IPP_NETDATA_IPv6_TYPE				1
#define IPP_NETDATA_SNMP_TYPE				2
#define IPP_NETDATA_NETBIOS_TYPE			3
#define IPP_NETDATA_LLMNR_TYPE				4
#define IPP_NETDATA_OFFLOAD_TYPE			5

/* IPP_NETDATA Control Byte 2 Types */

/* for IPv6_TYPE */
#define IPP_IPv6_UCAST					1
#define IPP_IPv6_MCAST					2

/* for SNMP_TYPE */
#define IPP_SNMP_COMMUNITY				1
#define IPP_SNMP_MIB_DEVICE_STATUS			2
#define IPP_SNMP_MIB_PRINTER_ERROR_DETECT  		3
#define IPP_SNMP_MIB_MEM_SIZE				4
#define IPP_SNMP_MIB_STORAGE_TYPE			5
#define IPP_SNMP_MIB_MIB_MEDIA_PATH_TYPE		6
#define IPP_SNMP_MIB_PRINTER_STATUS			7

/* for NETBIOS_TYPE */
#define IPP_NETBIOS_TTL  				1
#define IPP_NETBIOS_NAME 				2
#define IPP_NETBIOS_NODE_TYPE 				3
#define IPP_NETBIOS_NODE_STATUS 			3

/* For LLMNR_TYPE */
#define IPP_LLMNR_TTL					1
#define IPP_LLMNR_NAME					2

/* For Net Offload */
#define IPP_NETOFFLOAD_ENABLE				1
#define IPP_NETOFFLOAD_DISABLE				2
#define IPP_NETOFFLOAD_IP4ADDR				3
#define IPP_NETOFFLOAD_INGRESS_WQPB			4
#define IPP_NETOFFLOAD_INGRESS_WQ			5
#define IPP_NETOFFLOAD_EGRESS_WQ			6

/* All of the below type uses ADDR MSG and share the types
 * from DATA MSG. 
 * IPP_NETDATA Control Byte 3 Types Carries Length for 
 * IPP_SNMP_COMMUNITY,
 * IPP_SNMP_MIB_STORAGE_TYPE,
 * IPP_NETBIOS_NAME
 * IPP_NETBIOS_NODE_STATUS
 * IPP_LLMNR_NAME
*/

/*@}*/

#ifndef __ASSEMBLY__

/** 
 * Power State Structure uses for passing power-save/restore information 
 * between PPC and iPP. SCU_PPCx_RESUME_DS reg points to Power state location
 * Don't use enums in this structure since the compiler doesn't assign 
 * word by default and it messes up. \n
 * WARNING: When you CHANGE any FIELD or order please UPDATE macros used by 
 * PPC bootloader above.             
 */
struct ipp_pwr_state {
	unsigned int poweron_flag; /**< PowerON flag */
	unsigned int resume_pc; /**< Resume Program Counter */
	unsigned int tlb_word0; /**< TLB Word0 for PowerPC */
	unsigned int tlb_word1; /**< TLB Word1 for PowerPC */
	unsigned int tlb_word2; /**< TLB Word2 for PowerPC */
};

typedef struct ipp_pwr_state ipp_pwr_state_t;

/*
** iPP Common Interface
*/

/** iPP error codes */
enum ipp_error_codes {
	IPP_OK=0, 		/**< OK (=No Error) */
	IPP_ERR_FAIL, 		/**< Generic Failure */
	IPP_ERR_INVALID, 	/**< Invalid Operation */
	IPP_ERR_NOTAVAIL, 	/**< Not Available */
	IPP_ERR_NOSPACE, 	/**< No Space */
	IPP_ERR_BUSY, 		/**< Busy */
	IPP_ERR_BOOT_INVALIMG, 	/**< Invalid Image */
	IPP_ERR_SD_UNPLUGGED, 	/**< SDIO card Unplugged */
	IPP_ERR_SDHW, 		/**< SDIO hardware error */
	IPP_ERR_NO_I2C_DEV, 	/**< No I2C device found */
	IPP_ERR_I2C_TIMEOUT, 	/**< I2C timedout */
	IPP_ERR_NO_SPI_DEV, 	/**< No SPI device found */
	IPP_ERR_OCM_MEM_NOT_RDY, /**< OCM not ready */
	IPP_ERR_HW_INIT_DONE, 	 /**< Hardware init already done */
	IPP_ERR_UNSUPPORTED_MSG, /**< Unsupported message */
	IPP_ERR_UNSUPPORTED_CMD, /**< Unsupported command */
	IPP_ERR_INVALID_PARAM,	 /**< Invalid parameter */
	IPP_ERR_INVALID_AXI_ADDR,	/**< Invalid AXI Address */
	IPP_ERR_CSR_MEM_NOT_RDY,	/**< CSR Memory Not ready */
	IPP_SELF_REFRESH_FAIL,	 /**< Self refresh mode fail */
	IPP_CPM_READ_FAIL,	 /**< Reading CPM register failed */
	IPP_CPM_WRITE_FAIL,	 /**< Writing to CPM reigster failed */
};

/* 
 * iPP Messaging Interface 
 */

/** iPP message types */
enum ipp_message_type {
	IPP_DEBUG_MSG=0x0, 	/**< Debug Message */
	IPP_OK_MSG=0x1, 	/**< OK Message */
	IPP_PING_MSG=0x2, 	/**< Ping Message */
	IPP_RESET_MSG=0x3, 	/**< Reset Message */
	IPP_DATA_MSG=0x4, 	/**< Data Message */
	IPP_ADDR_MSG=0x5, 	/**< Address Message */
	IPP_USER_MSG=0x6, 	/**< User Message */
	IPP_ERROR_MSG=0x7, 	/**< Error Message */
	IPP_FWLOAD_MSG=0x8, 	/**< Firmware Load Message */
	IPP_MAX_MSG_NUM=0xF
};

/** iPP firmware load modes */
enum ipp_firmware_modes {
	IPP_FWLOAD_FROM_PPC=0x1, /**< iPP firmware loaded by PPC */
	IPP_FWLOAD_FROM_I2C=0x2  /**< iPP firmware loaded from I2C */
};

/** iPP user message handlers */
enum ipp_user_message_handlers {
	IPP_USER_MSG_HDLR_UNUSED=0, /**< User Msg Handler Unused */
	IPP_PWRMGMT_HDLR=1, /**< Power Mode Handler */
	IPP_CONFIG_SET_HDLR=2, /**< iPP Configuration Set Handler */
	IPP_MAX_USER_MSG_HANDLERS=0xf
};

/** iPP data message handlers */
enum ipp_data_message_handlers {
	IPP_DATA_MSG_HDLR_UNUSED=0, /**< Data Msg Handler Unused */
	IPP_BOOTACCESS_HDLR=1, /**< Data Msg handler Boot RAW interface */
	IPP_NETDATA_HDLR=2, /**< Data Msg handler for NETDATA msg */
	IPP_MAX_DATA_MSG_HANDLERS=0xf
};

/** iPP address message handlers */
enum ipp_addr_message_handlers {
	IPP_ADDR_MSG_HDLR_UNUSED=0, /**< Addr Msg Handler Unused */
	IPP_BOOTACCESS_ADDR_MSG_HDLR=1, /**< Address Msg handler Boot RAW interface */
	IPP_NETDATA_ADDR_MSG_HDLR=2, /**< NetData Addr Msg handler for NETDATA msg */
	IPP_MAX_ADDR_MSG_HANDLERS=0xf
};

/** iPP debug message subtype */
enum ipp_debug_msg_type {
	IPP_DBG_SUBTYPE_UNUSED=0, /**< Debug subtype Unused */
	IPP_DBG_SUBTYPE_CPMREAD=1, /**< CPM Reg read */
	IPP_DBG_SUBTYPE_CPMWRITE=2, /**< CPM Reg write */
	IPP_DBG_SUBTYPE_CPMREAD_RESP=3, /**< CPM Reg read response */
	IPP_DBG_SUBTYPE_MAX=0xf
};

/* 
** iPP Boot Access Interface 
*/

/** iPP bootaccess devices */
enum ipp_bootaccess_devices {
	IPP_BOOTACCESS_DEVICE_UNUSED=0, /**< Unused bootaccess device */
	IPP_BOOTACCESS_DEVICE_DEFAULT=1, /**< Default bootaccess device */
	IPP_BOOTACCESS_DEVICE_SDIO=2, /** SDIO bootaccess device */
	IPP_BOOTACCESS_DEVICE_USB=3, /** USB bootaccess device */
	IPP_BOOTACCESS_DEVICE_SPI=4, /** SPI bootaccess device */
	IPP_BOOTACCESS_DEVICE_SATA=5, /** SATA bootaccess device */
	IPP_MAX_BOOTACCESS_DEVICE=0xf
};

/** iPP bootacess operations */
enum ipp_bootaccess_operations {
	IPP_BOOTACCESS_OP_UNUSED=0, /**< Unused bootraw read operation */
	IPP_BOOTACCESS_OP_READ=1, /**< Bootaccess read operation */
	IPP_BOOTACCESS_OP_READFULL=2, /**< Bootaccess read full operation */
	IPP_BOOTACCESS_OP_WRITE=3, /**< Bootaccess write operation */
	IPP_MAX_BOOTACCESS_OP=0xf
};

/* 
** iPP Power Management Interface 
*/

/** iPP POWER Management Commands */
enum ipp_pwrmgmt_cmd {
	IPP_PWRMGMT_CMD_CPU_SET_NORMAL=0, /**< Restore CPU clock */
	IPP_PWRMGMT_CMD_CPU_CLKGATE, 	/**< Clock Gate CPU */
	IPP_PWRMGMT_CMD_CPU_POWEROFF, 	/**< Power off CPU */
	IPP_PWRMGMT_CMD_CPU_FREQ_SCALE, /**< Scale up/down CPU freq */
	IPP_PWRMGMT_CMD_PLB_FREQ_SCALE, /**< Scale up/down PLB freq */
	IPP_PWRMGMT_CMD_VOLTAGE_SCALE, 	/**< Scale up/down Voltage */
	IPP_PWRMGMT_CMD_ISCALE, 	/**< Scale internal voltage */
	IPP_PWRMGMT_CMD_CPU_DBGPWROFF, 	/**< Debug power off CPU */
	IPP_PWRMGMT_CMD_MAX
};

enum ipp_gpio {
	IPP_GPIO_0=0, 	/**< iPP GPIO 0 */
	IPP_GPIO_1, 	/**< iPP GPIO 1 */
	IPP_GPIO_2, 	/**< iPP GPIO 2 */
	IPP_GPIO_3, 	/**< iPP GPIO 3 */
	IPP_GPIO_4, 	/**< iPP GPIO 4 */
	IPP_GPIO_5, 	/**< iPP GPIO 5 */
	IPP_GPIO_6, 	/**< iPP GPIO 6 */
	IPP_GPIO_7, 	/**< iPP GPIO 7 */
	IPP_GPIO_8, 	/**< iPP GPIO 8 */
	IPP_GPIO_9, 	/**< iPP GPIO 9 */
	IPP_GPIO_10, 	/**< iPP GPIO 10 */
	IPP_GPIO_11, 	/**< iPP GPIO 11 */
	IPP_GPIO_12, 	/**< iPP GPIO 12 */
	IPP_GPIO_13, 	/**< iPP GPIO 13 */
	IPP_GPIO_14, 	/**< iPP GPIO 14 */
	IPP_GPIO_15, 	/**< iPP GPIO 15 */
	IPP_MAX_GPI0
};

#endif /* __ASSEMBLY__ */

/** 
 * Voltage Scaling Sub commands
 * Voltage Scaling can be done using DEFAULT_SUBCMD or I2C_SUBCMD
 * For DEFAULT_SUBCMD, iPP just drives SCU_PS_CTL[2] output pin 
 * For I2C_SUBCMD, PPC uses SCRATCH1/2 register to pass additional params
 */
/*@{*/
#define IPP_VOLTAGE_SCALE_DEFAULT_SUBCMD	1
#define IPP_VOLTAGE_SCALE_I2C_SUBCMD		2

/** 
 * Internal Scaling Sub commands
 */
/*@{*/
#define IPP_ISCALE_DISABLE		0	
#define IPP_ISCALE_ENABLE		1
/*@}*/

#endif /* __IPP_IFACE_H__ */
