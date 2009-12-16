#Copyright 2008-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
#!/bin/sh
###################################################################################################
#
#    @file   tvout_test_usercase.sh
#
#    @brief  shell script to test the TVout user case function block.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#Hake.Huang/-----             08/01/2008     N/A          Initial version
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
export TST_TOTAL=6

export TCID="setup"
export TST_COUNT=0
RC=0

if [ -e $LTPROOT ]
then
export LTPSET=0
else
export LTPSET=1
fi

trap "cleanup" 0

#enable TVout
if [ $TARGET == "37"  ]
then
  kerv=$(uname -r | grep 2.6.24|wc -l)
  if [ ! -z $kerv ]
  then
  echo U:720x480i-60 > /sys/class/graphics/fb1/mode
  echo 0 > /sys/class/graphics/fb1/blank
  else
   echo 0 > /sys/class/graphics/fb1/blank
   echo U:720x480i-60 > /sys/class/graphics/fb1/mode
  fi
else
 echo U:720x480i-60 > /sys/class/graphics/fb1/mode
 echo 0 > /sys/class/graphics/fb1/blank
fi
#echo U:720x576i-50 > /sys/class/graphics/fb1/mode

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

#echo U:480x640p-67 > /sys/class/graphics/fb0/mode

echo 1 > /sys/class/graphics/fb1/blank
return $RC
}

# Function:     test_tvout_usercase_01
# Description   - Test the TVout module functionality
#                 TVout basic function test
# TYPE:          auto test
test_tvout_usercase_01()
{
TCID="test_tvout_usercase_01"
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test #1: tvout_usercase 01"

#lcd_testapp_power -F /dev/fb1
/unit_tests/mxc_v4l2_output.out -iw 720 -ih 480 -d 5 -f YU12 

RC=$?

return $RC
}


# Function:     test_tvout_usercase_02
# Description   - Test the TVout module functionality
#                 TVout pal/ntsc test
# TYPE:          auto manual 
test_tvout_usercase_02()
{
TCID="test_tvout_usercase_02"
TST_COUNT=1
RC=0

echo U:720x480i-60 > /sys/class/graphics/fb1/mode
/unit_tests/mxc_v4l2_output.out -iw 128 -ih 128 -ow 720 -oh 480 -d 5 -r 0 -f YU12 

read -p "Does TV out function? y/n" rc

if [ $rc = "n" ]
then
RC=$TST_COUNT
return $RC
fi

echo U:720x576i-50 > /sys/class/graphics/fb1/mode
/unit_tests/mxc_v4l2_output.out -iw 128 -ih 128 -ow 720 -oh 576 -d 5 -r 0 -f YU12 

read -p "Does TV out function? y/n" rc

if [ $rc = "n" ]
then
RC=$TST_COUNT
return $RC
fi

return $RC
}


# Function:     test_tvout_usercase_03
# Description   - Test 
#                 TVout unit tests
# TYPE:          auto 
test_tvout_usercase_03()
{
TCID="test_tvout_usercase_03"
TST_COUNT=1
RC=0


# SDC input size test cases
for SIZE in 32 40 48 64 80 96 112 128 144 160 176 192 208 224 240 
do
 /unit_tests/mxc_v4l2_output.out -iw $SIZE -ih $SIZE -ow 720 -oh 480 -d 5 -r 0 -f YU12 || RC=1
 sleep 1
done

if [ $RC = 1 ]; then
 echo "input size test fail"
 return $RC
fi

# SDC output rotation test cases
for ROT in 0 1 2 3 4 5 6 7
do
 /unit_tests/mxc_v4l2_output.out -iw 352 -ih 288 -ow 720 -oh 480 -d 5 -r $ROT -f YU12 || RC=1 
 sleep 1
done

if [ $RC = 1 ]; then
 echo "rotation test fail"
 return $RC
fi

# SDC crop test
for OL in 10 16 20 720
do
 for OT in 10 16 20 480
 do
 /unit_tests/mxc_v4l2_output.out -iw 320 -ih 240 -ot $OT -ol $OL -ow 320  -oh 240 -d 5 -r $ROT -f YU12 || RC=1
 sleep 1
 done
done

if [ $RC = 1 ]; then
 echo "crop test fail"
 return $RC
fi
# SDC max input size test case
/unit_tests/mxc_v4l2_output.out -iw 480 -ih 640 -ow 720 -oh 480 -d 5 -fr 60 -f YU12 
/unit_tests/mxc_v4l2_output.out -iw 720 -ih 512 -ow 720 -oh 480 -d 5 -fr 60 -f YU12 

RC=$?
return $RC
}

