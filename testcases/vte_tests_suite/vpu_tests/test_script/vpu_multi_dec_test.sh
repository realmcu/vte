#!/bin/sh -x
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
#    @file  vpu_multi_test.sh
#
#    @brief  shell script for testcase design for VPU multi decode and/or encode
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#<Hake Huang>/-----             <2008/11/24>     N/A          Initial version
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
export TST_TOTAL=1

export TCID="setup"
export TST_COUNT=0
RC=1

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

cd $LTPROOT
return $RC
}


# Function:     test_case_01
# Description   - Test if multi decode test
#  
test_case_01()
{
#TODO give TCID 
TCID="vpu_multi_dec_test"
#TODO give TST_COUNT
TST_COUNT=1
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

vpu_dec_test.sh 1  >> .temp_dec &
vpu_dec_test.sh 2  >> .temp_dec || return 1

#vpu_dec_test.sh 3 >> .temp_dec &
#vpu_dec_test.sh 4 >> .temp_dec || return 1

vpu_dec_test.sh 10 >> .temp_dec &

vpu_dec_test.sh 7 >> .temp_dec &
vpu_dec_test.sh 8 >> .temp_dec || return 1

wait

CT=$(grep "Test PASS" .temp_dec | wc -l)

if [ $CT = "5" ]
then
RC=0
fi

rm -f .temp_dec

return $RC

}

# Function:     test_case_02
# Description   - Test if 4 multi decode test
#  
test_case_02()
{
#TODO give TCID 
TCID="vpu_4multi_dec_test"
#TODO give TST_COUNT
TST_COUNT=1
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

vpu_dec_test.sh 1  >> .temp_dec &
vpu_dec_test.sh 10 >> .temp_dec &
vpu_dec_test.sh 7 >> .temp_dec &
vpu_dec_test.sh 8 >> .temp_dec || return 1

wait

CT=$(grep "Test PASS" .temp_dec | wc -l)

if [ $CT = "4" ]
then
RC=0
fi

rm -f .temp_dec

return $RC

}

