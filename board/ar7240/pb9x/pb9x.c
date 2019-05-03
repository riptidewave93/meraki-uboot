#include <common.h>
#include <command.h>
#include <asm/mipsregs.h>
#include <asm/addrspace.h>
#include <config.h>
#include <version.h>
#include "ar7240_soc.h"

extern void ar7240_ddr_initial_config(uint32_t refresh);
extern int ar7240_ddr_find_size(void);

void
ar7240_usb_initial_config(void)
{
    ar7240_reg_wr_nf(AR7240_USB_PLL_CONFIG, 0x0a04081e);
    ar7240_reg_wr_nf(AR7240_USB_PLL_CONFIG, 0x0804081e);
}

int board_gpio_config(void)
{
    /* Enable SPI on GPIO2-5 and UART on GPIO9-10 */
    ar7240_reg_wr(AR7240_GPIO_FUNC, 0x00048002);
    /* Prepare to set GPIO0, GPIO1, GPIO11-GPIO15, and GPIO17 high */
    ar7240_reg_wr(AR7240_GPIO_OUT, 0x0002f803);
    /* Enable GPIO0, GPIO1, GPIO10-GPIO17 as outputs */
    ar7240_reg_wr(AR7240_GPIO_OE,  0x0003fc03);

    return 0;
}

int
ar7240_mem_config(void)
{
    unsigned int tap_val1, tap_val2;
    ar7240_ddr_initial_config(CFG_DDR_REFRESH_VAL);

    /* Default tap values for starting the tap_init*/
    if (!(is_ar7241() || is_ar7242()))  {
        ar7240_reg_wr (AR7240_DDR_TAP_CONTROL0, 0x8);
        ar7240_reg_wr (AR7240_DDR_TAP_CONTROL1, 0x9);
        ar7240_ddr_tap_init();
    }
    else {
	ar7240_reg_wr (AR7240_DDR_TAP_CONTROL0, 0x2);
        ar7240_reg_wr (AR7240_DDR_TAP_CONTROL1, 0x2);
        ar7240_reg_wr (AR7240_DDR_TAP_CONTROL2, 0x0);
        ar7240_reg_wr (AR7240_DDR_TAP_CONTROL3, 0x0); 
    }

    tap_val1 = ar7240_reg_rd(0xb800001c);
    tap_val2 = ar7240_reg_rd(0xb8000020);

    printf("#### TAP VALUE 1 = 0x%x, 2 = 0x%x [0x%x: 0x%x]\n",
                tap_val1, tap_val2, *(unsigned *)0x80500000,
                *(unsigned *)0x80500004);

    ar7240_usb_initial_config();

    return (ar7240_ddr_find_size());
}

long int initdram(int board_type)
{
    return (ar7240_mem_config());
}

#define GPIO_SIMPLE_LED_0 (1<<11)	//Alex:20100804
#define GPIO_RESET  (1<<8)		//Alex:20100804 

int check_reset_button(void)
{
        if((ar7240_reg_rd(AR7240_GPIO_IN ) & GPIO_RESET) ? 1 : 0)
        {
        printf("RESET is un-pushed\n");
        }
        else    {
        printf("RESET is pushed\n");
	ar7240_reg_rmw_clear(AR7240_GPIO_OUT, GPIO_SIMPLE_LED_0); //Alex:20100916
	udelay(6000000); /*for eth0 stable*/
        run_command("run factory_boot", 1);
        }
        return 0;
}

int checkboard (void)
{
    if ((is_ar7241() || is_ar7242()))
        printf("PB93 (ar7241 - Virian) U-boot\n");
    else
        printf("PB93 (ar7240 - Python) U-boot\n");

    return 0;
}
