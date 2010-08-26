###############################################################################
#
#    @file   Makefile
#
#    @brief  bonnie+=.
#
##############################################################################


SUBDIRS = util-linux-ng-2.18 

all: config
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	-@set -e; for i in $(SUBDIRS); do make -C $$i install ; done
	-ln -f  bin/* ../../bin/
	-ln -f  sbin/* ../../bin/

clean:
	-@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
	-@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i distclean ; done
	-rm -rf include bin sbin share lib

config:
	-@set -e; for i in $(SUBDIRS); do cd $$i; ./myconfig ; cd ..; done
