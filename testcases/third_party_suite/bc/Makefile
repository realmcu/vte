###############################################################################
#
#    @file   Makefile
#
#    @brief  dt.
#
##############################################################################

SUBDIRS = bc-1.06.95

all:
	cd $(SUBDIRS); ./configure --host=arm-none-linux-gnueabi; $(MAKE) CC=$(CROSS_COMPILE)gcc

install:
	ln -f $(SUBDIRS)/bc/bc ../../bin/

clean:
	cd $(SUBDIRS); $(MAKE) CC=$(CROSS_COMPILE)gcc clean
