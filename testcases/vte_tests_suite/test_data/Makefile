
DATA = butterfly.png 
SUBDIRS-$(CONFIG_VTE_V4L_PXP) += pxp_test_files

all:
	
install:
	-ln -f $(DATA) ../../bin/
	for i in $(SUBDIRS-y); do $(MAKE) -C $$i install; done 
clean:

