#!/bin/sh
#
#
# File :        dvfs_imx37.sh
#
# Description:  change cpu frequency 
#
#======================================================================
#
#   Freescale SemiconductorConfidential Proprietary
#  (c) Copyright 2004, Freescale Semiconductor, Inc.  All rights reserved.  
#            
#Presence of a copyright notice is not an acknowledgement of publication.  
#This software file listing contains information of Freescale Semiconductor, Inc. that is of a confidential and 
#proprietary nature and any viewing or use of this file is prohibited without specific written 
#permission from Freescale Semiconductor, Inc.
     
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
    export TCID="dvfs_imx37_test"
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
    local result=0
    PLATFORM=0

    # Get cpu info and which platform
    result=`cat /proc/cpuinfo | grep "Revision" | grep "37.*" | wc -l`
    if [ $result -eq 1 ];then
    PLATFORM=37
    DVFS_DIR=/sys/devices/system/cpu/cpu0/cpufreq/
    fi

    if [ $PLATFORM -ne 37 ]
    then
        tst_resm TFAIL "Only support imx37 platform! "
        RC=1
        return $RC
    fi

    return $RC
}

dvfs_test()
{
    RC=0

    # For imx37
    if [ $PLATFORM -eq 37 ];then
        for freq in 600000 532000 200000 600000 200000 532000
        do 
            cur_freq=`cat $DVFS_DIR/cpuinfo_cur_freq`
            echo $freq > $DVFS_DIR/scaling_setspeed
            res=`cat $DVFS_DIR/cpuinfo_cur_freq`
            if [ $res -eq $freq ];then
                tst_resm TPASS "cpu frequency changed from $cur_freq --> $res: ok"
            else
                tst_resm TFAIL "cpu frequency changed from $cur_freq --> $res: fail"
                RC=1
            fi
            sleep 3
        done 
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
dvfs_test || exit $RC

