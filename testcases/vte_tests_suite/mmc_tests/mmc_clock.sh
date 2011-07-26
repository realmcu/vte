#!/bin/sh -x
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

if [ ! -e "/dev/mmcblk0p1" ];then
RC=1
fi

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
#this is to test in normal state nfs no sd clock is on.
#and in read time the sd clock is ON
RC=0
ss=0
list=$(cat /proc/cpu/clocks | grep -i sdhc | awk '{print $3}')
for i in $list
do 
	ss=$(expr $ss + $i)
done 

if [ $ss -eq 0 ]; then
	dd if=/dev/mmcblk0p1 of=/dev/null bs=1024k count=1000 &
	sleep 1
	ss=0
	list=$(cat /proc/cpu/clocks | grep -i sdhc | awk '{print $3}')
	for i in $list
		do 
		ss=$(expr $ss + $i)
	done 
	if [ $ss -eq 0 ] ; then
		return 1
	fi
	return 0
fi
return 1
