#!/bin/sh
#Copyright (C) 2008,2010-2012 Freescale Semiconductor, Inc.
#All Rights Reserved.
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
#                      Modification
# Author                   Date       Description of Changes
#-------------------   ------------   ---------------------
# Spring Zhang          25/07/2008    Initial ver.
# Spring                24/10/2008    Add -A automation option
# Spring                28/11/2008    Modify COPYRIGHT header
# Spring                06/07/2010    Add -D hw option
# Spring                22/12/2010    Use temp direcotry
# Spring                17/03/2011    Match right ALSA device
# Spring                30/03/2012    Remove dependency of alsa dev detect
# Spring                06/04/2012    Use config file to get default device
#############################################################################

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
    export TCID="TGE_LV_DAC"       # Test case identifier
    export TST_COUNT=0   # Set up is initialized as test 0
    BIN_DIR=`dirname $0`
    export PATH=$PATH:$BIN_DIR

    if [ -z $LTPTMP ]
    then
        LTPTMP=/tmp
    fi

    while getopts f:c:d:s:aAMDNh arg
    do
        case $arg in
        f) FILE=$OPTARG;;
        c) CFG_FILE=$OPTARG;;
        d) CARD=$OPTARG;;
        s) SEARCH_STR=$OPTARG;;
        a) RUN_ALL="true";;
        # A is abandoned, used for auto, now it's default behavior
        A) ;;
        M) MANUAL="true";;
        D) HW="true";;
        N) ;;
        \?|h) usage;;
        esac
    done

    trap "cleanup" 0

    if [ ! -e /usr/bin/aplay ]; then
        tst_resm TBROK "ALSA utilities are not ready, pls check..."
        RC=65
        return $RC
    fi

    if [ ! -e $FILE ]; then
        tst_resm TBROK "audio stream is not ready, pls check..."
        RC=66
        return $RC
    fi

    if [ -n "$CFG_FILE" ]; then
        if [ -e "$CFG_FILE" ]; then
            tst_resm TBROK "Specified config file can't find, pls check..."
            RC=67
            return $RC
        fi
    fi

    if [ -z "$CARD" ]
        if [ -z "$SEARCH_STR" ]; then
            platfm=`platfm.sh`
            eval def_cfg_name="audio_dac_${platfm}.cfg"
            if [ -e /etc/asound/$def_cfg_name ]; then
                CFG_FILE=/etc/asound/$def_cfg_name
            elif [ -e $LTPROOT/testcases/bin/$def_cfg_name ]; then
                CFG_FILE=$LTPROOT/testcases/bin/$def_cfg_name
            else
                tst_resm TBROK "Default config file can't find, pls check..."
                RC=67
                return $RC
            fi
        fi
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

    needpause=$(cat /proc/cpuinfo | grep 378 | wc -l)
    if [ ! -z $needpause ]; then
      sleep 3
    fi

    if [ -e $tmpdir ]; then
        rm -rf $tmpdir
    fi

    return $RC
}

