#Copyright (C) 2008-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
#!/bin/sh
###################################################################################################
#
#    @file   vpu_multi_enc_test.sh
#
#    @brief  shell script template for testcase design "TODO" is where to modify block.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#<Hake Huang>/-----             <2008/11/24>     N/A          Initial version
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
export TST_TOTAL=2

export TCID="setup"
export TST_COUNT=0
RC=1

trap "cleanup" 0

#TODO add setup scripts
#add camera support
modprobe mxc_v4l2_capture 
modprobe ov3640_camera

echo 1 > /proc/sys/vm/lowmem_reserve_ratio

if [ -e /dev/mxc_vpu ]
then
RC=0
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
cd $LTPROOT
return $RC
}


# Function:     test_case_01
# Description   - Test if multi encode ok
#  
test_case_01()
{
#TODO give TCID 
TCID="vpu_unit_test"
#TODO give TST_COUNT
TST_COUNT=1
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

vpu_enc_test.sh 2 &
vpu_enc_test.sh 4 || return 1

vpu_enc_test.sh 3 &
vpu_enc_test.sh 2 || return 1
#vpu_enc_test.sh 5 || return 1

wait
RC=0
return $RC

}

# Function:     test_case_02
# Description   - Test if encode + decode  ok
#  
test_case_02()
{
#TODO give TCID 
TCID="vpu_enc_dec_process"
#TODO give TST_COUNT
TST_COUNT=2
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

vpu_enc_test.sh 2 &
vpu_multi_dec_test.sh 1 || return 1

vpu_enc_test.sh 3 &
vpu_multi_dec_test.sh 1 || return 1

vpu_enc_test.sh 4 &
vpu_multi_dec_test.sh  1 || return 1

vpu_enc_test.sh 4 &
vpu_multi_dec_test.sh 1 || return 1

wait

RC=0
return $RC

}


# Function:     test_case_03
# Description   - Test if encode + decode in one app ok
#  
test_case_03()
{
#TODO give TCID 
TCID="vpu_enc_dec_app"
#TODO give TST_COUNT
TST_COUNT=2
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
TSTCMD="/unit_tests/mxc_vpu_test.out"
VPATH="${STREAM_PATH}/video/"
SRCLIST="4+mpeg2_720x576.mpg 3+SD720x480.vc1.rcv 0+akiyo.mp4 2+starwars640x480.264 1+stream.263"
EFORMAT="0 1 2 3 7"

#${TSTCMD} -E "-o enc.264 -w 176 -h 144 -f 0" -D "-i /vectors/vga.264 -f 2"
echo "test play files while encode"
for i in $EFORMAT
do
 for j in $SRCLIST
 do
  FORMAT=$(echo $j | sed "s/+/ /g" | awk '{print $1}')
  srcfile=$(echo $j | sed "s/+/ /g" | awk '{print $2}')
  echo "do encode $i with dec $j"
  ${TSTCMD} -E "-i /dev/zero -o /dev/null -w 176 -h 144 -f $i -c 30" -D "-i ${VPATH}/${srcfile} -f $FORMAT" || return $RC
  rm -rf /tmp/enc.dat
  sleep 1
 done
done

#SIZELIST="64x64 176x144 352x288 320x640 640x480 320x240 352x288 176x144 64x64"

#echo "loopback test"
#for i in $EFORMAT
#do
# for j in $SIZELIST
# do
#   OWD=$(echo $j | sed "s/x/ /g" | awk '{print $1}')
#   OHT=$(echo $j | sed "s/x/ /g" | awk '{print $2}')	 
#   echo "loopback on format $i for size $j"
#   ${TSTCMD} -L "-f $i -w $OWD -h ${OHT} -t 1" || return $RC
# done
#done

RC=0
return $RC
}

