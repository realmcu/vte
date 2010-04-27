#Copyright (C) 2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
#!/bin/sh
##############################################################################
#
# Revision History:
#                      Modification     Tracking
# Author                   Date          Number    Description of Changes
#-------------------   ------------    ----------  ---------------------
# Spring Zhang          21/05/2009       n/a        Initial ver. 
#############################################################################
# Portability:  ARM sh 
#
# File Name:    
# Total Tests:        1
# Test Strategy: insert spdif module and find spdif device
# 
# Input:	- none
#
# Return:       - 
#
# Use command "./spdif_basic.sh" 

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
    export TCID="TGE_LV_SPDIF_PROBE"       # Test case identifier
    export TST_COUNT=0   # Set up is initialized as test 0
    BIN_DIR=`dirname $0`
    export PATH=$PATH:$BIN_DIR

    if [ -z $LTPTMP ]
    then
        LTPTMP=/tmp
    fi

    trap "cleanup" 0

    if [ ! -e /usr/bin/aplay ]
    then
        tst_resm TBROK "Test #1: ALSA utilities are not ready, \
        pls check..."
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
    modprobe snd_spdif -r
    return $RC
}

# Function:     spdif_probe()
#
# Description:  probe spdif module
#
# Exit:         zero on success
#               non-zero on failure.
#
spdif_probe()
{
    RC=0    # Return value from setup, and test functions.

    tst_resm TINFO "Test #1: Probe SPDIF module"
    modprobe snd_spdif || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #1: insert module error"
        return $RC
    fi

    tst_resm TINFO "Test #1: detect SPDIF device"
    aplay -l |grep -i spdif || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #1: can not detect SPDIF device"
        return $RC
    fi

    tst_resm TPASS "Test #1: probe SPDIF module success."

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

    Use this command to probe SPDIF module.
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

spdif_probe "$@" || exit $RC

