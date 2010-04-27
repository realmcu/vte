#Copyright (C) 2008-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
#!/bin/sh
#====================================================================================================
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#Ziye Yang                   14/8/2008       n/a        initialization of storage function test application
#Victor Cui                  24/10/2008      n/a        deal with exceptional situation
#====================================================================================================
#Portability:  ARM GCC  gnu compiler
#==================================================================================================*/
#
#/*==================================================================================================
#Total Tests:           1
#Test Executable Name:  storage_test_concurrent.sh
#Test Strategy:         
#=================================================================================================*/

#==================================================================================================*/
#parameters and usage explanation
#STR1: Function type: F(Format), C(Copy), CD(CopyDelte), CDF(CopyDeleteFormat)
#STR2: Device type: NAND, SD, HDD
#STR3: partition number
#STR4: Source folder
#STR5: Destination folder
#STR6: file size (unit: MB)
#STR7: copy times
#==================================================================================================*/
# Use command "./storage_test.sh [STR1] [STR2] [SRT3] [STR4] [SRT5] to test storage CDF


#initialize 
 TMP_RC=0
 rc=0
 MAX_TIMES=0
 TMP_TIME=0
 C_TIMES=0
 C_SIZE=0
 CON_NUM=0
 S_PATH=""
 D_PATH=""
 
 S_PATH=$2
 D_PATH=$3
 C_SIZE=$4
 C_TIMES=$5
 CON_NUM=$6

 if [ ! -d "$S_PATH" ] || [ ! -d "$D_PATH" ]; then
	echo "FAIL: the directory may be not exist, Please check!"
	echo " storage_test_concurrent.sh           TFAIL    "
	exit 1
 fi

 s_path_left=`df -k /$S_PATH|awk '{print $4}'`
 s_path_left=`expr substr "$s_path_left" 10 $$`

 d_path_left=`df -k /$D_PATH|awk '{print $4}'`
 d_path_left=`expr substr "$d_path_left" 10 $$`

 if [ -z $s_path_left ] || [ -z $d_path_left ]; then
	echo "FAIL: the directory may be not exist, Please check!!"
	echo " storage_test_concurrent.sh           TFAIL    "
	exit 1
 fi

if [ $S_PATH = $D_PATH ]; then
	times_tmp=`expr $C_TIMES "+" 1`
	size_tmp=`expr $C_SIZE "*" $times_tmp "*" $CON_NUM "*" 1024`
	if [ $s_path_left -lt $size_tmp ]; then
		echo "WARNING: the left volume is not enough!!!"
                C_SIZE=`expr $s_path_left "/" $times_tmp "/" $CON_NUM "/" 1024`
                echo "auto change the SIZE to $C_SIZE M"
	fi
else
	s_size_tmp=`expr $C_SIZE "*" $CON_NUM "*" 1024`
	d_size_tmp=`expr $C_SIZE "*" $C_TIMES "*" $CON_NUM "*" 1024`
	if [ $s_path_left -lt $s_size_tmp ]; then
		echo "FAIL: the soure directory left volume is not enough!!!"
		echo " storage_test_concurrent.sh           TFAIL    "
		exit 1
	fi
	if [ $d_path_left -lt $d_size_tmp ]; then
		echo "FAIL: the destination directory left volume is not enough!!!"
		echo " storage_test_concurrent.sh           TFAIL    "
		exit 1
	fi
	
fi

echo ""
echo "[Storage Concurrent copy/delete test begin]"
echo "CON_NUM is " $CON_NUM
i=0
rm -rf $2/src*
rm -rf $3/dst*

#modify by Victor storage_scripts-->/testcases/bin  021209
export | grep "PATH" | grep "/testcases/bin"
if [ $? -eq 0 ]; then
	execute_scripts=storage_test.sh
else
	execute_scripts=./storage_test.sh
fi

while [ $i -lt $CON_NUM ]; do
	mkdir $2/src$i
	mkdir $3/dst$i
	i=`expr $i "+" 1`
done

if [ -e storage_test_concurrent_log ]; then
	rm storage_test_concurrent_log
fi
if [ -e storage_test_pid ]; then
	rm storage_test_pid
fi
if [ -e ps_log ]; then
	rm ps_log
fi

i=0
while [ $i -lt $CON_NUM ]; do
	 #./storage_test.sh $1 $2/src$i $3/dst$i $4 $5 &
	 $execute_scripts  $1 $S_PATH/src$i $D_PATH/dst$i $C_SIZE $C_TIMES >> storage_test_concurrent_log &
	 echo -n "$! " >> storage_test_pid
	 i=`expr $i "+" 1`
done

#storage_test_num=`ps | grep "storage_test.sh" | wc -l`
#while [ $storage_test_num -gt 0 ];
#do
#        storage_test_num=`ps | grep "storage_test.sh" | wc -l`
#done

is_end=1
while [ $is_end -eq 1 ]
do
	con_num_tmp=0
	ps > ps_log
	for line in `cat storage_test_pid`
	do
		pid_num=`cat ps_log | grep $line | wc -l`
		if [ $pid_num -eq 1 ]; then
			con_num_tmp=`expr $con_num_tmp "+" 1`
		fi
	done
	
	if [ $con_num_tmp -eq 0 ]; then
		is_end=0
	fi
done

tpass_num=`cat storage_test_concurrent_log | grep "TPASS" | wc -l`
rm storage_test_concurrent_log
rm storage_test_pid
rm ps_log
if [ $tpass_num -eq $CON_NUM ]; then
        echo " storage_test_concurrent.sh           TPASS    "
	echo "[Storage Concurrent copy/delete test end]"
        exit 0
else
        echo " storage_test_concurrent.sh           TFAIL    "
	echo "[Storage Concurrent copy/delete test end]"
        exit 1
fi

