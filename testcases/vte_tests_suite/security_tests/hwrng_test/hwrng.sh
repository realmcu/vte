#!/bin/sh

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

mount -t debugfs none /sys/kernel/debug

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
TCID="Test initial SEED"
#TODO give TST_COUNT
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

echo "if the caamrng is build as module"
echo "then it is possible to change the seed at inital time"

modprobe caamrng
sleep 5
echo "just in case there are other hwrgn as current"
echo "rng-caam" >  /sys/devices/virtual/misc/hw_random/rng_current

dd if=/dev/hwrng of=/dev/null bs=4k count=100 || RC=1

return $RC
}

# Function:     test_case_02
# Description   - Test speed
test_case_02()
{
#TODO give TCID
TCID="rnc with noblock mode"
#TODO give TST_COUNT
TST_COUNT=2
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

dd if=/dev/hwrng of=/dev/null bs=4k count=100 iflag=nonblock || RC=1

return $RC
}

# Function:     test_case_03
# Description   - Test speed sepeartely
#
test_case_03()
{
#TODO give TCID
TCID="security power clock gating when not in use"
#TODO give TST_COUNT
TST_COUNT=3
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

user=$(clocks.sh | grep caam_mem_clk | awk '{print $3}')

if $user -ne 0
	RC=1
fi

user=$(clocks.sh | grep caam_mem_clk | awk '{print $4}')

if $user -ne 0
	RC=$(expr $RC + 2)
fi

return $RC
}

# Function:     test_case_04
# Description   - Test
#
test_case_04()
{
#TODO give TCID
TCID="caam rng test with suspend and resume"
#TODO give TST_COUNT
TST_COUNT=4
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
count=5
while [ $count -gt 1  ]; do
	dd if=/dev/hwrng of=/dev/null bs=4k count=100 iflag=nonblock || RC=1
	rtc_testapp_6 -T 15
	count=$(expr $count - 1 )
done

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
echo "1 sed generate"
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
