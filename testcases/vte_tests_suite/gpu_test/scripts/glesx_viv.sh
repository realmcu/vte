#!/bin/sh
###############################################################################
# Copyright (C) 2011-2013 Freescale Semiconductor, Inc. All Rights Reserved.
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
#    @file   glesx_test.sh
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
#Shelly Cheng                 09/02/2013       N/A      add more test demo and restructure the sceipt
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

	if [ -n "$trip_hot_old" ]; then
	 	echo $trip_hot_old > /sys/devices/virtual/thermal/thermal_zone0/trip_point_1_temp
	fi
	if [ -n "$trip_act_old" ]; then
	 	echo $trip_act_old > /sys/devices/virtual/thermal/thermal_zone0/trip_point_2_temp
	fi
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
	./egl_2 "width=800,height=600"|| RC=$(echo $RC egl_test)

	echo "ES1.1 Test case"
	echo "==========================="
	echo simple_draw
	echo "==========================="
	cd eglx_es1_1
	./eglx_es1_1 "width=800,height=600, subcase=0, loop=300" || RC=$(echo $RC simple_draw)

	echo "==========================="
	echo texture fence
	echo "==========================="
	cd ${TEST_DIR}/${APP_SUB_DIR}
	./eglx_es1_2 "width=800,height=600, subcase=0, loop=10,texwidth=1024,texheight=1024"|| RC=$(echo $RC eglx_es1_2_test)
	./eglx_es1_2 "width=400,height=600, subcase=0, loop=20,texwidth=32,texheight=32"|| RC=$(echo $RC eglx_es1_2_test)

	echo "==========================="
	echo shared context
	echo "==========================="
    ./eglx_es1_3 "width=800,height=600, loop=300"|| RC=$(echo $RC eglx_es1_3_test)


	echo "ES2.0 Test case"
	echo "==========================="
	echo simple draw vertex color
	echo "==========================="
	./eglx_es2_1 "width=800,height=600,subcase=0,loop=300" || RC=$(echo $RC simple_draw_vertex__test)

	echo "==========================="
	echo test synchronization of eglSwapbuffers
	echo "==========================="
	./eglx_es2_1 "width=800,height=600,subcase=0,loop=10,eglsynctest=20" || RC=$(echo $RC sync_eglswap_test)
	echo "==========================="
	echo rgb texture
	echo "==========================="
	./eglx_es2_1 "width=800,height=600,subcase=2,loop=30,texwidth=1024,texheight=1024" || RC=$(echo $RC rgb_testure_es2_1_test)

	echo "==========================="
	echo pure eglSwapBuffers
	echo "==========================="
	./eglx_es2_1 "width=800,height=600,subcase=3,loop=30" || RC=$(echo $RC pure_eglswapbuffer_test)

	echo "==========================="
	echo test texture fence
	echo "==========================="
	./eglx_es2_2 "width=800,height=600,subcase=0,loop=30,texwidth=1024,texheight=1024" || RC=$(echo $RC test_texture_fence_test)

	echo "==========================="
	echo sample_test ES2.0
	echo "==========================="
	cd ${TEST_DIR}/${APP_SUB_DIR}
	cd sample_test
	./sample_test 1000 || RC=$(echo $RC sample_test)

	cd ${TEST_DIR}/${APP_SUB_DIR}
	echo "==========================="
	echo mcube stencil test
	echo "==========================="
	./mcube 1000 || RC=$(echo $RC mcube)

	echo "==========================="
	echo mcube_es20 stencil test
	echo "==========================="
	./mcube_es2 1000 || RC=$(echo $RC mcube_es20)
	
	echo "==========================="
	echo glxs demo
	echo "==========================="
	cd ${TEST_DIR}/${APP_SUB_DIR}
	if [ -e GLXS ]; then
		cd GLXS/
		./glxs &
	fi
	pid_glxs=$!
    sleep 40
    kill  $pid_glxs
	RC=$(echo $RC glxs_demo)
	#echo "==========================="
	#echo draw quad
	#echo "==========================="
	#./glx_quad_1 "width=800,height=600" || RC=$(echo $RC draw_quad_test)

	wait	
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
	./egl_2 "width=800,height=600"|| RC=$(echo $RC egl_test) &
	pid_egl=$!

	echo "ES1.1 Test case"    
	echo "==========================="
	echo simple_draw
	echo "==========================="
	cd eglx_es1_1
	./eglx_es1_1 "width=800,height=600, subcase=0, loop=300" || RC=$(echo $RC simple_draw) &
	pid_es1_simdra=$!

	echo "==========================="
	echo texture fence                
	echo "==========================="	
	cd ${TEST_DIR}/${APP_SUB_DIR}
	./eglx_es1_2 "width=800,height=600, subcase=0, loop=10,texwidth=1024,texheight=1024"|| RC=$(echo $RC eglx_es1_2_test) &
	pid_es1_texfen_1=$!

	echo "==========================="
	echo shared context
	echo "==========================="
	#./eglx_es1_3 "width=800,height=600, loop=300"|| RC=$(echo $RC eglx_es1_3_test) &
	#pid_es1_shacon=$!


	echo "ES2.0 Test case"
	echo "==========================="
	echo simple draw vertex color
	echo "==========================="
	./eglx_es2_1 "width=800,height=600,subcase=0,loop=300" || RC=$(echo $RC simple_draw_vertex__test) &
	pid_es2_simvercol=$!

	echo "==========================="
	echo test synchronization of eglSwapbuffers
	echo "==========================="
	./eglx_es2_1 "width=800,height=600,subcase=0,loop=10,eglsynctest=20" || RC=$(echo $RC sync_eglswap_test) &
	pid_es2_synswapbuf=$!

	echo "==========================="
	echo rgb texture
	echo "==========================="
	./eglx_es2_1 "width=800,height=600,subcase=2,loop=30,texwidth=1024,texheight=1024" || RC=$(echo $RC rgb_testure_es2_1_test) &
	pid_es2_rgbtex=$!

	echo "==========================="
	echo pure eglSwapBuffers
	echo "==========================="
	./eglx_es2_1 "width=800,height=600,subcase=3,loop=30" || RC=$(echo $RC pure_eglswapbuffer_test) &
	pid_es2_purswapbuf=$!

	echo "==========================="
	echo test texture fence
	echo "==========================="
	./eglx_es2_2 "width=800,height=600,subcase=0,loop=30,texwidth=1024,texheight=1024" || RC=$(echo $RC test_texture_fence_test) &
	pid_es2_texfen=$!

	echo "==========================="
	echo sample_test ES2.0
	echo "==========================="
	cd sample_test
	./sample_test 1000 &
	pid_sample_test=$!
	#echo glx test
	#echo "==========================="
	#echo draw quad
	#echo "==========================="
	#./glx_quad_1 "width=800,height=random600" || RC=$(echo $RC draw_quad_test) &
	#pid_quad_1=$!
	wait $pid_egl&&wait $pid_es1_texfen_1&&wait $pid_es2_simvercol&&wait $pid_es2_synswapbuf&&wait $pid_es2_rgbtex&&wait $pid_es2_purswapbuf&&wait $pid_es2_texfen&&$pid_sample_test
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
    RC=$(echo $RC es20_conformance)
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
    ./eglx_es2_1 "width=800,height=600,subcase=0,loop=1000" &
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
    TCID="gles_pm_test"
    #TODO give TST_COUNT
    TST_COUNT=4
    RC=0

	#print test info
	tst_resm TINFO "test $TST_COUNT: $TCID "

	echo "==========================="
	echo "glmark2 test"
	echo "==========================="
	glmark2-es2

	cd ${TEST_DIR}/${APP_SUB_DIR}
	echo "==========================="
	echo "3DMark mm06 test"
	echo "==========================="

	if [ -e mm06/bin/fsl_imx_linux ]; then
		cd mm06/bin/fsl_imx_linux
		./fm_oes_player || RC="3Dmark"
	fi
	cd ${TEST_DIR}/${APP_SUB_DIR}
	echo "==========================="
    echo "3Dmark20 mm07 test"
    echo "==========================="
    if [ -e mm07 ]; then
        cd mm07
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
    if [ -e mirada ]; then
        cd mirada
        ./Mirada
    fi

    return 0
}


usage()
{
    echo "$0 [case ID]"
    echo "1: sequence test"
    echo "2: concurrent test"
    echo "3: conformance test"
    echo "4: pm test"
    echo "5: performance test"
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
rt="Yocto"
cat /etc/issue | grep Yocto || rt="others"

if [ $rt = "Yocto" ];then
    APP_SUB_DIR="yocto_1.5_x/bin"
    export VIV_DESKTOP=0
	export DISPLAY=:0.0
    export LD_LIBRARY_PATH=$TEST_DIR/yocto_1.5_x/lib

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
*)
    usage
    ;;
esac

tst_resm TINFO "Test Finish"
