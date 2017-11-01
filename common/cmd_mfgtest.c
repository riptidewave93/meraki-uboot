

#include <common.h>
#include <command.h>
#include <search.h>
#include <errno.h>
#include <malloc.h>
#include <watchdog.h>
#include <serial.h>
#include <linux/stddef.h>
#include <asm/byteorder.h>

#include <nand.h>
#include <asm/iproc/iproc_nand.h>
#include <iproc_gpio.h>
#include <iproc_gpio_cfg.h>


/***  constants ***/
#define NAND_TEST_ADDR_START    0x000E0000
#define NAND_TEST_ADDR_LEN      0x00020000
#define NAND_BADBLK_RATIO_MAX   (0.02)
#define MEM_TEMP_ADDR_START     0x81000000

#define MEM_TEST_ADDR_START     0x80208000 // kernel starting address
#define MEM_TEST_ADDR_LEN       0x00400000 // kernel image data size
#define MEM_TEST_ADDR_START_EXT 0x01000000 // after u-boot (estimation mem size: 1MB)
#define MEM_TEST_ADDR_LEN_EXT   0x04000000
#define MEM_TEST_ADDR_PER_LEN   0x00010000

#define GPIO_BTN_RESET          11

/***  static variables ***/
static iproc_gpio_chip *testDev = NULL;


/***  global variables ***/
uchar disableDefaultMsg = 0;


/***  static function prototype ***/
static void set_RGB_led(int BData, int GData, int RData);
static int GPIO_funcConfigTest( int cfg );
static void GPIO_testInit(void);


/***  function prototype ***/
static int do_mfgtest_btn(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    int ret = 0;

    if (argc == 2)
    {
        if (!strcmp(argv[1], "reset"))
        {
            volatile unsigned long delay;
            extern volatile int IntReceived;
            int i;

            disableDefaultMsg = 1;

            GPIO_testInit();
            GPIO_funcConfigTest(IPROC_GPIO_GENERAL);

            initGPIOState();
            registerGPIOEdgeInterrupt();
            
            setGPIOasInput(GPIO_BTN_RESET);
            enableGPIOInterrupt( GPIO_BTN_RESET, IRQ_TYPE_EDGE_RISING );

            delay = 10*1000*1000; // 10s = 10*1000*1000 us
            IntReceived= 0;

            printf("reset button: gpio-pin %d, low active.\n", GPIO_BTN_RESET);
            printf("Please press reset button within 10 seconds...\n");
          
            while (!IntReceived)
            {
                delay-=(100*1000);
                if (delay == 0)
                {
                    printf("reset button not detected. Fail!\n");
                    disableDefaultMsg = 0;
                    return 0;
                }
                udelay(100*1000); // 100ms
            }
            printf("reset button is pressed! Pass!\n");

            disableDefaultMsg = 0;
            return 0;
        }
//        else if (!strcmp(argv[1], "wps"))
//        {
//           
//            return 0;
//        }
    }
    printf("##Error! Invalid arguments!\n");

    return 1;
}

