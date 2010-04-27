#Copyright (C) 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
# list of platforms which want this test case

LIB = ipu screenlayer
SUBDIRS = app

SUBDIRS += scripts


all:
	@set -e; for i in $(LIB); do $(MAKE) -C $$i ; done
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
	@set -e; for i in $(LIB); do $(MAKE) -C $$i clean ; done

