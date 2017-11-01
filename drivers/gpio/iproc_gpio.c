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
#include <configs/iproc_board.h>
#include <asm/arch/iproc.h>
#include "iproc_gpio.h"




#define readl(addr) (*(volatile unsigned int*)(addr))
#define writew(b,addr) ((*(volatile unsigned int *) (addr)) = (b))

extern int countLevelHigh;
extern int countLevelLow;
extern int countRisingEdge;
extern int countFalliingEdge;
extern int numberOfGPIOs;

extern int receivedINT;

volatile int IntReceived = 0;

static iproc_gpio_chip gpioDev;
static int currentIRQType;

static unsigned int _iproc_gpio_readl(iproc_gpio_chip *chip, int reg)
{
    return readl(chip->ioaddr + reg);
}

static void _iproc_gpio_writel(iproc_gpio_chip *chip, unsigned int val, int reg)
{
    writew(val, chip->ioaddr + reg);
}


static int iproc_gpio_to_irq(iproc_gpio_chip *chip, unsigned int pin) {
    return (chip->irq_base + pin);
}

static int iproc_irq_to_gpio(iproc_gpio_chip *chip, unsigned int irq) {
    return (irq - chip->irq_base);
}


static void iproc_gpio_irq_unmask(unsigned int irq, IRQTYPE irq_type)
{
    iproc_gpio_chip *ourchip = &gpioDev;
    int pin;
    unsigned int int_mask;

    pin = iproc_irq_to_gpio(ourchip, irq);
    //irq_type = IRQ_TYPE_EDGE_BOTH;//for now swang //irq_desc[irq].status & IRQ_TYPE_SENSE_MASK;

    if (ourchip->id == IPROC_GPIO_CCA_ID) {
        unsigned int  event_mask; 

        event_mask = _iproc_gpio_readl(ourchip, IPROC_GPIO_CCA_INT_EVENT_MASK);
        int_mask = _iproc_gpio_readl(ourchip, IPROC_GPIO_CCA_INT_LEVEL_MASK);

        if (irq_type & IRQ_TYPE_EDGE_BOTH) {
            event_mask |= 1 << pin;
            _iproc_gpio_writel(ourchip, event_mask, 
                IPROC_GPIO_CCA_INT_EVENT_MASK);
        } else {
            int_mask |= 1 << pin;
            _iproc_gpio_writel(ourchip, int_mask, 
                IPROC_GPIO_CCA_INT_LEVEL_MASK);
        }
    }

}
static void iproc_gpio_irq_mask(unsigned int irq, IRQTYPE irq_type)
{
    iproc_gpio_chip *ourchip = &gpioDev;
    int pin;
    unsigned int int_mask;


    pin = iproc_irq_to_gpio(ourchip, irq);
    //irq_type = IRQ_TYPE_EDGE_BOTH; //for now swang ???//irq_desc[irq].status & IRQ_TYPE_SENSE_MASK;

    if (ourchip->id == IPROC_GPIO_CCA_ID) {
        unsigned int  event_mask;
        
        event_mask = _iproc_gpio_readl(ourchip, IPROC_GPIO_CCA_INT_EVENT_MASK);
        int_mask = _iproc_gpio_readl(ourchip, IPROC_GPIO_CCA_INT_LEVEL_MASK);

        if (irq_type & IRQ_TYPE_EDGE_BOTH) {
            event_mask &= ~(1 << pin);
            _iproc_gpio_writel(ourchip, event_mask,
                IPROC_GPIO_CCA_INT_EVENT_MASK);
        } else {
            int_mask &= ~(1 << pin);
            _iproc_gpio_writel(ourchip, int_mask, 
                IPROC_GPIO_CCA_INT_LEVEL_MASK);
        }
    }
}

