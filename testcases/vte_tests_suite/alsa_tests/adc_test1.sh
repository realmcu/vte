#!/bin/bash
################################################################################
#Copyright (C) 2008,2011-2012 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
################################################################################
#
# Revision History:
# Author                   Date        Description of Changes
#--------------------   ------------   ----------------------
# Spring Zhang           25/07/2008    Initial ver. 
# Spring                 21/10/2008    Add support for auto cases
# Spring                 28/11/2008    Modify COPYRIGHT header
# Spring                 13/04/2012    Abandon sound card detective
###############################################################################
# Test Purpose: Audio capture and playback check

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
    export TCID="TGE_LV_ADC"       # Test case identifier
    export TST_COUNT=0   # Set up is initialized as test 0
    BIN_DIR=`dirname $0`
    export PATH=$PATH:$BIN_DIR

    if [ -z $LTPTMP ]
    then
        LTPTMP=/tmp
    fi

    while getopts D:f:d:c:r:AM arg
    do 
        case $arg in
        D) DEVICE=$OPTARG;;
        f) SAM_FMT=$OPTARG;;
        d) DURATION=$OPTARG;;
        c) CHANNEL=$OPTARG;;
        r) SAM_FREQ=$OPTARG;;
        A) ;;
        M) MANUAL="true";;
        \?) usage ;;
        esac
    done

    if [ -z $DURATION ] || [ -z $SAM_FMT ] || [ -z $SAM_FREQ ] || [ -z $CHANNEL ]
    then
        usage
    fi

    trap "cleanup" 0

    if [ ! -e /usr/bin/arecord ]
    then
        echo TBROK "Test #1: ALSA utilities are not ready, pls check..."
        RC=65
        return $RC
    fi

    if [ -e audio.wav ]
    then
        rm -f audio.wav
    fi

    if [ -z "$DEVICE" ]; then
        dfl_alsa_dev=0
        plughw="-Dplughw:${dfl_alsa_dev}"
    else
        dfl_alsa_dev=$DEVICE
        plughw="-D${dfl_alsa_dev}"
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
    echo "clean up environment..."
    #rm -f audio.wav record.info
}

