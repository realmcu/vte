###############################################################################
#
#    @file   Makefile
#
#    @brief  dt.
#
##############################################################################

SUBDIRS = bc-1.06.95

all:
	cd $(SUBDIRS); ./configure --host=arm-none-linux-gnueabi; $(MAKE) CC=$(CROSS_COMPILER)gcc

install:
	ln -s $(SUBDIRS)/bc/bc ../../bin/

clean:
	cd $(SUBDIRS); $(MAKE) CC=$(CROSS_COMPILER)gcc clean
