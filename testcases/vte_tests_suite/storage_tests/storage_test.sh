#Copyright 2008-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
#Victor Cui                  14/10/2008      n/a        add nor function
#Victor Cui                  22/10/2008      n/a        modify copy_test function to automation
#                                                       adapt the fact device volume
#Victor Cui                  17/09/2009      n/a        add check_validity
#====================================================================================================
#Portability:  ARM GCC  gnu compiler
#==================================================================================================*/
#
#/*==================================================================================================
#Total Tests:           1
#Test Executable Name:  storage_test.sh
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
# Use command "./storage_test.sh [STR1] [STR2] [SRT3] [STR4] [SRT5] [STR6] [STR7]"to test storage CDF


#initialize 
 TMP_RC=0
 rc=0
 MAX_TIMES=0
 TMP_TIME=0
 C_TIMES=0
 C_SIZE=0
 S_SIZE=0
 P_NUMBER=0 
 S_PATH=""
 D_PATH=""
 FILE=""
 PLATFORM=""
 PLATFORM_NAND_PARTITION=4
 D_TYPE=""
 if_tmp=""

 
probe_platform()
{
	find=0;
	
	#change the judge method because of 31 and 32 cpuinfo "Hardware" item 
	# "Hardware        : Freescale MX31/MX32 3-Stack Board"

	# add a blank to grep "*.*"
	# Revision        : 35120

	find=`cat /proc/cpuinfo | grep "Revision" | grep " 25.*" | wc -l`;
	if [ $find -eq 1 ]; then
		PLATFORM="MX25";
	fi
	
	find=`cat /proc/cpuinfo | grep "Revision" | grep " 31.*" | wc -l`;
	if [ $find -eq 1 ]; then
		PLATFORM="MX31";
	fi
	
	find=`cat /proc/cpuinfo | grep "Revision" | grep " 32.*" | wc -l`;
	if [ $find -eq 1 ]; then
		PLATFORM="MX32";
	fi
	
	find=`cat /proc/cpuinfo | grep "Revision" | grep " 35.*" | wc -l`;
	if [ $find -eq 1 ]; then
   		PLATFORM="MX35";
   		PLATFORM_NAND_PARTITION=10
	fi
	
	find=`cat /proc/cpuinfo | grep "Revision" | grep " 37.*" | wc -l`;
	if [ $find -eq 1 ]; then
   		PLATFORM="MX37";
   		PLATFORM_NAND_PARTITION=5
	fi

	find=`cat /proc/cpuinfo | grep "Revision" | grep " 51.*" | wc -l`;
	if [ $find -eq 1 ]; then
   		PLATFORM="MX51";
	fi

	echo "The platform is $PLATFORM"

	df | grep "/mnt/nfs"
	if [ $? -eq 0 ]; then
		if_tmp=/mnt/nfs/test_stream/storage/tempfile
	else
		if_tmp=/dev/urandom
	fi
	echo $if_tmp	
}
check_validity()
{
        S_PATH_TMP=""
        D_PATH_TMP=""
        RW_TMP=""
        DEVICE_NODE_TMP=""

        case $1 in
        "F" | "Format")
                case $2 in
                        "SD")
                                S_PATH_TMP="/mnt/mmcblk0p$3"
                                D_PATH_TMP="/mnt/mmcblk0p$3"
                                ;;
                        "HDD")
                                S_PATH_TMP="/mnt/ata$3"
                                D_PATH_TMP="/mnt/ata$3"
                                ;;
                        "USBH")
                                S_PATH_TMP="/mnt/msc"
                                D_PATH_TMP="/mnt/msc"
                                ;;
                        "NAND")
                                S_PATH_TMP="/mnt/flc"
                                D_PATH_TMP="/mnt/flc"
                                ;;
                        "NOR")
                                S_PATH_TMP="/mnt/flb"
                                D_PATH_TMP="/mnt/flb"
                                ;;
                esac
                ;;
        "C" | "Copy" | "D" | "Delete" | "CD" | "CopyDelete" )
                S_PATH_TMP=$2
                D_PATH_TMP=$3
                ;;
        "CDF" | "CopyDeleteFormat")
                S_PATH_TMP=$4
                D_PATH_TMP=$5
                ;;
        "STRESS")
                S_PATH_TMP=$2
                D_PATH_TMP=$2
                ;;
        "--help" | "--h")
                print_help
                exit $rc
                ;;
        *)
                echo "please input right parameter! F or Format,CopyDelete,CopyDeleteFormat)";
                ;;
