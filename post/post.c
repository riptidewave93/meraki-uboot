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

#include <common.h>
#include <stdio_dev.h>
#include <watchdog.h>
#include <div64.h>
#include <post.h>

#ifdef CONFIG_SYS_POST_HOTKEYS_GPIO
#include <asm/gpio.h>
#endif

#ifdef CONFIG_LOGBUFFER
#include <logbuff.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

#define POST_MAX_NUMBER		32

#define BOOTMODE_MAGIC	0xDEAD0000


extern int serial_init_com8051(void);

static void post_init_strap_ctrl(void);


char string[200]={0};
#if (defined(CONFIG_NS_PLUS))
static int gDiags_board = NO_DIAGS_SUPPORT;
#endif

int post_init_f(void)
{
	int res = 0;
#ifndef IPROC_BOARD_DIAGS
	unsigned int i;

	for (i = 0; i < post_list_size; i++) {
		struct post_test *test = post_list + i;

		if (test->init_f && test->init_f())
			res = -1;
	}

	gd->post_init_f_time = post_time_ms(0);
	if (!gd->post_init_f_time)
		printf("%s: post_time_ms not implemented\n", __FILE__);
#endif
	return res;
}

/*
 * Supply a default implementation for post_hotkeys_pressed() for boards
 * without hotkey support. We always return 0 here, so that the
 * long-running tests won't be started.
 *
 * Boards with hotkey support can override this weak default function
 * by defining one in their board specific code.
 */
int __post_hotkeys_pressed(void)
{
#ifdef CONFIG_SYS_POST_HOTKEYS_GPIO
	int ret;
	unsigned gpio = CONFIG_SYS_POST_HOTKEYS_GPIO;

	ret = gpio_request(gpio, "hotkeys");
	if (ret) {
		printf("POST: gpio hotkey request failed\n");
		return 0;
	}

	gpio_direction_input(gpio);
	ret = gpio_get_value(gpio);
	gpio_free(gpio);

	return ret;
#endif

	return 0;	/* No hotkeys supported */
}
int post_hotkeys_pressed(void)
	__attribute__((weak, alias("__post_hotkeys_pressed")));


void post_bootmode_init(void)
{
#ifndef IPROC_BOARD_DIAGS
	int bootmode = post_bootmode_get(0);
	int newword;

	if (post_hotkeys_pressed() && !(bootmode & POST_POWERTEST))
		newword = BOOTMODE_MAGIC | POST_SLOWTEST;
	else if (bootmode == 0)
		newword = BOOTMODE_MAGIC | POST_POWERON;
	else if (bootmode == POST_POWERON || bootmode == POST_SLOWTEST)
		newword = BOOTMODE_MAGIC | POST_NORMAL;
	else
		/* Use old value */
		newword = post_word_load() & ~POST_COLDBOOT;

	if (bootmode == 0)
		/* We are booting after power-on */
		newword |= POST_COLDBOOT;

	post_word_store(newword);

	/* Reset activity record */
	gd->post_log_word = 0;
	gd->post_log_res = 0;
#endif
}

int post_bootmode_get(unsigned int *last_test)
{
#ifndef IPROC_BOARD_DIAGS
	unsigned long word = post_word_load();
	int bootmode;

	if ((word & 0xFFFF0000) != BOOTMODE_MAGIC)
		return 0;

	bootmode = word & 0x7F;

	if (last_test && (bootmode & POST_POWERTEST))
		*last_test = (word >> 8) & 0xFF;

	return bootmode;
#endif
    return 0;
}

/* POST tests run before relocation only mark status bits .... */
static void post_log_mark_start(unsigned long testid)
{
#ifndef IPROC_BOARD_DIAGS
	gd->post_log_word |= testid;
#endif
}

static void post_log_mark_succ(unsigned long testid)
{
#ifndef IPROC_BOARD_DIAGS
	gd->post_log_res |= testid;
#endif
}

/* ... and the messages are output once we are relocated */
void post_output_backlog(void)
{
#ifndef IPROC_BOARD_DIAGS
	int j;

	for (j = 0; j < post_list_size; j++) {
		if (gd->post_log_word & (post_list[j].testid)) {
			post_log("POST %s ", post_list[j].cmd);
			if (gd->post_log_res & post_list[j].testid)
				post_log("PASSED\n");
			else {
				post_log("FAILED\n");
				bootstage_error(BOOTSTAGE_ID_POST_FAIL_R);
			}
		}
	}
#endif
}

static void post_bootmode_test_on(unsigned int last_test)
{
#ifndef IPROC_BOARD_DIAGS
	unsigned long word = post_word_load();

	word |= POST_POWERTEST;

	word |= (last_test & 0xFF) << 8;

	post_word_store(word);
#endif
}

