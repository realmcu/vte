###############################################################################
#
#    @file   Makefile
#
#    @brief  the third part test suite for storage.
#
##############################################################################

CONFIG = fbv-1.0b
SUBDIRS_2 = fbv-1.0b 
SUBDIRS = mxcfb_test

all: config
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i CC=$(CROSS_COMPILER)gcc; done
	-@set -e; for i in $(SUBDIRS_2); do $(MAKE) -C $$i CC=$(CROSS_COMPILER)gcc; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done
	-ln -f fbv-1.0b/fbv ../../bin/
	-ln -f butterfly.png ../../bin/ 

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
	-@set -e; for i in $(SUBDIRS_2); do $(MAKE) -C $$i clean ; done

config:
	@set -e; for i in $(CONFIG); do cd $$i && ./myconfig && cd ..; done
