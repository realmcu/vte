#!/bin/sh
##############################################################################
#
#  Copyright 2004-2008 Freescale Semiconductor, Inc. All Rights Reserved.
#
##############################################################################
#
#  The code contained herein is licensed under the GNU Lesser General Public
#  License.  You may obtain a copy of the GNU Lesser General Public License
#  Version 2.1 or later at the following locations:
#
#  http://www.opensource.org/licenses/lgpl-license.html
#  http://www.gnu.org/copyleft/lgpl.html
#
##############################################################################
ifconfig eth1 up
ifconfig eth1 192.168.1.155
sleep 2

ifconfig
sleep 3

iwconfig eth1 essid TestAp009
sleep 3

iwconfig eth1 key 12345678901234567890
sleep 3

ping 192.168.1.1
