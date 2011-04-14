#!/bin/sh
###############################################################################
# Copyright (C) 2011 Freescale Semiconductor, Inc. All Rights Reserved.
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
# Spring Zhang        04/14/2011        n/a      Initial version
#
###############################################################################

SUBDIRS=
TARGETS=mx53_clks_sysidle mx53_clks_useridle mx53_voltage

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done
	@for i in $(TARGETS); do \
		ln -f $$i ../../../../bin/ ;\
		echo "***********************************************" ; \
		echo "** INSTALL : $$i " ;\
		echo "***********************************************" ; \
	done
	@echo

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done

