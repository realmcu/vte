subdir = lmbench3
confs-$(CONFIG_VTE_IMX5) = ./platform_configs/mx5x_lm_config
confs-$(CONFIG_VTE_IMX28) = ./platform_configs/mx28_lm_config

all:
	$(MAKE) -C $(subdir)/src OS=$$(echo $$CFLAGS | sed 's/-march=/:/g' | cut -d : -f 2 | cut -d - -f 1)-linux-gnu CC=$(CROSS_COMPILER)gcc
	#$(MAKE) -C $(subdir)/src OS=$$(echo $$CFLAGS | sed 's/-march=/:/g' | cut -d : -f 2 | cut -d - -f 1)-linux-gnu CC=$(CROSS_COMPILER)gcc clean

install:
	#-cp -a lmbench3/bin/$$(echo $$CFLAGS | sed 's/-march=/:/g' | cut -d : -f 2 | cut -d - -f 1)-linux-gnu/* ../../bin/
	-cp -a lmbench3 ../../bin/
	-ln -f $(confs-y) ../../bin/

clean:
	$(MAKE) -C $(subdir)/src OS=$$(echo $$CFLAGS | sed 's/-march=/:/g' | cut -d : -f 2 | cut -d - -f 1)-linux-gnu CC=$(CROSS_COMPILER)gcc clean
	-rm -rf $(subdir)/bin
	
distclean: clean
	-rm -rf $(subdir)/bin/*
