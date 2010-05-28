#!/bin/bash
################################################################################
#
#    @file   vpu_sequence_test.sh
#
#    @brief  shell script for testcase design for VPU
#
#Copyright (C) 2009-2010 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
#
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -----------------------
#<Justin Qiu>/-----             <2009/06/04>     N/A          Initial version
#<Justin Qiu>/-----             <2009/07/27>     N/A          Delete absolute path
#<Justin Qiu>/-----             <2009/10/29>     N/A          Add VPU performance test case
#Spring Zhang                2010/05/28          N/A      Add MX53 support
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

if [ $TARGET = "37" ]
then
if [ -e /sys/class/regulator/regulator_1_SW2/uV ]
then
echo 1100 > /sys/class/regulator/regulator_1_SW2/uV
fi
fi

if [ $TARGET = "37" -o $TARGET = "51" -o $TARGET = "53" ]
then
 echo 1 > /proc/sys/vm/lowmem_reserve_ratio
fi
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
if [ $TARGET = "37" ]
then
if [ -e /sys/class/regulator/regulator_1_SW2/uV ]
then
echo 1200 > /sys/class/regulator/regulator_1_SW2/uV
fi
fi

cd $LTPROOT
return $RC
}

check_platform()
{
LOCAL=0
if [ $LOCAL -eq 1 ]; then
PLATFORM="31 35 37 51 53"
  CPU_REV=$(platfm.sh)
  for i in $PLATFORM
  do
    find=$(echo $CPU_REV | grep $i | wc -l )
    if [ $find -ne 0 ]
    then
     TARGET=$i
    fi
  done
else
 platfm.sh
 TARGET=$?
 
 if [ $TARGET -eq 41 ]
 then
    TARGET=51
 fi
fi
}

# Function:     test_case_01
# Description   - Test if VPU decoder sequence test ok
#  
test_case_01()
{
#TODO give TCID 
TCID="vpu_dec_sequence_test"
#TODO give TST_COUNT
TST_COUNT=1
BS_TOTAL=0
BS_INDEX=0
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

BS_TOTAL=`wc -l ${LTPROOT}/testcases/bin/mx${TARGET}_vpu_dec_sequence_sanity_filelist.txt | awk '{print $1}'`
cat ${LTPROOT}/testcases/bin/mx${TARGET}_vpu_dec_sequence_sanity_filelist.txt | 
#BS_TOTAL=`wc -l ${LTPROOT}/testcases/bin/mx37_vpu_dec_sequence_sanity_filelist.txt | awk '{print $1}'`
#cat ${LTPROOT}/testcases/bin/mx37_vpu_dec_sequence_sanity_filelist.txt | 
while read line 
do 
    filename=`echo "$line" | awk '{print $1}'`
    fileformat=`echo "$line" | awk '{print $2}'`
    filedir=`echo "$line" | awk '{print $3}'`
    let BS_COUNT=BS_COUNT+1
    echo ""
    echo "---------------- ${BS_COUNT} / ${BS_TOTAL} --------------------------"
    echo "Now begin decoding: $filedir/$filename"
    echo "---------------------------------------------------------------------"
    if [ -z ${SaveFlag} ]
    then
        echo ""
    else
        SaveOutput=`echo "${SaveFlag} /tmp/${filename}.yuv"`
    fi
    
    mp4Class=
    if [ ${fileformat} -eq 105 ]
    then
        fileformat=0
        mp4Class="-l 5"
    fi
    ${TSTCMD} -D "-i ${STREAM_PATH}/video/sequence_test/${filedir}/${filename} -f ${fileformat} ${mp4Class} ${FrameNum} ${SaveOutput}"
    #echo "${TSTCMD} -D "-i ${STREAM_PATH}/video/sequence_test/${filedir}/${filename} -f ${fileformat} ${mp4Class} ${FrameNum} ${SaveOutput}""
    echo ""
done

RC=0

return $RC
}

# Function:     test_case_02
# Description   - Test if VPU encoder sequence test ok
#  
test_case_02()
{
#TODO give TCID 
TCID="vpu_enc_sequence_test"
#TODO give TST_COUNT
TST_COUNT=1
BS_TOTAL=0
BS_INDEX=0
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

BS_TOTAL=`wc -l ${LTPROOT}/testcases/bin/mx${TARGET}_vpu_enc_sequence_sanity_filelist.txt | awk '{print $1}'`
cat ${LTPROOT}/testcases/bin/mx${TARGET}_vpu_enc_sequence_sanity_filelist.txt | 
while read line 
do 
    filename=`echo "$line" | awk '{print $1}'`
    fileformat=`echo "$line" | awk '{print $2}'`
    filedir=`echo "$line" | awk '{print $3}'`
    let BS_COUNT=BS_COUNT+1
    echo ""
    echo "---------------- ${BS_COUNT} / ${BS_TOTAL} --------------------------"
    echo "Now begin encoding: $filedir/$filename"
    echo "---------------------------------------------------------------------"
    if [ -z ${SaveFlag} ]
    then
        echo ""
    else
        SaveOutput=`echo "${SaveFlag} /tmp/${filename}.enc"`
    fi
    ${TSTCMD} -E "-i ${STREAM_PATH}/video/sequence_test/${filedir}/${filename} -f ${fileformat} ${FrameNum} ${SaveOutput}"
    echo ""
done

RC=0

return $RC
}

