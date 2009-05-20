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
	-ln -f netperf-2.4.2/src/netperf ../../bin/netperf
	-ln -f netperf-2.4.2/doc/examples/* ../../bin/
	-ln -f mii-tool-1.9.1.1/mii-tool ../../bin/
	-ln -f ppp-2.4.3/pppd/pppd ../../bin

clean:
	-@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
	-@set -e; $(MAKE) -C netperf-2.4.2 distclean
	-@set -e; for i in $(CONFIG_SUBDIRS); do $(MAKE) -C $$i distclean ; done

config:
	-@set -e; if [ ! -z $(CROSS_COMPILER) ]; then \
	for i in $(CONFIG_SUBDIRS); do cd $$i && ./configure --host=arm-none-linux-gnueabi CC=$(CROSS_COMPILER)gcc && cd ..; done; \
	else for i in $(CONFIG_SUBDIRS); do cd $$i && ./configure --host=arm-none-linux-gnueabi CC=$(CROSS_COMPILER)gcc && cd ..; done; \
	fi
	cd netperf-2.4.2 && ./myconfig && cd ..
