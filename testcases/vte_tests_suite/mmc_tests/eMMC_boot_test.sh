#!/bin/sh -x
# Copyright (C) 2010 Freescale Semiconductor, Inc. All Rights Reserved.
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
echo "clean up..."
if [ -e $emmc_card ];then
  echo '0' > ${emmc_card}/boot_config
fi

return $RC
}

check_emmc_card()
{
  cards=$(ls /sys/devices/platform/*/mmc_host/*/*/boot_info)	
	for i in $cards
	do
   emmc_card=$(dirname $i)
	done
}
RC=0
emmc_card=""
check_emmc_card
setup

#main
if [ -e $emmc_card ];then
 echo '8' > ${emmc_card}/boot_config || exit 1
 is_ddr_support=$(echo $emmc_card | grep -i mxsdhci.2)
 if [ ! -z $is_ddr_support ]; then
    echo '1' > ${emmc_card}/boot_bus_config || exit 1
 fi
 sleep 1
 dd if=/dev/urandom of=urandom.bin bs=512 count=100
 card_path=$(ls $emmc_card/block)
 card_node=$(basename $card_path)
 dd if=urandom.bin of=/dev/${card_node} bs=512 count=100
 dd if=/dev/${card_node} of=emmc.bin bs=512 count=100
 cmp urandom.bin emmc.bin || RC=1
 rm -f urandom.bin emmc.bin
fi

return $RC
