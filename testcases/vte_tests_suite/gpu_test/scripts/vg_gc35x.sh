#!/bin/sh
#
# Copyright (C) 2011, 2013 Freescale Semiconductor, Inc. All Rights Reserved.
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
#Hake Huang/-----             20110817     N/A          Initial version
#Shelly Cheng                 20130517     N/A          change concurrent demo
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
	chip=$(platfm.sh)
	if [ $chip = "IMX6Sololite-ARM2" ];then
		modprobe  galcore baseAddress=0x80000000 
	fi

	if [ -z "$GPU_DRIVER_PATH" ];then
		export GPU_DRIVER_PATH=/usr/lib
	fi
	mv $GPU_DRIVER_PATH/libOpenVG.so $GPU_DRIVER_PATH/libOpenVG.so.bak
	ln -s $GPU_DRIVER_PATH/libOpenVG_355.so $GPU_DRIVER_PATH/libOpenVG.so
	echo ====== Using 355 VG library =======    
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
	rm $GPU_DRIVER_PATH/libOpenVG.so
	mv $GPU_DRIVER_PATH/libOpenVG.so.bak $GPU_DRIVER_PATH/libOpenVG.so

	if [ -z "$NOCLEANUP" ];then
		if [ $chip = "IMX6Sololite-ARM2" ];then
		modprobe -r galcore
    	fi
	fi
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

	cd ${TEST_DIR}/${APP_SUB_DIR}
	#TODO add function test scripte here
	echo "==========================="
	echo tiger
	echo "==========================="
	./tiger_vg -rgba 5650 -frameCount 1000 || RC="tiger"

	cd ${TEST_DIR}/${APP_SUB_DIR}
	echo "==========================="
	echo sample_testvg
	echo "==========================="
	./sample_testvg355 1000 || RC=$(echo $RC sample_testvg)


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
    cd ${TEST_DIR}/${APP_SUB_DIR}
    #TODO add function test script here
    echo "==========================="
    echo tiger
    echo "==========================="
    ./tiger_vg  -rgba 5650 -frameCount 10000 &
    pid_tiger=$!
    
    cd ${TEST_DIR}/${APP_SUB_DIR}
    echo "==========================="
    echo sample_testvg
    echo "==========================="
    ./sample_testvg355 10000 &
    
    wait $pid_tiger 
    RC=$?
    wait

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
    if [ -e vg11_conform/generation/make/linux/bin ]; then
        cd vg11_conform/generation/make/linux/bin
        ./generator.exe || RC="cts_1.1"
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
    cd ${TEST_DIR}/${APP_SUB_DIR} 
    ./tiger_vg  -rgba 5650 -frameCount 50000 &
    td=$!
    sleep 1
    rtc_testapp_6 -T 50
    sleep 1
    rtc_testapp_6 -T 50
    sleep 1
    rtc_testapp_6 -T 50
    sleep 1
    rtc_testapp_6 -T 50
    sleep 1
    rtc_testapp_6 -T 50
    sleep 1
    kill -9 $td
    echo "test PASS"
    return $RC
}
# Function:     test_case_05
# Description   - 2D performance est
test_case_05()
{
    #TODO give TCID
    TCID="gpu_2d_performance"
    #TODO give TST_COUNT
    TST_COUNT=5
    RC=0
    cd ${TEST_DIR}/${APP_SUB_DIR}
    echo "==========================="
    echo 2dperformance
    echo "==========================="
    ./2dperf
    RC=$?
    if [ $RC -eq 0 ]; then
         echo "TEST PASS"
    else
         echo "TEST FAIL"
    fi
    return $RC
}

test_case_06()
{
    #TODO give TCID 
    TCID="vgmark_test"
    #TODO give TST_COUNT
    TST_COUNT=1
    RC=0

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "

    cd ${TEST_DIR}/${APP_SUB_DIR}
    echo "==========================="
    echo vgMark
    echo "==========================="
    cd VGMark_gc355
    ./fm_oes_vg_player_gc355 || RC=$(echo $RC vgmark)
    echo $RC

    if [ "$RC" = "0" ]; then
        RC=0
    else
        RC=1
    fi

    return $RC

}
test_case_07()
{
	    #TODO give TCID
		TCID="multibuffer_test"
		#TODO give TST_COUNT
		TST_COUNT=1
		RC=0
		cd ${TEST_DIR}/${APP_SUB_DIR}
		echo "==========================="
		echo fb multiple buffer test
		echo "==========================="
		export FB_MULTI_BUFFER=1
		echo FB_MULTI_BUFFER=1
		./tiger_vg  -rgba 5650 -frameCount 300 
		export FB_MULTI_BUFFER=2
		echo FB_MULTI_BUFFER=2
		./tiger_vg  -rgba 5650 -frameCount 300
		export FB_MULTI_BUFFER=3
		echo FB_MULTI_BUFFER=3
		./tiger_vg  -rgba 5650 -frameCount 300
		export FB_MULTI_BUFFER=2
		echo FB_MULTI_BUFFER=2
		./tiger_vg  -rgba 5650 -frameCount 300
		export FB_MULTI_BUFFER=1
		echo FB_MULTI_BUFFER=1
		./tiger_vg  -rgba 5650 -frameCount 300
		export FB_MULTI_BUFFER=0
		echo FB_MULTI_BUFFER=0
		./tiger_vg  -rgba 5650 -frameCount 300
		export FB_FRAMEBUFFER_0=/dev/fb1
		echo 0 > /sys/class/graphics/fb1/blank
		./tiger_vg  -rgba 5650 -frameCount 300
		echo 1 > /sys/class/graphics/fb1/blank
		echo 0 > /sys/class/graphics/fb0/blank
		export FB_FRAMEBUFFER_0=/dev/fb0
		RC=$?
		if [ $RC -eq 0 ]; then
			echo "TEST PASS"
		else
			echo "TEST FAIL"
	    fi
        return $RC
}
usage()
{
    echo "$0 [case ID]"
    echo "1: sequence test"
    echo "2: concurrent test"
    echo "3: conformance test"
    echo "4: pm test"
    echo "5: 2dperf test"
    echo "6: vgmark test"
    echo "7: FB MULTIBUFFER test"
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
41)
  APP_SUB_DIR="imx51_rootfs/test"
  ;;
51)
  APP_SUB_DIR="imx51_rootfs/test"
 ;;
53)
  APP_SUB_DIR="imx53_rootfs/test"
 ;;
61)
  APP_SUB_DIR="imx61_rootfs/test"
 ;;
63)
  APP_SUB_DIR="imx61_rootfs/test"
 ;;
60)
  APP_SUB_DIR="imx61_rootfs/test"
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
5)
    test_case_05 || exit $RC
    ;;
6)
    test_case_06 || exit $RC
    ;;
7)
    test_case_07 || exit $RC
    ;;

*)
    usage
    ;;
esac

tst_resm TINFO "Test Finish"
