#Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
####################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the sockets test.
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

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done
    
install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
