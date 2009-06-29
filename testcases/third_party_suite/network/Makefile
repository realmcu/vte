###############################################################################
#
#    @file   Makefile
#
#    @brief  nettools.
#
##############################################################################
CONFIG_SUBDIRS = ethtool-6 ppp-2.4.3 netperf-2.4.2 tcpdump-3.9.8
DISTCLEAN_SUBDIRS = ethtool-6 netperf-2.4.2 tcpdump-3.9.8
SUBDIRS = ethtool-6 mii-tool-1.9.1.1 ppp-2.4.3 tcpdump-3.9.8

all: config
	-@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i CC=$(CROSS_COMPILER)gcc ; done
	-$(MAKE) -C netperf-2.4.2/src 

install:
	-ln -f ethtool-6/ethtool ../../bin/
	-ln -f netperf-2.4.2/src/netperf ../../bin/
	-ln -f netperf-2.4.2/doc/examples/* ../../bin/
	-ln -f mii-tool-1.9.1.1/mii-tool ../../bin/
	-ln -f ppp-2.4.3/pppd/pppd ../../bin/
	-ln -f tcpdump-3.9.8/tcpdump ../../bin/

clean:
	-@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
	-@set -e; for i in $(DISTCLEAN_SUBDIRS); do echo $$i ; $(MAKE) -C $$i distclean ; done
	-@$(MAKE) -C ppp-2.4.3 dist-clean
config:
	-@set -e;for i in $(CONFIG_SUBDIRS); do echo $$i ;cd $$i && ./myconfig && cd ..; done; 
