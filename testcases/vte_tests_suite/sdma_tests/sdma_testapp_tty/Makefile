#Copyright 2004-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
####################################################################################################
#                                                                                                  #
#    @file   Makefile                                                                              #
#                                                                                                  #
#    @brief  configuration file to compile the sockets test.                                       #
#                                                                                                  #
####################################################################################################
#Revision History:                                                                                 #
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes                    #
#-------------------------   ------------    ----------  ------------------------------------------#
#S.ZAVJALOV/-----             19/07/2004     TLSbo40259   Initial version                          #
#L.DELASPRE/rc149c            03/08/2004     TLSbo40891   VTE 1.4 integration                      #
#L.Delaspre/rc149c            08/12/2004     TLSbo40142   update with Freescale identity
#                                                                                                  #
####################################################################################################

CFLAGS+=	-Wall -I../../../../include -I$(KLINUX_SRCDIR)/include -I$(KLINUX_BLTDIR)/include
LOADLIBES+=	-L../../../../lib -lltp

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

#TO BE UPDATED WITH THE TEST APPLICATION NAME
TARGET=sdma_tty

all: $(TARGET)
	@echo "***********************************************"
	@echo "** ALL : TEST_${TARGET}                      **"
	@echo "***********************************************"
	@echo

install:
	ln -f $(TARGET) ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : TEST_${TARGET}                  **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : sdma_tty_test.h
