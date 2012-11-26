#!/bin/bash -x
#Copyright (C) 2012 Freescale Semiconductor, Inc.
#All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
##############################################################################
#
# Revision History:
#                      Modification
# Author                   Date       Description of Changes
#-------------------   ------------   ---------------------
#  Sally Guo                      31/10/2012     Initial ver.
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
    RC=0
    export TST_TOTAL=1   # Total number of test cases in this file.
    export TCID="TGE_LV_HDMI_LOADABLE_MODULE"       # Test case identifier
    export TST_COUNT=0   # Set up is initialized as test 0


    if [ ! -e /lib/modules/`uname -r`/kernel/sound/soc/codecs/snd-soc-mxc-hdmi.ko ] || [ ! -e /lib/modules/`uname -r`/kernel/sound/soc/imx/snd-soc-imx-hdmi.ko ]; then
       echo "loadable files don't exist"
       RC=1
       return $RC
    fi
    if [ $(cat /proc/cmdline | grep hdmi | wc -l) -eq 0 ]; then
       echo " not enable HDMI in boot cmdline"
       RC=2
       return $RC
    fi       
    if [ $(cat /sys/devices/platform/mxc_hdmi/cable_state) = "plugout" ]; then
       echo "Not plug in HDMI cable in board"
       RC=3
       return $RC
    fi    
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
    echo "please give only one parameter for loop times"
    exit 1
}
# Function:     
#
# Description   - hdmi loadable module stress test
#
# Return        - zero on success
#               - non zero on failure. return value from commands ($RC)
hdmi_loadable_stress()
{
loops=$1
i=0
echo 0 > /sys/class/graphics/fb0/blank
echo -e "\033[9;0]" > /dev/tty0
while [ $i -lt $loops ]
do
	i=`expr $i + 1`
  echo ==========================$i
  modprobe snd-soc-mxc-hdmi.ko && modprobe snd-soc-imx-hdmi.ko || RC=4
  lsmod
  num=`aplay -l |grep -i "imxhdmisoc" |awk '{ print $2 }'|sed 's/://'`
  aplay -Dplughw:$num ${STREAM_PATH}/alsa_stream_music/audio44k16S.wav || RC=5
  rmmod snd-soc-imx-hdmi.ko && rmmod snd-soc-mxc-hdmi.ko || RC=6
  if [ $RC -ne 0 ]; then
  	echo "error status $RC"
    break
  fi
done
return $RC
}
# Function:     main
#
# Description:  - Execute all tests, exit with test status.
#
# Exit:         - zero on success
#               - non-zero on failure.
#
if [ $# -ge 2 ] || [ $# -le 0 ]; then
    usage
fi

setup || exit $RC

hdmi_loadable_stress $1 || exit $RC
