#!/bin/sh
#
#
# File :        dptc_test.sh
#
# Description:  This is a test to set/get backlight functions.
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
    export TCID="dptc_test"
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
    result=`cat /proc/cpuinfo | grep "Revision" | grep "31.*" | wc -l`
    if [ $result -eq 1 ];then
    PLATFORM=31
    DPTC_DIR=/sys/devices/platform/mxc_dptc.0
    fi

    result=`cat /proc/cpuinfo | grep "Revision" | grep "37.*" | wc -l`
    if [ $result -eq 1 ];then
    PLATFORM=37
    DPTC_GP_DIR=/sys/devices/platform/mxc_dptc.0
    DPTC_LP_DIR=/sys/devices/platform/mxc_dptc.1
    fi

    if [ $PLATFORM -eq 0 ]
    then
        tst_resm TFAIL "Only support imx31 and imx37 platform! "
        RC=1
        return $RC
    fi

    return $RC
}

dptc_test()
{
    RC=0

    # For imx31
    if [ $PLATFORM -eq 31 ];then
        echo 1 > $DPTC_DIR/enable
        res=`cat $DPTC_DIR/enable | grep "enabled" | wc -l`
        if [ $res -eq 1 ];then
            tst_resm TPASS "dptc is enabled"
        else
            tst_resm TFAIL "fail to enable dptc"
            RC=1
        fi

        echo 0 > $DPTC_DIR/enable
        res=`cat $DPTC_DIR/enable | grep "disabled" | wc -l`
        if [ $res -eq 1 ];then
            tst_resm TPASS "dptc is disabled"
        else
            tst_resm TFAIL "fail to disable dptc"
            RC=1
        fi
    fi

    # For imx37
    if [ $PLATFORM -eq 37 ];then
        echo 1 > $DPTC_GP_DIR/enable
        res=`cat $DPTC_GP_DIR/enable | grep "enabled" | wc -l`
        if [ $res -eq 1 ];then
            tst_resm TPASS "dptc_gp is enabled"
        else
            tst_resm TFAIL "fail to enable dptc_gp"
            RC=1
        fi

        echo 0 > $DPTC_GP_DIR/enable
        res=`cat $DPTC_GP_DIR/enable | grep "disabled" | wc -l`
        if [ $res -eq 1 ];then
            tst_resm TPASS "dptc_gp is disabled"
        else
            tst_resm TFAIL "fail to disable dptc_gp"
            RC=1
        fi
        
        echo 1 > $DPTC_LP_DIR/enable
        res=`cat $DPTC_LP_DIR/enable | grep "enabled" | wc -l`
        if [ $res -eq 1 ];then
            tst_resm TPASS "dptc_lp is enabled"
        else
            tst_resm TFAIL "fail to enable dptc_lp"
            RC=1
        fi

        echo 0 > $DPTC_LP_DIR/enable
        res=`cat $DPTC_LP_DIR/enable | grep "disabled" | wc -l`
        if [ $res -eq 1 ];then
            tst_resm TPASS "dptc_lp is disabled"
        else
            tst_resm TFAIL "fail to disable dptc_lp"
            RC=1
        fi
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
dptc_test || exit $RC

