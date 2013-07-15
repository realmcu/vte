###############################################################################
#
#    @file   Makefile
#
##############################################################################


SUBDIRS = memtester-4.3.0

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	-@set -e; for i in $(SUBDIRS); do ln -f $$i/memtester ../../bin/ ; done

clean:
	-@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done

