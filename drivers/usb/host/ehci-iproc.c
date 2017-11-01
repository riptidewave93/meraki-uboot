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
#include <pci.h>
#include <usb.h>
#include <asm/io.h>

#include "ehci.h"
#include "ehci-core.h"

#if (defined(CONFIG_NS_PLUS) || defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2))

#define IPROC_USB2_CLK_CONTROL_ENABLE	  (0x1803f180)
#define IPROC_USB2_CLK_CONTROL_PLL		  (0x1803f164)
#define IPROC_STRAP_SKU_VECTOR			  (0x1811A500)
#define IPROC_IDM_USB2_RESET_CONTROL	  (0x18115800)
#define IPROC_EHCI                   	  (0x1802a000)
#define USB2_IDM_IDM_IO_CONTROL_DIRECT    (0x18115408)

#define IPROC_SKU_STRAP_MASK 0xC

#else
#define IPROC_USB2_CLK_CONTROL_ENABLE	  (0x1800C180)
#define IPROC_USB2_CLK_CONTROL_PLL		  (0x1800C164)
#define IPROC_STRAP_SKU_VECTOR			  (0x1810D500)
#define IPROC_IDM_USB2_RESET_CONTROL	  (0x18115800)
#define IPROC_EHCI                   	  (0x18021000)

#define IPROC_SKU_STRAP_MASK 0xC
#endif



#if defined(CONFIG_HELIX4)
#define IPROC_WRAP_USBPHY_CTRL	0x1803fc34
#endif

#if defined(CONFIG_KATANA2)
#define IPROC_WRAP_USBPHY_CTRL	0x1803fc20
#endif

#if (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2))
#define IPROC_IDM_USB2_IO_CONTROL_DIRECT	0x18115408
#define IPROC_XGPLL							0x1803fc2c
#define IPROC_USB_PHY_CTRL					IPROC_WRAP_USBPHY_CTRL
#define IPROC_CLK_NDIV_40					0x80
#define IPROC_CLK_NDIV_20					0x8C
#define USB_CLK_NDIV_MASK					0xFE7FFE00
#define USB_CLK_PLL_RESET_MASK				0xFF7FFE00
#define USB_CLK_PHY_RESET_MASK				0xFFFFFE00
#define USB_CLK_NDIV_40						0x30
#define USB_CLK_NDIV_20						0x60
#define SUPPLY_USBD_POWER					0xfffffffd
#define ChipcommonA_GPIOOut					0x18000064
#define ChipcommonA_GPIOOutEn				0x18000068

/*
 * Function to initialize USB host related low level hardware including PHY,
 * clocks, etc.
 *
 * TODO: expand support for more than one host in the future if needed
 */
