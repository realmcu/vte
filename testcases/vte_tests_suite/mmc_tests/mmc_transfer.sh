#!/bin/sh -x
# Copyright (C) 2011-2012 Freescale Semiconductor, Inc. All Rights Reserved.
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

# Function:     setup
#
# Description:  - Check if required commands exits
#               - Export global variables
#               - Check if required config files exits
#               - Create temporary files and directories
#
# Return        - zero on success
#               - non zero on failure. return value from commands ($RC)
setup()
{
#TODO Total test case
export TST_TOTAL=1

export TCID="setup"
export TST_COUNT=0
RC=0

trap "cleanup" 0

#TODO add setup scripts

return $RC
}

# Function:     cleanup
#
# Description   - remove temporary files and directories.
#
# Return        - zero on success
#               - non zero on failure. return value from commands ($RC)
cleanup()
{
RC=0

#TODO add cleanup code here
return $RC
}

setup

#main

rm -rf /tmp/rs
(time -p dd if=/dev/mmcblk0p1 of=/dev/null bs=1M count=10) 2>/tmp/rs
rs=$(cat /tmp/rs | grep real | awk '{print $2}')
#time -p dd if=/dev/mmcblk0p1 of=/dev/null bs=1M count=10 2>/tmp/us
user=$(cat /tmp/rs | grep user | awk '{print $2}')
#time -p dd if=/dev/mmcblk0p1 of=/dev/null bs=1M count=10 2>/tmp/sys 
sys=$(cat /tmp/rs | grep sys | awk '{print $2}')
rm -rf /tmp/rs

judge=$(echo "$rs > ( $user + $sys)" | bc)
if [  $judge -eq 1 ]; then
  echo "wait time implys there are DMA transfer, test PASS"
	exit 0
fi
echo "test FAIL, no DMA involved"
exit 1

