#!/bin/sh
###################################################################################################
#
#    @file   ipu_test.sh
#
#    @brief  shell script template for testcase design "IPU" is where to modify block.
#
###################################################################################################
#
#   Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
#   THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#   BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#   Freescale Semiconductor, Inc.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#Hake Huang/-----             <20081209>     N/A          Initial version
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
RC=1

trap "cleanup" 0

#TODO add setup scripts

if [ $TARGET = "31" ]
then
 MLIST="ipu_prp_enc ipu_prp_vf_sdc_bg ov2640_camera ipu_prp_vf_sdc ipu_still mxc_v4l2_capture"
 for i in $MLIST
 do
  modprobe $i
 done
fi

if [ $TARGET = "35" ]
then
 MLIST="ipu_prp_enc ipu_prp_vf_sdc_bg ov2640_camera ipu_prp_vf_sdc ipu_still mxc_v4l2_capture"
 for i in $MLIST
 do
   modprobe $i
 done
fi

sleep 3

if [ -e /dev/mxc_ipu ]
then
RC=0
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

return $RC
}

check_platform()
{
  PLATFORM="31 35 37 51"
  FB_AVAIL="37 51"
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
  if [ ! -z $TARGET ]
  then
   CF=$(echo $FB_AVAIL | grep $TARGET | wc -l)
   if [ $CF -ne 0 ]
   then
     FB_ENABLE=TRUE
   fi
  fi
}

# Function:     test_case_01
# Description   - Test if module exist ok
#  
test_case_01()
{
#TODO give TCID 
TCID="ipu_dev_node_test"
#TODO give TST_COUNT
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here


return $RC

}

# Function:     test_case_02
# Description   - Test ipu_ENC_dev ok
#  
test_case_02()
{
#TODO give TCID 
TCID="IPU_ENC_dev"
#TODO give TST_COUNT
TST_COUNT=2
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
for i in $IN_FILE
do
#parser the string
WD=$(echo $i | sed "s/+/ /g" | awk '{print $1}' )
HT=$(echo $i | sed "s/+/ /g" | awk '{print $2}' )
INFILE=$(echo $i | sed "s/+/ /g"| awk '{print $3}')
 #now test the resize
 for j in $RESLIST
 do
 OWD=$(echo $j | sed "s/x/ /g" | awk '{print $1}')
 OHT=$(echo $j | sed "s/x/ /g" | awk '{print $2}')
 echo "ipu_dev_test -iw $WD -ih $HT -if I420 -ow $OWD -oh $OHT -of BGR3 -r 0 -t ENC -out /tmp/out.dat ${INPATH}/${INFILE}"
 ipu_dev_test -iw $WD -ih $HT -if I420 -ow $OWD -oh $OHT -of BGR3 -r 0 -t ENC -out /tmp/out.dat ${INPATH}/${INFILE} || return 1
 rm -f /tmp/out.dat
 sleep 1 
 done
#test rotation
 for j in $ROTATION
 do
  echo "rotation $j"
  #to frame buffer
  if [ $FB_ENABLE ]
  then
  echo "ipu_dev_test -iw $WD -ih $HT -if I420 -ow 480 -oh 640 -of RGBP -r $j -c 10 -t ENC ${INPATH}/${INFILE}"
  ipu_dev_test -iw $WD -ih $HT -if I420 -ow 480 -oh 640 -of RGBP -r $j -c 10 -t ENC ${INPATH}/${INFILE} || return 1
  fi
  #to file
  echo "ipu_dev_test -iw $WD -ih $HT -if I420 -ow 352 -oh 288 -of BGR3 -r 0 -t ENC -out /tmp/out.dat ${INPATH}/${INFILE}"
  ipu_dev_test -iw $WD -ih $HT -if I420 -ow 352 -oh 288 -of BGR3 -r 0 -t ENC -out /tmp/out.dat ${INPATH}/${INFILE} || return 1
  rm -f /tmp/out.dat
  sleep 1
 done

 #now test the format conversion
 for j in $FMLIST
 do
  echo "ipu_dev_test -iw $WD -ih $HT -if I420 -ow 352 -oh 288 -of $j -r 0 -t ENC -out /tmp/out.dat ${INPATH}/${INFILE}"
  ipu_dev_test -iw $WD -ih $HT -if I420 -ow 352 -oh 288 -of $j -r 0 -t ENC -out /tmp/out.dat ${INPATH}/${INFILE} || return 1
  for k in $FMLIST
  do
   echo "$j->$k"
   echo "ipu_dev_test -iw $WD -ih $HT -if $j -ow 352 -oh 288 -of $k -r 0 -t ENC -out /tmp/out1.dat /tmp/out.dat"
   ipu_dev_test -iw $WD -ih $HT -if $j -ow 352 -oh 288 -of $k -r 0 -t ENC -out /tmp/out1.dat /tmp/out.dat || return 1
   rm -f /tmp/out1.dat
   sleep 1
  done
  rm -f /tmp/out.dat
  sleep 1
 done
 

 sleep 1
done
RC=0
return $RC

}

