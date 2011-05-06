#!/bin/sh
###############################################################################
# Copyright (C) 2010 Freescale Semiconductor, Inc. All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
###############################################################################
#Revision History:
#                    Modification     Tracking
#Author                  Date          Number    Description of Changes
#------------------  ------------    ----------  ----------------------
# Spring Zhang        08/13/2010        n/a      Initial version
#
###############################################################################

SUBDIRS=chk_configs
TARGETS=pwr_off_lcd.sh pwr_on_dvfs.sh pwr_mk_link.sh pwr_off_eth.sh pwr_on_conservative.sh
TARGETS+=pwr_mp3_128k.sh
TARGETS+=pwr_h264_720p_2770k.sh pwr_h264_720p_d1.sh pwr_h264_720p_9M.sh
TARGETS+=pwr_h264_1080p_10M.sh pwr_h264_1080p_20M.sh
TARGETS+=pwr_checkpoint.sh

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done
	@for i in $(TARGETS); do \
		ln -f $$i ../../../bin/ ;\
		echo "***********************************************" ; \
		echo "** INSTALL : $$i " ;\
		echo "***********************************************" ; \
	done
	@echo

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done

