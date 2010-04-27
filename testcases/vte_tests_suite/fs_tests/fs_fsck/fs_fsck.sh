#Copyright (C) 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
#!/bin/sh
##############################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#S.ZAVJALOV/-----             10/06/2004     TLSbo39741  Initial version 
#L.Delaspre/rc149c            08/12/2004     TLSbo40142   update with Freescale identity
#L.Delaspre/rc149c            23/03/2005     TLSbo47635   rework the header of test script
# Spring                      28/11/2008       n/a      Modify COPYRIGHT header
#
###################################################################################################

setup()
{
  export RC=0						# Return code from commands.
  export TST_TOTAL=1				# total numner of tests in this file.
  export TCID="TGE-LV-CRAMFS-0010"			# this is the init function.
  export TST_COUNT=0				# init identifier,

  # Initialize cleanup function.
  trap "cleanup" 0
  
  if ! [ -r $FS_DEVICE ]
  then
	tst_brkm TBROK NULL "SETUP: No such file $FS_DEVICE"
	return 1
  fi

  # Check if fsck command exists
  which fsck.$FS_TYPE &>/dev/null || RC=$?
  if [ $RC -ne 0 ]
  then
	tst_brkm TBROK NULL "SETUP: Command fsck.$FS_TYPE not found"
	return $RC
  fi

  return $RC
}

cleanup()
{
  RC=0
  
  return $RC
}


test01()
{
  TCID="test01"    # Identifier of this testcase.
  TST_COUNT=1      # Test case number.
  RC=0             # return code from commands.

  fsck.$FS_TYPE $FS_DEVICE &>/dev/null || RC=$?
  if [ $RC -ne 0 ]
  then
    tst_brkm TBROK NULL "TEST01: Error execute fsck.$FS_TYPE"
    return $RC
  fi
  
  return $RC
}


RC=0    # Return value from setup, and test functions.

#Usage.
if [ -z $2 ]
then
  echo "Usage: $0 fs_type device|file_name"
  exit 1
else
  export FS_TYPE=$1
  export FS_DEVICE=$2
fi

setup || RC=$?
if [ $RC -ne 0 ]
then
    exit $RC
fi

test01 || RC=$?

exit $RC
