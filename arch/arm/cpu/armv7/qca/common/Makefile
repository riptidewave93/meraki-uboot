ccflags-y += -I$(srctree)/board/qca/arm/common/
ccflags-$(CONFIG_ARCH_IPQ40xx)		+= -I$(srctree)/board/qca/arm/ipq40xx/
ccflags-$(CONFIG_ARCH_IPQ806x)		+= -I$(srctree)/board/qca/arm/ipq806x/
ccflags-$(CONFIG_ARCH_IPQ807x)		+= -I$(srctree)/board/qca/arm/ipq807x/

obj-y := smem.o
obj-$(CONFIG_ARCH_IPQ807x) += timer.o
obj-y += scm.o