esac

# check the directory
        if [ ! -d "$S_PATH_TMP" ]; then
                echo "The S_PATH: $S_PATH_TMP are not exist, please check!"
                rc=1
                exit $rc
        fi
        if [ ! -d "$D_PATH_TMP" ]; then
                echo "The D_PATH: $D_PATH_TMP are not exist, please check!"
                rc=1
                exit $rc
        fi

# check the D_PATH write property
	# special operation for some subdirectory(storage_test_concurrent.sh)
	# eg: /mnt/msc/test --> /mnt/msc
	NF_TMP=`echo $D_PATH_TMP | awk -F / '{print NF}'`
	if [ $NF_TMP -gt 3 ]; then
		D_PATH_TMP=`echo $D_PATH_TMP | awk -F / '{print "/" $2 "/" $3}'`
	fi
	# end

	DEVICE_NODE_TMP=`cat /proc/mounts | grep $D_PATH_TMP | awk '{print $1}'`
        if [ -z $DEVICE_NODE_TMP ]; then
                echo "Not find the device node mount to $D_PATH_TMP, please check! "
                rc=1
                exit $rc
        fi
        RW_TMP=`cat /proc/mounts | grep $D_PATH_TMP | awk '{print $4}' | awk -F , '{print $1}'`
        if [ $RW_TMP != "rw" ]; then
                echo "The D_PATH: $D_PATH_TMP can not write, please check!"
                rc=1
                exit $rc
        fi

# check the device node
        ls /dev/ | grep `echo $DEVICE_NODE_TMP | awk -F / '{print $3}'`
        if [ $? -ne 0 ]; then
                echo "The DEVICE_NODE: $DEVICE_NODE_TMP is not exist, please check!"
                rc=1
                exit $rc
        fi
}
 anal_copy_res()
{
	if [ $TMP_RC -eq 0 ]; then
		echo " -----------------------------------------------------------------"
		echo " copy $FILE from $S_PATH to $D_PATH, the ${TMP_TIME}th pass!!!"
		echo " -----------------------------------------------------------------"  
	else
		rc=1
		echo " -----------------------------------------------------------------"
		echo " The ${TMP_TIME}th step copy fail "
		echo " -----------------------------------------------------------------"
	fi
}
 
 anal_delete_res()
{
	if [ $TMP_RC -eq 0 ]; then
		echo " -----------------------------------------------------------------"
		echo " delete $FILE from $D_PATH $TMP_TIME pass!!!"
		echo " -----------------------------------------------------------------"  
	else   
		rc=1
		echo " -----------------------------------------------------------------"
		echo "The ${TMP_TIME}th step delete fail "
		echo " -----------------------------------------------------------------"
	fi
}

 anal_format_res()
{
	if [ $TMP_RC -eq 0 ]; then
		echo " -----------------------------------------------------------------"
		echo " format partition $P_NUMBER on $D_TYPE pass!!!"
		echo " -----------------------------------------------------------------"
	else
		rc=1
		echo " -----------------------------------------------------------------"
		echo " format partition $P_NUMBER on $D_TYPE fail "
		echo " -----------------------------------------------------------------"
	fi
}

 anal_stress_res()
{
	if [ $TMP_RC -eq 0 ]; then
		echo " -----------------------------------------------------------------"
		echo " stress test $C_SIZE on $S_PATH the ${TMP_TIME}th pass!!!"
		echo " -----------------------------------------------------------------"
	else
		rc=1
		echo " -----------------------------------------------------------------"
		echo " stress test $C_SIZE on $S_PATH the ${TMP_TIME}th fail "
		echo " -----------------------------------------------------------------"
	fi
}

