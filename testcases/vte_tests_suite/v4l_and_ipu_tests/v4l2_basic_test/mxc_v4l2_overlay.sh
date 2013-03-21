#!/bin/sh
#Copyright (C) 2009-2010,2012,2013 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
################################################################################
#
#    @file   mxc_v4l2_overlay.sh
#
#    @brief  shell script for v4l2 capture camera to overlay.
#
################################################################################
#Revision History:
#Author                          Date       Description of Changes
#-------------------------   ------------   -----------------------
#Hake Huang/-----             20090217      Initial version
#Andy Tian                    20120314      Update the support mode of fps 15
#                                           for ov5642
#Andy Tian                    20130321      Get the accurate support modes for
#                                           case 05 and 06
################################################################################



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
OV=0

trap "cleanup" 0

#TODO add setup scripts
echo -e "\033[9;0]" > /dev/tty0
#setup the fb on
echo 0 > /sys/class/graphics/fb0/blank


TSTCMD=/unit_tests/mxc_v4l2_overlay.out

v4l_module.sh setup
OV=$?

RESSIZE=$(fbset | grep "mode \"" |  sed "s/\"//g" | sed "s/-/ /" | awk {'print $2'})

platfm.sh
TARGET=$?

if [ "$TARGET" = "25" ]
then
RESSIZE=""
fi

if [ "$TARGET" = "31" ]
then
RESSIZE="240x320"
fi

if [ "$TARGET" = "35" ]
then
RESSIZE=""
fi

if [ "$TARGET" = "37" ]
then
RESSIZE="240x320"
fi

if [ "$TARGET" = "51" ]
then
RESSIZE="240x320 720x480"
fi

if [ "$TARGET" = "53" ]
then
RESSIZE="240x320 720x480 1280x720"
fi

if [ "$TARGET" = "61" ] || [ "$TARGET" = "63" ]
then
RESSIZE="240x320 720x480 1024x768 1280x720 1920x1280"
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
RC=0

#TODO add cleanup code here

v4l_module.sh cleanup
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
RESLIST="320x240 176x144 320x240"" "${RESSIZE}
echo "pure size change test"

FBX=$(fbset | grep geometry | awk '{print $2}')
FBY=$(fbset | grep geometry | awk '{print $3}')

for i in $RESLIST
do
   OWD=$(echo $i | sed "s/x/ /g" | awk '{print $1}')
   OHT=$(echo $i | sed "s/x/ /g" | awk '{print $2}')
	 if [ $OWD -lt $FBX  ] && [ $OHT -lt $FBY ]; then
   ${TSTCMD}  -iw 640 -ih 480 -ow $OWD -oh $OHT -r 0 -t 5|| return $RC
   sleep 1
	 fi
done

OFFSET="10 11 12 13 14 15 16 17 18 19 20 30 40 50"
echo "change size with offset"
for i in $OFFSET
do
 echo "now offset is $i"
 for j in $RESLIST
 do
   OWD=$(echo $j | sed "s/x/ /g" | awk '{print $1}')
   OHT=$(echo $j | sed "s/x/ /g" | awk '{print $2}')
	 WL=$(expr $FBX - $i)
	 HL=$(expr $FBY - $i)
	 if [ $OWD -lt $WL  ] && [ $OHT -lt $HL ]; then
    echo "fore ground"
    ${TSTCMD}  -iw 640 -ih 480 -ow $OWD -oh $OHT -ol $i -ot $i -r 0  -fg -t 5|| return $RC
    echo "frame buffer"
    ${TSTCMD}  -iw 640 -ih 480 -ow $OWD -oh $OHT -ol $i -ot $i -r 0 -t 5|| return $RC
    sleep 1
   fi
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
INSIZE="320x240 176x144 320x240"
RESLIST="320x240 176x144 320x240"" "${RESSIZE}
echo "pure size change test"
FBX=$(fbset | grep geometry | awk '{print $2}')
FBY=$(fbset | grep geometry | awk '{print $3}')
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
	 if [ $OWD -lt $FBX  ] && [ $OHT -lt $FBY ]; then
		 echo "foreground"
		 ${TSTCMD}  -iw $IWD -ih $IHT -ow $OWD -oh $OHT -r 0 -fg -t 5 || return $RC
		 echo "back ground"
		 ${TSTCMD}  -iw $IWD -ih $IHT -ow $OWD -oh $OHT -r 0 -t 5 || return $RC
		 sleep 1
	 fi
   done
done

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
FBX=$(fbset | grep geometry | awk '{print $2}')
FBY=$(fbset | grep geometry | awk '{print $3}')
ROTATION="0 1 2 3 4 5 6 7"
INSIZE="320x240 176x144 320x240"
RESLIST="320x240 176x144 320x240"" "${RESSIZE}
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
	 if [ $OWD -lt $FBX  ] && [ $OHT -lt $FBY ]; then
   	echo "foreground"
		${TSTCMD}  -iw $IWD -ih $IHT -ow $OWD -oh $OHT -r $k -fg -t 5 || return $RC
		echo "frame buffer"
		${TSTCMD}  -iw $IWD -ih $IHT -ow $OWD -oh $OHT -r $k -t 5|| return $RC
   	sleep 1
	 fi
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

