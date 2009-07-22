#Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
#    @file   module_verify.sh
#
#    @brief  this shell script is used to test the module insert and remove 
#             function.
#
################################################################################
#Revision History: 
#                            Modification     ClearQuest 
#Author                          Date          Number    Description of Changes 
#-------------------------   ------------    ----------  -----------------------
#Victor Cui/b19309            4/6/2009          N/A          Initial version 
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
	#trap "cleanup" 0 
 
	#TODO add setup scripts
	Platform=`cat /proc/cpuinfo | grep "Revision" | awk '{print $3}'| cut -c1-2`;

	echo "--------------------begin--------------------"
	echo "Mx${Platform}: do all modules insert and remove two times"
	# do all modules insert and remove two times
	C_TIMES=2;
	i=0;

	if [ -e module_verify_tmp0 ]; then	
			rm module_verify_tmp0
	fi
	if [ -e module_verify_tmp1 ]; then	
			rm module_verify_tmp1
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
	lsmod | grep "fsl_otg_arc";
	if [ $? -eq 0 ] && [ $Platform -eq 25 ]; then
		modprobe -r fsl_otg_arc;
	fi
	echo "-------------------- end --------------------" 

	return $RC 
} 
 
 
################################################################################
# Function:     ***_test 
# Description   
#
################################################################################ 
module_verify_test() 
{
	#TODO add function test scripte here
	cat /lib/modules/`uname -r`/modules.dep | while read line
	do
   
	echo "--------------------"
 
	RC=0 
	is_insert=0;
	is_remove=0;

        line=`echo $line | awk -F : '{print $1}' | awk -F / '{print $NF}' | awk -F . '{print $1}'`;
	echo $line
	
	case $line in
		g_file_storage)
			if [ $Platform -eq 25 ]; then
				modprobe fsl_otg_arc;
				sleep 2;
			fi
			mtd_nand_number=`cat /proc/mtd | wc -l`
			let mtd_nand_number=$mtd_nand_number-2
		  	echo "$mtd_nand_number ----"
			modprobe $line file=/dev/mtdblock$mtd_nand_number
			;;
		g_ether | g_serial)
			if [ $Platform -eq 25 ]; then
				modprobe fsl_otg_arc;
				sleep 2;
			fi
				modprobe $line;
			;;		
		*)
			modprobe $line;
			;;
	esac

	sleep 10
	lsmod
	line_tmp=`echo $line | sed "s/-/\./g" | sed "s/_/\./g"`
	is_insert=`lsmod | grep $line_tmp | wc -l`;
	if [ $is_insert -ge 1 ]; then
        	echo "insert $line is ok ----"
        	modprobe -r $line;
        	sleep 1
        	is_remove=`lsmod | grep $line_tmp | wc -l`
        	if [ $is_remove -eq 0 ]; then
               		echo "remove $line is ok ----"
        	else
			RC=1
                	echo "do not remove $line ****"
        	fi
	else
		RC=1
        	echo "do not insert $line ****"
       		modprobe -r $line
	fi

	if [ $RC -ne 0 ]; then
		echo "		$line  ---- fail" >> module_verify_tmp$i
	fi

	echo "--------------------"

	done
	
	#tst_resm TINFO "Test PASS"
} 


################################################################################
# Function:     anal_result() 
# Description   
# analyse the result 
################################################################################ 
anal_result()
{

	anal_i=0
	echo "******************************"
	while [ $anal_i -lt $C_TIMES ]; do
		echo "the $anal_i times result: "
		if [ -e module_verify_tmp$anal_i ]; then	
			cat module_verify_tmp$anal_i
			rm module_verify_tmp$anal_i
		else
			echo "all modules are ok!"
		fi

		let anal_i=$anal_i+1;
	done
	
	echo "******************************"
} 

################################################################################
# Function:     usage() 
# Description   
# give out how to and using example
################################################################################ 
usage()
{
	echo "insert and remove test function for all modules"
} 
 
# main function 
 
RC=0  
#TODO check parameter 

setup || exit $RC 
while [ $i -lt $C_TIMES ]; do
	module_verify_test || exit $RC
	let i=$i+1;
done
anal_result || exit $RC
cleanup || exit $RC


