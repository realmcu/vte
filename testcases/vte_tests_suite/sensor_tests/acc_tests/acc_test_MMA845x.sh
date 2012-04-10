#!/bin/bash
#Copyright (C) 2009-2011 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
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
 
declare -a MMA_REGS;
# Function:     get_regid 
# 
# Description:  - get register id by name
# Return        - zero on success 
get_regid()
{
name=$1
ri=0
ID=-1
while [ $ri -lt $REG_CNT ]
do
 if [ $name = ${MMA_REGS[$ri]} ]; then
    ID=$ri
    break;
 fi
 ri=$(expr $ri + 1)
done
return $ID
}

# Function:     write_reg 
# 
# Description:  - wrtie to mma regist via i2c command
# Return        - zero on success 
write_reg()
{
 BUSID=$1
 DEVICEID=$2
 REG=$3
 value=$4
 get_regid $REG 
 if [ $ID -ne -1 ];then
 echo Y | i2cset -f $BUSID $DEVICEID $ID $value
  return $?
 else
  echo "invald register name"
  return -1
 fi
}

# Function:     read_reg 
# 
# Description:  - read mma regist via i2c command
# Return        - zero on success 
read_reg()
{
 BUSID=$1
 DEVICEID=$2
 REG=$3
 get_regid $REG
 if [ $ID -ne -1 ];then
 value=$(i2cget -f $BUSID $DEVICEID $ID)
 echo $value
 return $value
 else
  echo "invald register name"
 	return-1
 fi
}

 
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
   MODE_STANDBY)
	 	cmd=0
	 ;;
	 MODE_2G)
	 	cmd=1
		;;
	 MODE_4G)
	 cmd=2
	 ;;
	 *)
	 cmd=3
	 ;;
	esac
#echo $cmd > $CTRL_INTERFACE
write_reg 0 0x1c MMA8450_CTRL_REG1 $cmd
RC=$?

list=$(find /sys/class/input/ -name event*)
for i in $list
do
cat $i/device/name | grep mma
if [ $? -eq 0 ];then
device=/dev/input/$(basename $i)
break
fi
done

#enable it before using
enable_list=$(find /sys/devices/virtual/input -name enable)
for i in $enable_list
do
echo $i
echo 1 > $i
done

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
	acctmp=$(mktemp)
  evtest $device > $acctmp &
  pth=$!
	echo "now shake the board!! for 30seconds"
	if [ $mode = "SUSPEND"  ]; then
		rtc_testapp_6 -T 20
	fi
	sleep 30
	ret=$(cat $acctmp | wc -l)
	echo "test done $ret"
  if [ ! -z $ret ]; then
    RC=0
  fi
	kill -9 $pth
	wait
	rm -f $acctmp
 fi
 return $RC
} 
usage()
{
 echo "-d /dev/input/event1 -m <mode>"
 echo "mode: MODE_STANDBY/MODE_2G/MODE_4G/MODE_8G"
} 
 
# main function 
 
RC=0  
device=/dev/input/event1
mode=MODE_2G
REG_CNT=64
ID=-1
MMA_REGS=(
"MMA8450_STATUS1" \
"MMA8450_OUT_X8" \
"MMA8450_OUT_Y8" \
"MMA8450_OUT_Z8" \
"MMA8450_STATUS2" \
"MMA8450_OUT_X_LSB" \
"MMA8450_OUT_X_MSB" \
"MMA8450_OUT_Y_LSB" \
"MMA8450_OUT_Y_MSB" \
"MMA8450_OUT_Z_LSB" \
"MMA8450_OUT_Z_MSB" \
"MMA8450_STATUS3" \
"MMA8450_OUT_X_DELTA" \
"MMA8450_OUT_Y_DELTA" \
"MMA8450_OUT_Z_DELTA" \
"MMA8450_WHO_AM_I" \
"MMA8450_F_STATUS" \
"MMA8450_F_8DATA" \
"MMA8450_F_12DATA" \
"MMA8450_F_SETUP" \
"MMA8450_SYSMOD" \
"MMA8450_INT_SOURCE" \
"MMA8450_XYZ_DATA_CFG" \
"MMA8450_HP_FILTER_CUTOFF" \
"MMA8450_PL_STATUS" \
"MMA8450_PL_PRE_STATUS" \
"MMA8450_PL_CFG" \
"MMA8450_PL_COUNT" \
"MMA8450_PL_BF_ZCOMP" \
"MMA8450_PL_P_L_THS_REG1" \
"MMA8450_PL_P_L_THS_REG2" \
"MMA8450_PL_P_L_THS_REG3" \
"MMA8450_PL_L_P_THS_REG1" \
"MMA8450_PL_L_P_THS_REG2" \
"MMA8450_PL_L_P_THS_REG3" \
"MMA8450_FF_MT_CFG_1" \
"MMA8450_FF_MT_SRC_1" \
"MMA8450_FF_MT_THS_1" \
"MMA8450_FF_MT_COUNT_1" \
"MMA8450_FF_MT_CFG_2" \
"MMA8450_FF_MT_SRC_2" \
"MMA8450_FF_MT_THS_2" \
"MMA8450_FF_MT_COUNT_2" \
"MMA8450_TRANSIENT_CFG" \
"MMA8450_TRANSIENT_SRC" \
"MMA8450_TRANSIENT_THS" \
"MMA8450_TRANSIENT_COUNT" \
"MMA8450_PULSE_CFG" \
"MMA8450_PULSE_SRC" \
"MMA8450_PULSE_THSX" \
"MMA8450_PULSE_THSY" \
"MMA8450_PULSE_THSZ" \
"MMA8450_PULSE_TMLT" \
"MMA8450_PULSE_LTCY" \
"MMA8450_PULSE_WIND" \
"MMA8450_ASLP_COUNT" \
"MMA8450_CTRL_REG1" \
"MMA8450_CTRL_REG2" \
"MMA8450_CTRL_REG3" \
"MMA8450_CTRL_REG4" \
"MMA8450_CTRL_REG5" \
"MMA8450_OFF_X" \
"MMA8450_OFF_Y" \
"MMA8450_OFF_Z"
);

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
if [ ! $mode = "MODE_STANDBY" ];then
acc_test || exit $RC
fi
echo "TEST PASS"

