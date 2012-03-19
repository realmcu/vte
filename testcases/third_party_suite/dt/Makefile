###############################################################################
#
#    @file   Makefile
#
#    @brief  dt.
#
##############################################################################

SUBDIRS = dt.d

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i -f Makefile.linux CC=$(CROSS_COMPILE)gcc; done

install:
	-ln -f dt.d/dt ../../bin/
clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
