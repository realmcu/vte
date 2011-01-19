
TARGET = fbv
SUBDIR = fbv-1.0b

all:
	cd $(SUBDIR); ./myconfig ; \
	$(MAKE) CC=$(CROSS_COMPILER)gcc

install:
	-ln -f $(SUBDIR)/$(TARGET) ../../bin/

clean:
	$(MAKE) -C $(SUBDIR) clean
	$(MAKE) -C $(SUBDIR) distclean
	
