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
#    @file   vpu_pm_test.sh
#
#    @brief  shell script template for testcase vpu power manager.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#hake huang/b20222/-----          20090112     N/A          Initial version
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

return $RC
}


# Function:     test_case_01
# Description   - Test if vpu dec power management ok
#  
test_case_01()
{
#TODO give TCID 
TCID="vpu_dec_pm_test"
#TODO give TST_COUNT
TST_COUNT=1
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
cd /tmp

$TSTCMD -D "-f 1 -i ${STREAM_PATH}/video/stream.263 -o stream.yuv" &
echo "*********************"
echo "please press key to resume power"
echo "*********************"
echo -n standby > /sys/power/state
echo wait the decode task to finish
sleep 10
SIZE=$(ls -l /tmp/stream.yuv| awk '{print $5}')
if [ $SIZE = "152064" ]
then 
RC=0
fi
rm -f /tmp/stream.yuv

return $RC
}

# Function:     test_case_02
# Description   - Test if vpu encode power management ok
#  
test_case_02()
{
#TODO give TCID 
TCID="vpu_enc_pm_test"
#TODO give TST_COUNT
TST_COUNT=2
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
mkdir /tmp/enc
cd /tmp/enc

if [ $TARGET = "51" ]
then
echo 1 > /proc/sys/vm/lowmem_reserve_ratio
fi
#echo "copy the test image"
#cp -f ${LTPROOT}/testcases/bin/config_enc_h263_P3 .
#cp -f ${STREAM_PATH}/video/COASTGUARD_CIF_IJT.yuv .
echo "run encoder  first"
#$TSTCMD -C config_enc_h263_P3 || return $TST_COUNT

$TSTCMD -E "-f 1 -w 352 -h 288 -i ${STREAM_PATH}/video/COASTGUARD_CIF_IJT.yuv -o /tmp/enc/test.263" 


SIZE1=$(ls -s /tmp/enc/test.263| awk '{print $1}')
echo "rm temp result"
rm -f /tmp/enc/test.263
echo "start a process to test enc"
#$TSTCMD -C config_enc_h263_P3 &
$TSTCMD -E "-f 1 -w 352 -h 288 -i ${STREAM_PATH}/video/COASTGUARD_CIF_IJT.yuv -o /tmp/enc/test.263" & 
echo "***********"
echo "press a key to resume power"
echo "***********"
echo -n standby > /sys/power/state

sleep 10

SIZE2=$(ls -s /tmp/enc/test.263 | awk '{print $1}')

if [ $SIZE1 = $SIZE2 ]
then
RC=0
fi
rm -f /tmp/enc/test.263

cd -

return $RC
}

# Function:     test_case_03
# Description   - Test if <TODO test function> ok
#  
test_case_03()
{
#TODO give TCID 
TCID="test_demo3_test"
#TODO give TST_COUNT
TST_COUNT=3
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

return $RC

}

# Function:     test_case_04
# Description   - Test if <TODO test function> ok
#  
test_case_04()
{
#TODO give TCID 
TCID="test_demo4_test"
#TODO give TST_COUNT
TST_COUNT=4
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

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

usage()
{
echo "$0 [case ID]"
echo "1: dec with power manager"
echo "2: enc with power manager"
#echo "3: "
#echo "4: "
#echo "5: "
}

# main function

RC=0
TARGET=

TSTCMD="/unit_tests/mxc_vpu_test.out"

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
  #test_case_03 || exit $RC
  ;;
4)
  #test_case_04 || exit $RC
  ;;
5)
  #test_case_05 || exit $RC
  ;;
*)
  usage
  ;;
esac

tst_resm TINFO "Test PASS"







