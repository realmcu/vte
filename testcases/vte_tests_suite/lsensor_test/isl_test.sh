#!/bin/bash
#Copyright (C) 2011 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
################################################################################
#
#    @file   isl_test.sh
#
#    @brief  this shell script is used to test the acc_module.
#
################################################################################
#Revision History: 
#                            Modification     ClearQuest 
#Author                          Date          Number    Description of Changes 
#-------------------------   ------------    ----------  -----------------------
#Gamma                        17/12/2007      N/A          Initial version 
#Hake Huang/b20222            01/10/2011      N/A          update for MMA845x
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
 
 
#
# Function:     test_case_01
# Description   - Test if isl basic work
#  
test_case_01()
{
#TODO give TCID 
TCID="isl_module_exist"
#TODO give TST_COUNT
TST_COUNT=1
RC=1
 
#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
 
#TODO add function test scripte here
for i in $(ls /sys/bus/i2c/devices/*/name)
do
 id=$(cat $i)
 if [ $id = "isl29023" ]; then
   isl_base=$(dirname $i)
   for j in $(ls ${isl_basl})
   do
    if [ ! -d $j ]; then
      cat $j || RC=$(echo $RC $j)
    fi
   done
   if [ "$RC" = "1" ];then
     RC=0
   fi
   break
 fi
done

return $RC
}

usage()
{
 echo "1 isl basic control reg access test"
}

setup || exit $RC
 
case "$1" in
1)
  test_case_01 || exit $RC 
  ;;
*)
  usage
  ;;
esac
 
tst_resm TINFO "Test PASS"

