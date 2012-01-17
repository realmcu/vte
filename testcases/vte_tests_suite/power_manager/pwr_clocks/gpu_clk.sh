#!/bin/sh -x
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
#find the dbugfs 
mountpt=$(mount | grep debugfs)
if [ -z $mountpt  ]; then
mount -t debugfs none /sys/kernel/debug
mount_pt=/sys/kernel/debug
else
mount_pt=$(echo $mountpt | awk '{print $3}')
fi

modprobe galcore
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
# Description   - Test if mixer operation for gpu ok
#
test_case_01()
{
#TODO give TCID
TCID="gpu clock test"
#TODO give TST_COUNT
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
#disable the framebuffer

gpu=0
#now check the clocks
cd /sys/kernel/debug/
gpu_list=$(find . -name gpu*)
for i in $gpu_list
do
temp=$(cat ${i}/enable_count)
gpu=$(expr $temp + $gpu)
done

if [ $gpu -gt 0 ]; then
RC=1
fi

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
echo "1: gpu clock test"
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

mount_pt=/sys/kernel/debug

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
