# list of platforms which want this test case

LIB = ipu screenlayer
SUBDIRS = app

SUBDIRS += scripts


all:
	@set -e; for i in $(LIB); do $(MAKE) -C $$i ; done
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
	@set -e; for i in $(LIB); do $(MAKE) -C $$i clean ; done

