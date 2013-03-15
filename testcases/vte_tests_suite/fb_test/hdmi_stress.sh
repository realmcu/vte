#!/bin/sh -x
#Copyright (C) 2012 Freescale Semiconductor, Inc.
#All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
##############################################################################
#
# Revision History:
#                      Modification
# Author                   Date       Description of Changes
#-------------------   ------------   ---------------------
# Sally Guo               29/05/2012     Initial ver.
# Andy Tian               18/09/2012  Make tools run more flexible.   
# Andy Tian               11/10/2012  Add case_03.   
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
    export TST_TOTAL=1
    export TCID="setup"
    export TST_COUNT=0
	# trap exit and ctrl+c
    trap "cleanup" 0 2
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
	platfm.sh
	platfm=$?
	#check the VTE env
	if [ -z "$STREAM_PATH" ]; then
		RC=1
		echo "Please enable VTE env firstly"
	fi
	
	#find the fb device related with hdmi
	dev_names=`cat /sys/class/graphics/fb*/fsl_disp_dev_property`
	i=0
	for name in $dev_names; do
		if [ "$name" = "hdmi" ]; then
			hdmi_fb="fb$i"
			let out_video=17+i
			break
		fi
		let i=i+1
	done
	if [ $platfm -eq 60 ]; then
		modes=`cat /sys/class/graphics/fb0/modes`
		hdmi_fb=fb0
	else
		modes=`cat /sys/class/graphics/${hdmi_fb}/modes`
	fi
  defvideomodes="U:1920x1080p-30 U:1920x1080p-50 U:1920x1080p-60 U:720x576p-50 U:720x480p-60 U:1280x720p-50 U:1280x720p-60 U:640x480p-60 V:1280x1024p-60 V:1024x768p-60" 
  defaudiomodes="U:1920x1080p-30 U:1920x1080p-50 U:1920x1080p-60 U:720x576p-50 U:720x480p-60 U:1280x720p-50 U:1280x720p-60 U:640x480p-60" 
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
	if [ -n "$pid_video" ]; then
		kill -9 $pid_video
	fi
	sleep 3
	umount /mnt/temp
    RC=0
}
usage()
{
   echo "1: display mode stress test"
   echo "2: audio mode stress test"
   echo "3: HDMI video and audio test under 1080p resolution"
   echo "4: default video stress test"
   echo "5: default audio stress test"
   echo "6: default video and audio stress test"
   echo "7: default modes switch video stress test"
   echo "8: default modes switch audio stress test"
   echo "9: default modes switch audio and video stress test"
   echo "10: RGB YCbCr switch test"
   echo "11: RGB YCbCr switch default test"
   echo "12: Normal modes switch audio and video stress test"
}
test_case_01()
{
#TODO give TCID 
TCID="Display_mode_stress_test"
#TODO give TST_COUNT
TST_COUNT=1
RC=1
echo 0 > /sys/class/graphics/${hdmi_fb}/blank
echo -e "\033[9;0]" > /dev/tty0 
#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
i=0
mkdir -p /mnt/temp
umount /mnt/temp
mount -t tmpfs tmpfs /mnt/temp || exit 1
stream_name=Mpeg4_SP3_1920x1080_23.97fps_9760kbps_AACLC_44KHz_2ch_track1_track1.cmp
cp $STREAM_PATH/video/$stream_name /mnt/temp
loops=5000
while [ $i -lt $loops ]
do
	i=`expr $i + 1`
	for mode in $modes; do
		echo $mode > /sys/class/graphics/${hdmi_fb}/mode
        echo q| fbv $LTPROOT/testcases/bin/butterfly.png -d /dev/${hdmi_fb}
		/unit_tests/mxc_vpu_test.out -D "-f 0 -i /mnt/temp/$stream_name -x $out_video -a 30"|| return $RC
        cur_mode=`cat /sys/class/graphics/${hdmi_fb}/mode`
		if [ "$mode" != "$cur_mode" ]; then
			echo "Error happens during set mode: $mode"
			return $RC
		fi
        echo "times: $i"
        sleep 4
	done
done
RC=0
return $RC
}

