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
    echo 0 > /sys/devices/platform/mxc_dvfs_core.0/enable
    #echo 1 > /sys/devices/platform/mxc_dvfsper.0/enable
    echo 0 > /sys/devices/platform/busfreq.0/enable 
}

usage()
{
    cat <<-EOF
    usage: ./${0##*/} on_or_off type [platfm]
        on_or_off: on, off
        type: lcd, eth, dvfs
        platfm: 53 51
        e.g. ./${0##*/} on dvfs 53
EOF
}

#TODO: add determination for on_or_off
#      merge on code

type=$1
case "$type" in
    "lcd")
    echo 1 > /sys/class/graphics/fb0/blank
    #no need to close fb1
    #echo 1 > /sys/class/graphics/fb1/blank
    #delete it for fb2 is TVout
    #echo 1 > /sys/class/graphics/fb2/blank
    ;;
    "eth")
    for iface in `ifconfig -a | grep "^eth" | awk '{print $1}'`; do
        ifconfig $iface down
    done
    ;;
    "dvfs")
    platfm=$2
    case "$platfm" in
        53)
        test_case_$platfm || exit $?
        ;;
        *)
        echo "please specify platform"
        ;;
    esac
    ;;
    *)
    usage
    exit 1
esac

