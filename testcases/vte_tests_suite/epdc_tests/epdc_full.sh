#!/bin/sh
#Copyright (C) 2009-2010 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
#
###############################################################################
#
#    @file   epdc_full.sh
#
#    @brief  shell script template for testcase design "epdc" is where to modify block.
#
################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#------------------------   ------------    ----------  -----------------------
#Hake Huang/-----             20010512     N/A          Initial version
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
export TST_TOTAL=4

export TCID="setup"
export TST_COUNT=0
RC=0

trap "cleanup" 0

#TODO add setup scripts
which ls || RC=1

sync
sleep 1
echo 3 > /proc/sys/vm/drop_caches

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
sleep 1
echo 3 > /proc/sys/vm/drop_caches
return $RC
}


# Function:     test_case_01
# Description   - Test if mixer operation for epdc ok
#
test_case_01()
{
#TODO give TCID
TCID="EPDC_FULL_test"
#TODO give TST_COUNT
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
LOOP_TIMES=2
ROT_LIST="0 1 2 3"
CASE_LIST="2 3 4 5 6 7"
EPDC_FLAGS="1 2"
EDPC_SCHEME="0 1 2"
#alt buffer is not test here

while [ $LOOP_TIMES -gt 0 ]
do
	for i in $CASE_LIST
	do
		for j in $ROT_LIST
		do
			echo "epdc_test -T $i -R $j"
			for k in $EPDC_FLAGS
			do
			epdc_test -T $i -R $j -S 0 -s 0:0:128:128,257,0,0,$k,0,0,0:0:0:0 || RC=$(expr $RC + 1)
			sleep 1
			epdc_test -T $i -R $j -S 1 -s 0:0:128:128,257,0,0,$k,0,0,0:0:0:0 || RC=$(expr $RC + 1)
			sleep 1
			epdc_test -T $i -R $j -S 2 -s 0:0:128:128,257,0,0,$k,0,0,0:0:0:0 || RC=$(expr $RC + 1)
			sleep 1
			done
		done
	done
	LOOP_TIMES=$(expr $LOOP_TIMES - 1)
done
return $RC
}

# Function:     test_case_02
# Description   - Test if overnight ok
#
test_case_02()
{
#TODO give TCID
TCID="overnight_test"
#TODO give TST_COUNT
TST_COUNT=2
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

while [ 1 ]
do
epdc_test -T 0 -g 1
sleep 2
epdc_test -T 0 -g 2
sleep 2
epdc_test -T 2 -g 1
done


return $RC
}

# Function:     test_case_03
# Description   - Test if gles ok
#
test_case_03()
{
#TODO give TCID
TCID="none_test"
#TODO give TST_COUNT
TST_COUNT=3
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

return $RC
}

# Function:     test_case_04
# Description   - Test if stress gles ok
#
test_case_04()
{
#TODO give TCID
TCID="none_test"
#TODO give TST_COUNT
TST_COUNT=4
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
return $RC
}

usage()
{
echo "$0 [case ID]"
echo "1: EPDC full test"
echo "2: TBD test"
echo "3: TBD test"
echo "4: TBD test"
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
4)
  test_case_04 || exit $RC
  ;;
5)
  test_case_05 || exit $RC
  ;;
*)
  usage
  ;;
esac

tst_resm TINFO "Test PASS"
