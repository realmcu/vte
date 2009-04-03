 ###################################################################################################
#
#    @file   mbx_test_stress.sh
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


# Function:     test_mbx_stress
# Description   - stress test 
#  
test_mbx_stress_01()
{
TCID="test_mbx_stress_01"
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test #1: stress test 1"

echo "press q to quit stree test"

OGLESLighting

read -p "is the displaying still OK? y/n" RC

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

setup || exit $RC

test_mbx_stress_01 || exit $RC

tst_resm TINFO "test #1: PASS"
