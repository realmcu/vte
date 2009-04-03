#!/bin/sh
#
#
# File :        scc_test.sh
#
# Description:  security test 
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
    export TCID="scc_test.sh"
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
    local RC=0

    tmp=`find $LTPROOT/testcases/bin -name "scc_test_module.ko" | wc -l`
    if [ $tmp -gt 0 ]
    then
        tst_resm TINFO "success to search scc_test_module.ko"
    else
        tst_resm TFAIL "fail to search scc_test_module.ko"
        RC=1
        return $RC
    fi

    insmod $LTPROOT/testcases/bin/scc_test_module.ko
    sleep 3
    
    tmp=`lsmod | grep "scc_test_module" | wc -l`
    if [ $tmp -gt 0 ]
    then
        tst_resm TINFO "success to insmod scc_test_module.ko"
    else
        tst_resm TFAIL "fail to insmod scc_test_module.ko"
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
