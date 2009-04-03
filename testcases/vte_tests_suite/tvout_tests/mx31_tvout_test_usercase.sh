###################################################################################################
#
#    @file   tvout_test_usercase.sh
#
#    @brief  shell script to test the TVout user case function block.
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
#Hake.Huang/-----             08/01/2008     N/A          Initial version
# 
###################################################################################################
#!/bin/sh


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
export TST_TOTAL=2

export TCID="setup"
export TST_COUNT=0
RC=0

if [ -e $LTPROOT ]
then
export LTPSET=0
else
export LTPSET=1
fi

trap "cleanup" 0

#switch to main screen
echo U:480x640p-67 > /sys/class/graphics/fb0/mode
#echo U:640x480p-50 > /sys/class/graphics/fb0/mode
#wake up the system
echo -e "\033[9;0]" > /dev/tty0 

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

echo U:480x640p-67 > /sys/class/graphics/fb0/mode

return $RC
}

# Function:     test_tvout_usercase_01
# Description   - Test the TVout module functionality
#                 of back from system suspend
#                 need third part mplayer program and a video stream
test_tvout_usercase_01()
{
TCID="test_tvout_usercase_01"
TST_COUNT=1
RC=0

echo U:640x480p-60 > /sys/class/graphics/fb0/mode

#print test info
tst_resm TINFO "test #1: tvout_usercase 01"

echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
echo "press resume key ..."
echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"

echo standby > /sys/power/state

lcd_testapp_power -F /dev/fb0

read -p "Did the TV display normal y/n?" RC

if [ $RC == 'y' ]
then
RC=0
return $RC
fi
RC=1
return $TST_COUNT
}

# Function:     test_tvout_usercase_02
# Description   - Test the TVout module functionality
#                 power test
#                 need third part mplayer program and a video stream
test_tvout_usercase_02()
{
TCID="test_tvout_usercase_02"
TST_COUNT=1
RC=0

echo U:640x480p-60 > /sys/class/graphics/fb0/mode

MODE=$(cat /sys/class/graphics/fb0/mode)

if [ $MODE != "U:640x480p-60" ]
then 
RC=1
return $TST_COUNT
fi

echo U:640x480p-50 > /sys/class/graphics/fb0/mode

MODE=$(cat /sys/class/graphics/fb0/mode)

if [ $MODE != "U:640x480p-50" ]
then
RC=1
return $TST_COUNT
fi

return $RC
}


RC=0

setup || exit $RC

if [ $# -ne 1 ]
then
echo "usage $0 <1/2>"
exit 1 
fi

case "$1" in
1)
  test_tvout_usercase_01 || exit $RC 
    ;;
2)
  test_tvout_usercase_02 || exit $RC
   ;;
*)
  echo "usage $0 <1/2>"
  ;;
esac

tst_resm TINFO "Test PASS"





