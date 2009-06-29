#Copyright 2008-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
#!/bin/sh
##############################################################################
#Revision History:
#                 Modification     Tracking
#Author               Date          Number    Description of Changes
#---------------  ------------    ----------  ----------------------
# Z.Spring          2/12/2008     ENGR100588   Initial ver, enabled, move from
#                                           testscripts to vte_test_suite
##############################################################################
#
# @file   wifi_testap010.sh
#
# @brief  Test 802.11b/g AP with wrong 128-bit WEP security, set wrong key
#
###############################################################################
RC=0

ifconfig eth1 up
ifconfig eth1 192.168.1.155
sleep 2

ifconfig
sleep 2

iwconfig eth1 mode managed
sleep 3

iwconfig eth1 key 12345678901234567890
sleep 3

iwconfig eth1 essid TestAp010
sleep 3

ping -c 4 192.168.1.1 ||RC=$?

echo "========================================"
if [ $RC -ne 0 ]
then
    echo -e "\n\t ${0##*/} Test PASS"
else
    echo -e "\n\t ${0##*/} Test FAIL"
fi
echo "========================================"

exit $RC