static int do_mfgtest_led_ledrgb(void)
{
    int i;

    printf("RGB Led Test:\n");

    GPIO_testInit();
	GPIO_funcConfigTest(IPROC_GPIO_GENERAL);

//    printf("Off\n");
//	setGPIOasOutput(12, 0);
//	setGPIOasOutput(13, 0);
//    set_RGB_led(0, 0, 0);
//    udelay(2000*1000);

//    printf("On\n");
//	setGPIOasOutput(12, 1);
//	setGPIOasOutput(13, 1);
//	set_RGB_led(0xFFFF, 0xFFFF, 0xFFFF);
//    udelay(2000*1000);

    printf("White\n");
	setGPIOasOutput(12, 1);
	setGPIOasOutput(13, 0);
    set_RGB_led(0, 0, 0);
    udelay(2000*1000);

    printf("Orange\n");
	setGPIOasOutput(12, 0);
	setGPIOasOutput(13, 1);
    set_RGB_led(0, 0, 0);
    udelay(2000*1000);

    printf("Red\n");
	setGPIOasOutput(12, 0);
	setGPIOasOutput(13, 0);
    set_RGB_led(0, 0, 0xFFFF);
    udelay(2000*1000);

    printf("Green\n");
	setGPIOasOutput(12, 0);
	setGPIOasOutput(13, 0);
    set_RGB_led(0, 0xFFFF, 0);
    udelay(2000*1000);

    printf("Blue\n");
	setGPIOasOutput(12, 0);
	setGPIOasOutput(13, 0);
    set_RGB_led(0xFFFF, 0, 0);
    udelay(2000*1000);

    printf("Black\n");
	setGPIOasOutput(12, 0);
	setGPIOasOutput(13, 0);
    set_RGB_led(0, 0, 0);
    udelay(2000*1000);

//    printf("RGB White\n");
//	setGPIOasOutput(12, 0);
//	setGPIOasOutput(13, 0);
//    set_RGB_led(0xFFFF, 0xFFFF, 0xFFFF);
//    udelay(2000*1000);

//    printf("RGB White to Black...\n");
//    for (i=65535; i>=0; i=i-(i/1000+1))
//    //for (i=65535; i>=0; i-=4)
//    {
//        set_RGB_led(i, i, i);
//        udelay(1050);
//    }
//    set_RGB_led(0, 0, 0);
//    udelay(2100);

//    printf("RGB Black to White...\n");
//    for (i=0; i<65536; i=i+(i/1000+1))
//    //for (i=0; i<65536; i+=4)
//    {
//        set_RGB_led(i, i, i);
//        udelay(1050);
//    }
//    set_RGB_led(0xFFFF, 0xFFFF, 0xFFFF);
//    udelay(2100);

    // back to original status(?)
	setGPIOasOutput(12, 0);
	setGPIOasOutput(13, 1);
	set_RGB_led(0, 0, 0);

	return 0;
}

static int do_mfgtest_gpio(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    int pin = -1;
    int val = -1;

    if (argc >= 3)
        pin = simple_strtoul(argv[2], NULL, 0);
    if (argc >= 4)
        pin = simple_strtoul(argv[3], NULL, 0);
    if (argc >= 5)
        val = simple_strtoul(argv[4], NULL, 0);

    // "mfg gpio get <pin>"
    if (( argc == 3 ) && 
        ( !strcmp(argv[1],"get") && pin >=0 && pin <24 )) 
    {
        GPIO_testInit();
	    //iproc_gpio_saveCFGRegisters();
	    GPIO_funcConfigTest(IPROC_GPIO_GENERAL);
        val = getGPIOpinValue(pin);
        printf("*** GPIO %d value = %d ***\n", pin, val);
    }
//    // "mfg gpio input set <pin>"
//    else if (( argc ==4 ) && 
//        ( !strcmp(argv[1],"input") && !strcmp(argv[2],"set") && pin >=0 && pin <24 )) 
//    {
//        GPIO_testInit();
//	    //iproc_gpio_saveCFGRegisters();
//	    GPIO_funcConfigTest(IPROC_GPIO_GENERAL);
//        printf("*** set GPIO %d as input ***\n", pin);
//        setGPIOasInput(pin);
//    }
    // "mfg gpio output set <pin> <val>"
    else if (   ( argc == 5 ) && 
                (!strcmp(argv[1],"output") && !strcmp(argv[2],"set") && (pin >=0 && pin <24) &&
                ((argv[4][0] == '0' || argv[4][0] == '1') && argv[4][1] == 0)
            )) 
    {
        GPIO_testInit();
	    //iproc_gpio_saveCFGRegisters();
	    GPIO_funcConfigTest(IPROC_GPIO_GENERAL);
        printf("*** set GPIO %d output = %d ***\n", pin, val);
        setGPIOasOutput(pin, val);
    }
    else
    {
        printf("##Error! Invalid arguments!\n");
        return (-1);
    }

    return (0);
}

