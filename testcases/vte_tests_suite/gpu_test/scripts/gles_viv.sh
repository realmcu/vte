#!/bin/sh
###############################################################################
# Copyright (C) 2013 Freescale Semiconductor, Inc. All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
###############################################################################
#
#    @file   gles_test.sh
#
#    @brief  shell script template for testcase design "gpu" is where to modify block.
#
################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#------------------------   ------------    ----------  -----------------------
#Hake Huang/-----             20110817     N/A          Initial version
#Andy Tian                    05/15/2012       N/A      add wait for background
#Andy Tian                    12/14/2012       N/A      add GPU thermal test
#Shelly Cheng                 01/02/2013       N/A      add more test demo
#Shelly Cheng                 05/17/2013       N/A      add more demo
# 
################################################################################

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

    export TCID="setup"
    export TST_COUNT=0
    RC=0

    trap "cleanup" 0 3
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


# Function:     test_case_01
# Description   - Test if gles applications sequence ok
#  
test_case_01()
{
    #TODO give TCID 
    TCID="gles_test"
    #TODO give TST_COUNT
    TST_COUNT=1
    RC=0

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "

    #TODO add function test scripte here
    echo "egl Test case"    

    cd ${TEST_DIR}/${APP_SUB_DIR}
    echo "==========================="
    echo egl_test
    echo "==========================="
    ./egl_test || RC=egl_test

    echo "ES1.1 Test case"    
    
    cd ${TEST_DIR}/${APP_SUB_DIR}
    echo "==========================="
    echo cube
    echo "==========================="
    ./cube 1000 || RC=$(echo $RC cube)

    cd ${TEST_DIR}/${APP_SUB_DIR}
    echo "==========================="
    echo mcube stencil test
    echo "==========================="
    ./mcube 1000 || RC=$(echo $RC mcube)

    echo "==========================="
    echo gpubench
    echo "==========================="
    cd gpubench
    ./gpuBench || RC=$(echo $RC gpuBench)

    echo "ES2.0 Test case"    
    cd ${TEST_DIR}/${APP_SUB_DIR}
    echo "==========================="
    echo simple draw ES2.0
    echo "==========================="
    ./simple_draw 100 || RC=$(echo $RC simple draw)
    ./simple_draw 100 -s || RC=$(echo $RC simple draw -s)

    echo "==========================="
    echo torusknot ES2.0
    echo "==========================="
    ./torusknot || RC=$(echo $RC torusknot)

    echo "==========================="
    echo model3d ES2.0
    echo "==========================="
    cd ${TEST_DIR}/${APP_SUB_DIR}
	cd model3d
	./model3d 1000 1000 || RC=$(echo $RC model3d)

    echo "==========================="
    echo sample_test ES2.0
    echo "==========================="
    cd ${TEST_DIR}/${APP_SUB_DIR}
	./sample_test 1000 || RC=$(echo $RC sample_test)

    echo "==========================="
    echo mcube_es20 stencil test
    echo "==========================="
    ./mcube_es2 1000 || RC=$(echo $RC mcube_es20)

    if [ "$RC" = "0" ]; then
        RC=0
    else
        RC=1
    fi

    return $RC

}

# Function:     test_case_02
# Description   - Test if gles concurrent ok
#  
test_case_02()
{
    #TODO give TCID 
    TCID="gles_con_test"
    #TODO give TST_COUNT
    TST_COUNT=2
    RC=0

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "

    #TODO add function test scripte here


    cd ${TEST_DIR}/${APP_SUB_DIR}
    echo "==========================="
    echo egl_test
    echo "==========================="
    ./egl_test &
    pid_egl=$!

    echo "==========================="
    echo fps triangle
    echo "==========================="
    ./fps_triangle 10000 &
    pid_tri=$!

    echo "==========================="
    echo simple draw
    echo "==========================="
    ./simple_draw 1000 &
    pid_s1=$!
    ./simple_draw 1000 -s &
    pid_s2=$!

    echo "==========================="
    echo simple triangle
    echo "==========================="
    ./simple_triangle &
    pid_sTri=$!

    echo "==========================="
    echo torusknot
    echo "==========================="
    ./torusknot &
    pid_tor=$!

    echo "==========================="
    echo model3d ES2.0
    echo "==========================="
    ./model3d 1000 1000 &
    pid_m3d=$!
    echo "==========================="
    echo sample_test ES2.0
    echo "==========================="
    ./sample_test 1000 &

    wait $pid_egl && wait $pid_tri && wait $pid_s1 && wait $pid_s2 && wait $pid_sTri && wait $pid_tor && wait $pid_m3d
    RC=$?
    wait

    if [ $RC -eq 0 ]; then
        echo "TEST PASS"
    else
        RC=1
        echo "TEST FAIL"
    fi
    return $RC
}

