#!/bin/sh
###################################################################################################
#
#    @file   keypad_test.sh
#
#    @brief  shell script template for testcase design "key pad" is where to modify block.
#
###################################################################################################
#
#   Copyright (C) 2004-2008, Freescale Semiconductor, Inc. All Rights Reserved
#   THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#   BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#   Freescale Semiconductor, Inc.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#hake huang/-----             24/11/2008     N/A          Initial version
#
###################################################################################################



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
export TST_TOTAL=2

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

usage()
{
echo "Usage: "
echo "keypad_test.sh <case number>"
echo "case number is 1, 2, 3, 4"
echo "1 for module exist test"
echo "2 for evtest test"
echo "3 for loadkmap test"
echo "4 power test"
}

# Function:     test_case_01
# Description   - Test if module exist
# Type:         Smoke/BAT test auto
#
test_case_01()
{
#TODO give TCID
TCID="module exist test"
#TODO give TST_COUNT
TST_COUNT=1
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
if [ -e /dev/input/event0 ]
then
RC=0
fi

return $RC

}

# Function:     test_case_02
# Description   - Test event case
# Type:         function semi-auto
#
test_case_02()
{
#TODO give TCID
TCID="test_evtest_keypad"
#TODO give TST_COUNT
TST_COUNT=2
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
echo "please check every key except the reset and power key"
evtest /dev/input/event0

read -p "is the key value match?y/n" Rt

if [ $Rt == y ]
then
RC=0
fi

return $RC
}


# Function:     test_case_03
# Description   - Test loadkey case
# Type:         function semi-auto
#
test_case_03()
{
#TODO give TCID
TCID="test_loadkeymap_keypad"
#TODO give TST_COUNT
TST_COUNT=2
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

cmd=$(which loadkeys)
if [ -z $cmd ]
then
cmd=${LTPROOT}/../util/console_tool/loadkeys
$cmd ${LTPROOT}/testcases/bin/keymap.txt
else
$cmd ${LTPROOT}/testcases/bin/keymap.txt
fi

evtest /dev/input/event0

read -p "are the keys value match below?y/n" Rt

if [ $Rt == y ]
then
RC=0
fi

return $RC
}


# Function:     test_case_04
# Description   - Test power state resume
# Type:         function manual
#
test_case_04()
{
#TODO give TCID
TCID="test_loadkeymap_keypad"
#TODO give TST_COUNT
TST_COUNT=2
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

echo now enter power standby
echo "please press a key to wakeup system"
echo standby > /sys/power/state

echo now perform key test
evtest /dev/input/event0

read -p "are the keys value match below?y/n" Rt

if [ $Rt == y ]
then
RC=0
fi

return $RC
}

# main function

RC=0

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
3)
  test_case_04 || exit $RC
  ;;
*)
#TODO check parameter
  usage && exit 1
  ;;
esac

tst_resm TINFO "Test PASS"
