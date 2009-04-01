###############################################################################
#
#    @file   Makefile
#
#    @brief  dt.
#
##############################################################################
#
#  Copyright 2004-2008 Freescale Semiconductor, Inc. All Rights Reserved.
#
###############################################################################
#
#  The code contained herein is licensed under the GNU Lesser General Public
#  License.  You may obtain a copy of the GNU Lesser General Public License
#  Version 2.1 or later at the following locations:
#
#  http://www.opensource.org/licenses/lgpl-license.html
#  http://www.gnu.org/copyleft/lgpl.html
#
###############################################################################
#Revision History:
#                 Modification     Tracking
#Author               Date          Number    Description of Changes
#---------------  ------------    ----------  ----------------------
# Victor Cui      01/12/2008                  Initial ver
#
#
###############################################################################
#Portability: Indicate if this module is portable to other compilers
#             or platforms.
#             If not, indicate specific reasons why is it not portable.
#
###############################################################################


# Notes by Victor
#source:  http://home.comcast.net/~scsiguy/SCSI_FAQ/RMiller_Tools/dt.html
#download "Linux Intel Data Test Program dt Kit" ---->  intel-dt.tar.tar 

SUBDIRS = dt.d

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
