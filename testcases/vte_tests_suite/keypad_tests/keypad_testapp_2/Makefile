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
#        @file   Makefile
#
#        @brief  Makefile to compile the Keypad test 2
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  ------------------------------------------
#V.Becker/rc023c              01/04/2004     TLSbo38735   Initial version
#V.Becker/rc023c              17/05/2004     TLSbo38735   MXC Keypad configuration
#V.Becker/rc023c              25/05/2004     TLSbo38735   Change file name
#L.Delaspre/rc149c            22/06/2004     TLSbo40109   Replace hard include path by a variable
#L.Delaspre/rc149c            16/08/2004     TLSbo40891   VTE 1.4 integration
#L.Delaspre/rc149c            08/12/2004     TLSbo40142   update with Freescale identity
#A.Ozerov/NONE                10/01/2006     TLSbo61037   Update in accordance with linux-2.6.10-rel-L26_1_15
#
###################################################################################################
#Portability: ARM GCC
#
###################################################################################################
CFLAGS = -Wall -I../../../../include -I$(KLINUX_BLTDIR)/include/ -I$(KLINUX_SRCDIR)/include/
LOADLIBES = -L../../../../lib -lltp -lpthread

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

#TO BE UPDATED WITH THE TEST APPLICATION NAME
TARGET=keypad_testapp_2

all: $(TARGET)
	rm -f $(OBJS)
	@echo "***********************************************"
	@echo "** ALL : $(TARGET) test application   **"
	@echo "***********************************************"
	@echo

install:
	ln -f $(TARGET) ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGET)                **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)
	@echo "***********************************************"
	@echo "** CLEAN : $(TARGET)                  **"
	@echo "***********************************************"
	@echo

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : keypad_test_2.h
