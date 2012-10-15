#!/bin/bash -x
##############################################################################
#Copyright (C) 2011,2012 Freescale Semiconductor, Inc.
#All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
##############################################################################

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
    #TODO Total test case
    export TST_TOTAL=6

    export TCID="SETUP"
    export TST_COUNT=0
    RC=0

    trap "cleanup" 0
    i=0
    count=$(cpufreq-info -s | wc -w)
    while [ $i -lt $count ]
    do
        # cut: fields and positions are numbered from 1
        field=$(expr $i + 1)
        freq=$(cpufreq-info -s | cut -d " " -f $field | cut -d ":" -f 1)
        if [ ! -z $freq ]; then
            cpufreq_value[$i]=$freq
        fi
        i=$(expr $i + 1)
    done

    TOTAL_PT=${#cpufreq_value[@]}
    echo -n "CPUFREQ is:"
    for (( i = 0; i < TOTAL_PT; i++ )); do
        echo -n " ${cpufreq_value[$i]}"
    done
    echo

    #check for right uart port to test
    cn=$(cat /proc/cmdline | wc -w)
    i=1
    while [ $i -le $cn ]
    do
        pw=$(cat /proc/cmdline | cut -d " " -f $i)
        var=$(echo $pw | cut -d "=" -f 2 | cut -d "," -f 1)
        match=$(echo $var | grep ttymxc | wc -l)
        if [ $match -eq 1 ]; then
            UART=/dev/${var}
        fi
        i=$(expr $i + 1)
    done

   echo 8 > /proc/sys/kernel/printk
   echo enabled > /sys/class/tty/$(basename ${UART})/power/wakeup
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

    #TODO add cleanup code here


    return $RC
}


run_manual_test_list()
{
    RC=1
    errcnt=0
    echo "please insert usb hard disk with partion 1 is vfat"
    echo "please connect cpu board jp17 1,2,3 pin"
    read -p "press any key when ready" key
    modprobe ehci-hcd
    sleep 20
    mkfs.vfat /dev/sda1 || return $RC
    mkdir -p /media/sda1; mount -t vfat /dev/sda1 /media/sda1 || return $RC
    bonnie\+\+ -d /media/sda1 -u 0:0 -s 10 -r 5 || return $RC
    limit=`get_dt_limit /media/sda1 128000` || limit=128000
    dt of=/media/sda1/test_file bs=4k limit=${limit}k passes=20 || return $RC
    modprobe -r ehci-hcd
    echo "please un plug the usb card"
    read -p "press any key when ready" key
    sleep 3

    echo "  "
    echo "  "
    echo "usb otg storage test"
    echo "please connect make sure the jp17 usb pin is not power"
    echo "please connect usb cable to pc host"
    read -p "press any key when ready" key
    echo "on PC host please run:"
    echo " modprobe usb-storage"
    echo "mkdir /mnt/flash"
    read -p "press any key when ready" key
    dd if=/dev/zero of=/var/storage.img bs=1M count=64
    mkfs.ext3 /var/storage.img
    modprobe g_file_storage file=/var/storage.img
    echo "now please mount the usb device on PC"
    echo "please run bwloe on pc"
    echo "mount -t ext3 /dev/sd? /mnt/flash"
    echo "bonnie\+\+ -d /mnt/flash -u 0:0 -s 10 -r 5"
    limit=`get_dt_limit /mnt/flash 128000` || limit=128000
    echo "dt of=/mnt/flash/test_file bs=4k limit=${limit}k passes=20"

    read -p "is pc run above case ok? y/n" key
    if [ "$key" = 'n' ]; then
        errcnt=$(expr $errcnt + 1)
    fi
    modprobe -r g_file_storage


    read -p "is above case ok? y/n" key
    if [ "$key" = 'n' ]; then
        errcnt=$(expr $errcnt + 1)
    fi

    echo "SD test"
    echo "please insert SD card"
    read -p "press any key when ready" key
    mkfs.vfat /dev/mmcblk0p1
    mkdir -p /mnt/mmc
    mount -t vfat /dev/mmcblk0p1 /mnt/mmc
    bonnie\+\+ -d /mnt/mmc -u 0:0 -s 10 -r 5
    limit=`get_dt_limit /mnt/mmc 128000` || limit=128000
    dt of=/mnt/mmc/test_file bs=4k limit=${limit}k passes=20

    RC=0
    return $RC
}

