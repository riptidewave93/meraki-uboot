/*
 * (C) Copyright 2000
 * Rob Taylor, Flying Pig Systems. robt@flyingpig.com.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
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
 */

#include <common.h>
#include <linux/compiler.h>
#include <post.h>

#include <ns16550.h>
#ifdef CONFIG_NS87308
#include <ns87308.h>
#endif

#if defined (CONFIG_SERIAL_MULTI)
#include <serial.h>
#endif

#include <asm/arch/iproc.h>

DECLARE_GLOBAL_DATA_PTR;

/* See comment above early_access_env_vars() about the value */
#define EARLY_ACC_BUF_SIZE 512

#if (defined(CONFIG_NS_PLUS))
u32 com2Configured = 0;
u32 com3Configured = 0;
#endif
#if !defined(CONFIG_CONS_INDEX)
#if defined (CONFIG_SERIAL_MULTI)
/*   with CONFIG_SERIAL_MULTI we might have no console
 *  on these devices
 */
#else
#error	"No console index specified."
#endif /* CONFIG_SERIAL_MULTI */
#elif (CONFIG_CONS_INDEX < 1) || (CONFIG_CONS_INDEX > 4)
#error	"Invalid console index value."
#endif

#if CONFIG_CONS_INDEX == 1 && !defined(CONFIG_SYS_NS16550_COM1)
#error	"Console port 1 defined but not configured."
#elif CONFIG_CONS_INDEX == 2 && !defined(CONFIG_SYS_NS16550_COM2)
#error	"Console port 2 defined but not configured."
#elif CONFIG_CONS_INDEX == 3 && !defined(CONFIG_SYS_NS16550_COM3)
#error	"Console port 3 defined but not configured."
#elif CONFIG_CONS_INDEX == 4 && !defined(CONFIG_SYS_NS16550_COM4)
#error	"Console port 4 defined but not configured."
#endif

/* Note: The port number specified in the functions is 1 based.
 *	 the array is 0 based.
 */
static NS16550_t serial_ports[4] = {
#ifdef CONFIG_SYS_NS16550_COM1
	(NS16550_t)CONFIG_SYS_NS16550_COM1,
#else
	NULL,
#endif
#ifdef CONFIG_SYS_NS16550_COM2
	(NS16550_t)CONFIG_SYS_NS16550_COM2,
#else
	NULL,
#endif
#ifdef CONFIG_SYS_NS16550_COM3
	(NS16550_t)CONFIG_SYS_NS16550_COM3,
#else
	NULL,
#endif
#ifdef CONFIG_SYS_NS16550_COM4
	(NS16550_t)CONFIG_SYS_NS16550_COM4
#else
	NULL
#endif
};

#define PORT	serial_ports[port-1]

#if defined(CONFIG_SERIAL_MULTI)

/* Multi serial device functions */
#define DECLARE_ESERIAL_FUNCTIONS(port) \
    int  eserial##port##_init (void) {\
	int clock_divisor; \
	clock_divisor = calc_divisor(serial_ports[port-1]); \
	NS16550_init(serial_ports[port-1], clock_divisor); \
	return(0);}\
    void eserial##port##_setbrg (void) {\
	serial_setbrg_dev(port);}\
    int  eserial##port##_getc (void) {\
	return serial_getc_dev(port);}\
    int  eserial##port##_tstc (void) {\
	return serial_tstc_dev(port);}\
    void eserial##port##_putc (const char c) {\
	serial_putc_dev(port, c);}\
    void eserial##port##_puts (const char *s) {\
	serial_puts_dev(port, s);}

/* Serial device descriptor */
#define INIT_ESERIAL_STRUCTURE(port, name) {\
	name,\
	eserial##port##_init,\
	NULL,\
	eserial##port##_setbrg,\
	eserial##port##_getc,\
	eserial##port##_tstc,\
	eserial##port##_putc,\
	eserial##port##_puts, }

#endif /* CONFIG_SERIAL_MULTI */

