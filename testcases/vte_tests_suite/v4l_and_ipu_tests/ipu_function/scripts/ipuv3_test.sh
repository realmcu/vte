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
#    @file   ipuv3_test.sh
#
#    @brief  shell script for testcase design "ipu v3" .
#
###################################################################################################
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#Hake Huang/-----             20090910         N/A          Initial version
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
# Description   - Test if DP YUV display color key ok
#  
test_case_01()
{
#TODO give TCID 
TCID="test_DP_YUV_COLORKEY"
#TODO give TST_COUNT
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

echo "swith two layer to TVOUT"
echo 0 > /sys/class/graphics/fb0/blank 
echo 4 > /sys/class/graphics/fb0/blank
echo 4 > /sys/class/graphics/fb1/blank
echo 4 > /sys/class/graphics/fb2/blank
echo 1-layer-fb > /sys/class/graphics/fb0/fsl_disp_property
echo U:720x480i-60 > /sys/class/graphics/fb1/mode
echo 0 > /sys/class/graphics/fb1/blank

sleep 2
#set fb1 alpha to 255
imx_fb SET ALPHA 255 1

#set fb1 color key to red
imx_fb SET COLORKEY 255 0 0 1

#draw a red pattern on fb
imx_fb DRAW PATTERN RED 1

echo "please see the screen with video"

#play a v4l video in fb0
/unit_tests/mxc_v4l2_overlay.out -iw 640 -ih 480  -r 0 -t 5


echo "now only green screen is displayed"

#draw a red pattern on fb
imx_fb DRAW PATTERN GREEN 1

#play a v4l video in fb0
/unit_tests/mxc_v4l2_overlay.out -iw 640 -ih 480  -r 0 -t 5

echo "please confirm that you see video with red block and"
echo "no video when green "

read -p "y / n? :" re

if [ $re = 'y' ]; then
RC=0
else
RC=1
fi

return $RC

}

# Function:     test_case_02
# Description   - Test if <TODO test function> ok
#  
test_case_02()
{
#TODO give TCID 
TCID="test_demo2_test"
#TODO give TST_COUNT
TST_COUNT=2
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

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

usage()
{
echo "$0 [case ID]"
echo "1: "
echo "2: "
echo "3: "
echo "4: "
echo "5: "
}

# main function

RC=0

#TODO check parameter
if [ $# -ne 1 ]
then
usage
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
  usage
  ;;
esac

tst_resm TINFO "Test PASS"