static int do_mfgtest_i2c(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    extern u8 i2cdata[8];

    if (strcmp(argv[1], "test"))
        return (-1);

    printf("*** I2C Test ***\n");
    run_command("i2c reset", 0);

//    // (1) EEPROM: 0x56
//    printf("-- EEPROM:\n");
//    printf("reading 0x00 => ");
//    run_command("i2c mdb 0x56 0x0 0x1", 0);
//    printf("\n");

//    // (2) Temperature: 0x4c
//    printf("Temperature:\n");
//    printf(" reading 0xfe => ");
//    run_command("i2c md 0x4c 0xfe 0x1", 0); // WORKAROUND! not work using ***run_command("i2c md 0x4c 0xfe 0x1", 0);***
//    if (i2cdata[0] != 0x55) // Manufacture ID
//    {
//        printf(" (should be 0x55)\n");
//        goto do_mfgtest_i2c_error;
//    }
//    printf(" reading 0xff => ");
//    run_command("i2c md 0x4c 0xff 0x1", 0); // WORKAROUND! not work using ***run_command("i2c md 0x4c 0xff 0x1", 0);***
//    if (i2cdata[0] != 0x23) // TMP423 Device ID
//    {
//        printf(" (should be 0x23)\n");
//        goto do_mfgtest_i2c_error;
//    }

    // (3) Current/Power: 0x40
    printf("Current/Power:\n");
    printf(" reading 0x00 => ");
    run_command("i2c mdw 0x40 0x0 0x1", 0);
    if (i2cdata[0] != 0x39 || i2cdata[1] != 0x9f)
    {
        printf(" (should be 0x399f)\n");
        goto do_mfgtest_i2c_error;
    }
    printf("\n");

    printf("Test result: PASSED!\n");
    return 0;

do_mfgtest_i2c_error:
    printf("Test result: FAILED!\n");
    return 0; //return (-1);
}

// refer to "5301X-PR105-RDS-docs717514.pdf" p538,539 and p2815,2816
static int do_mfgtest_eth(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    uchar *pt = NULL;
    int val = 0;

    if (argc == 1)
    {
        // ethernet link status
        run_command("mw 0x1800702c 0x01000001", 0);
        pt = 0x1800703c;
        printf("Link status: %s\n", (*pt&0x1?"Connected":"Disconnected"));
        if (*pt&0x1)
        {
            // ethernet link speed status
            run_command("mw 0x1800702c 0x01040001", 0);
            pt = 0x1800703c;
            printf("Link speed: ");
            switch(*pt & 0x3)
            {
                case 0:
                    val = 10;
                    break;
                case 1:
                    val = 100;
                    break;
                case 2:
                    val = 1000;
                    break;
                case 3:
                default:
                    printf("invalid value!\n");
                    return (-1);
            }
            printf("%d Mbps\n", val);
        }
    }
    else if (argc == 3)
    {
        if (!strcmp(argv[1], "spd"))
        {
            if (!strcmp(argv[2], "1000") || !strcmp(argv[2], "auto"))
            {
                run_command("mw 0x18007034 0x00001140", 0);
                run_command("mw 0x1800702c 0x10000003", 0);
            }
            else if (!strcmp(argv[2], "100"))
            {
                run_command("mw 0x18007034 0x00002100", 0);
                run_command("mw 0x1800702c 0x10000003", 0);
            }
            else if (!strcmp(argv[2], "10"))
            {
                run_command("mw 0x18007034 0x00000100", 0);
                run_command("mw 0x1800702c 0x10000003", 0);
            }
            else
                return (-1);
        }
        else if (!strcmp(argv[1], "test"))
        {
            int i;
            int rate[] = {1000, 100, 10};
            char cmdbuf[32];
            extern IPaddr_t NetPingIP;
    
            for (i=0; i<3; i++)
            {
                printf("Force to %dM... ", rate[i]);
                sprintf(cmdbuf, "mfg eth spd %d", rate[i]);
                run_command(cmdbuf, 0);
                udelay(3*1000*1000); // wait link established
            
                val = -1;
                run_command("mw 0x1800702c 0x01000001", 0);
                pt = 0x1800703c;
                if (*pt&0x1)
                {
                    run_command("mw 0x1800702c 0x01040001", 0);
                    pt = 0x1800703c;
        
                    switch(*pt & 0x3)
                    {
                        case 0:
                            val = 10;
                            break;
                        case 1:
                            val = 100;
                            break;
                        case 2:
                            val = 1000;
                            break;
                        default:
                            break;
                    }
                }
                if (val == rate[i])
                {
                    //udelay(12*1000*1000); // wait network ready for ping...
                    printf("Ping... ");
                    NetPingIP = string_to_ip(argv[2]);
	                if (NetPingIP == 0)
		                return CMD_RET_USAGE;
	                if (NetLoop(PING) < 0)
	                    printf("Failed!\n");
	                else
                        printf("Passed!\n");
                }
                else
                    printf("Failed!\n");
            }
            run_command("mfg eth spd 1000", 0);
            printf("Back to Auto... ");
            
            NetPingIP = string_to_ip(argv[2]);
            NetLoop(PING);
            //udelay(15*1000*1000); // wait link established
            printf("Done!\n");
        }
    }
    else
        return (-1);

    return 0;
}