format_test()
{
	if [ $D_TYPE = "HDD" ]; then	
		#if [ $PLATFORM = "MX31" ] || [ $PLATFORM = "MX32" ]; then
			#device=hda
		#else
			device=sda
		#fi
		cd /
		#umount /mnt/${device}${P_NUMBER}
		umount /mnt/ata${P_NUMBER}
		mkfs.vfat /dev/${device}${P_NUMBER}
		
		#mount -t vfat /dev/${device}${P_NUMBER}  /mnt/${device}${P_NUMBER}
		mount -t vfat /dev/${device}${P_NUMBER}  /mnt/ata${P_NUMBER}	
	
		sleep 5
  		#TMP_V=`mount |grep /mnt/${device}${P_NUMBER} |wc -l`
  		TMP_V=`mount |grep /mnt/ata${P_NUMBER} |wc -l`
  		if [ $TMP_V -eq 0 ]; then
  			TMP_RC=1
  		fi
  		anal_format_res	
  	elif [ $D_TYPE = "SD" ]; then
  		cd /
  		umount /mnt/mmcblk0p$P_NUMBER/
  		mkfs.vfat /dev/mmcblk0p$P_NUMBER
  	
  		#restore the environement
  		mount -t vfat /dev/mmcblk0p$P_NUMBER /mnt/mmcblk0p$P_NUMBER/  
  	
		sleep 5
  		TMP_V=`mount |grep /mnt/mmcblk0p$P_NUMBER |wc -l`
  		if [ $TMP_V -eq 0 ]; then
  		TMP_RC=1
  		fi
  		anal_format_res
  	elif [ $D_TYPE = "NAND" ]; then
 		if [ $P_NUMBER -eq $PLATFORM_NAND_PARTITION ]; then
 			cd /
 			umount /mnt/flc
 			flash_eraseall /dev/mtd$P_NUMBER > /dev/null
 			
			#mkfs.jffs2 -c /dev/mtdblock$P_NUMBER
 			mount -t jffs2 /dev/mtdblock$P_NUMBER /mnt/flc
 			
 			sleep 5
 			TMP_V=`mount |grep /mnt/flc |wc -l`
 			if [ $TMP_V -eq 0 ]; then
 				TMP_RC=1
  			fi
  			anal_format_res
 		else
 			echo "Set parameter STR3=4(MX31),10(MX35),5(MX37) to format NAND storage"
 			rc=1
 		fi
	elif [ $D_TYPE = "NOR" ]; then
 		cd /
		umount /mnt/flb
 		flash_eraseall /dev/mtd$P_NUMBER > /dev/null
 			
		#mkfs.jffs2 -c /dev/mtdblock$P_NUMBER
 		mount -t jffs2 /dev/mtdblock$P_NUMBER /mnt/flb
 			
 		sleep 5
 		TMP_V=`mount |grep /mnt/flb |wc -l`
 		if [ $TMP_V -eq 0 ]; then
 			TMP_RC=1
  		fi
  		anal_format_res
 	elif [ $D_TYPE = "USBH" ];then
 		device_partition=`mount | grep /mnt/msc | awk '{print $1}'`
  		umount /mnt/msc
  		mkfs.vfat $device_partition
  	
  		#restore the environement
  		mount -t vfat $device_partition /mnt/msc 
  	
		sleep 5
  		TMP_V=`mount |grep /mnt/msc |wc -l`
  		if [ $TMP_V -eq 0 ]; then
  			TMP_RC=1
  		fi
  		anal_format_res
 	else 
 		echo "Invalid storage type"
 		rc=1
 	fi
}