static int calc_divisor (NS16550_t port)
{
#ifdef CONFIG_OMAP1510
	/* If can't cleanly clock 115200 set div to 1 */
	if ((CONFIG_SYS_NS16550_CLK == 12000000) && (gd->baudrate == 115200)) {
		port->osc_12m_sel = OSC_12M_SEL;	/* enable 6.5 * divisor */
		return (1);				/* return 1 for base divisor */
	}
	port->osc_12m_sel = 0;			/* clear if previsouly set */
#endif
#ifdef CONFIG_OMAP1610
	/* If can't cleanly clock 115200 set div to 1 */
	if ((CONFIG_SYS_NS16550_CLK == 48000000) && (gd->baudrate == 115200)) {
		return (26);		/* return 26 for base divisor */
	}
#endif

#ifdef CONFIG_APTIX
#define MODE_X_DIV 13
#else
#define MODE_X_DIV 16
#endif

	/* Compute divisor value. Normally, we should simply return:
	 *   CONFIG_SYS_NS16550_CLK) / MODE_X_DIV / gd->baudrate
	 * but we need to round that value by adding 0.5.
	 * Rounding is especially important at high baud rates.
	 */
	return (CONFIG_SYS_NS16550_CLK + (gd->baudrate * (MODE_X_DIV / 2))) /
		(MODE_X_DIV * gd->baudrate);
}

#if !defined(CONFIG_SERIAL_MULTI)
int serial_init (void)
{
	int clock_divisor;


#ifdef CONFIG_NS87308
	initialise_ns87308();
#endif

#ifdef CONFIG_SYS_NS16550_COM1
	clock_divisor = calc_divisor(serial_ports[0]);
	NS16550_init(serial_ports[0], clock_divisor);
#endif
#ifdef CONFIG_SYS_NS16550_COM2


#if (!defined(CONFIG_NS_PLUS))

	//Make sure GPIO is turned off swang
	clock_divisor = calc_divisor(serial_ports[1]);
	NS16550_init(serial_ports[1], clock_divisor);

#endif
#endif
#ifdef CONFIG_SYS_NS16550_COM3


#ifdef CONFIG_NS_PLUS
	//Bring it down to 19200 since the ref
	//clock is at 25Mhz and this is the highest it can go
	//clock_divisor = 162;
	//clock_divisor = 81;
        //Clock rate of 115.2k is supported now
	clock_divisor = 14;
#endif

#ifdef CONFIG_NORTHSTAR
	//This is not how we do it
	//Make sure GPIO is turned off swang
	//clock_divisor = calc_divisor(serial_ports[2]);
	clock_divisor = 68;

	//clock_divisor = 406????;
	//UART clk is 125Mhz APB Clk
	// 115200 = 125Mhz / ( 16 * clock_divisor )
#endif
#if (!defined(CONFIG_NS_PLUS))

	NS16550_init_uart2((NS16550_UART2_t)serial_ports[2], clock_divisor);
#endif
#endif

#ifdef CONFIG_SYS_NS16550_COM4
	clock_divisor = calc_divisor(serial_ports[3]);
	NS16550_init(serial_ports[3], clock_divisor);
#endif

	return (0);
}
#endif
#ifdef IPROC_BOARD_DIAGS
//Initialize the Console 3, UART 2 to
//Communicate to the console for the 8051
//which controls the strap pin setting
int serial_init_com8051(void)
{
    int clock_divisor = 81;

    //UART clk is 125Mhz APB Clk
    // 19200 = 125Mhz / ( 16 * clock_divisor )
    printf("\n ===> Configure COM2 to 19200 Baud ( 81 ) <=====\n");
    NS16550_init_uart2((NS16550_UART2_t)serial_ports[2], clock_divisor);
}
#endif

#if (defined(CONFIG_NS_PLUS))
int serial_init_nsp (void)
{
	int clock_divisor;
#ifdef CONFIG_SYS_NS16550_COM2
	//Com2
	//Make sure GPIO is turned off swang
	clock_divisor = calc_divisor(serial_ports[1]);
	NS16550_init(serial_ports[1], clock_divisor);
	com2Configured = 1;
#endif

#ifdef CONFIG_SYS_NS16550_COM3
	clock_divisor = 14;
	//Com3
	NS16550_init_uart2((NS16550_UART2_t)serial_ports[2], clock_divisor);
	com3Configured = 1;
#endif

	return 0;
}
#endif

void
_serial_putc(const char c,const int port)
{
#ifdef IPROC_BOARD_DIAGS
	if ( port < CONFIG_UART2_INDEX )
	{
		if (c == '\n')
			NS16550_putc(PORT, '\r');
		NS16550_putc(PORT, c);
	}
	else if ( CONFIG_UART2_INDEX == port )
	{
		if (c == '\n')
			NS16550_putc_uart2((NS16550_UART2_t)PORT, '\r');

		NS16550_putc_uart2((NS16550_UART2_t)PORT, c);

	}
#else
        if (c == '\n')
                NS16550_putc(PORT, '\r');

        NS16550_putc(PORT, c);
#endif
}

