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

setup()
{
#TODO Total test case
export TST_TOTAL=1

export TCID="setup"
export TST_COUNT=0
RC=1
trap "cleanup" 0

mtdnode=/dev/$(cat /proc/mtd | tail -1| cut -d : -f 1)
if [ ! -e $mtdnode ];then
return 1
fi
mtdsize=$(cat /proc/mtd | tail -1|awk '{print $2}')
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
RC=2
echo "nor performance test"
nor_mtd_testapp -T PERFORM -D $mtdnode -V || return $RC
return 0
}

test_case_02()
{
TCID="test_api_simple"
RC=1
#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
flash_eraseall $mtdnode 
ret=$(hexdump $mtdnode | grep ffff | wc -l)
if [ $ret -eq 1 ]; then
RC=0
fi
return $RC
}

usage()
{
echo "$0 [case ID]"
echo "1: RW Erase and performace"
echo "2: api simpale check"
echo "3: power manager test"
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
*)
  usage
  ;;
esac

tst_resm TINFO "Test PASS"