copy_test()
{
	if [ ! -d "$S_PATH" ] || [ ! -d "$D_PATH" ]; then
		echo "FAIL: the directory may be not exist, Please check!"
		echo " storage_test.sh           TFAIL    "
		exit 1
	fi

	s_size=`expr $C_SIZE "*" 1024`

	s_path_left=`df -k /$S_PATH|awk '{print $4}'`
        s_path_left=`expr substr "$s_path_left" 10 $$`
	if [ "$S_PATH" = "/mnt/flc" ] || [ "$S_PATH" = "/mnt/flb" ];then
		# 3% is used for log or backup data(jffs2 filesystem)
		s_path_left=`expr $s_path_left "*" 100 "/" 103`
	fi

        d_path_left=`df -k /$D_PATH|awk '{print $4}'`
        d_path_left=`expr substr "$d_path_left" 10 $$`
	if [ "$D_PATH" = "/mnt/flc" ] || [ "$D_PATH" = "/mnt/flb" ]  ;then
		# 3% is used for log or backup data(jffs2 filesystem)
		d_path_left=`expr $d_path_left "*" 100 "/" 103`
	fi

	if [ -z $s_path_left ] || [ -z $d_path_left ]; then
		echo "FAIL: the directory may be not exist, Please check!!"
		echo " storage_test.sh           TFAIL    "
		exit 1
        fi

	if [ "$S_PATH" = "$D_PATH" ];then
		C_TIMES_TMP=`expr $C_TIMES "+" 1`
		
                max_times=`expr $s_path_left "/" $s_size`
		
		if [ "$max_times" -lt "$C_TIMES_TMP" ]; then
			C_SIZE=`expr $s_path_left "/" $C_TIMES_TMP "/" 1024`
			echo "max_times is $max_times, need to modify C_SIZE to $C_SIZE"
		fi
        else
        	if [ "$s_path_left" -ge "$s_size" ]; then
               		tmp1=`expr $d_path_left "/" $s_size`
                	if [ "$tmp1" -gt "0" ] && [ "$tmp1" -lt "$C_TIMES" ]; then
                        	C_TIMES=$tmp1;
				echo "need to modify C_TIMES to $C_TIMES"
                	fi
               		if [ "$tmp1" -eq "0" ]; then
                        	C_SIZE=`expr $d_path_left "/" $C_TIMES "/" 1024`
				echo "need to modify C_SIZE to $C_SIZE"
                	fi
        	else
                	C_SIZE=`expr $s_path_left "/" 1024`
			echo "because of S_PATH_LEFT too small, need to modify C_SIZE to $C_SIZE"
                	s_size=`expr $C_SIZE "*" 1024`
                	tmp2=`expr $d_path_left "/" $s_size`
                	if [ "$tmp2" -gt "0" ] && [ "$tmp2" -lt "$C_TIMES" ]; then
                        	C_TIMES=`expr $d_path_left "/" $s_size`
				echo "need to modify C_TIMES to $C_TIMES"
                	fi
                	if [ "$tmp2" -eq "0" ]; then
                        	C_SIZE=`expr $d_path_left "/" $C_TIMES "/" 1024`
				echo "need to modify C_SIZE to $C_SIZE"
                	fi
        	fi
	fi
		
	cd /$S_PATH
		
	#produce the tempfile
	FILE=tempfile
	#dd if=/dev/zero of=$FILE bs=1024 count=`expr $C_SIZE "*" 1024`
	#dd if=/dev/urandom of=$FILE bs=1024 count=`expr $C_SIZE "*" 1024`
	dd if=$if_tmp of=$FILE bs=1024 count=`expr $C_SIZE "*" 1024`
		
	i=0
	while [ $i -lt $C_TIMES ]; do
		if [ -e "/$D_PATH/test$i" ]; then
			rm -rf /$D_PATH/test$i
                	mkdir /$D_PATH/test$i
		else
                	mkdir /$D_PATH/test$i
		fi
  		cp /$S_PATH/$FILE /$D_PATH/test$i 
    		TMP_RC=$?
    	
    		#diff /$S_PATH/$FILE /$D_PATH/test$i/$FILE
    		#TMP_RC=$?||$TMP_RC
		     
    		D_SIZE=`ls -l /$D_PATH/test$i/$FILE|awk '{print $5}'`
  		TMP_RC=$TMP_RC||$?
  	
  		if [ $S_SIZE -eq $D_SIZE ]; then
			TMP_RC=$TMP_RC||$?;
   		fi
   		TMP_TIME=$i
   		anal_copy_res
   		i=`expr $i "+" 1`
  	done
}


delete_test()
{
  	# delete the produce file
        FILE="tempfile"
	if [ -f /$S_PATH/$FILE ]; then
		rm -rf /$S_PATH/$FILE 
	else
		echo "the /$S_PATH/$FILE is not exist! Please check your parameters"
	fi

	i=0
	while [ $i -lt $C_TIMES ]; do
		if [ -e "/$D_PATH/test$i" ]; then 
			rm -rf /$D_PATH/test$i
    			TMP_RC=$?
    			TMP_TIME=$i
   		 	anal_delete_res
		else
			echo "the /$D_PATH/test$i is not exist! Please check your parameters"
		fi
		i=`expr $i "+" 1`
	done
}

