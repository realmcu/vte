export ARCH=$(ARCH_CPU)
export CROSS_COMPILE=$(CROSS_COMPILER)

obj-m += tcrypt.o

modules:
	make -C $(KERNEL_SRCDIR) SUBDIRS=$$PWD modules

modules_install:
	make -C $(KERNEL_SRCDIR) SUBDIRS=$$PWD modules_install

install:
	ln -f  tcrypt.ko ../../../bin/

all: modules modules_install

clean:
	-rm -f *.ko *.o *.mod.c modules.order  Module.symvers .*
