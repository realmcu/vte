ifndef KERNEL_PATH
	KERNEL_PATH=/root/git/ubi-2.6.git
endif

obj-m += torture.o

modules:
	make -C $(KERNEL_PATH) SUBDIRS=$$PWD modules

modules_install:
	make -C $(KERNEL_PATH) SUBDIRS=$$PWD modules_install

all: modules modules_install

clean:
	rm -rf torture.ko torture.o torture.mod.c torture.mod.o \
	Module.symvers .torture* torture.mod .tmp_versions torture.c.bak modules.order
