#
# (C) Copyright 2008
# Stefan Roese, DENX Software Engineering, sr@denx.de.
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
include $(TOPDIR)/nand_spl/board/$(BOARDDIR)/config.mk

LDSCRIPT= $(TOPDIR)/nand_spl/board/$(BOARDDIR)/u-boot.lds
LDFLAGS	= -Bstatic -T $(nandobj)u-boot.lds -Ttext $(TEXT_BASE) $(PLATFORM_LDFLAGS)
AFLAGS	+= -DCONFIG_NAND_SPL
CFLAGS	+= -DCONFIG_NAND_SPL

SOBJS	:= start.o
SOBJS	+= init.o
SOBJS	+= resetvec.o
SOBJS	+= cache.o
SOBJS	+= ppcstring.o
SOBJS	+= ppccache.o
COBJS	:= 44x_spd_ddr2.o
COBJS	+= 4xx_ibm_ddr2_autocalib.o
COBJS	+= tlb.o
COBJS	+= misc.o
COBJS	+= nand_boot.o
COBJS	+= nand_ecc.o
COBJS	+= ndfc.o
COBJS	+= 4xx_uart.o
COBJS	+= cpu_init.o
COBJS	+= speed.o

SRCS	:= $(addprefix $(obj),$(SOBJS:.o=.S) $(COBJS:.o=.c))
OBJS	:= $(addprefix $(obj),$(SOBJS) $(COBJS))
__OBJS	:= $(SOBJS) $(COBJS)
LNDIR	:= $(OBJTREE)/nand_spl/board/$(BOARDDIR)

nandobj	:= $(OBJTREE)/nand_spl/

ALL	= $(nandobj)u-boot-spl $(nandobj)u-boot-spl.bin $(nandobj)u-boot-spl-16k.bin

all:	$(obj).depend $(ALL)

$(nandobj)u-boot-spl-16k.bin: $(nandobj)u-boot-spl
	$(OBJCOPY) ${OBJCFLAGS} --pad-to=$(PAD_TO) -O binary $< $@

$(nandobj)u-boot-spl.bin:	$(nandobj)u-boot-spl
	$(OBJCOPY) ${OBJCFLAGS} -O binary $< $@

$(nandobj)u-boot-spl:	$(OBJS) $(nandobj)u-boot.lds
	cd $(LNDIR) && $(LD) $(LDFLAGS) $(__OBJS) \
		-Map $(nandobj)u-boot-spl.map \
		-o $(nandobj)u-boot-spl

$(nandobj)u-boot.lds: $(LDSCRIPT)
	$(CPP) $(CPPFLAGS) $(LDPPFLAGS) -ansi -D__ASSEMBLY__ -P - <$^ >$@

# create symbolic links for common files

# from cpu directory
$(obj)ndfc.c:
	@rm -f $(obj)ndfc.c
	ln -s $(SRCTREE)/drivers/mtd/nand/ndfc.c $(obj)ndfc.c

$(obj)resetvec.S:
	@rm -f $(obj)resetvec.S
	ln -s $(SRCTREE)/cpu/ppc4xx/resetvec.S $(obj)resetvec.S

$(obj)start.S:
	@rm -f $(obj)start.S
	ln -s $(SRCTREE)/cpu/ppc4xx/start.S $(obj)start.S

$(obj)cache.S:
	@rm -f $(obj)cache.S
	ln -s $(SRCTREE)/cpu/ppc4xx/cache.S $(obj)cache.S

$(obj)tlb.c:
	@rm -f $(obj)tlb.c
	ln -s $(SRCTREE)/cpu/ppc4xx/tlb.c $(obj)tlb.c

$(obj)cpu_init.c:
	@rm -f $(obj)cpu_init.c
	ln -s $(SRCTREE)/cpu/ppc4xx/cpu_init.c $(obj)cpu_init.c

$(obj)speed.c:
	@rm -f $(obj)speed.c
	ln -s $(SRCTREE)/cpu/ppc4xx/speed.c $(obj)speed.c

$(obj)4xx_uart.c:
	@rm -f $(obj)4xx_uart.c
	ln -s $(SRCTREE)/cpu/ppc4xx/4xx_uart.c $(obj)4xx_uart.c

$(obj)44x_spd_ddr2.c:
	@rm -f $(obj)44x_spd_ddr2.c
	ln -s $(SRCTREE)/cpu/ppc4xx/44x_spd_ddr2.c $(obj)44x_spd_ddr2.c

$(obj)4xx_ibm_ddr2_autocalib.c:
	@rm -f $(obj)4xx_ibm_ddr2_autocalib.c
	ln -s $(SRCTREE)/cpu/ppc4xx/4xx_ibm_ddr2_autocalib.c $(obj)4xx_ibm_ddr2_autocalib.c

$(obj)ppcstring.S:
	@rm -f $(obj)ppcstring.S
	ln -s $(SRCTREE)/lib_ppc/ppcstring.S $(obj)ppcstring.S

$(obj)ppccache.S:
	@rm -f $(obj)ppccache.S
	ln -s $(SRCTREE)/lib_ppc/ppccache.S $(obj)ppccache.S

$(obj)time.c:
	@rm -f $(obj)time.c
	ln -s $(SRCTREE)/lib_ppc/time.c $(obj)time.c

# from board directory
$(obj)init.S:
	@rm -f $(obj)init.S
	ln -s $(SRCTREE)/board/amcc/bluestone/init.S $(obj)init.S

# from nand_spl directory
$(obj)nand_boot.c:
	@rm -f $(obj)nand_boot.c
	ln -s $(SRCTREE)/nand_spl/nand_boot.c $(obj)nand_boot.c

# from drivers/mtd/nand directory
$(obj)nand_ecc.c:
	@rm -f $(obj)nand_ecc.c
	ln -s $(SRCTREE)/drivers/mtd/nand/nand_ecc.c $(obj)nand_ecc.c

#########################################################################

$(obj)%.o:	$(obj)%.S
	$(CC) $(AFLAGS) -c -o $@ $<

$(obj)%.o:	$(obj)%.c
	$(CC) $(CFLAGS) -c -o $@ $<

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
