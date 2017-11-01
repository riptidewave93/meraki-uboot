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

#ifndef __IPROC_PLAT_GPIO_H
#define __IPROC_PLAT_GPIO_H

#define  CCA_CHIPID                                                   0x18000000

#define  CCA_INT_STS                                                  0x18000020
#define  CCA_INT_STS_BASE                                                  0x020
#define  CCA_INT_STS_DATAMASK                                         0xffffffff
#define  CCA_INT_STS_RDWRMASK                                         0x00000000
#define  CCA_INT_MASK                                                 0x18000024
#define  CCA_INT_MASK_BASE                                                 0x024
#define  CCA_INT_MASK_DATAMASK                                        0xffffffff
#define  CCA_INT_MASK_RDWRMASK                                        0x00000000
#define  CCA_CHIP_CTRL                                                0x18000028
#define  CCA_CHIP_CTRL_BASE                                                0x028
#define  CCA_CHIP_CTRL_CTRL_RST_VAL                                   0x00000000
#define  CCA_CHIP_CTRL_DATAMASK                                       0xffffffff
#define  CCA_CHIP_CTRL_RDWRMASK                                       0x00000000
#define  CCA_CHIP_STS                                                 0x1800002c
#define  CCA_CHIP_STS_BASE                                                 0x02c
#define  CCA_CHIP_STS_RST_VAL                                         0x00000000
#define  CCA_CHIP_STS_DATAMASK                                        0xffffffff
#define  CCA_CHIP_STS_RDWRMASK                                        0x00000000



