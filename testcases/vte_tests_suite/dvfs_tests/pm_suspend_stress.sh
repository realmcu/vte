#!/bin/sh
#Copyright 2008-2010 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
#
#
# File :        pm_suspend_stress.sh
#
# Description:  Stress test of suspend and resume
#
#==============================================================================
#Revision History:
#                      Modification     Tracking
# Author                   Date          Number    Description of Changes
#-------------------   ------------    ----------  ----------------------
# Spring Zhang          Jan.19,2010      n/a        Initial version
# Spring Zhang          May.10,2010      n/a        Delay the time

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
    RC=0
    # Total number of test cases in this file. 
    export TST_TOTAL=1

    # The TCID and TST_COUNT variables are required by the LTP 
    # command line harness APIs, these variables are not local to this program.

    # Test case identifier
    export TCID="PM_SUSPEND_STRESS"
    # Set up is initialized as test 0
    export TST_COUNT=0
    # Initialize cleanup function to execute on program exit.
    # This function will be called before the test program exits.
    trap "cleanup" 0

    return $RC
}

cleanup()
{
    RC=0
    return $RC
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

    i=0
    while [ $i -lt 1000 ]; do
        rtc_testapp_6 -m standby -T 15
        echo -e "\033[;31m PM_SUSPEND_STRESS: Test STOP mode times: $i \033[0m"
        i=`expr $i + 1`
    done

    i=0
    while [ $i -lt 1000 ]; do
        rtc_testapp_6 -m mem -T 15
        echo -e "\033[;31m PM_SUSPEND_STRESS: Test WAIT mode times: $i \033[0m"
        i=`expr $i + 1`
    done

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
