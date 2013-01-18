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
}
test_case_01()
{
#TODO give TCID 
TCID="Display mode stress test"
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
		/unit_tests/mxc_vpu_test.out -D "-f 0 -i /mnt/temp/$stream_name -x $out_video -a 30"|| exit 1
        cur_mode=`cat /sys/class/graphics/${hdmi_fb}/mode`
		if [ "$mode" != "$cur_mode" ]; then
			echo "Error happens during set mode: $mode"
			exit 1
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
TCID="Audio mode stress test"
#TODO give TST_COUNT
TST_COUNT=2
RC=2
echo 0 > /sys/class/graphics/${hdmi_fb}/blank
echo -e "\033[9;0]" > /dev/tty0
#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
mkdir -p /mnt/temp
mount -t tmpfs tmpfs /mnt/temp || exit 2
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
        aplay -Dplughw:$num -M /mnt/temp/audio*.wav || exit 2
        cur_mode=`cat /sys/class/graphics/${hdmi_fb}/mode`
		if [ "$mode" != "$cur_mode" ]; then
			echo "Error happens during set mode: $mode"
			exit 2
		fi
        echo "times: $i"
        sleep 4
	done
done
umount /mnt/temp
RC=0
return $RC
}

test_case_03()
{
#TODO give TCID 
TCID="HDMI video and audio test under 1080p resolution"
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
		exit 3
	else
		echo ${mode_1080} > /sys/class/graphics/${hdmi_fb}/mode
	fi
fi

#1080P video playback 
num=`aplay -l |grep -m 1 -i "hdmi" |awk '{ print $2 }'|sed 's/://'`
a_stream=ToyStory3_H264HP_1920x1080_10Mbps_24fps_AAC_48kHz_192kbps_2ch_track1.h264
mkdir -p /mnt/temp
umount /mnt/temp
mount -t tmpfs tmpfs /mnt/temp || exit 3
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
    aplay -Dplughw:$num -M /mnt/temp/audio*.wav || exit 3
    aplay -Dplughw:$num -M /mnt/temp/audio*.wav || exit 3
    aplay -Dplughw:$num -M /mnt/temp/audio*.wav || exit 3
	wait $pid_video || exit 3
	let i=i+1
done

RC=0
return $RC
}

test_case_04()
{
#TODO give TCID 
TCID="default video stress test"
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
	/unit_tests/mxc_vpu_test.out -D "-f 0 -i /mnt/temp/$stream_name -x $out_video -a 30" || exit 41
	/unit_tests/mxc_vpu_test.out -D "-i ${STREAM_PATH}/video/H264_HP51_bwp_1280x720.h264 -f 2" || exit 42
  echo "times: $i"
  sleep 4
done
RC=0
return $RC
}

test_case_05()
{
#TODO give TCID 
TCID="default audio stress test"
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
  aplay -Dplughw:$num -M /mnt/temp/*.wav || exit 51
  echo "times: $i"
  sleep 4
done
RC=0
return $RC
}
test_case_06()
{
#TODO give TCID 
TCID="default video and audio stress test"
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
  aplay -Dplughw:$num -M /mnt/temp/audio44k16S.wav || exit 61
  wait $pid_video || exit 62
  echo "times: $i"
  sleep 4
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
  test_case_01 || exit 1
  ;;
2)
  test_case_02 || exit 2
  ;;
3)
  test_case_03 || exit 3
  ;;
4)
  test_case_04 || exit 4
  ;;
5)
  test_case_05 || exit 5
  ;;
6)
  test_case_06 || exit 6
  ;;
*)
  usage
  ;;
esac
tst_resm TINFO "Test Finish"
