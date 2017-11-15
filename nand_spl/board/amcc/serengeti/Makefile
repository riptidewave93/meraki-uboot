include $(TOPDIR)/config.mk

all: u-boot-spl.bin

u-boot-spl.bin: stage-1-spl.bin stage-2-spl.bin
	cat $(OBJTREE)/nand_spl/board/$(BOARDDIR)/stage-1/u-boot-spl.bin.swp $(OBJTREE)/nand_spl/board/$(BOARDDIR)/stage-2/u-boot-spl.bin > $(OBJTREE)/nand_spl/u-boot-spl.bin

stage-1-spl.bin:
	make -C stage-1 all

stage-2-spl.bin:
	make -C stage-2 all
