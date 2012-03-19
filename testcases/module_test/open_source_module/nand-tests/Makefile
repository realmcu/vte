ifndef KERNEL_PATH
	KERNEL_PATH=$(KLINUX_BLTDIR)
endif

obj-m += mtd_nandecctest.o  
obj-m += mtd_oobtest.o
obj-m += mtd_pagetest.o  
obj-m += mtd_readtest.o  
obj-m += mtd_speedtest.o  
obj-m += mtd_stresstest.o  
obj-m += mtd_subpagetest.o  
obj-m += mtd_torturetest.o


modules:
	make -C $(KERNEL_PATH) SUBDIRS=$$PWD ARCH=$(ARCH_CPU) modules

modules_install:
	make -C $(KERNEL_PATH) SUBDIRS=$$PWD ARCH=$(ARCH_CPU) modules_install

install:
	@set -e; ln -f *.ko ../../../bin/

all: modules modules_install

clean:
	rm -rf *.ko *.o *.mod.c  \
	Module.symvers  .*.cmd *.mod .tmp_versions *.c.bak modules.order
