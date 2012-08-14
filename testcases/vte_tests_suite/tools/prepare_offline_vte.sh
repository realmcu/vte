#!/bin/sh
#############################################################################
#
#  Copyright 2012 Freescale Semiconductor, Inc. All Rights Reserved.
#
##############################################################################
#
#  The code contained herein is licensed under the GNU Lesser General Public
#  License.  You may obtain a copy of the GNU Lesser General Public License
#  Version 2.1 or later at the following locations:
#
#  http://www.opensource.org/licenses/lgpl-license.html
#  http://www.gnu.org/copyleft/lgpl.html
#
##############################################################################
# Author: Spring Zhang
# Version1: 08.14.2012

if [ $# -lt 1 ]; then
    echo "please specify rootfs dir path"
    exit 1
fi

# Rootfs
rfs=$1/mnt/nfs
# server IP
if [ -n "$2" ]; then
    ip=$2
else
    ip=10.192.225.222
fi

folder_list="vte/testcases/bin vte/results vte/output util test_stream/video"
for f in $folder_list; do
    mkdir -p $rfs/$f
done

rsync -avz --progress --delete ${ip}::vte_mx61_d/testcases/bin $rfs/vte/testcases
vte_list="lib pan runalltests.sh runltp ltpmenu runtest ver_linux"
vte_list="$vte_list manual_test"
for f in $vte_list; do
    rsync -avz --progress --delete ${ip}::vte_mx61_d/$f $rfs/vte
done

video="mpeg2_720x576.mpg SD720x480.vc1.rcv divx311_320x240.avi akiyo.mp4"
video="$video COASTGUARD_CIF_IJT.263 cif.263 HPCV_BRCM_A.264 starwars640x480.264"
video="$video stream.263 mpeg2_720x576.mpg H264_MP30_interlaced_poc2_720x576.h264"
video="$video WMV9_MPHL_NTSCV9.rcv blue_sky_mp8_2mbps_sh7_1920x1088.vp8"
video="$video 12_zju_0_0_6.0_foreman_cif.avs"

for v in $video; do
    rsync -avz --progress --delete ${ip}::test_stream/video/$v $rfs/test_stream/video
done

echo "Finished"
