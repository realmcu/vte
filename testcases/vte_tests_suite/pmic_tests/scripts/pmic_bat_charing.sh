#!/bin/bash -x
# Copyright (C) 2011 Freescale Semiconductor, Inc. All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
#    @file   pmic_bat_charing.sh
#
#    @brief  shell script for battery charing test.
#
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#Hake                         2011/04/25        NA        init pmic bat test
#-------------------------   ------------    ----------  -------------------------------------------
# 

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
RC=1
trap "cleanup" 0

modprobe mc34708_battery

RC=0

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

modprobe -r mc34708_battery

return $RC
}


# Function:     test_case_01
# Description   - Test if basic charging is OK
#  
test_case_01()
{
#TODO give TCID 
TCID="test_aux_charging"
#TODO give TST_COUNT
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

#test list
clear
echo "please ensure you boot from battery"
read -p "please ensure the dc 5v is switch on, press Enter to continue"

u_online=$(cat /sys/class/power_supply/*aux_charger/online)
if [ "$u_online" = "0" ]; then
RC=1
fi

sleep 2
bat_status=$(cat /sys/class/power_supply/*_bat/status)
if [ ! "$bat_status" = "Charging"  ];then
RC=1
fi

bat_charge_current=$(cat /sys/class/power_supply/ripley_bat/charge_now | grep "-")
if [ -z "$bat_charge_current" ]; then
RC=$(expr $RC + 1)
fi

return $RC
}

# Function:     test_case_02
# Description   - Test if basic charging is OK
#  
test_case_02()
{
#TODO give TCID 
TCID="test_usb_charging"
#TODO give TST_COUNT
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

#test list
clear
echo "please ensure you boot from battery"
read -p "please ensure the usb 5v is switch on, press Enter to continue"

sleep 2

u_online=$(cat /sys/class/power_supply/*usb_charger/online)
if [ "$u_online" = "0" ]; then
RC=1
fi

bat_status=$(cat /sys/class/power_supply/*_bat/status)
if [ ! "$bat_status" = "Charging"  ];then
RC=$(expr $RC + 1)
fi

bat_charge_current=$(cat /sys/class/power_supply/ripley_bat/charge_now | grep "-")
if [ -z "$bat_charge_current" ]; then
RC=$(expr $RC + 1)
fi

return $RC
}


test_case_02()
{
#TODO give TCID 
TCID="test_charging_PM"
#TODO give TST_COUNT
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

#test list
clear
echo "please ensure you boot from battery"
read -p "please ensure the usb 5v is in, press Enter to continue"

sleep 2

u_online=$(cat /sys/class/power_supply/*usb_charger/online)
if [ "$u_online" = "0" ]; then
RC=1
fi

bat_status=$(cat /sys/class/power_supply/*_bat/status)
if [ ! "$bat_status" = "Charging"  ];then
RC=$(expr $RC + 1)
fi

bat_charge_current=$(cat /sys/class/power_supply/ripley_bat/charge_now | grep "-")
if [ -z "$bat_charge_current" ]; then
RC=$(expr $RC + 1)
fi

rtc_testapp_6 -T 15

bat_status=$(cat /sys/class/power_supply/*_bat/status)
if [ ! "$bat_status" = "Charging"  ];then
RC=$(expr $RC + 1)
fi

bat_charge_current=$(cat /sys/class/power_supply/ripley_bat/charge_now | grep "-")
if [ -z "$bat_charge_current" ]; then
RC=$(expr $RC + 1)
fi

return $RC
}

usage()
{
echo "1 aux charging test"	
echo "2 usb charging test"	
echo "3 charging pm charging test"	
}


# main function

RC=0

setup || exit $RC

case "$1" in
1)
  test_case_01 || exit $RC 
  ;;
2)
  test_case_02 || exit $RC 
  ;;
3)
  test_case_03 || exit $RC 
  ;;
*)
  usage
  ;;
esac

tst_resm TINFO "Test PASS"
