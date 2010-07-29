
DATA-y = butterfly.png 
DATA-$(CONFIG_VTE_SYS) += output_video_yuv420 
SUBDIRS-$(CONFIG_VTE_V4L_PXP) += pxp_test_files
SUBDIRS-$(CONFIG_VTE_VPU)     += pc_tool
SUBDIRS-$(CONFIG_VTE_V4l_IPU_PF) += ipu_pf 
SUBDIRS-$(CONFIG_VTE_V4l_OUTPUT) += v4l_output_config

all:
	
install:
	-ln -f $(DATA-y) ../../bin/
	for i in $(SUBDIRS-y); do $(MAKE) -C $$i install; done 
clean:

