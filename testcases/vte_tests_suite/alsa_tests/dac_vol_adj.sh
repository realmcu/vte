#!/bin/sh
#Copyright (C) 2005-2010 Freescale Semiconductor, Inc. All Rights Reserved.
#
# The code contained herein is licensed under the GNU General Public
# License. You may obtain a copy of the GNU General Public License
# Version 2 or later at the following locations:
#
# http://www.opensource.org/licenses/gpl-license.html
# http://www.gnu.org/copyleft/gpl.html
##############################################################################
#
# Revision History:
#                       Modification     Tracking
# Author                    Date          Number    Description of Changes
#--------------------   ------------    ----------  ---------------------
# Spring Zhang           06/08/2008       n/a      Initial ver. 
# Spring                 05/09/2008       n/a      Add mx51 support
# Spring                 26/09/2008       n/a      Fix some volume 0 bug
# Spring                 27/10/2008       n/a      Add mx35&mx37 support  
# Spring                 28/11/2008       n/a      Modify COPYRIGHT header
# Spring                 22/09/2009       n/a      Optimize code
# Spring                 11/12/2009       n/a      Add support to other case
# Spring                 24/03/2010       n/a      Add support to mx53
#############################################################################
# Portability:   ARM sh 
# File Name:     dac_vol_adj.sh   
# Total Tests:   1
# Test Strategy: play audio streams with volume up and down
# 
# Tested on : i.MX51&35&37&53&50
# Support: i.MX31&MX51&MX35&MX37&MX53&50

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
    export TCID="TGE_LV_DAC_VOL_ADJ"       # Test case identifier
    export TST_COUNT=0   # Set up is initialized as test 0
    BIN_DIR=`dirname $0`
    export PATH=$PATH:$BIN_DIR

    if [ -z $LTPTMP ]
    then
        LTPTMP=/tmp
    fi

    if [ $# -lt 2 ]
    then
        usage
        exit 1
    fi

    trap "cleanup" 0

    if [ ! -e /usr/bin/aplay ]
    then
        tst_resm TBROK "Test #1: ALSA utilities are not ready, \
        pls check..."
        RC=65
        return $RC
    fi

    audio_file=$2

    if [ ! -e $2 ]
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
    echo "clean up environment..."

    ctl_id=`amixer_ctl_id`
    max_vol || MAX=$?
    MAX=`expr $MAX / 3 \* 2`
    amixer -c 0 cset "$ctl_id" $MAX 

    echo "clean up environment end"
    return $RC
}

# Function:     amixer_ctl_id
#
# Description:  - return amixer control id
#
# Exit:         - success: id
#               - "" on failure.
#
amixer_ctl_id()
{
    local ctl_id=""

    if [ $platfm -eq 31 ]
    then
        ctl_id=name='Master Playback Volume'
    elif [ $platfm -eq 35 ] || [ $platfm -eq 51 ] \
        || [ $platfm -eq 41 ] || [ $platfm -eq 53 ] #sgtl5k
    then
        ctl_id=name='Headphone Volume'
    elif [ $platfm -eq 37 ]
    then
        ctl_id=name='Playback PCM Volume'
    elif [ $platfm -eq 50 ]
		then
				ctl_id=name='Headphone Volume'
		fi
 
    echo $ctl_id
}

# Function:     max_vol
#
# Description:  - return max volume based on platform
#
# Exit:         - max volume on success
#               - 0 on failure.
#
max_vol()
{
    MAX_MX31=99
    MAX_SGTL5K=127
    MAX_MX37=255
		MAX_MX50=190

    if [ $platfm -eq 31 ]
    then
        return $MAX_MX31
    elif [ $platfm -eq 35 ] || [ $platfm -eq 51 ] \
        || [ $platfm -eq 41 ] || [ $platfm -eq 53 ]  #sgtl5k
    then
        return $MAX_SGTL5K
    elif [ $platfm -eq 37 ]
    then
        return $MAX_MX37
		elif [ $platfm -eq 50 ]
		then
			  return $MAX_MX50
    fi

    return 0
}


# Function:     adjust volume
#
# Description:  - adjust voice volume and play a audio file
#
# Exit:         - zero on success
#               - non-zero on failure.
#
adj_vol()
{
    RC=0    # Return value from setup, and test functions.

    platfm.sh || platfm=$?
    if [ $platfm -eq 67 ]
    then
        RC=$platfm
        return $RC
    fi
    platform="mx$platfm"

    tst_resm TINFO "Test #1: play the audio stream, please check the HEADPHONE,\
 hear if the voice is from MIN(0) to MAX(MX31:100, MX51:128, MX37:256)."

    STEP=5
    #if DECREASE=0, vol: MIN->MAX; DECREASE=1, vol: MAX->MIN
    for DECREASE in 0 1
    do
        i=0
        while [ $i -le $STEP ]
        do
            max_vol || MAX=$?
            vol=`expr $DECREASE % 2 \* -2 + 1` 
            vol=`expr $vol \* $MAX \* $i / $STEP + $DECREASE % 2 \* $MAX`
            ctl_id=`amixer_ctl_id`
            amixer -c 0 cset "$ctl_id" $vol

            aplay -M -N -D hw:0,0 $1 2>/dev/null || RC=$?
            if [ $RC -ne 0 ]
            then
                tst_resm TFAIL "Test #1: play error, please check the stream file"
                return $RC
            fi

            i=`expr $i + 1`
        done

        tst_resm TINFO "Do you hear the voice volume from MIN to MAX or MAX to MIN?[y/n]"
        read answer
        if [ "x$answer" = "xy" ]
        then
            tst_resm TPASS "Test #1: ALSA DAC volume adjust test from MIN to MAX or MAX to MIN success."
        else
            tst_resm TFAIL "Test #1: ALSA DAC volume adjust test fail"
            RC=67
            return $RC
        fi

    done

    return $RC
}

# Function:     left_right_channel
#
# Description:  - Test left channel then test right channel
#
# Exit:         - zero on success
#               - non-zero on failure.
#
left_right_channel()
{
    RC=0    # Return value from setup, and test functions.

    platfm.sh || platfm=$?
    if [ $platfm -eq 67 ]
    then
        RC=$platfm
        return $RC
    fi
    platform="mx$platfm"

    max_vol || MAX=$?
    if [ $MAX -eq 0 ]; then
        tst_resm TFAIL "Test #2: Platform not supported, please check."
        return 67
    fi
    vol=`expr $MAX / 3 \* 2`
    ctl_id=`amixer_ctl_id`
    amixer -c 0 cset "$ctl_id" $vol,0
    aplay -M -N -D hw:0,0 $1 2>/dev/null || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #2: play error, please check the stream file"
        return $RC
    fi
    amixer -c 0 cset "$ctl_id" 0,$vol
    aplay -M -N -D hw:0,0 $1 2>/dev/null || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #2: play error, please check the stream file"
        return $RC
    fi
    tst_resm TINFO "Do you hear the voice from left channel then from right channel[y/n]"
    read answer
    if [ "x$answer" = "xy" ]
    then
        tst_resm TPASS "Test #2: left and right channel test."
    else
        tst_resm TFAIL "Test #2: left and right channel test fail."
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

    Use this command to test ALSA DAC volume adjust functions.
    TCID 1: The volume will turn from MIN(0) to MAX(100) and then from MAX to MIN.
    TCID 2: Test mute left and then right channel.
    usage: ./${0##*/} [TC Id] [audio stream]
    e.g.: ./${0##*/} 1 audio48k16S.wav
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

case "$1" in
    1)
    adj_vol "$audio_file" || exit $RC
    ;;
    2)
    left_right_channel "$audio_file" || exit $RC
    ;;
    *)
    usage
    exit 1
    ;;
esac