# Function:     test_case_03
# Description   - Test if gles conformance ok
#  
test_case_03()
{
    #TODO give TCID 
    TCID="gles_conform_test"
    #TODO give TST_COUNT
    TST_COUNT=3
    RC=0

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "

    #TODO add function test scripte here
    cd ${TEST_DIR}/${APP_SUB_DIR}
    echo "==========================="
    echo es11 conformance
    echo "==========================="
    cd es11_conform/conform
    conform/conform -r 32555 -l conform/TESTLIST && \
        conform/conform -r 32556 -l conform/TESTLIST -p 1 && \
        conform/conform -r 32557 -l conform/TESTLIST -p 2 && \
        conform/conform -r 32558 -l conform/TESTLIST -p 3 \
        || RC=es11_conformance

    cd ${TEST_DIR}/${APP_SUB_DIR}
    echo "==========================="
    echo es20 conformance
    echo "==========================="
    if [ -e es20_conform/GTF_ES/glsl/GTF/GTF ]; then
        cd es20_conform/GTF_ES/glsl
        ./GTF/GTF -width=64 -height=64 -noimagefileio \
            -l=/home/root/es20_conformance_mustpass_64x64 -run="$(pwd)/GTF/mustpass.run" \
            && ./GTF/GTF -width=113 -height=47 -noimagefileio \
            -l=/home/root/es20_conformance_mustpass_113x47 -run="$(pwd)/GTF/mustpass.run" \
            && ./GTF/GTF -width=640 -height=480 -noimagefileio \
            -l=/home/root/es20_conformance_mustpass_640x480 -run="$(pwd)/GTF/mustpass.run" \
            || RC=$(echo $RC es20_conformance)
    fi

    echo $RC

    if [ "$RC" = "0" ]; then
        RC=0
    else
        RC=1
    fi

    return $RC
}

test_case_04()
{
    #TODO give TCID 
    TCID="gles_pm_test"
    #TODO give TST_COUNT
    TST_COUNT=4
    RC=0

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "

    cd ${TEST_DIR}/${APP_SUB_DIR}
    ./simple_draw 10000 &
    pid=$!

    rtc_testapp_6 -T 50
    sleep 1
    rtc_testapp_6 -T 50
    sleep 1
    rtc_testapp_6 -T 50
    sleep 1
    rtc_testapp_6 -T 50
    sleep 1
    rtc_testapp_6 -T 50
    sleep 1

    wait $pid
    RC=$?

    if [ $RC = 0 ];then
        echo "TEST PASS"
    else
        echo "TEST FAIL"
    fi
    return $RC
}

test_case_05()
{
    #TODO give TCID 
    TCID="gles_perf_test"
    #TODO give TST_COUNT
    TST_COUNT=4
    RC=0

	#print test info
	tst_resm TINFO "test $TST_COUNT: $TCID "

	cd ${TEST_DIR}/${APP_SUB_DIR}
	echo "==========================="
	echo "3DMark mm06 test"
	echo "==========================="
	if [ -e 3DMarkMobile/fsl_imx_linux ]; then
		cd 3DMarkMobile/fsl_imx_linux/
		./fm_oes_player || RC="3Dmark"
	fi

    cd ${TEST_DIR}/${APP_SUB_DIR}
    echo "==========================="
    echo "3Dmark20 mm07 test"
    echo "==========================="
    if [ -e mm07_v21 ]; then
        cd mm07_v21
        ./fm_oes2_mobile_player
    fi

    cd ${TEST_DIR}/${APP_SUB_DIR}
    echo "==========================="
    echo "3Dmark22 test"
    echo "==========================="
    if [ -e basemark_v2 ]; then
        cd basemark_v2
        ./fm_oes2_player
    fi

    cd ${TEST_DIR}/${APP_SUB_DIR}
    echo "==========================="
    echo "Mirada test"
    echo "==========================="
    if [ -e Mirada ]; then
        cd Mirada
        ./Mirada
    fi

    return 0
}


