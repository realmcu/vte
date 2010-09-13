subdir = lmbench3

all:
	$(MAKE) -C $(subdir)/src OS=armv5tel-linux-gnu  CC=$(CROSS_COMPILER)gcc

install:
	-ln -f $(subdir)/bin/armv5tel-linux-gnu/* ../../bin/

clean:
	$(MAKE) -C $(subdir)/src OS=armv5tel-linux-gnu CC=$(CROSS_COMPILER)gcc clean
