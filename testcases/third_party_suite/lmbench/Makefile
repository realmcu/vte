subdir = lmbench3

all:
	$(MAKE) -C $(subdir)/src OS=$$(echo $$CFLAGS | sed 's/-march=/:/g' | cut -d : -f 2 | cut -d - -f 1)-linux-gnu CC=$(CROSS_COMPILER)gcc
	$(MAKE) -C $(subdir)/src OS=$$(echo $$CFLAGS | sed 's/-march=/:/g' | cut -d : -f 2 | cut -d - -f 1)-linux-gnu CC=$(CROSS_COMPILER)gcc clean

install:
	-cp -a lmbench3/bin/$$(echo $$CFLAGS | sed 's/-march=/:/g' | cut -d : -f 2 | cut -d - -f 1)-linux-gnu/* ../../bin/
	-cp -a lmbench3 ../../bin/

clean:
	$(MAKE) -C $(subdir)/src OS=$$(echo $$CFLAGS | sed 's/-march=/:/g' | cut -d : -f 2 | cut -d - -f 1)-linux-gnu CC=$(CROSS_COMPILER)gcc clean
	-rm -rf $(subdir)/bin
	
distclean: clean
	-rm -rf $(subdir)/bin/*
