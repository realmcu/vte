#Copyright (C) 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
##############################################################################
#
#   @file   Makefile
#
#   @brief  configuration file to compile  MMC/SD device read/write testcase with locking switch
#
#   @par Portability:
#           Indicate if this module is portable to other compilers or platforms.
#           If not, indicate specific reasons why is it not portable.
#
########################### REVISION HISTORY #################################
#
#Author (core ID)      Date         CR Number    Description of Changes
#-------------------   ----------   ----------   ------------------------------
#S.ZAVJALOV/zvjs001c   04/04/2005   tlsbo45047   Initial version
#L.Delaspre/rc149c     12/04/2005   TLSbo48760   VTE 1.9 integration
#
#
##############################################################################

CFLAGS+=          -Wall -I../../../../include
LOADLIBES+=       -L../../../../lib -lltp

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

TARGET=lock_mmc_test

all: $(TARGET)
	@echo "***********************************************"
	@echo "** ALL : LOCK_MMC_TEST                       **"
	@echo "***********************************************"
	@echo

install:
	ln -f $(TARGET) ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : LOCK_MMC_TEST                   **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : lock_mmc_test.h
