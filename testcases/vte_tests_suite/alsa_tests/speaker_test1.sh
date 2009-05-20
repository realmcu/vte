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
# Spring Zhang               24/07/2008       n/a        Initial ver.
# Spring                     02/09/2008       n/a        Add MX51 support
# Spring                     22/10/2008       n/a        Modify MX51 enable
# Spring                     28/11/2008       n/a        Modify COPYRIGHT header
# Spring                     18/05/2009       n/a        Add BBG2 support
#############################################################################
# Portability:  ARM sh bash
#
# File Name:
# Total Tests:        1
# Test Strategy: basic speaker function test
#
# Input: - $1 - audio stream
#
# Return:       -
#
# Use command "./speaker_test1.sh [audio stream]"

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
    export TCID="TGE_LV_SPEAKER_0001"       # Test case identifier
    export TST_COUNT=0   # Set up is initialized as test 0
    BIN_DIR=`dirname $0`
    export PATH=$PATH:$BIN_DIR

    if [ -z $LTPTMP ]
    then
        LTPTMP=/tmp
    fi

    if [ $# -lt 1 ]
    then
        usage
        exit 1
    fi

    trap "cleanup" 0

    if [ ! -e /usr/bin/aplay ] || [ ! -e /usr/bin/amixer ]
    then
        tst_brkm TBROK cleanup "Test #1: ALSA utilities are not ready, \
        pls check..."
        RC=65
        return $RC
    fi

    if [ ! -e $1 ]
    then
        tst_brkm TBROK cleanup "Test #1: audio stream is not ready, \
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
    echo "clean up environment..."
    echo "clean up environment end"
    return $RC
}

# Function:     speaker_basic_test
#
# Description:  - speaker test, adjust voice volumn and play a audio file
#
# Exit:         - zero on success
#               - non-zero on failure.
#
speaker_basic_test()
{
    RC=0    # Return value from setup, and test functions.

    # Determine the platform
    platfm.sh || platform=$?
    
    # enable speaker
    if [ $platform -eq 31 ]
    then
        amixer -c 0 cset name='Master Output Playback Volume' 2 ||RC=$?
        if [ $RC -ne 0 ]
        then
            tst_resm TFAIL "Test #1:adjust mixer volume failed..."
            return $RC
        fi
    fi

    if [ $platform -eq 51 ] || [ $platform -eq 41 ]
    then
        amixer cset name='Speaker Function' 1 ||RC=$?
        if [ $RC -ne 0 ]
        then
            tst_resm TFAIL "Test #1: enable speaker failed..."
            return $RC
        fi
    fi
    sleep 2

    tst_resm TINFO "Test #1: adjust mixer volumn..."

    if [ $platform -eq 31 ]
    then
        amixer -c 0 cset name='Master Playback Volume' 80||RC=$?
        if [ $RC -ne 0 ]
        then
            tst_resm TFAIL "Test #1:adjust mixer volume failed..."
            return $RC
        fi
    elif [ $platform -eq 51 ] || [ $platform -eq 41 ]
    then
        :
    fi
    sleep 2

    tst_resm TINFO "Test #1: play the audio stream, please check the SPEAKER, \
 hear if there is voice."
    aplay -D hw:0,0 $1 || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #1: play error, please check the stream file"
        return $RC
    fi

    tst_resm TINFO "Do you hear the speaker?[y/n]"
    read answer
    if [ $answer = "y" ]
    then
        tst_resm TPASS "Test #1: speaker test success."
    else
        tst_resm TFAIL "Test #1: speaker test fail!"
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

    Use this command to test speaker functions.
    usage: ./${0##*/} [audio stream]
    e.g.: ./${0##*/} audio44k16M.wav

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

speaker_basic_test "$@" || exit $RC