test_case_02()
{
#TODO give TCID 
TCID="Audio_mode_stress_test"
#TODO give TST_COUNT
TST_COUNT=2
RC=2
echo 0 > /sys/class/graphics/${hdmi_fb}/blank
echo -e "\033[9;0]" > /dev/tty0
#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
mkdir -p /mnt/temp
umount /mnt/temp
mount -t tmpfs tmpfs /mnt/temp || exit 1
FILES="audio192k16S.wav audio176k16S.wav audio96k16S.wav audio88k16S.wav audio48k16S.wav audio44k16S.wav audio32k16S.wav audio11k16S.wav"
for fname in $FILES
do
cp $STREAM_PATH/alsa_stream/$fname /mnt/temp
done
num=`aplay -l |grep -i "hdmi" |awk '{ print $2 }'|sed 's/://'`

i=0
loops=10000
while [ $i -lt $loops ]
do
    i=`expr $i + 1`
	for mode in $modes; do
		echo $mode > /sys/class/graphics/${hdmi_fb}/mode
        aplay -Dplughw:$num -M /mnt/temp/audio*.wav || { RC=21; return $RC; }
        cur_mode=`cat /sys/class/graphics/${hdmi_fb}/mode`
		if [ "$mode" != "$cur_mode" ]; then
			echo "Error happens during set mode: $mode"
			return $RC;
		fi
        echo "times: $i"
        sleep 4
	done
done
RC=0
return $RC
}

test_case_03()
{
#TODO give TCID 
TCID="HDMI_video_audio_test_1080p resolution"
#TODO give TST_COUNT
TST_COUNT=3
RC=3
echo 0 > /sys/class/graphics/${hdmi_fb}/blank
echo -e "\033[9;0]" > /dev/tty0
#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#check mode is 1080p, if not, set it
mode=`cat /sys/class/graphics/${hdmi_fb}/mode`
echo $mode | grep "1920x1080"
if [ $? -ne 0 ]; then
	#set 1080p mode
	mode_1080=`cat /sys/class/graphics/${hdmi_fb}/modes | grep -m 1 "1920x1080"`
	if [ $? -ne 0 ]; then
		#1080p mode is not supported by display, exit
		echo "1080p mode is not supported by this display, case can not run."
		return $RC
	else
		echo ${mode_1080} > /sys/class/graphics/${hdmi_fb}/mode
	fi
fi

#1080P video playback 
num=`aplay -l |grep -m 1 -i "hdmi" |awk '{ print $2 }'|sed 's/://'`
a_stream=ToyStory3_H264HP_1920x1080_10Mbps_24fps_AAC_48kHz_192kbps_2ch_track1.h264
mkdir -p /mnt/temp
umount /mnt/temp
mount -t tmpfs tmpfs /mnt/temp || exit 1
FILES="audio192k16S.wav audio176k16S.wav audio96k16S.wav audio88k16S.wav audio48k16S.wav audio44k16S.wav audio32k16S.wav audio11k16S.wav"
for fname in $FILES
do
cp $STREAM_PATH/alsa_stream/$fname /mnt/temp
done
cp $STREAM_PATH/video/$a_stream /mnt/temp

#audio + video playback
loops=5000
i=0
while [ $i -lt $loops ]; do
	/unit_tests/mxc_vpu_test.out -D "-f 2 -i /mnt/temp/${a_stream} -x $out_video -a 30" &
	pid_video=$!
    aplay -Dplughw:$num -M /mnt/temp/audio*.wav || return $RC
    aplay -Dplughw:$num -M /mnt/temp/audio*.wav || return $RC
    aplay -Dplughw:$num -M /mnt/temp/audio*.wav || return $RC
	wait $pid_video || return $RC
	let i=i+1
done

RC=0
return $RC
}

test_case_04()
{
#TODO give TCID 
TCID="default_video_stress_test"
#TODO give TST_COUNT
TST_COUNT=4
RC=4
echo 0 > /sys/class/graphics/${hdmi_fb}/blank
echo -e "\033[9;0]" > /dev/tty0 
#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
i=0
mkdir -p /mnt/temp
umount /mnt/temp
mount -t tmpfs tmpfs /mnt/temp || exit 1
stream_name=Mpeg4_SP3_1920x1080_23.97fps_9760kbps_AACLC_44KHz_2ch_track1_track1.cmp
b_stream=H264_HP51_bwp_1280x720.h264
cp $STREAM_PATH/video/$stream_name /mnt/temp
cp $STREAM_PATH/video/$b_stream /mnt/temp
loops=300
while [ $i -lt $loops ]
do
	i=`expr $i + 1`
	/unit_tests/mxc_vpu_test.out -D "-f 0 -i /mnt/temp/$stream_name -x $out_video -a 30" || { RC=41; return $RC; }
	/unit_tests/mxc_vpu_test.out -D "-i ${STREAM_PATH}/video/H264_HP51_bwp_1280x720.h264 -f 2" || { RC=42; return $RC; }
  echo "times: $i"
  sleep 4
done
RC=0
return $RC
}

