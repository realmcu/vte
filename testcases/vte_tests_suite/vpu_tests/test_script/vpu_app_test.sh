#Copyright (C) 2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
#    @file   vpu_app_test.sh
#
#    @brief  shell script for testcase design for VPU app
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#<Hake Huang>/-----             <2009/02/12>     N/A          Initial version
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
export TST_TOTAL=8

export TCID="setup"
export TST_COUNT=0
RC=1

trap "cleanup" 0

#TODO add setup scripts
if [ -e /dev/mxc_vpu ]
then
RC=0
fi

if [ $TARGET = "37" ]
then
if [ -e /sys/class/regulator/regulator_1_SW2/uV ]
then
echo 1100 > /sys/class/regulator/regulator_1_SW2/uV
fi
fi

v4l_module.sh setup
#setup the fb on
echo 0 > /sys/class/graphics/fb0/blank


sleep 1
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
if [ $TARGET = "37" ]
then
if [ -e /sys/class/regulator/regulator_1_SW2/uV ]
then
echo 1200 > /sys/class/regulator/regulator_1_SW2/uV
fi
fi

v4l_module.sh cleanup
cd $LTPROOT
return $RC
}

check_platform()
{
LOCAL=0
if [ $LOCAL -eq 1 ]; then
  PLATFORM="31 35 37 51"
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
else
  platfm.sh
  TARGET=$?
fi
}

# Function:     test_dec_exec
# Description   - Test app options for defined format and srcfile
#  
test_dec_exec()
{
RC=1

#check local file decode
#srcfile=${STREAM_PATH}/video/mpeg2_720x576.mpg

echo "pure decode 1 frame to LCD"
${TSTCMD} -D "-i $srcfile -f $FORMAT -c 1" || return $RC
echo "pure decode i frame to file"
${TSTCMD} -D "-i $srcfile -f $FORMAT -c 1 -o /tmp/_out_.yuv" || return $RC
SIZE=$(ls -s /tmp/_out_.yuv | awk '{print $1}') 
rm -rf /tmp/_out_.yuv
if [ $SIZE -eq 0 ]
then
return $RC
fi
echo "decode 1 frame to lcd with deblock"
${TSTCMD} -D "-i $srcfile -f $FORMAT -c 1 -d 1" || return $RC
if [ $FORMAT == 0 ] || [ $FORMAT == 2 ]; then
 echo "decode 1 frame to lcd with debering"
 ${TSTCMD} -D "-i $srcfile -f $FORMAT -c 1 -e 1" || return $RC
 echo "decode 1 frame to lcd with debering & deblock"
 ${TSTCMD} -D "-i $srcfile -f $FORMAT -c 1 -e 1 -d 1" || return $RC
fi
echo "decode with ROTATION"

for l in $CHINT
do
echo "******************"
echo "following is chroma interleave mode $l"
 for j in $MIRROR
 do
   echo "******************"
   echo "following is mirror mode $j"
   for i in $ROTATION
   do
     echo "#################"
     echo "with ROTATION mode $i"
     for k in $UIPU
     do
      echo "-----------------"
      echo "IPU is used $k"
      echo "rotation"
      ${TSTCMD} -D "-i $srcfile -f $FORMAT -c 1 -r $i -m $j -u $k -t $l" || return $RC
      if [ $FORMAT == 0 ] || [ $FORMAT == 2 ]; then
       echo "rotation with deblocking"
       ${TSTCMD} -D "-i $srcfile -f $FORMAT -c 1 -d 1 -r $i -m $j -u $k -t $l" || return $RC
       echo "rotation with debering"
       ${TSTCMD} -D "-i $srcfile -f $FORMAT -c 1 -e 1 -r $i -m $j -u $k -t $l" || return $RC
       echo "rotation with debering & deblock"
      ${TSTCMD} -D "-i $srcfile -f $FORMAT -c 1 -e 1 -d 1 -r $i -m $j -u $k -t $l" || return $RC
      fi
      sleep 1
      echo "now change to different size"
      for n in $SIZELIST
      do
       OWD=$(echo $n | sed "s/x/ /g" | awk '{print $1}')
       OHT=$(echo $n | sed "s/x/ /g" | awk '{print $2}')
        echo "output width is $OWD height is $OHT"       
       ${TSTCMD} -D "-i $srcfile -f $FORMAT -c 1 -e 1 -d 1 -r $i -m $j -u $k -t $l -w $OWD -h $OHT" || return $RC
	sleep 1
      done
      #echo "test prescan in 7 frames"
      #${TSTCMD} -D "-i $srcfile -f $FORMAT -c 7 -e 1 -d 1 -r $i -m $j -u $k -t $l -s 1" || return $RC
      sleep 1
    done
   done
 done
done

if [ $TARGET = "51" ]
then
 echo "test camera to lcd loop back mode for 30 frames"
# for i in $SIZELIST
# do
#   OWD=$(echo $i | sed "s/x/ /g" | awk '{print $1}')
#   OHT=$(echo $i | sed "s/x/ /g" | awk '{print $2}')
#   ${TSTCMD} -L "-f $FORMAT -w $OWD -h $OHT -t 1 -c 30" || return $RC
# done 
fi	

echo "$srcfile test dec app PASS"
RC=0

return $RC
}