int linux_usbh_init(void)
{
	int clk_enable;
	unsigned int iClk, USBClk, usbdgpiopwr, usb2_reset_state;

	/* Do USB PHY reset */
	USBClk = __raw_readl(IPROC_USB_PHY_CTRL);
	__raw_writel(USBClk & (~(1<<23)), IPROC_USB_PHY_CTRL);
	udelay(100);

	/* enable clock */
	clk_enable = __raw_readl(IPROC_IDM_USB2_IO_CONTROL_DIRECT);
	printf("Initial usb2h clock is: %08x\n", clk_enable);
	clk_enable |= 1;
	printf("Initial usb2h clock is: %08x\n", clk_enable);
	__raw_writel(clk_enable, IPROC_IDM_USB2_IO_CONTROL_DIRECT);

#if defined(CONFIG_HELIX4)
	/* set USB clock to configured */
	iClk = __raw_readl(IPROC_XGPLL);
	USBClk = __raw_readl(IPROC_USB_PHY_CTRL);
	printf("iClk = %08x, USBClk = %08x\n", iClk, USBClk);
	if ((iClk & 0xff) == IPROC_CLK_NDIV_40)
	{
		__raw_writel((USBClk & USB_CLK_NDIV_MASK) | USB_CLK_NDIV_40, IPROC_USB_PHY_CTRL);
		udelay(10);
		__raw_writel((USBClk & USB_CLK_PLL_RESET_MASK) | USB_CLK_NDIV_40, IPROC_USB_PHY_CTRL);
		udelay(10);
		__raw_writel((USBClk & USB_CLK_PHY_RESET_MASK) | USB_CLK_NDIV_40, IPROC_USB_PHY_CTRL);
		udelay(10);
		USBClk = __raw_readl(IPROC_USB_PHY_CTRL);
		printf("iClk = %08x, USBClk = %08x\n", iClk, USBClk);
	}
	else if ((iClk & 0xff) == IPROC_CLK_NDIV_20)
	{
		__raw_writel((USBClk & USB_CLK_NDIV_MASK) | USB_CLK_NDIV_20, IPROC_USB_PHY_CTRL);
		udelay(10);
		__raw_writel((USBClk & USB_CLK_PLL_RESET_MASK) | USB_CLK_NDIV_20, IPROC_USB_PHY_CTRL);
		udelay(10);
		__raw_writel((USBClk & USB_CLK_PHY_RESET_MASK) | USB_CLK_NDIV_20, IPROC_USB_PHY_CTRL);
		udelay(10);
		USBClk = __raw_readl(IPROC_USB_PHY_CTRL);
		printf("iClk = %08x, USBClk = %08x\n", iClk, USBClk);
	}
#endif

	/* bring USB PHY out of reset */
	__raw_writel(USBClk | (1<<23), IPROC_USB_PHY_CTRL);
	udelay(100);

    printf("\nBring usb2h_out of reset.......\n");
    __raw_writel(0x0, IPROC_IDM_USB2_RESET_CONTROL);
    udelay(100000);
    usb2_reset_state = __raw_readl(IPROC_IDM_USB2_RESET_CONTROL);
    printf("usb2_reset_state is set and now it is: %08x\n", usb2_reset_state);

#if (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2))
	/* supply power for USB device connected to the host */
	usbdgpiopwr = __raw_readl(ChipcommonA_GPIOOut);
	usbdgpiopwr &= SUPPLY_USBD_POWER;
	__raw_writel(usbdgpiopwr, ChipcommonA_GPIOOut);
	__raw_writel(0x2, ChipcommonA_GPIOOutEn);
#endif
	return 0;
}

#endif

/*
 * Function to initialize USB host related low level hardware including PHY,
 * clocks, etc.
 *
 * TODO: expand support for more than one host in the future if needed
 */