static int iproc_gpio_irq_set_type(unsigned int irq, IRQTYPE type)
{
    iproc_gpio_chip *ourchip = &gpioDev;
    int pin;    

    currentIRQType = type;

    pin = iproc_irq_to_gpio(ourchip, irq);

    if (ourchip->id == IPROC_GPIO_CCA_ID) {
        unsigned int  event_pol, int_pol;

        switch (type) {
        case IRQ_TYPE_EDGE_RISING:
            event_pol = _iproc_gpio_readl(ourchip, IPROC_GPIO_CCA_INT_EDGE);
            event_pol &= ~(1 << pin);
            _iproc_gpio_writel(ourchip, event_pol, IPROC_GPIO_CCA_INT_EDGE);
            break;
        case IRQ_TYPE_EDGE_FALLING:
            event_pol = _iproc_gpio_readl(ourchip, IPROC_GPIO_CCA_INT_EDGE);
            event_pol |= (1 << pin);
            _iproc_gpio_writel(ourchip, event_pol, IPROC_GPIO_CCA_INT_EDGE);
            break;
        case IRQ_TYPE_LEVEL_HIGH:
            int_pol = _iproc_gpio_readl(ourchip, IPROC_GPIO_CCA_INT_LEVEL);
            int_pol &= ~(1 << pin);
            _iproc_gpio_writel(ourchip, int_pol, IPROC_GPIO_CCA_INT_LEVEL);
            break;
        case IRQ_TYPE_LEVEL_LOW:
            int_pol = _iproc_gpio_readl(ourchip,IPROC_GPIO_CCA_INT_LEVEL);
            int_pol |= (1 << pin);
            _iproc_gpio_writel(ourchip, int_pol, IPROC_GPIO_CCA_INT_LEVEL);
            break;
        default:
            //printf(KERN_ERR "unsupport irq type !\n");
            return -1;
        }
    }
    return 0;
}


void  iproc_gpio_irq_handler(void *data)

{
    unsigned int  val;
    unsigned int  int_mask, event_pol, int_pol, in;
    unsigned int  event_mask, event,tmp = 0;
    int iter;
    iproc_gpio_chip *ourchip = &gpioDev;


    val = readl(ourchip->intr_ioaddr + CCA_INT_STS_BASE);
    
    if (val & IPROC_CCA_INT_F_GPIOINT)
    {

        //Disable GPIO interrupt at top level
        tmp = readl(gpioDev.intr_ioaddr + CCA_INT_MASK_BASE);
        writew ( (tmp & ~IPROC_CCA_INT_F_GPIOINT),gpioDev.intr_ioaddr + CCA_INT_MASK_BASE);

        int_mask = _iproc_gpio_readl(ourchip, IPROC_GPIO_CCA_INT_LEVEL_MASK);
        int_pol = _iproc_gpio_readl(ourchip, IPROC_GPIO_CCA_INT_LEVEL);
        in = _iproc_gpio_readl(ourchip, IPROC_GPIO_CCA_DIN);
        event_mask = _iproc_gpio_readl(ourchip, IPROC_GPIO_CCA_INT_EVENT_MASK);
        event = _iproc_gpio_readl(ourchip, IPROC_GPIO_CCA_INT_EVENT);
        event_pol = _iproc_gpio_readl(ourchip, IPROC_GPIO_CCA_INT_EDGE);

        //24/32is the number of GPIOs
        for (iter = 0; iter < numberOfGPIOs; iter ++)
        {
           if ( (IRQ_TYPE_EDGE_FALLING == currentIRQType) ||
                (IRQ_TYPE_EDGE_RISING == currentIRQType))
           {
            tmp = event_mask;
            tmp &= event;
            // If there is an event interrupt ( edge triggered )
            if ( tmp & ( 1 << iter ))
            {
                if ((~event_pol)&(1 << iter))
                {
                    countRisingEdge |= 1 << iter;
                    puts("\nRising edge interrupt");
                    receivedINT = iter;
                }
                else
                {
                    countFalliingEdge |= 1 << iter;
                    puts("\nFalling edge interrupt");
                    receivedINT = iter;
                }
                IntReceived = 1;
                //Clear the edge interrupt
                *(volatile unsigned long*)(CCA_GPIO_EVT) |=(1<<iter);
                *(volatile unsigned long*)(CCA_GPIO_EVTINT_MASK) &=~(1<<iter);
            }
           }
           else
           {
               tmp = in ^ int_pol;
               tmp &= int_mask;

               if ( tmp & (1 << iter))
               {
                   if ( int_pol & ( 1 << iter ))
                   {
                       countLevelLow |= 1 << iter;
                       //puts("\nLevel low interrupt");

                   }
                   else
                   {
                       countLevelHigh |= 1 << iter;
                       //puts("\nLevel high interrupt");

                   }
                   *(volatile unsigned long*)(CCA_GPIOINT_MASK) &=~(1<<iter);
                   IntReceived = 1;
               }
           }

        }
        //Enable GPIO interrupt at top level
        tmp = readl(gpioDev.intr_ioaddr + CCA_INT_MASK_BASE);
        writew ( (tmp |IPROC_CCA_INT_F_GPIOINT),gpioDev.intr_ioaddr + CCA_INT_MASK_BASE);

    }

    return ;
}

