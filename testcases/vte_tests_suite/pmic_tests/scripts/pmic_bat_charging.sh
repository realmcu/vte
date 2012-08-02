#!/bin/bash
# Copyright (C) 2011,2012 Freescale Semiconductor, Inc. All Rights Reserved.
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
#    @file   pmic_bat_charging.sh
#
#    @brief  shell script for battery charging test.
#
#Revision History:
#Author                          Date        Description of Changes
#Hake                         2011/04/25     init pmic bat test
#Spring                       2012/06/28     Add support for mx6 max8903
#-------------------------   ------------    -----------------------
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
    export TST_TOTAL=3

    export TCID="setup"
    export TST_COUNT=0
    RC=1
    trap "cleanup" 0

    platfm.sh || platfm_id=$?

    if [ $platfm_id -eq 50 ]; then
        modprobe mc34708_battery
        AC_CHARGER=/sys/class/power_supply/*aux_charger/online
        USB_CHARGER=/sys/class/power_supply/*usb_charger/online
        BATTERY=/sys/class/power_supply/*_bat/status
    else
        AC_CHARGER=/sys/class/power_supply/*-ac/online
        USB_CHARGER=/sys/class/power_supply/*-usb/online
        BATTERY=/sys/class/power_supply/*-charger/status
    fi


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

    #TODO add function test script here

    #test list
    clear
    echo "please ensure you boot from battery"
    read -p "please ensure the dc 5v is switch on, press Enter to continue"

    u_online=$(cat $AC_CHARGER)
    if [ "$u_online" = "0" ]; then
        RC=$(expr $RC + 1)
    fi

    sleep 2
    bat_status=$(cat $BATTERY)
    if [ ! "$bat_status" = "Charging"  ];then
        RC=$(expr $RC + 1)
    fi

    if [ $platfm_id -eq 50 ]; then
        bat_charge_current=$(cat /sys/class/power_supply/ripley_bat/charge_now | grep "-")
        if [ -z "$bat_charge_current" ]; then
            RC=$(expr $RC + 1)
        fi
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

    #TODO add function test script here

    #test list
    clear
    echo "please ensure you boot from battery"
    read -p "please ensure the usb 5v is switch on, press Enter to continue"

    sleep 2

    set -x
    u_online=$(cat $USB_CHARGER)
    if [ "$u_online" = "0" ]; then
        RC=$(expr $RC + 1)
    fi

    bat_status=$(cat $BATTERY)
    [ "$bat_status" = "Charging" ] || RC=$(expr $RC + 1)

    if [ $platfm_id -eq 50 ]; then
        bat_charge_current=$(cat /sys/class/power_supply/ripley_bat/charge_now | grep "-")
        if [ -z "$bat_charge_current" ]; then
            RC=$(expr $RC + 1)
        fi
    fi
    set +x

    return $RC
}


test_case_03()
{
    #TODO give TCID 
    TCID="Test_Charging_Standby"
    #TODO give TST_COUNT
    TST_COUNT=3
    RC=0

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "

    #TODO add function test script here

    #test list
    clear
    echo "please ensure you boot from battery"
    read -p "please ensure the usb 5v is in, press Enter to continue"

    sleep 2

    set -x

    u_online=$(cat $USB_CHARGER)
    if [ "$u_online" = "0" ]; then
        RC=$(expr $RC + 1)
    fi

    bat_status=$(cat $BATTERY)
    [ "$bat_status" = "Charging" ] ||RC=$(expr $RC + 1)

    if [ $platfm_id -eq 50 ]; then
        bat_charge_current=$(cat /sys/class/power_supply/ripley_bat/charge_now | grep "-")
        if [ -z "$bat_charge_current" ]; then
            RC=$(expr $RC + 1)
        fi
    fi

    rtc_testapp_6 -T 50

    bat_status=$(cat $BATTERY)
    [ "$bat_status" = "Charging" ] ||RC=$(expr $RC + 1)

    if [ $platfm_id -eq 50 ]; then
        bat_charge_current=$(cat /sys/class/power_supply/ripley_bat/charge_now | grep "-")
        if [ -z "$bat_charge_current" ]; then
            RC=$(expr $RC + 1)
        fi
    fi

    set +x

    return $RC
}

usage()
{
    echo "1 aux charging test"	
    echo "2 usb charging test"	
    echo "3 charging suspend test"	

    exit 1
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