static void post_bootmode_test_off(void)
{
#ifndef IPROC_BOARD_DIAGS
	unsigned long word = post_word_load();

	word &= ~POST_POWERTEST;

	post_word_store(word);
#endif
}

#ifndef CONFIG_POST_SKIP_ENV_FLAGS
static void post_get_env_flags(int *test_flags)
{
	int  flag[] = {  POST_POWERON,   POST_NORMAL,   POST_SLOWTEST,
			 POST_CRITICAL };
	char *var[] = { "post_poweron", "post_normal", "post_slowtest",
			"post_critical" };
	int varnum = ARRAY_SIZE(var);
	char list[128];			/* long enough for POST list */
	char *name;
	char *s;
	int last;
	int i, j;

	for (i = 0; i < varnum; i++) {
		if (getenv_f(var[i], list, sizeof(list)) <= 0)
			continue;

		for (j = 0; j < post_list_size; j++)
			test_flags[j] &= ~flag[i];

		last = 0;
		name = list;
		while (!last) {
			while (*name && *name == ' ')
				name++;
			if (*name == 0)
				break;
			s = name + 1;
			while (*s && *s != ' ')
				s++;
			if (*s == 0)
				last = 1;
			else
				*s = 0;

			for (j = 0; j < post_list_size; j++) {
				if (strcmp(post_list[j].cmd, name) == 0) {
					test_flags[j] |= flag[i];
					break;
				}
			}

			if (j == post_list_size)
				printf("No such test: %s\n", name);

			name = s + 1;
		}
	}
}
#endif

static void post_get_flags(int *test_flags)
{
	int j;

	for (j = 0; j < post_list_size; j++)
		test_flags[j] = post_list[j].flags;

#ifndef CONFIG_POST_SKIP_ENV_FLAGS
	post_get_env_flags(test_flags);
#endif

	for (j = 0; j < post_list_size; j++)
		if (test_flags[j] & POST_POWERON)
			test_flags[j] |= POST_SLOWTEST;
}

void __show_post_progress(unsigned int test_num, int before, int result)
{
}
void show_post_progress(unsigned int, int, int)
			__attribute__((weak, alias("__show_post_progress")));

static int post_run_single(struct post_test *test,
				int test_flags, int flags, unsigned int i)
{
	int testResult;
#if (defined(CONFIG_NS_PLUS))
	if ( post_get_board_diags_type() == NO_DIAGS_SUPPORT )
	{
		printf("\nNo diags support defined in the env, need to be one of the "
				"\nBCM953022K\nBCM953025K\nBCM953025K_REV2\nBCM958622HR",
				"\nBCM958623HR\nBCM958522ER\nBCM958625HR\n");
		return -1;
	}
#endif

	if ((flags & test_flags & POST_ALWAYS) &&
		(flags & test_flags & POST_MEM)) {
		WATCHDOG_RESET();

		if (!(flags & POST_REBOOT)) {
			if ((test_flags & POST_REBOOT) &&
				!(flags & POST_MANUAL)) {
				post_bootmode_test_on(
					(gd->flags & GD_FLG_POSTFAIL) ?
						POST_FAIL_SAVE | i : i);
			}

			if (test_flags & POST_PREREL)
				post_log_mark_start(test->testid);
			else
				post_log("POST %s ", test->cmd);
		}

		show_post_progress(i, POST_BEFORE, POST_FAILED);

		if (test_flags & POST_PREREL) {
			if ((*test->test)(flags) == 0) {
				post_log_mark_succ(test->testid);
				show_post_progress(i, POST_AFTER, POST_PASSED);
			} else {
				show_post_progress(i, POST_AFTER, POST_FAILED);
				if (test_flags & POST_CRITICAL)
					gd->flags |= GD_FLG_POSTFAIL;
				if (test_flags & POST_STOP)
					gd->flags |= GD_FLG_POSTSTOP;
			}
		} else {
			testResult =(*test->test)(flags);
			if ( testResult == -1) {
				post_log("FAILED\n");
				bootstage_error(BOOTSTAGE_ID_POST_FAIL_R);
				show_post_progress(i, POST_AFTER, POST_FAILED);
				if (test_flags & POST_CRITICAL)
					gd->flags |= GD_FLG_POSTFAIL;
				if (test_flags & POST_STOP)
					gd->flags |= GD_FLG_POSTSTOP;

				return -1;
			}
			else if ( testResult == 2)
			{
				post_log("Not Supported\n");
				return 0;
			}
			else {
				post_log("PASSED\n");
				show_post_progress(i, POST_AFTER, POST_PASSED);
				return 0;
			}
		}

		if ((test_flags & POST_REBOOT) && !(flags & POST_MANUAL))
			post_bootmode_test_off();

		return 0;
	} else {
		return -1;
	}
}

