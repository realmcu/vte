#Copyright (C) 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
###################################################################################################
#
#    @file   mlb_test.sh
#
#    @brief  shell script to test the mlb 3rd party function block.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#Hake.Huang/-----             07/28/2008     N/A          Initial version
# 
###################################################################################################
#!/bin/sh


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
export TST_TOTAL=1

export TCID="setup"
export TST_COUNT=0
RC=0

if [ -e $LTPROOT ]
then
export LTPSET=0
else
export LTPSET=1
fi

trap "cleanup" 0

modprobe mxc_mlb

if [ $? -ne 0 ]
then
tst_resm TINFO "mlb init failed!"
RC=2
return $RC
fi

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
modprobe -r mxc_mlb
return $RC
}


# main function

RC=0

setup || exit $RC

echo case 1
echo Test control channel with 256fps frame rate.	
echo mlb-test -f 256 -t ctrl -b
echo -------------------------------------------------
echo case 2
echo Test control channel with 512fps frame rate.	
echo mlb-test -f 512 -t ctrl -b
echo -------------------------------------------------
echo case 3
echo Test control channel with 1024fps frame rate.	
echo mlb-test -f 1024 -t ctrl -b
echo not supported in imx31 due to hardware 
echo -------------------------------------------------
echo case 4
echo Test control channel non block I/O with 256fps frame rate.	
echo mlb-test -f 256 -t ctrl
echo -------------------------------------------------
echo case 5
echo Test control channel non block I/O with 512fps frame rate.	
echo mlb-test -f 512 -t ctrl
echo -------------------------------------------------
echo case 6
echo not supported in imx31 due to hardware  
echo Test control channel non block I/O with 1024fps frame rate.	
echo mlb-test -f 1024 -t ctrl
echo not supported in imx31 due to hardware 
echo -------------------------------------------------
echo case 7
echo Test asynchronous channel with 256fps frame rate.	
echo mlb-test -f 256 -t async -b
echo -------------------------------------------------
echo case 8
echo Test asynchronous channel with 512fps frame rate.	
echo mlb-test -f 512 -t async -b
echo -------------------------------------------------
echo case 9
echo Test asynchronous channel with 1024fps frame rate.	
echo mlb-test -f 1024 -t async -b
echo not supported in imx31 due to hardware 
echo -------------------------------------------------
echo case 10
echo Test asynchronous channel non block I/O with 256fps frame rate.	
echo mlb-test -f 256 -t async
echo -------------------------------------------------
echo case 11
echo Test asynchronous channel non block I/O with 512fps frame rate.	
echo mlb-test -f 512 -t async
echo -------------------------------------------------
echo case 12
echo Test asynchronous channel non block I/O with 1024fps frame rate.	
echo mlb-test -f 1024 -t async
echo not supported in imx31 due to hardware  


read -p "select  case to test <1-12>" tid

case "$tid" in
1)
   mxc_mlb_test -f 256 -t ctrl -b || exit $RC 
  ;;
2)
  mxc_mlb_test -f 512 -t ctrl -b || exit $RC
  ;;
3)
  mxc_mlb_test -f 1024 -t ctrl -b || exit $RC
  ;;
4)
  mxc_mlb_test -f 256 -t ctrl || exit $RC
  ;;
5)
  mxc_mlb_test -f 512 -t ctrl  || exit $RC
  ;;
6)
  mxc_mlb_test -f 1024 -t ctrl || exit $RC
  ;;
7)
  mxc_mlb_test -f 256 -t async -b || exit $RC
  ;;
8)
  mxc_mlb_test -f 512 -t async -b || exit $RC
  ;;
9)
  mxc_mlb_test -f 1024 -t async -b || exit $RC
  ;;
10)
  mxc_mlb_test -f 256 -t async || exit $RC
  ;;
11)
  mxc_mlb_test -f 512 -t async || exit $RC
  ;;
12)
   mxc_mlb_test -f 1024 -t async || exit $RC
  ;;
*)
  echo "wrong number quit"
  RC=13 
  exit $RC
  ;;
esac

tst_resm TINFO "test PASS"

