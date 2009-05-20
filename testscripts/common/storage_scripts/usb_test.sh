#!/bin/sh

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
#ZiYe Yang                   15/08/2008       n/a        initialization of usb storage probe test application
#====================================================================================================
#Portability:  ARM GCC  gnu compiler
#==================================================================================================*/
#
#/*==================================================================================================
#Total Tests:           1
#Test Executable Name:  usb_test.sh
#Test Strategy:
#=================================================================================================*/


#STR1: P(Probe) or R(Remove)
#STR[2...n]: Device name, eg /dev/sda1, /dev/hda1
# Use command "./usb_test.sh [STR1] [STR2] [STR3]...."to test USB Probe/Remove


#initialize
TMP_RC=0
rc=0
FILES=""
ACTION=""

anal_res()
{
 if [ $TMP_RC -eq 0 ]; then
  echo " -----------------------------------------------------------------"
  echo " USB $ACTION $FILE storage pass!!!"
  echo " -----------------------------------------------------------------"
 else
  rc=1
  echo " -----------------------------------------------------------------"
  echo " USB $ACTION $FILE storage fail "
  echo " -----------------------------------------------------------------"
 fi
}
remove_test()
{
 modprobe -r g_file_storage
 TMP_RC=$?
 anal_res
}
probe_test()
{
        #$modprobe arcotg_udc
 modprobe g_file_storage file=${FILES} removable=1
 TMP_RC=$?
 anal_res
}

D_NUMBER=$#
Total_String=$@
i=0
for d_name in $@
do
 echo $d_name
 if [ $i -gt 0 ];then
  if [ -e $d_name ];then
   if [ -z $FILES ];then
    FILES=${d_name}
   else
    FILES=${FILES}","${d_name}
   fi
  else
   echo "Please enter the correct device"
   exit 1
  fi
 fi
 i=`expr $i "+" 1`
done
echo $FILES


echo "----USB TEST BEGIN--"
case $1 in
 "P" | "Probe")
  ACTION="Probe";
  probe_test
  ;;
 "R" | "Remove")
  ACTION="Remove";
  remove_test
  ;;
 *)
  rc=1
  echo "please input right parameter! P or Probe,R,Remove)";
  ;;
esac

echo""
echo " final script RESULT   "
echo " -------------------------------------------"
if [ $rc -eq 0 ]; then
 echo " usb_test.sh: $ACTION           TPASS    "
else
 echo " usb_test.sh: $ACTION           TFAIL    "
fi
echo""
exit $rc






