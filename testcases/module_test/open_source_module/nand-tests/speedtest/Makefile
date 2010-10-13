ifndef KERNEL_PATH
	KERNEL_PATH=/home/vobs/linux-2.6-imx
endif

obj-m += speedtest.o

modules:
	make -C $(KERNEL_PATH) SUBDIRS=$$PWD modules

modules_install:
	make -C $(KERNEL_PATH) SUBDIRS=$$PWD modules_install

all: modules modules_install

clean:
	rm -rf speedtest.ko speedtest.o speedtest.mod.c speedtest.mod.o \
	Module.symvers .speedtest* speedtest.mod .tmp_versions speedtest.c.bak modules.order
