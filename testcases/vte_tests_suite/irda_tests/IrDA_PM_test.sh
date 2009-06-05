################################################################################################### 
# 
#    @file   IrDA_PM_test.sh
# 
#    @brief  Callback of PM(Power Management) SUSPEND/RESUME methods for IrDA
# 
################################################################################################### 
#
#   Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
#   THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#   BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#   Freescale Semiconductor, Inc.
#     
################################################################################################### 
#Revision History: 
#			Modification	Tracking 
#Author			Date		Number		Description of Changes 
#------------------	------------	----------	-------------------------------------------
#V.HALABUDA/HLBV001	08-04-2005	TLSbo40417	Initial version 
# 
###################################################################################################

#!/bin/sh

setup()
{
	PATH="/bin:/usr/bin:/usr/lib:/usr/local/bin"
	export PATH
	export RC=0		# Return code from commands.
	export TST_TOTAL=2	# total numner of tests in this file.
	export TCID="setup"	# this is the init function.
	export TST_COUNT=0	# init identifier,

	export RESUME=""
	export FILE_NAME="/sys/devices/platform/mxc-fir0/power/state"
#	export FILE_STATE="/sys/power/state"
	export LEVEL="0"
	export STATE=""

	if [ -z $TMP ]
	then
		LTPTMP=/tmp/tst_dpm_test.$$
	else
		LTPTMP=$TMP/tst_dpm_test.$$
	fi

	# Initialize cleanup function.
	trap "cleanup" 0

	# create the temporary directory used by this testcase
	mkdir -p $LTPTMP > /dev/null 2>&1 || RC=$?
	if [ $RC -ne 0 ]
	then
		tst_brkm TBROK NULL "SETUP: Unable to create temporary directory"
	return $RC
	fi

	# Check device power status
	cat $FILE_NAME >$LTPTMP/tst_template.out 2>&1 || RC=$?
	if [ $RC -ne 0 ]
	then
		tst_brkm TBROK NULL "SETUP: Device power status is not establish/undefined"
	return $RC
	fi

#	cd $LTPTMP  

	return $RC
}

cleanup()
{
	RC=0

	cd $CUR_DIR

	if [ -d $LTPTMP ]
	then
		rm -rf $LTPTMP
	fi

	return $RC
}

test_suspend()
{
	TCID="test_suspend"	# Identifier of this testcase.
	TST_COUNT=1		# Test case number.
	RC=0			# return code from commands.

	tst_resm TINFO "Suspend $FILE_NAME ..."
	# echo "suspend powerdown $LEVEL" >$FILE_NAME 2>&1 || RC=$?
	echo -n "$LEVEL" >$FILE_NAME 2>&1 || RC=$?

	if [ $RC -ne 0 ]
	then
		tst_brkm TBROK NULL "Can't suspend, ERROR"
		return $RC
	fi
  
	return $RC
}

test_resume()
{
	TCID="test_resume"	# Identifier of this testcase.
	TST_COUNT=2		# Test case number.
	RC=0			# return code from commands.

	tst_resm TINFO "RESUME POWERON $FILE_NAME ..."
	# echo "resume poweron" >$FILE_NAME 2>&1 || RC=$?
	echo -n "0" >$FILE_NAME 2>&1 || RC=$?
	if [ $RC -ne 0 ]
	then
		tst_brkm TBROK NULL "Can't resume $FILE_NAME"
		return $RC
	fi

	return $RC
}

usage()
{
	echo "
	Usage:  $0 -[s level|r]
			level = [1,2,3]
	" >&2
	exit 2
}

# ------------------------------------------------------------------
# MAIN
# ------------------------------------------------------------------
RC=0    # Return value from setup, and test functions.

#Store curent dir
export CUR_DIR=`pwd`

setup || RC=$?
if [ $RC -ne 0 ]
then
	exit $RC
fi

ALLPARAM=$*

while getopts "s:rh" opt
do
	case "$opt" in
	r) RESUME=1;;
	s) LEVEL="$OPTARG";;
	h|*) usage;;
	esac
done
shift `expr $OPTIND - 1`
[ -z "${ALLPARAM}" ] && usage

for i in "${ALLPARAM}"
do
	! [ -z "$RESUME" ] && [ -z $LEVEL ] && {
		test_resume || RC=$?
		[ $RC -ne 0 ] && exit $RC
	}
	! [ -z "$LEVEL" ] && [ -z "$RESUME" ] && {
		if [ "$LEVEL" = "1" ] || [ "$LEVEL" = "2" ] || [ "$LEVEL" = "3" ]
		then
			test_suspend || RC=$?
			[ $RC -ne 0 ] && exit $RC
		else usage
		fi
	}
done

# Initialize cleanup function.
trap "cleanup" 0
tst_resm TPASS "$0 test case worked as expected"
#echo -ne "$0\tPASSED\n"
exit $RC
