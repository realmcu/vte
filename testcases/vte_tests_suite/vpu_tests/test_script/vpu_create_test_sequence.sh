#!/bin/sh
###################################################################################################
#
#    @file   vpu_create_test_sequence.sh
#
#    @brief  shell script for testcase design for VPU app
#
###################################################################################################
#
#   Copyright (C) 2009, Freescale Semiconductor, Inc. All Rights Reserved
#   THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#   BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#   Freescale Semiconductor, Inc.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#<Justin Qiu>/-----             <2009/07/23>     N/A          Initial version
# 
###################################################################################################

# Function:     Group_bs
#
# Description:  - The bitstream lists are stored in test file
#
Group_bs()
{

if [ ! '-a H.264_MX${Target_CPU}_Sanity.txt' ]
then
    echo "ERROR!! File H.264_MX${Target_CPU}_Sanity.txt isn't existing"
    usage
    exit 1
fi
if [ ! '-a Mpeg2_MX${Target_CPU}_Sanity.txt' ]
then
    echo "ERROR!! File Mpeg2_MX${Target_CPU}_Sanity.txt isn't existing"
    usage
    exit 1
fi
if [ ! '-a Mpeg4_MX${Target_CPU}_Sanity.txt' ]
then
    echo "ERROR!! File Mpeg4_MX${Target_CPU}_Sanity.txt isn't existing"
    usage
    exit 1
fi
if [ ! '-a VC-1_MX${Target_CPU}_Sanity.txt' ]
then
    echo "ERROR!! File VC-1_MX${Target_CPU}_Sanity.txt isn't existing"
    usage
    exit 1
fi

if [ ${Target_CPU} -eq 51 ]; then
if [ ! '-a Divx4_MX51_Sanity.txt' ]
then
    echo "ERROR!! File Divx4_MX51_Sanity.txt isn't existing"
    usage
    exit 1
fi
if [ ! '-a Divx3_MX51_Sanity.txt' ]
then
    echo "ERROR!! File Divx3_MX51_Sanity.txt isn't existing"
    usage
    exit 1
fi
fi

#H264_Baseline/H264_High/H264_Main
H264_Baseline=$(awk '{print $1,$2,$3,$4,$5}' H.264_MX${Target_CPU}_Sanity.txt | grep "\bBaseline\b" | awk '{print $1}')
H264_High=$(awk '{print $1,$2,$3,$4,$5}' H.264_MX${Target_CPU}_Sanity.txt | grep "\bHigh\b" | awk '{print $1}')
H264_Main=$(awk '{print $1,$2,$3,$4,$5}' H.264_MX${Target_CPU}_Sanity.txt | grep "\bMain\b" | awk '{print $1}')

#MPEG2_MP/MPEG2_SP
Mpeg2_MP=$(awk '{print $1,$2,$3,$4,$5}' Mpeg2_MX${Target_CPU}_Sanity.txt | grep "\bMP\b" | awk '{print $1}')
Mpeg2_SP=$(awk '{print $1,$2,$3,$4,$5}' Mpeg2_MX${Target_CPU}_Sanity.txt | grep "\bSP\b" | awk '{print $1}')

#MPEG4_SP/MPEG4_ASP
Mpeg4_SP=$(awk '{print $1,$2,$3,$4,$5}' Mpeg4_MX${Target_CPU}_Sanity.txt | grep "\bSP\b" | awk '{print $1}')
Mpeg4_ASP=$(awk '{print $1,$2,$3,$4,$5}' Mpeg4_MX${Target_CPU}_Sanity.txt | grep "\bASP\b" | awk '{print $1}')

#VC1_MP/VC1_AP
VC1_MP=$(awk '{print $1,$2,$3,$4,$5}' VC-1_MX${Target_CPU}_Sanity.txt | grep "\bMP\b" | awk '{print $1}')
VC1_AP=$(awk '{print $1,$2,$3,$4,$5}' VC-1_MX${Target_CPU}_Sanity.txt | grep "\bAP\b" | awk '{print $1}')

if [ ${Target_CPU} -eq 51 ]; then

#Divx3_PD
Divx3_PD=$(awk '{print $1,$2,$3,$4,$5}' Divx3_MX51_Sanity.txt | grep "\bPortable Device\b" | awk '{print $1}')

#Divx4_PD
Divx4_PD=$(awk '{print $1,$2,$3,$4,$5}' Divx4_MX51_Sanity.txt | grep "\bPortable Device\b" | awk '{print $1}')
fi
}

# Function:     Create_bs_sequence
#
# Description:  - The bitstream lists are stored in test file
#

