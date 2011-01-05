#Copyright (C) 2008-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
#!/bin/sh
###################################################################################################
#
#    @file   rtc_test.sh
#
#    @brief  shell script template for testcase design "TODO" is where to modify block.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#<Hake Huang>/-----             <2008-11-27>     N/A          Initial version
# 
###################################################################################################



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
export TST_TOTAL=2

export TCID="setup"
export TST_COUNT=0
RC=0

trap "cleanup" 0

#TODO add setup scripts

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

return $RC
}

usage()
{
 echo "rtc_test.sh <test case ID>"
 echo "Test case ID <1/2/3>"
 echo "1: module existence test"
 echo "2: set rtc hardware and check"
 echo "3: rtc accuracy test"
}

# Function:     test_case_01
# Description   - Test if rtc module exist
#  
test_case_01()
{
#TODO give TCID 
TCID="rtc_module_exist"
#TODO give TST_COUNT
TST_COUNT=1
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
if [ -e /sys/class/rtc/rtc0 ]
then 
RC=0
fi

return $RC

}


# Function:     test_case_02
# Description   - Test if stty hw rtc is ok
#  
test_case_02()
{
#TODO give TCID 
TCID="rtc_hw"
#TODO give TST_COUNT
TST_COUNT=1
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

date -u 200803211658
date "2010-07-14 10:06:00"
date -u 200808111730
hwclock --systohc
#only check the year suppose enough
ret=$(hwclock | grep 2010 | wc -l)

if [ $ret ]
then
RC=0
fi

return $RC

}

# Function:     test_case_03
# Description   - Test if rtc accuracy is ok
#  
test_case_03()
{
#TODO give TCID 
TCID="rtc_ac"
#TODO give TST_COUNT
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
#test 1hr and check rtc accuracy

hwclock -w
sleep 300
diffs=$(hwclock -r ;date)
echo $diffs
wd1=$(echo $diffs | awk '{print $1}')
wd2=$(echo $diffs | awk '{print $8}')
if [ $wd1 != $wd2 ];then
  RC=1
fi

m1=$(echo $diffs | awk '{print $2}')
m2=$(echo $diffs | awk '{print $9}')
if [ $m1 != $m2 ];then
  RC="$RC 2"
fi

d1=$(echo $diffs | awk '{print $3}')
d2=$(echo $diffs | awk '{print $10}')
if [ $d1 -ne $d2 ];then
  RC="$RC 3"
fi

h1=$(echo $diffs | awk '{print $4}' | cut -d: -f 1)
h2=$(echo $diffs | awk '{print $11}' | cut -d: -f 1)
if [ $h1 -ne $h2 ];then
  RC="$RC 4"
fi

mm1=$(echo $diffs | awk '{print $4}' | cut -d: -f 2)
mm2=$(echo $diffs | awk '{print $11}' | cut -d: -f 2)
if [ $mm1 -ne $mm2 ];then
  RC="$RC 5"
fi

ss1=$(echo $diffs | awk '{print $4}' | cut -d: -f 3)
ss2=$(echo $diffs | awk '{print $11}' | cut -d: -f 3)
if [ $ss2 -lt $ss1 ];then
offset=$(echo $ss2 $ss1 - p | dc)
else
offset=$(echo $ss1 $ss2 - p | dc)
fi
if [ $offset -gt 3 ];then
RC="$RC 6"
fi

y1=$(echo $diffs | awk '{print $5}')
y2=$(echo $diffs | awk '{print $13}')
if [ $y1 -ne $y2 ];then
  RC="$RC 6"
fi

return $RC
}



# main function

RC=0


#TODO check parameter
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
*)
  usage
  ;;
esac

tst_resm TINFO "Test PASS"