void
_serial_putc_raw(const char c,const int port)
{
#ifdef IPROC_BOARD_DIAGS
	if ( port < CONFIG_UART2_INDEX )
	{
		NS16550_putc(PORT, c);
	}
	else if ( CONFIG_UART2_INDEX == port )
	{
		NS16550_putc_uart2((NS16550_UART2_t)PORT, c);
	}
#else
        NS16550_putc(PORT, c);
#endif
}

void
_serial_puts (const char *s,const int port)
{
	while (*s) {
		_serial_putc (*s++,port);
	}
}


int
_serial_getc(const int port)
{
#ifdef IPROC_BOARD_DIAGS
	if ( port < CONFIG_UART2_INDEX )
	{
		return NS16550_getc(PORT);
	}
	else
	{
		return NS16550_getc_uart2((NS16550_UART2_t)PORT);
	}
#else
        return NS16550_getc(PORT);
#endif
}

int
_serial_tstc(const int port)
{
#ifdef IPROC_BOARD_DIAGS
	if ( port < 3 )
	{
		return NS16550_tstc(PORT);
	}
	else
	{
		return NS16550_tstc_uart2((NS16550_UART2_t)PORT);
	}
#else
        return NS16550_tstc(PORT);
#endif
}
#ifdef IPROC_BOARD_DIAGS
void
_serial_enter_loopback( const int port )
{
	if ( port < CONFIG_UART2_INDEX )
	{
		return NS16550_enterLoopback(PORT);
	}
	else
	{
		return NS16550_enterLoopback_uart2((NS16550_UART2_t)PORT);
	}
}
void
_serial_exit_loopback( const int port )
{
	if ( port < CONFIG_UART2_INDEX )
	{
		return NS16550_exitLoopback(PORT);
	}
	else
	{
		return NS16550_exitLoopback_uart2((NS16550_UART2_t)PORT);
	}
}
#endif

void
_serial_setbrg (const int port)
{
	int clock_divisor;
#ifdef IPROC_BOARD_DIAGS
	if ( port < CONFIG_UART2_INDEX )
	{
		clock_divisor = calc_divisor(PORT);
		NS16550_reinit(PORT, clock_divisor);
	}
	else if ( CONFIG_UART2_INDEX == port)
	{
		clock_divisor = 68;
		NS16550_reinit_uart2((NS16550_UART2_t)PORT, clock_divisor);
	}
#else
        clock_divisor = calc_divisor(PORT);
        NS16550_reinit(PORT, clock_divisor);
#endif
}

#if defined(CONFIG_SERIAL_MULTI)
static inline void
serial_putc_dev(unsigned int dev_index,const char c)
{
	_serial_putc(c,dev_index);
}
#else
void
serial_putc(const char c)
{
	_serial_putc(c,CONFIG_CONS_INDEX);
}
#endif

#if defined(CONFIG_SERIAL_MULTI)
static inline void
serial_putc_raw_dev(unsigned int dev_index,const char c)
{
	_serial_putc_raw(c,dev_index);
}
#else
void
serial_putc_raw(const char c)
{
	_serial_putc_raw(c,CONFIG_CONS_INDEX);
}
#endif

#if defined(CONFIG_SERIAL_MULTI)
static inline void
serial_puts_dev(unsigned int dev_index,const char *s)
{
	_serial_puts(s,dev_index);
}
#else
void
serial_puts(const char *s)
{
	_serial_puts(s,CONFIG_CONS_INDEX);
}
#endif

#if defined(CONFIG_SERIAL_MULTI)
static inline int
serial_getc_dev(unsigned int dev_index)
{
	return _serial_getc(dev_index);
}
#else
int
serial_getc(void)
{
	return _serial_getc(CONFIG_CONS_INDEX);
}
#endif

#if defined(CONFIG_SERIAL_MULTI)
static inline int
serial_tstc_dev(unsigned int dev_index)
{
	return _serial_tstc(dev_index);
}
#else
int
serial_tstc(void)
{
	return _serial_tstc(CONFIG_CONS_INDEX);
}
#endif

#if defined(CONFIG_SERIAL_MULTI)
static inline void
serial_setbrg_dev(unsigned int dev_index)
{
	_serial_setbrg(dev_index);
}
#else
void
serial_setbrg(void)
{
	_serial_setbrg(CONFIG_CONS_INDEX);
}
#endif

