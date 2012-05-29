#!/bin/sh -x
#Copyright (C) 2012 Freescale Semiconductor, Inc.
#All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#!/bin/sh
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
##############################################################################
#
# Revision History:
#                      Modification
# Author                   Date       Description of Changes
#-------------------   ------------   ---------------------
# Sally Guo               29/05/2012     Initial ver.
#############################################################################
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
    if [ $(cat /proc/cmdline | grep hdmi | wc -l) -eq 1 ]; then
       echo "Already enable HDMI in boot cmdline"
       if [ $(cat /sys/devices/platform/mxc_hdmi/cable_state) = "plugout" ]; then
          echo "Not plug in HDMI cable in board"
          RC=1
       fi
    else
       echo "Not enable HDMI in boot cmdline"
       RC=1
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
}
usage()
{
   echo "1: display mode stress test"
   echo "2: audio mode stress test"
}
test_case_01()
{
#TODO give TCID 
TCID="Display mode stress test"
#TODO give TST_COUNT
TST_COUNT=1
RC=1
#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
i=0
loops=1000000
while [ $i -lt $loops ]
do
	i=`expr $i + 1`

        echo S:1920x1080p-60 > /sys/class/graphics/fb0/mode
        echo 0 > /sys/class/graphics/fb0/blank
        echo q| fbv $LTPROOT/testcases/bin/butterfly.png
        cat /sys/class/graphics/fb0/mode
        echo "times: $i"
        sleep 4

        echo S:720x480p-60 > /sys/class/graphics/fb0/mode
        echo 0 > /sys/class/graphics/fb0/blank
        echo q| fbv $LTPROOT/testcases/bin/butterfly.png
        cat /sys/class/graphics/fb0/mode
        echo "times: $i"
        sleep 4
done
RC=0
return $RC
}
# Function:     main
#
# Description:  - Execute all tests, exit with test status.
#
RC=0    # Return value for setup, and test functions.

setup || exit $RC
case "$1" in
1)
  test_case_01 || exit 2
  ;;
*)
  usage
  ;;
esac
tst_resm TINFO "Test Finish"
