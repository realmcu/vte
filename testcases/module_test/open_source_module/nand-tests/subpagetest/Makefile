ifndef KERNEL_PATH
	KERNEL_PATH=/root/git/ubi-2.6.git
endif

obj-m += subpagetest.o

modules:
	make -C $(KERNEL_PATH) SUBDIRS=$$PWD modules

modules_install:
	make -C $(KERNEL_PATH) SUBDIRS=$$PWD modules_install

all: modules modules_install

clean:
	rm -rf subpagetest.ko subpagetest.o subpagetest.mod.c subpagetest.mod.o \
	Module.symvers .subpagetest* subpagetest.mod .tmp_versions subpagetest.c.bak modules.order
