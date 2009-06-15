###############################################################################
#
#    @file   Makefile
#
#    @brief  nettools.
#
##############################################################################


CONFIG_SUBDIRS = ethtool-6 ppp-2.4.3
SUBDIRS = ethtool-6 mii-tool-1.9.1.1 ppp-2.4.3 tcpdump-3.9.8
TARGETS=ethtool mii-tool

all: config
	-@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i CC=$(CROSS_COMPILER)gcc ; done
	-$(MAKE) -C netperf-2.4.2/src 

install:
	-ln -f ethtool-6/ethtool ../../bin/
	-ln -f netperf-2.4.2/src/netperf ../../bin/netperf
	-ln -f netperf-2.4.2/doc/examples/* ../../bin/
	-ln -f mii-tool-1.9.1.1/mii-tool ../../bin/
	#-$(MAKE) -C netperf-2.4.2 install 
	-ln -f ppp-2.4.3/pppd/pppd ../../bin/
	-ln -f tcpdump-3.9.8/tcpdump ../../bin/

clean:
	-@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
	-@set -e; $(MAKE) -C netperf-2.4.2 clean
	-@set -e; for i in $(CONFIG_SUBDIRS); do $(MAKE) -C $$i distclean ; done

config:
	-@set -e; if [ ! -z $(CROSS_COMPILER) ]; then \
	for i in $(CONFIG_SUBDIRS); do cd $$i && ./configure --host=arm-none-linux-gnueabi CC=$(CROSS_COMPILER)gcc && cd ..; done; \
	else for i in $(CONFIG_SUBDIRS); do cd $$i && ./configure --host=arm-none-linux-gnueabi CC=$(CROSS_COMPILER)gcc && cd ..; done; \
	fi
	-cd netperf-2.4.2 && ./myconfig && cd ..
	-cd tcpdump-3.9.8 && ./myconfig && cd ..