#define  CCA_GPIO_PULL_UP                                             0x18000058
#define  CCA_GPIO_PULL_UP_BASE                                             0x058
#define  CCA_GPIO_PULL_UP_RST_VAL                                     0x00000000
#define  CCA_GPIO_PULL_UP_DATAMASK                                    0xffffffff
#define  CCA_GPIO_PULL_UP_RDWRMASK                                    0x00000000
#define  CCA_GPIO_PULL_DN                                             0x1800005c
#define  CCA_GPIO_PULL_DN_BASE                                             0x05c
#define  CCA_GPIO_PULL_DN_RST_VAL                                     0x00000000
#define  CCA_GPIO_PULL_DN_DATAMASK                                    0xffffffff
#define  CCA_GPIO_PULL_DN_RDWRMASK                                    0x00000000
#define  CCA_GPIO_INPUT                                               0x18000060
#define  CCA_GPIO_INPUT_BASE                                               0x060
#define  CCA_GPIO_INPUT_RST_VAL                                       0x00000000
#define  CCA_GPIO_INPUT_DATAMASK                                      0xffffffff
#define  CCA_GPIO_INPUT_RDWRMASK                                      0x00000000
#define  CCA_GPIO_OUT                                                 0x18000064
#define  CCA_GPIO_OUT_BASE                                                 0x064
#define  CCA_GPIO_OUT_GPIO_OUT_RST_VAL                                0x00000000
#define  CCA_GPIO_OUT_DATAMASK                                        0xffffffff
#define  CCA_GPIO_OUT_RDWRMASK                                        0x00000000
#define  CCA_GPIO_OUT_EN                                              0x18000068
#define  CCA_GPIO_OUT_EN_BASE                                              0x068
#define  CCA_GPIO_OUT_EN_RST_VAL                                      0x00000000
#define  CCA_GPIO_OUT_EN_DATAMASK                                     0xffffffff
#define  CCA_GPIO_OUT_EN_RDWRMASK                                     0x00000000
#define  CCA_GPIO_CTRL                                                0x1800006c
#define  CCA_GPIO_CTRL_BASE                                                0x06c
#define  CCA_GPIO_CTRL_CTRL_RST_VAL                                   0x00000000
#define  CCA_GPIO_CTRL_DATAMASK                                       0xffffffff
#define  CCA_GPIO_CTRL_RDWRMASK                                       0x00000000
#define  CCA_GPIO_INT_POLARITY                                        0x18000070
#define  CCA_GPIO_INT_POLARITY_BASE                                        0x070
#define  CCA_GPIO_INT_POLARITY_RST_VAL                                0x00000000
#define  CCA_GPIO_INT_POLARITY_DATAMASK                               0xffffffff
#define  CCA_GPIO_INT_POLARITY_RDWRMASK                               0x00000000
#define  CCA_GPIOINT_MASK                                             0x18000074
#define  CCA_GPIOINT_MASK_BASE                                             0x074
#define  CCA_GPIOINT_MASK_RST_VAL                                     0x00000000
#define  CCA_GPIOINT_MASK_DATAMASK                                    0xffffffff
#define  CCA_GPIOINT_MASK_RDWRMASK                                    0x00000000
#define  CCA_GPIO_EVT                                                 0x18000078
#define  CCA_GPIO_EVT_BASE                                                 0x078
#define  CCA_GPIO_EVT_GPIO_EVT_RST_VAL                                0x00000000
#define  CCA_GPIO_EVT_DATAMASK                                        0xffffffff
#define  CCA_GPIO_EVT_RDWRMASK                                        0x00000000
#define  CCA_GPIO_EVTINT_MASK                                         0x1800007c
#define  CCA_GPIO_EVTINT_MASK_BASE                                         0x07c
#define  CCA_GPIO_EVTINT_MASK_EVTINT_MASK_RST_VAL                     0x00000000
#define  CCA_GPIO_EVTINT_MASK_DATAMASK                                0xffffffff
#define  CCA_GPIO_EVTINT_MASK_RDWRMASK                                0x00000000
#define  CCA_WDOG_CTR                                                 0x18000080
#define  CCA_WDOG_CTR_BASE                                                 0x080
#define  CCA_WDOG_CTR_COUNTER_RST_VAL                                 0x00000000
#define  CCA_WDOG_CTR_DATAMASK                                        0xffffffff
#define  CCA_WDOG_CTR_RDWRMASK                                        0x00000000
#define  CCA_GPIO_EVT_INT_POLARITY                                    0x18000084
#define  CCA_GPIO_EVT_INT_POLARITY_BASE                                    0x084
#define  CCA_GPIO_EVT_INT_POLARITY_INT_POLARITY_RST_VAL               0x00000000
#define  CCA_GPIO_EVT_INT_POLARITY_DATAMASK                           0xffffffff
#define  CCA_GPIO_EVT_INT_POLARITY_RDWRMASK                           0x00000000
#define  CCA_GPIO_TMR_VAL                                             0x18000088
#define  CCA_GPIO_TMR_VAL_BASE                                             0x088
#define  CCA_GPIO_TMR_VAL_DATAMASK                                    0xffffffff
#define  CCA_GPIO_TMR_VAL_RDWRMASK                                    0x00000000
#define  CCA_GPIO_TMR_OUT_MASK                                        0x1800008c
#define  CCA_GPIO_TMR_OUT_MASK_BASE                                        0x08c
#define  CCA_GPIO_TMR_OUT_MASK_TMR_OUT_MASK_RST_VAL                   0x00000000
#define  CCA_GPIO_TMR_OUT_MASK_DATAMASK                               0xffffffff
#define  CCA_GPIO_TMR_OUT_MASK_RDWRMASK                               0x00000000

#define IPROC_GPIO_REG_SIZE     (0x50)


/* Chipcommon A GPIO */
#define IPROC_GPIO_CCA_BASE CCA_GPIO_INPUT

#define IPROC_GPIO_CCA_DIN              (CCA_GPIO_INPUT_BASE -  CCA_GPIO_INPUT_BASE)
#define IPROC_GPIO_CCA_DOUT             (CCA_GPIO_OUT_BASE - CCA_GPIO_INPUT_BASE)
#define IPROC_GPIO_CCA_EN               (CCA_GPIO_OUT_EN_BASE - CCA_GPIO_INPUT_BASE)
#define IPROC_GPIO_CCA_INT_LEVEL        (CCA_GPIO_INT_POLARITY_BASE - CCA_GPIO_INPUT_BASE)
#define IPROC_GPIO_CCA_INT_LEVEL_MASK   (CCA_GPIOINT_MASK_BASE - CCA_GPIO_INPUT_BASE)
#define IPROC_GPIO_CCA_INT_EVENT        (CCA_GPIO_EVT_BASE - CCA_GPIO_INPUT_BASE)
#define IPROC_GPIO_CCA_INT_EVENT_MASK   (CCA_GPIO_EVTINT_MASK_BASE - CCA_GPIO_INPUT_BASE)
#define IPROC_GPIO_CCA_WATCHDOG_COUNTER (CCA_WDOG_CTR_BASE - CCA_GPIO_INPUT_BASE)
#define IPROC_GPIO_CCA_INT_EDGE         (CCA_GPIO_EVT_INT_POLARITY_BASE - CCA_GPIO_INPUT_BASE)
#define IPROC_GPIO_CCA_TIMER_VAL        (CCA_GPIO_TMR_VAL_BASE - CCA_GPIO_INPUT_BASE)
#define IPROC_GPIO_CCA_TIMEOUT_MASK     (CCA_GPIO_TMR_OUT_MASK_BASE - CCA_GPIO_INPUT_BASE)
#define IPROC_GPIO_CCA_CLK_DIV          (CCA_CLK_DIV_BASE - CCA_GPIO_INPUT_BASE)
#define IPROC_GPIO_CCA_DEBUG            (CCA_GPIODBG_SEL_BASE - CCA_GPIO_INPUT_BASE)

