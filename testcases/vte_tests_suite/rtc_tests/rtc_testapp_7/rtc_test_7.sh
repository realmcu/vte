#Copyright 2008-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
#!/bin/sh
#
#
# File :        rtc_test_7.sh
#
# Description:  wakeup system by rtc alarm.
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
    export TCID="rtc_test_7"
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

    rtc_testapp_7 -O on -T 5 
    sleep 2    
    echo standby > /sys/power/state
    
    rtc_testapp_7 -O off 
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
