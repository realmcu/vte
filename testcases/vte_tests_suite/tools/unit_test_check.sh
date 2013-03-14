#!/bin/sh
##############################################################################
#Copyright 2013 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
##############################################################################
#
# Revision History:
# Author                   Date        Changes
#-------------------   ------------    ---------------------
# Spring Zhang/B17931   03/14/2013     Initial ver.
##############################################################################

unit_test_apps="mxc_asrc_test.out mxc_epdc_fb_test.out mxc_ipudev_test.out \
mxc_v4l2_capture.out mxc_v4l2_output.out mxc_v4l2_overlay.out mxc_v4l2_still.out \
mxc_v4l2_tvin.out mxc_vpu_test.out pxp_test.out wdt_driver_test.out"

for i in $unit_test_apps; do
    if [ ! -e /unit_tests/$i ]; then
        echo "/unit_tests/$i doesn't exist, please check"
        exit 1
    else
        echo "/unit_tests/$i exist"
    fi
done

exit 0
