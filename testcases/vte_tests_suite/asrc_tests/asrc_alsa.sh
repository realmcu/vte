#!/bin/sh
#Copyright (C) 2012 Freescale Semiconductor, Inc. All Rights Reserved.
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
# Author                   Date       Description of Changes
#-------------------   ------------   ---------------------
# Spring Zhang          19/11/2012    Initial ver. 
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
    export TCID="TGE_LV_ASRC_ALSA"       # Test case identifier
    export TST_COUNT=0   # Set up is initialized as test 0
    BIN_DIR=`dirname $0`
    export PATH=$PATH:$BIN_DIR

    if [ -z $LTPTMP ]
    then
        LTPTMP=/tmp
    fi

    trap "cleanup" 0

    [ ! -z "$STREAM_PATH" ] || {
        tst_resm TBROK "STREAM_PATH not set, pls check!" 
        RC=66
        return $RC
    } 

    # Prepare the config file of 44.1kHz
    if [ -e ~/.asoundrc ]; then
        cp ~/.asoundrc ~/.asoundrc.bak
    fi

    cat > ~/.asoundrc <<-EOF
defaults.pcm.rate_converter "asrcrate" 

pcm.dmix_44100 {
    type dmix
    ipc_key 5678293
    ipc_key_add_uid yes
    slave{
        pcm "hw:0,0" 
        period_time 10000
        format S16_LE
        rate 44100
    }
}

pcm.asrc {
    type plug
    route_policy "average" 
    slave.pcm "dmix_44100" 
}
EOF

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
    echo "Clean up..."
    if [ -e ~/.asoundrc ]; then
        sed -i 's/rate 96000/rate 44100/g' ~/.asoundrc
    fi
    if [ -e ~/.asoundrc.bak ]; then
        mv ~/.asoundrc.bak ~/.asoundrc
    fi
    return $RC
}

# Function:     asrc_plug_playback()
#
# Description:  - asrc test, convert source stream to dest, and play dest stream
#
# Exit:         - zero on success
#               - non-zero on failure.
#
asrc_plug_playback()
{
    export TST_COUNT=1

    RC=0    # Return value from setup, and test functions.

    (sleep 30; rtc_testapp_6 -T 20 -m mem) &
    (sleep 60; sed -i 's/rate 44100/rate 48000/g' ~/.asoundrc) &
    (sleep 90; sed -i 's/rate 48000/rate 32000/g' ~/.asoundrc) &
    (sleep 120; sed -i 's/rate 32000/rate 64000/g' ~/.asoundrc) &
    (sleep 150; sed -i 's/rate 64000/rate 96000/g' ~/.asoundrc) &

    stream_list=`ls ${STREAM_PATH}/alsa_stream_music/audio*.wav| grep -v "24k"`
    error_list=""
    for i in $stream_list; do
        aplay -D asrc $i || RC=$?
        if [ $RC -ne 0 ]; then
            target_rate=`grep 'rate ' ~/.asoundrc |awk '{print $2}'`
            error_list="$error_list $i=>$target_rate"
        fi
    done

    wait

    if [ -n "$error_list" ]; then
        tst_resm TFAIL "Some error during playback, please check list:"
        echo $error_list
        # return last error code
        return $RC
    fi

    tst_resm TPASS "Playback finished."

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

    Use this command to test ASRC ALSA lib plugin
    usage: ./${0##*/} 0 - prepare the .asoundrc config for ASRC lib plugin
    ./${0##*/} 1 - Run test for ASRC lib plugin

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
if [ $# -lt 1 ]; then
    usage
fi

#"" will pass the whole args to function setup()
setup || exit $RC

asrc_plug_playback "$@" || exit $RC

