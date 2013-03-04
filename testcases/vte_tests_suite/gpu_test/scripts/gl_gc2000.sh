#!/bin/sh
###############################################################################
# Copyright (C) 2011,2012 Freescale Semiconductor, Inc. All Rights Reserved.
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
#    @file   gl_2000.sh
#
#    @brief  shell script template for testcase design "gpu" is where to modify block.
#
################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#------------------------   ------------    ----------  -----------------------
#Shelly Cheng/-----             20121218       N/A          Initial version
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
	export TST_TOTAL=2

	export TCID="setup"
	export TST_COUNT=0
	RC=0

	trap "cleanup" 0
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
# Description   - Test GLMark demo
#  
test_case_01()
{
	#TODO give TCID 
	TCID="glmark_test"
	#TODO give TST_COUNT
	TST_COUNT=1
	RC=0

	#print test info
	tst_resm TINFO "test $TST_COUNT: $TCID "

	#TODO add function test scripte here
	cd ${TEST_DIR}/${APP_SUB_DIR}
	echo "==========================="
	echo GLMark Demo
	echo "==========================="
	if [ -e GLMark ]; then
		cd GLMark/
		./glmark || RC="GLMark"
	fi
	if [ "$RC" = "0" ]; then
		RC=0
	else
		RC=1
	fi

	return $RC

}

# Function:     test_case_02
# Description   - Test if GLXS demo ok
#  
test_case_02()
{
	#TODO give TCID 
	TCID="glxs_test"
	#TODO give TST_COUNT
	TST_COUNT=2
	RC=0

	#print test info
	tst_resm TINFO "test $TST_COUNT: $TCID "

	#TODO add function test scripte here


	cd ${TEST_DIR}/${APP_SUB_DIR}
	echo "==========================="
	echo glxs demo
	echo "==========================="
	if [ -e GLXS ]; then
		cd GLXS/
		./glxs &
	fi
	pid_glxs=$!
	sleep 40
	kill  $pid_glxs
	RC=$?

	if [ $RC -eq 0 ]; then
		echo "TEST PASS"
	else
		RC=1
		echo "TEST FAIL"
	fi
	return $RC
}


usage()
{
	echo "$0 [case ID]"
	echo "1: GLMark demo"
	echo "2: GLXS demo"
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
APP_SUB_DIR="ubuntu_11.10/test"
export DISPLAY=:0.0
export XAUTHORITY=/home/linaro/.Xauthority
setup || exit $RC
case "$1" in
	1)
		test_case_01 || exit $RC 
		;;
	2)
		test_case_02 || exit $RC
		;;
	*)
		usage
		;;
esac

tst_resm TINFO "Test Finish"
