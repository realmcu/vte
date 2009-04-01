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
# Spring Zhang          07/08/2008       n/a        Initial ver. 
# Spring                28/11/2008       n/a        Modify COPYRIGHT header
#############################################################################
# Portability:  ARM sh bash 
#
# File Name:    
# Total Tests:        1
# Test Strategy: switch from speaker to headphone
# 
# Input:	- $1 - audio stream
#
# Return:       - 
#
# Use command "./speaker_switch_headset.sh [audio stream]" 

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
    export TCID="TGE_LV_SPEAKER_0002"       # Test case identifier
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

# Function:     speaker_switch_test
#
# Description:  - speaker test, adjust voice volumn and play a audio file
#
# Exit:         - zero on success
#               - non-zero on failure.
#
speaker_switch_test()
{
    RC=0    # Return value from setup, and test functions.

    find=0
    
    # Determine the platform
    find=`cat /proc/cpuinfo | grep "Revision" | grep "31.*" | wc -l`;
    if [ $find -eq 1 ]
    then
        platform=mx31
    fi

    find=`cat /proc/cpuinfo | grep "Revision" | grep "51.*" | wc -l`;
    if [ $find -eq 1 ]
    then
        platform=mx51
    fi

    # enable speaker
    if [ $platform = "mx31" ]
    then
        amixer -c 0 cset numid=1,iface=MIXER,name='Master Output Playback Volume' 2 ||RC=$?
        if [ $RC -ne 0 ]
        then
            tst_resm TFAIL "Test #1:adjust mixer volume failed..."
            return $RC
        fi
    elif [ $platform = "mx51" ]
    then
        amixer cset numid=42,iface=MIXER,name='Speaker Switch' 2 ||RC=$?
        if [ $RC -ne 0 ]
        then
            tst_resm TFAIL "Test #1: switch speaker failed..."
            return $RC
        fi

        amixer cset numid=46,iface=MIXER,name='Right Speaker Mixer DACR Switch' 1 ||RC=$?
        if [ $RC -ne 0 ]
        then
            tst_resm TFAIL "Test #1: switch right speaker failed..."
            return $RC
        fi

        amixer cset numid=49,iface=MIXER,name='Left Speaker Mixer DACL Switch' 1 ||RC=$?
        if [ $RC -ne 0 ]
        then
            tst_resm TFAIL "Test #1: switch left speaker failed..."
            return $RC
        fi
        amixer cset numid=68,iface=MIXER,name='Speaker Function' 1
    else
        # don't support on other platform
        RC=67
        return $RC
    fi
    sleep 2

    tst_resm TINFO "Test #1: adjust mixer volumn..."

    if [ $platform = "mx31" ]
    then
        amixer -c 0 cset numid=2,iface=MIXER,name='Master Playback Volume' 80||RC=$?
        if [ $RC -ne 0 ]
        then
            tst_resm TFAIL "Test #1:adjust mixer volume failed..."
            return $RC
        fi
    elif [ $platform = "mx51" ] 
    then
        amixer -c 0 cset numid=44,iface=MIXER,name='Speaker Volume' 40 ||RC=$?
        if [ $RC -ne 0 ]
        then
            tst_resm TFAIL "Test #1:adjust mixer volume failed..."
            return $RC
        fi
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
        echo
    else
        tst_resm TFAIL "Test #1: speaker error." 
        RC=67
        return $RC
    fi

    tst_resm TINFO "Test #1: now the voice will switch from speaker to headphone."
    if [ $platform = "mx31" ]
    then
    # before mx31 2.3.0 release, there is a bug, should use the following meanwhile
    #aplay a.wav&
    #amixer -c 0 cset numid=1,iface=MIXER,name='Master Output Playback Volume' 4
        amixer -c 0 cset numid=1,iface=MIXER,name='Master Output Playback Volume' 4 ||RC=$?
        if [ $RC -ne 0 ]
        then
            tst_resm TFAIL "Test #1:switch voice failed..."
            return $RC
        fi
        amixer -c 0 cset numid=2,iface=MIXER,name='Master Playback Volume' 40
    elif [ $platform = "mx51" ]
    then
        amixer cset numid=42,iface=MIXER,name='Speaker Switch' 0 ||RC=$?
        if [ $RC -ne 0 ]
        then
            tst_resm TFAIL "Test #1: switch speaker failed..."
            return $RC
        fi
        amixer cset numid=46,iface=MIXER,name='Right Speaker Mixer DACR Switch' 0
        amixer cset numid=49,iface=MIXER,name='Left Speaker Mixer DACL Switch' 0
        amixer cset numid=68,iface=MIXER,name='Speaker Function' 0
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

    tst_resm TINFO "Do you hear the speaker is mute and the headphone output the voice?[y/n]"
    read answer
    if [ $answer = "y" ]
    then
        tst_resm TPASS "Test #1: speaker switch to headphone success." 
    else
        tst_resm TFAIL "Test #1: speaker error." 
        RC=67
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

    Use this command to test speaker switch to headphone functions.
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

speaker_switch_test "$@" || exit $RC

