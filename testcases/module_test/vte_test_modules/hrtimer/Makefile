
obj-m := hrtimer_test.o
TARGETS = hrtimer_test.ko
all:
	$(MAKE) -C $(KERNEL_SRCDIR) SUBDIRS=`pwd` KBUILD_OUTPUT=$(KLINUX_BLTDIR) ARCH=$(ARCH_CPU) modules

install:
	ln -f $(TARGETS) ../../../bin/
clean:
	@rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions Module.symvers modules.order


.PHONY: install clean