# Function:     dac_play()
#
# Description:  play an WAV stream file
#
# Exit:         zero on success
#               non-zero on failure.
#
dac_play()
{
    RC=0

    tst_resm TINFO "Test #1: play the audio stream, please check the HEADPHONE,\
 hear if there is voice."
    basefn=$(basename $FILE)
    tmpdir=`mktemp -d -p $LTPTMP`
    if [ -e $tmpdir ]; then
        cp -f $FILE $tmpdir || RC=$?
        if [ $RC -ne 0 ]; then
            tst_resm TFAIL "Test #1: copy from NFS to tmp error, no space left in $LTPTMP"
            return $RC
        fi
    else
        RC=2
        return $RC
    fi

    if [ -z "$CARD" ]; then
        if [ -z "$SEARCH_STR" ]; then
            # get keyword from config file
            HW_keyword=`head -n 1 $CFG_FILE`
            # consider it as card index
            if [ `echo $HW_keyword| wc -c` -eq 1 ]; then
                HW_keyword="card $HW_keyword"
            fi
        fi
    fi

    if_card_specified=""
    #if card is specified
    if [ -n "$CARD" ]; then
        HW_keyword="card $CARD"
        if_card_specified="card"
    elif [ -n "$SEARCH_STR" ]; then
        #if search string is specified
        HW_keyword=$SEARCH_STR
        if_card_specified="pattern"
    fi

    if [ -z "$RUN_ALL" ]; then
        alsa_dev=`aplay -l |grep card| grep -i "$HW_keyword" |awk '{ print $2 }'|sed 's/://'`
        if [ -z "$alsa_dev" ]; then
            RC=69
            tst_resm TFAIL "Specified card $HW_keyword not found, please check."
            return $RC
        fi
        dev_num=0
        for dev in $alsa_dev; do
            dev_num=`expr $dev_num + 1`
        done
        if [ $dev_num -gt 1 ]; then
            RC=70
            tst_resm TFAIL "Specified card $HW_keyword match num is not single, please provide more detail name"
            return $RC
        fi
        #in case there are more than one card match the search pattern
        total_card_num=1
        if [ -z "$HW" ]; then
            plugin="plug"
        fi
        #parameter of HW playback and plughw playback
        play_iface="-D${plugin}hw:${alsa_dev}"
        card_name=`aplay -l | grep -i "$HW_keyword" | awk '{ print $3 }'`
        echo
        echo "TINFO play on: $card_name"
        aplay ${play_iface} $tmpdir/$basefn || RC=$?
        if [ $RC -eq 0 ]; then
            echo
            echo "TPASS play on: $card_name"
        else
            echo
            echo "TFAIL play on: $card_name"
        fi

        #if manual is set, ask the answer!
        if [ -n "$MANUAL" ]; then
            manual_check || RC=$?
        fi

        return $RC
    fi

    # Now start to play on all devices
    total_card_num=`aplay -l| grep card |wc -l`

    # if card is not specified, try loop on all cards
    i=0
    while [ $i -lt $total_card_num ]; do
        if [ -z "$alsa_dev" ]; then
            HW_keyword="card $i"
            alsa_dev=`aplay -l |grep card| grep -i "$HW_keyword" |awk '{ print $2 }'|sed 's/://'`
        fi

        #in case 'continue' in the middle
        i=`expr $i + 1`

        if [ -z "$alsa_dev" ]; then
            tst_resm TFAIL "Specified card can not find, please check"
            RC=68
            continue
        fi

        if [ -z "$HW" ]; then
            plugin="plug"
        fi
        #parameter of HW playback and plughw playback
        play_iface="-D${plugin}hw:${alsa_dev}"
        card_name=`aplay -l | grep -i "$HW_keyword" | awk '{ print $3 }'`
        test_cards="$test_cards $card_name"
        #empty it to loop to next card
        alsa_dev=""
        echo
        echo "TINFO play on: $card_name"
        aplay ${play_iface} $tmpdir/$basefn ||RC=$?
        if [ $RC -eq 0 ]; then
            test_results="$test_results PASS"
            echo
            echo "TPASS play on: $card_name"
        else
            test_results="$test_results FAIL"
            echo
            echo "TFAIL play error on: $card_name, please check"
            continue
        fi

        #if manual is set, ask the answer!
        if [ -n "$MANUAL" ]; then
            manual_check || RC=$?
        fi

    done

    echo "====================="
    echo "=====Test Results===="
    for name in $test_cards; do
        echo -n "$name "
    done
    echo
    for result in $test_results; do
        echo -n "$result \t"
    done
    echo
    echo "====================="

    if [ $RC -eq 0 ]; then
        tst_resm TPASS "Pass playback on all available or specified ALSA device"
    fi

    return $RC
}

manual_check()
{
    tst_resm TINFO "Do you hear the music from the headphone?[y/n]"
    read answer
    if [ "$answer" = "y" ]; then
        tst_resm TPASS "Test #1: ALSA DAC test success."
        return 0
    else
        tst_resm TFAIL "Test #1: ALSA DAC play audio fail"
        RC=67
    fi
}

# Function:     usage
#
# Description:  - display help info.
#
# Return        - none
usage()
{
    cat <<-EOF

    Use this command to test ALSA DAC play functions.
    usage: ./${0##*/} -f [audio stream] -D -M -d [card num] -s [card string]
            -D: if using hw to playback
            -a: Test on all devices, but it will report failure if either one fails
            -c: specify configuration file, or it will load from default place
            -M: manual mode check
            -d: specify card num, e.g. 0, 1, 2
            -s: specify card search string, e.g. wm8962, sgtl5000, hdmi
    Play on card 0 with manual check, only use hardware supported stream
    e.g.: ./${0##*/} -f audio44k16M.wav -M -D -d 0
    Play on "hdmi" card:
    e.g.: ./${0##*/} -f audio44k16M.wav -s hdmi
    Play on all available sound cards:
    e.g.: ./${0##*/} -f audio44k16M.wav -a

    Default config file store place: /etc/asound, $LTPROOT/testcases/bin
    The file name should be like audio_dac_${platfm}.cfg
    The file only contain one line: the card keyword, e.g. wm8962, sgtl5000

EOF
    exit 1
}


# Function:     main
#
# Description:  - Execute all tests, exit with test status.
#
# Exit:         - zero on success
#               - non-zero on failure.
#
RC=0    # Return value from setup, and test functions.

if [ $# -le 1 ]; then
    usage
fi

#"" will pass the whole args to function setup()
setup "$@" || exit $RC

dac_play "$@" || exit $RC