stress_test()
{
	if [ $C_SIZE = "min" ]; then
		SIZE=1;
	fi
	
	if [ $C_SIZE = "max" ]; then
		SIZE=`df | grep $S_PATH | awk '{print $4}'`;
		if [ "$S_PATH" = "/mnt/flc" ] || [ "$S_PATH" = "/mnt/flb" ];then
			# 4% is used for log or backup data(jffs2 filesystem)
			SIZE=`expr $SIZE "*" 100 "/" 104 `
		fi
		# the size must be divided exactly by the 4K, vfat is alignment with 4K 
		SIZE=`expr $SIZE "/" 4 "*" 4`
	fi

	cd /$S_PATH

	i=0
	while [ $i -lt $C_TIMES ]; do
  	        #dd if=/dev/zero of=tempfile bs=1024 count=$SIZE
  	        #dd if=/dev/urandom of=tempfile bs=1024 count=$SIZE
  	        dd if=$if_tmp of=tempfile bs=1024 count=$SIZE
    		TMP_RC=$?
		rm tempfile
		TMP_RC=$TMP_RC||$?
		TMP_TIME=$i
		anal_stress_res
        	i=`expr $i "+" 1`
	done
}


print_help()
{
	echo "----Help information for the script: the usage of parameters----"
	echo ""
	echo "Parameter1: C(Copy), D(Delete), F(Format), CDF(CopyDeteleTest), STRESS"
	echo "Parameter2: device type of storage: SD, NAND, HDD, USBH"
	echo "Parameter3: Partition Number of the device"
	echo "Parameter4: Source Folder of the file to be copy"
	echo "Parameter5: Destination Folder of the file to be copy"
	echo "Parameter6: File Size in Megebyte"
	echo "Patramter7: copy times"
	echo ""
}



echo ""
echo "[Storage Copy/Delete/Format test begin]"

probe_platform
check_validity $*
case $1 in 
	"F" | "Format")
                D_TYPE=$2
                if [ $D_TYPE = "SD" ] || [ $D_TYPE = "HDD" ]; then
                        P_NUMBER=$3
                fi
                if [ $D_TYPE = "USBH" ]; then
                        P_NUMBER=""
                fi
                if [ $D_TYPE = "NAND" ]; then
                        P_NUMBER=$PLATFORM_NAND_PARTITION
                fi
		if [ $D_TYPE = "NOR" ]; then
			if [ $PLATFORM = "MX35" ]; then
                        	P_NUMBER=2
			else
				echo "Only Mx35 platform has nor device" 
			fi
                fi
                format_test
                ;;
	"C" | "Copy")
		S_PATH=$2
		D_PATH=$3
		C_SIZE=$4
		C_TIMES=$5
		copy_test
		;;
	"D" | "Delete")
	 	S_PATH=$2
	 	D_PATH=$3
         	C_SIZE=$4
	 	C_TIMES=$5
	 	delete_test
		;;
	"CD" | "CopyDelete")
	 	 S_PATH=$2
		D_PATH=$3
		C_SIZE=$4
		C_TIMES=$5
		copy_test
		delete_test
		;;
	"CDF" | "CopyDeleteFormat")	
		D_TYPE=$2
		P_NUMBER=$3
		S_PATH=$4
		D_PATH=$5
		C_SIZE=$6
		C_TIMES=$7
		copy_test
		delete_test
		format_test
		;;  
        "STRESS")
		S_PATH=$2
		C_SIZE=$3
		C_TIMES=$4
		stress_test
		;;
                                	
	"--help" | "--h")
		print_help
		exit $rc
		;;
	*) 
		echo "please input right parameter! F or Format,CopyDelete,CopyDeleteFormat)";
		;;
esac

echo""
echo " final script      		RESULT   "
echo " -------------------------------------------"
	if [ $rc -eq 0 ]; then
		echo " storage_test.sh           TPASS    "
	else
		echo " storage_test.sh           TFAIL    "
	fi
echo""
exit $rc

