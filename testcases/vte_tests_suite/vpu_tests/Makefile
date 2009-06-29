#Copyright 2007-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file.
#
###################################################################################################
BIN_DIR= ../../
SUBDIRS = test_script

all:

install:
	@echo "*install vpu test scripts*"
	-@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done
	-ln -f ./configs/* $(BIN_DIR)/bin

clean:
