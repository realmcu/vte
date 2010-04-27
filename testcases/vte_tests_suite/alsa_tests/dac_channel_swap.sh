#!/bin/sh
##############################################################################
#
#  Copyright (C) 2004-2010 Freescale Semiconductor, Inc. All Rights Reserved.
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
# Spring Zhang          25/03/2009       n/a        Initial ver. 
# Spring Zhang          25/03/2010       n/a        Compatible with sh
#############################################################################
# Portability:  ARM sh 
#
# File Name:    
# Total Tests:        1
# Test Strategy: test channel swap
# 
# Input:	- $1 - audio stream
#
# Return:       - 
#
# Use command "./dac_channel_swap.sh [short audio stream] [long audio stream]" 

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
    export TCID="TGE_LV_DAC_CHNL_SWAP"       # Test case identifier
    export TST_COUNT=0   # Set up is initialized as test 0
    BIN_DIR=`dirname $0`
    export PATH=$PATH:$BIN_DIR

    if [ -z $LTPTMP ]
    then
        LTPTMP=/tmp
    fi

    s_stream=$1
    l_stream=$2

    trap "cleanup" 0

    if [ ! -e /usr/bin/aplay ]
    then
        tst_resm TBROK "Test #1: ALSA utilities are not ready, \
        pls check..."
        RC=65
        return $RC
    fi

    if [ -z "$s_stream" ] || [ -z "$l_stream" ]
    then
        usage
    fi

    if [ ! -e $s_stream ] || [ ! -e $l_stream ]
    then
        tst_resm TBROK "Test #1: audio stream is not ready, \
             pls check..."
        RC=66
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
    return $RC
}

# Function:     test_swap()
#
# Description:  play an long and short stream file to test channel swap
#
# Exit:         zero on success
#               non-zero on failure.
#
test_swap()
{
    RC=0    # Return value from setup, and test functions.

    tst_resm TINFO "Test #1: play the long audio stream, please check the \
    HEADPHONE, see if channel is swapped."
    aplay -N -M $l_stream || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #1: play error, please check the stream file"
        return $RC
    fi

    tst_resm TINFO "Do you hear the channel is swapped?[y/n]"
    read answer
    if [ "$answer" = "y" ]
    then
        tst_resm TFAIL "Test #1: ALSA DAC channel swap fail when long stream \
        plackback "
        RC=67
    fi

    i=1
    tst_resm TINFO "Test #1: play a short audio stream for 50 times, please \
    check the HEADPHONE, see if channel is swapped."
    while [ $i -le 50 ]; do
        aplay -N -M $s_stream
        i=$(expr $i + 1)
        echo $i
        sleep 5
    done

    tst_resm TINFO "Do you hear the channel is swapped?[y/n]"
    read answer
    if [ "$answer" = "y" ]
    then
        tst_resm TFAIL "Test #1: ALSA DAC channel swap fail"
        RC=67
    else
        tst_resm TPASS "Test #1: ALSA DAC channel swap test success."
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

    Use this command to test ALSA DAC channel swap.
    usage: ./${0##*/} [short audio stream] [long audio stream]

    e.g.: ./${0##*/} audio44k16S_onlyright_short.wav audio44k16S_onlyright_long.wav

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

test_swap "$@" || exit $RC