FBX=$(fbset | grep geometry | awk '{print $2}')
FBY=$(fbset | grep geometry | awk '{print $3}')
ROTATION="0 1 2 3 4 5 6 7"
RESLIST="320x240 176x144 320x240"" "${RESSIZE}
OFFSET="10 11 12 13 14 15 16 17 18 19 20 30 40 50"
echo "rotation with offset"
for k in $ROTATION
do
 echo "rotation is $k"
 for i in $OFFSET
 do
  echo "now offset input & output is $i"
  for j in $RESLIST
  do
   OWD=$(echo $j | sed "s/x/ /g" | awk '{print $1}')
   OHT=$(echo $j | sed "s/x/ /g" | awk '{print $2}')
   OW=$(expr $OWD - $i)
   OH=$(expr $OHT - $i)
	 WL=$(expr $FBX - $i)
	 HL=$(expr $FBY - $i)
	 if [ $OW -lt $WL  ] && [ $OH -lt $HL ]; then
   	${TSTCMD}  -iw 128 -ih 128 -it $i -il $i -ow $OW -oh $OH -ol $i -ot $i -r $k -t 5|| return $RC
   	sleep 1
	 fi
  done
 done
done
#TODO add function test scripte here

RC=0
return $RC
}

# Function:     test_case_05
# Description   - Test if capture mode ok
#
test_case_05()
{
#TODO give TCID
TCID="capture_mode"
#TODO give TST_COUNT
TST_COUNT=5
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
FBX=$(fbset | grep geometry | awk '{print $2}')
FBY=$(fbset | grep geometry | awk '{print $3}')

#TODO add function test scripte here
ROTATION="0 1 2 3 4 5 6 7"
OFFSET="10 15 80 100"
RESLIST="320x240 176x144 320x240"" "${RESSIZE}
echo "rotation with offset"
for k in $ROTATION
do
 echo "rotation is $k"
 for i in $OFFSET
 do
  echo "now offset input & output is $i"
  for j in $RESLIST
  do
   OWD=$(echo $j | sed "s/x/ /g" | awk '{print $1}')
   OHT=$(echo $j | sed "s/x/ /g" | awk '{print $2}')
	 WL=$(expr $FBX - $i)
	 HL=$(expr $FBY - $i)
	 if [ $OWD -lt $WL  ] && [ $OHT -lt $HL ]; then
   	echo "frame rate 15"
	for mode in $modes_15fps; do
		${TSTCMD}  -iw 128 -ih 128 -it $i -il $i -ow $OWD -oh $OHT -ol $i -ot $i -r $k -m $mode -t 3 -fr 15 || return $RC
	done
   	 echo "farme rate 30"
	for mode in $modes_30fps; do
		${TSTCMD}  -iw 128 -ih 128 -it $i -il $i -ow $OWD -oh $OHT -ol $i -ot $i -r $k -m $mode -t 3 -fr 15 || return $RC
	done
        fi
  done
 done
done
RC=0
return $RC

}
# Function:     test_case_06
# Description   - Test if gamma setting ok
#
test_case_06()
{
#TODO give TCID
TCID="gamma setting"
#TODO give TST_COUNT
TST_COUNT=6
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

FBX=$(fbset | grep geometry | awk '{print $2}')
FBY=$(fbset | grep geometry | awk '{print $3}')
GAMMA="0 1 2 3 4 5"
OFFSET="10 15 80 100"
RESLIST="320x240 176x144 320x240"" "${RESSIZE}
echo "rotation with offset"
for k in $GAMMA
do
 echo "rotation is $k"
 for i in $OFFSET
 do
  echo "now offset input & output is $i"
  for j in $RESLIST
  do
   echo "frame rate 15"
   OWD=$(echo $j | sed "s/x/ /g" | awk '{print $1}')
   OHT=$(echo $j | sed "s/x/ /g" | awk '{print $2}')
	 WL=$(expr $FBX - $i)
	 HL=$(expr $FBY - $i)
	 if [ $OWD -lt $WL  ] && [ $OHT -lt $HL ]; then
   	echo "frame rate 15"
	for mode in $modes_15fps; do
		${TSTCMD}  -iw 128 -ih 128 -it $i -il $i -ow $OWD -oh $OHT -ol $i -ot $i -r $k -m $mode -t 3 -fr 15 || return $RC
	done
   	 echo "farme rate 30"
	for mode in $modes_30fps; do
		${TSTCMD}  -iw 128 -ih 128 -it $i -il $i -ow $OWD -oh $OHT -ol $i -ot $i -r $k -m $mode -t 3 -fr 15 || return $RC
	done
	fi
  done
 done
done
RC=0
return $RC

}

TARGET=
TSTCMD=
RESSIZE=

setup || exit $RC

if [ $1 -gt 4 ]; then
	if [ "$CAMERA" = "ov5640_mipi" ]; then
		modes_15fps=`v4l_modes -D /dev/video1 -r 15`
		modes_30fps=`v4l_modes -D /dev/video1`
	else
		modes_15fps=`v4l_modes -r 15`
		modes_30fps=`v4l_modes`
	fi
fi


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
