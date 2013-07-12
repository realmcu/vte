#!/bin/bash -x
##############################################################################
#Copyright (C) 2012 Freescale Semiconductor, Inc.
#All Rights Reserved.
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
#                      Modification
# Author                   Date       Description of Changes
#-------------------   ------------   ---------------------
# Andy Tian            27/09/2012      Initial ver.
# Andy Tian            19/10/2012      Remove eth0 shutdown part.
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
    # Initialize return code to zero.
    RC=1                # Exit values of system commands used

    export TST_TOTAL=1   # Total number of test cases in this file.
    export TCID="WIFI_PERFORMANCE"       # Test case identifier
    export TST_COUNT=0   # Set up is initialized as test 0

    trap "cleanup" 0 2

	# Set global variables to the default value
	std_time=0		#default test start time is Beijing time 0:00AM
	end_time=8		#default test ending time is local 8:00AM
	counts=100		#default test 100 times
	#default name the log  file with MAC address
	mac=`ifconfig | grep -m 1 HWaddr | awk {'print $5'}`
	log_file=/var/log/${mac}_wifi.log

	# Update global variables accroding to given parameters
	while getopts s:c:e:f: arg
	do
		case $arg in
		f) log_file=/var/log/$OPTARG;;
		c) counts=$OPTARG;;
		s) std_time=$OPTARG;;
		e) end_time=$OPTARG;;
		\?|h) usage;;
		esac
	done

	#prepare the iperf tool and do time sync
	if [ ! -e "/usr/bin/iperf" ]; then
		cp `which iperf` /usr/bin || { RC=-1; echo "Setup Error: Pls set VTE env!"; }
	fi
	if [ ! -e "/usr/bin/ntpdate" ]; then
		cp `which ntpdate` /usr/bin || cp /mnt/nfs/util/ntpdate /usr/bin
	fi
	ntpdate 10.192.225.222
	[ $? -eq 0 ] || { RC=-1; echo "Setup Error: Can not sync time with 222 server"; }
	TZ='Asia/Shanghai'; export TZ

	# shut down eth0 to avoid impact to the performance data
	sync
	#nfs_list=`mount | awk '/:\// {print $3}'`
	#for dir in ${nfs_list}; do
	#	umount $dir
	#done
	#ifconfig eth0 down
	RC=0

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

	# Remove ath6kl_sdio and bring up eth0
	rmmod ath6kl_sdio
	count=100
	#ifconfig eth0 up
	#udhcpc -i eth0 || dhclient eth0

	if [ "$best_speed" -gt 0 ]; then
	    echo "The best result is:  $best_log" 
	fi
	exit $RC

   # return $RC
}

# Function:     wifi_perf()
#
# Description:  perform wifi performance test one time
#
# Exit:         zero on success
#               non-zero on failure.
#
wifi_perf()
{
	RC=1
	modprobe ath6kl_sdio
	sleep 5
	iwconfig wlan0 mode managed || RC=1
	iwconfig wlan0 key bbd9837522 || RC=1
	iwconfig wlan0 essid FSLLBGAP_001 || RC=1
	udhcpc -i wlan0 || dhclient wlan0
	route add -host 10.192.225.222 dev wlan0
	[ $? -eq 0 ] || { RC=1; echo "Error: Can not get IP addr for wlan0"; exit 1; }
	sleep 5
	LOCALIP=$(ifconfig wlan0  | grep 'inet addr:'| grep -v '127.0.0.1' | cut -d: -f2 | awk '{ print $1}'); 
	time -p iperf -c 10.192.225.222 -n 500M -t 100 -f m
	sleep 5
	rmmod ath6kl_sdio
	RC=0
}

# Function:     wifi_auto()
#
# Description:  wifi performance test many times and find out the best
#				result.
#
# Exit:         zero on success
#               non-zero on failure.
#
wifi_auto()
{
    RC=1
	cur_time=`date +%k`
	count=0
	best_speed=0
	
	# Save the original start and end time
	std_time_bk=$std_time
	end_time_bk=$end_time

	while [ true ]; do
		# Update std_time or end_time if std_time > end_time
		if [ $std_time_bk -gt $end_time_bk ]; then
			if [ $cur_time -gt $std_time_bk ]; then
				end_time=24
			fi
			if [ $cur_time -lt $end_time_bk ]; then
				std_time=0
			fi
		fi
		if [ $cur_time -ge $std_time -a $cur_time -le $end_time ];then
			# It is the time to do wifi performance test
			if [ $count -lt $counts ]; then
				wifi_perf >> $log_file
				if [ $? -eq 0 ]; then
				  	speed=`tail $log_file -n 15 | grep -m 1 "Mbits/sec" | awk {'print $7'} | cut -d. -f1`
				fi
				if [ $speed -gt $best_speed ]; then
					best_speed=$speed
					best_log=`tail $log_file -n 15 | grep -m 1 "Mbits/sec"`
				fi
				let count=count+1
			else
				# Max time reached, test finished
				break
			fi
		else
			sleep 1800
		fi
		cur_time=`date +%k`
	done
	[ $best_speed -gt 0 ] && { RC=0; echo "--------------"; echo "The best performance data is:"; \
		echo "$best_log" >> $log_file; echo "-------------"; }

	return $RC
}

# Function:     usage
#
# Description:  - display help info.
#
# Return        - none
usage()
{
    cat <<-EOF

    Use this tool to test wifi performance automatically as your plan.
	usage: ./${0##*/} [-f logname -s start_time -e end_time -c max_times]
            -f: Specify the log file name stored under /var/log. By default it is named with your FEC MAC addr.
            -s: Specify test start time by hour in 24 format. By default it is 0.
            -e: Specify test end time by hour in 24 format. By default it is 8.
            -c: specify the maximun test times. By default we do wifi performance 100 times. 
    e.g.: ./${0##*/}
    Run in default configuration
    e.g.: ./${0##*/} -f lucy_wifi.log
    Set the log file as /var/log/lucy_wifi.log

EOF
    exit 1
}


# Function:     main
#
# Description:  - Execute all tests, exit with test status.
#
# Exit:         - zero on success
#               - non-zero on failure.
#
RC=1    # Return value from setup, and test functions.

if [ $# -gt 8 ]; then
    usage
fi

#"" will pass the whole args to function setup()

setup "$@" || exit $RC

wifi_auto || exit $RC