# Function: test_enc_exec
# Description: -Test encode app for given file
test_enc_exec()
{
RC=1

 echo "encode $srcfile in format $FORMAT to out_enc.dat"
 $TSTCMD -E "-i $srcfile $ESIZE -f $FORMAT -o /tmp/out_enc.dat" || return $RC
 $TSTCMD -D "-f $FORMAT -i /tmp/out_enc.dat" || return $RC
 rm -rf /tmp/out_enc.dat

 if [ "$NO_CAMER" = 'y' ]; then
   echo "No camera test"
 else
 	echo "encode from Camera"
 	for k in $ROTATION
 	do
  		echo "rotation mode $k"
  		for i in $MIRROR
  		do	
   			echo "mirror mode $i"
   			for j in $SIZELIST
   			do
    			OWD=$(echo $j | sed "s/x/ /g" | awk '{print $1}')
    			OHT=$(echo $j | sed "s/x/ /g" | awk '{print $2}')
    			echo "size is $OWD x $OHT"
    			$TSTCMD -E "-f $FORMAT -w $OWD -h $OHT -o /tmp/out_enc.dat -c 10 -m $i -r $k" || return $RC
    			sleep 1
    			echo "now chroma interleave mode"
    			$TSTCMD -E "-f $FORMAT -w $OWD -h $OHT -o /tmp/out_enc.dat -c 10 -m $i -r $k -t 1" || return $RC
   			done
  		done
 	done
 fi
 echo "test enc app PASS"

RC=0

return $C

}


# Function:     test_case_01
# Description   - Test if MPEG2 decode ok
#  
test_case_01()
{
#TODO give TCID 
TCID="vpu_MPEG2_test"
#TODO give TST_COUNT
TST_COUNT=1
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

srcfile=${STREAM_PATH}/video/mpeg2_720x576.mpg
FORMAT=4

echo "test decode app"
test_dec_exec || return $RC

RC=0
return $RC
}


# Function:     test_case_02
# Description   - Test if  vc1 decode ok
#  
test_case_02()
{
#TODO give TCID 
TCID="vpu_vc1_app"
#TODO give TST_COUNT
TST_COUNT=2
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

srcfile=${STREAM_PATH}/video/SD720x480.vc1.rcv
FORMAT=3
test_dec_exec || return $RC
srcfile=${STREAM_PATH}/video/COASTGUARD_CIF_IJT.yuv
ESIZE="-w 352 -h 288"
test_enc_exec || return $RC

RC=0

return $RC
}

