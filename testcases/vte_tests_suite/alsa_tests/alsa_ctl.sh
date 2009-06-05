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
# Spring Zhang          29/12/2008      ENGR99691     Initial ver. 
#############################################################################
# Portability:  ARM sh 
#
# File Name:    
# Total Tests:        1
# Test Strategy: play audio streams
# 
# Input:	- $1 - audio stream
#
# Return:       - 
#
# Use command "./alsa_ctl.sh" 

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
    export TCID="TGE_LV_ALSA_CTL"       # Test case identifier
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

    [ -e file ] && rm -f file
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

# Function:     alsa_ctl()
#
# Description:  Test ALSA control function
#
# Exit:         zero on success
#               non-zero on failure.
#
alsa_ctl()
{
    RC=0    
    TST_COUNT=1

    tst_resm TINFO "Test #1: save soundcard driver setting to a file."
    alsactl -f "file" store || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #1: save soundcard driver setting to a file."
        return $RC
    fi
    grep "DAC Playback Volume" file ||RC=$? 
    [ $RC -eq 0 ] || {
        tst_resm TFAIL "Test #1: alsa setting file error." 
        return $RC
    }

    amixer cset name='DAC Playback Volume' 200

    alsactl -f "file" restore ||RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #1: restore soundcard driver setting to a file."
        return $RC
    fi
    amixer cget name='DAC Playback Volume' |grep "values=200,200" ||RC=$?

    if [ $RC -eq 0 ] ;then
        tst_resm TPASS "Test #1: ALSA CTL test success."
    else
        tst_resm TFAIL "Test #1: ALSA CTL test fail"
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

    Use this command to test ALSA CONTROL functions.
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

alsa_ctl "$@" || exit $RC

