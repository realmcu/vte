#!/bin/sh
#Copyright (C) 2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
###################################################################################################
#
#    @file  battery_pm_test.sh 
#
#    @brief  shell script template for testcase battery  power manager.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#Eline Ma--b32072-----          20110623     N/A          Initial version
# 
###################################################################################################



# Function:     setup
#
# Description:  - Check if DS2438 smart battery monitor exits
#               - Export global variables
#               - Check if required config files exits
#               - Create temporary files and directories
#
# Return        - zero on success
#               - non zero on failure. return value from commands ($RC)
setup()
{
    #TODO Total test case
    export TST_TOTAL=5

    export TCID="setup"
    export TST_COUNT=0
    RC=1

    trap "cleanup" 0

    #TODO add setup scripts
    modprobe ds2438_battery
    #Define Battery Monitor name
    gBattery_Monitor=max17085_bat.0
    if [ -e /sys/devices/platform/max17085_bat.0 ]
    then
    RC=0
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
    modprobe -r ds2438_battery
    # Description   - Test if battery present on board

    #TODO add cleanup code here

    return $RC
}


# Function:     test_case_01
# Description   - Test if battery present on board
#  
test_case_01()
{
    #TODO give TCID 
    TCID="battery_exist_test"
    #TODO give TST_COUNT
    TST_COUNT=1
    RC=1

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "

    #TODO add function test scripte here
    if [ "1" = "$(cat /sys/devices/platform/${gBattery_Monitor}/power_supply/battery/present)" ]
    then 
        RC=0
    else
        tst_resm TINFO "FAIL"
    fi

    return $RC
}

# Function:     test_case_02
# Description    --Test battery driver support suspend/resume 10 times
test_case_02()
{
    #TODO give TCID
    TCID="battery_suspend_test"
    #GIVE TST_COUNT
    TST_COUNT=2
    RC=1

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID"

    i=0
    looptimes=10

    if [ "1" = "$(cat /sys/devices/platform/${gBattery_Monitor}/power_supply/battery/present)" ]
    then
        while [ "$i" -lt "$looptimes" ]
        do
            rtc_testapp_6 -m mem -T 10
            sleep 5
            i=`expr $i + 1`
        done
        RC=0
    else 
        tst_resm TINFO " Battery is not exsit "
        tst_resm TINFO " TEST FAIL"
    fi
    
    return $RC
}

# Function:          test_case_03
# Description        --Test Power source changes
test_case_03()
{
    #TODO give TCID 
    TCID="battery_source_changes"
    #TODO give TST_COUNT
    TST_COUNT=3
    RC=1
    FLAG="True"

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "
    #TODO add function test scripte here
    #Test battery on and AC off
    tst_resm TINFO "Make battery connect,if AC power is on,unplug it first"
    sleep 3
    read -p "If it's ready, pls input ENTER key:" temp

    if [ "0" = "$(cat /sys/devices/platform/${gBattery_Monitor}/power_supply/battery/present)" ] \
        || [ "1" = "$(cat /sys/devices/platform/${gBattery_Monitor}/power_supply/ac/online)" ]
    then
        tst_resm TINFO "Fail,battery off or AC on,Battery should be on and AC off"
        FLAG="False"
    fi
    
    RC=1
    #test battery and AC both on status
    tst_resm TINFO "Make battery connect,Plug the AC power:"
    sleep 3
    read -p "If it's ready, pls input ENTER key:" temp

    if [ "0" = "$(cat /sys/devices/platform/${gBattery_Monitor}/power_supply/battery/present)" ] \
    || [ "0" = "$(cat /sys/devices/platform/${gBattery_Monitor}/power_supply/ac/online)" ]    
    then
        tst_resm TINFO "Fail,Battery or AC is off,they should be both on"
        FLAG="False"
    fi
    
    #Test AC on ,battery dynamic off status
    tst_resm TINFO "Make battery and AC on,then uplug battery:"
    sleep 3
    read -p "If it's ready, pls input ENTER key:" temp
    if [ "1" = "$(cat /sys/devices/platform/${gBattery_Monitor}/power_supply/battery/present)" ] \
    || [ "0" = "$(cat /sys/devices/platform/${gBattery_Monitor}/power_supply/ac/online)" ]
    then
        tst_resm TINFO "Fail,Battery on or AC off,The battery should be off and AC on"
        FLAG="False"
    fi
   
    if [ "False" = "${FLAG}" ]
    then
        RC=1
        tst_resm TINFO "TEST FAIL"
    fi
    return $RC
}

# Function : test_case_04
# Description:  ----Test the battery usage status
test_case_04()
{

    #TODO give TCID
    TCID="Power_status_check"
    #TODO give TST_COUNT
    TST_COUNT=4
    RC=0
    
    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "
    tst_resm TINFO "Battery Status,Pls check"
    cat /sys/devices/platform/${gBattery_Monitor}/power_supply/battery/uevent
     
    tst_resm TINFO "AC power supply status,pls check"
    cat /sys/devices/platform/${gBattery_Monitor}/power_supply/ac/uevent
    cat /sys/devices/platform/${gBattery_Monitor}/power_supply/ac/online
    cat /sys/devices/platform/${gBattery_Monitor}/power_supply/ac/type
    
    read -p "If it is ok,pls print y" yes
    if [ "y" != "$yes" ]
    then
        RC=1
        tst_resm TINFO "TEST FAIL"
    fi

    return $RC
}

#Function: test_case_05
#Description:  -------The battery charger report complete charge

test_case_05()
{
    #TODO give TCID
    TCID="Battery_charging_full"
    #TODO give TST_COUNT
    TST_COUNT=5
    RC=1
 
    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "
     
    tst_resm TINFO "Make sure the battery charging enough time"
    if [ "100" = "$(cat /sys/devices/platform/${gBattery_Monitor}/power_supply/battery/capacity)" ] \
      && [ "Full" = "$(cat /sys/devices/platform/${gBattery_Monitor}/power_supply/battery/status)" ]
    then 
        tst_resm TINFO "Battery charing capacity are 100 and staus are full"
        RC=0
    else
       tst_resm TINFO "Battery capacity:should be 100"
       cat /sys/devices/platform/${gBattery_Monitor}/power_supply/battery/capacity
       tst_resm TINFO "Battery Charging status:should be Full"
       cat /sys/devices/platform/${gBattery_Monitor}/power_supply/battery/status
       tst_resm TINFO "TEST FAIL"
    fi 
   
    return $RC
}

usage()
{
echo "$0 [case ID]"
echo "1: check if battery on"
echo "2: Check suspend/wakeup 10 times when connecting battery"
echo "3: Check the battery and AC changes "
echo "4: Check the battery current staus "
echo "5: Check if the battery report full "
}

# main function
#TODO check parameter
if [ $# -ne 1 ]
then
usage
exit 1 
fi

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
4)
  test_case_04 || exit $RC
  ;;
5)
  test_case_05 || exit $RC
  ;;
*)
  usage
  ;;
esac

tst_resm TINFO "Test PASS"







