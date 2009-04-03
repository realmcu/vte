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
# Spring Zhang          26/11/2008     ENGR100354     Initial ver. 
# Spring Zhang          27/11/2008     n/a          abandoned, use bt_smoke.sh
# Spring                28/11/2008     n/a          Modify COPYRIGHT header
#############################################################################
# Portability:  ARM sh 
#
# File Name:     bt_sanity.sh
# Total Tests:   1
# Test Strategy: Test basic BT functions 
# 
# Input:	    Test type
#
# Return:       0: PASS, non-0: FAIL
#
# Command:      "./bt_sanity.sh" 

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
    export TCID="TGE_LV_BT"       # Test case identifier
    export TST_COUNT=0   # Set up is initialized as test 0
    BIN_DIR=`dirname $0`
    export PATH=$PATH:$BIN_DIR

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

    [ ! -e /usr/local/bin/hci_spp_demo_app ] && { 
        tst_resm TBROK "BT application miss!!"
        RC=67
    }

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
    tst_resm TINFO "Clean BT module..."
    rmmod mxc_bt
    stty echo
    return $RC
}

# Function:     bt_sanity()   
#
# Description:  Test if BT module function is OK 
#
# Exit:         zero on success
#               non-zero on failure.
#
bt_sanity()
{
    RC=0    # Return value from setup, and test functions.

    tst_resm TINFO "Test #1: Probe BT module"

    modprobe mxc_bt || RC=$?
    [ $RC -ne 0 ] && {
    tst_resm TFAIL "Probe BT module fail" 
    return $RC 
    }

    sleep 3

    echo "0" > 0.cmd
    stty echo
    /usr/local/bin/hci_spp_demo_app -C /dev/ttymxc1 -B 921600 < 0.cmd > bt_search.log &
    bgpid=$!

    sleep 20
    tst_resm TINFO "Grep BT device in BT search log"
    kill $bgpid
    
    grep "^0:\ " bt_search.log ||RC=$?
    if [ $RC -eq 0 ]
    then
        tst_resm TPASS "Test #1: BT module sanity test success."
    else
        tst_resm TFAIL "Test #1: BT module sanity test fail."
        RC=67
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

    Use this command to test BT driver can search device.
    usage: ./${0##*/}
    e.g.: ./${0##*/}

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

bt_sanity "$@" || exit $RC