# Function:     test_case_03
# Description   - Test if decode Divx ok
#  
test_case_03()
{
#TODO give TCID 
TCID="vpu_divx_app_test"
#TODO give TST_COUNT
TST_COUNT=3
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
srcfile=${STREAM_PATH}/video/divx311_320x240.avi 
FORMAT=5
test_dec_exec || return $RC
 
RC=0

return $RC
}

# Function:     test_case_04
# Description   - Test if decode MPEG4 ok
#  
test_case_04()
{
#TODO give TCID 
TCID="vpu_mpeg4_app_test"
#TODO give TST_COUNT
TST_COUNT=4
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
srcfile=${STREAM_PATH}/video/akiyo.mp4
FORMAT=0
test_dec_exec || return $RC
srcfile=${STREAM_PATH}/video/akiyomp4.yuv
ESIZE="-w 176 -h 144"
test_enc_exec || return $RC

RC=0
return $RC
}

# Function:     test_case_05
# Description   - Test if decode H264 with short head ok
#  
test_case_05()
{
#TODO give TCID 
TCID="vpu_h264_app"
#TODO give TST_COUNT
TST_COUNT=5
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
# todo function
srcfile=${STREAM_PATH}/video/HPCV_BRCM_A.264
FORMAT=2
test_dec_exec || return $RC
srcfile=${STREAM_PATH}/video/akiyomp4.yuv
ESIZE="-w 176 -h 144"
test_enc_exec || return $RC

RC=0
return $RC
}

# Function:     test_case_06
# Description   - Test if decode H264 BP with short head ok
#  
test_case_06()
{
#TODO give TCID 
TCID="vpu_h264BP_app"
#TODO give TST_COUNT
TST_COUNT=6
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
# main function
srcfile=${STREAM_PATH}/video/starwars640x480.264
FORMAT=2
test_dec_exec || return $RC

RC=0
return $RC
}

# Function:     test_case_7
# Description   - Test H263 app
#  
test_case_07()
{
#TODO give TCID 
TCID="vpu_dec_h263-basic"
#TODO give TST_COUNT
TST_COUNT=7
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
# main function
srcfile=${STREAM_PATH}/video/stream.263
FORMAT=1
test_dec_exec || return $RC
srcfile=${STREAM_PATH}/video/COASTGUARD_CIF_IJT.yuv
ESIZE="-w 352 -h 288"
test_enc_exec || return $RC

RC=0
return $RC
}


# Function:     test_case_8
# Description   - Test enc MJPEG app
#  
test_case_08()
{
#TODO give TCID 
TCID="vpu_MJPEG_app"
#TODO give TST_COUNT
TST_COUNT=7
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
# main function
FORMAT=7
srcfile=${STREAM_PATH}/video/akiyomp4.yuv
ESIZE="-w 176 -h 144"
test_enc_exec || return $RC

RC=0
return $RC
}


usage()
{
echo "usage $0 <1/2/3/4/5/6/7/8>"
echo "1: MPEG2 decoder test"
echo "2: VC-1 decoder test"
echo "3: Divx decoder test"
echo "4: MPEG4 decoder test"
echo "5: H264 HP decoder test"
echo "6: H264 BP decoder test"
echo "7: H263 basic test"
echo "8: MJPEG test"
}


#TODO check parameter
if [ $# -ne 1 ]
then
echo "usage $0 <1/2/3/4/5/6/7/8>"
exit 1 
fi

srcfile=
FORMAT=
SIZELIST="176x144 320x640 640x480 720x480 720x576 1024x768 1280x720 1920x1080"
ROTATION="0 90 180 270"
#ROTATION="0"
MIRROR="0 1 2 3"
UIPU="0 1"
CHINT="0 1"
ESIZE=

TARGET=


check_platform

if [ -z $TARGET ]
then
echo "unknow target"
exit 1
fi

TSTCMD="/unit_tests/mxc_vpu_test.out"

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
7)
  test_case_07 || exit $RC
  ;;
8)
  test_case_08 || exit $RC
  ;;
*)
#TODO check parameter
  usage
  ;;
esac

tst_resm TINFO "Test PASS"