static int do_mfgtest_led(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    int ret = 0;

    if (argc == 2)
    {
        if (!strcmp(argv[1], "rgbled"))
	        return do_mfgtest_led_ledrgb();
        //else if (!strcmp(argv[1], "off") || !strcmp(argv[1], "on"))
        //    return 0;
    }
    else if (argc == 3)
    {
        if (!strcmp(argv[1], "phyled") && !argv[2][1])
        {
            // refer to "5301X-PR105-RDS-docs717514.pdf" p538,539 and p2796-2798
            switch (argv[2][0])
            {
                case '0': // OFF
                    run_command("mw 0x18007034 0x000001fe", 0);
                    run_command("mw 0x1800702c 0x00180003", 0);
                    run_command("mw 0x18007034 0x000001fe", 0);
                    run_command("mw 0x1800702c 0x001a0003", 0);
                    return 0;
                case '1': // ON
                    run_command("mw 0x18007034 0x000001ff", 0);
                    run_command("mw 0x1800702c 0x00180003", 0);
                    run_command("mw 0x18007034 0x000001fe", 0);
                    run_command("mw 0x1800702c 0x001a0003", 0);
                    return 0;
                case '2': // BLINK
                    run_command("mw 0x18007034 0x000001fe", 0);
                    run_command("mw 0x1800702c 0x00180003", 0);
                    run_command("mw 0x18007034 0x000001ff", 0);
                    run_command("mw 0x1800702c 0x001a0003", 0);
                    return 0;
                case '3': // AUTO
                    run_command("mw 0x18007034 0x000001ff", 0);
                    run_command("mw 0x1800702c 0x00180003", 0);
                    run_command("mw 0x18007034 0x000001ff", 0);
                    run_command("mw 0x1800702c 0x001a0003", 0);
                    return 0;
                default:
                    break;
            }
        }
    }
    printf("##Error! Invalid arguments!\n");

    return 1;
}

