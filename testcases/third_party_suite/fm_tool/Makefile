

SUBDIR = fmtools-1.0.2


all:
	$(MAKE) -C $(SUBDIR) CC=$(CROSS_COMPILE)gcc
install:
	-ln -f $(SUBDIR)/fm ../../bin/
	-ln -f $(SUBDIR)/fmscan ../../bin/
clean:
	$(MAKE) -C $(SUBDIR) clean