Create_bs_sequence()
{
OutLoopSeq=`echo H264_Baseline H264_High H264_Main Mpeg2_MP Mpeg2_SP Mpeg4_SP Mpeg4_ASP VC1_MP VC1_AP Divx3_PD Divx4_PD`
InnerLoopSeq=`echo H264_Baseline H264_High H264_Main Mpeg2_MP Mpeg2_SP Mpeg4_SP Mpeg4_ASP VC1_MP VC1_AP Divx3_PD Divx4_PD`

# total number of combination
Combination_Total=0

for bs_type1 in ${OutLoopSeq}
do
    case "${bs_type1}" in
    Divx3_PD)
        bs_format1=5
        dir_name1=DIV3
        ;;
    Divx4_PD)
        bs_format1=105
        dir_name1=DIVX
        ;;
    H264_Baseline | H264_High | H264_Main)
        bs_format1=2
        dir_name1=H264
        ;;
    Mpeg2_MP | Mpeg2_SP)
        bs_format1=4
        dir_name1=MPG2
        ;;
    Mpeg4_SP | Mpeg4_ASP)
        bs_format1=0
        dir_name1=MPG4
        ;;
    VC1_MP | VC1_AP)
        bs_format1=3
        dir_name1=WMV9
        ;;
    *)
        echo "bad bistream format"
        echo ${bs_type1}
        ;;
    esac
        
    for bs_type2 in ${InnerLoopSeq}
    do
        case "${bs_type2}" in
        Divx3_PD)
            bs_format2=5
            dir_name2=DIV3
            ;;
        Divx4_PD)
            bs_format2=105
            dir_name2=DIVX
            ;;
        H264_Baseline | H264_High | H264_Main)
            bs_format2=2
            dir_name2=H264
            ;;
        Mpeg2_MP | Mpeg2_SP)
            bs_format2=4
            dir_name2=MPG2
            ;;
        Mpeg4_SP | Mpeg4_ASP)
            bs_format2=0
            dir_name2=MPG4
            ;;
        VC1_MP | VC1_AP)
            bs_format2=3
            dir_name2=WMV9
            ;;
        *)
            echo "bad bistream format"
            echo ${bs_type2}
            ;;
        esac
        
        if [ "${bs_type1}" != "${bs_type2}" ]
        then
            eval file_of_type1=\$${bs_type1}
            eval file_of_type2=\$${bs_type2}
            
            file1_Cnt=0
            for file1 in ${file_of_type1}
            do
                if [ ${file1_Cnt} -lt ${file1_MAXNUM} ]
                then
                    file2_Cnt=0
                    for file2 in ${file_of_type2}
                    do
                        if [ ${file2_Cnt} -lt ${file2_MAXNUM} ]
                        then
                            echo ${file1} ${bs_format1} ${dir_name1} >> ${ListFile}
                            echo ${file2} ${bs_format2} ${dir_name2} >> ${ListFile}
                            file2_Cnt=$((${file2_Cnt}+1))
                            Combination_Total=$((${Combination_Total}+1))
                            
                            echo "${bs_type1},${bs_type2}, ${file1}, ${file2}"  >> ${csvfile}
                        else
                            break
                        fi
                    done
                    file1_Cnt=$((${file1_Cnt}+1))
                else
                    break
                fi
            done
        fi
    done
done

echo "There are totally ${Combination_Total} combination!"
}

usage()
{
echo "usage $0 [-t ] [-n1 ] [-n2 ] [-o ] [-h]"
echo "-t: 37 for i.MX37; 51 for i.MX51 (default:37)"
echo "-n1: the max bitstream number for the 1st bitstream format, (default:1)"
echo "-n2: the max bitstream number for the 2nd bitstream format, (default:1)"
echo "-o: the output filename, (default:vpu_dec_sequence_filelist.txt)"
echo "-h: print this message"
}

H264_Baseline=
H264_High=
H264_Main=
Mpeg2_MP=
Mpeg2_SP=
Mpeg4_SP=
Mpeg4_ASP=
VC1_MP=
Divx3_PD=
Divx4_PD=

# set the max number of selected bitstreams in each format
file1_MAXNUM=1
file2_MAXNUM=1


while [ ! -z "$1" ]
do
case "$1" in
-h)
    usage
    exit 1
    ;;
-t)
    if [ ! -z "$2" ]; then
        if [ $2 -eq 37 ]; then
            Target_CPU=37
        elif [ $2 -eq 51 ]; then
            Target_CPU=51    
        else
            usage
            exit 1
        fi
    else
        Target_CPU=37
    fi
    echo "target cup is $Target_CPU"
    ListFile=mx${Target_CPU}_vpu_dec_sequence_sanity_filelist.txt
    csvfile=mx${Target_CPU}_vpu_dec_sequence_sequence.csv
    ;;    
-n1)
    if [ ! -z "$2" ]
    then
        file1_MAXNUM=$2
    else
        echo "use the default value 1 for n1"
    fi
    ;;
-n2) 
    if [ ! -z "$2" ]
    then
        file2_MAXNUM=$2
    else
        echo "use the default value 1 for n2"
    fi
    ;;
-o)
    if [ ! -z "$2" ]
    then
        ListFile=$2
    else
        echo "use the default file for output"
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

echo "----------- Create bitstream sequence begin -----------------"
echo "max file number for 1st bitstream type: ${file1_MAXNUM}"
echo "max file number for 2nd bitstream type: ${file2_MAXNUM}"
echo "the output listfile is ${ListFile}"


rm -f ${ListFile}
rm -f ${csvfile}

Group_bs
Create_bs_sequence

echo "----------- Create bitstream sequence finished! -------------"
