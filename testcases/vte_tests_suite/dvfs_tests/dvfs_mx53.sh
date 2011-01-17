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
#    @file   dvfs_mx53.sh
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
echo 1 > /sys/devices/platform/mxc_dvfs_core.0/enable
echo 1 > /sys/devices/platform/busfreq.0/enable
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
echo 0 > /sys/devices/platform/mxc_dvfs_core.0/enable
echo 1 > /sys/devices/platform/busfreq.0/enable

return $RC
}


run_manual_test_list()
{
 RC=1
 errcnt=0
 echo "  "
 echo "  "
 echo "usb otg storage test"
 echo "please connect make sure the jp17 usb pin is not power"
 echo "please connect usb cable to pc host"
 read -p "press any key when ready" key
 echo "on PC host please run:"
 echo " modprobe usb-storage"
 echo "mkdir /mnt/flash"
 read -p "press any key when ready" key
 dd if=/dev/zero of=/var/storage.img bs=1M count=64
 mkfs.vfat /var/storage.img
 modprobe g_file_storage file=/var/storage.img
 echo "now please mount the usb device on PC"
 echo "please run bwloe on pc"
 echo "mount /dev/sd? /mnt/flash"
 echo "bonnie\+\+ -d /mnt/flash -u 0:0 -s 10 -r 5"
 echo "dt of=/mnt/flash/test_file bs=4k limit=128m passes=20"

 read -p "is pc run above case ok? y/n" key
 if [ $key == 'n' ]; then
    errcnt=$(expr $errcnt + 1) 
 fi
 modprobe -r g_file_storage


 
 RC=0
 return $RC
}

run_auto_test_list()
{
   RC=0
   echo "uart test"
   cat /etc/passwd > /dev/ttymxc0 || return $RC
   echo "rtc test"
   rtc_test.sh 2 || return 1
   echo "frambuffer test"
   dd if=/dev/urandom of=/dev/fb0 bs=1k count=150 || return 2
   echo "ALSA test"
   aplay -vv $STREAM_PATH/alsa_stream/audio44k16M.wav || return 14
	 echo "USB Host test"
   mkfs.vfat /dev/sda1 || return 15
   mkdir -p /media/sda1; mount -t vfat /dev/sda1 /media/sda1 || return $16
	 bonnie\+\+ -d /media/sda1 -u 0:0 -s 10 -r 5 || return $17
	 dt of=/media/sda1/test_file bs=4k limit=128m passes=20 || return $18
	 echo "SD test"
	 mkdir -p /mnt/mmc
	 mkfs.vfat /dev/mmcblk0p1
	 mount /dev/mmcblk0p1 /mnt/mmc
	 bonnie\+\+ -d /mnt/mmc -u 0:0 -s 10 -r 5 
	 dt of=/mnt/mmc/test_file bs=4k limit=128m passes=20
	 return $RC
}


# Function:     test_case_01
# Description   - Test if <CPU freq> ok
#  
test_case_01()
{
#TODO give TCID 
TCID="test_CPUFreq_stress"
#TODO give TST_COUNT
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here


#test list
run_auto_test_list || RC=1

return $RC

}

# Function:     test_case_02
# Description   - Test if PMIC DVFS test ok
#  
test_case_02()
{
#TODO give TCID 
TCID="PMIC_DVFS_manual_test"
#TODO give TST_COUNT
TST_COUNT=2
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#echo enabled > /sys/devices/platform/fsl-usb2-otg/power/wakeup
#echo enabled > /sys/devices/platform/fsl-usb2-udc/power/wakeup
low_power_usb.sh

#TODO add function test scripte here
   run_manual_test_list

RC=0
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
# Description   - Test if <TODO test function> ok
#  
test_case_04()
{
#TODO give TCID 
TCID="test_demo4_test"
#TODO give TST_COUNT
TST_COUNT=4
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

return $RC

}

# Function:     test_case_05
# Description   - Test if <TODO test function> ok
#  
test_case_05()
{
#TODO give TCID 
TCID="test_demo5_test"
#TODO give TST_COUNT
TST_COUNT=5
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

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
*)
  usage
  ;;
esac

tst_resm TINFO "Test PASS"







