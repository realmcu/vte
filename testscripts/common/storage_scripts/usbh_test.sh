#!/bin/bash

#/*================================================================================================= 

#    Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved 
#    THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT 
#    BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF 
#    Freescale Semiconductor, Inc. 
#
#====================================================================================================
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#Ziye Yang                   19/08/2008       n/a        initialization of usb host storage remove test application
#====================================================================================================
#Portability:  ARM GCC  gnu compiler
#==================================================================================================*/
#
#/*==================================================================================================
#Total Tests:           1
#Test Executable Name:  usbh_test.sh
#Test Strategy:         
#=================================================================================================*/


# STR1: storage name;
# Use command "./usbh_test.sh [STR1] [STR2] to run the problem 

#initialize 
TMP_RC=0
rc=0
ACTION=""

anal_res()
{
	if [ $TMP_RC -eq 0 ]; then
		echo " -----------------------------------------------------------------"
		echo " USBH $ACTION storage pass!!!"
    		echo " -----------------------------------------------------------------"
	else
		rc=1
		echo " -----------------------------------------------------------------"
		echo " USBH $ACTION storage fail "
		echo " -----------------------------------------------------------------"
	fi
}

probe_test()
{
	line_num1=0;
	line_num2=0;

	line_num1=`cat /proc/partitions | wc -l`;

	if [ -e /mnt/msc ]; then
		rm -rf /mnt/msc
	fi
	mkdir /mnt/msc
	modprobe ehci-hcd
	sleep 10

	line_num2=`cat /proc/partitions | wc -l`;

	if [ $line_num1 -lt $line_num2 ]; then
       		TMP_RC=0
	else
		echo "Please input u disk!"
        	TMP_RC=1
	fi

	if [ $TMP_RC -eq 0 ];then
		FILE=`cat /proc/partitions | tail -n 1 | awk '{print $4}'`
		if [ -e /dev/$FILE ]; then
			mount -t vfat /dev/$FILE /mnt/msc
			TMP_RC=$TMP_RC||$?
			anal_res
		else
			echo "no such device:$FILE"
			rc=1
		fi
	else
		lsmod | grep ehci*
		if [ $? -eq 0 ]; then
			echo "modprobe module success!"
			rc=0
		else
			echo "modprobe module fail!"
			rc=1
		fi
	fi
}

remove_test()
{
	lsmod | grep ehci*
	if [ $? -eq 0 ]; then
		mount | grep msc
		if [ $? -eq 0 ]; then
			umount /mnt/msc
		fi
 
 		modprobe -r ehci-hcd
 		sleep 5

		lsmod | grep ehci*
		if [ $? -eq 0 ]; then
 			TMP_RC=1
		else
			TMP_RC=0
		fi
 		anal_res
	else
		echo "the ehci-hcd module is not exist!"
	fi
}

echo "----USB HOST TEST BEGIN--"
case $1 in 
	"P" | "Probe") 
		ACTION="probe";
		probe_test
		;;
	"R" | "Remove")
		ACTION="remove";
		remove_test
		;;
	*) 
		rc=1
		echo "please input right parameter! P or Probe,R,Remove)";
		;;
esac

echo""
echo " final script      		RESULT   "
echo " -------------------------------------------"
if [ $rc -eq 0 ]; then
	echo " usbh_test.sh: $ACTION           TPASS    " 
else
	echo " usbh_test.sh: $ACTION           TFAIL    "
fi
echo""
exit $rc




