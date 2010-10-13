ifndef KERNEL_PATH
	KERNEL_PATH=/root/git/ubi-2.6.git
endif

obj-m += pagetest.o

modules:
	make -C $(KERNEL_PATH) SUBDIRS=$$PWD modules

modules_install:
	make -C $(KERNEL_PATH) SUBDIRS=$$PWD modules_install

all: modules modules_install

clean:
	rm -rf pagetest.ko pagetest.o pagetest.mod.c pagetest.mod.o \
	Module.symvers .pagetest* pagetest.mod .tmp_versions pagetest.c.bak modules.order
