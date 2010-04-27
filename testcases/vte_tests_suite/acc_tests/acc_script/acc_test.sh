#Copyright (C) 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
#!/bin/sh
################################################################################
#
#    @file   acc_test.sh
#
#    @brief  this shell script is used to test the acc_module.
#
################################################################################
#Revision History: 
#                            Modification     ClearQuest 
#Author                          Date          Number    Description of Changes 
#-------------------------   ------------    ----------  -----------------------
#Gamma                        17/12/2007      N/A          Initial version 
#Ziye Yang/b21182             17/12/2008      N/A          N/A 
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
RC=0 
trap "cleanup" 0 
 
#TODO add setup scripts 
modprobe mxc_mma7450
sleep 1
rm -rf acc_result
MMA7450_CTL=/sys/class/i2c-adapter/i2c-0/0-001d/mma7450_ctl
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
rm -rf acc_temp
return $RC 
} 
 
 
# Function:     acc_test 
# Description   - test scenario 
#Record 8g Data:
#reg0x00: XL 10 bits output value X LSB X[7] X[6] X[5] X[4] X[3] X[2] X[1] X[0]
#reg0x01: XH 10 bits output value X MSB -- -- -- -- -- -- X[9] X[8]
#reg0x02: YL 10 bits output value Y LSB Y[7] Y[6] Y[5] Y[4] Y[3] Y[2] Y[1] Y[0]
#reg0x03: YH 10 bits output value Y MSB -- -- -- -- -- -- Y[9] YOUT[8]
#reg0x04: ZL 10 bits output value Z LSB Z[7] Z[6] Z[5] Z[4] Z[3] Z[2] Z[1] Z[0]
#reg0x05: ZH 10 bits output value Z MSB -- -- -- -- -- -- Z[9] Z[8]
#Record 4g/2g Data
#reg0x06: X8 8 bits output value X X[7] X[6] X[5] X[4] X[3] XT[2] X[1] X[0]
#reg0x07: Y8 8 bits output value Y Y[7] Y[6] Y[5] Y[4] Y[3] YT[2] Y[1] Y[0]
#reg0x08: Z8 8 bits output value Z Z[7] Z[6] Z[5] Z[4] Z[3] ZT[2] Z[1] Z[0]
################################################################################  
acc_test() 
{
RC=0 
#TODO add function test scripte here
if [ $MODE -eq 0 ]
then
echo "SET stangby mode"
elif [ $MODE -eq 1 ]
then
echo "SET measurement mode"
elif [ $MODE -eq 2 ]
then
echo "SET level detection mode"
elif [ $MODE -eq 3 ]
then
echo "SET pulse detection mode"
else
echo "mode parameter error!"
fi
 
echo setmod $MODE > $MMA7450_CTL
sleep 1


if [ $G_SELECT -eq 0 ]
then
echo "SET g-select 8g"
elif [ $G_SELECT -eq 1 ]
then
echo "SET g-select 4g"
elif [ $G_SELECT -eq 2 ]
then
echo "SET g-select 2g"
else
echo "g-select parameter error!"
fi
echo setg $G_SELECT > $MMA7450_CTL
echo "Please Keep moving the board during the test."
sleep 5
i=0
while [ $i -lt 30 ]
do
dmesg -c
cat $MMA7450_CTL
dmesg > acc_temp
awk '{if($3~/reg0x0[0-8]:/) printf("%s\t",$4);}; 
	END {printf("\n");}' acc_temp >> acc_result
echo " "
usleep 100000 #sleep 0.1s
i=`expr $i + 1`
done 
} 
usage()
{
 echo "First Parameter should be: 0, 1, 2, 3"
 echo "0: standby mode"
 echo "1: measurement mode"
 echo "2: level detection mode"
 echo "3: pulse detection"
 echo "Second Parameter should be 0, 1, 2"
 echo "0: 0-8g"
 echo "1: 2-4g"
 echo "2: 1-2g"
} 
 
# main function 
 
RC=0  
#TODO check parameter 
if [ $# -ne 2 ] 
then 
usage
exit 1  
fi
MODE=$1
G_SELECT=$2 
setup || exit $RC 
acc_test || exit $RC
 
