#Copyright (C) 2009-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
#    @file   mx37_vpu_dec_test.sh
#
#    @brief  shell script for testcase design for VPU decode
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#<Hake Huang>/-----             <2008/11/14>     N/A          Initial version
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

if [ $TARGET = "37" ] || [ $TARGET = "51"  ]
then
 echo 1 > /proc/sys/vm/lowmem_reserve_ratio
fi

export VPU_TV_MODE=720P


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

export VPU_TV_MODE=

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
# Description   - Test if H264 HD decode ok
#  
test_case_01()
{
#TODO give TCID 
TCID="vpu_H264_HD_test"
#TODO give TST_COUNT
TST_COUNT=1
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
echo "TST_INFO: check BP decode"
${TSTCMD} -D "-i ${STREAM_PATH}/video/H264_BP11_1280x720.h264 -f 2" || return $RC
echo "TST_INFO: check HP decode"
${TSTCMD} -D "-i ${STREAM_PATH}/video/H264_HP51_bwp_1280x720.h264 -f 2" || return $RC
echo "TST_INFO: check MP decode"
${TSTCMD} -D "-i ${STREAM_PATH}/video/H264_MP51_1280x720.h264 -f 2" || return $RC


RC=0
return $RC
}

# Function:     test_case_02
# Description   - Test if  MPEG4 HD decode ok
#  
test_case_02()
{
#TODO give TCID 
TCID="vpu_dec_MPEG4_HD_test"
#TODO give TST_COUNT
TST_COUNT=2
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
echo "MP4 HD SP 15fps"
${TSTCMD} -D "-i ${STREAM_PATH}/video/Mpeg4_SP1_1280x720_15fps.mp4 -f 0" || return $RC
echo "MP4 HD SP 30fps"
${TSTCMD} -D "-i ${STREAM_PATH}/video/Mpeg4_SP1_1280x720_30fps.mp4 -f 0" || return $RC

RC=0
return $RC
}


usage()
{
echo "usage $0 <1/2>"
echo "1: H264 HD decoder test"
echo "2: MPEG4 HD decoder test"
}

#TODO check parameter
if [ $# -ne 1 ]
then
echo "usage $0 <1/2>"
usage
exit 1 
fi


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
*)
#TODO check parameter
  usage
  ;;
esac

tst_resm TINFO "Test PASS"







