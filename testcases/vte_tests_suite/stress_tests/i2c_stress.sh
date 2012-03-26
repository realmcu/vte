#!/bin/sh
#Copyright (C) 2011 Freescale Semiconductor, Inc. All Rights Reserved.
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
#                       Modification
# Author                    Date        Description of Changes
#--------------------   ------------    ---------------------
# Spring Zhang           27/12/2011     Initial ver. 
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

    export TST_TOTAL=2   # Total number of test cases in this file.
    LTPTMP=${TMP}        # Temporary directory to create files, etc.
    export TCID="TGE_LV_I2C_STRESS"       # Test case identifier
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
    fi

    trap "cleanup" 0
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

    #kill all background processes
    kill -9 $hdmi_pid
    kill -9 $audio_pid
    kill -9 $camera_pid
    kill -9 $ts_pid
    kill -9 $acc_pid
    kill -9 $sensor_pid
    cd $old_dir
    umount /mnt/stream

    echo "clean up environment end"
    return $RC
}

# Function:     basic examination
#
# Description:  - check i2c information and i2c devices in /sys
#
# Exit:         - success: id
#               - "" on failure.
#
basic_examine()
{
    export TCID="TGE_LV_I2C_CHECK"       # Test case identifier
    export TST_COUNT=1   # Set up is initialized as test 0

    echo "i2c information from system log:"
    dmesg | grep -i i2c
    echo

    #i2c_dir should be like:/sys/devices/platform/imx-i2c.0/i2c-0
    i2c_dir=`find /sys/devices/platform/ -name "i2c*" |sed -n '1p'`
    #now it's /sys/devices/platform/imx- 
    i2c_dir=${i2c_dir%%i2c*}

    echo "i2c sys directory is $i2c_dir"
    echo "i2c devices list:"
    for i in 1 2 3; do
        cat ${i2c_dir}i2c.$i/i2c-$i/*/name
    done
}

# Function:     overload_test
#
# Description:  - test on multiple i2c devices simulatenously
#
# Exit:         - zero on success
#               - non-zero on failure.
#
overload_test()
{
    RC=0    # Return value from setup, and test functions.

    if [ $# -le 0 ]; then
        usage
    fi

    while [ $# -gt 0 ]; do
        case $1 in
            audio)
            if [ aplay -l |grep 'card 0' ]; then
                while true; do aplay $STREAM_PATH/alsa_stream/audio44k16S_long.wav; done &
                audio_pid=$!
            else
                echo "WARNING: No sound card found, won't proceed with audio test"
            fi
            shift;;
            camera)
            v4l_capture_testapp -T 1800 &
            camera_pid=$!
            shift;;
            hdmi)
            old_dir=$(pwd)
            mkdir -p /mnt/stream && mount -t nfs -o nolock 10.192.225.210:/d2/01_CodecVectors /mnt/stream && cd /mnt/stream/SHAVectors/H264Dec/Conformance/1080p
            if [ ! -d /mnt/stream/SHAVectors/H264Dec/Conformance/1080p ];then
                echo "WARNING: HDMI test stream mount failed, ignore HDMI test, please check."
            else
                gplay H264_MP40_1920x1080_23.976_9682_AACLC_44.1_98_2_CBR_donmckay.mov
                if [ $? -eq 0 ]; then
                    while true; do gplay H264_MP40_1920x1080_23.976_9682_AACLC_44.1_98_2_CBR_donmckay.mov; done &
                    hdmi_pid=$!
                else
                    echo "WARNING: gplay can't play the streams, please check."
                fi
            fi
            shift;;
            ts)
            evtest /dev/input/ts0 &
            ts_pid=$!
            shift;;
            acc)
            #TODO add support
            shift;;
            sensor)
            #TODO add support
            shift;;
            *) shift;;
        esac
    done

    echo "Please check all i2c device output, e.g. click touchscreen, check HDMI and audio, will sleep 2 mins..."

    sleep 120

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

    Use this command to do I2C stress test
    TCID 1: basic examination test
    TCID 2: multiple i2c device overload test
    usage: ./${0##*/} [TC Id] <more parameters>
            ./${0##*/} 1
            ./${0##*/} 2 audio camera hdmi ts acc sensor
    e.g.: ./${0##*/} 2 audio hdmi ts
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

#"" will pass the whole args to function setup()
setup "$@" || exit $RC

case "$1" in
    1)
    basic_examine || exit $?
    ;;
    2)
    shift
    overload_test "$@" || exit $RC
    ;;
    *)
    usage
    ;;
esac
