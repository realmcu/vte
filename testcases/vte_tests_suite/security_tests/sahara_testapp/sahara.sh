#!/bin/sh
#
# Copyright (C) 2011 Freescale Semiconductor, Inc. All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
###############################################################################
#
#    @file   sahara.sh
#
#    @brief  shell script template for testcase design "sahara"
#
################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#------------------------   ------------    ----------  -----------------------
#Hake Huang/-----             20110214     N/A          Initial version
# 
################################################################################

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
insmod $LTPROOT/testcases/bin/sahara_module.ko

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
# Description   - Test if sahara ok
#  
test_case_01()
{
#TODO give TCID 
TCID="sahara_test"
#TODO give TST_COUNT
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
tiger &
td=$!
sleep 100
kill -9 $td

cd ${TEST_DIR}/${APP_SUB_DIR}
if [ -e openVG/VGMark_10_src/run/vgmark/run  ];then
cd openVG/VGMark_10_src/run/vgmark
./run || RC="vgmark"
fi

echo $RC

if [ "$RC" = "0" ]; then
 RC=0
else
  RC=1
fi

echo "system reboot in 10 s"

sh -c "sleep 5;reboot" &

return $RC
}

usage()
{
echo "$0 [case ID]"
echo "1: sahara test all"
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
*)
  usage
  ;;
esac

tst_resm TINFO "Test Finish"
