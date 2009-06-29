#Copyright 2008-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
#    @file   uart_test.sh
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
export TST_TOTAL=1

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


# Function:     test_case_01
# Description   - Test if stty baud rate ok
#  
test_case_01()
{
#TODO give TCID 
TCID="test_baud_rate"
#TODO give TST_COUNT
TST_COUNT=1
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
BRATE="9600 19200 38400 57600 115200"

for i in $BRATE
do
stty -F $DEVICE $i || exit $RC
done

RC=0

return $RC

}


# main function

RC=0


#TODO check parameter
if [ $# -ne 2 ]
then
echo "usage $0 <serial device> <case ID>"
exit 1 
fi

DEVICE=$1

if [ ! -e $DEVICE ]
then
RC=1
echo device not exist!
exit $RC
fi


setup || exit $RC

case "$2" in
1)
  test_case_01 || exit $RC 
  ;;
*)
  usage
  ;;
esac

tst_resm TINFO "Test PASS"