static int do_mfgtest_mem(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    const unsigned int test_pattern[] = {   0x00000000, 0xFFFFFFFF, 
                                            0x55555555, 0xAAAAAAAA, 
                                            0x33333333, 0xCCCCCCCC,
                                            0x0F0F0F0F, 0xF0F0F0F0, 
                                            0xFF00FF00, 0x00FF00FF};
    unsigned int i, j, l, loops = 1, test_pattern_num = 0;
    unsigned int *pt = NULL, mem_test_start = 0, mem_test_length = 0;

    if (!strcmp(argv[1], "test"))
    {
        mem_test_start = MEM_TEST_ADDR_START;
        mem_test_length = MEM_TEST_ADDR_LEN;
    }
    else if (!strcmp(argv[1], "ext"))
    {
        mem_test_start = MEM_TEST_ADDR_START_EXT;
        mem_test_length = MEM_TEST_ADDR_LEN_EXT;
    }
    else
        goto do_mfgtest_mem_error;

    if (argc == 3 && !(loops = simple_strtoul(argv[2], NULL, 0 )))
        loops = 1;
    else if (argc > 3)
        goto do_mfgtest_mem_error;

    test_pattern_num = sizeof(test_pattern)/sizeof(test_pattern[0]);

    printf("range: 0x%08x-0x%08x\n", mem_test_start, mem_test_start+mem_test_length);

    // read/write test loop......
    for (l=0; l<loops; l++)
    {
        printf("loop %d: \n", l);
        for (i=0; i<test_pattern_num; i++)
        {
            printf("Pattern: 0x%08x%08x\n", test_pattern[i], test_pattern[i]);
            for (pt=mem_test_start; pt<mem_test_start+mem_test_length; pt++/* 4bytes shift*/)
            {
                *pt = test_pattern[i];
            }
            for (pt=mem_test_start; pt<mem_test_start+mem_test_length; pt++)
            {
                if (*pt != test_pattern[i])
                {
                    printf("Memory test FAIL!\n");
                    printf("Address: 0x%08x\n", pt);
                    return 1;
                }
            }
    		if (ctrlc()) {
    			puts("Abort\n");
    			return 0;
    		}
        }
    }
    printf("Memory test PASS!\n");
    return 0;

do_mfgtest_mem_error:
    printf("##Error! Invalid arguements!\n");
    return 1;
}

static int do_mfgtest_nand(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    char cmd[512];

    if (argc == 0)
    {
        goto do_mfgtest_nand_error;
    }
    else if (argc == 1)
    {
        // e.g. "Device: NAND 128MB 3.3V 8-bit, sector size 128KB"
        extern unsigned long total_nand_size; // KiB
        extern INAND *inand;
    
        nand_info_t *nand = &nand_info[0];
    
        printf("Device: NAND %dMB 3.3V %d-bit, sector size %dKB\n", 
                total_nand_size/1024,
                (inand?inand[0].device_width:0),
                nand->erasesize >> 10);
    }
    else
    {
        if (!strcmp(argv[1], "test"))
        {
            const uchar test_pattern[] = {0xff, 0xaa, 0x55, 0x00};
            int i, j, k, loops = 1, test_pattern_num = 0;
            uchar *pt = NULL;
            
            test_pattern_num = sizeof(test_pattern)/sizeof(test_pattern[0]);
            pt = MEM_TEMP_ADDR_START;

            if (argc == 3 && !(loops = simple_strtoul(argv[2], NULL, 0 )))
                loops = 1;
            else if (argc > 3)
                goto do_mfgtest_nand_error;

            disableDefaultMsg = 1;
            printf("Flash read/write test (range: 0x%08x - 0x%08x)\n", 
                    NAND_TEST_ADDR_START, NAND_TEST_ADDR_START+NAND_TEST_ADDR_LEN);
            for (i=0; i<loops; i++)
            {
                printf("loop %d: ", i);
                for (j=0; j<test_pattern_num; j++)
                {
                    printf("(0x%02x)", test_pattern[j]);
                    sprintf(cmd, "mw.b 0x%x 0x%02x 0x%x", MEM_TEMP_ADDR_START, test_pattern[j], NAND_TEST_ADDR_LEN);
                    run_command(cmd, 0);
                    printf("W");
                    sprintf(cmd, "nand write 0x%x 0x%x 0x%x", MEM_TEMP_ADDR_START, NAND_TEST_ADDR_START, NAND_TEST_ADDR_LEN);
                    run_command(cmd, 0);
                    sprintf(cmd, "mw.b 0x%x 0x%x 0x%x", MEM_TEMP_ADDR_START, (j+1)%test_pattern_num, NAND_TEST_ADDR_LEN);
                    run_command(cmd, 0);
                    printf("/R");
                    sprintf(cmd, "nand read 0x%x 0x%x 0x%x", MEM_TEMP_ADDR_START, NAND_TEST_ADDR_START, NAND_TEST_ADDR_LEN);
                    run_command(cmd, 0);
                    printf("  ");
                    for (k=0; k<NAND_TEST_ADDR_LEN; k++)
                    {
                        if (pt[k] != test_pattern[j])
                        {
                            printf("NG!\n");
                            printf("NAND Flash Read/Write Test: Fail!\n");
                            disableDefaultMsg = 0;
                            return 1;
                         }
                    }
            		if (ctrlc()) {
            			puts("Abort\n");
            			return 0;
            		}
                }
                printf("OK!\n");
            }
            printf("NAND Flash Read/Write Test: Pass!\n");
            disableDefaultMsg = 0;
        }
        else if ((!strcmp(argv[1], "badblk") || !strcmp(argv[1], "badblklst")) && argc == 2)
        {
	        nand_info_t *nand = &nand_info[0];
	        loff_t off;
	        int good = 0, bad = 0;
	        int detail = 0;
	        int i;

	        if (!strcmp(argv[1], "badblklst"))
		        detail = 1;

		    for (i=0, off = 0; off < nand->size; i++, off += nand->erasesize)
		    {
			    if (nand_block_isbad(nand, off))
			    {
			        if (detail)
			            printf("Badblock #%d\n", i);
			        bad++;
			    }
			    else
			        good++;
		    }
            printf("Total blocks: %d\n", good+bad);
            printf("Bad blocks: %d\n", bad);
            printf("Good blocks: %d\n", good);
            printf("NAND Flash Badblock Check: %s!\n", ((bad>(good+bad)*NAND_BADBLK_RATIO_MAX)?"Fail":"Pass") );
        }

    }


    return 0;

do_mfgtest_nand_error:
    printf("##Error! Invalid arguements!\n");
    return 1;
}

