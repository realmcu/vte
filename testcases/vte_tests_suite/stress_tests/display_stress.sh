#!/bin/bash
# Copyright (C) 2011 Freescale Semiconductor, Inc. All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
#    @file   display_stress.sh
#
#    @brief  shell script template for testcase design "cpu freq test" is where to modify block.
#
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
# 



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
#TODO Total test case
export TST_TOTAL=4

export TCID="setup"
export TST_COUNT=0
RC=0

trap "cleanup" 0

#TODO add setup scripts
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

#TODO add cleanup code here

return $RC
}

run_auto_test_list()
{
   RC=0
   mkfs.vfat /dev/sda1 || return 15
   mkdir -p /media/sda1; mount -t vfat /dev/sda1 /media/sda1 || return $16
	 echo "USB Host test"
	 bonnie\+\+ -d /media/sda1 -u 0:0 -s 500 -r 100 &
	 loop=100
	 while [ $loop -gt 0 ]
		 do
   echo "uart test"
   cat /etc/passwd > /dev/ttymxc0 || return $RC
   echo "ALSA test"
   aplay -vv $STREAM_PATH/alsa_stream/audio44k16M.wav || return 14
	 arecord -D plughw:0 -f S16_LE -r 44100 -c 2 -traw | aplay -D plughw:0 -f S16_LE -r 44100 -c 2 || RC='$RC 1'  || return 141
	  loop=$(expr $loop - 1)
	 done
	 wait
	 RC=$?
	 return $RC
}

# Function:     test_case_01
# Description   - Test if <CPU freq> ok
#  
test_case_01()
{
#TODO give TCID 
TCID="test_clock_stress"
#TODO give TST_COUNT
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

echo 0 > /sys/class/graphics/fb0/blank
echo 1 > /sys/class/graphics/fb0/blank
echo 2 > /sys/class/graphics/fb0/blank

echo -e "\033[9;0]" > /dev/tty0

#test list
stream_benchmark &
dry2 &
export VDK_FRAMEBUFFER=/dev/fb2
gles_viv.sh 1 &
loops=100
while [ $loops -gt 0 ]
	do
lcd_testapp -T 1 -B /dev/fb0 -D 16 -X 1
  loops=$(expr $loops - 1)
  done
wait
RC=$?
return $RC
}

# Function:     test_case_02
# Description   - Test if bus stress test ok
#  
test_case_02()
{
#TODO give TCID 
TCID="bus_test"
#TODO give TST_COUNT
TST_COUNT=2
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
export VDK_FRAMEBUFFER=/dev/fb2
gles_viv.sh 1 &
run_auto_test_list &
loops=100
while [ $loops -gt 0 ]
	do
lcd_testapp -T 1 -B /dev/fb0 -D 16 -X 1
  loops=$(expr $loops - 1)
  done
#TODO add function test scripte here
wait
RC=$?
return $RC
}

# Function:     test_case_03
# Description   - Test if multi-stress ok
#  
test_case_03()
{
#TODO give TCID 
TCID="multi_stress_test"
#TODO give TST_COUNT
TST_COUNT=3
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
   echo "ADC DAC test"
	 dac_test1.sh -f $STREAM_PATH/alsa_stream/audio16k16M.wav -A
	 sh -c "arecord -D plughw:0 -f S16_LE -r 44100 -c 2 -traw | aplay -D plughw:0 -f S16_LE -r 44100 -c 2 || RC='$RC 1' &" 
	 sleep 2
	 echo "USB Host test"
   sh -c "mkfs.vfat /dev/sda1 && mkdir -p /media/sda1; mount -t vfat /dev/sda1 /media/sda1 && bonnie\+\+ -d /media/sda1 -u 0:0 -s 10 -r 5 && dt of=/media/sda1/test_file bs=4k limit=128m passes=20 || RC='$RC 2' &"
	 echo "SD test"
	 sleep 2
	 sh -c "mkdir -p /mnt/mmc && mkfs.vfat /dev/mmcblk0p1 && mount /dev/mmcblk0p1 /mnt/mmc && bonnie\+\+ -d /mnt/mmc -u 0:0 -s 10 -r 5 && dt of=/mnt/mmc/test_file bs=4k limit=128m passes=20 || RC='$RC 3' &"
	 echo "epdc test"
	 sh -c "epdc_test -T 7 || RC='$RC 4' &"
	 sleep 2
	 echo "wifi test"
	 sh -c "modprobe ath6kl ; sleep 10; ifconfig wlan0 up && iwconfig wlan0 mode managed && sleep 5 &&iwlist wlan0 scanning | grep FSLLBGAP_001 && iwconfig wlan0 key $(echo Happy123 | md5sum | cut -c 1-10) && iwconfig wlan0 essid FSLLBGAP_001 && sleep 5 && udhcpc -i wlan0"
	 export LOCALIP=$(ifconfig wlan0 | grep inet |  cut -d: -f 2 | awk '{print $1}')
   cd ${LTPROOT}/testcases/bin
	 if [ ! -z $LOCALIP ];then
	 sh -c "tcp_stream_2nd_script 10.192.225.222 CPU $LOCALIP || RC='$RC 5' &"
	 fi
   echo "gpu test"
	 modprobe gpu
	 #sh -c "gpu_test.sh 2 || RC='$RC 6' &"
	 sh -c "tiger &"
  read -p "use Ctrl+c to quit"
return $RC
}

