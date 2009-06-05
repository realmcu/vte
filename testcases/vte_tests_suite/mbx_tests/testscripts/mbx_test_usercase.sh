###################################################################################################
#
#    @file   mbx_test_usercase.sh
#
#    @brief  shell script to test the MBx 3rd party function block.
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
#Hake.Huang/-----             07/28/2008     N/A          Initial version
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
export TST_TOTAL=3

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

if [ -e /etc/rc.d/init.d/rc.pvr ]
then
tst_resm TINFO "MBx script installed"
else
tst_resm TINFO "MBx initial script not found on /etc/rc.d/init.d"
RC=1
return $RC 
fi

echo U:480x640p-67 > /sys/class/graphics/fb0/mode

/etc/rc.d/init.d/rc.pvr start

if [ $? -ne 0 ]
then
tst_resm TINFO " MBx init failed!"
RC=2
return $RC
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
/etc/rc.d/init.d/rc.pvr stop

echo U:480x640p-67 > /sys/class/graphics/fb0/mode

return $RC
}

# Function:     test_mbx_usercase_01
# Description   - Test the MBX module functionality
#                 after resuming from power saving
test_mbx_usercase_01()
{
TCID="test_mbx_usercase_01"
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test #1: mbx_usercase 01"

echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
echo "press resume key ..."
echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"

echo standby > /sys/power/state

sleep 1

setup

egl_test 1000

read -p "Did the TV display normal y/n?" RC

if [ $RC == 'y' ]
then
RC=0
return $RC
fi
RC=1
return $TST_COUNT
}


# Function:     test_mbx_usercase_02
# Description   - Test the MBX module functionality
#                 switch to TVout channel while playing 
#  
test_mbx_usercase_02()
{
TCID="test_mbx_usercase_02"
TST_COUNT=2
RC=0

#print test info
tst_resm TINFO "test #2: usercase test 2"

egl_test 1000 &

sleep 3
 
echo U:640x480p-60 > /sys/class/graphics/fb0/mode

read -p "Did the TV display normal y/n?" RC

#wait the egl_test to end
wait $!

if [ $RC == 'y' ]
then
RC=0
return $RC
fi
RC=1
return $TST_COUNT
}

# Function:     test_mbx_usercase_03
# Description   - Test the MBX module functionality
#                 when diaplaying in TVout
#  
test_mbx_usercase_03()
{
TCID="test_mbx_usercase_03"
TST_COUNT=3
RC=0

#print test info
tst_resm TINFO "test #3: usercase 03"

cleanup

echo U:640x480p-60 > /sys/class/graphics/fb0/mode 


#reinitial the MBX module 
/etc/rc.d/init.d/rc.pvr start

egl_test 1000

read -p "did the TVout play normally? y/n" RC

if [ $RC == 'y' ]
then
RC=0
return $RC
fi
RC=1
return $TST_COUNT
}

# Function:     test_mbx_usercase_04
# Description   - Test the MBX module functionality
#                 in TVout->play->powersave->resume->play
#  
test_mbx_usercase_04()
{
TCID="test_mbx_usercase_04"
TST_COUNT=4
RC=0

#print test info
tst_resm TINFO "test #3: usercase 04"

#enable TV out 
echo U:640x480p-60 > /sys/class/graphics/fb0/mode 

#display the 3D image
egl_test 1000 &

sleep 1

echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
echo "press resume key in 10s..."
echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"

echo standby > /sys/power/state

sleep 1

#display the 3D image
egl_test 100

read -p "Is there any displaying on LCD/TVout y/n?" RC


if [ $RC == 'y' ]
then
RC=0
return $RC
fi
RC=1
return $TST_COUNT
}


# main function

RC=0

if [ $# -ne 1 ]
then
echo "usage $0 <1/2/3>"
exit 1 
fi

setup || exit $RC

case "$1" in
1)
  test_mbx_usercase_01 || exit $RC 
  ;;
2)
  test_mbx_usercase_02 || exit $RC
  ;;
3)
  test_mbx_usercase_03 || exit $RC
  ;;
4)
  test_mbx_usercase_04 || exit $RC
  ;;
*)
  echo "usage $0 <1/2/3/4>"
  ;;
esac

tst_resm TINFO "Test PASS"





