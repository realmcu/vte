#Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
#!/bin/sh
#2008.9.10 by Spring, add ping count
RC=0

ifconfig eth1 up
ifconfig eth1 192.168.1.155
sleep 2

ifconfig
sleep 2

iwconfig eth1 mode managed
sleep 3

iwconfig eth1 key 12345678901234567890aaabbb
sleep 3

iwconfig eth1 essid TestAp003
sleep 3

ping -c 4 192.168.1.1 ||RC=$?

echo "========================================"
if [ $RC -eq 0 ]
then
    echo -e "\n\t ${0##*/} Test PASS"
else
    echo -e "\n\t ${0##*/} Test FAIL"
fi
echo "========================================"

exit $RC
