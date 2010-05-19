#Copyright (C) 2008-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
#    @file   vpu_enc_test.sh
#
#    @brief  shell script template for testcase design "TODO" is where to modify block.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#<Hake Huang>/-----             <2008/09/19>     N/A          Initial version
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
export TST_TOTAL=5

export TCID="setup"
export TST_COUNT=0
RC=1

trap "cleanup" 0

#TODO add setup scripts
if [ -e /dev/mxc_vpu ]
then
RC=0
fi
mkdir /tmp/enc
cd /tmp/enc

if [ $TARGET = "51" ]
then
echo 1 > /proc/sys/vm/lowmem_reserve_ratio
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
#rm -rf /tmp/enc
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


# Function:     test_case_01
# Description   - Test if unit test ok
#  
test_case_01()
{
#TODO give TCID 
TCID="vpu_unit_test"
#TODO give TST_COUNT
TST_COUNT=1
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

#/unit_tests/mxc_vpu_test.out -C config_enc || return $TST_COUNT

RC=0
return $RC

}

# Function:     test_case_02
# Description   - Test if encode h263 ok
#  
test_case_02()
{
#TODO give TCID 
TCID="vpu_enc_h263_test"
#TODO give TST_COUNT
TST_COUNT=2
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
cp -f ${LTPROOT}/testcases/bin/config_enc_h263_P3 .
cp -f ${STREAM_PATH}/video/COASTGUARD_CIF_IJT.yuv .

$TSTCMD -C config_enc_h263_P3 || return $TST_COUNT
 
rm -f config_enc_h263_P3
rm -f COASTGUARD_CIF_IJT.yuv 

$TSTCMD -D "-i test.263 -f 1 -o /dev/null" || return $TST_COUNT

rm -f test.h263

RC=0
return $RC

}

# Function:     test_case_03
# Description   - Test if encode H264 ok
#  
test_case_03()
{
#TODO give TCID 
TCID="vpu_enc_h264_test"
#TODO give TST_COUNT
TST_COUNT=3
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
cp -f ${LTPROOT}/testcases/bin/config_enc_h264_BP .
cp -f ${STREAM_PATH}/video/starwars640x480.yuv .

$TSTCMD -C config_enc_h264_BP || return $TST_COUNT
 
rm -f config_enc_h264_BP
rm -f starwars640x480.yuv

$TSTCMD -D "-i test.264 -f 2 -o /dev/null" || return $TST_COUNT

rm -f test.h264

RC=0
return $RC

}

# Function:     test_case_04
# Description   - Test if encode MPEG4 ok
#  
test_case_04()
{
#TODO give TCID 
TCID="vpu_enc_mpeg4_test"
#TODO give TST_COUNT
TST_COUNT=4
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
cp -f ${LTPROOT}/testcases/bin/config_enc_MPEG4_SP .
cp -f ${STREAM_PATH}/video/akiyomp4.yuv .

$TSTCMD -C config_enc_MPEG4_SP || return $TST_COUNT
 
rm -f config_enc_MPEG4_SP
rm -f akiyomp4.yuv

$TSTCMD -D "-i test.mp4 -f 0 -o /dev/null" || return $TST_COUNT

rm -f test.mp4

RC=0
return $RC

}

# Function:     test_case_05
# Description   - Test if encode MJPEG ok
#  
test_case_05()
{
#TODO give TCID 
TCID="vpu_enc_mjpeg_test"
#TODO give TST_COUNT
TST_COUNT=5
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
cp -f ${LTPROOT}/testcases/bin/config_enc_MJPEG_BL .
cp -f ${STREAM_PATH}/video/akiyomp4.yuv .

$TSTCMD -C config_enc_MJPEG_BL || return $TST_COUNT
 
rm -f config_enc_MJPEG_BL
rm -f akiyomp4.yuv

$TSTCMD -D "-i test.mpg -f 7 -o /dev/null" || return $TST_COUNT

rm -f test.mpg

RC=0
return $RC

}

# Function:     test_case_06
# Description   - Test if encode VC1 ok
#
test_case_06()
{
#TODO give TCID
TCID="vpu_enc_vc1_test"
#TODO give TST_COUNT
TST_COUNT=6
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
cp -f ${LTPROOT}/testcases/bin/config_enc_vc1 .
cp -f ${STREAM_PATH}/video/akiyomp4.yuv .

$TSTCMD -C config_enc_vc1 || return $TST_COUNT

rm -f config_enc_vc1
rm -f akiyomp4.yuv

$TSTCMD -D "-i test.wmv -f 3 -o /dev/null" || return $TST_COUNT

rm -f test.wmv

RC=0
return $RC

}


# main function

RC=0

TSTCMD="/unit_tests/mxc_vpu_test.out"

#TODO check parameter
if [ $# -ne 1 ]
then
echo "usage $0 <1/2/3/4/5>"
exit 1 
fi

TARGET=

check_platform

if [ -z $TARGET ]
then
echo "unknown target"
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
*#TODO check parameter
  echo "usage $0 <1/2/3/4/5>"
  ;;
esac

tst_resm TINFO "Test PASS"







