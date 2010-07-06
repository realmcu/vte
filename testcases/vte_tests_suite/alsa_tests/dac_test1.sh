#!/bin/sh
#Copyright (C) 2008,2010 Freescale Semiconductor, Inc. All Rights Reserved.
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
#                      Modification     Tracking
# Author                   Date          Number    Description of Changes
#-------------------   ------------    ----------  ---------------------
# Spring Zhang          25/07/2008       n/a        Initial ver. 
# Spring                24/10/2008       n/a        Add -A automation option   
# Spring                28/11/2008       n/a        Modify COPYRIGHT header
# Spring                06/07/2010       n/a        Add -D hw option
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
# Use command "./dac_test1.sh [audio stream]" 

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

    while getopts f:ANMD arg
    do 
        case $arg in
        f) FILE=$OPTARG;;
        A) AUTO="true";;
        D) HW="true";;
        N|M) ;;
        \?) usage
        exit 67;;
        esac
    done

    trap "cleanup" 0

    if [ ! -e /usr/bin/aplay ]; then
        tst_resm TBROK "Test #1: ALSA utilities are not ready, \
        pls check..."
        RC=65
        return $RC
    fi

    if [ ! -e $FILE ]; then
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

    needpause=$(cat /proc/cpuinfo | grep 378 | wc -l)
    if [ ! -z $needpause ]; then
      sleep 3;
    fi
    
    rm -f /tmp/$basefn
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
    RC=0    # Return value from setup, and test functions.

    tst_resm TINFO "Test #1: play the audio stream, please check the HEADPHONE,\
 hear if there is voice."
    #aplay -D hw:0,0 $1 || RC=$?
    #args=`echo $@|sed 's/-A//g'`
    basefn=$(basename $FILE)
    cp -f $FILE /tmp
    if [ $RC -ne 0 ]; then
        tst_resm TFAIL "Test #1: copy from NFS to tmp error, no space left in /tmp"
        return $RC
    fi

    if [ -n "$HW" ]; then
        aplay -Dhw:0,0 -N -M /tmp/$basefn || RC=$?
        if [ $RC -ne 0 ]; then
            tst_resm TFAIL "Test #1: play error with HW, please check"
            return $RC
        fi
    fi

    aplay -N -M /tmp/$basefn || RC=$?
    if [ $RC -ne 0 ]; then
        tst_resm TFAIL "Test #2: play error, please check"
        return $RC
    fi

    #if auto, ignore ask the answer!
    if [ -n "$AUTO" ]; then
        return $RC
    fi

    tst_resm TINFO "Do you hear the music from the headphone?[y/n]"
    read answer
    if [ $answer = "y" ]; then
        tst_resm TPASS "Test #1: ALSA DAC test success."
    else
        tst_resm TFAIL "Test #1: ALSA DAC play audio fail"
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

    Use this command to test ALSA DAC play functions.
    usage: ./${0##*/} -f [audio stream] -D
            -D: if using hw to playback
            -A: automation mode without interaction
    e.g.: ./${0##*/} -f audio44k16M.wav -A -D

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

dac_play "$@" || exit $RC

