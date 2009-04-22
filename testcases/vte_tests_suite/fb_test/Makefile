###############################################################################
#
#    @file   Makefile
#
#    @brief  the third part test suite for storage.
#
##############################################################################


SUBDIRS = mxcfb_test

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
