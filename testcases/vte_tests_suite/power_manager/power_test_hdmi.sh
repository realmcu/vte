#!/bin/bash -x

setup()
{
    RC=0
    # Total number of test cases in this file. 
    export TST_TOTAL=2

    # The TCID and TST_COUNT variables are required by the LTP 
    # command line harness APIs, these variables are not local to this program.

    # Test case identifier
    export TCID="HDMI_POWER_test"
    # Set up is initialized as test 0
    export TST_COUNT=0
    # Initialize cleanup function to execute on program exit.
    # This function will be called before the test program exits.
    trap "cleanup" 0
    if [ $(cat /proc/cmdline | grep hdmi | wc -l) -eq 1 ]; then
    echo "Already enable HDMI in boot cmdline"
    if [ $(cat /sys/devices/platform/mxc_hdmi/cable_state) = "plugout" ]; then
          echo "Not plug in HDMI cable in board"
          RC=1
          return $RC
    fi
    else
          echo "Not enable HDMI in boot cmdline"
          RC=1
          return $RC
    fi
    num=`aplay -l |grep -i "imxhdmisoc" |awk '{ print $2 }'|sed 's/://'`
    mkdir /mnt/temp
    mount -t tmpfs tmpfs /mnt/temp || RC=1
    cp /mnt/nfs/test_stream/alsa_stream/audio48k16S.wav /mnt/temp || RC=1      
    return $RC
}

cleanup()
{
    echo "CLEANUP "
    umount /mnt/temp
    RC=0
}

usage()
{
   echo "1: for device suspend resume case for no boot cores "	
   echo "2: for device suspend resume case for all cores "	
}

# Function:     test_case_01
# Description   - Test if device suspend and resume without bootcore
#  
test_case_01()
{
#TODO give TCID 
TCID="HDMI_PM_NOBOOTCORE"
#TODO give TST_COUNT
TST_COUNT=1
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
echo 0 > /sys/class/graphics/fb0/blank
echo -e "\033[9;0]" > /dev/tty0
tloops=20000
count=0
while [ $count -lt $tloops ]
do 
   ( aplay -Dplughw:$num -M /mnt/temp/audio48k16S.wav & )
   sleep 5
   echo "core test"
   i=0
   loops=10
   echo core > /sys/power/pm_test
   while [ $i -lt $loops ]
   do
      i=$(expr $i + 1)
      echo mem > /sys/power/state
      echo standby > /sys/power/state
   done
   echo none > /sys/power/pm_test
   wait
   count=$(expr $count + 1)
done
RC=0
return $RC
}

# Function:     test_case_02
# Description   - Test if device suspend and resume without bootcore
#  
test_case_02()
{
#TODO give TCID 
TCID="HDMI_PM_BOOTCORE"
#TODO give TST_COUNT
TST_COUNT=2
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
echo 0 > /sys/class/graphics/fb0/blank
echo -e "\033[9;0]" > /dev/tty0
tloops=20000
count=0
#TODO add function test scripte here
while [ $count -lt $tloops ]
do
   ( aplay -Dplughw:$num -M /mnt/temp/audio48k16S.wav & )
   sleep 5
   i=0
   loops=10
   while [ $i -lt $loops ]
   do
        i=$(expr $i + 1)
	rtc_testapp_6 -T 10
	rtc_testapp_6 -T 10 -M standby
   done
   wait
   count=$(expr $count + 1)
done
RC=0
return $RC
}

# Function:     test_case_03
# Description   - Test if device suspend and resume without bootcore
#  
test_case_03()
{
#TODO give TCID 
TCID="HDMI_PM_WAITMODE"
#TODO give TST_COUNT
TST_COUNT=3
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
echo 0 > /sys/class/graphics/fb0/blank
( aplay -Dplughw:$num -M /mnt/temp/audio48k16S.wav & )
sleep 5
echo "core test"
i=0
loops=10
echo core > /sys/power/pm_test
while [ $i -lt $loops ]
do
  i=$(expr $i + 1)
 echo standby > /sys/power/state
done
echo none > /sys/power/pm_test
wait
RC=0
return $RC
}


setup || exit 1

case "$1" in
1)
  test_case_01 || exit 2 
  ;;
2)
  test_case_02 || exit 3
  ;;
3)
  test_case_03 || exit 4
  ;;
*)
  usage
  ;;
esac

tst_resm TINFO "Test Finish"
