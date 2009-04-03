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
ifconfig eth1 up
sleep 3

ifconfig eth1 192.168.1.100
sleep 3

iwconfig eth1 mode ad-hoc
sleep 3

iwconfig eth1 key 1234567890
sleep 3

iwconfig eth1 essid TestAdhoc006

#sleep 30
#iwconfig eth1 essid TestAp006 &

echo "=============================="
echo "        Config End            "
echo "=============================="
