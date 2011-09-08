#!/bin/sh -x
###################################################################################################
#
#    @file   vpu_performance.sh
#
#    @brief  shell script for testcase design for VPU app
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#<Hake Huang>/-----             <2011/09/08>     N/A          Initial version
# 
###################################################################################################



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
export TST_TOTAL=1

export TCID="setup"
export TST_COUNT=0
RC=1

trap "cleanup" 0

#TODO add setup scripts
if [ -e /dev/mxc_vpu ]
then
RC=0
fi

#setup the fb on
echo 0 > /sys/class/graphics/fb0/blank

sleep 1
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
# Description   - Test if MPEG2 decode ok
#  
test_case_01()
{
#TODO give TCID 
TCID="vpu_PERFORMANCE"
#TODO give TST_COUNT
TST_COUNT=1
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

stream_path=/mnt/nfs/test_stream/video/mx51_vpu_performance_testvector

vpu_sequence_test.sh 2 0 0 ${stream_path} ${stream_path}/mx51_vpu_performance_test_filelist.txt

RC=0
return $RC
}

usage()
{
echo "usage $0 <1/2/3/4/5/6/7/8>"
echo "1: performance test"
}


#TODO check parameter
if [ $# -ne 1 ]
then
echo "usage $0 <1>"
exit 1 
fi

setup || exit $RC

case "$1" in
1)
  test_case_01 || exit $RC 
  ;;
*)
#TODO check parameter
  usage
  ;;
esac

tst_resm TINFO "Test PASS"

