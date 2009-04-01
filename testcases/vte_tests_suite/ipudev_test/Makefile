# list of platforms which want this test case

LIB = ipu
SUBDIRS = app

SUBDIRS += scripts


all:
	$(MAKE) -C $(LIB)
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done

