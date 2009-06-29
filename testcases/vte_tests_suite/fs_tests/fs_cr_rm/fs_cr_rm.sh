#Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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


#revolver=('|' '/' '-' '\')

count=0

stamp()
{
#    echo -ne "\b${revolver[$((++rev_i%${#revolver[*]}))]}" >&0

    case $count in
        0) echo -ne "\b-"
           count=1;;
        1) echo -ne "\b\\"
           count=2;;
        2) echo -ne "\b|"
           count=3;;
        3) echo -ne "\b/"
           count=0;;
    esac
}


setup()
{
  export RC=0						# Return code from commands.
  export TST_TOTAL=1				# total numner of tests in this file.
  export TCID="TGE-LV-RAMFS-0030"			# this is the SETUP function.
  export TST_COUNT=0				# SETUP identifier,

  if [ -z $TMP_PATH ]
  then
    LTPTMP=./cr_rm_files.$$/
  else
    LTPTMP=$TMP_PATH/cr_rm_files.$$/
  fi

  trap "cleanup" 0

  # create the temporary directory used by this testcase
  mkdir -p $LTPTMP/ &>/dev/null || RC=$?
  if [ $RC -ne 0 ]
  then
	tst_brkm TBROK NULL "SETUP: Unable to create temporary directory"
	return $RC
  fi
    

  cd $LTPTMP  

  return $RC
}

cleanup()
{
  RC=0
  
  cd ..
  
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

  for count in `seq 1 3`;
  do
	echo -ne ' '
    mkdir a
    for i in `seq 1 1000`;
    do
      echo  0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz > a/$i || RC=$?
	    if [ $RC -ne 0 ]
	    then
	      tst_brkm TBROK NULL "TEST01: Can't create file a/$i"
		  return $RC
	    fi
	  stamp
    done

	mkdir b
    for j in `seq 1 1000`;
	do
  	  ln -s `pwd`/a/$j b/$j || RC=$?
      if [ $RC -ne 0 ]
      then
    	tst_brkm TBROK NULL "TEST01: Can't create symbol link b/$j"
        return $RC
      fi
      stamp
	done

    rm -fr b
    rm -fr a
  done

  return $RC
}


RC=0    # Return value from setup, and test functions.

if [ -z $1 ]
then
  export TMP_PATH=$1
fi

setup || RC=$?
if [ $RC -ne 0 ]
then
    exit $RC
fi

test01 || RC=$?

exit $RC
