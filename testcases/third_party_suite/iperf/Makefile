#Copyright 2005-2010 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html

SUBDIRS=iperf-2.0.5
TARGETS=
BINS=iperf
 
all:
	cd iperf-2.0.5; ./configure --host=arm-none-linux-gnueabi CC=${CROSS_COMPILER}gcc; cd ..
	$(MAKE) -C iperf-2.0.5 CC=${CROSS_COMPILER}gcc

install:
		ln -f $(SUBDIRS)/src/$(BINS) ../../bin/

clean:
	-$(MAKE) -C $(SUBDIRS) clean
	-$(MAKE) -C $(SUBDIRS) distclean