run_auto_test_list()
{
    RC=0
    echo "uart test"
    cat /etc/passwd > ${UART} || RC=$(expr $RC + 1)
    echo "core test"
    coremark_F4.exe  0x0 0x0 0x66 0 7 1 2000 &&  coremark_F4.exe  0x3415 0x3415 0x66 0 7 1 2000 \
        && coremark_F4.exe 8 8 8 0 7 1 1200 || RC=$(expr $RC + 7)
    echo "storage"
    modprobe ahci_platform
    storage_all.sh 4 || RC=$(expr $RC + 9)
    modprobe -r ahci_platform
    echo "ALSA test"
    aplay -vv $STREAM_PATH/alsa_stream/audio44k16M.wav || RC=$(expr $RC + 23)
	echo "timer interrupt"
	timer_interrupt
	echo "EPDC test"
	epdc_test -T 7
    return $RC
}

# Set cpufreq governor to userspace
set_governor_userspace()
{
    # Set cpufreq governor to userspace
    cpufreq-set -g userspace || {
        RC=$?
        echo "CPUFreq governor can't set to userspace"
        exit $RC
    }
}


# Function:     test_case_01
# Description   - Test if <CPU freq> ok
#
test_case_01()
{
    #TODO give TCID
    TCID="CPUFreq_STRESS"
    #TODO give TST_COUNT
    TST_COUNT=1
    RC=1

    #print test info
    echo "TINFO test $TST_COUNT: $TCID "

    count=0
    set_governor_userspace

    while [ $count -lt 7 ]; do
        count=$(expr $count + 1)
        value=${cpufreq_value[$RANDOM%${TOTAL_PT}]}
        echo $value
        cpufreq-set -f ${value}
        if [ $(cpufreq-info -f) != $(cpufreq-info -w) ]; then
            echo "CPU Freq reading from cpufreq core and HW is different"
            RC=2
            return $RC
        fi

        value_ret=$(cpufreq-info -f | grep $value | wc -l)
        if [ $value_ret -eq 1 ] ; then
            echo sleep...
            sleep 3
            echo "TSTINFO: CPUFREQ at $value "
            #test list
            run_auto_test_list || RC=$(expr $RC + 1)
        else
            return $RC
        fi
    done

    cpufreq-set -f ${cpufreq_value[0]}
    return $RC
}

# Function:     test_case_02
# Description   - Test if PMIC DVFS test ok
#
test_case_02()
{
    #TODO give TCID
    TCID="PMIC_DVFS_core_test"
    #TODO give TST_COUNT
    TST_COUNT=2
    RC=0

    #print test info
    echo TINFO "test $TST_COUNT: $TCID "

    #TODO add function test script here
    echo 1 > /sys/devices/platform/imx_dvfscore.0/enable

    sleep 5
    run_auto_test_list || RC=$(expr $RC + 1)

    return $RC

}

# Function:     test_case_03
# Description   - Test if wait mode ok at different freq
#
test_case_03()
{
    #TODO give TCID
    TCID="CPUFREQ_WAIT_MODE"
    #TODO give TST_COUNT
    TST_COUNT=3
    RC=0

    #print test info
    echo TINFO "test $TST_COUNT: $TCID "

    #TODO add function test script here
    count=0
    set_governor_userspace

    while [ $count -lt 7 ]; do
        count=$(expr $count + 1)
        value=${cpufreq_value[$RANDOM%${TOTAL_PT}]}
        echo $value
        cpufreq-set -f ${value}
        value_ret=$(cpufreq-info -f | grep $value | wc -l)
        if [ $value_ret -eq 1 ] ; then
            echo sleep...
            sleep 3
            echo "TSTINFO: CPUFREQ at $value "
            #test list
            powerstate_test.sh  || RC=$(expr $RC + 1)
        else
            return $RC
        fi
    done

    cpufreq-set -f ${cpufreq_value[0]}

    return $RC

}

# Function:     test_case_04
# Description   - Test if dvfs on the fly change
#
test_case_04()
{
    #TODO give TCID
    TCID="CPUFreq_change_on_the_fly"
    #TODO give TST_COUNT
    TST_COUNT=4
    RC=0

    #print test info
    echo TINFO "test $TST_COUNT: $TCID "

    set_governor_userspace
    run_auto_test_list &
    cpid=$!

    sleep 2
    pth_count=10
    while [ $pth_count -gt 0 ]; do
        value=${cpufreq_value[$RANDOM%${TOTAL_PT}]}
        echo $value
        cpufreq-set -f ${value}
        value_ret=$(cpufreq-info -f | grep $value | wc -l)
        if [ $value_ret -eq 1 ] ; then
            echo sleep...
            sleep 3
        else
            break;
        fi
        jobs -l
        pth_count=$(ps | grep $cpid | grep -v "grep" | wc -l)
    done

    wait $cpid
    RT=$?

    cpufreq-set -f ${cpufreq_value[0]}

    return $RC

}