static cmd_tbl_t cmd_mfgtest_sub[] = {
	U_BOOT_CMD_MKENT(nand, CONFIG_SYS_MAXARGS, 0, do_mfgtest_nand, "", ""),
    U_BOOT_CMD_MKENT(mem, 1, 0, do_mfgtest_mem, "", ""),
    U_BOOT_CMD_MKENT(led, CONFIG_SYS_MAXARGS, 0, do_mfgtest_led, "", ""),
    U_BOOT_CMD_MKENT(btn, CONFIG_SYS_MAXARGS, 0, do_mfgtest_btn, "", ""),
    U_BOOT_CMD_MKENT(eth, 1, 0, do_mfgtest_eth, "", ""),
    U_BOOT_CMD_MKENT(i2c, CONFIG_SYS_MAXARGS, 0, do_mfgtest_i2c, "", ""),

    U_BOOT_CMD_MKENT(gpio, CONFIG_SYS_MAXARGS, 0, do_mfgtest_gpio, "", ""),
};

int do_mfgtest(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	cmd_tbl_t *cp;

	if (argc < 2)
		return CMD_RET_USAGE;

	/* drop initial "mfg" arg */
	argc--;
	argv++;

	cp = find_cmd_tbl(argv[0], cmd_mfgtest_sub, ARRAY_SIZE(cmd_mfgtest_sub));

	if (cp)
		return cp->cmd(cmdtp, flag, argc, argv);

	return CMD_RET_USAGE;
}

