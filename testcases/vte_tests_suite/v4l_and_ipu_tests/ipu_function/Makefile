#Copyright (C) 2005-2010 Freescale Semiconductor, Inc. All Rights Reserved.
#
###The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html

#CC = $(CROSS_COMPILER)gcc
#CFLAGS += -O3 
#LDFLAGS += 

SUBDIRS = tools

TARGET = scripts/ipuv3_test.sh 

all:	
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i; done 
install:
	@set -e; for i in $(SUBDIRS);do  $(MAKE) -C $$i install; done 
	-ln -f $(TARGET) ../../../bin
clean:
	@set -e; for i in $(SUBDIRS);do  $(MAKE) -C $$i clean; done 
	