# Function:     test_case_05
# Description   - CPUFreq change freq on the fly overnight test
#
test_case_05()
{
    #TODO give TCID
    TCID="CPUFreq_overnight_change"
    #TODO give TST_COUNT
    TST_COUNT=5
    RC=0

    #print test info
    echo TINFO "test $TST_COUNT: $TCID "

    i=1
    while [ $i -lt 500 ]; do
        echo $i
        test_case_04 || RC=$(expr $RC + 1)
        i=$(expr $i + 1)
    done

    return $RC

}

# Function:     test_case_06
# Description   - Test if interrupt latency measurement
#
test_case_06()
{
    #TODO give TCID
    TCID="CPUFreq_timer"
    #TODO give TST_COUNT
    TST_COUNT=6
    RC=0

    #print test info
    echo TINFO "test $TST_COUNT: $TCID "

    set_governor_userspace
	i=0
	while [ $i -lt $TOTAL_PT ];do
		value=${cpufreq_value[$i]}
        cpufreq-set -f ${value}
        value_ret=$(cpufreq-info -f | grep $value | wc -l)
        if [ $value_ret -eq 1 ] ; then
			timer_interrupt
		else
			RC=$(echo $RC $i)
		fi
        i=`expr $i + 1`
	done
    return $RC
}

pre_bus_mode()
{
	mount -t debugfs nodev /sys/kernel/debug
	axi_path=$(find /sys/kernel/debug/clock/osc_clk -name axi_clk)
	ddr_path=$(find /sys/kernel/debug/clock/osc_clk -name mmdc_ch1_axi_clk)
	a_stream_path=/mnt/nfs/test_stream/video/ToyStory3_H264HP_1920x1080_10Mbps_24fps_AAC_48kHz_192kbps_2ch_track1.h264
	platfm.sh
	platfm=$?
    mount -t tmpfs tmpfs /tmp
	cp /mnt/nfs/test_stream/alsa_stream/audio44k16S_long.wav /tmp/audio_test.wav
	cp ${LTPROOT}/testcases/bin/epdc_test /tmp/
	cp ${LTPROOT}/testcases/bin/dry2 /tmp/
	cp ${LTPROOT}/testcases/bin/rtc_testapp_6 /tmp/
}

clean_bus_mode()
{
	ifconfig eth0 up
	udhcpc eth0
    umount /tmp
}

check_status()
{
RC=0
axi_real=$(cat ${axi_path}/rate)
ddr_real=$(cat ${ddr_path}/rate)

declare -a axi;
declare -a ddr;

if [ $platfm -eq 63 ]; then
  axi=("24000000" "198000000" "264000000" "264000000");
  ddr=("24000000" "50000000"  "396000000" "528000000");
elif [ $platfm -eq 61  ];then
  axi=("24000000" "198000000" "264000000" "264000000");
  ddr=("24000000" "50000000" "396000000" "396000000");
elif [ $platfm -eq 60 ]; then
  axi=("24000000" "198000000" "198000000" "198000000");
  ddr=("24000000" "50000000" "396000000" "396000000");
else
  axi=("24000000" "198000000" "264000000" "264000000");
  ddr=("24000000" "50000000"  "396000000" "528000000");
fi

case "$1" in
low)
#axi bus to 24M
     while [ ${axi[0]} -ne $axi_real ]
	 do 
		 echo "$1 mode pedning wait more"
		 sleep 1
		 axi_real=$(cat ${axi_path}/rate)
		 ddr_real=$(cat ${ddr_path}/rate)
	 done
    ;;
audio)
    count=10
	while [ ${ddr[1]} -lt $ddr_real ] || [ ${ddr[1]} -eq $ddr_real ]
	do
		echo "$1 mode pedning wait more"
		 axi_real=$(cat ${axi_path}/rate)
		 ddr_real=$(cat ${ddr_path}/rate)
		 if [ $count -lt 1 ];then
		 	break
		 fi
		count=$(expr $count - 1 )
		sleep 1
	done
    ;;
medium)
    while [ ${ddr[2]} -ne  $ddr_real ]
	do
		echo "$1 mode pedning wait more"
		 axi_real=$(cat ${axi_path}/rate)
		 ddr_real=$(cat ${ddr_path}/rate)
		sleep 1
	done
    ;;
