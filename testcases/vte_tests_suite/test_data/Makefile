
DATA-y = butterfly.png 
DATA-$(CONFIG_VTE_SYS) += output_video_yuv420 
SUBDIRS-$(CONFIG_VTE_V4L_PXP) += pxp_test_files
SUBDIRS-$(CONFIG_VTE_VPU)     += pc_tool

all:
	
install:
	-ln -f $(DATA-y) ../../bin/
	for i in $(SUBDIRS-y); do $(MAKE) -C $$i install; done 
clean:

