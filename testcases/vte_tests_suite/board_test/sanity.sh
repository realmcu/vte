#!/bin/bash
# Function      - sanity test for MX6 series boards
#
# Description   - execute it only when new boards arrived. 
#
# Return        - zero on success
#               - non zero on failure. return value from commands ($RC)
#
mx6dl_sanity_test()
{
	RC=0
 #run 1080P video playback 8 hours to single display
	display_stress.sh 7 || RC=$?
	if [ $RC ne 0 ];then 
		echo "test fail in S1"
	fi
	return $RC
#for MX6DL, run dual 720P video playback for 2 hours to dual display
	i=0; while [ $i -lt 50 ]; do echo "S2 $i times===="; display_stress.sh 8; i=`expr $i + 1`; done || RC=$?
	if [ $RC ne 0 ];then
		echo "test fail in S2"
	fi
	return $RC
#bonnie++ to all storage devices on board.
	echo "S3 $i times====";storage_all.sh 2 || RC=$?
	if [ $RC ne 0 ];then
		echo "test fail in S3"
	fi
	return $RC
#Audio playback loop test.
	i=0; while [ $i -lt 100 ]; do echo "S4 $i times===="; aplay -Dplughw:0 $STREAM_PATH/alsa_stream/audio32k16S.wav; i=`expr $i + 1`;done || RC=$?
	if [ $RC ne 0 ];then 
		echo "test fail in S4"
	fi
	return $RC
#Suspend&resume loop test.
	i=0; while [ $i -lt 100 ]; do echo "S5 $i times===="; rtc_testapp_6 -T 15; i=`expr $i + 1`; done || RC=$?
	if [ $RC ne 0 ];then
		echo "test fail in S5"
	return $RC
	fi
return $RC
}
#
#
#
mx6dls_ard_sanity_test()
{
#run 1080P video playback 8 hours to single display
	display_stress.sh 7 || RC=$?
	if [ $RC ne 0 ];then 
		echo "test fail in S1"
	return $RC
	fi
#for MX6DL, run dual 720P video playback for 2 hours to dual display
	i=0; while [ $i -lt 50 ]; do echo "S2 $i times===="; display_stress.sh 8; i=`expr $i + 1`; done || RC=$?
	if [ $RC ne 0 ];then 
		echo "test fail in S2"
	return $RC
	fi
#bonnie++ to all storage devices on board.
	echo "S3 $i times===="; storage_all.sh 2 || RC=$?
	if [ $RC ne 0 ];then 
		echo "test fail in S3"
	return $RC
	fi
#Audio playback loop test.
	i=0; while [ $i -lt 100 ]; do echo "S4 $i times===="; aplay -Dplughw:0 $STREAM_PATH/alsa_stream/audio32k16S.wav; i=`expr $i + 1`;done || RC=$?
	if [ $RC ne 0 ];then 
		echo "test fail in S4"
	return $RC
	fi
return $RC
}
#
#
#
mx6q_sanity_test()
{
#run 1080P video playback 8 hours to single display
	display_stress.sh 4 || RC=$?
	if [ $RC ne 0 ];then 
	echo "test fail in S1"
	return $RC
	fi
#for MX6DQ, run dual 1080P video playback for 2 hours to dual display
	i=0; while [ $i -lt 2 ]; do echo "S2 $i times===="; display_stress.sh 4; i=`expr $i + 1`; done || RC=$?
	if [ $RC ne 0 ];then 
	echo "test fail in S2"
	return $RC
	fi
#bonnie++ to all storage devices on board.
	echo "S3 $i times===="; storage_all.sh 2 || RC=$?
	if [ $RC ne 0 ];then 
	echo "test fail in S3"
	return $RC
	fi
#Audio playback loop test.
	i=0; while [ $i -lt 2 ]; do echo "S4 $i times===="; aplay -Dplughw:0 $STREAM_PATH/alsa_stream/audio32k16S.wav; i=`expr $i + 1`;done || RC=$?
	if [ $RC ne 0 ];then 
	echo "test fail in S4"
	return $RC
	fi
#Suspend&resume loop test.
	i=0; while [ $i -lt 2 ]; do echo "S5 $i times===="; rtc_testapp_6 -T 15; i=`expr $i + 1`; done || RC=$?
	if [ $RC ne 0 ];then 
	echo "test fail in S5"
	return $RC
	fi
return $RC
}
#
#
#
mx6q_ard_sanity_test()
{
#run 1080P video playback 8 hours to single display
	display_stress.sh 7 || RC=$?
	if [ $RC ne 0 ];then
        echo "test fail in S1"
        return $RC
        fi
#for MX6DQ, run dual 1080P video playback for 2 hours to dual display
	i=0; while [ $i -lt 50 ]; do echo "S2 $i times===="; display_stress.sh 4; i=`expr $i + 1`; done || RC=$?
        if [ $RC ne 0 ];then
        echo "test fail in S2"
        return $RC
        fi
#bonnie++ to all storage devices on board.
	echo "S3 $i times===="; storage_all.sh 2 || RC=$?
        if [ $RC ne 0 ];then
        echo "test fail in S3"
        return $RC
        fi
#Audio playback loop test.
	i=0; while [ $i -lt 100 ]; do echo "S4 $i times===="; aplay -Dplughw:0 $STREAM_PATH/alsa_stream/audio32k16S.wav; i=`expr $i + 1`;done || RC=$?
        if [ $RC ne 0 ];then
        echo "test fail in S4"
        return $RC
        fi
return $RC
}


if [ `platfm.sh` = "IMX6-SABRELITE" ] || [ `platfm.sh` = "IMX6ARM2" ] || [ `platfm.sh` = "IMX6Q-Sabre-SD" ]
then 
	mx6q_sanity_test
fi
if [ `platfm.sh` = "IMX6Solo-SABREAUTO" ]
then
	mx6q_ard_sanity_test
fi
if [ `platfm.sh` = "IMX6-SABRELITE" ] || [ `platfm.sh` = " IMX6ARM2" ] || [ `platfm.sh` = " IMX6Q-Sabre-SD" ]
then
	 mx6dl_sanity_test
fi
if [ `platfm.sh` = "IMX6DL-ARM2" ] || [ `platfm.sh` = "IMX6DL-Sabre-SD" ]
then
	mx6dls_ard_sanity_test
fi
