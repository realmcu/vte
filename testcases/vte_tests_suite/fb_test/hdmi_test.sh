#!/bin/sh -x
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
        echo 0 > /sys/class/graphics/fb0/blank
        sleep 1
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

hdmi_audio_playback_modeSwitch()
{
    num=`aplay -l |grep -i "imxhdmisoc" |awk '{ print $2 }'|sed 's/://'`    
    for i in $(cat /sys/class/graphics/fb0/modes)
    do
    echo $i > /sys/class/graphics/fb0/mode
    real_mode=$(cat /sys/class/graphics/fb0/mode)
    echo "$real_mode"
    if [ $real_mode = $i ]; then
        echo "Already set HDMI mode to $mode, begin Audio playback on HDMI"
	aplay -Dhw:$num $STREAM_PATH/alsa_stream/audio44k16S.wav
    else
        echo "Can not set HDMI mode to $mode"
    RC=2
    fi
		done
    return $RC
}

hdmi_audio_playback_multichannel()
{
	tst_resm TINFO "test hdmi multi channel"
	num=`aplay -l |grep -i "imxhdmisoc" |awk '{ print $2 }'|sed 's/://'`
	stream_path=$STREAM_PATH/esai_stream/
	FILELIST="sine-6ch192k16bit.wav sine-6ch176k16bit.wav sine-6ch96k16bit.wav sine-6ch88k16bit.wav  sine-6ch48k16bit.wav  sine-6ch44k16bit.wav sine-6ch32k16bit.wav sine-8ch192k16bit.wav sine-8ch176k16bit.wav sine-8ch96k16bit.wav sine-8ch88k16bit.wav sine-8ch48k16bit.wav sine-8ch44k16bit.wav sine-8ch32k16bit.wav"    
    	for i in $FILELIST
    	do
		aplay -Dhw:$num ${stream_path}$i || RC=$?
	done
    	return $RC
}

# Function:     main
#
# Description:  - Execute all tests, exit with test status.
#
RC=0    # Return value for setup, and test functions.

setup || exit $RC
if [ "$1" = "all" ]; then
    hdmi_playback_modeSwitch || exit $?
elif [ "$1" = "audiomode" ]; then
    hdmi_audio_playback_modeSwitch || exit $?
elif [ "$1" = "audiochannel" ]; then
    hdmi_audio_playback_multichannel || exit $?
else
    hdmi_playback_asInputMode $1 || exit $RC
fi
