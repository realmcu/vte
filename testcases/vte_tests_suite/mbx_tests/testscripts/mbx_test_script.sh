#Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
###################################################################################################
#
#    @file   mbx_test_script.sh
#
#    @brief  shell script to test the MBx 3rd party function block.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#Hake.Huang/-----             07/11/2008     N/A          Initial version
# 
###################################################################################################
#!/bin/sh

#source script_template_header.sh

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
export TST_TOTAL=4

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
return $RC
}

# Function:     test_mbx_driver_01
# Description   - Test if service is ok
#  
test_mbx_driver_01()
{
TCID="test_mbx_driver_01"
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test #1: driver test"

#EXIST=$(which services_test 2>&1 | awk '{print $2}' )
#if [ $EXIST ]
#then
#tst_resm TINFO "services_test not exist"
#RC=1
#return $TST_COUNT 
#fi

Rnt=$(services_test | grep FAIL| awk '{print $1}')

if [ -z $Rnt ]
then
return $RC
fi

RC=1
return $TST_COUNT
}


# Function:     test_mbx_driver_02
# Description   - Test if driver is ok
#  
test_mbx_driver_02()
{
TCID="test_mbx_driver_02"
TST_COUNT=2
RC=0

#print test info
tst_resm TINFO "test #2: driver test 2"

#EXIST=$(which power_test 2>&1 | awk '{print $2}' )
#if [ $EXIST ]
#then
#$tst_resm TINFO "power_test not exist"
#RC=1
#return $TST_COUNT 
#fi

if [ -z $TMP ]
then
tst_resm TINFO "warning TEMP DIR does not exit"
TMP=/tmp
fi

rm -rf $TMP/out_mbx
power_test > $TMP/out_mbx &

sleep 5

killall power_test

sleep 1

if [ -e $TMP/out_mbx ]
then
Rnt= $(grep "FAIL" $TMP/out_mbx )
else
tst_resm TINFO "can not write to temp file"
RC=1
return $TST_COUNT  
fi

if [ -z $Rnt ]
then
return $RC
fi

RC=1
return $TST_COUNT
}

# Function:     test_mbx_egl_01
# Description   - Test if gl es function is ok
#  
test_mbx_egl_01()
{
TCID="test_mbx_egl_01"
TST_COUNT=3
RC=0

#print test info
tst_resm TINFO "test #3: egl test 1"

#EXIST=$(which egl_test 2>&1 | awk '{print $2}' )
#if [ $EXIST ]
#then
#$tst_resm TINFO "egl_test not exist"
#RC=1
#return $TST_COUNT 
#fi

Rnt=$(egl_test 1000 | grep error)

if [ -z $Rnt ]
then
read -p "Did you see the 2 triangle y/n?" Rnt
else
RC=1
return $TST_COUNT
fi

if [ $Rnt == 'y' ]
then
return $RC
fi
RC=1
return $TST_COUNT
}

# Function:     test_mbx_egl_02
# Description   - Test if egl function is ok
#  
test_mbx_egl_02()
{
TCID="test_mbx_egl_02"
TST_COUNT=4
RC=0

#print test info
tst_resm TINFO "test #4: gl test 2"

#EXIST=$(which glinfo 2>&1 | awk '{print $2}' )
#if [ $EXIST ]
#then
#$tst_resm TINFO "glinfo not exist"
#RC=1
#return $TST_COUNT 
#fi

glinfo

sleep 1

if [ $? -eq 0 ]
then
return $RC
else
RC=1
return $TST_COUNT
fi
}

# main function

RC=0

if [ $# -ne 1 ]
then
echo "usage $0 <A/M>"
exit 1 
fi

setup || exit $RC

if [ $1 == "A" ]
then
#auto test set
test_mbx_driver_01 || exit $RC 
test_mbx_driver_02 || exit $RC
test_mbx_egl_02    || exit $RC
else
#AM test set
test_mbx_egl_01 || exit $RC
fi

tst_resm TINFO "Test PASS"





