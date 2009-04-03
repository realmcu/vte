###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the sockets test.
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
#S.ZAVJALOV/-----             10/06/2004     TLSbo39738   Initial version
#Ludovic Delaspre / rc149c    22/06/2004     TLSbo39941   VTE 1.3 integration
#L.Delaspre/rc149c            08/12/2004     TLSbo40142   update with Freescale identity
#
###################################################################################################

CFLAGS+=	-Wall -I../../../../include
LOADLIBES+=	-L../../../../lib -lltp

SRCS=socket_client_main.c socket_client_test.c socket_server_main.c socket_server_test.c 
OBJS=$(SRCS:.c=.o)

#TO BE UPDATED WITH THE TEST APPLICATION NAME
TARGET=socket_client socket_server

all: $(TARGET)
	@echo "***********************************************"
	@echo "** ALL : $(TARGET) test applications **"
	@echo "***********************************************"
	@echo

socket_client: socket_client_main.o socket_client_test.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) $(LOADLIBES)

socket_server: socket_server_main.o socket_server_test.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) $(LOADLIBES)

$(OBJS) : $(SRCS)

$(SRCS) :socket_client_test.h  socket_server_test.h

install:
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGET) **"
	@echo "***********************************************"
	ln -f $(TARGET) ../../../bin
	@echo

clean:
	rm -f $(OBJS) $(TARGET)
