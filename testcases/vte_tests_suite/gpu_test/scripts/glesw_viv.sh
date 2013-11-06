#!/bin/sh
###############################################################################
# Copyright (C) 2013 Freescale Semiconductor, Inc. All Rights Reserved.
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
#    @file   glesw_test.sh
#
#    @brief  shell script template for testcase design "gpu" is where to modify block.
#
################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#------------------------   ------------    ----------  -----------------------
#Shelly Cheng              20131106     N/A          Initial version
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
# Description   - Test if egl applications sequence ok
#  
test_case_01()
{
    #TODO give TCID 
    TCID="GLES_TEST"
    #TODO give TST_COUNT
    TST_COUNT=1
    RC=0
    simple-egl -f &
    pid_egl=$!
	sleep 5
	kill $pid_egl
	
	simple-shm&
	pid_shm=$!
	sleep 5
	kill $pid_shm
	
	return $RC

}

# Function:     test_case_02
# Description   - Test if egl and vg concurrent ok
#  
test_case_02()
{
	#TODO give TCID 
	TCID="GLES_CON_TEST"
	#TODO give TST_COUNT
	TST_COUNT=2
	RC=0

	#print test info
	tst_resm TINFO "test $TST_COUNT: $TCID "

	#TODO add function test scripte here
	echo "==========================="
	echo simple-egl
	echo "==========================="
	simple-egl &
	pid_egl=$!

	echo "==========================="
	echo simple_shm
	echo "==========================="
	simple-shm & 
	pid_shm=$!

    echo "==========================="
    echo tiger
    echo "==========================="
    cd /opt/viv_samples/tiger/
	./tiger -frameCount 100 &
	pid_tiger=$!

	wait $pid_tiger
	sleep 5
	kill $pid_egl
	kill $pid_shm

	RC=$?
	wait
    if [ $RC -eq 0 ]; then
        echo "TEST PASS"
    else
        RC=1
        echo "TEST FAIL"
    fi
    return $RC
}


test_case_03()
{
    #TODO give TCID 
    TCID="WAYLAND_PM_TEST"
    #TODO give TST_COUNT
    TST_COUNT=3
    RC=0

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "

    cd /opt/viv_samples/tiger/
    ./tiger -frameCount 1000 &
	pid_tiger=$!

    simple-egl&
	pid_egl=$!

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
	wait $pid_tiger
    sleep 5
	kill $pid_egl
	RC=$?

    if [ $RC -eq 0 ];then
        echo "TEST PASS"
    else
        echo "TEST FAIL"
    fi
    return $RC
}

test_case_04()
{
    #TODO give TCID 
    TCID="WAYLAND_PERF_TEST"
    #TODO give TST_COUNT
    TST_COUNT=4
    RC=0

	#print test info
	tst_resm TINFO "test $TST_COUNT: $TCID "

	cd ${TEST_DIR}/${APP_SUB_DIR}
	echo "==========================="
	echo "3DMark mm06 test"
	echo "==========================="

	if [ -e mm06/fsl_imx_linux ]; then
		cd mm06/fsl_imx_linux
		./fm_oes_player
	fi
	cd ${TEST_DIR}/${APP_SUB_DIR}
	echo "==========================="
    echo "3Dmark20 mm07 test"
    echo "==========================="
    if [ -e mm07 ]; then
        cd mm07
        ./fm_oes2_mobile_player
    fi

	return 0
}


usage()
{
    echo "$0 [case ID]"
	echo "the script is for wayland test"
    echo "1: sequence test"
    echo "2: concurrent test"
    echo "3: pm test"
    echo "4: performance test"
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

setup || exit $RC
#judge rootfs type
APP_SUB_DIR=wayland

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