# Function:     test_case_03
# Description   - Test if VPU decoder performance test ok
#  
test_case_03()
{
#TODO give TCID 
TCID="vpu_dec_performance_test"
#TODO give TST_COUNT
TST_COUNT=1
BS_TOTAL=0
BS_INDEX=0
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

rm -f pid_filename_filedir.log pid_top.log

line=`top -d1 -n1 | grep CPU`
column_count=0
for word in $line
do
     echo "word=$word"
     column_count=`expr $column_count + 1`
     if [ "$word" = "%CPU" ]
     then
         break
     fi
done

top -d1 | grep mxc_vpu_test >> pid_top.log & 
PID_top=`echo $!`

BS_TOTAL=`wc -l ${test_filelist} | awk '{print $1}'`
cat ${test_filelist} | 
while read line 
do 
    filename=`echo "$line" | awk '{print $1}'`
    fileformat=`echo "$line" | awk '{print $2}'`
    filedir=`echo "$line" | awk '{print $3}'`
    let BS_INDEX=BS_INDEX+1
    echo ""
    echo "---------------- ${BS_INDEX} / ${BS_TOTAL} --------------------------"
    echo "Now begin decoding: $filedir/$filename"
    echo "---------------------------------------------------------------------"
    if [ -n ${SaveFlag} ]
    then
        SaveOutput=`echo "${SaveFlag} /tmp/${filename}.yuv"`
    fi
    ${TSTCMD} -D "-i ${testvectordir}/${filedir}/${filename} \
    -f ${fileformat} ${FrameNum} ${SaveOutput}" &
    PID_Dec=`echo $!`
    echo "${PID_Dec}  ${filename} ${filedir}" >> pid_filename_filedir.log

    wait ${PID_Dec}
    echo ""
done

kill -9 ${PID_top}

########################################################################
rm -f vpu_performance_test_result.csv
cat pid_filename_filedir.log | 
while read line 
do 
    PID_Dec=`echo "$line" | awk '{print $1}'`
    filename=`echo "$line" | awk '{print $2}'`
    filedir=`echo "$line" | awk '{print $3}'`
    loading_array=`grep ${PID_Dec} pid_top.log | awk '{print $'$column_count'}' | sed 's/\%//'`

    echo "------------- loading for $filename -------------------------------"
    min=100
    max=0
    sum=0
    array_size=0
    for la in $loading_array 
    do
        la=`echo ${la%%.*}`
        if [ $la -eq 0 ]
        then
            continue
        fi

        echo "la=$la"
        let array_size=array_size+1
        let sum=sum+la
        if [ $la -lt $min  ]
        then
            let min=la
        fi
        if [ $la -gt $max ]
        then
            let max=la
        fi
    done

    echo "array_size=$array_size"
    echo "min=$min" 
    echo "max=$max"
    echo "sum=$sum"
    
    echo "delete the mimum one: $min"
    let array_size=array_size-1
    let sum=sum-min
    if [ $array_size -gt 0 ]
    then
        loading_avg=`expr $sum / $array_size`
    else
        echo "sampled cpu loading is not valid"
        loading_avg=100
    fi
    echo "average loading=$loading_avg"

    echo "$filedir $filename $min $max $loading_avg" >> vpu_performance_test_result.csv

done
#########################################################################

RC=0

return $RC
}


usage()
{
echo "usage $0 [test type] [frame number] [save output] [directory] [filelist]"
echo "test type: 0 -- Decode"
echo "           1 -- Encode (not support now)"
echo "           2 -- Decode performance test"
echo "frame number: <=0 -- decode the whole bitstream"
echo "              > 0 -- decode frame number from the start"
echo "save output: 0 -- not save decoder output"
echo "             1 -- save decoder output as tmp/filename.yuv"

echo "below is for performance test only"
echo "directory of testvectors, prefer to copy testvector onto sd card or usb"
echo "filelist of testvectors"
}

#TODO check parameter
if [ $# -lt 3 ]
then
echo "usage $0 [test_type] [frame number] [save output]"
usage
exit 1 
fi


TARGET=

check_platform

if [ -z $TARGET ]
then
echo "unknow target"
exit 1
fi

TSTCMD="/unit_tests/mxc_vpu_test.out"

setup || exit $RC

if [ $2 -le 0 ]
then
    FrameNum=
else
    FrameNum="-c $2"
fi

if [ $3 -eq 0 ]
then
    SaveFlag=
elif [ $3 -eq 1 ]
then
    SaveFlag="-o"
else
    usage
    exit 1
fi

testvectordir=$4
test_filelist=$5

case "$1" in
0)
    test_case_01 || exit $RC 
    ;;
1)
    echo "Encoder is not supported now!"
    usage
    exit 1
    #test_case_02 || exit $RC
    ;;
2)
    test_case_03 || exit $RC 
    ;;    
*)
    usage
    exit 1
    ;;
esac

tst_resm TINFO "Test PASS"
