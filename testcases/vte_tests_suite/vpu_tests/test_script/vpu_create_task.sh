#!/bin/sh
###################################################################################################
#
#    @file   vpu_create_test_sequence.sh
#
#    @brief  shell script for testcase design for VPU app
#
#Copyright (C) 2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#<Justin Qiu>/-----             <2009/06/04>     N/A          Initial version
#<Justin Qiu>/-----             <2009/08/03>     N/A          Update for i.MX51
# 
###################################################################################################


create_task()
{
BS_CNT=0

echo "listfile=${ListFile}"

BS_TOTAL=`wc -l $ListFile | awk '{print $1}'`

echo "bs total = $BS_TOTAL"
echo "[Config Number]" >> ${tsk_file}
echo "Config Number=${BS_TOTAL}" >> ${tsk_file}

cat $ListFile | 
while read line 
do 
    filename=`echo "$line" | awk '{print $1}'`
    fileformat=`echo "$line" | awk '{print $2}'`
    filedir=`echo "$line" | awk '{print $3}'`
    
    SKIP_FLAG=0
    case "${fileformat}" in
    2)
        #H.264
        Test_Type=32821
        Target_CPU=2 
        Target_OS=0
        Connection=0
        Target_IP=$target_ip
        Decoder_Type=2       
        Decoder_Binary=0
        Decoder_Binary_Name=$dut_library
        Decoder_Input_Mode=0
        RAW=1
        AVI=0
        RCV=0
        RealMedia=0
        RealVideo=0
        RealVideo_Raw=0
        Error_Type=0
        Packet_Length=1024
        Error_Distribution=0
        Error_Rate=1000
        Error_Pattern=
        Number_of_Runs=1
        ;;
    4)
        #Mpeg2
        Test_Type=32821
        Target_CPU=2
        Target_OS=0
        Connection=0
        Target_IP=$target_ip
        Decoder_Type=3
        Decoder_Binary=0
        Decoder_Binary_Name=$dut_library
        Decoder_Input_Mode=0
        RAW=1
        AVI=0
        RCV=0
        RealMedia=0
        RealVideo=0
        RealVideo_Raw=0
        Error_Type=0
        Packet_Length=1024
        Error_Distribution=0
        Error_Rate=1000
        Error_Pattern=
        Number_of_Runs=1
        ;;        
    0)
        #Mpeg4
        Test_Type=32821
        Target_CPU=2
        Target_OS=0
        Connection=0
        Target_IP=$target_ip
        Decoder_Type=4
        Decoder_Binary=0
        Decoder_Binary_Name=$dut_library
        Decoder_Input_Mode=0
        RAW=1
        AVI=0
        RCV=0
        RealMedia=0
        RealVideo=0
        RealVideo_Raw=0
        Error_Type=0
        Packet_Length=1024
        Error_Distribution=0
        Error_Rate=1000
        Error_Pattern=
        Number_of_Runs=1
        ;;         
    3)
        #VC-1/WMV
        Test_Type=32821
        Target_CPU=2
        Target_OS=0
        Connection=0
        Target_IP=$target_ip
        Decoder_Type=6
        Decoder_Binary=0
        Decoder_Binary_Name=$dut_library
        Decoder_Input_Mode=0
        RAW=0
        AVI=0
        RCV=1
        RealMedia=0
        RealVideo=0
        RealVideo_Raw=0
        Error_Type=0
        Packet_Length=1024
        Error_Distribution=0
        Error_Rate=1000
        Error_Pattern=
        Number_of_Runs=1
        ;;                 
    5)
        #DIV3
        Test_Type=32821
        Target_CPU=2
        Target_OS=0
        Connection=0
        Target_IP=$target_ip
        Decoder_Type=0
        Decoder_Binary=0
        Decoder_Binary_Name=$dut_library
        Decoder_Input_Mode=0
        RAW=0
        AVI=0
        RCV=1
        RealMedia=0
        RealVideo=0
        RealVideo_Raw=0
        Error_Type=0
        Packet_Length=1024
        Error_Distribution=0
        Error_Rate=1000
        Error_Pattern=
        Number_of_Runs=1
        ;;
    105)
        #DIVX4
        Test_Type=32821
        Target_CPU=2
        Target_OS=0
        Connection=0
        Target_IP=$target_ip
        Decoder_Type=1
        Decoder_Binary=0
        Decoder_Binary_Name=$dut_library
        Decoder_Input_Mode=0
        RAW=0
        AVI=0
        RCV=1
        RealMedia=0
        RealVideo=0
        RealVideo_Raw=0
        Error_Type=0
        Packet_Length=1024
        Error_Distribution=0
        Error_Rate=1000
        Error_Pattern=
        Number_of_Runs=1
        ;;       
    *)
        echo "not support bitstream type!"
        echo "skip it"
        SKIP_FLAG=1
        ;;
    esac
    
    if [ ${SKIP_FLAG} -ne 1 ]
    then
        echo "[Config ${BS_CNT}]" >> ${tsk_file}
        echo "Test Type=${Test_Type}" >> ${tsk_file}
        echo "Target CPU=${Target_CPU}" >> ${tsk_file}
        echo "Target OS=${Target_OS}" >> ${tsk_file}
        echo "Connection=${Connection}" >> ${tsk_file}
        echo "Target IP=${Target_IP}" >> ${tsk_file}
        echo "Decoder Type=${Decoder_Type}"  >> ${tsk_file}
        echo "Decoder Binary=${Decoder_Binary}" >> ${tsk_file}
        echo "Decoder Binary Name=${Decoder_Binary_Name}" >> ${tsk_file}
        echo "Decoder Input Mode=${Decoder_Input_Mode}" >> ${tsk_file}
        echo "RAW=${RAW}" >> ${tsk_file}
        echo "AVI=${AVI}" >> ${tsk_file}
        echo "RCV=${RCV}" >> ${tsk_file}
        echo "RealMedia=${RealMedia}" >> ${tsk_file} 
        echo "RealVideo=${RealVideo}" >> ${tsk_file}
        echo "RealVideo Raw=${RealVideo_Raw}" >> ${tsk_file}
        echo "Error Type=${Error_Type}" >> ${tsk_file}
        echo "Packet Length=${Packet_Length}" >> ${tsk_file}
        echo "Error Distribution=${Error_Distribution}" >> ${tsk_file}
        echo "Error Rate=${Error_Rate}" >> ${tsk_file}
        echo "Error Pattern=${Error_Pattern}" >> ${tsk_file}
        echo "Number of Runs=${Number_of_Runs}" >> ${tsk_file} 
        
        rm -f TestVectorList_${BS_CNT}.txt
        echo ";START" >> TestVectorList_${BS_CNT}.txt
        echo "${stream_path}\\${filedir}\\${filename}" >> TestVectorList_${BS_CNT}.txt
        echo ";END" >> TestVectorList_${BS_CNT}.txt
        echo $BS_CNT, $line, "... done!"
        let BS_CNT=BS_CNT+1
    fi
    