# Function:     test_case_06
# Description   - Test if gpu works ok when changed thermal
#  
test_case_06()
{
    #TODO give TCID 
    TCID="gles_thermal_test"
    #TODO give TST_COUNT
    TST_COUNT=6
    RC=0

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "



	thermal_dir= 
	trippoint1_dir=$(find /sys/ -name trip_point_1_temp)
	thermal_dir=`dirname $trippoint1_dir`
	trip_hot_old=`cat $trippoint1_dir`
	trip_act_old=`cat $thermal_dir/trip_point_2_temp`
	cur_temp=`cat $thermal_dir/temp`
	cd ${TEST_DIR}/${APP_SUB_DIR}
	if [ $cur_temp -lt $trip_hot_old ]; then
		norm_fps=`./simple_draw 200 | grep FPS | cut -f3 -d:`
	else
		echo "Already in trip hot status"
		exit 6
	fi

	let trip_hot_new=cur_temp-10
	let trip_act_new=cur_temp-15
	# Set new trip hot value to trigger the trip_hot flag
	echo ${trip_act_new} > $thermal_dir/trip_point_2_temp
	echo ${trip_hot_new} > $thermal_dir/trip_point_1_temp

	sleep 2
	if [ $cur_temp -gt $trip_hot_new ]; then
		low_fps=`./simple_draw 200 | grep FPS | cut -f3 -d: `
	else
		echo "Set trip hot flag failure"
		exit 6
	fi

	let drop_fps=norm_fps-low_fps
	let half_fps=norm_fps/2
	[ $drop_fps -gt $half_fps ] || RC=6
	if [ -n "$trip_hot_old" ]; then
		echo $trip_hot_old >  $thermal_dir/trip_point_1_temp
	fi
	if [ -n "$trip_act_old" ]; then
		echo $trip_act_old >  $thermal_dir/trip_point_2_temp
	fi

    return $RC

}
# Function:     test_case_07
# Description   - Test directviv_test
#
test_case_07()
{
	    #TODO give TCID
		TCID="directviv_test"
		#TODO give TST_COUNT
		TST_COUNT=6
		RC=0

		cd ${TEST_DIR}/${APP_SUB_DIR}
		echo "==========================="
		echo directviv_test 
		echo "==========================="
		cd directviv_test
		echo "==========================="
		echo TexDirect_ES11 
		echo "==========================="
		./TexDirect_ES11 -f 5000 || RC=TexDirect_ES11
		echo "==========================="
		echo TexDirectMap_ES11 
		echo "==========================="
		./TexDirectMap_ES11 -f 5000 || RC=$(echo $RC TexDirectMap_ES11)
		echo "==========================="
		echo TexDirectTiledMap_ES11 
		echo "==========================="
		./TexDirectTiledMap_ES11 -f 5000 || RC=$(echo $RC TexDirectTiledMap_ES11)
		echo "==========================="
		echo TexDirect_ES20 
		echo "==========================="
		./TexDirect_es20 -f 5000 || RC=TexDirect_es20
		echo "==========================="
		echo TexDirectMap_ES20 
		echo "==========================="
		./TexDirectMap_ES20 -f 5000 || RC=$(echo $RC TexDirectMap_ES20)
		echo "==========================="
		echo TexDirectTiledMap_ES20 
		echo "==========================="
		./TexDirectTiledMap_ES20 -f 5000 || RC=$(echo $RC TexDirectTiledMap_ES20)

		RC=$?
		if [ $RC -eq 0 ]; then
			echo "TEST PASS"
		else
			echo "TEST FAIL"
		fi
		return $RC	
}
# Function:     test_case_08
# Description   - Test sample test
#
test_case_08()
{
	    #TODO give TCID
		TCID="FB MULTIPLEBUFFER test"
		#TODO give TST_COUNT
		TST_COUNT=1
		RC=0
		cd ${TEST_DIR}/${APP_SUB_DIR}
		echo "==========================="
		echo fb multiple buffer test
		echo "==========================="
		export FB_MULTI_BUFFER=1
		echo FB_MULTI_BUFFER=1
		./cube 300 
		./model3d 1000 300
		export FB_MULTI_BUFFER=2
		echo FB_MULTI_BUFFER=2
		./cube 300 
		./model3d 1000 300
		export FB_MULTI_BUFFER=3
		echo FB_MULTI_BUFFER=3
		./cube 300 
		./model3d 1000 300
		export FB_MULTI_BUFFER=2
		echo FB_MULTI_BUFFER=2
		./cube 300 
		./model3d 1000 300
		export FB_MULTI_BUFFER=1
		echo FB_MULTI_BUFFER=1
		./cube 300 
		./model3d 1000 300
		export FB_MULTI_BUFFER=0
		echo FB_MULTI_BUFFER=0
		./cube 300 
		./model3d 1000 300
		
		export FB_FRAMEBUFFER_0=/dev/fb1
		echo 0 > /sys/class/graphics/fb1/blank
		./cube 300 
		./model3d 1000 300
		echo 1 > /sys/class/graphics/fb1/blank
		echo 0 > /sys/class/graphics/fb0/blank
		export FB_FRAMEBUFFER_0=/dev/fb0
		
		RC=$?
		if [ $RC -eq 0 ]; then
			echo "TEST PASS"
		else
			echo "TEST FAIL"
	    fi
        return $RC
}