int post_run(char *name, int flags)
{

	unsigned int i;
	int test_flags[POST_MAX_NUMBER];
	int test_results[POST_MAX_NUMBER];
#if (defined(CONFIG_NS_PLUS))
	if ( post_get_board_diags_type() == NO_DIAGS_SUPPORT )
	{
		printf("No diags support defined in the env, need to be");
				"\nBCM953022K\nBCM953025K\nBCM953025K_REV2\nBCM958622HR",
				"\nBCM958623HR\nBCM958522ER\nBCM958625HR\n",
		printf("Please set env DIAGS_BOARD then power cycle the board\n");
		return -1;
	}
#endif


	if ( (flags &  POST_AUTO) || (flags  & POST_SEMI_AUTO ))
	{
	    for ( i = 0; i< POST_MAX_NUMBER ; i++)
	    {
	        test_results[i]= 0x1234;
	    }
#if (defined(CONFIG_NS_PLUS))
	    if ( post_get_board_diags_type() & SVK_BOARDS )
	    {
	    	post_init_strap_ctrl();
	    }
#else
	    //Init strap pin control when running in auto mode
	    post_init_strap_ctrl();
#endif
	}

	post_get_flags(test_flags);

	if (name == NULL) {
		unsigned int last;

		if (gd->flags & GD_FLG_POSTSTOP)
			return 0;

		if (post_bootmode_get(&last) & POST_POWERTEST) {
			if (last & POST_FAIL_SAVE) {
				last &= ~POST_FAIL_SAVE;
				gd->flags |= GD_FLG_POSTFAIL;
			}
			if (last < post_list_size &&
				(flags & test_flags[last] & POST_ALWAYS) &&
				(flags & test_flags[last] & POST_MEM)) {

				post_run_single(post_list + last,
						 test_flags[last],
						 flags | POST_REBOOT, last);

				for (i = last + 1; i < post_list_size; i++) {
					if (gd->flags & GD_FLG_POSTSTOP)
						break;
					post_run_single(post_list + i,
							 test_flags[i],
							 flags, i);
				}
			}
		} else {
			for (i = 0; i < post_list_size; i++) {
				if (gd->flags & GD_FLG_POSTSTOP)
					break;
				if ( (test_flags[i] &  POST_AUTO) || (test_flags[i] & POST_SEMI_AUTO ))
				{
				    test_results[i]= post_run_single(post_list + i,test_flags[i],flags, i);

				}
				else
				{
				    post_run_single(post_list + i,test_flags[i],flags, i);
				}
			}
		}

		for (i = 0; i < post_list_size; i++)
		{
		   if ( (flags & test_flags[i]) & ( POST_AUTO | POST_SEMI_AUTO) )
		   {
               if ( test_results [i] == 0 )
               {
                   post_log("\n%s Passed !",post_list[i].name);
               }
               else if ( test_results[i] == -1 )
               {
                   post_log("\n%s Failed !",post_list[i].name);
               }
               else
               {
            	   post_log("\n%s Skipped!",post_list[i].name);
               }
		   }
		}
		post_log("\n\n");

		return 0;
	} else {
		for (i = 0; i < post_list_size; i++) {
			if (strcmp(post_list[i].cmd, name) == 0)
				break;
		}

		if (i < post_list_size) {
			WATCHDOG_RESET();
			return post_run_single(post_list + i,
						test_flags[i],
						flags, i);
		} else {
			return -1;
		}
	}
}

static int post_info_single(struct post_test *test, int full)
{
	if (test->flags & POST_MANUAL) {
		if (full)
			printf("%s - %s\n"
				"  %s\n", test->cmd, test->name, test->desc);
		else
			printf("  %-15s - %s\n", test->cmd, test->name);

		return 0;
	} else {
		return -1;
	}
}

