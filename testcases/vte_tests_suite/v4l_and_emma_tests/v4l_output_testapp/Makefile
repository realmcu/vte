#Copyright 2006-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
#Kardakov Dmitriy/ID         09/11/06        TLSbo76802  Initial version
###################################################################################################
#Portability: ARM GCC
###################################################################################################

CFLAGS       += -Wall -I ../../../../include 
LOADLIBES    += -L../../../../lib -lltp -lm
PROGRAM = 
LIBRARY =

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

TARGET = v4l_output_testapp


all: $(TARGET)
	@echo "****************************************************************"
	@echo "** ALL : V4L output testapp test application                  **"
	@echo "***********************************************"****************
	@echo

install:
	ln -f $(TARGET) ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : V4L output testapp              **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : v4l_output_test.h