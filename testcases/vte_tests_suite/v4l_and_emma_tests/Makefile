###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the VTE MC13783 test applications.
#
###################################################################################################
#
#                              Motorola Confidential Proprietary
#                   (c) Copyright 2004, Motorola, Inc.  All rights reserved.  
#            
#Presence of a copyright notice is not an acknowledgement of publication.  
#This software file listing contains information of Motorola, Inc. that is of a confidential and 
#proprietary nature and any viewing or use of this file is prohibited without specific written 
#permission from Motorola, Inc.
#     
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#Kardakov Dmitriy/ID         09/11/06        TLSbo76802  Initial version     
###################################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms. 
#             If not, indicate specific reasons why is it not portable.
#
###################################################################################################
export CFLAGS=-Wall -I../../../../include -I$(KLINUX_SRCDIR)/include
export LOADLIBES=-L../../../../lib -lltp -lpthread -lm

SUBDIRS = v4l_capture_testapp \
v4l_output_testapp 

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done

