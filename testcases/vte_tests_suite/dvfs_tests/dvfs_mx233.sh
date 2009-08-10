#Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
#!/bin/bash
###################################################################################################
#
#    @file   dvfs_mx25.sh
#
#    @brief  shell script template for testcase design "cpu freq test" is where to modify block.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
# 
###################################################################################################



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


run_manual_test_list()
{
 RC=1
 errcnt=0
 echo "please insert usb hard disk with partion 1 is vfat"
 echo "please connect cpu board jp17 1,2,3 pin"
 read -p "press any key when ready" key
 modprobe ehci-hcd
 sleep 20
 mkfs.vfat /dev/sda1 || return $RC
 mkdir -p /media/sda1; mount -t vfat /dev/sda1 /media/sda1 || return $RC
 bonnie\+\+ -d /media/sda1 -u 0:0 -s 10 -r 5 || return $RC
 dt of=/media/sda1/test_file bs=4k limit=128m passes=20 || return $RC
 modprobe -r ehci-hcd
 echo "please un plug the usb card"
 read -p "press any key when ready" key
 sleep 3

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

 echo "touch screen test"
 ts_test3.sh 0 || errcnt=$(expr $errcnt + 1)

 echo "key pad test"
 keypad_test.sh 2 || errcnt=$(expr $errcnt + 1)

 echo "rotary test"
 evtest /dev/input/event2 
 
 read -p "is above case ok? y/n" key
 if [ $key == 'n' ]; then
    errcnt=$(expr $errcnt + 1) 
 fi

 echo "SD test"
 echo "please insert SD card"
 read -p "press any key when ready" key
 mkfs.vfat /dev/mmcblk0p1 
 mkdir -p /mnt/mmc
 mount -t vfat /dev/mmcblk0p1 /mnt/mmc
 bonnie\+\+ -d /mnt/mmc -u 0:0 -s 10 -r 5 
 dt of=/mnt/mmc/test_file bs=4k limit=128m passes=20
 
 RC=0
 return $RC
}

run_auto_test_list()
{
   RC=1
   echo "uart test"
   cat /etc/passwd > /dev/ttyAM0 || return $RC
   echo "rtc test"
   rtc_test.sh 2 || return $RC
   echo "frambuffer test"
   dd if=/dev/urandom of=/dev/fb0 bs=1k count=150 || return $RC
   echo "tv out test"
   fbset -xres 720 -yres 576 -vxres 720 -vyres 576 || return $RC
   fbset -xres 320 -yres 240 -vxres 320 -vyres 240 || return $RC
   fbset -xres 720 -yres 480 -vxres 720 -vyres 480 || return $RC 
   fbset -xres 320 -yres 240 -vxres 320 -vyres 240 || return $RC
   echo "back light test"
   echo 0 > /sys/class/graphics/fb0/blank || return $RC
   cat /sys/class/backlight/stmp3xxx-bl/max_brightness || return $RC
   LEVEL="0 10 20 30 40 50 60 70 80 90 100 80 50 40 30 20 10 0 50" 
   for i in $LEVEL
   do
   echo $i level test  
   echo $i > /sys/class/backlight/stmp3xxx-bl/brightness || return $RC
   sleep 1
   done
   echo "NAND test"
   modprobe gpmi
   sleep 2
   ubiattach /dev/ubi_ctrl -m 1 || return $RC
   mkdir -p /mnt/ubifs
   mount -t ubifs ubi0:test /mnt/ubifs || return $RC
   bonnie -d /mnt/ubifs -u 0:0 -s 10 -r 5 || return $RC
   dt of=/mnt/ubifs/test_file bs=4k limit=128m passes=20 || return $RC
   umount /mnt/ubifs
   ubidetach /dev/ubi_ctrl -m 1 || return $RC
   modprobe -r gpmi
   echo "ALSA test"
   aplay -vv $STREAM_PATH/alsa_stream/audio44k16M.wav || return $RC
   RC=0
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
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

declare -a cpufreq_value;
cpufreq_value=(454740 64000);
count=0

while [ $count -lt 5 ]; do
  count=$(expr $count + 1)
  value=${cpufreq_value[$RANDOM%3]}
  cpufreq-set -f $value
  value_ret=$(cpufreq-info -f)
if [ $value_ret -eq "$value" ] ; then
   echo sleep...
   sleep 3
   #test list
   run_auto_test_list
else
  return $RC
fi
done

RC=0
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
echo "case can not run on nfs"
declare -a cpufreq_value;
cpufreq_value=(454740 64000);
count=0

for i in $cpufreq_value
do
   echo -n $i > /sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed
   echo sleep...
   sleep 3
   #test list
   run_manual_test_list
done


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







