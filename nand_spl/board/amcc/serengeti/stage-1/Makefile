#
# Copyright (c) 2010 Applied Micro Circuits Corporation.
# All rights reserved. Feng Kan <fkan@apm.com>.
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
include $(TOPDIR)/nand_spl/board/$(BOARDDIR)/stage-1/config.mk

LDSCRIPT= $(TOPDIR)/nand_spl/board/$(BOARDDIR)/stage-1/u-boot.lds
LDFLAGS	= -Bstatic -T $(nandobj)u-boot.lds -Ttext $(TEXT_BASE) $(PLATFORM_LDFLAGS)
AFLAGS	+= -DCONFIG_NAND_SPL 
AFLAGS  += -DCONFIG_FIXED_DDR_INIT
#AFLAGS	+= -DCONFIG_NOT_COHERENT_CACHE 
AFLAGS  += -UCONFIG_MP
#AFLAGS  += -DCONFIG_FULL_FAM
CFLAGS	+= -DCONFIG_NAND_SPL 
CFLAGS	+= -DCONFIG_NAND_SPL_S1
AFLAGS	+= -DCONFIG_NAND_SPL_S1
CFLAGS  += -DCONFIG_FIXED_DDR_INIT 
#CFLAGS	+= -DCONFIG_NOT_COHERENT_CACHE 
CFLAGS  += -UCONFIG_MP 

SOBJS	:= start.o 
SOBJS	+= resetvec.o 
SOBJS	+= init.o
SOBJS   += ppcstring.o
# COBJS	:= apm_scu.o
# COBJS	+= apm_spd_ddr.o
COBJS	+= nand_boot_apm_ufc.o
COBJS	+= ufc.o
#COBJS   += bch.o
# COBJS   += nand_support.o

SRCS	:= $(addprefix $(obj),$(SOBJS:.o=.S) $(COBJS:.o=.c))
OBJS	:= $(addprefix $(obj),$(SOBJS) $(COBJS))
__OBJS	:= $(SOBJS) $(COBJS)
LNDIR	:= $(OBJTREE)/nand_spl/board/$(BOARDDIR)/stage-1

nandobj	:= $(OBJTREE)/nand_spl/board/$(BOARDDIR)/stage-1/

ALL	= $(nandobj)u-boot-spl $(nandobj)u-boot-spl.bin $(nandobj)u-boot-spl.disasm $(nandobj)u-boot-spl.bin.swp

all:	$(obj).depend $(ALL)

$(nandobj)u-boot-spl.disasm:	$(nandobj)u-boot-spl
		$(OBJDUMP) -D $< > $@

$(nandobj)u-boot-spl.bin.swp: $(nandobj)u-boot-spl.bin
	byteswap -o $(@) 4 $(^)

$(nandobj)u-boot-spl.bin:	$(nandobj)u-boot-spl
	$(OBJCOPY) ${OBJCFLAGS} -O binary $< $@

$(nandobj)u-boot-spl:	$(OBJS) $(nandobj)u-boot.lds
	cd $(LNDIR) && $(LD) $(LDFLAGS) $(__OBJS) $(PLATFORM_LIBS) \
		-Map $(nandobj)u-boot-spl.map \
		-o $(nandobj)u-boot-spl

$(nandobj)u-boot.lds: $(LDSCRIPT)
	$(CPP) $(CPPFLAGS) $(LDPPFLAGS) -ansi -D__ASSEMBLY__ -P - <$^ >$@

# create symbolic links for common files

# from cpu directory
# $(obj)apm_spd_ddr.c: $(obj)ecc.h
#         @rm -f $(obj)apm_spd_ddr.c
#         ln -s $(SRCTREE)/cpu/ppc4xx/apm_spd_ddr.h $(obj)apm_spd_ddr.h
#         ln -s $(SRCTREE)/cpu/ppc4xx/apm_spd_ddr.c $(obj)apm_spd_ddr.c

# $(obj)apm_scu.c:
#         @rm -f $(obj)apm_scu.c
#         ln -s $(SRCTREE)/cpu/ppc4xx/apm_scu.c $(obj)apm_scu.c

$(obj)init.S:
	@rm -f $(obj)init.S
	ln -s $(SRCTREE)/board/$(BOARDDIR)/init.S $(obj)init.S

$(obj)ecc.h:
	@rm -f $(obj)ecc.h
	ln -s $(SRCTREE)/cpu/ppc4xx/ecc.h $(obj)ecc.h

$(obj)ufc.c:
	@rm -f $(obj)ufc.c
	ln -s $(SRCTREE)/drivers/mtd/nand/ufc.c $(obj)ufc.c

$(obj)resetvec.S:
	@rm -f $(obj)resetvec.S
	ln -s $(SRCTREE)/cpu/ppc4xx/resetvec.S $(obj)resetvec.S

$(obj)start.S:
	@rm -f $(obj)start.S
	ln -s $(SRCTREE)/cpu/ppc4xx/start.S $(obj)start.S

# from nand_spl directory
$(obj)nand_boot_apm_ufc.c: $(SRCTREE)/nand_spl/nand_boot_apm_ufc.c
	@rm -f $(obj)nand_boot_apm_ufc.c
	ln -s $(SRCTREE)/nand_spl/nand_boot_apm_ufc.c $(obj)nand_boot_apm_ufc.c

# $(obj)nand_support.c:
#         ln -sf $(SRCTREE)/nand_spl/board/amcc/serengeti/nand_support.c $(obj)nand_support.c

# from drivers/nand directory
$(obj)nand_ecc.c:
	@rm -f $(obj)nand_ecc.c
	ln -s $(SRCTREE)/drivers/mtd/nand/nand_ecc.c $(obj)nand_ecc.c

# $(obj)bch.c:
#         @rm -f $(obj)bch.c
#         ln -s $(SRCTREE)/lib_generic/bch/bch.c $(obj)bch.c

$(obj)ppcstring.S:
	@rm -f $(obj)ppcstring.S
	ln -s $(SRCTREE)/lib_ppc/ppcstring.S $(obj)ppcstring.S

#########################################################################

$(obj)%.o:	$(obj)%.S
	$(CC) $(AFLAGS) -c -o $@ $<

$(obj)%.o:	$(obj)%.c
	$(CC) $(CFLAGS) -c -o $@ $<

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
