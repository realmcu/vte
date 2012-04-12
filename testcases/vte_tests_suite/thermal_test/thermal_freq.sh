#!/bin/sh -x
##############################################################################
#Copyright (C) 2011,2012 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
################################################################################
#Change History:
#
#Author               Date        Description of Changes
#-----------------   -----------  -----------------------
#
#Andy Tian/-----     11/16/2011   1, Set the hot temp and active temp with a gap of 10
#                                 to make the cpu frequency change event easy to see;
#                                 2, re-struct the code according to the template
#Spring              04/12/2012   Change cpufreq governor for it's enabled
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
    RC=1
    #TODO Total test case
    export TST_TOTAL=1

    export TCID="setup"
    export TST_COUNT=0

    #trap exit and ctrl+c
    trap "cleanup" 0 2


    #TODO add setup scripts

    THERMO_PATH="/sys/devices/virtual/thermal/thermal_zone0"

    loops=10

    #store the old temp setting
    normal_temp=$(cat ${THERMO_PATH}/temp)
    trip0=$(cat ${THERMO_PATH}/trip_point_0_temp)
    trip1=$(cat ${THERMO_PATH}/trip_point_1_temp)
    trip2=$(cat ${THERMO_PATH}/trip_point_2_temp)

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

    #TODO add cleanup code here
    # Recover the original thermal setting

    echo $trip2 > ${THERMO_PATH}/trip_point_2_temp
    echo $trip1 > ${THERMO_PATH}/trip_point_1_temp
    if [ $RC -ne 0 ]
    then
        tst_resm TINFO "Test FAIL"
    fi

    exit $RC
}

usage()
{
    echo "Usage: $0"
    exit 1
}

test_case_01()
{
    RC=1
    #TODO give TCID
    TCID="thermal_frequency_test"
    #TODO give TST_COUNT
    TST_COUNT=1

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "


    # change cpufreq to userspace governor
    cpufreq-set -g userspace
    maxfreq=$(cpufreq-info -l|awk '{print $2}')
    cpufreq-set -f $maxfreq

    i=0
    while [ $i -lt $loops ]
    do
        i=$(expr $i + 1 )

        #change the hot value
        # we expect the cpufreq is lower when hot critical is meet

        test_temp_hot=$(expr $normal_temp - 5)
        test_temp_active=$(expr $normal_temp - 15)

        old_cpufreq=$(cpufreq-info -f)

        echo $test_temp_active > ${THERMO_PATH}/trip_point_2_temp
        echo $test_temp_hot > ${THERMO_PATH}/trip_point_1_temp

        sleep 5

        cur_cpufreq=$(cpufreq-info -f)

        if [ $cur_cpufreq -lt $old_cpufreq ];then
            echo "PASS 1"
        else
            echo "Fail 1"
            RC=$i
            return $i
        fi

        #move the trip temp back
        # we expect the cpufreq up
        echo $trip2 > ${THERMO_PATH}/trip_point_2_temp
        echo $trip1 > ${THERMO_PATH}/trip_point_1_temp

        sleep 5

        cur_cpufreq2=$(cpufreq-info -f)

        if [ $cur_cpufreq2 -gt $cur_cpufreq ];then
            echo "PASS 2"
        else
            echo "Fail 2"
            RC=$i
            return $i
        fi

    done

    RC=0

    return $RC
}

# main function

if [ $# -ne 0 ]
then
    usage
    exit 1
fi

setup || exit $RC

test_case_01 || exit $RC

tst_resm TINFO "Test PASS"
