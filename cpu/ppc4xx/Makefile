#
# (C) Copyright 2000-2006
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

LIB	= $(obj)lib$(CPU).a

START	:= resetvec.o
START	+= start.o

SOBJS	:= cache.o
SOBJS	+= dcr.o
SOBJS	+= kgdb.o

ifndef CONFIG_APM86XXX
COBJS-y	:= 40x_spd_sdram.o
COBJS-y	+= 44x_spd_ddr.o
COBJS-y	+= 44x_spd_ddr2.o
ifdef CONFIG_PPC4xx_DDR_AUTOCALIBRATION
COBJS-y	+= 4xx_ibm_ddr2_autocalib.o
endif
COBJS-y	+= 4xx_pci.o
COBJS-y	+= 4xx_pcie.o
COBJS-y	+= i2c.o
endif
ifdef CONFIG_CMD_CHIP_CONFIG
COBJS-y	+= cmd_chip_config.o
endif
COBJS-y	+= commproc.o
COBJS-y	+= cpu.o
COBJS-y	+= cpu_init.o
ifndef CONFIG_APM86XXX
COBJS-y	+= bedbug_405.o
COBJS-y	+= denali_data_eye.o
COBJS-y	+= denali_spd_ddr2.o
COBJS-y	+= iop480_uart.o
endif
COBJS-y	+= ecc.o
COBJS-y	+= fdt.o
COBJS-y	+= interrupts.o
ifdef CONFIG_CMD_REGINFO
COBJS-y	+= reginfo.o
endif
COBJS-y	+= sdram.o
COBJS-y	+= speed.o
COBJS-y	+= tlb.o
COBJS-y	+= traps.o
ifndef CONFIG_NO_SUB
COBJS-y	+= usb.o
COBJS-y	+= usb_ohci.o
COBJS-y	+= usbdev.o
endif
ifdef CONFIG_MP
COBJS-y	+= mp.o
endif  
ifndef CONFIG_XILINX_440
COBJS-y	+= 4xx_uart.o
ifndef CONFIG_APM86XXX
COBJS-y	+= gpio.o
COBJS-y	+= miiphy.o
COBJS-y	+= uic.o
endif
else
COBJS-y	+= xilinx_irq.o
endif

ifdef CONFIG_APM86XXX
ifdef CONFIG_MEDIA_U_BOOT
COBJS-y	+= apm_ipp_booting.o
SOBJS   += apm_ipp_helper.o
endif 
COBJS-y	+= apm_spd_ddr.o
COBJS-y	+= apm_sw_ddrcal.o
COBJS-y	+= apm_mosys.o
COBJS-y	+= apm_pcie.o
COBJS-y += apm_mpic.o
COBJS-y += apm_scu.o
COBJS-y += apm_cpufreq.o
# Mamba QM
COBJS-$(CONFIG_APM86xxx_ENET)	+= apm_qm_access.o
COBJS-$(CONFIG_APM86xxx_ENET)	+= apm_qm_core.o
COBJS-$(CONFIG_CMD_APM86xxx_ENET) += apm_qm_utils.o

#Mamba Classifier
COBJS-$(CONFIG_APM86xxx_ENET)	+= apm_cle_access.o
COBJS-$(CONFIG_APM86xxx_ENET)	+= apm_preclass_base.o
COBJS-$(CONFIG_APM86xxx_ENET)	+= apm_preclass_api.o

# Mamba ETH
COBJS-$(CONFIG_APM86xxx_ENET)	+= apm_enet.o
COBJS-$(CONFIG_APM86xxx_ENET)	+= apm_enet_mac.o
COBJS-$(CONFIG_APM86xxx_ENET)	+= apm_enet_misc.o
COBJS-$(CONFIG_APM86xxx_ENET)	+= apm_cle_cfg.o

endif

COBJS   := $(COBJS-y)
SRCS    := $(START:.o=.S) $(SOBJS:.o=.S) $(COBJS:.o=.c)
OBJS    := $(addprefix $(obj),$(SOBJS) $(COBJS))
START   := $(addprefix $(obj),$(START))

all:    $(obj).depend $(START) $(LIB)

$(LIB): $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
