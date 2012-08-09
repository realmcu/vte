#!/bin/sh -x
###################################################################################################
#
#    @file   vpu_performance.sh
#
#    @brief  shell script for testcase design for VPU app
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#<Hake Huang>/-----             <2011/09/08>     N/A          Initial version
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

#setup the fb on
echo 0 > /sys/class/graphics/fb0/blank
echo -e "\033[9;0]" > /dev/tty0

mkdir /mnt/temp
mount -t tmpfs tmpfs /mnt/temp || RC=1

sleep 1
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

umount /mnt/temp
return $RC
}

# Function:     test_case_01
# Description   - Test if MPEG2 decode ok
#  
test_case_01()
{
#TODO give TCID 
TCID="vpu_DEC_PERFORMANCE"
#TODO give TST_COUNT
TST_COUNT=1
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

stream_path=/mnt/nfs/test_stream/video/mx51_vpu_performance_testvector

stream_list=mx51_vpu_performance_test_filelist.txt


cat ${stream_path}/${stream_list} |
while read line
do
  filename=$(echo $line | cut -d " " -f 1 )
	fileformat=$(echo $line | cut -d " " -f 2)
	filedir=$(echo $line | cut -d " " -f 3)
	echo "--------------------------------"
	echo "start decode $filename"
	cp ${stream_path}/${filedir}/${filename} /mnt/temp/${filename}
  time -p ${TSTCMD} -D "-a 100 -i /mnt/temp/${filename} \
  -f ${fileformat}"
	 rm -rf /mnt/temp/${filename}
	echo "end of decoding $filename"
	echo "================================"
done


RC=0
return $RC
}

# Function:     test_case_02
# Description   - Test if MPEG2 decode ok
#  
test_case_02()
{
#TODO give TCID 
TCID="vpu_ENC_PERFORMANCE"
#TODO give TST_COUNT
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

echo "encode h264 from random"

SIZELIST="720x480 1280x720 1920x1080"

BITRATE="30000 10000 5000"

ROTATION="0 90 180 270"

for i in $SIZELIST
   do
 OWD=$(echo $i | sed "s/x/ /g" | awk '{print $1}')
 OHT=$(echo $i | sed "s/x/ /g" | awk '{print $2}')
  echo "size is $OWD x $OHT"
	for j in $BITRATE
		do
			for r in $ROTATION
				do
time -p $TSTCMD -E "-i /dev/zero -f 2 -w $OWD -h $OHT -o /dev/null -c 10 -b $j -r $r -a 100" || RC=$(expr $RC + 1)
        done
    done
 done
return $RC
}

test_case_03()
{
#TODO give TCID
	TCID="vpu_VDOA_Dec test"
#TODO give TST_COUNT
	TST_COUNT=1
	RC=0
	tst_resm TINFO "test $TCID"
#	stream_path=/mnt/nfs/test_stream/video
#	FILELIST="sunflower_2B_2ref_WP_40Mbps.264 h264_bp_l31_mp3_1280x720_30fps_3955kbps_a_48khz_64kbps_stereo_broken-ntsc_tvc_video.h264 h264_bp_l31_mp3_720x480_15fps_1940kbps_a_48khz_64kbps_stereo_broken-ntsc_tvc_video.h264 balloons_3d.264"
#	for i in $FILELIST
#   	do
#		cp ${stream_path}/$i /mnt/temp/$i
#		$TSTCMD -D "-f 2 -y 1 -i /mnt/temp/$i"
#		$TSTCMD -D "-f 2 -y 0 -i /mnt/temp/$i"
#		#$TSTCMD -D "-f 2 -y 2 -i /mnt/temp/$i"
#		rm -rf /mnt/temp/$i
#	done
#	echo "VPU VDOA dec test"
	stream_path=/mnt/nfs/test_stream/video/mx51_vpu_performance_testvector
	stream_list=mx51_vpu_performance_test_filelist.txt

	cat ${stream_path}/${stream_list} |
	while read line
	do
  		filename=$(echo $line | cut -d " " -f 1 )
		fileformat=$(echo $line | cut -d " " -f 2)
		filedir=$(echo $line | cut -d " " -f 3)
		echo "--------------------------------"
		echo "start decode $filename"
		cp ${stream_path}/${filedir}/${filename} /mnt/temp/${filename}
	  ${TSTCMD} -D "-a 100 -i /mnt/temp/${filename} \
	  -f ${fileformat} -y 1" || RC=$?
		 rm -rf /mnt/temp/${filename}
		echo "end of decoding $filename"
		echo "================================"
	done
	return $RC
}

test_case_04()
{
#TODO give TCID
	TCID="vpu_VDOA_Dec test2"
#TODO give TST_COUNT
	TST_COUNT=1
	RC=0
	tst_resm TINFO "test $TCID"
#	stream_path=/mnt/nfs/test_stream/video
#	FILELIST="sunflower_2B_2ref_WP_40Mbps.264 h264_bp_l31_mp3_1280x720_30fps_3955kbps_a_48khz_64kbps_stereo_broken-ntsc_tvc_video.h264 h264_bp_l31_mp3_720x480_15fps_1940kbps_a_48khz_64kbps_stereo_broken-ntsc_tvc_video.h264 balloons_3d.264"
#	for i in $FILELIST
#   	do
#		cp ${stream_path}/$i /mnt/temp/$i
#		$TSTCMD -D "-f 2 -y 1 -i /mnt/temp/$i"
#		$TSTCMD -D "-f 2 -y 0 -i /mnt/temp/$i"
#		#$TSTCMD -D "-f 2 -y 2 -i /mnt/temp/$i"
#		rm -rf /mnt/temp/$i
#	done
#	echo "VPU VDOA dec test"
	stream_path=/mnt/nfs/test_stream/video/
	stream_list=vpu_performance_test2.txt

	cat ${stream_path}/${stream_list} |
	while read line
	do
  		filename=$(echo $line | cut -d " " -f 1 )
		fileformat=$(echo $line | cut -d " " -f 2)
		echo "--------------------------------"
		echo "start decode $filename"
		cp ${stream_path}/${filename} /mnt/temp/${filename}
	  ${TSTCMD} -D "-a 100 -i /mnt/temp/${filename} \
	  -f ${fileformat} -y 1" || RC=$(expr $RC + 1)
		 rm -rf /mnt/temp/${filename}
		echo "end of decoding $filename"
		echo "================================"
	done

	return $RC
}




usage()
{
echo "usage $0 <1/2/3/4/5/6/7/8>"
echo "1: Dec performance test"
echo "2: Enc performance test"
echo "3: VDOA Dec test"
echo "4: VDOA Dec test 2"
}

TSTCMD="/unit_tests/mxc_vpu_test.out"

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
#TODO check parameter
  usage
  ;;
esac

tst_resm TINFO "Test PASS"

