#Copyright 2010 Freescale Semiconductor, Inc. All Rights Reserved.
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
#    @file   gpu_test.sh
#
#    @brief  shell script template for testcase design "crypto" is where to modify block.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#Hake Huang/-----             2010     N/A          Initial version
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
export TST_TOTAL=4

export TCID="setup"
export TST_COUNT=0
RC=0

trap "cleanup" 0


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


# Function:     test_case_01
# Description   - Test crypto mode
#
test_case_01()
{
#TODO give TCID
TCID="CRYPTO MODE"
#TODO give TST_COUNT
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

MODE_LIST="0 1 2 3 4 5 6 7 8 9 10 \
           11 12 13 14 15 16 17 18 19 20 \
	   21 22 23 24 25 26 27 28 29 30 \
	   31 32 33 34 35 36 37 38 39 40 \
	   41 42 43 44 45                \
	   100 101 102 103 104 105"

echo "TST_INFO test"

for i in $MODE_LIST
do
insmod ${LTPROOT}/testcases/bin/tcrypt.ko mode=$i
Ret=$?
if [ $Ret -ne 0 ]; then
RC=$(expr $RC + 1)
fi
rmmod tcrypt
sleep 2
done

return $RC
}

# Function:     test_case_02
# Description   - Test speed
test_case_02()
{
#TODO give TCID
TCID="CRYPTO SPEED"
#TODO give TST_COUNT
TST_COUNT=2
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

MODE_LIST="200 201 202 203 204 205 206 300"

for i in $MODE_LIST
do
insmod ${LTPROOT}/testcases/bin/tcrypt.ko mode=$i sec=3
Ret=$?
if [ $Ret -ne 0 ]; then
RC=$(expr $RC + 1)
fi
rmmod tcrypt
sleep 2
done

return $RC
}

# Function:     test_case_03
# Description   - Test speed sepeartely
#
test_case_03()
{
#TODO give TCID
TCID="CRYPTO SPEED single"
#TODO give TST_COUNT
TST_COUNT=3
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
MODE_LIST="301 302 303 304 305 306 307 308 309 310 \
           311 312 313 314 315 316 317"

for i in $MODE_LIST
do
insmod ${LTPROOT}/testcases/bin/tcrypt.ko mode=$i sec=3
Ret=$?
if [ $Ret -ne 0 ]; then
RC=$(expr $RC + 1)
fi
rmmod tcrypt
sleep 2
done

return $RC
}

# Function:     test_case_04
# Description   - Test
#
test_case_04()
{
#TODO give TCID
TCID="_test"
#TODO give TST_COUNT
TST_COUNT=4
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
return $RC
}

# Function:     test_case_05
# Description   - Test
test_case_05()
{
#TODO give TCID
TCID="test"
#TODO give TST_COUNT
TST_COUNT=5
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

return $RC
}

usage()
{
echo "$0 [case ID]"
echo "1 mode test"
echo "2 speed test inchain"
echo "3 speed test single"
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
*)
  usage
  ;;
esac

tst_resm TINFO "Test PASS"
