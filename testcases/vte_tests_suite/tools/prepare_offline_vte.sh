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

usage()
{
    cat <<-EOF
    ./${0##*/} [SD rootfs root directory path] [Server IP] [vte folder name]
    e.g.: ./${0##*/} /mnt/msc
    armhf VTE: ./${0##*/} /mnt/msc 10.192.244.6 vte_mx63_hf
    softfp VTE: ./${0##*/} /mnt/msc 10.192.225.222 vte_mx63_d
    /mnt/msc is the mount point of SD rootfs root directory
EOF
    exit 1
}

if [ $# -lt 1 ]; then
    echo "please specify rootfs dir path"
    usage
fi

# Rootfs
rfs=$1/mnt/nfs
# server IP
if [ -n "$2" ]; then
    ip=$2
else
    ip=10.192.225.222
fi

if [ -n "$3" ]; then
    VTE=$3
else
    VTE=vte_mx63
fi

folder_list="vte/testcases/bin vte/results vte/output util/Graphics/imx61_rootfs/test/3DMarkMobile test_stream/video"
for f in $folder_list; do
    mkdir -p $rfs/$f
done

rsync -avz --progress --delete ${ip}::${VTE}/testcases/bin $rfs/vte/testcases
vte_list="lib pan runalltests.sh runltp ltpmenu runtest ver_linux"
vte_list="$vte_list manual_test"
for f in $vte_list; do
    rsync -avz --progress --delete ${ip}::${VTE}/$f $rfs/vte
done

gpu_list="Graphics/imx61_rootfs/test/3DMarkMobile Graphics/imx61_rootfs/test/fps_triangle Graphics/imx61_rootfs/test/simple_draw \
	Graphics/imx61_rootfs/test/simple_triangle Graphics/imx61_rootfs/test/torusknot"
#sync GPU materials
for f in $gpu_list; do
    rsync -avz --progress --delete ${ip}::util/$f $rfs/util/Graphics/imx61_rootfs/test
done

video="mpeg2_720x576.mpg SD720x480.vc1.rcv divx311_320x240.avi akiyo.mp4"
video="$video COASTGUARD_CIF_IJT.263 cif.263 HPCV_BRCM_A.264 starwars640x480.264"
video="$video stream.263 mpeg2_720x576.mpg H264_MP30_interlaced_poc2_720x576.h264"
video="$video WMV9_MPHL_NTSCV9.rcv blue_sky_mp8_2mbps_sh7_1920x1088.vp8"
video="$video 12_zju_0_0_6.0_foreman_cif.avs"
video="$video ToyStory3_H264HP_1920x1080_10Mbps_24fps_AAC_48kHz_192kbps_2ch_track1.h264 \
	Mpeg4_SP3_1920x1080_23.97fps_9760kbps_AACLC_44KHz_2ch_track1_track1.cmp"
video="$video H264_ML_1920x1080_10Mbps_15fps_noaudio.h264"

for v in $video; do
    rsync -avz --progress --delete ${ip}::test_stream/video/$v $rfs/test_stream/video
done


