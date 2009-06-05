#!/bin/sh
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
#
# Revision History:
#                          Modification     Tracking
# Author                       Date          Number    Description of Changes
#-----------------------   ------------    ----------  ---------------------
# Spring Zhang               03/07/2008       n/a        Initial ver. 
# Spring                     28/11/2008       n/a      Modify COPYRIGHT header
#############################################################################
# Portability:  ARM sh bash 
#
# File Name:    fs_perms_list.sh
# Total Tests:        21
# Test Strategy: file access permission test
# 
# Input:	none
#
# Return:     
#
# Use command "./fs_perms_list.sh to test file system access permission


# Function:     main
#
# Description:  - Execute all tests, exit with test status.
#
# Exit:         - zero on success
#               - the last error test number on failure.
#
#####################################
# Test if test.txt testx.file exsit!
#####################################
if [ ! -e "test.txt" ] || [ ! -e "testx.file" ]
then
    tst_resm TBROK "Test #1: test.txt and testx.file don't exist!"
    exit 255
fi


# return value
RC=0
# the last error test number
last_err_no=0

fs_perms 001 99 99 12 100 x 1 \
|| RC=$?&& last_err=`expr $last_err_no + 1`
sleep 1

fs_perms 010 99 99 200 99 x 1 \
|| RC=$?&& last_err=`expr $last_err_no + 1`
sleep 1

fs_perms 100 99 99 99 500 x 1 \
|| RC=$?&& last_err=`expr $last_err_no + 1`
sleep 1

fs_perms 002 99 99 12 100 w 1 \
|| RC=$?&& last_err=`expr $last_err_no + 1`
sleep 1

fs_perms 001 99 99 12 100 x 1 \
|| RC=$?&& last_err=`expr $last_err_no + 1`
sleep 1

fs_perms 020 99 99 200 99 w 1 \
|| RC=$?&& last_err=`expr $last_err_no + 1`
sleep 1

fs_perms 200 99 99 99 500 w 1 \
|| RC=$?&& last_err=`expr $last_err_no + 1`
sleep 1

fs_perms 004 99 99 12 100 r 1 \
|| RC=$?&& last_err=`expr $last_err_no + 1`
sleep 1

fs_perms 040 99 99 200 99 r 1 \
|| RC=$?&& last_err=`expr $last_err_no + 1`
sleep 1

fs_perms 004 99 99 12 100 r 1 \
|| RC=$?&& last_err=`expr $last_err_no + 1`
sleep 1

fs_perms 004 99 99 12 100 r 1 \
|| RC=$?&& last_err=`expr $last_err_no + 1`
sleep 1

fs_perms 000 99 99 99 99 w 0 \
|| RC=$?&& last_err=`expr $last_err_no + 1`
sleep 1

fs_perms 000 99 99 99 99 x 0 \
|| RC=$?&& last_err=`expr $last_err_no + 1`
sleep 1

fs_perms 010 99 99 99 500 x 0 \
|| RC=$?&& last_err=`expr $last_err_no + 1`
sleep 1

fs_perms 100 99 99 200 99 x 0 \
|| RC=$?&& last_err=`expr $last_err_no + 1`
sleep 1

fs_perms 020 99 99 99 500 w 0 \
|| RC=$?&& last_err=`expr $last_err_no + 1`
sleep 1

fs_perms 200 99 99 200 99 w 0 \
|| RC=$?&& last_err=`expr $last_err_no + 1`
sleep 1

fs_perms 040 99 99 99 500 r 0 \
|| RC=$?&& last_err=`expr $last_err_no + 1`
sleep 1

fs_perms 400 99 99 200 99 r 0 \
|| RC=$?&& last_err=`expr $last_err_no + 1`
sleep 1
# Two additional
fs_perms 755 50084 1026 0 0 r 1 \
|| RC=$?&& last_err=`expr $last_err_no + 1`
sleep 1

fs_perms 755 50084 1026 0 0 w 1 \
|| RC=$?&& last_err=`expr $last_err_no + 1`
sleep 1


# exit value
if [ $RC -eq 0 ]
then
    exit $RC
else
    exit $last_err_no
fi

