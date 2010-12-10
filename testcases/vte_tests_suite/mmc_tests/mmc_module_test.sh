# Copyright (C) 2010 Freescale Semiconductor, Inc. All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

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
export TST_TOTAL=8

export TCID="setup"
export TST_COUNT=0
RC=0

trap "cleanup" 0

#TODO add setup scripts

insmod ${LTPROOT}/testcases/bin/mmc_test.ko

sleep 1
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

rmmod mmc_test
return $RC
}

usage()
{
echo "usage $0 <1/2/3/4>"
echo "1: MMC module read test all"
echo "2: MMC module write test all"
echo "3: MMC module HIGHMEM read test all"
echo "4: MMC module HIGHMEM write test all"
}


# Function:     test_case_01
# Description   - Test if MMC module read test ok
#  
test_case_01()
{
#TODO give TCID 
TCID="MMC_module_read_test"
#TODO give TST_COUNT
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
#the casesID are the struct list of mmc_test_case
caseID="2 4 6 8 10 12 14 16 18"

card=$(ls /sys/bus/mmc/drivers/mmcblk/ | grep mmc${MMCID})
if [ -z $card  ];then
RC=1
return $RC
fi


echo $card > /sys/bus/mmc/drivers/mmcblk/unbind
echo $card > /sys/bus/mmc/drivers/mmc_test/bind
test=$(find /sys -name test)
for i in $caseID
do
dmesg -c
echo $i > $test
sleep 1
ret=$(dmesg -c | grep  Result | awk '{print $3}')
[ "$ret" = "OK" ] || [ "ret" = "UNSUPPORTED" ] || RC=$(echo $RC $i)
done

echo $card > /sys/bus/mmc/drivers/mmc_test/unbind
echo $card > /sys/bus/mmc/drivers/mmcblk/bind
if [ "$RC" = "0" ]; then
	return 0
else
	RC=1
  return 1
fi
}

# Function:     test_case_02
# Description   - Test if MMC module write test ok
#  
test_case_02()
{
#TODO give TCID 
TCID="MMC_module_write_test"
#TODO give TST_COUNT
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
echo "this test will destroy the MMC card partition"
#the casesID are the struct list of mmc_test_case
caseID="1 3 5 7 9 11 13 15 17"
MMCP=$(cat /proc/partitions | grep mmc)

if [ -z "$MMCP" ]; then
echo "no mmc present"
return 1
fi

card=$(ls /sys/bus/mmc/drivers/mmcblk/ | grep mmc${MMCID})
echo $card > /sys/bus/mmc/drivers/mmcblk/unbind
echo $card > /sys/bus/mmc/drivers/mmc_test/bind
test=$(find /sys -name test)
for i in $caseID
do
dmesg -c
echo $i > $test
sleep 1
ret=$(dmesg -c | grep Result | awk '{print $3}')
[ $ret = "OK" ] || RC=$(echo $RC $i)
done
echo $card > /sys/bus/mmc/drivers/mmc_test/unbind
echo $card > /sys/bus/mmc/drivers/mmcblk/bind
if [ "$RC" = "0" ]; then
	return 0
else
	RC=1
  return 1
fi
}

# Function:     test_case_03
# Description   - Test if MMC module read test ok
#  
test_case_03()
{
#TODO give TCID 
TCID="MMC_module_hm_read_test"
#TODO give TST_COUNT
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
#the casesID are the struct list of mmc_test_case
caseID="20 22"
MMCP=$(cat /proc/partition | grep mmc${MMCID})

if [ -z "$MMCP" ]; then
echo "no mmc present"
return 1
fi

card=$(ls /sys/bus/mmc/drivers/mmcblk/ | grep mmc1)
echo $card > /sys/bus/mmc/drivers/mmcblk/unbind
echo $card > /sys/bus/mmc/drivers/mmc_test/bind
test=$(find /sys -name test)
for i in $caseID
do
dmesg -c
echo $i > $test
ret=$(dmesg -c | grep  Result | awk '{print $3}')
[ $ret = "OK" ] || RC=$(echo $RC $i)
done

echo $card > /sys/bus/mmc/drivers/mmc_test/unbind
echo $card > /sys/bus/mmc/drivers/mmcblk/bind
if [ "$RC" = "0" ]; then
	return 0
else
	RC=1
  return 1
fi

}

# Function:     test_case_04
# Description   - Test if MMC module HM write test ok
#  
test_case_04()
{
#TODO give TCID 
TCID="MMC_module_write_test"
#TODO give TST_COUNT
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
echo "this test will destroy the MMC card partition"
#the casesID are the struct list of mmc_test_case
caseID="19 21"
MMCP=$(cat /proc/partition | grep mmc)

if [ -z "$MMCP" ]; then
echo "no mmc present"
return 1
fi

card=$(ls /sys/bus/mmc/drivers/mmcblk/ | grep mmc${MMCID})
echo $card > /sys/bus/mmc/drivers/mmcblk/unbind
echo $card > /sys/bus/mmc/drivers/mmc_test/bind
test=$(find /sys -name test)
for i in $caseID
do
dmesg -c
echo $i > $test
ret=$(dmesg -c | grep  Result | awk '{print $3}')
[ $ret = "OK" ] || RC=$(echo $RC $i)
done
echo $card > /sys/bus/mmc/drivers/mmc_test/unbind
echo $card > /sys/bus/mmc/drivers/mmcblk/bind
if [ "$RC" = "0" ]; then
	return 0
else
	RC=1
  return 1
fi
}


setup  || exit $RC

MMCID=0
if [ "$2" ]; then
MMCID=$2
fi

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
*)
#TODO check parameter
  usage
  ;;
esac

tst_resm TINFO "Test PASS"
