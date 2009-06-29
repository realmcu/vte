#Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
#!/bin/sh

#!/bin/sh
# Used for ad-hoc test between two boards
RC=0

ifconfig eth1 down
sleep 3

ifconfig eth1 192.168.1.122
ifconfig eth1 up
sleep 3

iwconfig eth1 mode ad-hoc
sleep 3

iwconfig eth1 key off
sleep 1

iwconfig eth1 essid TestAdhoc005w
sleep 10

ping -c 4 192.168.1.100 ||RC=$?

echo "========================================"
if [ $RC -ne 0 ]
then
    echo -e "\n\t ${0##*/} Test PASS"
else
    echo -e "\n\t ${0##*/} Test FAIL"
fi
echo "========================================"

exit $RC
