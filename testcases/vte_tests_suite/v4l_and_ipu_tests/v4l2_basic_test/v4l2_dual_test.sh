#!/bin/sh
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
# Description   - Test if capture mode,rotation,input/output size is ok
#
test_case_01()
{
	#TODO give TCID
	TCID="capture_mode"
	#TODO give TST_COUNT
	TST_COUNT=1
	RC=1

	#print test info
	tst_resm TINFO "test $TST_COUNT: $TCID "
	FBX=$(fbset | grep geometry | awk '{print $2}')
	FBY=$(fbset | grep geometry | awk '{print $3}')

	#TODO add function test scripte here
	ROTATION="0 1 2 3 4 5 6 7"
	RESLIST="320x240 176x144 640x480 1024x768"
	MIPIMODES30FPS="0 1 2 3 4 5 6 7"
	MIPIMODES15FPS="0 1 2 3 4 5 6 7"
	MODES564230FPS="0 1 2 3 4 7"
	MODES564215FPS="0 1 2 3 5 6 7"
	echo "rotation with offset"
	for k in $ROTATION
	do
		echo "rotation is $k"
		for j in $RESLIST
		do
			OWD=$(echo $j | sed "s/x/ /g" | awk '{print $1}')
			OHT=$(echo $j | sed "s/x/ /g" | awk '{print $2}')
			echo "frame rate 15"
			for m in $MIPIMODES15FPS
			do
				for n in $MODES564215FPS
				do
					${TSTCMD} -T 5 -C 2 -D /dev/video0 -R $k -M $n -W $OWD -H  $OHT & ${TSTCMD} -T 5 -C 2 -s CSI_MEM -O YUV420 -R $k -H $OHT -W $OWD -D /dev/video1 -M $m ||return $RC
				done
			done
			echo "farme rate 30"
			for m in $MIPIMODES30FPS
			do
				for n in $MODES564230FPS
				do
					${TSTCMD} -T 5 -C 2 -D /dev/video0 -R $k -M $n -W $OWD -H  $OHT & ${TSTCMD} -T 5 -C 2 -s CSI_MEM -O YUV420 -R $k -H $OHT -W $OWD -D /dev/video1 -M $m ||return $RC
				done
			done
		done
	done
	return $RC
}

usage()
{
	echo "$0 [case ID]"
	echo "1: "
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

tst_resm TINFO "Test PASS"
