#Copyright 2010 Freescale Semiconductor, Inc. All Rights Reserved.
#
#This program is free software; you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation; either version 2 of the License, or
#(at your option) any later version.
#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#You should have received a copy of the GNU General Public License along
#with this program; if not, write to the Free Software Foundation, Inc.,
#51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
##############################################################################
#
#   @file   Makefile
#
#   @brief  configuration file template to compile the TEMPLATE_testapp_X test.
#
#   @par Portability:
#           Indicate if this module is portable to other compilers or platforms.
#           If not, indicate specific reasons why is it not portable.
#
########################### REVISION HISTORY #################################
#
#Author (core ID)      Date         CR Number    Description of Changes
#-------------------   ----------   ----------   ------------------------------
#Developer Name/ID     DD/MM/YYYY   TLSboXXXXX   BRIEF desc. of changes 
#Delaspre/rc149c       07/12/2004   TLSbo40142   update copyrights with Freescale
#
##############################################################################

CFLAGS+=	-Wall -I../../../../include
LOADLIBES+=	-L../../../../lib -lltp

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

TARGET=TEMPLATE_testapp_X

all: $(TARGET)
	@echo "***********************************************"
	@echo "** ALL : TEMPLATE_testapp_X built            **"
	@echo "***********************************************"
	@echo

install:
	ln -f $(TARGET) ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : TEMPLATE_testapp_X set          **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : TEMPLATE_test_X.h



