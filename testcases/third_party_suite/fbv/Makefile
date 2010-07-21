
TARGET = fbv
SUBDIR = fbv-1.0b

all:
	$(MAKE) -C $(pwd)../../../openlibs 
	cd $(SUBDIR); ./myconfig;cd ..
	$(MAKE) -C $(SUBDIR) CC=$(CROSS_COMPILER)gcc

install:
	ln -f $(SUBDIR)/$(TARGET) ../../bin/

clean:
	$(MAKE) -C $(SUBDIR) clean
	$(MAKE) -C $(SUBDIR) distclean
	

