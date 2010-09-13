subdir = lmbench3

all:
	$(MAKE) -C $(subdir)/src OS=armv5tel-linux-gnu  CC=$(CROSS_COMPILER)gcc
	$(MAKE) -C $(subdir)/src OS=armv5tel-linux-gnu CC=$(CROSS_COMPILER)gcc clean

install:
	-cp -a lmbench3 ../../bin/

clean:
	$(MAKE) -C $(subdir)/src OS=armv5tel-linux-gnu CC=$(CROSS_COMPILER)gcc clean
	
distclean: clean
	-rm -rf $(subdir)/bin/*