# Function:     test_case_04
# Description   - Test if H263 encode + H263 decode + other format ok
#  
test_case_04()
{
#TODO give TCID 
TCID="vpu_enc_h263_app"
#TODO give TST_COUNT
TST_COUNT=2
RC=1

echo "this case will run for 24 hours"

STRLIST="4+mpeg2_720x576.mpg 3+SD720x480.vc1.rcv 0+akiyo.mp4 2+starwars640x480.264 1+stream.263"
SRCLIST="1+stream.263 1+CITY_640x480_30.263 1+CREW_640x480_30.263 \
1+HARBOUR_640x480_30.263 1+ICE_640x480_30.263 1+SOCCER_640x480_30.263"
RAWLIST="ICE_640x480_30.yuv SOCCER_640x480_30.yuv HARBOUR_640x480_30.yuv \
CREW_640x480_30.yuv CITY_640x480_30.yuv"
TSTCMD="/unit_tests/mxc_vpu_test.out"
#TSTCMD="/mnt/nfs/util/mxc_vpu_test.out.h263_jtk"
VPATH="${STREAM_PATH}/video/"
SPATH="${STREAM_PATH}/video/vga4v2ip"
YY=$(date +"%Y")
MM=$(date +"%m")
DD=$(date +"%d")
STIME=$(expr ${YY} \* 365 + ${MM} \* 31 + ${DD})
GOON=1
while [ $GOON = "1" ]
do
#file test iencode form camera
 for i in $SRCLIST
 do
  srcfile=$(echo $i | sed "s/+/ /g" | awk '{print $2}')
  for j in $STRLIST
  do
  FORMAT=$(echo $j | sed "s/+/ /g" | awk '{print $1}')
  strfile=$(echo $j | sed "s/+/ /g" | awk '{print $2}')
  echo "format $FORMAT, $srcfile, $strfile Encode form camera "
  ${TSTCMD} -E "-o /dev/null -w 640 -h 480 -f 1 -c 3000" \
  -D "-i ${SPATH}/${srcfile} -f 1" \
  -D "-i ${VPATH}/${strfile} -f ${FORMAT} -o /dev/null"|| return $RC
  sleep 1
   for k in $RAWLIST
   do
   rm -rf /tmp/enc.dat
   echo "now encode form file $k"
  ${TSTCMD} -E "-i ${VPATH}/$k -o /tmp/enc.dat -w 640 -h 480 -f 1" \
  -D "-i ${SPATH}/${srcfile} -f 1" \
  -D "-i ${VPATH}/${strfile} -f ${FORMAT} -o /dev/null"|| return $RC
   rm -rf /tmp/enc.dat
   done
  done
 done
 GOON=0
done
RC=0
return $RC
}


# Function:     test_case_05
# Description   - Test if H264 encode + H264 decode + other format ok
#  
test_case_05()
{
#TODO give TCID 
TCID="vpu_enc_h264_app"
#TODO give TST_COUNT
TST_COUNT=2
RC=1
echo "this case will run for 24 hours"

STRLIST="4+mpeg2_720x576.mpg 3+SD720x480.vc1.rcv  0+akiyo.mp4 2+starwars640x480.264 1+stream.263"
SRCLIST="2+starwars640x480.264 2+CITY_640x480_30.264 2+CREW_640x480_30.264 \
2+HARBOUR_640x480_30.264 2+ICE_640x480_30.264 2+SOCCER_640x480_30.264"
RAWLIST="ICE_640x480_30.yuv SOCCER_640x480_30.yuv HARBOUR_640x480_30.yuv \
CREW_640x480_30.yuv CITY_640x480_30.yuv"
TSTCMD="/unit_tests/mxc_vpu_test.out"
VPATH="${STREAM_PATH}/video/"
SPATH="${STREAM_PATH}/video/vga4v2ip"
GOON=1
while [ $GOON = "1" ]
do
#file test 
 for i in $SRCLIST
 do
  srcfile=$(echo $i | sed "s/+/ /g" | awk '{print $2}')
  for j in $STRLIST
  do
  FORMAT=$(echo $j | sed "s/+/ /g" | awk '{print $1}')
  strfile=$(echo $j | sed "s/+/ /g" | awk '{print $2}')
  ${TSTCMD} -E "-i /dev/zero -o /dev/null -w 640 -h 480 -f 2 -c 3000" -D "-i ${VPATH}/${srcfile} -f 2" \
 -D "-i ${VPATH}/${strfile} -f ${FORMAT} -o /dev/null"|| return $RC
  sleep 1
   for k in $RAWLIST
   do
   rm -rf /tmp/enc.dat
   echo "now encode form file $k"
  ${TSTCMD} -E "-i ${VPATH}/$k -o /tmp/enc.dat -w 640 -h 480 -f 2" \
  -D "-i ${VPATH}/${strfile} -f ${FORMAT} -o /dev/null "|| return $RC
   rm -rf /tmp/enc.dat
   done
  done
 done
 GOON=0
done
killall ${TSTCMD}
RC=0
return $RC
}

#TODO check parameter
if [ $# -ne 1 ]
then
echo "usage $0 <1/2/3/4/5/6>"
exit 1 
fi


#main
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
5)
  test_case_05 || exit $RC
  ;;
6)
  test_case_06 || exit $RC
  ;;
*)
#TODO check parameter
  echo "usage $0 <1/2/3/4/5>"
  ;;
esac

tst_resm TINFO "Test PASS"
