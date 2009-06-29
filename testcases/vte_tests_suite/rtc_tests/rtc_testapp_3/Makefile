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
#    @file   Makefile
#
#    @brief  Makefile to compile the rtc_testapp_3 test.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#V.Becker/rc023c         25/05/2004   TLSbo38652  Change file name 
# Ludovic DELASPRE/RC149C     11/06/2004     TLSbo40109   Replace hard include path by a variable
#L.Delaspre/rc149c            09/12/2004     TLSbo40142   update with Freescale identity
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

#TO BE UPDATED WITH THE TEST APPLICATION NAME
TARGET=rtc_testapp_3

all: $(TARGET)
	@echo "****************************************************"
	@echo "** ALL : rtc_testapp_3          test application **"
	@echo "***************************************************"
	@echo

install:
	ln -f $(TARGET) ../../../bin
	@echo "****************************************************"
	@echo "** INSTALL : rtc_testapp_3                        **"
	@echo "****************************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : rtc_test_3.h



