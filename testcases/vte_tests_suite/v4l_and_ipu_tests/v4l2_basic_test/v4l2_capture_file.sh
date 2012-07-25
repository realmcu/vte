#!/bin/bash
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
#    @file   v4l_capture_file.sh
#
#    @brief  shell script to check the quality of v4l2 capture to file function.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  ---------------------------------------
#Andy Tian	                 20120715        N/A         Initial
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

	trap "cleanup" 0

	#TODO add setup scripts
	echo -e "\033[9;0]" > /dev/tty0
	#setup the fb on
	echo 0 > /sys/class/graphics/fb0/blank
	#get mac address to avoid naming confict
	mac=`ifconfig | grep -m 1 "HWaddr" | awk '{printf $5}'`
	if [ "$CAMERA" = "ov5640_mipi" ]; then
		TSTCMD="v4l_capture_testapp -D /dev/video1"
	else
		TSTCMD="v4l_capture_testapp"
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
	#set screen as black
	dd if=/dev/zero of=/dev/fb
	v4l_module.sh cleanup
	return $RC
}

# Function:     test_case_01
# Description   - capture to file and check the quality of the pictures
#
test_case_01()
{
	#TODO give TCID
	TCID="capture to file for ${fps}fps"
	#TODO give TST_COUNT
	TST_COUNT=1
	RC=1

	#print test info
	tst_resm TINFO "test $TST_COUNT: $TCID "

	#TODO add function test scripte here

	local -a size
	size=(640 480 320 240 720 480 720 576 1280 720 1920 1080 2592 1944 176 144 1024 768)
	modes="0 1 2 3 4 5 6 7 8"
	for M in $modes; do
		let w=2*M
		let h=2*M+1
		if [ ${size[$w]} -gt 1024 -o ${size[$h]} -gt 1024 ]; then
			#use CSI_MEM by default and YUV420 format
			$TSTCMD -r ${fps} -C 3 -M $M -W ${size[$w]} -H ${size[$h]} -O YUV420 -o /mnt/nfs/temp/${mac}_${fps}fps_output_M${M}
		else
			#if resolution w/h < 1024, use CSI_IC_MEM use BGR24 to verify IPU
			#color space convert
			$TSTCMD -r ${fps} -C 3 -M $M -W ${size[$w]} -H ${size[$h]} -O BGR24 -o /mnt/nfs/temp/${mac}_${fps}fps_output_M${M}
		fi
		if [ $? -ne 0 ];then
			skip_modes="$skip_modes $M"
			continue
		fi
		if [ ${size[$w]} -gt 1024 -o ${size[$h]} -gt 1024 ]; then
			conv -i /mnt/nfs/temp/${mac}_${fps}fps_output_M${M}_YUV420 -b 256 -I YUV420 -x ${size[$w]} -y ${size[$h]} \
				-o /mnt/nfs/temp/${mac}_${fps}fps_out_M${M}.bmp
		else
			conv -i /mnt/nfs/temp/${mac}_${fps}fps_output_M${M}_BGR24 -b 256 -I BGR24 -x ${size[$w]} -y ${size[$h]} \
				-o /mnt/nfs/temp/${mac}_${fps}fps_out_M${M}.bmp
		fi
		if [ $? -ne 0 ]; then
			echo "conv to bmp fail. Pls check it manually"
			tst_resm TINFO "$TCID: Test FAIL"
			exit 1
		fi
		#fill the screen with random data to see the picture more clear
		dd if=/dev/urandom of=/dev/fb
		fbv -f -s 30 /mnt/nfs/temp/${mac}_${fps}fps_out_M${M}.bmp
		read -p "Is the quality of this picture acceptable? (y/n)" ans
		if [ "$ans" != "y" -a "$ans" != "Y" ]; then
			bad_quality="$bad_quality $M"
		fi
	done
	echo $skip_modes
	echo $bad_quality
	if [ -n "$skip_modes" ];then
		echo "Below modes are not support, please check it"
		echo $skip_modes
	fi
	if [ -n "$bad_quality" ];then
		echo "The picture quality is bad for below modes"
		echo $bad_quality
		tst_resm TINFO "$TCID: Test FAIL"
		return 1
	fi
}

usage()
{
	echo "`bashname $0` [case ID]"
	echo "1: 30fps capture to file quality test"
	echo "2: 15fps capture to file quality test"
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
		fps=30
		test_case_01 || exit $RC
		;;
	2)
		fps=15
		test_case_01 || exit $RC
		;;
	*)
		usage
		;;
esac

tst_resm TINFO "Test PASS"
