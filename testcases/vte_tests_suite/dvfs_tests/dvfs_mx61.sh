#!/bin/bash -x

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

count=0
for  i in $(cpufreq-info | grep stat)
do  
freq=$(echo $i  | grep -v [^0-9])
if [ ! -z $freq ]; then
cpufreq_value[$count]=${freq}
count=$(expr $count + 1)
fi
done
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
 if [ "$key" = 'n' ]; then
    errcnt=$(expr $errcnt + 1) 
 fi
 modprobe -r g_file_storage


 read -p "is above case ok? y/n" key
 if [ "$key" = 'n' ]; then
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
   RC=0
   echo "uart test"
   cat /etc/passwd > /dev/ttymxc0 || return $RC
   echo "frambuffer test"
   dd if=/dev/urandom of=/dev/fb0 bs=1k count=150 || return 2
   echo "tv out test"
   #fbset -xres 720 -yres 576 -vxres 720 -vyres 576 || return 3
   #fbset -xres 320 -yres 240 -vxres 320 -vyres 240 || return 4
   #fbset -xres 720 -yres 480 -vxres 720 -vyres 480 || return 5
   #fbset -xres 320 -yres 240 -vxres 320 -vyres 240 || return 6
   echo 0 > /sys/class/graphics/fb0/blank || return 7
   echo "core test"
	 coremark_F4.exe  0x0 0x0 0x66 0 7 1 2000 &&  coremark_F4.exe  0x3415 0x3415 0x66 0 7 1 2000 && coremark_F4.exe 8 8 8 0 7 1 1200 || return 8
   echo "storage"
	 moprobe ahci_platform
	 storage_all.sh 1 || return 9
	 modprobe -r ahci_platform
	 echo "v4l"
	 v4l_output_testapp -B 0,0,2048,2048 -C 2 -R 3 -F $LTPROOT/testcases/bin/green_RGB24 || return 10
	 echo "vpu dec"
	 vpu_dec_test.sh 1 | return 11
	 echo "vpu_enc"
	 vpu_enc_test.sh 1 | return 12
	 echo "gpu"
	 gles_viv.sh 1
	 echo "ALSA test"
   aplay -vv $STREAM_PATH/alsa_stream/audio44k16M.wav || return 14
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


count=0

while [ $count -lt 7 ]; do
  count=$(expr $count + 1)
  value=${cpufreq_value[$RANDOM%4]}
	echo $value
  cpufreq-set -f ${value}M
  value_ret=$(cpufreq-info -f | grep $value | wc -l)
if [ $value_ret -eq 1 ] ; then
   echo sleep...
   sleep 3
   echo "TSTINFO: CPUFREQ at $value "
   #test list
   run_auto_test_list || RC=$(expr $RC + 1)
else
  return $RC
fi
done

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
cpufreq_value=(454736 392727 360000 261818);
count=0

for i in $cpufreq_value
do
   echo -n $i > /sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed
   echo sleep...
   sleep 3
   echo "TSTINFO: CPUFREQ at $i"
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

declare -a cpufreq_value;
cpufreq_value=(996M 792M 400M 167M);
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
