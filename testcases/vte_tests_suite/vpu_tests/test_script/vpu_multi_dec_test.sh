#!/bin/sh
###################################################################################################
#
#    @file  vpu_multi_test.sh
#
#    @brief  shell script for testcase design for VPU multi decode and/or encode
#
###################################################################################################
#
#   Copyright (C) 2004-2008, Freescale Semiconductor, Inc. All Rights Reserved
#   THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#   BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#   Freescale Semiconductor, Inc.
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

vpu_dec_test.sh 3 >> .temp_dec &
vpu_dec_test.sh 4 >> .temp_dec || return 1

vpu_dec_test.sh 10 >> .temp_dec &

vpu_dec_test.sh 7 >> .temp_dec &
vpu_dec_test.sh 8 >> .temp_dec || return 1

wait

CT=$(grep "Test PASS" .temp_dec | wc -l)

if [ $CT = "7" ]
then
RC=0
fi

rm -f .temp_dec

return $RC

}

#TODO check parameter
if [ $# -ne 1 ]
then
echo "usage $0 <1>"
exit 1
fi

setup || exit $RC

case "$1" in
1)
  test_case_01 || exit $RC
  ;;
*)
#TODO check parameter
  echo "usage $0 <1>"
  ;;
esac

tst_resm TINFO "Test PASS"







