###############################################################################
#
#    @file   Makefile
#
#    @brief  bonnie+=.
#
##############################################################################


SUBDIRS = bonnie++-1.03d

all: config
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	-@set -e; for i in $(SUBDIRS); do ln -f $$i/bonnie++ $$i/zcav ../../bin/ ; done

clean:
	-@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
	-@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i realclean ; done

config:
	-@set -e; for i in $(SUBDIRS); do cd $$i; ./myconfig ; cd ..; done
