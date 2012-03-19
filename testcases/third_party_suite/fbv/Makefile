
TARGET = fbv
SUBDIR = fbv-1.0b

all:
	cd $(SUBDIR); ./myconfig ; \
	$(MAKE) CC=$(CROSS_COMPILE)gcc

install:
	-ln -f $(SUBDIR)/$(TARGET) ../../bin/

clean:
	$(MAKE) -C $(SUBDIR) clean
	$(MAKE) -C $(SUBDIR) distclean
	
