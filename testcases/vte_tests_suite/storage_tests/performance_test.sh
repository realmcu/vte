#Copyright (C) 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
#!/bin/sh
################################################################################
#
#    @file   performance_test.sh
#
#    @brief  this shell script is used to test the performance of all storage
#	     modules. including: nand, nor, sd/mmc, ata, usbh;
#
################################################################################
#Revision History: 
#                            Modification     ClearQuest 
#Author                          Date          Number    Description of Changes 
#-------------------------   ------------    ----------  -----------------------
#Victor Cui/b19309            18/12/2007      N/A          Initial version 
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
RC=0 
trap "cleanup" 0 
 
#TODO add setup scripts 


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

return $RC 
} 
 
 
################################################################################
# Function:     ***_test 
# Description   
#
################################################################################ 
performance_test() 
{
	RC=0 

	#TODO add function test scripte here
	echo "******do performace test******";
	export | grep "PATH" | grep "$PATH_PERFORMANCE"
	if [ $? -eq 0 ]; then
        	execute_scripts=auto_prepare.sh
	else
        	execute_scripts=./auto_prepare.sh
	fi
	
	$execute_scripts -P $device_tmp
	file=${operation_tmp}_${device_tmp}
	echo $file
	#rm format_command_*
	#time -p dd if=/dev/zero of=/mnt/flc/aaa.img bs=1024k count=30 2>&1 >> $PATH_PERFORMANCE/time_tmp.txt;
	echo "the $device_tmp performance test" > $PATH_PERFORMANCE/time_tmp_$file.txt;
	echo "--------------------" >> $PATH_PERFORMANCE/time_tmp_$file.txt;
	echo "time:" >> $PATH_PERFORMANCE/time_tmp_$file.txt
	time -p $execute_command 2>&1 | grep "real" >> $PATH_PERFORMANCE/time_tmp_$file.txt;
	time -p sync 2>&1 | grep "real" >> $PATH_PERFORMANCE/time_tmp_$file.txt;
	echo "--------------------" >> $PATH_PERFORMANCE/time_tmp_$file.txt;

	cat $PATH_PERFORMANCE/time_tmp_$file.txt | grep "real" | awk '{print $2}'| sed 's/\.//g' > $PATH_PERFORMANCE/time_tmp_real.txt

	real_time1=`head -n 1 $PATH_PERFORMANCE/time_tmp_real.txt`;
	real_time2=`tail -n 1 $PATH_PERFORMANCE/time_tmp_real.txt`;

	total_real_time=`expr $real_time1 "+" $real_time2`;
	#rm $PATH_PERFORMANCE/time_tmp_real.txt;	
	echo "     $total_real_time" >> $PATH_PERFORMANCE/time_tmp_$file.txt;
	echo "********************" >> $PATH_PERFORMANCE/time_tmp_$file.txt;
	echo "volume:" >> $PATH_PERFORMANCE/time_tmp_$file.txt;
	if [ $operation_tmp = "write" ]; then
		volume_tmp=`du -k $d_file | awk '{print $1}'`;
	else
		volume_tmp=$volume_read;
	fi
	
	echo "       $volume_tmp" >> $PATH_PERFORMANCE/time_tmp_$file.txt;
	echo "********************" >> $PATH_PERFORMANCE/time_tmp_$file.txt;

        volume_tmp=`expr $volume_tmp "*" 100`;	
	speed_tmp=`expr $volume_tmp "/" $total_real_time`;
	echo "the speed is:" >> $PATH_PERFORMANCE/time_tmp_$file.txt;
	echo "             $speed_tmp KB/s" >> $PATH_PERFORMANCE/time_tmp_$file.txt;
} 

################################################################################
# Function:     usage() 
# Description   
# give out how to and using example
################################################################################ 
usage()
{
	echo "usage"
} 
 
# main function 
 
RC=0  

setup || exit $RC 

#TODO check parameter
echo "$#"
if [ $# = "3" ]; then
	echo "do "cp" operation!";
	execute_command=$*
	echo $execute_command;
	cp_tmp=$1;
	s_file_tmp=$2;
	d_file_tmp=$3;
	echo $3 >> $PATH_PERFORMANCE/device_tmp.txt
	device_directory=`awk -F /  '{print $3}' $PATH_PERFORMANCE/device_tmp.txt`;
	echo $device_directory;
	d_file=$3;
	echo $d_file;
	operation_tmp="write"
fi 
if [ $# = "5" ]; then
	echo "do "dd" write operation!"
	execute_command=$*
	echo $execute_command;
	dd_tmp=$1;
	if_tmp=$2;
	of_tmp=$3;
	bs_tmp=$4;
	count_tmp=$5;
	echo $3 >> $PATH_PERFORMANCE/device_tmp.txt
	device_directory=`awk -F /  '{print $3}' $PATH_PERFORMANCE/device_tmp.txt`;
	echo $device_directory;
	d_file=`awk -F =  '{print $2}' $PATH_PERFORMANCE/device_tmp.txt`;
	echo $d_file;
	operation_tmp="write"
fi
if [ $# = "7" ]; then
	echo "do "dd" read operation!"
	execute_command="$3 $4 $5 $6 $7"
	echo $execute_command
	device_directory=$1;
	echo $device_directory > $PATH_PERFORMANCE/device_tmp.txt;
	volume_read=$2
	dd_tmp=$3;
	if_tmp=$4;
	of_tmp=$5;
	bs_tmp=$6;
	count_tmp=$7;
	operation_tmp="read"
	
fi

rm $PATH_PERFORMANCE/device_tmp.txt;

case $device_directory in 
	# for write
	"flc")
        	echo "******do nand write performace test******";
		device_tmp="nand"
                ;;
	"flb")
             	echo "******do nor write performace test******";
		device_tmp="nor"
                ;;
	"mmcblk0p1")
             	echo "******do sd write performace test******";
		device_tmp="sd"
                ;;
	"ata1")
             	echo "******do ata write performace test******";
		device_tmp="ata"
                ;;
	"msc")
             	echo "******do usbh write performace test******";
		device_tmp="usbh"
                ;;
	# for read
	"nand"|"NAND")
        	echo "******do nand read performace test******";
		device_tmp="nand"
                ;;
	"nor"|"NOR")
             	echo "******do nor read performace test******";
		device_tmp="nor"
                ;;
	"sd"|"SD"|"mmc"|"MMC")
             	echo "******do sd read performace test******";
		device_tmp="sd"
                ;;
	"ata"|"ATA")
             	echo "******do ata read performace test******";
		device_tmp="ata"
                ;;
	"usbh"|"USBH")
             	echo "******do usbh read performace test******";
		device_tmp="usbh"
                ;;	
	*) 
		usage;
		;;
esac

performance_test || exit $RC
 

