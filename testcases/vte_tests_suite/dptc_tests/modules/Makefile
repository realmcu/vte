
obj-m = dryice_test.o
TARGET = dryice_test.ko 


# Need to remove sahara objects because the test app uses same objects
all:
	$(MAKE) -C $(KLINUX_SRCDIR) SUBDIRS=`pwd` KBUILD_OUTPUT=$(KLINUX_BLTDIR) ARCH=$(ARCH_CPU) CROSS_COMPILE=$(CROSS_COMPILER) modules

install:
	-ln -f $(TARGET) ../../../bin/

.PHONY: clean
clean :
	@rm -f *.o *.mod.c *.ko .*.cmd
	@rm -rf .tmp_versions
	@for X in $(shell /bin/ls -d */); do	\
		cd $$X;				\
		rm -f *.o *.mod.c *.ko .*.cmd;	\
		rm -rf .tmp_versions;		\
		cd ..;				\
	done