high)
    while [ ${ddr[3]} -ne $ddr_real ]
	do
		echo "$1 mode pedning wait more"
		 axi_real=$(cat ${axi_path}/rate)
		 ddr_real=$(cat ${ddr_path}/rate)
		sleep 1
	done
    ;;
*)
    ;;
esac

echo "$1 status is ok"

return $RC
}

screen_off()
{
	for i in $(find  /sys/class/graphics/ -name fb* )
		do
	if [ -e ${i}/blank  ]; then
		echo 1 > ${i}/blank	
	fi
	done
	
}

low_bus_mode()
{
	RC=0
	#screen off
	#ethernet off
	screen_off
	ifconfig eth0 down
	sleep 25
    check_status low	
	#now do suspned and resume
	/tmp/rtc_testapp_6 -m mem -T 50 || RC=1
	screen_off
	sleep 5
    check_status low	
	return $RC
}

audio_mode()
{
	RC=0
	#screen off
	#ethernet off
	screen_off
	ifconfig eth0 down
	sleep 25
	aplay /tmp/audio_test.wav &
	check_status audio
	wait
	/tmp/rtc_testapp_6 -m mem -T 50
	return $RC
}

medium_mode()
{
#note this can not be test automatically, as usboh can not be off when there is usb
	RC=0
	#screen off
	#ethernet off
	screen_off
	ifconfig eth0 down
	sleep 5
	#usboh3_clk is the medium one
	mount /dev/sda1 /mnt/sda1
	/tmp/rtc_testapp_6 -m mem -T 50 || RC=1
	umount /dev/sda1 /mnt/sda1
}

high_mode()
{
	RC=0
	ifconfig eth0 up
	udhclient || dhclient
	echo 0 > /sys/class/graphics/fb0/blank
	echo 0 > /sys/class/graphics/fb1/blank
	sleep 5
	/tmp/rtc_testapp_6 -m mem -T 50
	check_status high
	/tmp/epdc_test -T 7
	wait
	return $RC
}




# Function:     test_case_07
# Description   - bus freq state transition test
#
test_case_07()
{
    #TODO give TCID
    TCID="CPUFreq_timer"
    #TODO give TST_COUNT
    TST_COUNT=7
    RC=0

    #print test info
    echo TINFO "test $TST_COUNT: $TCID "

    #echo interactive > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
    sleep 5
	cnt=0
	pre_bus_mode
	LOOPS=$TOTAL_PT
	while [ $cnt -lt $LOOPS ]; do
		low_bus_mode || RC=$(expr $RC + 1)
		audio_mode || RC=$(expr $RC + 1)
		high_mode || RC=$(expr $RC + 1)
		cnt=$(expr $cnt + 1)
	done

	clean_bus_mode
    return $RC
}

# Function:     test_case_08
# Description   - bus freq state transition test
#
test_case_08()
{
    #TODO give TCID
    TCID="CPUFreq_timer"
    #TODO give TST_COUNT
    TST_COUNT=8
    RC=0

    #print test info
    echo TINFO "test $TST_COUNT: $TCID "

    #echo interactive > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
    sleep 5
	cnt=0
	pre_bus_mode
	LOOPS=100
	while [ $cnt -lt $LOOPS ]; do
		low_bus_mode || RC=$(expr $RC + 1)
	done

	clean_bus_mode
    return $RC
}


usage()
{
    echo "$0 [case ID]"
    echo "1: CPU working points randomly switch"
    echo "2: run modules test with DVFS core enabled"
    echo "3: test wait mode on random CPU working points"
    echo "4: run modules test with CPU working points switching on the fly"
    echo "5: stress test of 4"
    echo "6: test interrupt latency on random CPU working points"
    echo "7: low power mode loop"
    echo "8: low power mode suspend"

    exit 1
}

# main function

RC=0
UART=/dev/ttymxc0
TOTAL_PT=0

#TODO check parameter
if [ $# -ne 1 ]
then
    usage
    exit 1
fi

# cpufreq_value[] array will be discovered in setup()
declare -a cpufreq_value

source `which api_storage`

setup || exit $RC

case "$1" in
1)
    test_case_01 || exit $RC
    ;;
2)
    test_case_02 || exit $RC
    ;;
3)
    test_case_03 || exit $RC
    ;;
4)
    test_case_04 || exit $RC
    ;;
5)
    test_case_05 || exit $RC
    ;;
6)
    test_case_06 || exit $RC
    ;;
7)
    test_case_07 || exit $RC
    ;;
8)
    test_case_08 || exit $RC
    ;;
*)
    usage
    ;;
esac

echo TINFO "Test PASS"

