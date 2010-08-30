#!/bin/bash
# Copyright (C) 2010 Freescale Semiconductor, Inc. All Rights Reserved.
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
#    @file   dvfs_mx50.sh
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
 mkfs.ext3 /var/storage.img
 modprobe g_file_storage file=/var/storage.img
 echo "now please mount the usb device on PC"
 echo "please run bwloe on pc"
 echo "mount -t ext3 /dev/sd? /mnt/flash"
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

#TODO add function test scripte here
   run_manual_test_list

RC=0
return $RC

}

# Function:     test_case_03
# Description   - Test if <TODO test function> ok
#  
test_case_03()
{
#TODO give TCID 
TCID="test_demo3_test"
#TODO give TST_COUNT
TST_COUNT=3
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

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







