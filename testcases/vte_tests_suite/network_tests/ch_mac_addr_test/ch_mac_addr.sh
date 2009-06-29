#Copyright 2004-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
#!/bin/sh
##############################################################################
# 
#    @file   ch_mac_addr.sh
# 
#    @brief  change mac address test 
# 
##############################################################################
#Revision History: 
#                       Modification     Tracking 
#Author                     Date          Number    Description of Changes 
#-------------------   ------------    ----------  ---------------------------
#S.ZAVJALOV/-----        10-06-2004     TLSbo39738   Initial version 
# 
###############################################################################


setup()
{
  export RC=0						# Return code from commands.
  export TST_TOTAL=1				# total numner of tests in this file.
  export TCID="TGE-LV-CS-0050"			# this is the init function.
  export TST_COUNT=0				# init identifier,

  if [ `id -u` -ne 0 ]
  then
    tst_brkm TBROK NULL "SETUP: You must be a root"
    return 1
  fi
  
  if [ -z `echo $MAC_ADDR | sed -ne '/^[0-9|A-F|a-f][0-9|A-F|a-f]:[0-9|A-F|a-f][0-9|A-F|a-f]:[0-9|A-F|a-f][0-9|A-F|a-f]:[0-9|A-F|a-f][0-9|A-F|a-f]:[0-9|A-F|a-f][0-9|A-F|a-f]:[0-9|A-F|a-f][0-9|A-F|a-f]$/p'` ]
  then
	tst_brkm TBROK NULL "SETUP: This is $MAC_ADDR are not a mac address"
	return 1
  fi
  
  if ! grep $ETH /proc/net/dev 2>&1>/dev/null 
  then
	tst_brkm TBROK NULL "SETUP: No such device $ETH"
    return 1
  fi

  if [ -z $TMP ]
  then
	LTPTMP=/tmp/ch_mac_addr.$$/
  else
	LTPTMP=$TMP/ch_mac_addr.$$/
  fi

  # SETUPialize cleanup function.
  trap "cleanup" 0

  # create the temporary directory used by this testcase
  mkdir -p $LTPTMP/ 2>&1>/dev/null || RC=$?
  if [ $RC -ne 0 ]
  then
	tst_brkm TBROK "SETUP: Unable to create temporary directory"
	return $RC
  fi
    
  # Check if ip command exists
  which ip 2>&1>$LTPTMP/tst_template.out || RC=$?
  if [ $RC -ne 0 ]
  then
	tst_brkm TBROK NULL "SETUP: Command ip not found"
	return $RC
  fi

  return $RC
}

cleanup()
{
  RC=0
  
  if [ -z `ip l | grep $ETH | sed -ne 's/.*<.*,UP>.*/\1/p'` ]
  then
	ip link set $ETH up 2>&1> /dev/null
  fi
  
  if [ -d $LTPTMP ]
  then
    rm -rf $LTPTMP
  fi
  
  return $RC
}


test01()
{
  TCID="test01"    # Identifier of this testcase.
  TST_COUNT=1      # Test case number.
  RC=0             # return code from commands.
  
  ip link set down dev $ETH 2>&1> /dev/null || RC=$?
  if [ $RC -ne 0 ]
  then
    tst_brkm TBROK NULL "TEST01: Can't bring down device $ETH"
    return $RC
  fi

  ip link set dev $ETH lladdr $MAC_ADDR 2>&1> /dev/null || RC=$?
  if [ $RC -ne 0 ]
  then
    tst_brkm TBROK NULL "TEST01: Can't set up mac addr to device $ETH"
    return $RC
  fi

  ip link set up dev $ETH 2>&1> /dev/null || RC=$?
  if [ $RC -ne 0 ]
  then
    tst_brkm TBROK NULL "TEST01: Can't bring up device $ETH"
    return $RC
  fi
  
  return $RC
}


RC=0    # Return value from setup, and test functions.

#Usage.
if [ -z $1 ] || [ -z $2 ]
then
  echo "Usage: $0 device_name mac_addr"
  exit 1
else
  export ETH=$1
  export MAC_ADDR=$2
fi

setup || RC=$?
if [ $RC -ne 0 ]
then
    exit $RC
fi

test01 || RC=$?
if [ $RC -ne 0 ]
then
    exit $RC
fi

echo -ne "$0\tPASSED\n"
exit $RC