#if defined(CONFIG_NS_PLUS)
#define NORTHSTAR_DMU_BASE 0x1803f000
#else
#define NORTHSTAR_DMU_BASE 0x1800c000
#endif
#define IPROC_GPIO_CCA_PULL_UP          (0x01dc)
#define IPROC_GPIO_CCA_PULL_DOWN        (0x01e0)
#define IPROC_GPIO_CCA_CTRL0            (0x01c0)

#define IPROC_GPIO_CCA_HYSTERESIS       (0x01d4)
#define IPROC_GPIO_CCA_SLEWRATE         (0x01d8)

#define IPROC_GPIO_CCA_DS2              (0x01C4)
#define IPROC_GPIO_CCA_DS1              (0x01C8)
#define IPROC_GPIO_CCA_DS0              (0x01CC)

#define IPROC_GPIO_CCA_INPUT_DISABLE    (0x01D0)




#define REGOFFSET_GPIO_DIN          0x000 /* GPIO Data in register */
#define REGOFFSET_GPIO_DOUT         0x004 /* GPIO Data out register */
#define REGOFFSET_GPIO_EN           0x008 /* GPIO driver enable register */


#define IPROC_GPIO_CCA_ID   (0)
#define IPROC_GPIO_CCB_ID   (1)

#define IPROC_NR_IRQS           (256)
#define IPROC_IRQ_GPIO_0        (IPROC_NR_IRQS)

//copied from C:\iProc\sw\iproc\kernel\linux\arch\arm\plat-iproc\include\mach\irqs.h

//swang ??? documentation is lacking for the GPIO interrupt mapping in GIC
#define IPROC_GPIO_CCA_IRQ_BASE     (IPROC_IRQ_GPIO_0)
#define IPROC_GPIO_CCB_IRQ_BASE     (IPROC_IRQ_GPIO_0 + 32)

#define IPROC_GPIO_CCA_INT          (117)
#define IPROC_CCA_INT_F_GPIOINT     (1)

//#define IPROC_GPIO_CCB_INT          (119)

typedef struct iprocGPIO {
    int id;
    //struct gpio_chip	chip;
    //struct iproc_gpio_cfg	*config;
    void *ioaddr;
    void *intr_ioaddr;
    void *dmu_ioaddr;
    //spinlock_t		 lock;
    int irq_base;
	//struct resource	* resource;
}iproc_gpio_chip;

typedef enum irqTypes
{
	IRQ_TYPE_EDGE_RISING = 1,
	IRQ_TYPE_EDGE_FALLING =2,
	IRQ_TYPE_LEVEL_LOW =4,
	IRQ_TYPE_LEVEL_HIGH =8,
	IRQ_TYPE_EDGE_BOTH = 0x3,
}IRQTYPE;


int iproc_gpio_irq_handler_cca(int irq);

void iprocGPIOInit(void);
iproc_gpio_chip *getGPIODev ( void );
void setGPIOasInput( int pin );
void setGPIOasOutput( int pin, int value );
int getGPIOpinValue ( int pin );
void enableGPIOInterrupt(int pin, IRQTYPE irqtype );
void disableGPIOInterrupt(int pin, IRQTYPE irqtype );
void initGPIOState( void );
void registerGPIOEdgeInterrupt ( void );
void registerGPIOLevelInterrupt ( void );


#endif
