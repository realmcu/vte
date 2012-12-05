#!/bin/bash
#Copyright (C) 2012 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html

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

echo 0 > /sys/class/graphics/fb0/blank

echo -e "\033[9;0]" > /dev/tty0

#TODO add setup scripts
if [ -e /dev/mxc_vpu ]
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
rm -rf $STREAM_FILE

return $RC
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
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

cp ${STREAM_PATH}/video/mpeg2_720x576.mpg /tmp
STREAM_FILE=/tmp/mpeg2_720x576.mpg
${TSTCMD} -D "-i ${STREAM_FILE} -f 4 -y 0" || RC=1
${TSTCMD} -D "-i ${STREAM_FILE} -f 4 -y 2" || RC=$(expr $RC + 2)
rm -rf $STREAM_FILE

if [ $RC -eq 0 ]
then
RC=0
fi

return $RC
}

# Function:     test_case_02
# Description   - Test if  vc1 decode ok
#  
test_case_02()
{
#TODO give TCID 
TCID="vpu_dec_vc1_test"
#TODO give TST_COUNT
TST_COUNT=2
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

if [ $RC -eq 0 ]
then 
RC=0
fi

return $RC
}

# Function:     test_case_03
# Description   - Test if decode Divx ok
#  
test_case_03()
{
#TODO give TCID 
TCID="vpu_dec_divx_test"
#TODO give TST_COUNT
TST_COUNT=3
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
#install the firmware

return $RC
}

# Function:     test_case_04
# Description   - Test if decode MPEG4 ok
#  
test_case_04()
{
#TODO give TCID 
TCID="vpu_dec_mpeg4_test"
#TODO give TST_COUNT
TST_COUNT=4
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
cp ${STREAM_PATH}/video/akiyo.mp4 /tmp
STREAM_FILE=/tmp/akiyo.mp4
${TSTCMD} -D "-i $STREAM_FILE -f 0 -y 0" || RC=1
${TSTCMD} -D "-i $STREAM_FILE -f 0 -y 1" || RC=$(expr $RC + 2)
rm -rf $STREAM_FILE

if [ $RC -eq 0  ]
then
RC=0
fi
return $RC
}

# Function:     test_case_05
# Description   - Test if decode H263 ok
#  
test_case_05()
{
#TODO give TCID 
TCID="vpu_dec_h263-IJK"
#TODO give TST_COUNT
TST_COUNT=5
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

return $RC

}

# Function:     test_case_06
# Description   - Test if decode H263 with short head ok
#  
test_case_06()
{
#TODO give TCID 
TCID="vpu_dec_h263-head"
#TODO give TST_COUNT
TST_COUNT=6
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
# main function

return $RC
}

# Function:     test_case_07
# Description   - Test if decode H264 HP with short head ok
#  
test_case_07()
{
#TODO give TCID 
TCID="vpu_dec_h264HP_test"
#TODO give TST_COUNT
TST_COUNT=7
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
# main function
FILES="sunflower_2B_2ref_WP_40Mbps.264 HPCV_BRCM_A.264"
for sf in $FILES
do
cp ${STREAM_PATH}/video/${sf} /tmp
STREAM_FILE=/tmp/${sf}
${TSTCMD} -D "-i ${STREAM_FILE} -f 2 -y 0" || RC=1
${TSTCMD} -D "-i ${STREAM_FILE} -f 2 -y 1" || RC=$(expr $RC + 2)
rm -rf $STREAM_FILE
done
return $RC
}

# Function:     test_case_08
# Description   - Test if decode H264 BP with short head ok
#  
test_case_08()
{
#TODO give TCID 
TCID="vpu_dec_h264BP_test"
#TODO give TST_COUNT
TST_COUNT=8
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
# main function
cp ${STREAM_PATH}/video/starwars640x480.264 /tmp
STREAM_FILE=/tmp/starwars640x480.264
${TSTCMD} -D "-i ${STREAM_FILE} -f 2 -y 0" || RC=1
${TSTCMD} -D "-i ${STREAM_FILE} -f 2 -y 1" || RC=$(expr $RC + 2)
rm -rf $STREAM_FILE

if [ $RC -eq 0 ]
then
RC=0
fi

return $RC
}

# Function:     test_case_09
# Description   - Test if decode MPEG4 with deblocking ok
#  
test_case_09()
{
#TODO give TCID 
TCID="vpu_dec_mpeg4_deblock_test"
#TODO give TST_COUNT
TST_COUNT=9
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
# main function
cp ${STREAM_PATH}/video/akiyo.mp4 /tmp
STREAM_FILE=/tmp/akiyo.mp4
${TSTCMD} -D "-i $STREAM_FILE -f 0 -y 0 -d 1" || RC=1
${TSTCMD} -D "-i $STREAM_FILE -f 0 -y 1 -d 1" || RC=$(expr $RC + 2)
rm -rf $STREAM_FILE

return $RC
}

# Function:     test_case_10
# Description   - Test if decode H264 basic
#  
test_case_10()
{
#TODO give TCID 
TCID="vpu_dec_h264"
#TODO give TST_COUNT
TST_COUNT=10
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
# main function
return $RC
}


