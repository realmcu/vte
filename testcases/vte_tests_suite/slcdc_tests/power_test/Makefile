#Copyright (C) 2004-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
#    @brief  configuration file to compile the sleep test that checks SLCDC driver by
#            putting device to the various VESA blanking levels.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#I. Semenchukov/smng001c      21/09/2004     TLSbo41672   Initial version 
#L. Delaspre/rc149c           08/10/2004     TLSbo42821   VTE 1.5.1 integration 
#L.Delaspre/rc149c            08/12/2004     TLSbo40142   update with Freescale identity
#
###################################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms. 
#             If not, indicate specific reasons why is it not portable.
#
###################################################################################################

CFLAGS+=	-Wall -I../../../../include -I$(KLINUX_SRCDIR)/include
LOADLIBES+=	-L../../../../lib -lltp

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

#TO BE UPDATED WITH THE TEST APPLICATION NAME
TARGET=lcd_testapp_power
 
all: $(TARGET)
	@echo "***********************************************"
	@echo "** ALL : lcd_testapp_power application       **"
	@echo "***********************************************"
	@echo

install:
	@echo "***********************************************"
	@echo "** INSTALL : lcd_testapp_power               **"
	@echo "***********************************************"
	ln -f $(TARGET) ../../../bin
	@echo

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : lcd_power_test.h
