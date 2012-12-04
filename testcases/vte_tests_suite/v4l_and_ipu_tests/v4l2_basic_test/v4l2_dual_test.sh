#!/bin/bash -x
#Copyright (C)  2012 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
###################################################################################################
#
#    @file   v4l_dual_test.sh
#
#    @brief  shell script for v4l2 dual camera capture test.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  ---------------------------------------
#Shelly Cheng                 20120614        N/A         Initial
#Andy Tian	                  20120715        N/A         add some modes and fix fps 15 setting error
#Andy Tian	                  20121204        N/A         Fix some syntax error and add wait command
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
	RC=0
	OV=0

	trap "cleanup" 0

	#TODO add setup scripts
	echo -e "\033[9;0]" > /dev/tty0
	#setup the fb on
	echo 0 > /sys/class/graphics/fb0/blank
	TSTCMD=v4l_capture_testapp
	v4l_module.sh setup
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

	v4l_module.sh cleanup
	return $RC
}

# Function:     test_case_01
# Description   - Test if 30 fps capture mode input/output size is ok
#
test_case_01()
{
	#TODO give TCID
	TCID="capture_mode"
	#TODO give TST_COUNT
	TST_COUNT=1
	RC=0

	#print test info
	tst_resm TINFO "test $TST_COUNT: $TCID "
	FBX=$(fbset | grep geometry | awk '{print $2}')
	FBY=$(fbset | grep geometry | awk '{print $3}')

	#TODO add function test scripte here
	CMD_Mode=v4l_modes
	MIPIMODES30FPS=`$CMD_Mode -D /dev/video1`
	MODES564230FPS=`$CMD_Mode -D /dev/video0`
	echo "farme rate 30"
	for m in $MIPIMODES30FPS
	do
		for n in $MODES564230FPS
		do
		    echo "--------------current 5642 mode is: $n"
			echo "--------------current 5640 mode is: $m"
			${TSTCMD} -T 100 -C 2 -s CSI_MEM -D /dev/video0 -O YUV420 -M $n -u /dev/fb2  & 
			pid=$!
			${TSTCMD} -T 100 -C 2 -s CSI_IC_MEM -O YUV420 -D /dev/video1 -M $m || RC=$(expr $RC + 1)
			wait $pid || RC=$(expr $RC + 1)
		done
	done
	return $RC
}
# Function:     test_case_02
# Description   - Test if 15 fps dual capture mode input/output size is ok
#
test_case_02()
{
	#TODO give TCID
	TCID="capture_mode"
	#TODO give TST_COUNT
	TST_COUNT=1
	RC=0

	#print test info
	tst_resm TINFO "test $TST_COUNT: $TCID "
	FBX=$(fbset | grep geometry | awk '{print $2}')
	FBY=$(fbset | grep geometry | awk '{print $3}')

	#TODO add function test scripte here
	MIPIMODES15FPS=`v4l_modes -D /dev/video1 -r 15`
	MODES564215FPS=`v4l_modes -D /dev/video0 -r 15`
	echo "farme rate 15"
	for m in $MIPIMODES15FPS
	do
		for n in $MODES564215FPS
		do
			echo "--------------current 5642 mode is: $n"
			echo "--------------current 5640 mode is: $m"
			${TSTCMD} -T 100 -C 2 -s CSI_MEM -D /dev/video0 -O YUV420 -M $n -u /dev/fb2 -r 15  & 
			pid=$!
			${TSTCMD} -T 100 -C 2 -s CSI_IC_MEM -O YUV420 -D /dev/video1 -M $m -r 15 ||RC=$(expr $RC + 1)
			wait $pid || RC=$(expr $RC + 1)
		done
	done
	return $RC
}
usage()
{
	echo "$0 [case ID]"
	echo "1: 30fps test on dual camera"
	echo "2: 15fps test on dual camera"
}

# main function

RC=0


#TODO check parameter
usage()

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
	2)
		test_case_02 || exit $RC
		;;
	*)
		usage
		;;
esac

tst_resm TINFO "Test PASS"
