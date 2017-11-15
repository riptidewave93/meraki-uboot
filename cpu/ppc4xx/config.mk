#
# (C) Copyright 2000
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

PLATFORM_RELFLAGS += -fPIC -ffixed-r14 -meabi
PLATFORM_CPPFLAGS += -DCONFIG_4xx -ffixed-r2 -mstring -msoft-float

cfg=$(shell grep configs $(OBJTREE)/include/config.h | sed 's/.*<\(configs.*\)>/\1/')
is440:=$(shell grep CONFIG_440 $(TOPDIR)/include/$(cfg))
is460:=$(shell grep CONFIG_460 $(TOPDIR)/include/$(cfg))

ifneq (,$(findstring CONFIG_460,$(is460)))
# Check if compiler supports -mcpu=464, if not then default to -mcpu=440
MCPU_464_SUPPORT:=$(shell if $(CROSS_COMPILE)gcc -mcpu=464 -S -o /dev/null -xc /dev/null > /dev/null 2>&1; then echo 464; else echo 440; fi;)
PLATFORM_CPPFLAGS += -Wa,-m$(MCPU_464_SUPPORT) -mcpu=$(MCPU_464_SUPPORT)
else 
ifneq (,$(findstring CONFIG_440,$(is440)))
PLATFORM_CPPFLAGS += -Wa,-m440 -mcpu=440
else
PLATFORM_CPPFLAGS += -Wa,-m405 -mcpu=405
endif
endif
