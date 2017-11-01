#
# (C) Copyright 2002-2006
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#
include $(TOPDIR)/config.mk

LIB	= libpostdrivers.o

COBJS-$(CONFIG_HAS_POST)	+= PWM_diags.o memory.o NAND_diags.o  QSPI_diags.o
COBJS-$(CONFIG_HAS_POST)	+= GPIO_diags.o UART_diags.o GSIO_SPI_diags.o USB20_diags.o
COBJS-$(CONFIG_HAS_POST)	+= PCIE_diags.o i2c.o MMC_diags.o I2S_diags.o USB30_diags.o
COBJS-$(CONFIG_HAS_POST)	+= I2S_8955_diags.o SPDIF_diags.o RGMII_diags.o voip_diags.o SATA_diags.o 
COBJS-$(CONFIG_HAS_POST)	+= eMMC_diags.o SGMII_diags.o PVTmon_diags.o

include $(TOPDIR)/post/rules.mk
