###############################################################################
#
#    @file   Makefile
#
#    @brief  iozone
#
##############################################################################

SUBDIRS = fbset-2.1 

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i CC=$(CROSS_COMPILE)gcc; done

install:
	-ln -f fbset-2.1/fbset ../../bin/
clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
