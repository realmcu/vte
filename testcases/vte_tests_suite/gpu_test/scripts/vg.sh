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
#    @file   vg.sh
#
#    @brief  shell script template for testcase design "gpu" is where to modify block.
#
################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#------------------------   ------------    ----------  -----------------------
#Hake Huang/-----             20110210     N/A          Initial version
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

modprobe gpu
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
# Description   - Test if gles applications sequence ok
#  
test_case_01()
{
#TODO give TCID 
TCID="vg_test"
#TODO give TST_COUNT
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
echo "==========================="
echo tiger
echo "==========================="
tiger &
td=$!
sleep 100
kill -9 $td

cd ${TEST_DIR}/${APP_SUB_DIR}
echo "==========================="
echo vgMark
echo "==========================="
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

return $RC

}

# Function:     test_case_02
# Description   - Test if gles concurrent ok
#  
test_case_02()
{
#TODO give TCID 
TCID="vg_multi_test"
#TODO give TST_COUNT
TST_COUNT=2
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
tiger &
td=$!

cd ${TEST_DIR}/${APP_SUB_DIR}
if [ -e openVG/VGMark_10_src/run/vgmark/run  ];then
cd openVG/VGMark_10_src/run/vgmark
./run || RC="vgmark"
fi

kill -9 $td

echo $RC

if [ "$RC" = "0" ]; then
 RC=0
else
  RC=1
fi

return $RC
}

# Function:     test_case_03
# Description   - Test if gles conformance ok
#  
test_case_03()
{
#TODO give TCID 
TCID="vg_conform_test"
#TODO give TST_COUNT
TST_COUNT=3
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
cd ${TEST_DIR}/${APP_SUB_DIR}
echo "==========================="
echo vg1.1 conformance
echo "==========================="
if [ -e openVG/cts_1.1/generation/make/linux/bin/generator ]; then
	cd openVG/cts_1.1/generation/make/linux/bin/
	./generator || RC="cts_1.1"
fi

cd ${TEST_DIR}/${APP_SUB_DIR}
echo "==========================="
echo vg1.0.1 conformance
echo "==========================="
if [ -e openVG/cts_1.0.1/generation/make/linux/bin/generator ]; then
	cd openVG/cts_1.0.1/generation/make/linux/bin
	./generator || RC=$(echo $RC cts_1.0.1)
fi

echo $RC

if [ $RC = "0" ]; then
 RC=0
else
  RC=1
fi

return $RC
}

test_case_04()
{
#TODO give TCID 
TCID="gles_pm_test"
#TODO give TST_COUNT
TST_COUNT=3
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

tiger &
td=$!

rtc_testapp_6 -T 5
sleep 1
rtc_testapp_6 -T 5
sleep 1
rtc_testapp_6 -T 5
sleep 1
rtc_testapp_6 -T 5
sleep 1
rtc_testapp_6 -T 5
sleep 1

kill -9 $td

echo "test PASS"

return $RC
}
usage()
{
echo "$0 [case ID]"
echo "1: sequence test"
echo "2: concurrent test"
echo "3: conformance test"
echo "4: pm test"
}

# main function

RC=0

#TODO check parameter
if [ $# -ne 1 ]
then
usage
exit 1 
fi

TEST_DIR=/mnt/nfs/util/Graphics/

APP_SUB_DIR=

setup || exit $RC
#judge rootfs type
rt="Ubuntu"
cat /etc/issue | grep Ubuntu || rt="others"

if [ $rt = "Ubuntu" ];then
APP_SUB_DIR="ubuntu_10.10/test"
export DISPLAY=:0.0
else
#judge the rootfs
platfm.sh
case "$?" in
50)
  APP_SUB_DIR="imx50_rootfs/test"
	;;
53)
  APP_SUB_DIR="imx53_rootfs/test"
 ;;
*)
  exit 0
  ;;
esac
fi


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
*)
  usage
  ;;
esac

tst_resm TINFO "Test Finish"
