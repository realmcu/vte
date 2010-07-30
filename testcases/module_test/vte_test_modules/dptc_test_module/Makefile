#Copyright (C) 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html

obj-m = dryice_test.o
TARGET = dryice_test.ko 

EXTRA_CFLAGS += -Wall \
                -I$(KLINUX_SRCDIR)/include \
		-I$(KLINUX_BLTDIR)/include


# Need to remove sahara objects because the test app uses same objects
all:
	$(MAKE) -C $(KLINUX_SRCDIR) SUBDIRS=`pwd` KBUILD_OUTPUT=$(KLINUX_BLTDIR) ARCH=$(ARCH_CPU) CROSS_COMPILE=$(CROSS_COMPILER) modules

install:
	-ln -f $(TARGET) ../../../bin/

.PHONY: clean
clean :
	@rm -f *.o *.mod.c *.ko .*.cmd
	@rm -rf .tmp_versions modules.order
	@for X in $(shell /bin/ls -d */); do	\
		cd $$X;				\
		rm -f *.o *.mod.c *.ko .*.cmd;	\
		rm -rf .tmp_versions;		\
		cd ..;				\
	done
