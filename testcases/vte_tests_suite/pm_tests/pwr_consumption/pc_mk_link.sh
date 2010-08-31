#!/bin/sh
###############################################################################
# Copyright (C) 2010 Freescale Semiconductor, Inc. All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
###############################################################################

test_case_53()
{
    ln -sf /sys/devices/platform/mxc_dvfs_core.0/enable dvfscore
    ln -sf /sys/devices/platform/mxc_dvfsper.0/enable dvfsper
    ln -sf /sys/devices/platform/busfreq.0/enable busfreq
    ln -sf /proc/cpu/clocks clocks
}

platfm=$1
case "$platfm" in
    53)
    test_case_$platfm || exit $?
    ;;
    *)
    echo "please specify platform"
    ;;
esac
