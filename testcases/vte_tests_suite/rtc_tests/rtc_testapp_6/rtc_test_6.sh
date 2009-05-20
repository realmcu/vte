#!/bin/sh
#
#
# File :        rtc_test_6.sh
#
# Description:  wakeup system by rtc alarm.
#
#======================================================================
#
#   Freescale SemiconductorConfidential Proprietary
#  (c) Copyright 2004, Freescale Semiconductor, Inc.  All rights reserved.
#
#Presence of a copyright notice is not an acknowledgement of publication.
#This software file listing contains information of Freescale Semiconductor, Inc.
#that is of a confidential and proprietary nature and any viewing or use of
#this file is prohibited without specific written permission from
#Freescale Semiconductor, Inc.

#==============================================================================
#Revision History:
#                            Modification     Tracking
# Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  ----------------------
# Blake                      29/12/2008

#Set path variable to add vte binaries
#export TESTCASES_HOME= `pwd`
#export PATH=${PATH}:${TESTCASES_HOME}

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
    # Total number of test cases in this file.
    export TST_TOTAL=1

    # The TCID and TST_COUNT variables are required by the LTP
    # command line harness APIs, these variables are not local to this program.

    # Test case identifier
    export TCID="rtc_test_6"
    # Set up is initialized as test 0
    export TST_COUNT=0
    # Initialize cleanup function to execute on program exit.
    # This function will be called before the test program exits.
    trap "cleanup" 0
    RC=0
    return $RC
}

cleanup()
{
    echo "CLEANUP ";
}

env_test()
{
    RC=0

    # get cpu info
    DIR=/sys/power/state


    if [ ! -e $DIR ]
    then
        tst_resm TFAIL "There is no such this file: $DIR "
        RC=1
        return $RC
    fi

    return $RC
}

wakeup_test()
{
    RC=0

    rtc_testapp_6 -T 5 &
    sleep 3
    echo standby > /sys/power/state

    echo hello
    if [ $? -eq 0 ]
    then
        tst_resm TPASS " test case work as expected"
    else
        tst_resm TFAIL " test case do NOT work as expected"
        RC=1
    fi

    return $RC
}

# Function:     main
#
# Description:  - Execute all tests, exit with test status.
#
# Exit:         - zero on success
#               - non-zero on failure.
#
# Return value from setup, and test functions.
RC=0

setup  || exit $RC
env_test || exit $RC
wakeup_test || exit $RC
