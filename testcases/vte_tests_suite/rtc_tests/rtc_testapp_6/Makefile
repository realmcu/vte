#Copyright (C) 2008-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
#    @brief  configuration file to compile the rtc_test2 test.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#Blake                       12/29/2008
#
###################################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms. 
#             If not, indicate specific reasons why is it not portable.
#
###################################################################################################

CFLAGS+= -Wall -I../../../../include -I$(KLINUX_SRCDIR)/include/
LOADLIBES+=	-L../../../../lib -lltp

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

#TEST APPLICATION NAME
TARGET=rtc_testapp_6

all: $(TARGET)
	@echo "***********************************************"
	@echo "** ALL : rtc_testapp_6      test application **"
	@echo "***********************************************"
	@echo

install:
	ln -f $(TARGET) ../../../bin
	ln -f rtc_test_6.sh ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : rtc_testapp_6                   **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : rtc_test_6.h
