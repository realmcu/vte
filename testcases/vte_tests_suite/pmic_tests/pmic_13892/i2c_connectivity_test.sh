#Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
# File :        i2c_connectivity_test.sh
#
# Description:  This is a test to i2c connectivity of pmic13892.
#    
#=====================================================================================
#Revision History:
#                            Modification     Tracking
# Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
# Blake                      20081015

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
    export TCID="i2c_connectivity"
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
    info=`cat /proc/cpuinfo | grep "Revision" | grep "51.*" | wc -l`
    if [ $info -eq 1 ]
    then
        I2C_DIR=/sys/class/i2c-adapter/i2c-1/1-0008
    fi

    if [ ! -d $I2C_DIR ]
    then
        tst_resm TFAIL "There is no such directory: $I2C_DIR "
        RC=1
        return $RC
    fi

    return $RC
}

connectivity_test()
{
    RC=0

    result=`cat $I2C_DIR/mc13892_ctl; dmesg | grep "reg01" | wc -l`
    if [ $result -ne 0 ]
    then
        tst_resm TPASS "Pass to test connectivity"
    else 
        tst_resm TFAIL "Fail to test connectivity"
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
connectivity_test || exit $RC
