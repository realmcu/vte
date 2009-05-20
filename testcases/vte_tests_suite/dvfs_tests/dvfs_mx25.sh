#!/bin/bash
###################################################################################################
#
#    @file   dvfs_mx25.sh
#
#    @brief  shell script template for testcase design "cpu freq test" is where to modify block.
#
###################################################################################################
#
#   Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
#   THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#   BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#   Freescale Semiconductor, Inc.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#<TODO_Author>/-----             <TODO_DATE>     N/A          Initial version
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
export TST_TOTAL=4

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


run_test_list()
{
   RC=1
   media_player=/mnt/nfs/util/mplayer
   media_name=/mnt/nfs/test_stream/goto_tibei.mp3
   nand_test="flash_eraseall /dev/mtd4"
   nand_stress_test="mount -t jffs2 /dev/mtdblock4 /mnt/src && bonnie++ -d /mnt/src -u 0:0 -s 10 -r 5 && umount /mnt/src"
   $media_player $media_name || return $RC
   $nand_test  || return $RC
   $nand_stress_test || return $RC

   RC=0
   return $RC
}


# Function:     test_case_01
# Description   - Test if <CPU freq> ok
#  
test_case_01()
{
#TODO give TCID 
TCID="test_CPUFreq_stress"
#TODO give TST_COUNT
TST_COUNT=1
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

declare -a cpufreq_value;
cpufreq_value=(133000 266000 399000);
count=0

while [ $count -lt 5 ]; do
  count=$(expr $count + 1)
  value=${cpufreq_value[$RANDOM%3]}
  cpufreq-set -f $value
  value_ret=$(cpufreq-info -f)
if [ "$value_ret" -eq "$value" ] ; then
   echo sleep...
   sleep 3
   #test list
   run_test_list
else
  return $RC
fi
done

RC=0
return $RC

}

# Function:     test_case_02
# Description   - Test if <TODO test function> ok
#  
test_case_02()
{
#TODO give TCID 
TCID="test_demo2_test"
#TODO give TST_COUNT
TST_COUNT=2
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

return $RC

}

# Function:     test_case_03
# Description   - Test if <TODO test function> ok
#  
test_case_03()
{
#TODO give TCID 
TCID="test_demo3_test"
#TODO give TST_COUNT
TST_COUNT=3
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

return $RC

}

# Function:     test_case_04
# Description   - Test if <TODO test function> ok
#  
test_case_04()
{
#TODO give TCID 
TCID="test_demo4_test"
#TODO give TST_COUNT
TST_COUNT=4
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

return $RC

}

# Function:     test_case_05
# Description   - Test if <TODO test function> ok
#  
test_case_05()
{
#TODO give TCID 
TCID="test_demo5_test"
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
echo "1: "
echo "2: "
echo "3: "
echo "4: "
echo "5: "
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







