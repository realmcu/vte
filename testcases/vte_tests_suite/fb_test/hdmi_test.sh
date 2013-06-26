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
# Andy Tian            05/12/2012     Copy to mem for multi-channel case to avoid
#                                     underrun issue when large size audio stream
#                                     used.
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
    export TST_TOTAL=4   # Total number of test cases in this file.
    export TCID="TGE_LV_HDMI_TEST"       # Test case identifier
    export TST_COUNT=0   # Set up is initialized as test 0

    platfm.sh || platid=$?
    if [ $platid -eq 53 ] || [ $platid -eq 60 ]; then
        cable_dir="/sys/devices/platform/sii902x.0"
    else
    	  if [ `uname -r` > "3.5" ]; then
	     cable_dir="/sys/devices/soc.0/20e0000.hdmi_video"
	  else
	     cable_dir="/sys/devices/platform/mxc_hdmi"        
	  fi
    fi

    if [ $(cat /proc/cmdline | grep hdmi | wc -l) -eq 1 ]; then
       echo "Already enable HDMI in boot cmdline"
       if [ "$(cat $cable_dir/cable_state)" = "plugout" ]; then
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
    umount /mnt/temp
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
		dd if=/dev/urandom of=/dev/fb0 bs=1024 count=1024
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
    modes=`cat /sys/class/graphics/fb0/modes`
    if [ $def = 'true' ]; then
    modes="U:1920x1080p-30 U:1920x1080p-50 U:1920x1080p-60 U:720x576p-50 U:720x480p-60 U:1280x720p-50 U:1280x720p-60 U:640x480p-60 V:1280x1024p-60 V:1024x768p-60"
    fi
    for i in $modes
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
    modes=`cat /sys/class/graphics/fb0/modes`
    if [ $def = 'true' ]; then
    modes="U:1920x1080p-30 U:1920x1080p-50 U:1920x1080p-60 U:720x576p-50 U:720x480p-60 U:1280x720p-50 U:1280x720p-60"
    fi
    for i in $modes
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
    export TST_COUNT=3   # Set up is initialized as test 0
    RC=4
    tst_resm TINFO "Test HDMI multi channel"
    fbx="0 1 2 3 4"
    for i in $fbx; do
        echo 0 > /sys/class/graphic/fb$i/blank
    done
	  echo -e "\033[9;0]" > /dev/tty0
    num=`aplay -l |grep -i "imxhdmisoc" |awk '{ print $2 }'|sed 's/://'`
    mkdir -p /mnt/usb
    umount /mnt/usb
    mount -t ext3 /dev/sda1 /mnt/usb || exit 1
    stream_path=$STREAM_PATH/esai_stream
    FILELIST="sine-6ch192k16bit.wav sine-6ch176k16bit.wav sine-6ch96k16bit.wav sine-6ch88k16bit.wav sine-6ch48k16bit.wav sine-6ch44k16bit.wav sine-6ch32k16bit.wav \
              sine-8ch192k16bit.wav sine-8ch176k16bit.wav sine-8ch96k16bit.wav sine-8ch88k16bit.wav sine-8ch48k16bit.wav sine-8ch44k16bit.wav sine-8ch32k16bit.wav \
              32k24bit-six-S24_LE.wav 44k24bit-six-S24_LE.wav 48k24bit-six-S24_LE.wav 88k24bit-six-S24_LE.wav 96k24bit-six-S24_LE.wav sine-8ch192k24bit.wav sine-8ch48k24bit.wav"    
    for i in $FILELIST; do
        cp $stream_path/$i /mnt/temp
    done    
    tst_resm TINFO "Use plughw to playback" 
        aplay -Dplughw:$num -M /mnt/temp/*.wav || return $RC
    RC=0
    return $RC
}

# Function:     main
#
# Description:  - Execute all tests, exit with test status.
#
RC=0    # Return value for setup, and test functions.

setup || exit $RC
if [ "$2" = "default" ];then
def=true
fi
if [ "$1" = "all" ]; then
    hdmi_playback_modeSwitch || exit $RC
elif [ "$1" = "audiomode" ]; then
    hdmi_audio_playback_modeSwitch || exit $RC
elif [ "$1" = "audiochannel" ]; then
    hdmi_audio_playback_multichannel || exit $RC
else
    hdmi_playback_asInputMode $1 || exit $RC
fi
