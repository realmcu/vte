###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the opl_mirror_TestApp test.
#
###################################################################################################
#
#   Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
#   THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#   BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#   Freescale Semiconductor, Inc.
#     
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#Kazachkov,Smirnov,Simakov    09/09/2004     TLSbo41679   BRIEF description of changes made 
#Ludovic Delaspre / rc149c    17/09/2004     TLSbo42022   VTE 1.5 integration 
#Ludovic DELASPRE/RC149C      02/12/2004     TLSbo45299   CHUCK drop2.5 W449 test campaign
#
###################################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms. 
#             If not, indicate specific reasons why is it not portable.
#
###################################################################################################

# to compile under Laramade Linux Server
LIB_DIR = /local/CHUCK/nfs_vte/vte-1.6_chuck/opl/lib
HEADER_DIR = /local/CHUCK/nfs_vte/vte-1.6_chuck/opl/include

CFLAGS       += -Wall -I../../../../include -I$(HEADER_DIR)
LOADLIBES    += -L../../../../lib -lltp -lpthread -lm

LIBRARY = $(LIB_DIR)/opl.a
OBJ_DIR       = ./obj
IMAGE_DIR     = ../Images

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

TARGET = opl_mirror_testapp


all: $(TARGET)
	@echo "***********************************************"
	@echo "** OPL library mirroring function test app   **"
	@echo "***********************************************"
	@echo

install:
	ln -f $(TARGET) ../../../bin
	cp -f $(IMAGE_DIR)/* ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : OPL lib test app                **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) $(LIBRARY) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : opl_mirror_test.h



