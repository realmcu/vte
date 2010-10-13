ifndef KERNEL_PATH
	KERNEL_PATH=/home/vobs/linux-2.6-imx
endif

obj-m += oobtest.o

modules:
	make -C $(KERNEL_PATH) SUBDIRS=$$PWD modules

modules_install:
	make -C $(KERNEL_PATH) SUBDIRS=$$PWD modules_install

all: modules modules_install

clean:
	rm -rf oobtest.ko oobtest.o oobtest.mod.c oobtest.mod.o \
	Module.symvers .oobtest* oobtest.mod .tmp_versions oobtest.c.bak modules.order
