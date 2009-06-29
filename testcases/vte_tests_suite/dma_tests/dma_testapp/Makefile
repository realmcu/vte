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
#    @brief  configuration file to compile the VTE DMA test applications.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#D.Simakov/b00236            09/01/2006      TLSbo76801  Initial version
#
###################################################################################################
#Portability: ARM GCC
#
###################################################################################################

MOD_DIR = ../dma_test_module
MXC_DIR = $(KLINUX_SRCDIR)/drivers/mxc

CFLAGS    +=    -Wall -I../../../../include -I$(KLINUX_BLTDIR)/include -I$(MOD_DIR) -I$(MXC_DIR) -DTEST_USER_SPACE
LOADLIBES +=    -L../../../../lib -lltp -lpthread

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

TARGET = dma_testapp

all: $(TARGET)
	rm -f $(OBJS)
	@echo "***********************************************"
	@echo "** ALL:      $(TARGET)                     **"
	@echo "***********************************************"
	@echo

install:
	cp -rf $(TARGET) ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGET)                     **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)
	@echo "***********************************************"
	@echo "** CLEAN :   $(TARGET)                     **"
	@echo "***********************************************"
	@echo

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