# Function:     test_case_03
# Description   - Test if IPU_PP_test ok
#  
test_case_03()
{
#TODO give TCID 
TCID="IPU_PP_TEST"
#TODO give TST_COUNT
TST_COUNT=3
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

for i in $IN_FILE
do
#parser the string
WD=$(echo $i | sed "s/+/ /g" | awk '{print $1}' )
HT=$(echo $i | sed "s/+/ /g" | awk '{print $2}' )
INFILE=$(echo $i | sed "s/+/ /g"| awk '{print $3}')
#test rotation
 for j in $ROTATION
 do
  echo "rotation $j"
  #to frame buffer
  if [ $FB_ENABLE ]
  then
   echo "ipu_dev_test -iw $WD -ih $HT -if I420 -ow 480 -oh 640 -of RGBP -r $j -c 10 -t PP ${INPATH}/${INFILE}"
   ipu_dev_test -iw $WD -ih $HT -if I420 -ow 480 -oh 640 -of RGBP -r $j -c 10 -t PP ${INPATH}/${INFILE} || return 1
  fi
  #to file
  echo "ipu_dev_test -iw $WD -ih $HT -if I420 -ow 352 -oh 288 -of BGR3 -r 0 -t PP -out /tmp/out.dat ${INPATH}/${INFILE}"
  ipu_dev_test -iw $WD -ih $HT -if I420 -ow 352 -oh 288 -of BGR3 -r 0 -t PP -out /tmp/out.dat ${INPATH}/${INFILE} || return 1
  rm -f /tmp/out.dat
  sleep 1
 done

 #now test the format conversion
 for j in $FMLIST
 do
  echo "ipu_dev_test -iw $WD -ih $HT -if I420 -ow 352 -oh 288 -of $j -r 0 -t PP -out /tmp/out.dat ${INPATH}/${INFILE}"
  ipu_dev_test -iw $WD -ih $HT -if I420 -ow 352 -oh 288 -of $j -r 0 -t PP -out /tmp/out.dat ${INPATH}/${INFILE} || return 1
  for k in $FMLIST
  do
   echo "ipu_dev_test -iw $WD -ih $HT -if $j -ow 352 -oh 288 -of $k -r 0 -t PP -out /tmp/out1.dat /tmp/out.dat "
   ipu_dev_test -iw $WD -ih $HT -if $j -ow 352 -oh 288 -of $k -r 0 -t PP -out /tmp/out1.dat /tmp/out.dat || return 1
   rm -f /tmp/out1.dat
   sleep 1
  done
  rm -f /tmp/out.dat
  sleep 1
 done
 
 #now test the resize
 for j in $RESLIST
 do
 OWD=$(echo $j | sed "s/x/ /g" | awk '{print $1}')
 OHT=$(echo $j | sed "s/x/ /g" | awk '{print $2}')
 echo "ipu_dev_test -iw $WD -ih $HT -if I420 -ow $OWD -oh $OHT -of BGR3 -r 0 -t PP -out /tmp/out.dat ${INPATH}/${INFILE}"
 ipu_dev_test -iw $WD -ih $HT -if I420 -ow $OWD -oh $OHT -of BGR3 -r 0 -t PP -out /tmp/out.dat ${INPATH}/${INFILE} || return 1
 rm -f /tmp/out.dat
 sleep 1 
 done

 sleep 1
done
RC=0
return $RC

}

