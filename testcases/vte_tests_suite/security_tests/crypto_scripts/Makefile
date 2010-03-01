#Copyright 2004-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
###################################################################################################
#
#        @file   Makefile
#
#        @brief  configuration file to compile the VTE tests.
#

###################################################################################################
#Portability: ARM GCC
#
###################################################################################################

SCRIPTS = tcryp.sh

all:
	$(MAKE) -C ../../../third_party_suite/crypto_test/

install:
	$(MAKE) -C ../../../third_party_suite/crypto_test/ install
	ln -f $(SCRIPTS) ../../../bin/

clean:
	$(MAKE) -C ../../../third_party_suite/crypto_test/ clean

