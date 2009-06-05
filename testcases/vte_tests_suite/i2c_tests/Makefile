####################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the sockets test.
#
####################################################################################################
#
#    Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
#    THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#    BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#    Freescale Semiconductor, Inc.
#
####################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#V.Khalabuda/hlbv001          20/07/2004     TLSboXXXXX  Initial version
#L.Delaspre/rc149c            13/09/2004     TLSbo42022  VTE 1.5 integration
#L.Delaspre/rc149c            20/10/2004     TLSbo43922  I2C updates for L26.1.5 LSP
#D.Kazachkov/e1403c           29/10/2004     TLSbo43941  I2C updates
#L.Delaspre/rc149c            08/12/2004     TLSbo40142  update with Freescale identity
#L.Delaspre/rc149c            02/03/2005     TLSbo47726  VTE 1.8 integration
#V.KHalabuda/hlbv001          15/04/2005     TLSbo48461  add testapp_4
#V.KHalabuda/b00306           15/04/2005     TLSbo48461  mv functionality from testapp_4 to testapp_1
#
####################################################################################################

SUBDIRS =  i2c_testapp

#tina changed i2c_module \
#i2c_testapp_1 \
#i2c_testapp_3

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done
    
install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
