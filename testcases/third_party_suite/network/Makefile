###############################################################################
#
#    @file   Makefile
#
#    @brief  nettools.
#
##############################################################################


CONFIG_SUBDIRS = ethtool-6
SUBDIRS = ethtool-6 mii-tool-1.9.1.1
TARGETS=ethtool mii-tool

all: config
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i CC=$(CROSS_COMPILER)gcc ; done

install:
	-ln -f ethtool-6/ethtool ../../bin/
	-ln -f mii-tool-1.9.1.1/mii-tool ../../bin/

clean:
	-@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
	-@set -e; for i in $(CONFIG_SUBDIRS); do $(MAKE) -C $$i distclean ; done

config:
	if [ ! -z $(CROSS_COMPILER) ]; then \
	-@set -e; for i in $(CONFIG_SUBDIRS); do cd $$i && ./configure --host=arm-none-linux-gnueabi CC=$(CROSS_COMPILER)gcc && cd ..; done; \
	else -@set -e; for i in $(CONFIG_SUBDIRS); do cd $$i && ./configure --host=arm-none-linux-gnueabi CC=$(CROSS_COMPILER)gcc && cd ..; done; \
	fi

