#Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
###############################################################################
#
#    @file   Makefile
#
#    @brief  the third part test suite for storage.
#
##############################################################################

CONFIG = ../../third_party_suite/fbv-1.0b
SUBDIRS = mxcfb_test

all: config
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i CC=$(CROSS_COMPILER)gcc; done
	-@set -e; for i in $(CONFIG); do $(MAKE) -C $$i CC=$(CROSS_COMPILER)gcc \
	CFLAGS="-O2 -Wall -D_GNU_SOURCE -I../../../openlibs/include"; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done
	-ln -f $(CONFIG)/fbv ../../bin/
	-ln -f butterfly.png ../../bin/ 

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
	-@set -e; for i in $(CONFIG); do $(MAKE) -C $$i clean ; done

config:
	-@set -e; for i in $(CONFIG); do cd $$i && ./myconfig cd -; done
