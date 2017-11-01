/*
 * (C) Copyright 2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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

/*
 * I2C test
 *
 * For verifying the I2C bus, a full I2C bus scanning is performed.
 *
 * #ifdef CONFIG_SYS_POST_I2C_ADDRS
 *   The test is considered as passed if all the devices and only the devices
 *   in the list are found.
 *   #ifdef CONFIG_SYS_POST_I2C_IGNORES
 *     Ignore devices listed in CONFIG_SYS_POST_I2C_IGNORES.  These devices
 *     are optional or not vital to board functionality.
 *   #endif
 * #else [ ! CONFIG_SYS_POST_I2C_ADDRS ]
 *   The test is considered as passed if any I2C device is found.
 * #endif
 */

#include <common.h>
#include <post.h>
#include <i2c.h>

#if CONFIG_POST & CONFIG_SYS_POST_I2C

#if (defined(CONFIG_NS_PLUS))
#define SHADOW_CPY_BUFFER  0x70000000
#else
#define SHADOW_CPY_BUFFER  0x89000000
#endif

static int i2c_ignore_device(unsigned int chip)
{
#ifdef CONFIG_SYS_POST_I2C_IGNORES
	const unsigned char i2c_ignore_list[] = CONFIG_SYS_POST_I2C_IGNORES;
	int i;

	for (i = 0; i < sizeof(i2c_ignore_list); i++)
		if (i2c_ignore_list[i] == chip)
			return 1;
#endif

	return 0;
}

int i2c_post_test (int flags)
{
#if 1
        char argv_buf[7*10];
        char * test_argv[7];
        int  i;

#if (defined(CONFIG_NS_PLUS))

    if ( post_get_board_diags_type() & HR_ER_BOARDS )
	{
		printf("\nThis diag is not supported on this platform\n ");
		return 2;
	}
#endif

		printf("\nRun diags: Start I2C test\n");
        memset(&argv_buf, 0, 7*10);
        for(i=0; i<7; i++)
		test_argv[i] = &argv_buf[i*10];

        if ( (flags & POST_AUTO) !=  POST_AUTO )
        {
            post_log("\nPlease set strap pin: A2=0, A3=0x3, A4=0xf, A5=0x4f, A6=0xf3", 0);
            post_getConfirmation("\nReady? (Y/N)");
        }
        else
        {
            post_set_strappins("A2 0%A3 3%A4 f%A5 5f%A6 f3%");
        }

        printf("\n\nI2C Reset & Init\n");
        strcpy(test_argv[0], "i2c");
        strcpy(test_argv[1], "reset");
        do_i2c(0, 0, 2, test_argv);
		udelay(2000);
#if 0//DEBUG
        printf("\n\nI2C Read -- i2c md chip address[.0, .1, .2]\n");
        printf("I2C Read -- i2c md 50 0.1, i2c_chip_addr=0x50, EEPROM mem_addr=0x0, 1-byte address\n\n");
        strcpy(test_argv[0], "i2c");
        strcpy(test_argv[1], "md");
        strcpy(test_argv[2], "50");
        strcpy(test_argv[3], "0.1");
        do_i2c(0, 0, 4, test_argv);
		udelay(2000);

        printf("\n\nI2C Read -- i2c md chip address[.0, .1, .2] [# of objects]\n");
        printf("I2C Read -- i2c md 50 0.1 f0, i2c_addr=0x50, mem_addr=0x0, count=0xf0\n\n");
        strcpy(test_argv[0], "i2c");
        strcpy(test_argv[1], "md");
        strcpy(test_argv[2], "50");
        strcpy(test_argv[3], "0.1");
        strcpy(test_argv[4], "F0");
        do_i2c(0, 0, 5, test_argv);
		udelay(2000);
#endif
        printf("\n\nI2C Write (memory fill) -- i2c mw chip address[.0, .1, .2] value [count]\n");
        printf("I2C Write -- i2c mw 50 10.1 68 10, i2c_addr=0x50, mem_addr=0x10, value=0x68, count=0x10\n");
        strcpy(test_argv[0], "i2c");
        strcpy(test_argv[1], "mw");
        strcpy(test_argv[2], "50");
        strcpy(test_argv[3], "10.1");
        strcpy(test_argv[4], "68");
        strcpy(test_argv[5], "10");
        do_i2c(0, 0, 6, test_argv);
		udelay(2000);


		printf("\n\nI2C Read -- i2c md chip address[.0, .1, .2] [# of objects]\n");
        printf("I2C Read -- i2c md 50 0.1 f0, i2c_addr=0x50, mem_addr=0x0, count=0xf0\n\n");
        strcpy(test_argv[0], "i2c");
        strcpy(test_argv[1], "md");
        strcpy(test_argv[2], "50");
        strcpy(test_argv[3], "10.1");
        strcpy(test_argv[4], "10");
        do_i2c(0, 0, 5, test_argv);
		udelay(2000);

		for ( i = 0; i< 0x10; i++ )
		{
		    if ( *((unsigned char*)SHADOW_CPY_BUFFER+i) != 0x68 )
		    {
		        post_log("\"Failed, at location 0x%x, data ", SHADOW_CPY_BUFFER+ i, *((unsigned char*)SHADOW_CPY_BUFFER+i));
		        return -1;
		    }
		}
		return 0;

#else
	unsigned int i;
#ifndef CONFIG_SYS_POST_I2C_ADDRS
	/* Start at address 1, address 0 is the general call address */
	for (i = 1; i < 128; i++) {
		if (i2c_ignore_device(i))
			continue;
		if (i2c_probe (i) == 0)
			return 0;
	}

	/* No devices found */
	return -1;
#else
	unsigned int ret  = 0;
	int j;
	unsigned char i2c_addr_list[] = CONFIG_SYS_POST_I2C_ADDRS;

	/* Start at address 1, address 0 is the general call address */
	for (i = 1; i < 128; i++) {
		if (i2c_ignore_device(i))
			continue;
		if (i2c_probe(i) != 0)
			continue;

		for (j = 0; j < sizeof(i2c_addr_list); ++j) {
			if (i == i2c_addr_list[j]) {
				i2c_addr_list[j] = 0xff;
				break;
			}
		}

		if (j == sizeof(i2c_addr_list)) {
			ret = -1;
			post_log("I2C: addr %02x not expected\n", i);
		}
	}

	for (i = 0; i < sizeof(i2c_addr_list); ++i) {
		if (i2c_addr_list[i] == 0xff)
			continue;
		post_log("I2C: addr %02x did not respond\n", i2c_addr_list[i]);
		ret = -1;
	}

	return ret;
#endif // CONFIG_SYS_POST_I2C_ADDRS

#endif //
}

#endif /* CONFIG_POST & CONFIG_SYS_POST_I2C */
