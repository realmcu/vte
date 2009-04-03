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
# File Name:    original growfiles_list.sh
# Total Tests:       30 
# Test Strategy: file system stress test
# 
# Input:	none
#
# Return:     
#
# Use command "./growfiles_list.sh to stress test file system


# Function:     main
#
# Description:  - Execute all tests, exit with test status.
#
# Exit:         - zero on success
#               - the last error test number on failure.
#

# return value
RC=0
# the last error test number
last_err_no=0

growfiles -W gf01 -b -e 1 -u -i 0 -L 20 -w -C 1 -l -I r -T 10 glseek20 glseek20.2 \
|| RC=$?&& last_err_no=`expr $last_err_no + 1`

sleep 5

growfiles -W gf02 -b -e 1 -L 10 -i 100 -I p -S 2 -u -f gf03_ \
|| RC=$?&& last_err_no=`expr $last_err_no + 1`
sleep 5

growfiles -W gf03 -b -e 1 -g 1 -i 1 -S 150 -u -f gf05_ \
|| RC=$?&& last_err_no=`expr $last_err_no + 1`
sleep 5

growfiles -W gf04 -b -e 1 -g 4090 -i 500 -t 39000 -u -f gf06_ \
|| RC=$?&& last_err_no=`expr $last_err_no + 1`
sleep 5

growfiles -W gf05 -b -e 1 -g 5000 -i 500 -t 49900 -T10 -c9 -I p -u -f gf07_ \
|| RC=$?&& last_err_no=`expr $last_err_no + 1`
sleep 5

growfiles -W gf06 -b -e 1 -u -r 1-5000 -R 0--1 -i 0 -L 30 -C 1 g_rand10 g_rand10.2 \
|| RC=$?&& last_err_no=`expr $last_err_no + 1`
sleep 5

growfiles -W gf07 -b -e 1 -u -r 1-5000 -R 0--2 -i 0 -L 30 -C 1 -I p g_rand13 g_rand13.2 \
|| RC=$?&& last_err_no=`expr $last_err_no + 1`
sleep 5

growfiles -W gf08 -b -e 1 -u -r 1-5000 -R 0--2 -i 0 -L 30 -C 1 g_rand11 g_rand11.2 \
|| RC=$?&& last_err_no=`expr $last_err_no + 1`
sleep 5

growfiles -W gf09 -b -e 1 -u -r 1-5000 -R 0--1 -i 0 -L 30 -C 1 -I p g_rand12 g_rand12.2 \
|| RC=$?&& last_err_no=`expr $last_err_no + 1`
sleep 5

growfiles -W gf10 -b -e 1 -u -r 1-5000 -i 0 -L 30 -C 1 -I l g_lio14 g_lio14.2 \
|| RC=$?&& last_err_no=`expr $last_err_no + 1`
sleep 5

growfiles -W gf11 -b -e 1 -u -r 1-5000 -i 0 -L 30 -C 1 -I L g_lio15 g_lio15.2 \
|| RC=$?&& last_err_no=`expr $last_err_no + 1`
sleep 5

mkfifo gffifo17; growfiles -b -W gf12 -e 1 -u -i 0 -L 30 gffifo17 \
|| RC=$?&& last_err_no=`expr $last_err_no + 1`
sleep 5

mkfifo gffifo18; growfiles -b -W gf13 -e 1 -u -i 0 -L 30 -I r -r 1-4096 gffifo18 \
|| RC=$?&& last_err_no=`expr $last_err_no + 1`
sleep 5

growfiles -W gf14 -b -e 1 -u -i 0 -L 20 -w -l -C 1 -T 10 glseek19 glseek19.2 \
|| RC=$?&& last_err_no=`expr $last_err_no + 1`
sleep 5

growfiles -W gf15 -b -e 1 -u -r 1-49600 -I r -u -i 0 -L 120 Lgfile1 \
|| RC=$?&& last_err_no=`expr $last_err_no + 1`
sleep 5

