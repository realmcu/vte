#!/bin/bash -x
# Copyright (C) 2011 Freescale Semiconductor, Inc. All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
#    @file   storage_mx53.sh
#
#    @brief  shell script template for "storage".
#
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#Hake                         2011/06/13        NA        nor r/w test
#-------------------------   ------------    ----------  -------------------------------------------
# 
#test method
#1. find the mtd partition with /proc/mtd
#2. slect the last one to test.
#3. run nor_testapp of vte test.
#4. $(flash_eraseall /dev/mtd? ; hexdump /dev/mtd? | grep ffff | wc -l) -eq 1
#

mtdnode=
mtdsize=
device=

setup()
{
    #TODO Total test case
    export TST_TOTAL=9

    export TCID="setup"
    export TST_COUNT=0
    RC=1
    trap "cleanup" 0
if [ `uname -r |cut -c -6` \> "3.0.35" ];then
   if [ $(cat /proc/mtd | grep spi | wc -l) -eq 1 ]; then			 
   			 mtdnode=/dev/$(cat /proc/mtd | grep spi | cut -c 1-4)
		  	 mtdsize=$(grep spi /proc/mtd |awk '{ print $2 }')
		  	 device=$(cat /proc/mtd | grep spi | cut -c 1-4);
	 elif [ $(cat /proc/mtd | grep kernel | wc -l) -eq 1 ]; then	
       	 mtdnode=/dev/`grep kernel /proc/mtd |awk '{ print $1 }' |cut -d : -f1`
         mtdsize=$(grep kernel /proc/mtd |awk '{ print $2 }')
		     device=$(cat /proc/mtd | grep kernel | cut -c 1-4);	 
	 fi
else
		if [ $(cat /proc/cmdline | grep spi-nor | wc -l) -eq 1 ]; then
		     mtdnode=/dev/`grep kernel /proc/mtd |awk '{ print $1 }' |cut -d : -f1`
         mtdsize=$(grep kernel /proc/mtd |awk '{ print $2 }')
		     device=$(cat /proc/mtd | grep kernel | cut -c 1-4);
		elif [ $(cat /proc/cmdline | grep weim-nor | wc -l) -eq 1 ]; then
         mtdnode=/dev/`grep rootfs /proc/mtd |awk '{ print $1 }' |cut -d : -f1`
         mtdsize=$(grep rootfs /proc/mtd |awk '{ print $2 }')
		     device=$(cat /proc/mtd | grep rootfs | cut -c 1-4);		
		fi
fi
   if [ ! -e $mtdnode ];then
        echo "TEST FAIL: No MTD device found, please check..."
        return 1
   fi
}

cleanup()
{
    RC=0

    #TODO add cleanup code here

    return $RC
}


test_case_01()
{
    TCID="test_RW_ERASE"
    RC=1
    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "
    nor_mtd_testapp -T RDRW -D $mtdnode -L 0x${mtdsize} -V || return $RC
    return 0
}

test_case_02()
{
    TCID="test_api_simple"
    RC=2
    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "
    flash_eraseall $mtdnode 
    ret=$(hexdump $mtdnode | grep ffff | wc -l)
    if [ $ret -eq 1 ]; then
        RC=0
    fi
    return $RC
}

test_case_03()
{
    TCID="test_RW_PERFORMANCE"
    RC=3
    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "
    echo "nor performance test"
    nor_mtd_testapp -T PERFORM -D $mtdnode -V || return $RC
    return 0
}

test_case_04()
{
    TCID="test_RW_several times"
    RC=4
    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "
    echo "nor RW_several times test"
    nor_mtd_testapp -T THRDRWE -D $mtdnode -V || return $RC
    return 0
}

test_case_05()
{
    TCID="NOR stress test"
    tst_resm TINFO "test $TST_COUNT: $TCID "
    RC=5
    flash_eraseall /dev/$device && insmod ${LTPROOT}/testcases/bin/mtd_stresstest.ko dev=$(echo $device | cut -c 4) count=5 && rmmod mtd_stresstest.ko || return $RC
    return 0
}

test_case_06()
{
    TCID="NOR jffs2 test"
    tst_resm TINFO "test $TST_COUNT: $TCID "
    RC=6
    mkdir -p /mnt/src; flash_eraseall /dev/$device && mount -t jffs2 /dev/mtdblock$(echo $device | cut -c 4) /mnt/src && bonnie++ -d /mnt/src -u 0:0 -s 10 -r 5 && umount /mnt/src || return $RC
    return 0
}

test_case_07()
{
    TCID="WEIM NOR ubi fs test"
    tst_resm TINFO "test $TST_COUNT: $TCID "
    RC=7
    flash_eraseall /dev/$device && ubiattach /dev/ubi_ctrl -m $(echo $device | cut -c 4) -d 0 && ubimkvol /dev/ubi0 -n 0 -N rootfs -s 2500000 && mkdir /mnt/ubifs; mount -t ubifs ubi0:rootfs /mnt/ubifs && umount /mnt/ubifs && ubidetach /dev/ubi_ctrl -d 0 || return $RC
    return 0
}

test_case_08()
{
    TCID="WEIM NOR speed test"
    tst_resm TINFO "test $TST_COUNT: $TCID "
    RC=8
    flash_eraseall /dev/$device && insmod ${LTPROOT}/testcases/bin/mtd_speedtest.ko dev=$(echo $device | cut -c 4) || return $RC
    return 0
}

test_case_09()
{
    TCID="WEIM NOR ubi fs stress test"
    tst_resm TINFO "test $TST_COUNT: $TCID "
    RC=9
    flash_eraseall /dev/$device && ubiattach /dev/ubi_ctrl -m $(echo $device | cut -c 4) -d 0 && ubimkvol /dev/ubi0 -n 0 -N rootfs -s 25000000 && mkdir /mnt/ubifs; mount -t ubifs ubi0:rootfs /mnt/ubifs && i=1; while [ $i -lt 5 ]; do bonnie\+\+ -d /mnt/ubifs -u 0:0 -s 10 -r 5; i=`expr $i + 1`; done && umount /mnt/ubifs && ubirmvol /dev/ubi0 -n 0 && ubidetach /dev/ubi_ctrl -d 0 || return $RC
    return 0
}

usage()
{
    echo "$0 [case ID]"
    echo "1: RW Erase test"
    echo "2: api simpale check"
    echo "3: performance test"
    echo "4: test_RW_several time"
    echo "5: NOR stress test"
    echo "6: NOR jffs2 test"
    echo "7: WEIM NOR ubi fs test"
    echo "8: WEIM NOR speed test"
    echo "9: WEIM NOR ubi fs stress test"
}

# main function

RC=0

#check parameter
if [ $# -ne 1 ]
then
    usage
    exit 1 
fi

setup || exit $RC

case "$1" in
1)
  test_case_01 || exit $RC 
  ;;
2)
  test_case_02 || exit $RC 
  ;;
3)
  test_case_03 || exit $RC
  ;;
4)
  test_case_04 || exit $RC
  ;;
5)
  test_case_05 || exit $RC
  ;;
6)
  test_case_06 || exit $RC
  ;;
7)
  test_case_07 || exit $RC
  ;;
8)
  test_case_08 || exit $RC
  ;;
9)
  test_case_09 || exit $RC
  ;;
*)
  usage
  ;;
esac

tst_resm TINFO "Test PASS"