# Function:    adc_record 
#
# Description:  - Alsa ADC test, record a audio and play to check if it is right
#
# Exit:         - zero on success
#               - non-zero on failure.
#
adc_record()
{
    RC=0    # Return value from setup, and test functions.

    echo TINFO "Test #1: record audio stream with format $SAM_FMT, channel $CHANNEL, \
 sample rate $SAM_FREQ, $DURATION seconds, please speak to the microphone" 
    args=`echo $@|sed 's/-A//g'`

    arecord $plughw $args audio.wav 2> record.info || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_res TFAIL record.info "Test #1: record audio stream failed"
        return $RC
    fi

    # Check if the record file parameter are right
    grep $SAM_FREQ record.info >/dev/null || RC=$?
    if [ $RC -ne 0 ]
    then
        echo TFAIL "Test #1: record audio stream format is not right"
        return $RC
    fi

    if [ $CHANNEL = 1 ]
    then
        grep "Mono" record.info >/dev/null || RC=$?
    elif [ $CHANNEL = 2 ]
    then
        grep "Stereo" record.info >/dev/null || RC=$?
    fi
    if [ $RC -ne 0 ]
    then
        echo TFAIL "Test #1: record audio stream format is not right"
        return $RC
    fi

    if echo $SAM_FMT |grep "S" >/dev/null 
    then
        grep "Signed" record.info >/dev/null || RC=$?
    elif echo $SAM_FMT |grep "U" >/dev/null 
    then
        grep "Unsigned" record.info >/dev/null || RC=$?
    fi
    if [ $RC -ne 0 ]
    then
        echo TFAIL "Test #1: record audio stream format is not right"
        return $RC
    fi

    if echo $SAM_FMT |grep "LE" >/dev/null 
    then
        grep "Little Endian" record.info >/dev/null || RC=$?
    elif echo $SAM_FMT |grep "BE" >/dev/null 
    then
        grep "Big Endian" record.info >/dev/null || RC=$?
    fi
    if [ $RC -ne 0 ]
    then
        echo TFAIL "Test #1: record audio stream format is not right"
        return $RC
    fi

    if echo $SAM_FMT |grep "16" >/dev/null 
    then
        grep "16 bit" record.info >/dev/null || RC=$?
    elif echo $SAM_FMT |grep "24" >/dev/null 
    then
        grep "24 bit" record.info >/dev/null || RC=$?
    fi
    if [ $RC -ne 0 ]
    then
        echo TFAIL "Test #1: record audio stream format is not right"
        return $RC
    fi

    #play the recorded audio
    echo TINFO "Test #1: play the audio stream, please check the HEADPHONE,\
 hear if there is voice."
    aplay $plughw audio.wav 2> play.info|| RC=$?
    if [ $RC -ne 0 ]
    then
        tst_res TFAIL play.info "Test #1: play error, please check the stream file"
        return $RC
    fi
    # Check if the recorded file parameter are right when playing
    grep $SAM_FREQ play.info >/dev/null || RC=$?
    if [ $RC -ne 0 ]
    then
        echo TFAIL "Test #1: recorded audio stream sample rate is not same \
                with the given one"
        return $RC
    fi

    if [ $CHANNEL  = 1 ]
    then
        grep "Mono" play.info >/dev/null || RC=$?
    elif [ $CHANNEL = 2 ]
    then
        grep "Stereo" play.info >/dev/null || RC=$?
    fi
    if [ $RC -ne 0 ]
    then
        echo TFAIL "Test #1: recorded audio stream format is not right"
        return $RC
    fi

    if echo $SAM_FMT |grep "S" >/dev/null 
    then
        grep "Signed" play.info >/dev/null || RC=$?
    elif echo $SAM_FMT |grep "U" >/dev/null  
    then
        grep "Unsigned" play.info >/dev/null || RC=$?
    fi
    if [ $RC -ne 0 ]
    then
        echo TFAIL "Test #1: recorded audio stream format is not right"
        return $RC
    fi

    if echo $SAM_FMT |grep "LE" >/dev/null 
    then
        grep "Little Endian" play.info >/dev/null || RC=$?
    elif echo $SAM_FMT |grep "BE" >/dev/null 
    then
        grep "Big Endian" play.info >/dev/null || RC=$?
    fi
    if [ $RC -ne 0 ]
    then
        echo TFAIL "Test #1: recorded audio stream format is not right"
        return $RC
    fi

    if echo $SAM_FMT |grep "16" >/dev/null 
    then
        grep "16 bit" play.info >/dev/null || RC=$?
    elif echo $SAM_FMT |grep "24" >/dev/null 
    then
        grep "24 bit" play.info >/dev/null || RC=$?
    fi
    if [ $RC -ne 0 ]
    then
        echo TFAIL "Test #1: recorded audio stream format is not right"
        return $RC
    fi

    #if manual, ask the answer!
    if [ -n "$MANUAL" ]; then
        #check if tester hear the voice
        echo TINFO "Do you hear the music from the headphone?[y/n]"
        #TODO: There's an error here if manual is triggered.
        #read: read error: 0: Resource temporarily unavailable
        read answer
        if [ "$answer" != "y" ] ; then
            RC=67
        fi
    fi

    if [ $RC -eq 0 ]
    then
        echo TPASS "Test #1: ALSA ADC test success."
    else
        echo TFAIL "Test #1: ALSA ADC play audio fail"
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

    Use this command to test ALSA ADC record functions. -A is auto without confirm
    usage: ./${0##*/} -D [device name] -f [sample format] -d [interrupt \
 after #seconds] -c [channel number] -r [sample rate] -A
    e.g.: ./${0##*/} -D hw:0 -f S16_LE -d 5 -c 1 -r 8000

EOF
    exit 67
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

adc_record "$@" || exit $RC

