###################################################################################################
#
#    @file  mipi_camera.sh
#
#    @brief  shell script template for testcase design "TODO" is where to modify block.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#Hake Huang/-----             mipi_camera     N/A          Initial version
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
export CAMERA=ov5640_mipi
v4l_module.sh setup

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
# Description   - Test ifmipi camera unit test brightness ok
#  
test_case_01()
{
#TODO give TCID 
TCID="camera_mipi_unit_test"
#TODO give TST_COUNT
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
CAPTURER=/unit_tests/mxc_v4l2_capture.out
PLAYER=/unit_tests/mxc_v4l2_output.out
TARGET=/tmp/test.yuv
# V4L2 Capture Tests
rm -f ${TARGET}
${CAPTURER} -iw 640 -ih 480 -ow 640 -oh 480 -m 0 -i 1 -r 0 -c 50 -fr 30 ${TARGET} || RC=A
${PLAYER} -iw 640 -ih 480 -ow 640 -oh 480 -r 0 -fr 30 ${TARGET} || RC=$(echo $RC B)
rm -f ${TARGET}
${CAPTURER} -iw 320 -ih 240 -ow 320 -oh 240 -m 1 -i 1 -r 0 -c 50 -fr 30 ${TARGET} || RC=$(echo $RC  C)
${PLAYER} -iw 320 -ih 240 -ow 640 -oh 480 -r 0 -fr 30 ${TARGET} || RC=$(echo $RC D)
rm -f ${TARGET}
${CAPTURER} -iw 1280 -ih 720 -ow 1280 -oh 720 -m 4 -i 1 -r 0 -c 50 -fr 30 ${TARGET} || RC=$(echo $RC  E)
${PLAYER} -iw 1280 -ih 720 -ow 640 -oh 480 -r 0 -fr 30 ${TARGET} || RC=$(echo $RC F)
rm -f ${TARGET}
${CAPTURER} -iw 1920 -ih 1080 -ow 1920 -oh 1080 -m 5 -i 1 -r 0 -c 50 -fr 30 ${TARGET}|| RC=$(echo $RC G)
${PLAYER} -iw 1920 -ih 1080 -ow 640 -oh 480 -r 0 -fr 30 ${TARGET} || RC=$(echo $RC H)
rm -f ${TARGET}

echo $RC

if [ "$RC" != "0" ];then
 RC=$(echo $RC | wc -l)
fi

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

