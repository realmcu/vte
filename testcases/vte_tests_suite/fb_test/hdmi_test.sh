#!/bin/sh
#Copyright (C) 2012 Freescale Semiconductor, Inc.
#All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#!/bin/sh
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
##############################################################################
#
# Revision History:
#                      Modification
# Author                   Date       Description of Changes
#-------------------   ------------   ---------------------
# Ada Lu               25/04/2012     Initial ver.
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
    if [ $(cat /proc/cmdline | grep hdmi | wc -l) -eq 1 ]; then
       echo "Already enable HDMI in boot cmdline"
       if [ $(cat /sys/devices/platform/mxc_hdmi/cable_state) = "plugout" ]; then
          echo "Not plug in HDMI cable in board"
          RC=1
       fi
    else
       echo "Not enable HDMI in boot cmdline"
       RC=1
     fi
       
    return $RC
       
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
}

# Function:     
#
# Description   - display under different HDMI mode
#
# Return        - zero on success
#               - non zero on failure. return value from commands ($RC)
hdmi_playback_asInputMode()
{
    mode=$1
    echo $mode > /sys/class/graphics/fb0/mode
    real_mode=$(cat /sys/class/graphics/fb0/mode)

    if [ $real_mode = $mode ]; then
        echo "Already set HDMI mode to $mode, begin playback on HDMI"
        /unit_tests/mxc_vpu_test.out -D "-i ${STREAM_PATH}/video/H264_HP51_bwp_1280x720.h264 -f 2"
        /unit_tests/mxc_vpu_test.out -D "-i ${STREAM_PATH}/video/H264_DAKEAI1080.avi -f 2"
        echo "========display normally? Input y or n"
        read; echo "========display normally? $REPLY"
        if [ $REPLY = 'y' ]; then
            echo "Display normally on HDMI mode $mode"
            RC=0
        else
            echo "Can not display normally on HDMI mode $mode"
            RC=3
        fi
    else
        echo "Can not set HDMI mode to $mode"
    RC=1
    fi

    return $RC
}


hdmi_playback_modeSwitch()
{
    mode_errorNO=0
    for i in $(cat /sys/class/graphics/fb0/modes)
    do
        hdmi_playback_asInputMode $i
        if [ $? -ne 0 ];then
            mode_errorNO=$(($mode_errorNO+1))
            echo ======$mode_errorNO
        fi 
    done
    echo "Total $mode_errorNO HDMI mode can not display correctly"
    return $mode_errorNO
}

# Function:     main
#
# Description:  - Execute all tests, exit with test status.
#
RC=0    # Return value for setup, and test functions.

setup || exit $RC
if [ $1 == all ]; then
    hdmi_playback_modeSwitch || exit $?
else
    hdmi_playback_asInputMode $1 || exit $RC
fi
