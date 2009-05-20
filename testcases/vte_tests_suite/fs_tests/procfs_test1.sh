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
#                          Modification     Tracking
# Author                       Date          Number    Description of Changes
#-----------------------   ------------    ----------  ---------------------
# Spring Zhang               23/05/2008       n/a        Initial ver.
# Spring                     28/11/2008       n/a      Modify COPYRIGHT header
#############################################################################
# Portability:  ARM sh bash
#
# File Name:    procfs_1.sh
# Total Tests:           1
# Test Strategy:
#
# Input: no - $1 -
#      - $2 -
#
# Return:       -
#
# Use command "./procfs_test1.sh "to test proc file system


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
    RC=0                # Exit values of system commands used

    export TST_TOTAL=1   # Total number of test cases in this file.
    LTPTMP=${TMP}        # Temporary directory to create files, etc.
    export TCID="TGE_LV_FS_0541"       # Test case identifier
    export TST_COUNT=0   # Set up is initialized as test 0

    trap "cleanup" 0

#    if [ -z $LTPTMP ] && [ -z $TMPBASE ]
#    then
#        LTPTMP=/tmp
#    else
#        LTPTMP=$TMPBASE
#    fi
#    if [ -z $LTPBIN ] && [ -z $LTPROOT ]
#    then
#        LTPBIN=./
#    else
#        LTPBIN=$LTPROOT/testcases/bin
#    fi

}



# Function:     cleanup
#
# Description   - remove temporary files and directories.
#
# Return        - zero on success
#               - non zero on failure. return value from commands ($RC)
cleanup()
{
    echo
}

# Function:     procinfo
#
# Description:  - cat /proc files info
#
# Exit:         - zero on success
#               - non-zero on failure.
#
procinfo()
{
    RC=0    # Return value from setup, and test functions.
    tst_resm TINFO "Test #1: display /proc/cpuinfo."

    [ -e /proc/cpuinfo ] && [ -e /proc/mtd ] || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #1: No proc devices(e.g. cpuinfo, mtd)"
        return $RC
    fi

    cat /proc/cpuinfo |grep -i Freescale || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #1: cpuinfo error"
        return $RC
    fi

    tst_resm TINFO "Test #1: display /proc/mtd."
    cat /proc/mtd |grep -i kernel || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #1: no mtd proc file system."
        return $RC
    fi

    tst_resm TPASS "Test #1: support proc file system."

    return $RC
}

# Function:     main
#
# Description:  - Execute all tests, exit with test status.
#
# Exit:         - zero on success
#               - non-zero on failure.
#
RC=0    # Return value from setup, and test functions.

setup  || exit $RC

procinfo || exit $RC





