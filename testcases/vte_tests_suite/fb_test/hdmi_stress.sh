#!/bin/sh -x
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
    export TST_TOTAL=1
    export TCID="setup"
    export TST_COUNT=0
    trap "cleanup" 0
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
	platfm=$(platfm.sh)
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
echo 0 > /sys/class/graphics/fb0/blank
echo -e "\033[9;0]" > /dev/tty0 
#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
i=0
loops=10000000
while [ $i -lt $loops ]
do
	i=`expr $i + 1`

        echo S:1920x1080p-60 > /sys/class/graphics/fb0/mode
        echo q| fbv $LTPROOT/testcases/bin/butterfly.png
        cat /sys/class/graphics/fb0/mode
        echo "times: $i"
        sleep 4

        if [ $platfm != 60 ]; then
        echo S:640x480p-60 > /sys/class/graphics/fb0/mode
        echo q| fbv $LTPROOT/testcases/bin/butterfly.png
        cat /sys/class/graphics/fb0/mode
        echo "times: $i"
        sleep 4
		fi

        echo S:1280x720p-50 > /sys/class/graphics/fb0/mode
        echo q| fbv $LTPROOT/testcases/bin/butterfly.png
        cat /sys/class/graphics/fb0/mode
        echo "times: $i"
        sleep 4

        if [ $platfm != 60 ]; then
        echo S:720x480p-60 > /sys/class/graphics/fb0/mode
        echo q| fbv $LTPROOT/testcases/bin/butterfly.png
        cat /sys/class/graphics/fb0/mode
        echo "times: $i"
        sleep 4
		fi

        echo S:1920x1080p-24 > /sys/class/graphics/fb0/mode
        echo q| fbv $LTPROOT/testcases/bin/butterfly.png
        cat /sys/class/graphics/fb0/mode
        echo "times: $i"
        sleep 4

        if [ $platfm != 60 ]; then
        echo S:720x576p-50 > /sys/class/graphics/fb0/mode
        echo q| fbv $LTPROOT/testcases/bin/butterfly.png
        cat /sys/class/graphics/fb0/mode
        echo "times: $i"
        sleep 4
		fi

        echo S:1280x720p-60 > /sys/class/graphics/fb0/mode
        echo q| fbv $LTPROOT/testcases/bin/butterfly.png
        cat /sys/class/graphics/fb0/mode
        echo "times: $i"
        sleep 4
done
RC=0
return $RC
}

test_case_02()
{
#TODO give TCID 
TCID="Audio mode stress test"
#TODO give TST_COUNT
TST_COUNT=2
RC=1
echo 0 > /sys/class/graphics/fb0/blank
echo -e "\033[9;0]" > /dev/tty0
#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
mkdir /mnt/temp
mount -t tmpfs tmpfs /mnt/temp || exit 4
FILES="audio192k16S.wav audio176k16S.wav audio96k16S.wav audio88k16S.wav audio48k16S.wav audio44k16S.wav audio32k16S.wav audio11k16S.wav"
for fname in $FILES
do
cp /mnt/nfs/test_stream/alsa_stream/$fname /mnt/temp
done
num=`aplay -l |grep -i "imxhdmisoc" |awk '{ print $2 }'|sed 's/://'`

i=0
loops=10000000
while [ $i -lt $loops ]
do
        i=`expr $i + 1`

        echo S:1920x1080p-60 > /sys/class/graphics/fb0/mode
        aplay -Dplughw:$num -M /mnt/temp/audio11k16S.wav
        cat /sys/class/graphics/fb0/mode
        echo "times: $i"
        sleep 4

        if [ $platfm != 60 ]; then
        echo S:640x480p-60 > /sys/class/graphics/fb0/mode
        aplay -Dplughw:$num -M /mnt/temp/audio32k16S.wav
        cat /sys/class/graphics/fb0/mode
        echo "times: $i"
        sleep 4
		fi

        echo S:1280x720p-50 > /sys/class/graphics/fb0/mode
        aplay -Dplughw:$num -M /mnt/temp/audio44k16S.wav
        cat /sys/class/graphics/fb0/mode
        echo "times: $i"
        sleep 4

        if [ $platfm != 60 ]; then
        echo S:720x480p-60 > /sys/class/graphics/fb0/mode
        aplay -Dplughw:$num -M /mnt/temp/audio48k16S.wav
        cat /sys/class/graphics/fb0/mode
        echo "times: $i"
        sleep 4
		fi

        echo S:1920x1080p-24 > /sys/class/graphics/fb0/mode
        aplay -Dplughw:$num -M /mnt/temp/audio88k16S.wav
        cat /sys/class/graphics/fb0/mode
        echo "times: $i"
        sleep 4

        if [ $platfm != 60 ]; then
        echo S:720x576p-50 > /sys/class/graphics/fb0/mode
        aplay -Dplughw:$num -M /mnt/temp/audio176k16S.wav
        cat /sys/class/graphics/fb0/mode
        echo "times: $i"
        sleep 4
		fi

        echo S:1280x720p-60 > /sys/class/graphics/fb0/mode
        aplay -Dplughw:$num -M /mnt/temp/audio192k16S.wav
        cat /sys/class/graphics/fb0/mode
        echo "times: $i"
        sleep 4

done
umount /mnt/temp
RC=0
return $RC
}

# Function:     main
#
# Description:  - Execute all tests, exit with test status.
#
RC=0    # Return value for setup, and test functions.
platfm=63

setup || exit $RC
case "$1" in
1)
  test_case_01 || exit 2
  ;;
2)
  test_case_02 || exit 3
  ;;
*)
  usage
  ;;
esac
tst_resm TINFO "Test Finish"
