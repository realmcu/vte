#!/bin/sh
###############################################################################
# Copyright (C) 2011 Freescale Semiconductor, Inc. All Rights Reserved.
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
#Revision History:
#                       Modification     Tracking
# Author                    Date          Number    Description of Changes
#--------------------   ------------    ----------  ----------------------
# Spring Zhang           Apr.13,2010       n/a      Initial version
# Spring Zhang           Apr.13,2010       n/a      Add voltage check


setup()
{
	#general setting
	VDDGP=/sys/class/regulator/regulator.10/microvolts
	VCC=/sys/class/regulator/regulator.11/microvolts
	CPU_CTRL=/sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed
	CUR_FREQ_GETTER=/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq
	DVFS_DIR=/sys/devices/platform/mxc_dvfs_core.0
	DVFSPER_DIR=/sys/devices/platform/mxc_dvfs_per.0
	BUSFREQ_DIR=/sys/devices/platform/busfreq.0
	CLK_CTRL=/proc/cpu/clocks
	#display setting
	fb0=/sys/class/graphics/fb0/blank
	fb1=/sys/class/graphics/fb1/blank
	#TVout
	fb2=/sys/class/graphics/fb2/blank

	#platform related
	if [ $platfm -eq 53 ]; then
		echo
	fi

    trap "cleanup" 0
}

cleanup()
{
    #TODO: restore old value
    RC=0
    return $RC
}

# @params:
# $1-platform code
sys_idle()
{
	enable_dvfs_$1
	echo 1 > $fb0
}

# @params:
# $1-platform code
user_idle()
{
	enable_dvfs_$1
	echo 0 > $fb0
}

enable_dvfs_53()
{
    echo 1 > $DVFSCORE_DIR/enable
    echo 1 > $BUSFREQ_DIR/enable 
}

enable_dvfs_51()
{
    echo 1 > $DVFSCORE_DIR/enable
    echo 1 > $DVFSPER_DIR/enable 
}

disable_dvfs_53()
{
    echo 0 > $DVFSCORE_DIR/enable
    echo 0 > $BUSFREQ_DIR/enable 
}

disable_dvfs_51()
{
    echo 0 > $DVFSCORE_DIR/enable
    echo 0 > $DVFSPER_DIR/enable 
}

# @params:
# $1-clocks check points file
# check points file format:
# clkname	usage	frequency
# ddr_clk-0	1		133MHz
check_clks()
{
	clk_matrix=`cat $1`
	cur_clk_matrix=`cat $CLK_CTRL`
	
	for line in $clk_matrix; do
		clk_name=`echo $line | awk '{ print $1 }'`
		clk_usage=`echo $line | awk '{ print $2 }'`
		clk_freq=`echo $line | awk '{ print $3 }'`
		if echo $cur_clk_matrix | grep $clk_name >/dev/null 2>&1; then
			cur_clk_usage=`echo $cur_clk_matrix | grep $clk_name | awk '{ print $2 }'`
			cur_clk_freq=`echo $cur_clk_matrix | grep $clk_name | awk '{ print $3 }'`
			if [ "$clk_usage" != "$cur_clk_usage" ]; then
				echo "WARNING $clk_name: usage not aligned, current usage is $cur_clk_usage"
			fi
			if ! echo $cur_clk_freq | grep $clk_freq >/dev/null 2>&1; then
				echo "FATAL ERROR $clk_name: frequency different, current freq is $cur_clk_freq"
			fi
		else
			echo "WARNING $clk_name: no such clock"
		fi		
	done
}

# Check voltage in different working points
# @params:
# $1-voltage check points file
# check points file format:
# wp		gp_voltage
# 400000	135000
check_voltage()
{
	#need to disable DVFS first
	vol_matrix=`cat $1`
	for line in $vol_matrix; do
		wp=`echo $line | awk '{ print $1 }'`
		vol=`echo $line | awk '{ print $2 }'`
		echo $wp > $CPU_CTRL
		cur_vol=`echo $VDDGP`
		if [ "$vol" != "$cur_vol" ]; then
			echo "FATAL ERROR WP-$wp: voltage different, current vol is $cur_vol"
		fi
	done
}

#main
platfm.sh || platfm=$?
if [ $platfm -eq 67 ]; then
	RC=$platfm
	echo "SoC Platform doesn't support"
	return $RC
fi

check_type=$1

#TODO: use getopts to handle params
case $check_type in
	"clock")
		if [ $# -lt 3 ]; then
			echo "please specify clock check point file"
			exit 3
		fi
		mode=$2
		clk_points_file=$3
		case $mode in
		"useridle")
		user_idle
		check_clks $clk_points_file || exit $?
		;;
		"sysidle")
		sys_idle
		check_clks $clk_points_file || exit $?
		;;
		*)
		echo "please specify modes: useridle or sysidle"
		;;
		esac
	;;
	"voltage")
		if [ $# -lt 2 ]; then
			echo "please specify voltage check point file"
			exit 2
		fi
		clk_points_file=$2
		#Disable DVFS first
		disable_dvfs_$platfm
		check_voltage $clk_points_file
	;;
	*)
		echo "please specify check type: clock or voltage"
	;;
esac
