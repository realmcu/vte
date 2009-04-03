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
#!/bin/sh
# Used for ad-hoc test between two boards
RC=0

ifconfig eth1 192.168.1.122
sleep 3

iwconfig eth1 mode ad-hoc
sleep 3

iwconfig eth1 key 12345678901234567890aaabbb
sleep 3

iwconfig eth1 essid TestAdhoc007
sleep 10

ping -c 4 192.168.1.100 ||RC=$? 

echo "========================================"
if [ $RC -eq 0 ]
then
    echo -e "\n\t ${0##*/} Test PASS"
else
    echo -e "\n\t ${0##*/} Test FAIL"
fi
echo "========================================"

exit $RC