done

echo "[Advanced Options]" >> ${tsk_file}
echo "Max First Delay=$max_first_delay" >> ${tsk_file}
echo "Max Output Interval=$max_output_interval" >> ${tsk_file}
echo "Reset Cmd=$reset_cmd" >> ${tsk_file}
}

usage()
{
echo "usage $0 -ip ip_of_target_board -so dut_dynamic_library_with_full_directory -d1 max_first_delay -d2 max_output_interval -r reset_cmd -p stream_path -f bitstream_file_list -o taskfile"
}

target_ip=192.168.1.101
dut_library="c:\Program Files\Freescale\VideoTestSuite\libDutDec_MX37TO111_elinux.so"
max_first_delay=180
max_output_interval=180
reset_cmd="perl d:\reset.pl"
stream_path="W:\Common\TestVectors"
ListFile=mx37_vpu_dec_sequence_sanity_filelist.txt
tsk_file=task1.tsk


#if [ $? -le 2 ]
#then
#	usage
#	exit 1
#fi

echo "parser command line:"
while [ ! -z "$1" ]
do
case "$1" in
-h)
    usage
    exit 1
    ;;
-ip)
    if [ ! -z "$2" ]
    then
        target_ip=$2
    else
        usage
        exit 1
    fi
    ;;
-so) 
    if [ ! -z "$2" ]
    then
        dut_library=$2
    else
        usage
        exit 1
    fi
    ;;
-d1)
    if [ ! -z "$2" ]
    then
        max_first_delay=$2
    else
        usage
        exit 1
    fi
    ;;
-d2)
    if [ ! -z "$2" ]
    then
        max_output_interval=$2
    else
        usage
        exit 1
    fi
    ;;
-r)
    if [ ! -z "$2" ]
    then
        reset_cmd=$2
    else
        usage
        exit 1
    fi
    ;;
-p)
    if [ ! -z "$2" ]
    then
        stream_path=$2
    else
        usage
        exit 1
    fi
    ;;
-f)
    if [ ! -z "$2" ]
    then
        ListFile=$2
    else
        usage
        exit 1
    fi
    ;;
-o)
    if [ ! -z "$2" ]
    then
        tsk_file=$2
    else
        usage
        exit 1
    fi
    ;;
-*)
    usage
    exit 1
    ;;
esac
shift
shift
done

rm -f ${tsk_file}

echo "create task begin:"
create_task
echo "create task end."