# Function:     test_case_04
# Description   - Test if dual 1080P ok
#  
test_case_04()
{
#TODO give TCID 
TCID="test_dual_1080P_test"
#TODO give TST_COUNT
TST_COUNT=4
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

#a_stream_path=/mnt/nfs/test_stream/power_stream/ToyStory3_H264HP_1920x1080_10Mbps_24fps_AAC_48kHz_192kbps_2ch.mp4
#b_stream_path=/mnt/nfs/test_stream/power_stream/V031204_Times_Square_traffic_1920x1088p30_20Mbps_600frm_H264_noaudio.mp4
a_stream_path=/mnt/nfs/test_stream/power_stream/ToyStory3_H264HP_1920x1080_10Mbps_24fps_AAC_48kHz_192kbps_2ch_track1.h264
b_stream_path=/mnt/nfs/test_stream/video/Mpeg4_SP3_1920x1080_23.97fps_9760kbps_AACLC_44KHz_2ch_track1_track1.cmp

/unit_tests/mxc_vpu_test.out -D "-f 2 -i ${a_stream_path}" &

/unit_tests/mxc_vpu_test.out -D "-f 0 -i ${b_stream_path} " &

wait

RC=$?

return $RC

}

# Function:     test_case_05
# Description   - Test if 1080P playback + 720P enc ok
#  
test_case_05()
{
#TODO give TCID 
TCID="test_1080DEC_720ENC_test"
#TODO give TST_COUNT
TST_COUNT=5
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

a_stream_path=/mnt/nfs/test_stream/video/Mpeg4_SP3_1920x1080_23.97fps_9760kbps_AACLC_44KHz_2ch_track1_track1.cmp
/unit_tests/mxc_vpu_test.out -D "-f 0 -i ${a_stream_path}" &

/unit_tests/mxc_vpu_test.out -E "-f 2 -i /dev/zero -w 1080 -h 720 -o /dev/null -c 100" &

wait

RC=$?

return $RC

}

# Function:     test_case_06
# Description   - Test if 720P camera loopback test enc ok
#  
test_case_06()
{
#TODO give TCID 
TCID="test_720CLOOPBACK_test"
#TODO give TST_COUNT
TST_COUNT=5
RC=0

/unit_tests/mxc_vpu_test.out -L "-f 2 -w 1080 -h 720 -t 1" &
pid=$!
a_stream_path=/mnt/nfs/test_stream/power_stream/ToyStory3_H264HP_1920x1080_10Mbps_24fps_AAC_48kHz_192kbps_2ch_track1.h264
/unit_tests/mxc_vpu_test.out -D "-f 2 -i ${a_stream_path} -o /dev/fb0" || RC=1
sleep 15
kill -9 $pid

return $RC
}

usage()
{
echo "$0 [case ID]"
echo "1: "
echo "2: "
echo "3: "
echo "4: "
echo "5: "
echo "6: "
}

# main function

RC=0

#TODO check parameter
if [ $# -ne 1 ]
then
usage
exit 1 
fi

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
*)
  usage
  ;;
esac

tst_resm TINFO "Test PASS"
