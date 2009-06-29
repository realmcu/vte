#Copyright 2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
#    @file   mxc_v4l2_output.sh
#
#    @brief  shell script for v4l2 output.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#Hake Huang/-----             20090217        N/A          Initial version
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
echo -e "\033[9;0]" > /dev/tty0
#setup the fb on
echo 0 > /sys/class/graphics/fb0/blank

check_platform

if [ $TARGET = 25 ]
then
DISPLAY=0
else
DISPLAY=3
fi

TSTCMD=/unit_tests/mxc_v4l2_output.out

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


# Function:     test_case_01
# Description   - Test if output size ok
#  
test_case_01()
{
#TODO give TCID 
TCID="test_size_output"
#TODO give TST_COUNT
TST_COUNT=1
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
RESLIST="800x480 640x480 352x288 320x240 176x144 320x240 352x288 640x480 800x480"

echo "pure size change test"
for i in $RESLIST
do
   OWD=$(echo $i | sed "s/x/ /g" | awk '{print $1}')
   OHT=$(echo $i | sed "s/x/ /g" | awk '{print $2}')
   ${TSTCMD}  -iw 128 -ih 128 -ow $OWD -oh $OHT -d $DISPLAY -r 0 || return $RC
   sleep 1
done

OFFSET="10 11 12 13 14 15 16 17 18 19 20 30 40 50 60 70 80 90 100"
echo "change size with offset"
for i in $OFFSET
do
 echo "now offset is $i"
 for j in $RESLIST
 do
   OWD=$(echo $j | sed "s/x/ /g" | awk '{print $1}')
   OHT=$(echo $j | sed "s/x/ /g" | awk '{print $2}')
   ${TSTCMD}  -iw 128 -ih 128 -ow $OWD -oh $OHT -ol $i -oh $i -d $DISPLAY -r 0 || return $RC
   sleep 1
 done 
done



RC=0
return $RC

}

# Function:     test_case_02
# Description   - Test if input size ok
#  
test_case_02()
{
#TODO give TCID 
TCID="input_test"
#TODO give TST_COUNT
TST_COUNT=2
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test script here
INSIZE="320x240 640x480 352x288 176x144 320x240 352x288 640x512"
RESLIST="800x480 640x480 352x288 320x240 176x144 320x240 352x288 640x480 800x480"
echo "pure size change test"
for i in $INSIZE
do
    echo "input size $i"
   IWD=$(echo $i | sed "s/x/ /g" | awk '{print $1}')
   IHT=$(echo $i | sed "s/x/ /g" | awk '{print $2}')
   for j in $RESLIST
   do
    echo "output size $j"
   OWD=$(echo $j | sed "s/x/ /g" | awk '{print $1}')
   OHT=$(echo $j | sed "s/x/ /g" | awk '{print $2}')
   ${TSTCMD}  -iw $IWD -ih $IHT -ow $OWD -oh $OHT -d $DISPLAY -r 0 || return $RC
   sleep 1
   done
done

echo "MAX input test"
if [ $TARGET = 25 ]
then
${TSTCMD} -iw 640 -ih 512 -ow 240 -oh 320 -d $DISPLAY -fr 60 -r 0 || return $RC
else
${TSTCMD} -iw 480 -ih 640 -ow 240 -oh 320 -d $DISPLAY -fr 60 -r 0 || return $RC
${TSTCMD} -iw 720 -ih 512 -ow 240 -oh 184 -d $DISPLAY -fr 60 -r 0 || return $RC
fi
RC=0
return $RC
}

# Function:     test_case_03
# Description   - Test if rotation ok
#  
test_case_03()
{
#TODO give TCID 
TCID="rotation"
#TODO give TST_COUNT
TST_COUNT=3
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test script here
ROTATION="0 1 2 3 4 5 6 7"
INSIZE="320x240 640x480 352x288 176x144 320x240 352x288 640x512"
RESLIST="800x480 640x480 352x288 320x240 176x144 320x240 352x288 640x480 800x480"
echo "rotation change test"
for k in $ROTATION
do
 echo "rotation at mode $k"
 for i in $INSIZE
 do
   echo "input size is $i"
   IWD=$(echo $i | sed "s/x/ /g" | awk '{print $1}')
   IHT=$(echo $i | sed "s/x/ /g" | awk '{print $2}')
   for j in $RESLIST
   do
     echo "out size is $j"
   OWD=$(echo $j | sed "s/x/ /g" | awk '{print $1}')
   OHT=$(echo $j | sed "s/x/ /g" | awk '{print $2}')
   ${TSTCMD}  -iw $IWD -ih $IHT -ow $OWD -oh $OHT -d $DISPLAY -r $k || return $RC
   sleep 1
   done
 done
done

RC=0
return $RC

}

# Function:     test_case_04
# Description   - Test if rotation with offset ok
#  
test_case_04()
{
#TODO give TCID 
TCID="rotation_offset_test"
#TODO give TST_COUNT
TST_COUNT=4
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

ROTATION="0 1 2 3 4 5 6 7"
#RESLIST="800x480 640x480 352x288 320x240 176x144 320x240 352x288 640x480 800x480"
RESLIST="176x144 800x480"
OFFSET="10 16 100"
echo "rotation with offset"
for k in $ROTATION
do
 echo "rotation is $k"
 for i in $OFFSET
 do
  echo "now offset is $i"
  for j in $RESLIST
  do
   OWD=$(echo $j | sed "s/x/ /g" | awk '{print $1}')
   OHT=$(echo $j | sed "s/x/ /g" | awk '{print $2}')
   ${TSTCMD}  -iw 128 -ih 128 -ow $OWD -oh $OHT -ol $i -oh $i -d $DISPLAY -r $k || return $RC
   sleep 1
  done 
 done
done
#TODO add function test scripte here

RC=0
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

check_platform()
{
 PLATFORM="25 31 35 37 51"
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
echo "$0 [case ID]"
echo "1: output with size change"
echo "2: output with input size change"
echo "3: rotation test"
echo "4: rotation and offset test"
echo "5: setup test"
}

# main function

RC=0

#TODO check parameter
if [ $# -ne 1 ]
then
usage
exit 1 
fi

TARGET=
DISPLAY=
TSTCMD=

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







