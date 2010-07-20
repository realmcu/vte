##############################################################################
#
#    @file   Makefile
#
#    @brief  nettools.
#
##############################################################################

include ../vte_configs/$(ARCH_PLATFORM)_tp_configs

SUBDIRS-$(CONFIG_TP_ETHTOOL) += ethtool-6
clean_SUBDIR-$(CONFIG_TP_ETHTOOL) += clean_ethtool-6
SUBDIRS-$(CONFIG_TP_PPP)     += ppp-2.4.3
clean_SUBDIR-$(CONFIG_TP_PPP) += clean_ppp-2.4.3
SUBDIRS-$(CONFIG_TP_NETPERF) +=  netperf-2.4.4
clean_SUBDIR-$(CONFIG_TP_NETPERF) += clean_netperf-2.4.4
SUBDIRS-$(CONFIG_TP_TCPDUMP) += tcpdump-3.9.8
clean_SUBDIR-$(CONFIG_TP_TCPDUMP) += clean_tcpdump-3.9.8
SUBDIRS-$(CONFIG_TP_MII)     += mii-tool-1.9.1.1
clean_SUBDIRS-$(CONFIG_TP_MII) += clean_mii-tool-1.9.1.1


all: $(SUBDIRS-y)
	for i in $(SUBDIRS-y) ; do $(MAKE) $$i -B ; done
install:
	-ln -f ethtool-6/ethtool ../../bin/
	-ln -f netperf-2.4.4/src/netperf ../../bin/
	-ln -f netperf-2.4.4/doc/examples/* ../../bin/
	-ln -f mii-tool-1.9.1.1/mii-tool ../../bin/
	-ln -f ppp-2.4.3/pppd/pppd ../../bin/
	-ln -f tcpdump-3.9.8/tcpdump ../../bin/

clean: $(clean_SUBDIR-y)

ethtool-6:
	cd $@;./myconfig; cd ..
	$(MAKE) -C $@ CC=$(CROSS_COMPILER)gcc

clean_ethtool-6:
	-$(MAKE) -C ethtool-6 clean
	-ac_cv_func_setpgrp_void=yes $(MAKE) -C ethtool-6 distclean

ppp-2.4.3:
	cd $@;./myconfig; cd ..
	$(MAKE) -C $@ CC=$(CROSS_COMPILER)gcc

clean_ppp-2.4.3:
	-$(MAKE) -C ppp-2.4.3 clean
	-$(MAKE) -C ppp-2.4.3 dist-clean

netperf-2.4.4:
	cd $@; ./myconfig; cd ..
	ac_cv_func_setpgrp_void=yes $(MAKE) -C $@ CC=$(CROSS_COMPILER)gcc

clean_netperf-2.4.4:
	-$(MAKE) -C netperf-2.4.4 clean
	-ac_cv_func_setpgrp_void=yes $(MAKE) -C netperf-2.4.4 distclean

tcpdump-3.9.8:
	$(MAKE) -C $(pwd)/../../../openlibs
	cd $@; ./myconfig; cd ..
	$(MAKE) -C $@

clean_tcpdump-3.9.8:
	-$(MAKE) -C  tcpdump-3.9.8 clean
	-$(MAKE) -C  tcpdump-3.9.8 distclean

mii-tool-1.9.1.1:
	$(MAKE) -C $@ CC=$(CROSS_COMPILER)gcc

clean_mii-tool-1.9.1.1:
	-$(MAKE) -C mii-tool-1.9.1.1 clean
	-$(MAKE) -C mii-tool-1.9.1.1 distclean

