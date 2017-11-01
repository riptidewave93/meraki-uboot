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
#include <post.h>

#if CONFIG_POST & CONFIG_SYS_POST_UART

static int UART_post_testLoopback(int uartIndex )
{
	char testChar[]= "Hello";
	volatile char *p;
	char t;
	int ret =0;


	p = testChar;
	_serial_enter_loopback(uartIndex);

	while(*p)
	{
		_serial_putc_raw(*p,uartIndex);
		t = _serial_getc(uartIndex);
		if ( t!=*p)
		{
			post_log("\nExpected: %c(0x%2x), Actual:  %c(0x%2x) ", *p,*p, t, t);
			ret = -1;
		}
		p++;
	}
	_serial_exit_loopback(uartIndex);
	return ret;

}

static void UART_post_init(void)
{

}
static int UART_post_TR_test(int uartIndex )
{
	int ret;
	unsigned long counter = 0;
	unsigned long timeout = 0;
	char ch;

	post_log("\nPlease connect terminals to console %d !!!", uartIndex);
	post_log("\nPlease make sure the console is configured as :");
	post_log("\nBaud 115200\n8 bit data 1 bit stop\nno parity, no HW flow control");
	post_log("\nReady to test (Y): ");

    //Get 'Y' from the console to continue the test

	do
	{
		ch = (char)serial_getc();
	}while ( (ch != 'y') && ( ch!='Y'));


	post_log("\nSend test string <can you see me now? press 'Y' to continue> to console %d", uartIndex);
	//Send test string to the console under test
	_serial_puts ("\ncan you see me now?press 'Y' to continue",uartIndex);
	counter = 0;

	do
	{
		counter++;
		if ( counter == 0xFFFFF )
		{
			post_log("\nWaiting on response from console %d",uartIndex);
			counter =0 ;
			timeout++;
			if ( timeout > 0xFFFF )
			{
				break;
			}
		}
		if (_serial_tstc(uartIndex) != 0 )
		{
			ch = (char)_serial_getc(uartIndex);
			if ( ('y' == ch) || ('Y'==ch ))
			{
				break;
			}
		}

	}while (1 );

	if ( timeout > 0xFFFF )
	{
		post_log("\nReceived no response from console %d",uartIndex );
		return -1;
	}
	post_log("\nReceived response from console %d",uartIndex );
	return 1;

}
int UART_post_test (int flags)
{
	int ret;
	int i;

#if defined(CONFIG_NS_PLUS)

	if ( post_get_board_diags_type() & HR_ER_BOARDS )
	{
		printf("\nThis diag is not supported on this platform\n ");
		return 2;
	}
#endif

	post_log("\nPlease set strap pin A4 to 0x18, A5 to 0x18");
	post_getConfirmation("\nReady? (Y/N)");


	post_log("\nUART diags starts  !!!");

	UART_post_init();

	post_log("\nUART loopback test...");

	for ( i = CONFIG_UART1_INDEX; i <= CONFIG_UART2_INDEX; i++)
	{
		ret = UART_post_testLoopback(i);

		if ( 0 == ret )
		{
			post_log("\nUART %d Loopback test successful!!!",i-1);
		}
		else
		{
			post_log("\nUART %d Loopback test failed!!!",i-1);
		}
	}

	for ( i = CONFIG_UART1_INDEX; i <= CONFIG_UART2_INDEX; i++)
	{
		ret = UART_post_TR_test(i);

		if ( 1 == ret )
		{
			post_log("\nUART %d TX/RX test successful!!!",i-1);
		}
		else
		{
			post_log("\nUART %d TX/RX test failed!!!",i-1);
		}
	}

    post_log("\nSuccessful, UART diags done  !!!\n");
    return 0;
}

#endif /* CONFIG_POST & CONFIG_SYS_POST_QSPI */
