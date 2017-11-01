# $Id: Makefile,v 1.17 2010/04/15 08:45:28 mlarsen Exp $
# $Copyright: (c) 2005-2007 Broadcom Corp.
# All Rights Reserved.$
#
#
# Build MDK and glue
#
CFG_MDK_DEBUG ?= 0
CFG_MDK_DIAG ?= 0

ifdef MDK

# Core MDK libraries
export CDK = ${MDK}/cdk
export BMD = ${MDK}/bmd
export PHY = ${MDK}/phy

# BMD diagnostics 
export BMDDIAG = $(MDK)/diag/bmddiag

#
# Include MDK CFLAGS from commandline.
#
CDK_CPPFLAGS += ${MDK_CFLAGS}
CDK_CPPFLAGS += -DCFG_MDK_DEBUG=${CFG_MDK_DEBUG}
CDK_CPPFLAGS += -DCFG_MDK_DIAG=${CFG_MDK_DIAG}

CDK_CPPFLAGS += -DCDK_INCLUDE_CUSTOM_CONFIG
CDK_CPPFLAGS += -I${CURDIR}/${TOP}/mdk
CDK_CPPFLAGS += -I${CURDIR}/${TOP}/include
CDK_CPPFLAGS += -I${CDK}/include

PHY_CPPFLAGS += -DPHY_SYS_USLEEP=mdk_sys_udelay
PHY_CPPFLAGS += -DPHY_INCLUDE_CUSTOM_CONFIG

BMD_CPPFLAGS += ${PHY_CPPFLAGS}
BMD_CPPFLAGS += -DBMD_SYS_USLEEP=mdk_sys_udelay
BMD_CPPFLAGS += -DBMD_SYS_DMA_ALLOC_COHERENT=mdk_sys_dma_alloc_coherent
BMD_CPPFLAGS += -DBMD_SYS_DMA_FREE_COHERENT=mdk_sys_dma_free_coherent
BMD_CPPFLAGS += -I${PHY}/include
BMD_CPPFLAGS += -I${BMD}/include

BMDDIAG_CPPFLAGS = $(BMD_CPPFLAGS)

#
# Add MDK libraries to CFE library list
#
include ${BMD}/targets.mk
LDLIBS += ${addprefix -lbmd,${BMD_TARGETS}}

include ${PHY}/targets.mk
LDLIBS += ${addprefix -lphy,${PHY_TARGETS}}

ifeq ($(strip ${CFG_MDK_DIAG}),1)
LDLIBS += -lbmddiag
endif

include ${CDK}/targets.mk
LDLIBS += ${addprefix -lcdk,${CDK_TARGETS}}

#
# uboot integration: below this line
#
# Combine u-boot and MDK compiler options and flags 
#
CFLAGS  += -DCFG_MDK=1 ${CDK_CPPFLAGS} ${BMD_CPPFLAGS}
#CFLAGS  += -DSYS_BE_PIO=0 -DSYS_BE_PACKET=0 -DSYS_BE_OTHER=0
ALL_CFLAGS  := ${filter-out -g -Wmissing-prototypes,${CFLAGS}}

#
# uboot integration: above this line
#

# MDK make targets
MDKLIBS = cdk bmd phy
ifeq ($(strip ${CFG_MDK_DIAG}),1)
MDKLIBS += bmddiag
endif

# For optional use by top-level makefile
BLDMDK = mdkpkgs mdklibs

endif # defined MDK

MDK_UBOOT_BLDDIR = ${BUILD_DIR}/mdk-lib

# Dummy target to avoid remaking MDK libraries when building in parallel
mdknolib:

checkmdk:
ifndef MDK
	@echo 'The $$MDK environment variable is not set'; exit 1
endif

mdkpkgs: checkmdk
	$(MAKE) -C ${CDK} instpkgs
	$(MAKE) -C ${PHY} instpkgs
	$(MAKE) -C ${BMD} instpkgs

mdklibs: ${MDKLIBS}

cdk:
	make -C ${CDK} CC=${CC} AR=${AR} CDK_BLDDIR=${MDK_UBOOT_BLDDIR} CDK_CPPFLAGS="${ALL_CFLAGS}"

bmd:
	make -C ${PHY} CC=${CC} AR=${AR} PHY_BLDDIR=${MDK_UBOOT_BLDDIR}/ PHY_CPPFLAGS="${ALL_CFLAGS} ${BMD_CPPFLAGS}"

phy:
	make -C ${BMD} CC=${CC} AR=${AR} BMD_BLDDIR=${MDK_UBOOT_BLDDIR}/ BMD_CPPFLAGS="${ALL_CFLAGS} ${PHY_CPPFLAGS}"

bmddiag:
	make -C ${BMDDIAG} CC=${CC} AR=${AR} BMDDIAG_BLDDIR=${MDK_UBOOT_BLDDIR}/ BMDDIAG_CPPFLAGS="${ALL_CFLAGS} ${BMDDIAG_CPPFLAGS}"

.PHONY: checkmdk mdkpkgs mdklibs cdk bmd phy bmddiag

CLEANOBJS += ${MDK_UBOOT_BLDDIR}/libcdk*.a
CLEANOBJS += ${MDK_UBOOT_BLDDIR}/libbmd*.a
CLEANOBJS += ${MDK_UBOOT_BLDDIR}/libphy*.a

# Clean MDK objects completely
cleanmdkall:
	make distclean
	make CFG_MDK=1 cleanmdkobjs
	make CFG_MDK=1 cleanpkgs

cleanmdkobjs:
	rm -rf ${MDK_UBOOT_BLDDIR}/obj
	rm -rf ${CLEANOBJS}

cleanpkgs: checkmdk
	make -C ${CDK} clean
	make -C ${PHY} cleanpkgs
	make -C ${BMD} cleanpkgs

.PHONY: cleanpkgs cleanmdkobjs cleanmdk
