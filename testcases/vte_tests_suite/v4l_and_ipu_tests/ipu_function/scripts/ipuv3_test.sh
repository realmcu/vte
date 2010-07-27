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

#set fb0 alpha to 255
imx_fb SET ALPHA 255 0

#set fb0 color key to red
imx_fb SET COLORKEY 255 0 0 0

#draw a red pattern on fb0
imx_fb DRAW PATTERN RED 0

#draw a green pattern on fb2
imx_fb DRAW PATTERN GREEN 2

echo  "did you see the screen in GREEN ?"

read -p "y / n? :" re

imx_fb DRAW PATTERN BLUE 0

echo  "did you see the screen in BLUE ?"

read -p "y / n? :" re

if [ $re = 'y' ]; then
RC=0
else
RC=1
fi

return $RC

}

# Function:     test_case_02
# Description   - Test if local alpha ok
#  
test_case_02()
{
#TODO give TCID 
TCID="test_local_alpha"
#TODO give TST_COUNT
TST_COUNT=2
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
#fbset -depth 32

imx_fb SET ALPHA 255 0

imx_fb DRAW PATTERN RED 0

sleep 2

imx_fb DRAW PATTERN GREEN 2

sleep 2

imx_fb SET LOCALALPHA 1 1 255 0 0
echo "Pattern *** red only"
read -p "press to continue"

imx_fb SET LOCALALPHA 1 1 0 255 0
echo "*** green only"
read -p "press to continue"

imx_fb SET LOCALALPHA 1 1 128 128 0
echo "*** mixer"
read -p "press to continue"
#imx_fb DRAW PATTERN RED 0

read -p  "above description right? y/n:" re

if [ $re = 'y' ]; then
RC=0
else
RC=1
fi

imx_fb SET LOCALALPHA 0 0 0 0 0

return $RC
}

run_bg()
{
echo 0 > /sys/class/graphics/fb0/blank;
#wait till the v4l output start
sleep 5
LOOP=21;
while [ $LOOP -gt 1 ];
do
echo 1 > /sys/class/graphics/fb0/blank ;
sleep 1 ;
echo 0 > /sys/class/graphics/fb0/blank;
LOOP=$(expr $LOOP - 1);
echo "next loop";
done;
}

# Function:     test_case_03
# Description   - Test if ipu blank / unblank ok
#  
test_case_03()
{
#TODO give TCID 
TCID="test_blank_test"
#TODO give TST_COUNT
TST_COUNT=3
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
run_bg &
v4l_output_testapp -B 10,10,320,240  -C 2 -R 3 -r 1000 -F $LTPROOT/testcases/bin/green_RGB24
RC=$?
wait
rm -rf /tmp/v4llog
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







