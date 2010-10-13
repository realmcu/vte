ifndef KERNEL_PATH
	KERNEL_PATH=/root/git/ubi-2.6.git
endif

obj-m += scantest.o

modules:
	make -C $(KERNEL_PATH) SUBDIRS=$$PWD modules

modules_install:
	make -C $(KERNEL_PATH) SUBDIRS=$$PWD modules_install

all: modules modules_install

clean:
	rm -rf scantest.ko scantest.o scantest.mod.c scantest.mod.o \
	Module.symvers .scantest* scantest.mod .tmp_versions scantest.c.bak modules.order
