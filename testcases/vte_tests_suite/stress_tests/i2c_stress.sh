
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
    kill -9 $ecompass_pid
	kill -9 $tvin_pid
	kill -9 $sensor_pid
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
        echo ${i2c_dir}i2c.$i/i2c-$i/
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
            aplay -l |grep 'card 0'
            if [ $? -eq 0 ]; then
                aplay $STREAM_PATH/alsa_stream/*.wav &
                audio_pid=$!
            else
                echo "WARNING: No sound card found, won't proceed with audio test"
            fi
            shift;;
            camera)
            v4l_capture_testapp -C 2 -T 1800 -s CSI_MEM -O YUV420 -M 2 &
            camera_pid=$!
            shift;;
            hdmi)
                 /unit_tests/mxc_vpu_test.out -D "-i ${STREAM_PATH}/video/H264_ML_1920x1080_10Mbps_15fps_noaudio.h264 -f 2 -x 18" &
                 hdmi_pid=$!
            shift;;
            ts)
            evtest /dev/input/ts0 &
            ts_pid=$!
            shift;;
            acc)
                 acc_test_MMA845x.sh -m MODE_2G &
                 acc_pid=$!
                 shift;;
            ecompass)
                 ecompass_mag3110.sh 1 &
                 ecompass_pid=$!
                 shift;;
            tvin)
                tvin_test.sh 2 &
                tvin_pid=$!
                shift;;
            sensor)
                 /unit_tests/mxc_isl29023.out &                                                                                                    
                 sensor_pid=$!
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
            ./${0##*/} 2 audio camera hdmi ts acc sensor ecompass tvin
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
