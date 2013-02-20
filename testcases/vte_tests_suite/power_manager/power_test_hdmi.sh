#!/bin/bash -x

setup()
{
    RC=0
    # Total number of test cases in this file.
    export TST_TOTAL=3

    # The TCID and TST_COUNT variables are required by the LTP
    # command line harness APIs, these variables are not local to this program.

    # Test case identifier
    export TCID="HDMI_POWER_test"
    # Set up is initialized as test 0
    export TST_COUNT=0
    # Initialize cleanup function to execute on program exit.
    # This function will be called before the test program exits.
    trap "cleanup" 0

    if [ $(cat /proc/cmdline | grep hdmi | wc -l) -eq 1 ]; then
        echo "Already enable HDMI in boot cmdline"
        if [ "$(cat /sys/devices/platform/mxc_hdmi/cable_state)" = "plugout" ]; then
            echo "Not plug in HDMI cable in board"
            RC=1
            return $RC
        fi
    else
        echo "Not enable HDMI in boot cmdline"
        RC=1
        return $RC
    fi

    num=`aplay -l |grep -i "imxhdmisoc" |awk '{ print $2 }'|sed 's/://'`
		#find the fb device related with hdmi
		dev_names=`cat /sys/class/graphics/fb*/fsl_disp_dev_property`
		i=0
		for name in $dev_names; do
		if [ "$name" = "hdmi" ]; then
			hdmi_fb="fb$i"
			let out_video=17+i
			break
		fi
		let i=i+1
		done
    tmpdir=`mktemp -d -p /tmp`
		alsa_stream="audio44k24S-S24_LE_long.wav"
		video_stream="ToyStory3_H264HP_1920x1080_10Mbps_24fps_AAC_48kHz_192kbps_2ch_track1.h264"
    cp /mnt/nfs/test_stream/alsa_stream_music/${alsa_stream} $tmpdir || RC=1
    cp $STREAM_PATH/video/$video_stream $tmpdir || RC=1
    return $RC
}

cleanup()
{
    echo "CLEANUP "
	kill -9 $bpid
    rm -rf $tmpdir
}

usage()
{
    echo "1: for device suspend resume case for no boot cores "
    echo "2: for device suspend resume case for all cores "
    exit 1
}

# Function:     test_case_01
# Description   - Test if device suspend and resume without bootcore
#
test_case_01()
{
    #TODO give TCID
    TCID="HDMI_PM_NOBOOTCORE"
    #TODO give TST_COUNT
    TST_COUNT=1
    RC=1

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "

    #TODO add function test scripte here
    echo 0 > /sys/class/graphics/fb0/blank
    echo -e "\033[9;0]" > /dev/tty0
		while [ true ]; do
	      /unit_tests/mxc_vpu_test.out -D "-f 2 -i $tmpdir/${video_stream} -x $out_video -a 30" &
        aplay -Dplughw:$num -M $tmpdir/${alsa_stream}
		done &
		bpid=$!
		sleep 10
    tloops=20000
    count=0
    while [ $count -lt $tloops ]
    do
        sleep 5
        echo "core test"
        i=0
        loops=10
        echo core > /sys/power/pm_test
        while [ $i -lt $loops ]
        do
            i=$(expr $i + 1)
            echo mem > /sys/power/state
			sleep 5
            echo standby > /sys/power/state
			sleep 5
        done

        sleep 30
        echo none > /sys/power/pm_test

        count=$(expr $count + 1)
    done
	kill -9 $bpid
    RC=0
    return $RC
}

# Function:     test_case_02
# Description   - Test if device suspend and resume without bootcore
#
test_case_02()
{
    #TODO give TCID
    TCID="HDMI_PM_BOOTCORE"
    #TODO give TST_COUNT
    TST_COUNT=2
    RC=1

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "
    echo 0 > /sys/class/graphics/fb0/blank
    echo -e "\033[9;0]" > /dev/tty0
    tloops=20000
    count=0
    #TODO add function test scripte here
	while [ true ]; do
        aplay -Dplughw:$num -M $tmpdir/${alsa_stream}
	done &
	bpid=$!
    while [ $count -lt $tloops ]
    do
        sleep 5
        i=0
        loops=10
        while [ $i -lt $loops ]
        do
            i=$(expr $i + 1)
            rtc_testapp_6 -T 50 -m mem
			sleep 5
            rtc_testapp_6 -T 50 -m standby
			sleep 5
        done

        count=$(expr $count + 1)
    done
	kill -9 $bpid
    RC=0
    return $RC
}

# Function:     test_case_03
# Description   - Test if device suspend and resume without bootcore
#
test_case_03()
{
    #TODO give TCID
    TCID="HDMI_PM_WAITMODE"
    #TODO give TST_COUNT
    TST_COUNT=3
    RC=1

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "

    #TODO add function test scripte here
    echo 0 > /sys/class/graphics/fb0/blank
    aplay -Dplughw:$num -M $tmpdir/${alsa_stream} < /dev/null &
    bpid=$!
    sleep 5
    echo "core test"
    i=0
    loops=10
    echo core > /sys/power/pm_test
    while [ $i -lt $loops ]
    do
        i=$(expr $i + 1)
        echo standby > /sys/power/state
    done

    sleep 30
    echo none > /sys/power/pm_test

    wait $bpid

    RC=0
    return $RC
}


setup || exit 1

case "$1" in
1)
    test_case_01 || exit 2
    ;;
2)
    test_case_02 || exit 3
    ;;
3)
    test_case_03 || exit 4
    ;;
*)
    usage
    ;;
esac

tst_resm TINFO "Test Finish"
