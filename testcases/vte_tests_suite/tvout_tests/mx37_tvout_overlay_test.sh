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
#    @file   tvout_overlay_test.sh
#
#    @brief  shell script template for testcase design "TODO" is where to modify block.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#Hake Huang/-----             20081010     N/A          Initial version
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
export TST_TOTAL=2

export TCID="setup"
export TST_COUNT=0
RC=0

trap "cleanup" 0

#TODO add setup scripts

kerv=$(uname -r | grep "2.6.24" | wc -l)
if [ ! -z $kerv ]
then
   if [ $TARGET == "37" ]
   then
    echo 4 > /sys/class/graphics/fb0/blank
    echo 4 > /sys/class/graphics/fb1/blank
    echo 4 > /sys/class/graphics/fb2/blank
   echo 1-layer-fb > /sys/class/graphics/fb0/fsl_disp_property
   echo U:720x576i-50 > /sys/class/graphics/fb1/mode
   echo 0 > /sys/class/graphics/fb1/blank
   else
     echo 4 > /sys/class/graphics/fb0/blank
     echo 4 > /sys/class/graphics/fb1/blank
     echo 4 > /sys/class/graphics/fb2/blank
    echo 0 > /sys/class/graphics/fb1/blank
    echo U:720x480i-60 > /sys/class/graphics/fb1/mode
   fi
else
   echo 0 > /sys/class/graphics/fb1/blank
   echo U:720x480i-60 > /sys/class/graphics/fb1/mode
#echo U:720x576i-50 > /sys/class/graphics/fb1/mode
fi

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


# Function:     cleanup
#
# Description   - remove temporary files and directories.
#
# Return        - zero on success#               - non zero on failure. return value from commands ($RC)
cleanup()
{
RC=0

echo 1 > /sys/class/graphics/fb1/blank
return $RC
}


# Function:     test_case_01
# Description   - Test if NTSC overlay test ok
#  
test_case_01()
{
#TODO give TCID 
TCID="overlay_NTSC_test"
#TODO give TST_COUNT
TST_COUNT=1
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

echo U:720x480i-60 > /sys/class/graphics/fb1/mode
#
# V4L2 Output Tests
#

# SDC input size test cases
for SIZE in 32 40 48 64 80 96 112 128 144 160 176 192 208 224 240 ; do
/unit_tests/mxc_v4l2_output.out -iw $SIZE -ih $SIZE -ow 720 -oh 480 -f YU12 -d $DISPLAY -r 0 || exit $RC
done

# SDC output rotation test cases
for ROT in 0 1 2 3 4 5 6 7; do
  /unit_tests/mxc_v4l2_output.out -iw 352 -ih 288 -ow 720 -oh 480 -f YU12 -d $DISPLAY -r $ROT || exit $RC
done

# crop test
for OL in 10 15 25 30 50 100 ; do
for OT in 10 15 25 30 50 100 ; do
/unit_tests/mxc_v4l2_output.out -iw 640 -ih 480 -ol $OL -ot $OT -ow 320 -oh 240 -f YU12 -d $DISPLAY -r 0 || exit $RC
done
done
# SDC max input size test case

/unit_tests/mxc_v4l2_output.out -iw 480 -ih 640 -ow 720 -oh 480 -f YU12 -d $DISPLAY -fr 60 || exit $RC

/unit_tests/mxc_v4l2_output.out -iw 720 -ih 512 -ow 720 -oh 480 -f YU12 -d $DISPLAY -fr 60 || exit $RC

RC=0

return $RC

}

# Function:     test_case_02
# Description   - Test if PAL overlay test ok
#  
test_case_02()
{
#TODO give TCID 
TCID="overlay_PAL_test"
#TODO give TST_COUNT
TST_COUNT=2
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

echo U:720x576i-50 > /sys/class/graphics/fb1/mode

# SDC input size test cases
for SIZE in 32 40 48 64 80 96 112 128 144 160 176 192 208 224 240; do
	/unit_tests/mxc_v4l2_output.out -iw $SIZE -ih $SIZE -ow 720 -oh 576 -f YU12 -d $DISPLAY -r 0 || exit $RC
done

# SDC output rotation test cases
for ROT in 0 1 2 3 4 5 6 7; do
  /unit_tests/mxc_v4l2_output.out -iw 352 -ih 288 -ow 720 -oh 576 -f YU12 -d $DISPLAY -r $ROT || exit $RC
done

# SDC max input size test case

/unit_tests/mxc_v4l2_output.out -iw 480 -ih 640 -ow 720 -oh 576 -f YU12 -d $DISPLAY -fr 60 || exit $RC

/unit_tests/mxc_v4l2_output.out -iw 720 -ih 512 -ow 720 -oh 576 -f YU12 -d $DISPLAY -fr 60 || exit $RC

RC=0

return $RC

}

# Function:     test_case_03
# Description   - Test if PAL overlay test ok
#  
test_case_03()
{
#TODO give TCID 
TCID="overlay_PAL_test_Simple"
#TODO give TST_COUNT
TST_COUNT=3
RC=0

echo U:720x576i-50 > /sys/class/graphics/fb1/mode

#this will last for 200s
/unit_tests/mxc_v4l2_output.out -iw 480 -ih 640 -ow 720 -oh 576 -f YU12 -d $DISPLAY -fr 60 -l 12000 || RC=2 &

times=100

while [ $times -gt 0 ]
do

echo "times $times"

cleanup 

sleep 1

setup

sleep 1

times=$(expr $times - 1)

done

return $RC
}


# main function
DISPLAY=3
RC=0

#TODO check parameter
if [ $# -ne 1 ]
then
echo "usage $0 <1/2/3>"
exit 1 
fi

check_platform

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
*)
*#TODO check parameter
  echo "usage $0 <1/2/3>"
  ;;
esac

tst_resm TINFO "Test PASS"







