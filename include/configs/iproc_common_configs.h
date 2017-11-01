#ifndef _IPROC_COMMON_H_
#define _IPROC_COMMON_H_

/* Serial Info */
#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_CONS_INDEX					1
#if defined(CONFIG_HURRICANE2) && !defined(CONFIG_HURRICANE2_EMULATION)
#define CONFIG_SYS_NS16550_COM1			(0x18000400) /* CCA UART 1 */
#else
#define CONFIG_SYS_NS16550_COM1			(0x18000300) /* CCA UART 0 */
#endif
#define CONFIG_SYS_NS16550_REG_SIZE		(1)	/* no padding */
#define CONFIG_SYS_NS16550_CLK			iproc_get_uart_clk(0)
#if defined(CONFIG_HURRICANE2_EMULATION) || defined(CONFIG_KATANA2_EMULATION)
#define CONFIG_BAUDRATE					4800
#else
#define CONFIG_BAUDRATE					115200
#endif
#define CONFIG_SYS_BAUDRATE_TABLE	{9600, 19200, 38400, 57600, 115200} 

#ifdef IPROC_BOARD_DIAGS
#define CONFIG_SYS_HZ       1000*1000
#else
#define CONFIG_SYS_HZ       1000
#endif

#define CONFIG_STANDALONE_LOAD_ADDR 0x61000000

#define CONFIG_BOOTFILE         "uImage"
#define CONFIG_MACH_TYPE        4735
#define CONFIG_BOOTARGS     				"console=ttyS0,115200n8 maxcpus=1 mem=496M"

#define CONFIG_BOOTDELAY			3	/* User can hit a key to abort kernel boot and stay in uboot cmdline */
#define CONFIG_BOOTCOMMAND 				""	/* UBoot command issued on power up */
#define CONFIG_SYS_PROMPT					"u-boot> "  

#define CONFIG_CMD_SAVEENV

#define CONFIG_INITRD_TAG        1       /*  send initrd params           */
#define CONFIG_CMD_CONSOLE
#define CONFIG_CMD_NET
#define CONFIG_CMD_PING
#define CONFIG_CMD_MEMORY
#define CONFIG_CMD_RUN
#define CONFIG_CMD_MTDPARTS
#define CONFIG_MTD_DEVICE
#define MTDIDS_DEFAULT      "nand0=nand_iproc.0"
#define MTDPARTS_DEFAULT    "mtdparts=mtdparts=nand_iproc.0:1024k(nboot),1024k(nenv),8192k(nsystem),1038336k(ndata)"
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_BOOT
#define CONFIG_CMD_LICENSE
#define CONFIG_CMD_ECHO
#define CONFIG_CMD_ITEST
#define CONFIG_CMD_MISC
#define CONFIG_CMD_SOURCE

#endif /* _IPROC_COMMON_H_ */
