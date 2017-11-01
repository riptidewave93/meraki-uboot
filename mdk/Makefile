include $(TOPDIR)/config.mk

LIB	= $(obj)libmdksys.o

# Add MDK system interface to lits of U-boot sources
UBOOT_MDK_CFLAGS += -I${MDK}/cdk/include -I${MDK}/bmd/include -I${MDK}/phy/include -I${MDK}/diag/bmddiag/include 
UBOOT_MDK_CFLAGS += -DBMD_SYS_USLEEP=mdk_sys_udelay -DBMD_SYS_DMA_ALLOC_COHERENT=mdk_sys_dma_alloc_coherent -DBMD_SYS_DMA_FREE_COHERENT=mdk_sys_dma_free_coherent
UBOOT_MDK_CFLAGS += -DPHY_SYS_USLEEP=mdk_sys_udelay
UBOOT_MDK_CFLAGS += -DSYS_BE_PIO=0 -DSYS_BE_PACKET=0 -DSYS_BE_OTHER=0
UBOOT_MDK_CFLAGS += -DCFG_MDK_DEBUG=1

CFLAGS += $(UBOOT_MDK_CFLAGS)

# core
COBJS-y += mdk_sys.o
COBJS-y += cmd_mdk.o

COBJS	:= $(sort $(COBJS-y))
SRCS	:= $(COBJS:.o=.c) $(XCOBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS))

CPPFLAGS += $(UBOOT_MDK_CFLAGS)

all:	$(LIB)

$(LIB): $(obj).depend $(OBJS)
	$(call cmd_link_o_target, $(OBJS))


#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