//set a GPIO pin as an input
static int iproc_gpiolib_input(iproc_gpio_chip *ourchip, unsigned gpio)
{
    unsigned int  val;
    unsigned int  nBitMask = 1 << gpio;


    //iproc_gpio_lock(ourchip, flags);

    val = _iproc_gpio_readl(ourchip, REGOFFSET_GPIO_EN);
    val &= ~nBitMask;
    _iproc_gpio_writel(ourchip, val, REGOFFSET_GPIO_EN);

    //iproc_gpio_unlock(ourchip, flags);
    return 0;
}

//set a gpio pin as a output
static int iproc_gpiolib_output(iproc_gpio_chip *ourchip,
                  unsigned gpio, int value)
{


    unsigned long val;
    unsigned int  nBitMask = 1 << gpio;

    //iproc_gpio_lock(ourchip, flags);

    val = _iproc_gpio_readl(ourchip, REGOFFSET_GPIO_EN);
    val |= nBitMask;
    _iproc_gpio_writel(ourchip, val, REGOFFSET_GPIO_EN);

    //iproc_gpio_unlock(ourchip, flags);
    return 0;
}

//set the gpio pin ( by gpio ) to value ( only for output, input is read only
static void iproc_gpiolib_set(iproc_gpio_chip *ourchip,
                unsigned gpio, int value)
{


    unsigned long val;
    unsigned int  nBitMask = 1 << gpio;

    //iproc_gpio_lock(ourchip, flags);


    /* determine the GPIO pin direction 
     */ 
    val = _iproc_gpio_readl(ourchip, REGOFFSET_GPIO_EN);
    val &= nBitMask;

    /* this function only applies to output pin
     */ 
    if (!val)
        return;

    val = _iproc_gpio_readl(ourchip, REGOFFSET_GPIO_DOUT);

    if ( value == 0 ){
        /* Set the pin to zero */
        val &= ~nBitMask;
    }else{
        /* Set the pin to 1 */
        val |= nBitMask;
    }    
    _iproc_gpio_writel(ourchip, val, REGOFFSET_GPIO_DOUT);

    //iproc_gpio_unlock(ourchip, flags);

}

//get the gpio pin value
static int iproc_gpiolib_get(iproc_gpio_chip *ourchip, unsigned gpio)
{


    unsigned int val, offset;
    unsigned int  nBitMask = 1 << gpio;    

    //iproc_gpio_lock(ourchip, flags);
    /* determine the GPIO pin direction 
     */ 
    offset = _iproc_gpio_readl(ourchip, REGOFFSET_GPIO_EN);
    offset &= nBitMask;

    //output pin
    if (offset){
        val = _iproc_gpio_readl(ourchip, REGOFFSET_GPIO_DOUT);
    }
    //input pin
    else {
        val = _iproc_gpio_readl(ourchip, REGOFFSET_GPIO_DIN);    
    }
    val >>= gpio;
    val &= 1;
    //printf("\nGPIO %d, input value reg = 0x%x,,value= %d ,offset %d", gpio, *((unsigned long*)0x18000060),val, offset);
    //iproc_gpio_unlock(ourchip, flags);

    return val;
}

void iprocGPIOInit( void )
{
    gpioDev.id = IPROC_GPIO_CCA_ID;
    gpioDev.ioaddr = (void*)IPROC_GPIO_CCA_BASE;
    gpioDev.dmu_ioaddr = (void*)NORTHSTAR_DMU_BASE;
    gpioDev.intr_ioaddr =(void*)CCA_CHIPID ;
    gpioDev.irq_base = IPROC_GPIO_CCA_IRQ_BASE;
}

