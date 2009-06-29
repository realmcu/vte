#Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
#    @brief  configuration file to compile the v4l_output test.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  ------------------------------------------
#KHOROSHEV.D                 09/29/2005      TLSbo55077  Initial version 
#
###################################################################################################
#Portability: ARM GCC
###################################################################################################

CFLAGS       += -Wall -I ../../../../include 
LOADLIBES    += -L../../../../lib -lltp -lm
PROGRAM = 
LIBRARY =

#SRCS = $(wildcard *.c)
#OBJS = $(patsubst %.c,%.o,$(SRCS))

#TARGET = mxc_v4l2_overlay mxc_v4l2_capture mxc_v4l2_output mxc_v4l2_still
TARGET = mxc_v4l2_output.sh mxc_v4l2_overlay.sh v4l2_basic_test.sh v4l2_still_test.sh 

all: $(TARGET)
	@echo "***********************************************"
	@echo "** ALL : V4L2 unit test                  **"
	@echo "***********************************************"
	@echo

install:
	@set -e ; for i in $(TARGET) ; do ln -f $$i ../../../bin ; done
	@echo "***********************************************"
	@echo "** INSTALL : V4L2 Basic testapp              **"
	@echo "***********************************************"
	@echo

clean:
	

#$(OBJS) : $(SRCS)