#if defined(CONFIG_SERIAL_MULTI)

DECLARE_ESERIAL_FUNCTIONS(1);
struct serial_device eserial1_device =
	INIT_ESERIAL_STRUCTURE(1, "eserial0");
DECLARE_ESERIAL_FUNCTIONS(2);
struct serial_device eserial2_device =
	INIT_ESERIAL_STRUCTURE(2, "eserial1");
DECLARE_ESERIAL_FUNCTIONS(3);
struct serial_device eserial3_device =
	INIT_ESERIAL_STRUCTURE(3, "eserial2");
DECLARE_ESERIAL_FUNCTIONS(4);
struct serial_device eserial4_device =
	INIT_ESERIAL_STRUCTURE(4, "eserial3");

__weak struct serial_device *default_serial_console(void)
{
#if CONFIG_CONS_INDEX == 1
	return &eserial1_device;
#elif CONFIG_CONS_INDEX == 2
	return &eserial2_device;
#elif CONFIG_CONS_INDEX == 3
	return &eserial3_device;
#elif CONFIG_CONS_INDEX == 4
	return &eserial4_device;
#else
#error "Bad CONFIG_CONS_INDEX."
#endif
}
#endif /* CONFIG_SERIAL_MULTI */

/* Find sub string 't' in string 's'. If found, return index within 's' pointing
 * to the last matching character. If not found, return -1
 */
int find_str_indx(char *s, size_t len, char *t)
{
    int i, j;
    
    i = j = 0;

    do {

        j = 0;

        while (s[i] == t[j]) {
            i++;
            j++;
        }

        if (t[j] == '\0') {
            /* var found */
            return(i - 1);
        } 

        i++;
    } while (i < len); /* (s[i] != '\0'); */

    return -1;
}

/*
 * Note about EARLY_ACC_BUF_SIZE used in this function. Since this function
 * is called very early in the init sequence, the amount of stack is limited,
 * so we limit the amount of data read from flash to EARLY_ACC_BUF_SIZE, that
 * is, we do not read the entire sector from flash (on helix4 its 64K, as per
 * CONFIG_ENV_SIZE). Since this function is written to get baudrate, which is
 * always towards beginning of uboot env list, reading 1st 1K (or even less
 * amount) is sufficient. If this function needs to be used to read some other
 * env var, the 'env_ptr' can be adjusted by doing some approximation, depending
 * on the name of env var (uboot stores env vars in sorted order), so that
 * a large buffer size need not be allocated simply to find a env var
 */
int early_access_env_vars(void)
{
    volatile uint8_t *env_ptr;
    int i;
    char buf[EARLY_ACC_BUF_SIZE];
    unsigned long baud;
    unsigned long bauds[] = CONFIG_SYS_BAUDRATE_TABLE;
    int baud_arr_size = sizeof(bauds) / sizeof(bauds[0]);


    /* Note CONFIG_ENV_OFFSET is different on nsp(0xa0000) and hx4(0xc0000) */
#if defined(CONFIG_ENV_IS_IN_NAND) && CONFIG_ENV_IS_IN_NAND
    env_ptr = (volatile uint8_t *)(IPROC_NAND_MEM_BASE + CONFIG_ENV_OFFSET);
#elif defined (CONFIG_ENV_IS_IN_FLASH) 
    env_ptr = (volatile uint8_t *)(IPROC_NOR_MEM_BASE + CONFIG_ENV_OFFSET);
#else
    env_ptr = (volatile uint8_t *)(IPROC_QSPI_MEM_BASE + CONFIG_ENV_OFFSET);
#endif

    for (i = 0; i < EARLY_ACC_BUF_SIZE; i++) {
        buf[i] = *(env_ptr + i);
    }

    baud = 0;
    i = find_str_indx(buf, sizeof(buf),  "baudrate=");
    if (i != -1) {
        /* Convert to integer value */
        while ((buf[i + 1] >= '0') && (buf[i + 1] <= '9')) {

            baud = baud * 10 + (buf[i + 1] - '0');
            i++;
        }

        /* Verify that the baudrate is a valid and a supported value */
        for (i = 0; i < baud_arr_size; i++) {
            if (baud == bauds[i]) {
                break;
            }
        }

        if (i < baud_arr_size) {
            /* Baud rate value is valid */
            gd->baudrate = baud;
        } else {
            return -1;
        }

    } else {

        return -1;
    }

	return 0;
}
