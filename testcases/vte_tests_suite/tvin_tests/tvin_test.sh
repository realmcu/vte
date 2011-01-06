#!/bin/sh
#Copyright (C) 2008,2010 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
###################################################################################################
#
#    @file   tvin_test.sh
#
#    @brief  shell script to test the TVout user case function block.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#Hake.Huang/-----             08/01/2008     N/A          Initial version
#Spring Zhang/---             07/16/2010     ENGR124683   Add MX53 support
# 
###################################################################################################

MLIST="ipu_prp_enc.ko ipu_prp_vf_sdc.ko ipu_prp_vf_sdc_bg.ko ipu_still.ko ipu_csi_enc.ko adv7180_tvin.ko mxc_v4l2_capture.ko"
#OV3640 module must be removed before ADV7180 TVIN test.
RMLIST="adv7180_tvin ipu_prp_enc ipu_prp_vf_sdc_bg ov2640_camera ov3640_camera ipu_prp_vf_sdc ipu_still ipu_csi_enc mxc_v4l2_capture"


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

#rmmod for camera which is conflict
for i in $RMLIST
do
  modprobe -r  $i
  sleep 1
done

#insmod for v4l2
for i in $MLIST
do
  insmod  /lib/modules/$(uname -r)/kernel/drivers/media/video/mxc/capture/$i
done

sleep 2

if [ -e /dev/video0 ]
then
echo "device exist"
else
RC=1
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

killall mxc_v4l2_tvin

sleep 2

#rmmod for v4l2
for i in $RMLIST
do
  modprobe -r  $i
  sleep 1
done

return $RC
}

# Function:     test_tvin_01
# Description   - Test the TVin module functionality
#                 TVout basic function test
# TYPE:          auto manual test
test_tvin_01()
{
TCID="test_tvin_01"
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test #1: tvout_usercase 01"

RES_LIST="640x480 320x240 1024x768 800x600"
#topxleft
WIN_POS="0x0 16x16 32x32"
ROT="90 180 270"
MOTION="0 1 2"
#FORMAT="YU12 YUYV UYVY NV12"

for i in $RES_LIST
	do
		echo "resolution at $i"
   for j in $WIN_POS
		 do
      echo "window position at $j"
			for k in $ROT
				do
         echo "rotation at $k"
				 for m in $MOTION
					 do
            echo "motion at $m"
						for f in $FORMAT
							do
               echo "format at $f"
							 w=$(echo $i | cut -d "x" -f 1)
							 h=$(echo $i | cut -d "x" -f 2)
							 t=$(echo $j | cut -d "x" -f 1)
							 l=$(echo $j | cut -d "x" -f 2)
echo "top field first"
$TVIN_APP -ow $w -oh $h -ot $t -ol $l -r $k -c 60 -m $m -tb -f $f \
|| RC=$(expr $RC + 1)
echo "bottom field first"
$TVIN_APP -ow $w -oh $h -ot $t -ol $l -r $k -c 60 -m $m -f $f \
|| RC=$(expr $RC +1)
							done
					done
				done
		done
	done

return $RC 
}


# Function:     test_tvin_02
# Description   - Test the TVin module functionality
#                 TVout pal/ntsc test
# TYPE:          auto manual 
test_tvin_02()
{
TCID="test_tvin_02"
TST_COUNT=1
RC=0

echo "now please play PAL TV"

$TVIN_APP &

sleep 5

echo "plase switch the input to NTSC mode"

sleep 5

read -p "did you see the picture form tvin? y/n" RC

if [ "$RC" = "y" ]
then
RC=0
return $RC
fi
RC=1
return $TST_COUNT
}


# Function:     test_tvin_03
# Description   - Test 
#                 TVin power resume test
# TYPE:          auto manual  
test_tvin_03()
{
TCID="test_tvin_03"
TST_COUNT=1
RC=0

echo "enter power saving mode"

echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
echo "press resume key ..."
echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"

echo mem > /sys/power/state
 
$TVIN_APP &

read -p "did you see the picture form tvin? y/n" RC

if [ "$RC" = "y" ]
then
RC=0
return $RC
fi
RC=1
return $RC
}

RC=0

setup || exit $RC

if [ $# -ne 1 ]
then
echo "export FROMAT=UYVY"
echo "$0 <1/2/3>"
exit 1 
fi

TVIN_APP=/unit-tests/mxc_v4l2_tvin.out
if [ -z $FORMAT ];then
FORMAT=UYVY
fi

case "$1" in
1)
  test_tvin_01 || exit $RC 
  ;;
2)
  test_tvin_02 || exit $RC
  ;;
3)
  test_tvin_03 || exit $RC
  ;;
*)
  echo "usage $0 <1/2/3>"
  ;;
esac

tst_resm TINFO "test PASS"
