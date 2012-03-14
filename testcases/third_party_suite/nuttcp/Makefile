#Copyright 2005-2010 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html

SUBDIRS=nuttcp-6.1.2
TARGETS=
BINS=nuttcp
 
all:
	cd nuttcp-6.1.2; CC=${CROSS_COMPILER}gcc;
	$(MAKE) -C nuttcp-6.1.2 CC=${CROSS_COMPILER}gcc

install:
		ln -f $(SUBDIRS)/$(BINS) ../../bin/

clean:
	-$(MAKE) -C $(SUBDIRS) clean
	-$(MAKE) -C $(SUBDIRS) distclean


