#Copyright (C) 2009-2011 Freescale Semiconductor, Inc. All Rights Reserved.
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

CTRL_INTERFACE=/dev/null
 case "$mode" in
   MODE_2G)
	 	cmd=0
		;;
	 MODE_4G)
	 cmd=1
	 ;;
	 *)
	 cmd=2
	 ;;
	esac
echo $cmd > $CTRL_INTERFACE
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
 RC=1
 #TODO add function test scripte here

 if [ -e $device ]; then 
  echo "test start"
  sh -c "evtest $device > /tmp/acctmp" &
  pth=$!
	echo "now shake the board!!"
	sleep 30
	cat /tmp/acctmp
	echo "test done"
  read -p "is data reasonable?y/n" ret
  if [ $ret = "y" ] || [ $ret ='Y' ];then
    RC=0
  fi
	kill -9 $pth
	rm -f /tmp/acctmp
 fi
 return $RC
} 
usage()
{
 echo "-d /dev/input/event1 -m <mode>"
 echo "mode: MODE_2G/MODE_4G/MODE_8G"
} 
 
# main function 
 
RC=0  
device=/dev/input/event1
mode=MODE_2G

while getopts d:m: OPTION
	do
		case $OPTION 
		in
		  m)
      mode=$OPTARG
			;;
			d)
			device=$OPTARG
			;;
			\?)
			echo "use dwfault option"
			echo "-d $device -m $mode"
			;;
    esac
done
setup || exit $RC 
acc_test || exit $RC
echo "TEST PASS"