# Function:     test_case_11
# Description   - Test if MPEG2 deblock ok
#  
test_case_11()
{
#TODO give TCID 
TCID="vpu_MPEG2_deblock_test"
#TODO give TST_COUNT
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

#vpu_testapp -C ${LTPROOT}/testcases/bin/config_dec_mpeg2 
cp ${STREAM_PATH}/video/mpeg2_720x576.mpg /tmp
STREAM_FILE=/tmp/mpeg2_720x576.mpg
${TSTCMD} -D "-i $STREAM_FILE -f 4 -y 0 -d 1" || RC=1
${TSTCMD} -D "-i $STREAM_FILE -f 4 -y 2 -d 1" || RC=$(expr $RC + 2)
rm -rf $STREAM_FILE
return $RC
}


# Function:     test_case_12
# Description   - Test if H264 ipu deInterlace VDI ok
#  
test_case_12()
{
#TODO give TCID 
TCID="vpu_H264_VDI_test"
#TODO give TST_COUNT
TST_COUNT=1
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
echo "TST_INFO: h264 MP VDI test"
FILES="H264_MP30_interlaced_poc2_720x576.h264 \
	  1080i_shields1088i2997_shields_ter_4x300_15fps_track1.h264 \
      H264_HP41_1920x1088_30fps_55.8Mbps_shields_ter_Noaudio_track1.h264"
for sf in $FILES
do
cp ${STREAM_PATH}/video/$sf /tmp/
STREAM_FILE=/tmp/$sf
${TSTCMD} -D "-i $STREAM_FILE -f 2 -w 720 -h 640 -y 0" || RC=1
${TSTCMD} -D "-i $STREAM_FILE -f 2 -w 720 -h 640 -y 2" || RC=$(expr $RC + 2)
rm -rf $STREAM_FILE
done


return $RC
}

# Function:     test_case_13
# Description   - Test if VC1 ipu deInterlace VDI ok
#  
test_case_13()
{
#TODO give TCID 
TCID="vpu_VC1_VDI_test"
#TODO give TST_COUNT
TST_COUNT=1
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

return $RC
}

# Function:     test_case_14
# Description   - Test if Real video  ok
test_case_14()
{
#TODO give TCID
TCID="vpu_REAL_test"
#TODO give TST_COUNT
TST_COUNT=1
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

RC=0
return $RC
}

# Function:     test_case_15
# Description   - Test if VP8 ok
test_case_15()
{
#TODO give TCID
TCID="vpu_VP8_test"
#TODO give TST_COUNT
TST_COUNT=1
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

echo "TST_INFO: VP8 playback"
cp ${STREAM_PATH}/video/blue_sky_mp8_2mbps_sh7_1920x1088.vp8 /tmp/
STREAM_FILE=/tmp/blue_sky_mp8_2mbps_sh7_1920x1088.vp8
${TSTCMD} -D "-i $STREAM_FILE -f 9 -y 0" || RC=1
${TSTCMD} -D "-i $STREAM_FILE -f 9 -y 1" || RC=$(expr $RC + 2)
rm -rf $STREAM_FILE

return $RC
}

# Function:     test_case_16
# Description   - Test AVS video  ok
test_case_16()
{
#TODO give TCID
TCID="vpu_AVS_test"
#TODO give TST_COUNT
TST_COUNT=16
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

echo "TST_INFO: AVS playback"
cp ${STREAM_PATH}/video/12_zju_0_0_6.0_foreman_cif.avs /tmp
STREAM_FILE=/tmp/12_zju_0_0_6.0_foreman_cif.avs
${TSTCMD} -D "-i $STREAM_FILE -f 8 -y 0" || RC=1
${TSTCMD} -D "-i $STREAM_FILE -f 8 -y 1" || RC=$(expr $RC + 2)
rm -rf $STREAM_FILE

return $RC
}

usage()
{
echo "usage $0 <1/2/3/4/5/6/7/8/9/10/11/12/13/14/15/16>"
echo "1: MPEG2 decoder test"
echo "2: VC-1 decoder test"
echo "3: Divx decoder test"
echo "4: MPEG4 decoder test"
echo "5: H263-P3 decoder test"
echo "6: H263+header decoder test"
echo "7: H264 HP decoder test"
echo "8: H264 BP decoder test"
echo "9: MPEG4 decoder+ deblock test"
echo "10: H263 basic test"
echo "11: MPEG2 decoder + deblock test"
echo "12: H264 vdi test"
echo "13: VC1 vdi test"
echo "14: RV test"
echo "15: VP8 test"
echo "16: AVS test"
}

#TODO check parameter
if [ $# -ne 1 ]
then
echo "usage $0 <1/2/3/4/5/6/7/8/9/10/11/12/13/14/15/16>"
usage
exit 1 
fi


platfm.sh
TARGET=$?


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
9)
  test_case_09 || exit $RC
  ;;
10)
  test_case_10 || exit $RC
  ;;
11)
  test_case_11 || exit $RC
  ;;
12)
  test_case_12 || exit $RC
  ;;
13)
  test_case_13 || exit $RC
  ;;
14)
  test_case_14 || exit $RC
  ;;
15)
  test_case_15 || exit $RC
  ;;
16)
  test_case_16 || exit $RC
  ;;
*)
  i=0
  for i in 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16
  do
    test_case_${i} || RC=$(echo case_${i}=$? $RC)
	i=$(expr $i + 1)
  done
  ;;
esac

if [ "$RC" = "0" ]; then
tst_resm TINFO "Test PASS"
else
tst_resm TINFO "Test FAIL"
echo $RC
fi