int ehci_hcd_init(void)
{
#if (defined(CONFIG_NS_PLUS) || defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2))

	int usb2_clk_cntrl, usb2_clk_enable, sku_vect, usb2_reset_state;

    /* Start clock */
	sku_vect = __raw_readl(IPROC_STRAP_SKU_VECTOR);

    //usb2_clk_enable = __raw_readl(IPROC_USB2_CLK_CONTROL_PLL);

#if defined(CONFIG_NS_PLUS)
	usb2_clk_enable = __raw_readl(USB2_IDM_IDM_IO_CONTROL_DIRECT);
    printf("Before reset, USB clk enable register is: %08x\n", usb2_clk_enable);
    usb2_clk_enable |= 0x1;
    __raw_writel( usb2_clk_enable, USB2_IDM_IDM_IO_CONTROL_DIRECT);
    usb2_clk_enable = __raw_readl(USB2_IDM_IDM_IO_CONTROL_DIRECT);
    printf("Before reset, USB clk enable register is: %08x\n", usb2_clk_enable);
    udelay(1000);
#else
	linux_usbh_init();
#endif

    /* Reset USBH 2.0 core */
    //__raw_writel(0x1, IPROC_IDM_USB2_RESET_CONTROL);
    usb2_reset_state = __raw_readl(IPROC_IDM_USB2_RESET_CONTROL);
	printf("usb2_reset_state is: %08x\n", usb2_reset_state);
	if ((usb2_reset_state & 1) == 1)
	{
		printf("\nBring usb2h_out of reset.......\n");
		__raw_writel(0x0, IPROC_IDM_USB2_RESET_CONTROL);
		udelay(1000);
		usb2_reset_state = __raw_readl(IPROC_IDM_USB2_RESET_CONTROL);
		printf("usb2_reset_state is set and now it is: %08x\n", usb2_reset_state);
	}

    //usb2_clk_enable = __raw_readl(IPROC_USB2_CLK_CONTROL_PLL);
    //printf("After reset, USB clk enable register is: %08x\n", usb2_clk_enable);

    /* Take out PHY? */
    //printf("\nusb2, take PHY out of reset\n");
    //__raw_writel(0x3ff, 0x18021200);
    //__raw_writel(0x3ff, 0x18021204);

    /* map registers */
    hccr = (struct ehci_hccr *)(IPROC_EHCI);
    hcor = (struct ehci_hcor *)((uint32_t) hccr +
            HC_LENGTH(ehci_readl(&hccr->cr_capbase)));

#else
	int usb2_clk_cntrl, usb2_clk_enable, sku_vect, usb2_reset_state;

    /* Start clock */
	sku_vect = __raw_readl(IPROC_STRAP_SKU_VECTOR);
	if ((sku_vect & IPROC_SKU_STRAP_MASK) != 0x0)
	{
		/* enable clocks */
		__raw_writel(0xEA68, IPROC_USB2_CLK_CONTROL_ENABLE);
		usb2_clk_cntrl = __raw_readl(IPROC_USB2_CLK_CONTROL_ENABLE);
		printf("USB clk control enable register is: %08x\n", usb2_clk_cntrl);

		__raw_writel(0xDD10F3, IPROC_USB2_CLK_CONTROL_PLL);
		usb2_clk_enable = __raw_readl(IPROC_USB2_CLK_CONTROL_PLL);
		printf("USB clk enable register is: %08x\n", usb2_clk_enable);

		__raw_writel(0x0, IPROC_USB2_CLK_CONTROL_ENABLE);
		usb2_clk_cntrl = __raw_readl(IPROC_USB2_CLK_CONTROL_ENABLE);
		printf("USB clk control enable register is: %08x\n", usb2_clk_cntrl);
	}

    usb2_clk_enable = __raw_readl(IPROC_USB2_CLK_CONTROL_PLL);
    //printf("Before reset, USB clk enable register is: %08x\n", usb2_clk_enable);

    /* Reset USBH 2.0 core */
    __raw_writel(0x1, IPROC_IDM_USB2_RESET_CONTROL);
    printf("\nusb2h_reset.......\n");
    __raw_writel(0x0, IPROC_IDM_USB2_RESET_CONTROL);
    usb2_reset_state = __raw_readl(IPROC_IDM_USB2_RESET_CONTROL);
    printf("usb2_reset_state is set and now it is: %08x\n", usb2_reset_state);

    usb2_clk_enable = __raw_readl(IPROC_USB2_CLK_CONTROL_PLL);
    //printf("After reset, USB clk enable register is: %08x\n", usb2_clk_enable);

    /* Take out PHY? */
    //printf("\nusb2, take PHY out of reset\n");
    //__raw_writel(0x3ff, 0x18021200);
    //__raw_writel(0x3ff, 0x18021204);

    /* map registers */
    hccr = (struct ehci_hccr *)(IPROC_EHCI);
    hcor = (struct ehci_hcor *)((uint32_t) hccr +
            HC_LENGTH(ehci_readl(&hccr->cr_capbase)));
#endif
    return 0;
}

/*
 * Function to terminate USB host related low level hardware including PHY,
 * clocks, etc.
 *
 * TODO: expand support for more than one host in the future if needed
 */
int ehci_hcd_stop(void)
{
	return 0;
}
