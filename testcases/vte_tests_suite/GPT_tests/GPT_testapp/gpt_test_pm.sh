#!/bin/bash
#Copyright (C) 2012 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
################################################################################
#
#    @file   gpt_test_pm.sh
#
#    @brief  shell script template for testcase design "TODO" is where to modify
#    block.
#
################################################################################
#Revision History:
#
#Author                                     Date        Description of Changes
#------------------------------------   ------------    ------------------------
#Andy Tian                               2012-03-06      Initial version
#
################################################################################

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
    #create a tmp file to store gpt_testapp log
    touch /tmp/gpt.log || exit 1 

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
    #remove the tmp log file
    rm -f /tmp/gpt.log || exit 1
    return $RC
}


# Function:     test_case_01
# Description   - Test if GPT works normal after syspend/resume
#
test_case_01()
{
    #TODO give TCID
    TCID="GPT_PM_Test"
    #TODO give TST_COUNT
    TST_COUNT=1
    RC=1

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "

    #TODO add function test scripte here

    #running gpt_testapp in bg which will run 20 secs if working well
    #gpt.log stores the gpt_testapp running log
    gpt_testapp > /tmp/gpt.log &  
    pid1=$!

    #sleep 7 secs to make sure do suspend when gpt_testapp executing sleep() statement
    sleep 7
    rtc_testapp_6 -T 3 || exit $RC

    #wait for gpt_testapp returns
    wait $pid1 

    #print out and analyze log of gpt_testapp to make sure gpt works normally
    #after suspend
    cat /tmp/gpt.log
    grep "Test case work well!" /tmp/gpt.log || exit $RC
    RC=0

    return $RC
}


usage()
{
    echo "usage $0 <case ID>"
    exit 1
}

# main function

RC=0

#TODO check parameter
if [ $# -ne 1 ]
then
    usage $0
fi

setup || exit $RC

case "$1" in
1)
  test_case_01 || exit $RC
  ;;
*)
  usage $0
  ;;
esac

tst_resm TINFO "Test PASS"
