subdir = lmbench3
confs-$(CONFIG_TP_IMX5) = ./platform_configs/mx5x_lm_config ./platform_configs/mx53_lm_config
confs-$(CONFIG_TP_IMX28) = ./platform_configs/mx28_lm_config
confs-$(CONFIG_TP_IMX6) = ./platform_configs/mx6x_lm_config

all:
	$(MAKE) -C $(subdir)/src OS=$$(echo $$CFLAGS | sed 's/-march=/:/g' | cut -d : -f 2 | cut -d - -f 1)-linux-gnu CC=$(CROSS_COMPILE)gcc
	#$(MAKE) -C $(subdir)/src OS=$$(echo $$CFLAGS | sed 's/-march=/:/g' | cut -d : -f 2 | cut -d - -f 1)-linux-gnu CC=$(CROSS_COMPILE)gcc clean

install:
	#-cp -a lmbench3/bin/$$(echo $$CFLAGS | sed 's/-march=/:/g' | cut -d : -f 2 | cut -d - -f 1)-linux-gnu/* ../../bin/
	-cp -a lmbench3 ../../bin/
	-ln -f $(confs-y) ../../bin/

clean:
	$(MAKE) -C $(subdir)/src OS=$$(echo $$CFLAGS | sed 's/-march=/:/g' | cut -d : -f 2 | cut -d - -f 1)-linux-gnu CC=$(CROSS_COMPILE)gcc clean
	-rm -rf $(subdir)/bin
	
distclean: clean
	-rm -rf $(subdir)/bin/*
