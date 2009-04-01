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
# Modified by Spring, 2008.9.10, modify WPA-PSK config to wrong
RC=0

ifconfig eth1 up
ifconfig eth1 192.168.1.155
sleep 2

ifconfig
sleep 3

iwconfig eth1 mode managed
sleep 3

iwconfig eth1 key off
sleep 2

/usr/local/sbin/wpa_supplicant -c WPA-PSK-wrong.conf -i eth1 &
wpid=$!
sleep 3

iwconfig eth1 essid TestAp004
sleep 3

ping -c 4 192.168.1.1 ||RC=$?

#kill wpa routine
kill $wpid

#negative test
echo "========================================"
if [ $RC -ne 0 ]
then
    echo -e "\t${0##*/} Test PASS"
else
    echo -e "\t${0##*/} Test FAIL"
fi
echo "========================================"


[ $RC != 0 ] ||exit $?