test_case_05()
{
#TODO give TCID 
TCID="default_audio_stress_test"
#TODO give TST_COUNT
TST_COUNT=5
RC=5
echo 0 > /sys/class/graphics/${hdmi_fb}/blank
echo -e "\033[9;0]" > /dev/tty0 
#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
i=0
mkdir -p /mnt/temp
umount /mnt/temp
mount -t tmpfs tmpfs /mnt/temp || exit 1
FILES="audio96k16S.wav audio88k16S.wav audio48k16S.wav audio44k16S.wav audio32k16S.wav audio11k16S.wav"
for fname in $FILES
do
cp $STREAM_PATH/alsa_stream_music/$fname /mnt/temp
done
num=`aplay -l |grep -m 1 -i "hdmi" |awk '{ print $2 }'|sed 's/://'`
loops=300
while [ $i -lt $loops ]
do
	i=`expr $i + 1`
  aplay -Dplughw:$num -M /mnt/temp/*.wav || return $RC
  echo "times: $i"
  sleep 4
done
RC=0
return $RC
}
test_case_06()
{
#TODO give TCID 
TCID="default_video_audio_stress_test"
#TODO give TST_COUNT
TST_COUNT=6
RC=6
echo 0 > /sys/class/graphics/${hdmi_fb}/blank
echo -e "\033[9;0]" > /dev/tty0 
#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
i=0
mkdir -p /mnt/temp
umount /mnt/temp
mount -t tmpfs tmpfs /mnt/temp || exit 1
stream_name=Mpeg4_SP3_1920x1080_23.97fps_9760kbps_AACLC_44KHz_2ch_track1_track1.cmp
cp $STREAM_PATH/video/$stream_name /mnt/temp
cp $STREAM_PATH/alsa_stream/audio44k16S.wav /mnt/temp
num=`aplay -l |grep -m 1 -i "hdmi" |awk '{ print $2 }'|sed 's/://'`
loops=300
while [ $i -lt $loops ]
do
	i=`expr $i + 1`
	/unit_tests/mxc_vpu_test.out -D "-f 0 -i /mnt/temp/$stream_name -x $out_video -a 30" & 
  pid_video=$!
  aplay -Dplughw:$num -M /mnt/temp/audio44k16S.wav || { RC=61; return $RC; }
  wait $pid_video || { RC=62; return $RC; }
  echo "times: $i"
  sleep 4
done
RC=0
return $RC
}
test_case_07()
{
#TODO give TCID 
TCID="default_modes_switch_video_stress_test"
#TODO give TST_COUNT
TST_COUNT=7
RC=7
echo 0 > /sys/class/graphics/${hdmi_fb}/blank
echo -e "\033[9;0]" > /dev/tty0 
#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
i=0
mkdir -p /mnt/temp
umount /mnt/temp
mount -t tmpfs tmpfs /mnt/temp || exit 1
stream_name=ToyStory3_H264HP_1920x1080_10Mbps_24fps_AAC_48kHz_192kbps_2ch_track1.h264
cp $STREAM_PATH/video/$stream_name /mnt/temp
loops=20
while [ $i -lt $loops ]
do
	i=`expr $i + 1`
	for mode in $defvideomodes; do
		echo $mode > /sys/class/graphics/${hdmi_fb}/mode
    echo q| fbv $LTPROOT/testcases/bin/butterfly.png -d /dev/${hdmi_fb}
		/unit_tests/mxc_vpu_test.out -D "-f 2 -i /mnt/temp/$stream_name -x $out_video -a 30"|| { RC=71; return $RC; }
    cur_mode=`cat /sys/class/graphics/${hdmi_fb}/mode`
		if [ "$mode" != "$cur_mode" ]; then
			echo "Error happens during set mode: $mode"
			return $RC
		fi
      echo "times: $i"
      sleep 4
	done
done
RC=0
return $RC
}

test_case_08()
{
#TODO give TCID 
TCID="default_modes_switch_audio_stress_test"
#TODO give TST_COUNT
TST_COUNT=8
RC=8
echo 0 > /sys/class/graphics/${hdmi_fb}/blank
echo -e "\033[9;0]" > /dev/tty0
#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
mkdir -p /mnt/temp
umount /mnt/temp
mount -t tmpfs tmpfs /mnt/temp || exit 1
FILES="audio192k16S.wav audio176k16S.wav audio96k16S.wav audio88k16S.wav audio48k16S.wav audio44k16S.wav audio32k16S.wav audio11k16S.wav"
for fname in $FILES
do
cp $STREAM_PATH/alsa_stream/$fname /mnt/temp
done
num=`aplay -l |grep -i "hdmi" |awk '{ print $2 }'|sed 's/://'`

i=0
loops=20
while [ $i -lt $loops ]
do
    i=`expr $i + 1`
	for mode in $defaudiomodes; do
		 echo $mode > /sys/class/graphics/${hdmi_fb}/mode
     aplay -Dplughw:$num -M /mnt/temp/audio*.wav || { RC=81; return $RC; }
     cur_mode=`cat /sys/class/graphics/${hdmi_fb}/mode`
		 if [ "$mode" != "$cur_mode" ]; then
			  echo "Error happens during set mode: $mode"
			  return $RC
		 fi
        echo "times: $i"
        sleep 4
	done
done
RC=0
return $RC
}
test_case_09()
{
#TODO give TCID 
TCID="default_modes_switch_audio_video_stress_test"
#TODO give TST_COUNT
TST_COUNT=9
RC=9
echo 0 > /sys/class/graphics/${hdmi_fb}/blank
echo -e "\033[9;0]" > /dev/tty0
#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
#1080P video playback 
num=`aplay -l |grep -m 1 -i "hdmi" |awk '{ print $2 }'|sed 's/://'`
a_stream=ToyStory3_H264HP_1920x1080_10Mbps_24fps_AAC_48kHz_192kbps_2ch_track1.h264
mkdir -p /mnt/temp
umount /mnt/temp
mount -t tmpfs tmpfs /mnt/temp || exit 1
FILES="audio192k16S.wav audio176k16S.wav audio96k16S.wav audio88k16S.wav audio48k16S.wav audio44k16S.wav audio32k16S.wav audio11k16S.wav"
for fname in $FILES
do
cp $STREAM_PATH/alsa_stream/$fname /mnt/temp
done
cp $STREAM_PATH/video/$a_stream /mnt/temp

#audio + video playback
loops=20
i=0
while [ $i -lt $loops ]; do
    i=`expr $i + 1`
   	for mode in $defaudiomodes; do
   	echo $mode > /sys/class/graphics/${hdmi_fb}/mode
	  /unit_tests/mxc_vpu_test.out -D "-f 2 -i /mnt/temp/${a_stream} -x $out_video -a 30" &
   	pid_video=$!
    aplay -Dplughw:$num -M /mnt/temp/audio*.wav || { RC=91; return $RC; }
	  wait $pid_video || { RC=92; return $RC; }
	  done
done
RC=0
return $RC
}
test_case_10()
{
#TODO give TCID 
TCID="RGB_and_YCbCr_switch_test"
#TODO give TST_COUNT
TST_COUNT=10
RC=10
echo 0 > /sys/class/graphics/${hdmi_fb}/blank
echo -e "\033[9;0]" > /dev/tty0
#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
#1080P video playback 
num=`aplay -l |grep -m 1 -i "hdmi" |awk '{ print $2 }'|sed 's/://'`
mkdir -p /mnt/temp
umount /mnt/temp
mount -t tmpfs tmpfs /mnt/temp || exit 1
cp $STREAM_PATH/alsa_stream/audio44k16S.wav /mnt/temp
cp $STREAM_PATH/video/H264_DAKEAI1080.avi /mnt/temp

#audio + video playback
loops=20
i=0
while [ $i -lt $loops ]; do
    i=`expr $i + 1`
    mod=$(expr $i % 2)
    if [ $mod -eq 0 ]; then
    echo 1 > /sys/devices/platform/mxc_hdmi/rgb_out_enable
    else
    echo 0 > /sys/devices/platform/mxc_hdmi/rgb_out_enable
    fi
    cat /sys/devices/platform/mxc_hdmi/rgb_out_enable
   	for mode in $modes; do
   	echo $mode > /sys/class/graphics/${hdmi_fb}/mode
   	cat /sys/class/graphics/${hdmi_fb}/mode
	  /unit_tests/mxc_vpu_test.out -D "-f 2 -i /mnt/temp/H264_DAKEAI1080.avi -x $out_video" &
   	pid_video=$!
    aplay -Dplughw:$num -M /mnt/temp/audio*.wav || { RC=100; return $RC; }
	  wait $pid_video || { RC=101; return $RC; }
	  done
done
RC=0
return $RC
}

test_case_11()
{
#TODO give TCID 
TCID="RGB_and_YCbCr_switch_default_modes_test"
#TODO give TST_COUNT
TST_COUNT=11
RC=11
echo 0 > /sys/class/graphics/${hdmi_fb}/blank
echo -e "\033[9;0]" > /dev/tty0
#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
#1080P video playback 
num=`aplay -l |grep -m 1 -i "hdmi" |awk '{ print $2 }'|sed 's/://'`
mkdir -p /mnt/temp
umount /mnt/temp
mount -t tmpfs tmpfs /mnt/temp || exit 1
cp $STREAM_PATH/alsa_stream/audio44k16S.wav /mnt/temp
cp $STREAM_PATH/video/H264_DAKEAI1080.avi /mnt/temp

#audio + video playback
loops=20
i=0
while [ $i -lt $loops ]; do
    i=`expr $i + 1`
    mo=$(expr $i % 2)
    if [ $mo = 0 ]; then
    echo 1 > /sys/devices/platform/mxc_hdmi/rgb_out_enable
    else
    echo 0 > /sys/devices/platform/mxc_hdmi/rgb_out_enable
    fi
    cat /sys/devices/platform/mxc_hdmi/rgb_out_enable
   	for mode in $defaudiomodes; do
   	echo $mode > /sys/class/graphics/${hdmi_fb}/mode
   	cat /sys/class/graphics/${hdmi_fb}/mode
	  /unit_tests/mxc_vpu_test.out -D "-f 2 -i /mnt/temp/H264_DAKEAI1080.avi -x $out_video" &
   	pid_video=$!
    aplay -Dplughw:$num -M /mnt/temp/audio44k16S.wav || { RC=110; return $RC; }
	  wait $pid_video || { RC=111; return $RC; }
	  done
done
RC=0
return $RC
}
test_case_12()
{
#TODO give TCID 
TCID="Normal_modes_switch_audio_video_stress_test"
#TODO give TST_COUNT
TST_COUNT=12
RC=12
echo 0 > /sys/class/graphics/${hdmi_fb}/blank
echo -e "\033[9;0]" > /dev/tty0
#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
#1080P video playback 
num=`aplay -l |grep -m 1 -i "hdmi" |awk '{ print $2 }'|sed 's/://'`
a_stream=ToyStory3_H264HP_1920x1080_10Mbps_24fps_AAC_48kHz_192kbps_2ch_track1.h264
mkdir -p /mnt/temp
umount /mnt/temp
mount -t tmpfs tmpfs /mnt/temp || exit 1
FILES="audio192k16S.wav audio176k16S.wav audio96k16S.wav audio88k16S.wav audio48k16S.wav audio44k16S.wav audio32k16S.wav audio11k16S.wav"
for fname in $FILES
do
cp $STREAM_PATH/alsa_stream/$fname /mnt/temp
done
cp $STREAM_PATH/video/$a_stream /mnt/temp

#audio + video playback
loops=20
i=0
while [ $i -lt $loops ]; do
    i=`expr $i + 1`
   	for mode in $modes; do
   	echo $mode > /sys/class/graphics/${hdmi_fb}/mode
	  /unit_tests/mxc_vpu_test.out -D "-f 2 -i /mnt/temp/${a_stream} -x $out_video -a 30" &
   	pid_video=$!
    aplay -Dplughw:$num -M /mnt/temp/audio*.wav || { RC=120; return $RC; }
	  wait $pid_video || { RC=121; return $RC; }
	  done
done
RC=0
return $RC
}
# Function:     main
#
# Description:  - Execute all tests, exit with test status.
#
RC=0    # Return value for setup, and test functions.
Platfm=63

setup || exit $RC
case "$1" in
1)
  test_case_01 || exit $RC
  ;;
2)
  test_case_02 || exit $RC
  ;;
3)
  test_case_03 || exit $RC
  ;;
4)
  test_case_04 || exit $RC
  ;;
5)
  test_case_05 || exit $RC
  ;;
6)
  test_case_06 || exit $RC
  ;;
7)
  test_case_07 || exit $RC
  ;;
8)
  test_case_08 || exit $RC
  ;;
9)
  test_case_09 || exit $RC
  ;;
10)
  test_case_10 || exit $RC
  ;;
11)
  test_case_11 || exit $RC
  ;;
12)
  test_case_12 || exit $RC
  ;;
*)
  usage
  ;;
esac
tst_resm TINFO "Test Finish"
