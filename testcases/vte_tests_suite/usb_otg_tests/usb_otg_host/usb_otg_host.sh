#Copyright 2007-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
################################################################################################### 
# 
#    @file   usb_otg_host.sh
# 
#    @brief  Test Belcarra USB-OTGi Host Support
# 
################################################################################################### 
#Revision History: 
#                            Modification     Tracking 
#Author/core ID                  Date          Number    Description of Changes 
#-------------------------   ------------    ----------  -------------------------------------------
#V.Khalabuda/b00306           17-07-2007     ENGR45124   Initial version
# 
###################################################################################################

#!/bin/sh

setup()
{
        PATH="/bin:/usr/bin:/usr/lib:/usr/local/bin"
        export PATH
        export RC=0                     # Return code from commands.
        export TST_TOTAL=2              # total numner of tests in this file.
        export TCID="setup"             # this is the init function.
        export TST_COUNT=0              # init identifier,

        export IP_HOST="172.16.0.5"     # IP of the host side
        export STATE="up"               # State of the USB Network Interface
        export IFACE="usbl0"            # USB Network Interface on the boards

        if [ -z $TMP ]
        then
                LTPTMP=/tmp/tst_usb_otg.$$
        else
                LTPTMP=$TMP/tst_usb_otg.$$
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
        ifconfig usbl0 > $LTPTMP/tst_usb_otg.out 2>&1 || RC=$?
        if [ $RC -ne 0 ]
        then
                tst_brkm TBROK NULL "SETUP: USB Network Interface status is not establish/undefined"
        return $RC
        fi

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

test_config()
{
        TCID="test_config"      # Identifier of this testcase.
        TST_COUNT=1             # Test case number.
        RC=0                    # return code from commands.

        ! [ -z "$STATE" ] && {
                tst_resm TINFO "$STATE USB Network interface $IFACE ..."
                ifconfig $IFACE $IP_HOST 2>&1 || RC=$?
                if [ $RC -ne 0 ]
                then
                        tst_brkm TBROK NULL "Can't state USB Network interface"
                fi
                ifconfig $IFACE $STATE 2>&1 || RC=$?
                if [ $RC -ne 0 ]
                then
                        tst_brkm TBROK NULL "Can't $STATE USB Network interface, ERROR"
                fi
        }
  
        return $RC
}

test_pocket()
{
        TCID="test_pocket"      # Identifier of this testcase.
        TST_COUNT=2             # Test case number.
        RC=0                    # return code from commands.

        tst_resm TINFO "Sending pocket to $IP_HOST ..."
        echo -n "0" >$FILE_NAME 2>&1 || RC=$?
        ping -s 56 $IP_HOST 2>&1 || RC=$?
        if [ $RC -ne 0 ]
        then
                tst_brkm TBROK NULL "Can't send pocket on site $IP_HOST"
                return $RC
        fi

        return $RC
}

usage()
{
        echo "
        Usage:  $0 -[s state|r] [-f iface] -d ip_address
                        state = [up,down]
                        iface = [usbl0|usb0|usb|...]
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

while getopts "d:s:f:rh" opt
do
        case "$opt" in
        r) RESUME=1;;
        d) IP_HOST="$OPTARG";;
        s) STATE="$OPTARG";;
        f) IFACE="$OPTARG";;
        h|*) usage;;
        esac
done
shift `expr $OPTIND - 1`
[ -z "${ALLPARAM}" ] && usage

for i in "${ALLPARAM}"
do
        ! [ -z "$STATE" ] && {
                if [ "$STATE" = "up" ] || [ "$STATE" = "down" ]
                then
                        test_config || RC=$?
                        [ $RC -ne 0 ] && exit $RC
                else usage
                fi
        }
        ! [ -z "$RESUME" ] && ! [ -z "$IP_HOST" ]&& {
                test_pocket || RC=$?
                [ $RC -ne 0 ] && exit $RC
        }
done

# Initialize cleanup function.
trap "cleanup" 0
tst_resm TPASS "$0 test case worked as expected"
#echo -ne "$0\tPASSED\n"
exit $RC
