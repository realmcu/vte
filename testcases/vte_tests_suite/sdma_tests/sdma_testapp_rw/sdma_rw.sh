#!/bin/sh
##############################################################################
#
#  Copyright 2004-2008 Freescale Semiconductor, Inc. All Rights Reserved.
#
##############################################################################
#
#  The code contained herein is licensed under the GNU Lesser General Public
#  License.  You may obtain a copy of the GNU Lesser General Public License
#  Version 2.1 or later at the following locations:
#
#  http://www.opensource.org/licenses/lgpl-license.html
#  http://www.gnu.org/copyleft/lgpl.html
#
##############################################################################
#
# Revision History:
#                      Modification     Tracking
# Author                   Date          Number    Description of Changes
#-------------------   ------------    ----------  ---------------------
# Spring Zhang          19/11/2008       n/a        Initial ver. 
# Spring                28/11/2008       n/a      Modify COPYRIGHT header
#############################################################################
# Portability:  ARM sh 
#
# File Name:   sdma_rw.sh 
# Total Tests:        1
# Test Strategy: 
# 
# Input:	
#
# Return:  
#
# Use command "./sdma_rw.sh"

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
    # Initialize return code to zero.
    RC=0                 # Exit values of system commands used

    export TST_TOTAL=1   # Total number of test cases in this file.
    LTPTMP=${TMP}        # Temporary directory to create files, etc.
    export TCID="TGE_LV_SDMA_RW"       # Test case identifier
    export TST_COUNT=0   # Set up is initialized as test 0
    BIN_DIR=`dirname $0`
    export PATH=$PATH:$BIN_DIR

    if [ -z $LTPTMP ]
    then
        LTPTMP=/tmp
    fi

    trap "cleanup" 0

    if [ ! -e $LTPROOT/testcases/bin/sdma_rw ]
    then
        tst_resm TBROK "Test #1: no sdma_rw file, pls check..."
        RC=65
        return $RC
    fi

    if [ ! -e $LTPROOT/testcases/bin/mxc_sdma_mem_test.ko ]
    then
        tst_resm TBROK "Test #1: no sdma driver file, pls check..."
        RC=65
        return $RC
    fi

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
    rmmod mxc_sdma_mem_test
    return $RC
}

# Function:     
#
# Description: 
#
# Exit:         - zero on success
#               - non-zero on failure.
#
sdma_rw_test()
{
    RC=0    # Return value from setup, and test functions.

    tst_resm TINFO "Test #1: Insert SDMA driver."
    insmod $LTPROOT/testcases/bin/mxc_sdma_mem_test.ko || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #1: Insert SDMA driver fail."
        return $RC
    fi

    sleep 5

    sdma_rw || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #1: SDMA WRITE/READ fail."
        return $RC
    fi

    return $RC
}

# Function:     usage
#
# Description:  - display help info.
#
# Return        - none
usage()
{
    cat <<-EOF 

    Use this command to test SDMA RW
    usage: ./${0##*/}

EOF
}


# Function:     main
#
# Description:  - Execute all tests, exit with test status.
#
# Exit:         - zero on success
#               - non-zero on failure.
#
RC=0    # Return value from setup, and test functions.

#"" will pass the whole args to function setup()
setup "$@" || exit $RC

sdma_rw_test "$@" || exit $RC

