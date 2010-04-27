#Copyright (C) 2004-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
#!/bin/bash
##############################################################################
# 
#    @file   file_get.sh
# 
#    @brief  Download files from ftp/http servers 
# 
##############################################################################
#Revision History: 
#                    Modification     Tracking 
#Author                  Date          Number    Description of Changes 
#-----------------   ------------    ----------  -----------------------------
#S.ZAVJALOV/-----     10-06-2004     TLSbo39738   Initial version 
# 
##############################################################################

setup()
{
  export RC=0                       # Return code from commands.
  export TST_TOTAL=1                # total numner of tests in this file.
  export TCID="TGE-LV-CS-0020"      # this is the init function.
  export TST_COUNT=0                # init identifier,



  if [ -z $TMP ]
  then
    LTPTMP=/tmp/tst_file_get.$$/
  else
    LTPTMP=$TMP/tst_file_get.$$/
  fi

  # Initialize cleanup function.
  trap "cleanup" 0

  # create the temporary directory used by this testcase
  mkdir -p $LTPTMP/ &>/dev/null || RC=$?
  if [ $RC -ne 0 ]
  then
    tst_brkm TBROK "SETUP: Unable to create temporary directory"
    return $RC
  fi
    
  # Check if wget command exists
  which wget &>$LTPTMP/tst_template.out || RC=$?
  if [ $RC -ne 0 ]
  then
    tst_brkm TBROK NULL "SETUP: Command wget not found"
    return $RC
  fi

  #Store curent dir
  export CUR_DIR=`pwd`

  cd $LTPTMP  

  return $RC
}

cleanup()
{
  RC=0
  
  cd $CUR_DIR
  
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

  wget -cq $FILE_NAME || RC=$?
  if [ $RC -ne 0 ]
  then
    tst_brkm TBROK NULL "TEST01: Can't download $FILE_NAME"
    return $RC
  fi
  
  return $RC
}


RC=0    # Return value from setup, and test functions.

#Usage.
if [ -z $1 ]
then
  echo "Usage: $0 [http|ftp://]host_name/.../file_name"
  exit 1
else
  export FILE_NAME=$1
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
