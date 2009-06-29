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
#    @brief  configuration file to compile the dvfs_dptc_testapp test.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#V.Khalabuda/b00306           22/11/2006     TLSbo83054  Initial version
#
###################################################################################################
#Portability: ARM GCC
#
###################################################################################################
MXC_DIR    =    $(KLINUX_SRCDIR)/drivers/mxc/pm

CFLAGS    +=    -Wall -I../../../../include -I$(KLINUX_BLTDIR)/include -I$(MXC_DIR)
LOADLIBES +=    -L../../../../lib -lltp

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

TARGET = dvfs_dptc_testapp
TABLE = dptc_table

all: $(TARGET)
	@rm -f $(OBJS)
	@echo "***********************************************"
	@echo "** ALL : $(TARGET)                        **"
	@echo "***********************************************"
	@echo

install:
	ln -f $(TARGET) ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGET)                    **"
	@echo "***********************************************"
	@echo

clean:
	@rm -f $(OBJS) $(TARGET) $(TABLE)
	@echo "***********************************************"
	@echo "** CLEAN : $(TARGET)                      **"
	@echo "***********************************************"
	@echo

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : dvfs_dptc_test.h
