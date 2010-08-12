#!/bin/sh
#Copyright (C) 2008-2010 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
##############################################################################
#
# Revision History:
#                          Modification     Tracking
# Author                       Date          Number    Description of Changes
#-----------------------   ------------    ----------  ---------------------
# Spring Zhang               10/06/2008       n/a        Initial ver. 
# Spring                     28/11/2008       n/a      Modify COPYRIGHT header
# Spring                     12/08/2010       n/a      Delete cleanup action
#############################################################################
# Portability:  ARM sh bash 
#
# File Name:    udev_sysfs_1.sh
# Total Tests:        1
# Test Goal: Check if sysfs and udev mechanism is working.
# Test Strategy: 
#           1. 
# 
# Input:	- $1 - none
#
# Return:       - 
#
# Use command "./udev_sysfs_1.sh" 
#               to test sysfs and udev mechanism


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
    export TCID="TGE_LV_FS_0561"       # Test case identifier
    export TST_COUNT=0   # Set up is initialized as test 0

    if [ -z $LTPTMP ]
    then
        LTPTMP=/tmp
    fi

    if [ $# -ne 0 ]
    then
        usage
        exit 1
    fi

    trap "cleanup" 0

    lsmod |grep scull
    if [ $? -eq 0 ]
    then
        rmmod scull
    fi

    insmod ${LTPROOT}/testcases/bin/scull.ko || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #1: module is wrong"
    fi

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
    return $RC
}

# Function:     sysfs_mech
#
# Description:  - sysfs mechanism
#
# Exit:         - zero on success
#               - non-zero on failure.
#
sysfs_mech()
{
    RC=0    # Return value from setup, and test functions.
    tst_resm TINFO "Test #1: detect /dev node"
    sleep 3
    
    ls -l /dev/scull1 || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #1: there is no /dev/scull* node"
        return $RC
    fi

    tst_resm TINFO "Test #1: test scull function"
    echo "abc" > /dev/scull1
    cat /dev/scull1 |grep "abc" || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #1: scull function is OK"
        return $RC
    fi

    tst_resm TINFO "Test #1: remove module"
    rmmod scull || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #1: remove module failed!"
        return $RC
    fi

    sleep 5

    tst_resm TINFO "Test #1: detect /dev node"
    if [ -e /dev/scull1 ]
    then
        tst_resm TFAIL "Test #1: There is still /dev/scull1 node"
        RC=2
        return $RC
    fi

    tst_resm TPASS "Test #1: test sysfs and udev success."

    return $RC
}

# Function:    udev_mech 
#
# Description:  - udev mechanism
#
# Exit:         - zero on success
#               - non-zero on failure.
#
udev_mech()
{
    # udev can't work, fix it!!!
   echo 'KERNEL=="scull",                NAME="scull/%k",SYMLINK+="scu0"' > /etc/udev/rules.d/99-user-scull.rules

}

# Function:     usage
#
# Description:  - display help info.
#
# Return        - none
usage()
{
    cat <<-EOF 

    Use this command to test sys &dev file system and udev mechanism.
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

sysfs_mech || exit $RC

