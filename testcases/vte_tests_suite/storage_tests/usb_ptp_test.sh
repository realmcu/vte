#!/bin/bash
##############################################################################
#
#  Copyright 2004-2008 Freescale Semiconductor, Inc. All Rights Reserved.
#
##############################################################################
#
#  The code contained herein is licensed under the GNU Lesser General Public
#  License.  You may obtain a copy of the GNU Lesser General Public License
#  Version 2.1 or later at the following locations:
#
#  http://www.opensource.org/licenses/lgpl-license.html
#  http://www.gnu.org/copyleft/lgpl.html
#
##############################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  ----------------------
#==============================================================================
#Portability:  ARM GCC  gnu compiler
#============================================================================*/
#
#/*===========================================================================
#Total Tests:           1
#Test Executable Name:  usb_PTP_test.sh
#Test Strategy:         
#============================================================================*/


# STR1: command options;
# Use command "./usb_PTP_test.sh [STR1] "to test usb ptp

#initialize 
 TMP_RC=0
 rc=0

 anal_res()
 {
  if [ $TMP_RC -eq 0 ]; then
    echo " -----------------------------------------------------------------"
     echo " usb PTP test on $command_options pass!!!"
    echo " -----------------------------------------------------------------"     
  else
    rc=1
    echo " -----------------------------------------------------------------"
    echo " usb PTP test on $command_options fail "
    echo " -----------------------------------------------------------------"
  fi
 }


 if [ $# -eq 1 ];
  then
    echo ""
    echo " [usb PTP $1 test begin] "
    command_options=$1
    
    
    if [ $command_options = "--auto-detect" ]; then
      echo "auto detect whether the camera can be talked to the board"
      ./gphoto2 --auto-detect
      TMP_RC=$?
      anal_res
    elif [ $command_options = "--storage-info" ]; then
        echo "get camera storage information"
        ./gphoto2 --storage-info
        TMP_RC=$?
        anal_res
     elif [ $command_options = "--list-ports" ]; then
        echo "get the USB ports info on board"
        ./gphoto2 --list-ports
        TMP_RC=$?
        anal_res
    elif [ $command_options = "--list-files" ]; then
        echo "list files in camera folders"
        ./gphoto2 -L --list-files
        TMP_RC=$?
        anal_res
    elif [ $command_options = "--get-all-files" ]; then
        echo "get all files in camera folders and these files will be in the current folder"
        ./gphoto2 -P --get-all-files
        TMP_RC=$?
        anal_res
     elif [ $command_options = "--get-all-raw-data" ]; then
        echo "get all raw data in camera folders and these files will be in the current folder"
        ./gphoto2 --get-all-raw-data
        TMP_RC=$?
        anal_res
    else
        echo "Invalid command options"
        rc=1
    fi      
    echo " [usb PTP $1 test finished]"                         

 else
     echo "Please enter parameter:[STR1] command options;"
     rc=1
 fi

 echo""
 echo " final script      		RESULT   "
 echo " -------------------------------------------"
 if [ $rc -eq 0 ];
  then
    echo " usb_PTP_test.sh           TPASS    "
  
  else
    echo " usb_PTP_test.sh           TFAIL    "
 fi

 echo""

 exit $rc