int post_info(char *name)
{
	unsigned int i;
#if (defined(CONFIG_NS_PLUS))
	if ( post_get_board_diags_type() == NO_DIAGS_SUPPORT )
	{
		printf("Supported boards:BCM953022K/BCM953025K/BCM953025K_REV2/BCM958622HR"
				"/BCM958623HR/BCM958522ER/BCM958625HR\n");
		printf("Set DIAGS_BOARD. saveenv and then power cycle to change the env\n");
	}
#endif
	if (name == NULL) {
		for (i = 0; i < post_list_size; i++)
			post_info_single(post_list + i, 0);

		return 0;
	} else {
		for (i = 0; i < post_list_size; i++) {
			if (strcmp(post_list[i].cmd, name) == 0)
				break;
		}

		if (i < post_list_size)
			return post_info_single(post_list + i, 1);
		else
			return -1;
	}
}
#ifdef IPROC_BOARD_DIAGS
int post_getUserResponse ( const char * const prompt )
{
    int nbytes;
    extern char console_buffer[];

    do
    {
        nbytes = readline(prompt);

        if(nbytes == 1)
        {
            if((console_buffer[0] == 'Y') ||(console_buffer[0] == 'y'))
            {
                return 0;
            }
            else if ((console_buffer[0] == 'N') ||(console_buffer[0] == 'n'))
            {
                return -1;
            }
        }
    }while ( 1 );

}
void post_getConfirmation (const char * const prompt )
{
    int nbytes;
    extern char console_buffer[];

    do
    {
        nbytes = readline(prompt);

        if(nbytes == 1)
        {
            if((console_buffer[0] == 'Y') ||(console_buffer[0] == 'y'))
            {
                return;
            }
        }
    }while ( 1 );
}


int post_getUserInput (const char * const prompt )
{
    int nbytes;
    extern char console_buffer[];

    do
    {
        nbytes = readline(prompt);

        if(nbytes == 1)
        {
        	if(console_buffer[0] == '0')
        		return 0;
            if(console_buffer[0] == '1')
                return 1;
            else if(console_buffer[0] == '2')
                return 2;
            else if(console_buffer[0] == '3')
                return 3;
            else if(console_buffer[0] == '4')
                return 4;
        }
    }while ( 1 );
}


void post_init_strap_ctrl ( void )
{
    printf("\nInitialize the UART 2, com 3 to 19200 baud.Ready for auto test\n");

    serial_init_com8051();

    printf("\nPlease make sure strap pin is set to A4 18  A5 18!!!");
    post_getConfirmation("\nReady? (Y/N)");


}

void post_set_strappins(char *strap )
{
    char chartoSend[10]={0},*p=strap;
    char ch;
    int i,j,k,l;


    i = strlen(strap);

    printf("\n\nSend =>%d chars: %s <== to strap pin control", i, strap);

    k= 0;
    for ( j = 0; j< i ; j++)
    {
        if ( strap[j]=='%')
        {
            strncpy(chartoSend, p,k);
            *(chartoSend+k)='\r';

            //printf("\n=>Sent %s  <==", chartoSend);

            _serial_puts (chartoSend,CONFIG_UART2_INDEX);
            //Send new line for CR
            //_serial_puts ("\n",CONFIG_UART2_INDEX);

            __udelay(10 * 1000);

            p+=k+1;
            k=0;


            //Flush out the echoed chars
            l=0;

            while ( 1 )
            {
                if ( _serial_tstc(CONFIG_UART2_INDEX) != 0 )
                {
                    ch = (char)_serial_getc(CONFIG_UART2_INDEX);
                    string[l]= ch;

                    if ( (string[l] =='>') && (string[l-1] == 'D'))
                    {
                        //printf("\n Got a %c, %c,loc: %d ", string[l],ch, l);
                        break;
                    }
                    //printf("\n %d th ch: %c",j,string[j]);
                    l++;
                }
            }
            //printf("\n==>Echoed chars %s      ==", string );
            memset(string, 0,  200);

            //printf("\n == after set == %s ===", string );

            __udelay(10 * 1000);
        }
        else
        {
            k++;
        }
    }


    _serial_puts ("?\r",CONFIG_UART2_INDEX);
    //_serial_puts ("\n",CONFIG_UART2_INDEX);

    //Flush out the echoed chars
    l=0;

    j = 0;
    while ( j< 1000 )
    {
        if ( _serial_tstc(CONFIG_UART2_INDEX) != 0 )
        {
            ch = (char)_serial_getc(CONFIG_UART2_INDEX);
            string[l]= ch;

            //printf("%c",string[l]);

            //if ( (string[l] =='7') && (string[l-1] == 'A'))
            //{
                //printf("\n Got a %c, %c,loc: %d ", string[l],ch, l);
              //  break;
            //}
            //printf("\n %d th ch: %c",j,string[j]);
            l++;
        }
        else
        {
            __udelay(100);
            j++;
        }
    }
    printf("\n Current strap pin settings \n %s", string);

}
#if (defined(CONFIG_NS_PLUS))
int post_check_board_cfg_env( void )
{
	char name[16]="DIAGS_BOARD";
	char *var;


	/* get port5 config */
	var = getenv(name);
	if (var == NULL)
	{
		/* if no variable defined*/
		puts("\n*********************************************************");
		puts("\nPlease add diag support DIAGS_BOARD to: ");
		puts("\nBCM953022K\nBCM953025K\nBCM953025K_REV2\nBCM958622HR");
		puts("\nBCM958623HR\nBCM958522ER\nBCM958625HR\n");
		puts("in env before you can run any diags                      \n");
		puts("*********************************************************\n");
		return 0;
	}

	if (strcmp(var, "BCM953022K")==0)
	{
		gDiags_board = BCM953022K;
		puts("Found BCM953022K diag support\n");
	}
	else if (strcmp(var, "BCM953025K")==0)
	{
		gDiags_board = BCM953025K;
		puts("Found BCM953025K diag support\n");
	}
	else if (strcmp(var, "BCM953025K_REV2")==0)
	{
		gDiags_board = BCM953025K_REV2;
		puts("Found BCM953025K_REV2 diag support\n");
	}
	else if (strcmp(var, "BCM958622HR")==0)
	{
		gDiags_board = BCM958622HR;
		puts("Found BCM958622HR diag support\n");
	}
	else if (strcmp(var, "BCM958623HR")==0)
	{
		gDiags_board = BCM958623HR;
		puts("Found BCM958623HR diag support\n");
	}
	else if (strcmp(var, "BCM958522ER")==0)
	{
		gDiags_board = BCM958522ER;
		puts("Found BCM958522ER diag support\n");
	}
	else if (strcmp(var, "BCM958625HR")==0)
	{
		gDiags_board = BCM958625HR;
		puts("Found BCM958625HR diag support\n");
	}
	else
	{
		gDiags_board = NO_DIAGS_SUPPORT;
		puts("\n*********************************************************");
		puts("\nPlease add diag support DIAGS_BOARD to:");
		puts("\nBCM953022K\nBCM953025K\nBCM953025K_REV2\nBCM958622HR");
		puts("\nBCM958623HR\nBCM958522ER\nBCM958625HR\n");
		puts("\in env before you can run any diags                      \n");
		puts("*********************************************************\n");
	}
	return 0;
}