iproc_gpio_chip *getGPIODev ( void )
{
    return &gpioDev;
}

void setGPIOasInput( int pin )
{
    if ( (pin < numberOfGPIOs ) && ( pin >= 0 ))
    {
        iproc_gpiolib_input(&gpioDev, pin );
    }
}
void setGPIOasOutput( int pin, int value )
{
    if ( (pin < numberOfGPIOs ) && ( pin >= 0 ))
    {
        //Set the pin to be output
        iproc_gpiolib_output(&gpioDev,pin,value);
        //set value
        iproc_gpiolib_set(&gpioDev,pin,value);
    }
}
int getGPIOpinValue ( int pin )
{
    if ( (pin < numberOfGPIOs ) && ( pin >= 0 ))
    {
        return iproc_gpiolib_get(&gpioDev, pin );
    }
    return -1;
}
void enableGPIOInterrupt(int pin, IRQTYPE irqtype )
{
    unsigned int irq;
    unsigned long temp;



    if ( (pin < numberOfGPIOs ) && ( pin >= 0 ))
    {
        //Map pin to IRQ number
        irq = iproc_gpio_to_irq(&gpioDev,pin);
        iproc_gpio_irq_set_type(irq,irqtype);
        iproc_gpio_irq_unmask(irq, irqtype);

        //while ( tempDelay < 0xFFFF )
        //{
        //    tempDelay++;
        //}
        //post_log("\n Enable Interrupt");
        //Enable GPIO interrupt at top level
        temp = readl(gpioDev.intr_ioaddr + CCA_INT_MASK_BASE);
        writew ( (temp |IPROC_CCA_INT_F_GPIOINT),gpioDev.intr_ioaddr + CCA_INT_MASK_BASE);
    }

}
void disableGPIOInterrupt(int pin, IRQTYPE irqtype )
{
    unsigned int irq;
    unsigned long temp;

    if ( (pin < numberOfGPIOs ) && ( pin >= 0 ))
    {
        //Map pin to IRQ number
        irq = iproc_gpio_to_irq(&gpioDev,pin);
        //iproc_gpio_irq_set_type(irq,irqtype);
        iproc_gpio_irq_mask(irq,irqtype);

        //Disable GPIO interrupt at top level
        temp = readl(gpioDev.intr_ioaddr + CCA_INT_MASK_BASE);
        writew ( (temp & ~IPROC_CCA_INT_F_GPIOINT),gpioDev.intr_ioaddr + CCA_INT_MASK_BASE);
    }

}
void initGPIOState ( void )
{
    _iproc_gpio_writel(&gpioDev, 0xFFFFFF, IPROC_GPIO_CCA_INT_EVENT);

    _iproc_gpio_writel(&gpioDev, 0x000000, IPROC_GPIO_CCA_INT_LEVEL);

    _iproc_gpio_writel(&gpioDev, 0x000000, IPROC_GPIO_CCA_INT_LEVEL_MASK);

    _iproc_gpio_writel(&gpioDev, 0x000000, IPROC_GPIO_CCA_INT_EVENT_MASK);

    _iproc_gpio_writel(&gpioDev, 0x000000, IPROC_GPIO_CCA_INT_EDGE);

}
void registerGPIOEdgeInterrupt ( void )
{
    disable_interrupts();
    gic_disable_interrupt(117);
    irq_install_handler(117,iproc_gpio_irq_handler,NULL );
    gic_config_interrupt(117, 1, IPROC_INTR_EDGE_TRIGGERED, 0, IPROC_GIC_DIST_IPTR_CPU0);
    enable_interrupts();
}
void registerGPIOLevelInterrupt ( void )
{
    disable_interrupts();
    gic_disable_interrupt(117);
    irq_install_handler(117,iproc_gpio_irq_handler,NULL );
    gic_config_interrupt(117, 1, IPROC_INTR_LEVEL_SENSITIVE, 0, IPROC_GIC_DIST_IPTR_CPU0);
    enable_interrupts();
}