# Function:     test_case_04
# Description   - Test if IPU_VF_test ok
#  
test_case_04()
{
#TODO give TCID 
TCID="IPU_VF_TEST"
#TODO give TST_COUNT
TST_COUNT=4
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

for i in $IN_FILE
do
#parser the string
WD=$(echo $i | sed "s/+/ /g" | awk '{print $1}' )
HT=$(echo $i | sed "s/+/ /g" | awk '{print $2}' )
INFILE=$(echo $i | sed "s/+/ /g"| awk '{print $3}')
 #now test the resize
 for j in $RESLIST
 do
 OWD=$(echo $j | sed "s/x/ /g" | awk '{print $1}')
 OHT=$(echo $j | sed "s/x/ /g" | awk '{print $2}')
 echo "ipu_dev_test -iw $WD -ih $HT -if I420 -ow $OWD -oh $OHT -of BGR3 -r 0 -t VF -out /tmp/out.dat ${INPATH}/${INFILE}"
 ipu_dev_test -iw $WD -ih $HT -if I420 -ow $OWD -oh $OHT -of BGR3 -r 0 -t VF -out /tmp/out.dat ${INPATH}/${INFILE} || return 1
 sleep 1 
 done
#test rotation
 for j in $ROTATION
 do
  echo "rotation $j"
  #to frame buffer
  if [ $FB_ENABLE ]
  then
   echo "ipu_dev_test -iw $WD -ih $HT -if I420 -ow 480 -oh 640 -of RGBP -r $j -c 10 -t VF ${INPATH}/${INFILE}"
   ipu_dev_test -iw $WD -ih $HT -if I420 -ow 480 -oh 640 -of RGBP -r $j -c 10 -t VF ${INPATH}/${INFILE} || return 1
  fi
  #to file
  echo "ipu_dev_test -iw $WD -ih $HT -if I420 -ow 352 -oh 288 -of BGR3 -r 0 -t VF -out /tmp/out.dat ${INPATH}/${INFILE}"
  ipu_dev_test -iw $WD -ih $HT -if I420 -ow 352 -oh 288 -of BGR3 -r 0 -t VF -out /tmp/out.dat ${INPATH}/${INFILE} || return 1
  rm -f /tmp/out.dat
  sleep 1
 done

 #now test the format conversion
 for j in $FMLIST
 do
  echo "ipu_dev_test -iw $WD -ih $HT -if I420 -ow 352 -oh 288 -of $j -r 0 -t VF -out /tmp/out.dat ${INPATH}/${INFILE}"
  ipu_dev_test -iw $WD -ih $HT -if I420 -ow 352 -oh 288 -of $j -r 0 -t VF -out /tmp/out.dat ${INPATH}/${INFILE} || return 1
  for k in $FMLIST
  do
   echo "$j->$k"
   echo "ipu_dev_test -iw $WD -ih $HT -if $j -ow 352 -oh 288 -of $k -r 0 -t VF -out /tmp/out1.dat /tmp/out.dat"
   ipu_dev_test -iw $WD -ih $HT -if $j -ow 352 -oh 288 -of $k -r 0 -t VF -out /tmp/out1.dat /tmp/out.dat || return 1
   sleep 1
  done
  sleep 1
 done
 

 sleep 1
done
RC=0
return $RC

}

# main function

RC=0

IN_FILE="352+288+COASTGUARD_CIF_IJT.yuv 176+144+akiyomp4.yuv"
ROTATION="0 1 2 3 4 5 6 7"
TARGET=
FB_ENABLE=
INPATH=${LTPROOT}/../test_stream/video
#format list
FMLIST="RGBP BGR3 RGB3 BGR4 BGRA RGB4 RGBA ABGR YUYV UYVY Y444 NV12 I420 422P YV16"
#resolution list for avga vga cif qcif
RESLIST="320x240 640x480 352x288 176x144"

usage()
{
 echo "$0 <case ID> "
 echo "1: module exist test"
 echo "2: IPU ENC task test"
 echo "3: IPU PP task test"
 echo "4: IPU VF task test"
}

#TODO check parameter
if [ $# -ne 1 ]
then
usage
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
4)
  test_case_04 || exit $RC
  ;;
*)
  usage
  ;;
esac

tst_resm TINFO "Test PASS"