int post_get_board_diags_type( void )
{
	return gDiags_board;
}
#endif
#endif

int post_log(char *format, ...)
{
	va_list args;
	char printbuffer[CONFIG_SYS_PBSIZE];

	va_start(args, format);

	/* For this to work, printbuffer must be larger than
	 * anything we ever want to print.
	 */
	vsprintf(printbuffer, format, args);
	va_end(args);

#ifdef CONFIG_LOGBUFFER
	/* Send to the logbuffer */
	logbuff_log(printbuffer);
#else
	/* Send to the stdout file */
	puts(printbuffer);
#endif

	return 0;
}

#ifdef CONFIG_NEEDS_MANUAL_RELOC
void post_reloc(void)
{
	unsigned int i;

	/*
	 * We have to relocate the test table manually
	 */
	for (i = 0; i < post_list_size; i++) {
		ulong addr;
		struct post_test *test = post_list + i;

		if (test->name) {
			addr = (ulong)(test->name) + gd->reloc_off;
			test->name = (char *)addr;
		}

		if (test->cmd) {
			addr = (ulong)(test->cmd) + gd->reloc_off;
			test->cmd = (char *)addr;
		}

		if (test->desc) {
			addr = (ulong)(test->desc) + gd->reloc_off;
			test->desc = (char *)addr;
		}

		if (test->test) {
			addr = (ulong)(test->test) + gd->reloc_off;
			test->test = (int (*)(int flags)) addr;
		}

		if (test->init_f) {
			addr = (ulong)(test->init_f) + gd->reloc_off;
			test->init_f = (int (*)(void)) addr;
		}

		if (test->reloc) {
			addr = (ulong)(test->reloc) + gd->reloc_off;
			test->reloc = (void (*)(void)) addr;

			test->reloc();
		}
	}
}
#endif


/*
 * Some tests (e.g. SYSMON) need the time when post_init_f started,
 * but we cannot use get_timer() at this point.
 *
 * On PowerPC we implement it using the timebase register.
 */
unsigned long post_time_ms(unsigned long base)
{
#ifndef IPROC_BOARD_DIAGS
#if defined(CONFIG_PPC) || defined(CONFIG_BLACKFIN) || defined(CONFIG_ARM)
	return (unsigned long)lldiv(get_ticks(), get_tbclk() / CONFIG_SYS_HZ)
		- base;
#else
#warning "Not implemented yet"
	return 0; /* Not implemented yet */
#endif
#endif
    return 0; /* Not implemented yet */
}
