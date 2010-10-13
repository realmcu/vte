ifndef KERNEL_PATH
	KERNEL_PATH=/root/git/ubi-2.6.git
endif

obj-m += stresstest.o

modules:
	make -C $(KERNEL_PATH) SUBDIRS=$$PWD modules

modules_install:
	make -C $(KERNEL_PATH) SUBDIRS=$$PWD modules_install

all: modules modules_install

clean:
	rm -rf stresstest.ko stresstest.o stresstest.mod.c stresstest.mod.o \
	Module.symvers .stresstest* stresstest.mod .tmp_versions \
	stresstest.c.bak modules.order