# Function:     test_case_03
# Description   - Test if 8 multi decode test
#  
test_case_03()
{
#TODO give TCID 
TCID="vpu_8multi_dec_test"
#TODO give TST_COUNT
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
rm -rf /tmp/out_enc.dat

srcfile=${STREAM_PATH}/video/COASTGUARD_CIF_IJT.yuv
ESIZE="-w 352 -h 288"
TSTCMD="/unit_tests/mxc_vpu_test.out"
echo "test H264 same 8 instances offscreen"

$TSTCMD -E "-i $srcfile $ESIZE -f 2 -o /tmp/out_enc.dat" || return 1
${TSTCMD} -D "-i /tmp/out_enc.dat -f 2 -w 720 -h 576 -o /dev/null" &
pID1=$!
${TSTCMD} -D "-i /tmp/out_enc.dat -f 2 -w 720 -h 576 -o /dev/null" &
pID2=$!
${TSTCMD} -D "-i /tmp/out_enc.dat -f 2 -w 720 -h 576 -o /dev/null" &
pID3=$!
${TSTCMD} -D "-i /tmp/out_enc.dat -f 2 -w 720 -h 576 -o /dev/null" &
pID4=$!
${TSTCMD} -D "-i /tmp/out_enc.dat -f 2 -w 720 -h 576 -o /dev/null" &
pID5=$!
${TSTCMD} -D "-i /tmp/out_enc.dat -f 2 -w 720 -h 576 -o /dev/null" &
pID6=$!
${TSTCMD} -D "-i /tmp/out_enc.dat -f 2 -w 720 -h 576 -o /dev/null" &
pID7=$!
${TSTCMD} -D "-i /tmp/out_enc.dat -f 2 -w 720 -h 576" &
pID8=$!

wait $pID1 
iRC1=$?
wait $pID2
iRC2=$?
wait $pID3
iRC3=$?
wait $pID4 
iRC4=$?
wait $pID5 
iRC5=$?
wait $pID6 
iRC6=$?
wait $pID7 
iRC7=$?
wait $pID8
iRC8=$?
RC=$(expr $iRC1 + $iRC2 + $iRC3 + $iRC4 + $iRC5 + $iRC6 + $iRC7 + $iRC8)

rm -f /tmp/out_enc.dat
if [ $RC -ne 0 ]; then
  echo "TFAIL: h264 8 instances test fails"
fi

echo "test MJPEG same 8 instances offscreen"

$TSTCMD -E "-i $srcfile $ESIZE -f 7 -o /tmp/out_enc.dat" || return 2
${TSTCMD} -D "-i /tmp/out_enc.dat -f 7 -w 720 -h 576 -o /dev/null" &
pID1=$!
${TSTCMD} -D "-i /tmp/out_enc.dat -f 7 -w 720 -h 576 -o /dev/null" &
pID2=$!
${TSTCMD} -D "-i /tmp/out_enc.dat -f 7 -w 720 -h 576 -o /dev/null" &
pID3=$!
${TSTCMD} -D "-i /tmp/out_enc.dat -f 7 -w 720 -h 576 -o /dev/null" &
pID4=$!
${TSTCMD} -D "-i /tmp/out_enc.dat -f 7 -w 720 -h 576 -o /dev/null" &
pID5=$!
${TSTCMD} -D "-i /tmp/out_enc.dat -f 7 -w 720 -h 576 -o /dev/null" &
pID6=$!
${TSTCMD} -D "-i /tmp/out_enc.dat -f 7 -w 720 -h 576 -o /dev/null" &
pID7=$!
${TSTCMD} -D "-i /tmp/out_enc.dat -f 7 -w 720 -h 576" &
pID8=$!

wait $pID1 
iRC1=$?
wait $pID2
iRC2=$?
wait $pID3
iRC3=$?
wait $pID4 
iRC4=$?
wait $pID5 
iRC5=$?
wait $pID6 
iRC6=$?
wait $pID7 
iRC7=$?
wait $pID8
iRC8=$?

RC=$(expr $RC + $iRC1 + $iRC2 + $iRC3 + $iRC4 + $iRC5 + $iRC6 + $iRC7 + $iRC8)

rm -f /tmp/out_enc.dat
if [ $RC -ne 0 ]; then
  echo "TFAIL: MJPEG 8 instances test fails"
fi

echo "test mixed format offscreen"
$TSTCMD -E "-i $srcfile $ESIZE -f 0 -o /tmp/out_enc1.dat" || return 3
$TSTCMD -E "-i $srcfile $ESIZE -f 1 -o /tmp/out_enc2.dat" || return 3
$TSTCMD -E "-i $srcfile $ESIZE -f 2 -o /tmp/out_enc3.dat" || return 3
#not support 3 then 7
$TSTCMD -E "-i $srcfile $ESIZE -f 7 -o /tmp/out_enc4.dat" || return 3
#not support 4 then 7
$TSTCMD -E "-i $srcfile $ESIZE -f 7 -o /tmp/out_enc5.dat" || return 3
#not support 5 then 7
$TSTCMD -E "-i $srcfile $ESIZE -f 7 -o /tmp/out_enc6.dat" || return 3
#not support 6 then 7
$TSTCMD -E "-i $srcfile $ESIZE -f 7 -o /tmp/out_enc7.dat" || return 3
$TSTCMD -E "-i $srcfile $ESIZE -f 7 -o /tmp/out_enc8.dat" || return 3

${TSTCMD} -D "-i /tmp/out_enc1.dat -f 0 -w 720 -h 576 -o /dev/null" &
pID1=$!
${TSTCMD} -D "-i /tmp/out_enc2.dat -f 1 -w 720 -h 576 -o /dev/null" &
pID2=$!
${TSTCMD} -D "-i /tmp/out_enc3.dat -f 2 -w 720 -h 576 -o /dev/null" &
pID3=$!
${TSTCMD} -D "-i /tmp/out_enc4.dat -f 7 -w 720 -h 576 -o /dev/null" &
pID4=$!
${TSTCMD} -D "-i /tmp/out_enc5.dat -f 7 -w 720 -h 576 -o /dev/null" &
pID5=$!
${TSTCMD} -D "-i /tmp/out_enc6.dat -f 7 -w 720 -h 576 -o /dev/null" &
pID6=$!
${TSTCMD} -D "-i /tmp/out_enc7.dat -f 7 -w 720 -h 576 -o /dev/null" &
pID7=$!
${TSTCMD} -D "-i /tmp/out_enc8.dat -f 7 -w 720 -h 576" &
pID8=$!

wait $pID1 
iRC1=$?
wait $pID2
iRC2=$?
wait $pID3
iRC3=$?
wait $pID4 
iRC4=$?
wait $pID5 
iRC5=$?
wait $pID6 
iRC6=$?
wait $pID7 
iRC7=$?
wait $pID8
iRC8=$?

RC=$(expr $RC + $iRC1 + $iRC2 + $iRC3 + $iRC4 + $iRC5 + $iRC6 + $iRC7 + $iRC8)

rm -f /tmp/out_enc*.dat
if [ $RC -ne 0 ]; then
  echo "TFAIL: Mixed 8 instances test fails"
fi

wait

return $RC
}


#TODO check parameter
if [ $# -ne 1 ]
then
echo "usage $0 <1/2/3>"
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
*)
#TODO check parameter
  echo "usage $0 <1>"
  ;;
esac

tst_resm TINFO "Test PASS"

