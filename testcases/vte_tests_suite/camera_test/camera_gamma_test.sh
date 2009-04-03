#!/bin/sh
###################################################################################################
#
#    @file  camera_gamm_test.sh
#
#    @brief  shell script template for testcase design "TODO" is where to modify block.
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
#Hake Huang/-----             camera gamma     N/A          Initial version
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
RC=0

trap "cleanup" 0

#TODO add setup scripts

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
# Description   - Test if camera gamma brightness ok
#  
test_case_01()
{
#TODO give TCID 
TCID="camera_gamma_brightness_test"
#TODO give TST_COUNT
TST_COUNT=1
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

# V4L2 Capture Tests
/unit_tests/mxc_v4l2_overlay.out  -iw 640 -ih 480 -ow 240 -oh 320 -r 4 -fr 30 -fg -t 10 -v 1 || exit $RC
/unit_tests/mxc_v4l2_overlay.out  -iw 640 -ih 480 -ow 240 -oh 320 -r 4 -fr 30 -t 10 -v 1 || exit $RC

for ROT in 0 1 2 3 4 5 6 7; do
	/unit_tests/mxc_v4l2_overlay.out  -iw 640 -ih 480 -ow 240 -oh 184 -r $ROT -fr 30 -fg -t 5 -v 1 || exit $RC
done

for POS in 0 4 8 16 32 64 128; do
	/unit_tests/mxc_v4l2_overlay.out  -iw 640 -ih 480 -ot $POS -ol $POS -ow 80 -oh 60 -fr 30 -fg -t 5 -v 1 || exit $RC
done

RC=0

return $RC

}

# Function:     test_case_02
# Description   - Test if camera gamma saturation ok
#  
test_case_02()
{
#TODO give TCID 
TCID="camera_gamma_saturation_test"
#TODO give TST_COUNT
TST_COUNT=1
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

# V4L2 Capture Tests
/unit_tests/mxc_v4l2_overlay.out  -iw 640 -ih 480 -ow 240 -oh 320 -r 4 -fr 30 -fg -t 10 -v 2 || exit $RC
/unit_tests/mxc_v4l2_overlay.out  -iw 640 -ih 480 -ow 240 -oh 320 -r 4 -fr 30 -t 10 -v 2 || exit $RC

for ROT in 0 1 2 3 4 5 6 7; do
	/unit_tests/mxc_v4l2_overlay.out  -iw 640 -ih 480 -ow 240 -oh 184 -r $ROT -fr 30 -fg -t 5 -v 2 || exit $RC
done

for POS in 0 4 8 16 32 64 128; do
	/unit_tests/mxc_v4l2_overlay.out  -iw 640 -ih 480 -ot $POS -ol $POS -ow 80 -oh 60 -fr 30 -fg -t 5 -v 2 || exit $RC
done

RC=0
return $RC
}

# Function:     test_case_03
# Description   - Test if camera gamma red balance ok
#  
test_case_03()
{
#TODO give TCID 
TCID="camera_gamma_redbalance_test"
#TODO give TST_COUNT
TST_COUNT=1
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

# V4L2 Capture Tests
/unit_tests/mxc_v4l2_overlay.out  -iw 640 -ih 480 -ow 240 -oh 320 -r 4 -fr 30 -fg -t 10 -v 3 || exit $RC
/unit_tests/mxc_v4l2_overlay.out  -iw 640 -ih 480 -ow 240 -oh 320 -r 4 -fr 30 -t 10 -v 3 || exit $RC

for ROT in 0 1 2 3 4 5 6 7; do
	/unit_tests/mxc_v4l2_overlay.out  -iw 640 -ih 480 -ow 240 -oh 184 -r $ROT -fr 30 -fg -t 5 -v 3 || exit $RC
done

for POS in 0 4 8 16 32 64 128; do
	/unit_tests/mxc_v4l2_overlay.out  -iw 640 -ih 480 -ot $POS -ol $POS -ow 80 -oh 60 -fr 30 -fg -t 5 -v 3 || exit $RC
done

RC=0
return $RC

}

# Function:     test_case_04
# Description   - Test if camera gamma blue balance ok
#  
test_case_04()
{
#TODO give TCID 
TCID="camera_gamma_bluebalance_test"
#TODO give TST_COUNT
TST_COUNT=1
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

# V4L2 Capture Tests
/unit_tests/mxc_v4l2_overlay.out  -iw 640 -ih 480 -ow 240 -oh 320 -r 4 -fr 30 -fg -t 10 -v 4 || exit $RC
/unit_tests/mxc_v4l2_overlay.out  -iw 640 -ih 480 -ow 240 -oh 320 -r 4 -fr 30 -t 10 -v 4 || exit $RC

for ROT in 0 1 2 3 4 5 6 7; do
	/unit_tests/mxc_v4l2_overlay.out  -iw 640 -ih 480 -ow 240 -oh 184 -r $ROT -fr 30 -fg -t 5 -v 4 || exit $RC
done

for POS in 0 4 8 16 32 64 128; do
	/unit_tests/mxc_v4l2_overlay.out  -iw 640 -ih 480 -ot $POS -ol $POS -ow 80 -oh 60 -fr 30 -fg -t 5 -v 4 || exit $RC
done

RC=0
return $RC

}

# Function:     test_case_05
# Description   - Test if camera gamma black level ok
#  
test_case_05()
{
#TODO give TCID 
TCID="camera_gamma_blacklevel_test"
#TODO give TST_COUNT
TST_COUNT=1
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

# V4L2 Capture Tests
/unit_tests/mxc_v4l2_overlay.out  -iw 640 -ih 480 -ow 240 -oh 320 -r 4 -fr 30 -fg -t 10 -v 5 || exit $RC
/unit_tests/mxc_v4l2_overlay.out  -iw 640 -ih 480 -ow 240 -oh 320 -r 4 -fr 30 -t 10 -v 5 || exit $RC

for ROT in 0 1 2 3 4 5 6 7; do
	/unit_tests/mxc_v4l2_overlay.out  -iw 640 -ih 480 -ow 240 -oh 184 -r $ROT -fr 30 -fg -t 5 -v 5 || exit $RC
done

for POS in 0 4 8 16 32 64 128; do
	/unit_tests/mxc_v4l2_overlay.out  -iw 640 -ih 480 -ot $POS -ol $POS -ow 80 -oh 60 -fr 30 -fg -t 5 -v 5 || exit $RC
done

RC=0

return $RC

}

# main function

RC=0

#TODO check parameter
if [ $# -ne 1 ]
then
echo "usage $0 <1/2/3/4/5>"
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