test_case_09()
{
	    #TODO give TCID
		TCID="vdk test"
		#TODO give TST_COUNT
		TST_COUNT=1
		RC=0
		cd ${TEST_DIR}/${APP_SUB_DIR}
		echo "==========================="
		echo vdk test
		echo "==========================="
        cd /opt/viv_samples/vdk; 
       
        for i in `ls tutorial*` 
            do 
                echo $i;
                ./$i -f 1000;                 
            done
        echo tiger
        /opt/viv_samples/tiger/tiger
        echo tvui "Press ESC to exit"
		cd /opt/viv_samples/hal/
        ./tvui
		
		RC=$?
		if [ $RC -eq 0 ]; then
			echo "TEST PASS"
		else
			echo "TEST FAIL"
	    fi
        return $RC
}

usage()
{
    echo "$0 [case ID]"
    echo "1: sequence test"
    echo "2: concurrent test"
    echo "3: conformance test"
    echo "4: pm test"
    echo "5: performance test"
    echo "6: Thermal control test"
    echo "7: TexDirect_viv"
    echo "8: FB MULTIBUFFER test"
    echo "9: VDK test"

}

# main function

RC=0

#TODO check parameter
if [ $# -ne 1 ]
then
    usage
    exit 1 
fi

TEST_DIR=/mnt/nfs/util/Graphics/

APP_SUB_DIR=

setup || exit $RC
#judge rootfs type
rt="Ubuntu"
cat /etc/issue | grep Ubuntu || rt="others"

if [ $rt = "Ubuntu" ];then
    APP_SUB_DIR="ubuntu_11.10/test"
    export DISPLAY=:0.0
    export XAUTHORITY=/home/linaro/.Xauthority 
else
    #judge the rootfs
    platfm.sh
    case "$?" in
    41)
        APP_SUB_DIR="imx51_rootfs/test"
        ;;
    51)
        APP_SUB_DIR="imx51_rootfs/test"
        ;;
    53)
        APP_SUB_DIR="imx53_rootfs/test"
        ;;
    61)
        APP_SUB_DIR="imx61_rootfs/test"
        ;;
    63)
        APP_SUB_DIR="imx61_rootfs/test"
        ;;
    *)
        exit 0
        ;;
    esac
fi


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
9)
    test_case_09 || exit $RC
    ;;
*)
    usage
    ;;
esac

tst_resm TINFO "Test Finish"
