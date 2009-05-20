#!/bin/sh
###################################################################################################
#
#    @file   rtc_test.sh
#
#    @brief  shell script template for testcase design "TODO" is where to modify block.
#
###################################################################################################
#
#   Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
#   THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#   BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#   Freescale Semiconductor, Inc.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#<Hake Huang>/-----             <2008-11-27>     N/A          Initial version
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
 echo "rtc_test.sh <test case ID>"
 echo "Test case ID <1/2>"
 echo "1: module existence test"
 echo "2: set rtc hardware and check"
}

# Function:     test_case_01
# Description   - Test if rtc module exist
#
test_case_01()
{
#TODO give TCID
TCID="rtc_module_exist"
#TODO give TST_COUNT
TST_COUNT=1
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
if [ -e /sys/class/rtc/rtc0 ]
then
RC=0
fi

return $RC

}


# Function:     test_case_02
# Description   - Test if stty hw rtc is ok
#
test_case_02()
{
#TODO give TCID
TCID="rtc_hw"
#TODO give TST_COUNT
TST_COUNT=1
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

date -u 032116582008
hwclock --systohc
#only check the year suppose enough
ret=$(hwclock | grep 2008 | wc -l)

if [ $ret ]
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
*)
  usage
  ;;
esac

tst_resm TINFO "Test PASS"







