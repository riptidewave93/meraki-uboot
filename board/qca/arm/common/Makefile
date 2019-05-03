obj-$(CONFIG_IPQ_SWITCH_ATHRS17) += athrs17_phy.o
obj-$(CONFIG_IPQ_SWITCH_QCA8511) += qca8511.o
obj-y += cmd_blowsecfuse.o
obj-$(CONFIG_ARCH_IPQ807x) += fdt_fixup.o fdt_info.o
ifeq ($(CONFIG_MERAKI_RELEASE),)
obj-y += meraki_dev_boot.o
else
obj-y += meraki_rel_boot.o
endif

ccflags-y      += -I$(srctree)/board/qca/arm/common