growfiles -W gf16 -b -e 1 -i 0 -L 120 -u -g 4090 -T 100 -t 408990 -l -C 10 -c 1000 -S 10 -f Lgf02_ \
|| RC=$?&& last_err_no=`expr $last_err_no + 1`
sleep 5

growfiles -W gf17 -b -e 1 -i 0 -L 120 -u -g 5000 -T 100 -t 499990 -l -C 10 -c 1000 -S 10 -f Lgf03_ \
|| RC=$?&& last_err_no=`expr $last_err_no + 1`
sleep 5

growfiles -W gf18 -b -e 1 -i 0 -L 120 -w -u -r 10-5000 -I r -T 10 -l -S 2 -f Lgf04_ \
|| RC=$?&& last_err_no=`expr $last_err_no + 1`
sleep 5

growfiles -W gf19 -b -e 1 -g 5000 -i 500 -t 49900 -T10 -c9 -I p -o O_RDWR,O_CREAT,O_TRUNC -u -f gf08i_ \
|| RC=$?&& last_err_no=`expr $last_err_no + 1`
sleep 5

growfiles -W gf20 -D 0 -b -i 0 -L 60 -u -B 1000b -e 1 -r 1-256000:512 -R 512-256000 -T 4 gfbigio-$$ \
|| RC=$?&& last_err_no=`expr $last_err_no + 1`
sleep 5

growfiles -W gf21 -D 0 -b -i 0 -L 60 -u -B 1000b -e 1 -g 20480 -T 10 -t 20480 gf-bld-$$ \
|| RC=$?&& last_err_no=`expr $last_err_no + 1`
sleep 5

growfiles -W gf22 -D 0 -b -i 0 -L 60 -u -B 1000b -e 1 -g 20480 -T 10 -t 20480 gf-bldf-$$ \
|| RC=$?&& last_err_no=`expr $last_err_no + 1`
sleep 5

growfiles -W gf23 -D 0 -b -i 0 -L 60 -u -B 1000b -e 1 -r 512-64000:1024 -R 1-384000 -T 4 gf-inf-$$ \
|| RC=$?&& last_err_no=`expr $last_err_no + 1`
sleep 5

growfiles -W gf24 -D 0 -b -i 0 -L 60 -u -B 1000b -e 1 -g 20480 gf-jbld-$$ \
|| RC=$?&& last_err_no=`expr $last_err_no + 1`
sleep 5

growfiles -W gf25 -D 0 -b -i 0 -L 60 -u -B 1000b -e 1 -r 1024000-2048000:2048 -R 4095-2048000 -T 1 gf-large-gs-$$ \
|| RC=$?&& last_err_no=`expr $last_err_no + 1`
sleep 5

growfiles -W gf26 -D 0 -b -i 0 -L 60 -u -B 1000b -e 1 -r 128-32768:128 -R 512-64000 -T 4 gfsmallio-$$ \
|| RC=$?&& last_err_no=`expr $last_err_no + 1`
sleep 5

growfiles -W gf27 -b -D 0 -w -g 8b -C 1 -b -i 1000 -u gfsparse-1-$$ \
|| RC=$?&& last_err_no=`expr $last_err_no + 1`
sleep 5

growfiles -W gf28 -b -D 0 -w -g 16b -C 1 -b -i 1000 -u gfsparse-2-$$ \
|| RC=$?&& last_err_no=`expr $last_err_no + 1`
sleep 5

growfiles -W gf29 -b -D 0 -r 1-4096 -R 0-33554432 -i 0 -L 60 -C 1 -u gfsparse-3-$$ \
|| RC=$?&& last_err_no=`expr $last_err_no + 1`
sleep 5

growfiles -W gf30 -D 0 -b -i 0 -L 60 -u -B 1000b -e 1 -o O_RDWR,O_CREAT,O_SYNC -g 20480 -T 10 -t 20480 gf-sync-$$ \
|| RC=$?&& last_err_no=`expr $last_err_no + 1`
sleep 5

# exit value
if [ $RC -eq 0 ]
then
    exit $RC
else
    exit $last_err_no
fi