U_BOOT_CMD(
    mfg, CONFIG_SYS_MAXARGS, 1, do_mfgtest,
    "test functions for manufacture",
    "nand - NAND flash info\n"
    "mfg nand test [loops] - NAND flash read/write test\n"
    "mfg nand badblk - bad block check\n"
    "mfg nand badblklst - bad block check and list\n"
    "mfg mem test [loops] - memory read/write test\n"
    "mfg mem ext [loops] -  memory read/write test on big range\n"
    "mfg led phyled [0|1|2|3] - ethernet PHY LED on/off test (0:Off; 1:On; 2:Blink 3:Default)\n"
    "mfg led rgbled - RGB LED test\n"
//    "mfg led [off|on] - top LED on/off test\n"
//    "mfg led led_blink - top LED blink test\n"
//    "mfg led led_pulse - top LED pulse test\n"
//    "mfg btn wps - WPS button pressing test\n"
    "mfg btn reset - reset button pressing test\n"
    "mfg eth - ethernet link status and speed\n"
    "mfg eth spd [10|100|1000|auto] - force ethernet speed\n"
    "mfg eth test [ipaddr] - ethernet link test (ipaddr: host for ping test)\n"
//    "mfg gpio input set <pin> - set GPIO pin as input\n"
    "mfg i2c test - I2C devices reading test\n"
    "mfg gpio output set <pin> [0|1] - set GPIO pin output\n"
    "mfg gpio get <pin> - get GPIO pin value\n" 
);

static void set_RGB_led(int BData, int GData, int RData)
{
	int gpio_Clock, gpio_Latch, gpio_Data;
	int serial_data[3];
	int data;
	int i, j;

	serial_data[0]=BData;
	serial_data[1]=GData;
	serial_data[2]=RData;

	gpio_Clock = 0;
	gpio_Latch = 1;
	gpio_Data = 2;

	for (i=0; i<3; i++)
	{
		for (j=1; j<=16; j++)
		{
			data = ((serial_data[i]<<j) & 0x10000) ? 1 : 0;
			//xprintf("serial_data[%d][%d]=%x\n", i, j, data);

			//Send Serial Data
			setGPIOasOutput(gpio_Data, data);
			udelay(1);    /* 1 usec delay */

			//Set Clock high
			setGPIOasOutput(gpio_Clock, 1);
			udelay(1);    /* 1 usec delay */

			//Set Clock low
			setGPIOasOutput(gpio_Clock, 0);
		}
	}

	udelay(1);    /* 1 usec delay */

	//Set Latch high
	setGPIOasOutput(gpio_Latch, 1);
	udelay(1);    /* 1 usec delay */

	//Set Latch low
	setGPIOasOutput(gpio_Latch, 0);
	udelay(1);    /* 1 usec delay */
}

static void GPIO_testInit(void)
{
    //Perform hardware init
    iprocGPIOInit();
    //Get the device handle
    testDev=getGPIODev();

}

static int GPIO_funcConfigTest( int cfg )
{
    int i,ret = 0;

    //Test GPIO DMU MUX select, there is a documentation error
    //in the Register programming guide, the MUX select does not
    // select GPIO 18-20
    for ( i = 0; i < 24; i++ )
    {
        //Do not set for GPIO 18-20 there is no MUX available
        if ( (i >=18) && ( i <=20 ) )
        {
            continue;
        }
        else if ( i > 20 )
        {
            //Use bit 18-20  in the register for GPIO pin 21-24
            iproc_gpio_set_config(testDev,i-3, cfg);
        }
        iproc_gpio_set_config(testDev,i, cfg);
    }
    for ( i = 0; i < 24; i++ )
    {
        if ( (i >=18) && ( i <=20 ) )
        {
            continue;
        }
        if ( i > 20 )
        {
            if ( cfg != iproc_gpio_get_config(testDev,i-3))
            {
                post_log("\nGPIO pin %d is not configured as expected", i);
                ret = -1;
            }
        }
        else if ( cfg != iproc_gpio_get_config(testDev,i))
        {
            post_log("\nGPIO pin %d is not configured as expected", i);
            ret = -1;
        }
    }
    if ( ret == -1 )
    {
        post_log("\nGPIO function configuration test failed");
    }
//    else
//    {
//        post_log("\nGPIO function configuration test passed");
//    }
    return ret;
}

//void do_gpio_input(int pin)
//{
////    GPIO_testInit();
////	GPIO_funcConfigTest(IPROC_GPIO_GENERAL);
////
////    //initGPIOState();
////    //registerGPIOEdgeInterrupt();
////
////    printf("*** set GPIO %d as input ***\n", pin);
////
////    setGPIOasInput(pin);
//}
