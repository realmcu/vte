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
#    @file   gles_test.sh
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
TCID="gles_test"
#TODO give TST_COUNT
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
cd ${TEST_DIR}/${APP_SUB_DIR}
echo "==========================="
echo 3DMark
echo "==========================="
if [ -e 3DMarkMobile/bin/bin/fsl_imx_linux/fm_oes_player ]; then
  cd 3DMarkMobile/bin/bin/fsl_imx_linux/
	./fm_oes_player || RC="3Dmark"
fi

cd ${TEST_DIR}/${APP_SUB_DIR}
echo "==========================="
echo egl_test
echo "==========================="
if [ -e openGLES/egl_test/linux/egl_test ]; then
  cd openGLES/egl_test/linux/
	./egl_test || RC=$(echo $RC egl_test)
fi


cd ${TEST_DIR}/${APP_SUB_DIR}
echo "==========================="
echo bbPinball
echo "==========================="
if [ -e bbPinball/bbPinball ]; then
  cd bbPinball
	./bbPinball || RC=$(echo $RC bbPinBall)
fi

cd ${TEST_DIR}/${APP_SUB_DIR}
echo "==========================="
echo fps triangle
echo "==========================="
if [ -e openGLES/opengles20/fps_triangle/linux/fps_triangle ]; then
  cd openGLES/opengles20/fps_triangle/linux/
	./fps_triangle || RC=$(echo $RC fps_triangle)
fi

echo "==========================="
echo simple draw
echo "==========================="
simple_draw 100 || RC=$(echo $RC simple draw)
simple_draw 100 -s || RC=$(echo $RC simple draw -s)

cd ${TEST_DIR}/${APP_SUB_DIR}
echo "==========================="
echo simple triangle
echo "==========================="
if [ -e openGLES/opengles20/simple_triangle/linux/simple_triangle ]; then
  cd openGLES/opengles20/simple_triangle/linux
	./simple_triangle || RC=$(echo $RC simple_triangle)
fi

cd ${TEST_DIR}/${APP_SUB_DIR}
echo "==========================="
echo torusknot
echo "==========================="
if [ -e openGLES/opengles20/torusknot/linux/torusknot ]; then
  cd openGLES/opengles20/torusknot/linux
	./torusknot || RC=$(echo $RC torusknot)
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
TCID="gles_con_test"
#TODO give TST_COUNT
TST_COUNT=2
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
cd ${TEST_DIR}/${APP_SUB_DIR}
echo "==========================="
echo 3DMark
echo "==========================="
if [ -e 3DMarkMobile/bin/bin/fsl_imx_linux/fm_oes_player ]; then
  cd 3DMarkMobile/bin/bin/fsl_imx_linux/
	./fm_oes_player &
fi

cd ${TEST_DIR}/${APP_SUB_DIR}
echo "==========================="
echo egl test
echo "==========================="
if [ -e openGLES/egl_test/linux/egl_test ]; then
  cd openGLES/egl_test/linux/
	./egl_test &
fi


cd ${TEST_DIR}/${APP_SUB_DIR}
echo "==========================="
echo bbPinball
echo "==========================="
if [ -e bbPinball/bbPinball ]; then
  cd bbPinball
	./bbPinball &
fi

cd ${TEST_DIR}/${APP_SUB_DIR}
echo "==========================="
echo fps triangle
echo "==========================="
if [ -e openGLES/opengles20/fps_triangle/linux/fps_triangle ]; then
  cd openGLES/opengles20/fps_triangle/linux/
	./fps_triangle &
fi

echo "==========================="
echo simple draw
echo "==========================="
	simple_draw 1000 &
	simple_draw 1000 -s &

cd ${TEST_DIR}/${APP_SUB_DIR}
echo "==========================="
echo simple triangle
echo "==========================="
if [ -e openGLES/opengles20/simple_triangle/linux/simple_triangle ]; then
  cd openGLES/opengles20/simple_triangle/linux
	./simple_triangle &
fi

wait

cd ${TEST_DIR}/${APP_SUB_DIR}
echo "==========================="
echo torusknot
echo "==========================="
if [ -e openGLES/opengles20/torusknot/linux/torusknot ]; then
  cd openGLES/opengles20/torusknot/linux
	./torusknot
fi


if [ $? -eq 0 ]; then
echo "TEST PASS"
else
RC=1
echo "TEST FAIL"
fi
return $RC
}

# Function:     test_case_03
# Description   - Test if gles conformance ok
#  
test_case_03()
{
#TODO give TCID 
TCID="gles_conform_test"
#TODO give TST_COUNT
TST_COUNT=3
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
cd ${TEST_DIR}/${APP_SUB_DIR}
echo "==========================="
echo es11 conformance
echo "==========================="
if [ -e openGLES/conformance/es11/conform/run_conformance_tests.sh ]; then
  cd openGLES/conformance/es11/conform/
	./run_conformance_tests.sh All || RC=$(echo $RC es10_conformance)
fi

cd ${TEST_DIR}/${APP_SUB_DIR}
echo "==========================="
echo es20 conformance
echo "==========================="
if [ -e openGLES/conformance/es20/GTF_ES/glsl/GTF/GTF ]; then
  cd openGLES/conformance/es20/GTF_ES/glsl
	./GTF/GTF -width=64 -height=64 -noimagefileio \
	-l=/root/es20_conformance_mustpass_64x64 -run="$(pwd)/GTF/mustpass.run" \
	&& ./GTF/GTF -width=113 -height=47 -noimagefileio \
	-l=/root/es20_conformance_mustpass_113x47 -run="$(pwd)/GTF/mustpass.run" \
	&& ./GTF/GTF -width=480 -height=640 -noimagefileio \
	-l=/root/es20_conformance_mustpass_480x640 -run="$(pwd)/GTF/mustpass.run" \
	|| RC=$(echo $RC es20_conformance)
fi

echo $RC

if [ "$RC" = "0" ]; then
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
TST_COUNT=4
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

cd ${TEST_DIR}/${APP_SUB_DIR}
if [ -e openGLES/opengles20/simple_draw/linux/simple_draw ]; then
  cd openGLES/opengles20/simple_draw/linux
	./simple_draw 10000 &
fi

rtc_testapp_6 -T 10
sleep 1
rtc_testapp_6 -T 10
sleep 1
rtc_testapp_6 -T 10
sleep 1
rtc_testapp_6 -T 10
sleep 1
rtc_testapp_6 -T 10
sleep 1

echo "TEST PASS"
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