# Function:     test_tvout_usercase_04
# Description   - Test 
#                 TVout function test
# TYPE:          auto manual 
test_tvout_usercase_04()
{
TCID="test_tvout_usercase_04"
TST_COUNT=1
RC=0

/unit_tests/mxc_v4l2_output.out -iw 128 -ih 128 -ow 720 -oh 480 -d 5 -r 0 -f YU12 

read -p "Does TV out function? y/n" rc

if [ $rc = "n" ]
then
RC=$TST_COUNT
return $RC
fi

RC=$?
return $RC
}

# Function:     test_tvout_usercase_05
# Description   - Test 
#                 TVout module basic test
# TYPE:          auto  
test_tvout_usercase_05()
{
TCID="test_tvout_usercase_05"
TST_COUNT=1
RC=0
 
#draw a picture test auto for 5 times
#lcd_testapp -T 2 -B /dev/fb1 -D 16 -X 5 -D 5

return $RC
}

# Function:     test_tvout_usercase_06
# Description   - Test
#                 TVout sys hardware control 
# TYPE:          auto
test_tvout_usercase_06()
{
TCID="test_tvout_usercase_06"
TST_COUNT=1
RC=0

echo U:720x480i-60 > /sys/class/graphics/fb1/mode

MODE=$(cat /sys/class/graphics/fb1/mode)

if [ $MODE != "U:720x480i-60" ]
then 
RC=1
return $TST_COUNT
fi

echo U:720x576i-50 > /sys/class/graphics/fb1/mode

MODE=$(cat /sys/class/graphics/fb1/mode)
if [ $MODE != "U:720x576i-50" ]
then 
RC=1
return $TST_COUNT
fi

return $RC
}

# Function:     test_tvout_usercase_07
# Description   - Test 
#                 TVout module power resume
# TYPE:          semi-auto  
test_tvout_usercase_07()
{
TCID="test_tvout_power_resume"
TST_COUNT=1
RC=1
 

/unit_tests/mxc_v4l2_output.out -iw 128 -ih 128 -ow 720 -oh 480 -d 5 -r 0 -f YU12 

echo  "***********************************"
echo "************************************"
echo "please press power key to wake up system"
echo  "***********************************"
echo "************************************"
sleep 1
echo standby > /sys/power/state

/unit_tests/mxc_v4l2_output.out -iw 128 -ih 128 -ow 720 -oh 480 -d 5 -r 0 -f YU12 
read -p "is tvout still dispaly? y/n" rt
if [ $rt == "y" ]
then
RC=0
return $RC
fi
RC=1
return $RC
}

PLATFORM="31 35 37 51 25"

TARGET=

check_platform()
{
#  CPU_REV=$(cat /proc/cpuinfo | grep "Revision")
  CPU_REV=$(platfm.sh)
  for i in $PLATFORM
  do
   find=$(echo $CPU_REV | grep $i | wc -l )
   if [ $find -ne 0 ]
   then
   TARGET=$i
   fi
  done
}

usage()
{
  echo "usage $0 <1/2/3/4/5/6/7>"
  echo "1: power test"
  echo "2: NTSC & PAL test"
  echo "3: Resize & rotation test"
  echo "4: TVout basice function test"
  echo "5: module available test"
  echo "6: tvout setting test"
  echo "7: power state test"
}


RC=0

check_platform

setup || exit $RC

if [ $# -ne 1 ]
then
usage
exit 1 
fi

case "$1" in
1)
  test_tvout_usercase_01 || exit $RC 
  ;;
2)
  test_tvout_usercase_02 || exit $RC
  ;;
3)
  test_tvout_usercase_03 || exit $RC
  ;;
4)
  test_tvout_usercase_04 || exit $RC
  ;;
5)
   test_tvout_usercase_05 || exit $RC
   ;;
6)
   test_tvout_usercase_06 || exit $RC
   ;;
7)
   test_tvout_usercase_07 || exit $RC
   ;;
*)
  usage
  exit 1
  ;;
esac

tst_resm TINFO "test PASS"
