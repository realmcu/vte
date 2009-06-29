#!/bin/sh
###################################################################################################
#
#    @file   runantests.sh
#
#    @brief  shell script template for run semi-auto test case is.
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
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#Hake Huang/-----             Sept. 27th 2008     0.9          Initial version
# 
###################################################################################################

#
# this script is used in the frame work of ltp & vte which needs the 
# modification of ltp/pan.c to enable child process input
# 

setup()
{
    cd `dirname $0` || \
    {
        echo "FATAL: unable to change directory to $(dirname $0)"
        exit 1
    }

    export LTPROOT=${PWD}
    export TMPBASE="/tmp"
    export TMP="${TMPBASE}/ltp-$$"
    export PATH="${PATH}:${LTPROOT}/testcases/bin:${LTPROOT}/testscripts"
    export PATH="${PATH}:${LTPROOT}/testscripts/common/storage_scripts/"
    
     [ -d $LTPROOT/testcases/bin ] ||
    {
        echo "FATAL: Test suite not installed correctly"
        echo "INFO: as root user type 'make ; make install'"
        exit 1
    }

    [ -e $LTPROOT/pan/pan ] ||
    {
        echo "FATAL: Test suite driver 'pan' not found"
        echo "INFO: as root user type 'make ; make install'"
        exit 1
    }
    
    mount -t tmpfs tmpfs /tmp

}

usage() 
{
    cat <<-EOF >&2

    usage: ./${0##*/} -c [-d TMPDIR] [-f CMDFILE ] [-i # (in Mb)] 
    [ -l LOGFILE ] [ -o OUTPUTFILE ] [ -m # (in Mb)] -N -n -q 
    [ -r LTPROOT ] [ -s PATTERN ] [ -t DURATION ] -v [ -x INSTANCES ] 
                
    -c NUM_PROCS    Run LTP under additional background CPU load.
    -d TMPDIR       Directory where temporary files will be created.
    -e              Prints the date of the current LTP release
    -f CMDFILE      Execute user defined list of testcases.
    -h              Help. Prints all available options.
    -i # (in Mb)    Run LTP with a _min_ IO load of # Mb in background.
    -l LOGFILE      Log results of test in a logfile.
    -m # (in Mb)    Run LTP with a _min_ memory load of # Mb in background.
    -n              Run LTP with network traffic in background.
    -o OUTPUTFILE   Redirect test output to a file.
    -p              Human readable format logfiles. 
    -q              Print less verbose output to screen.
    -r LTPROOT      Fully qualified path where testsuite is installed.
    -s PATTERN       Only run test cases which match PATTERN in CMDFILE.
    -t DURATION     Execute the testsuite for given duration. Examples:
                      -t 60s = 60 seconds
                      -t 45m = 45 minutes
                      -t 24h = 24 hours
                      -t 2d  = 2 days
    -v              Print more verbose output to screen.                   
    -x INSTANCES    Run multiple instances of this testsuite.

    example: ./${0##*/} -i 1024 -m 128 -p -q  -l /tmp/resultlog.$$ -d ${PWD}


	EOF
exit 0
}

main()
{
    local CMDFILE=""
    local PRETTY_PRT=""
    local ALT_DIR=0
    local QUIET_MODE=""
    local VERBOSE_MODE=""
    local NETPIPE=0
    local GENLOAD=0
    local MEMSIZE=0
    local DURATION=""
    local BYTESIZE=0
    local LOGFILE=""
    local PRETTY_PRT=""
    local TAG_RESTRICT_STRING=""
    local PAN_COMMAND=""
    version_date=`head -n 1 $LTPROOT/ChangeLog`

    while getopts c:d:f:ehi:l:m:Nno:pqr:s:t:vx: arg
    do  case $arg in
        c)       
	    NUM_PROCS=$(($OPTARG))
            $LTPROOT/testcases/bin/genload --cpu $NUM_PROCS >/dev/null 2>&1 &
            GENLOAD=1 ;;
                   
        d)  # append $$ to TMP, as it is recursively 
            # removed at end of script.
            TMPBASE=$OPTARG
            TMP="${TMPBASE}/ltp-$$"
            export TMPDIR="$TMP";;
        f)  # Execute user defined set of testcases.
            CMDFILE=$OPTARG;;
    
        h)  usage;;
        
        i)       
            BYTESIZE=$(($OPTARG * 1024 * 1024))
            $LTPROOT/testcases/bin/genload --io 1 >/dev/null 2>&1 &
            $LTPROOT/testcases/bin/genload --hdd 0 --hdd-bytes $BYTESIZE \
            >/dev/null 2>&1 & 
            GENLOAD=1 ;;
    
        l)      

            echo "INFO: creating $LTPROOT/results directory"
            [ ! -d $LTPROOT/results ] && \
            {
               mkdir -p $LTPROOT/results || \
               {
                   echo "ERROR: failed to create $LTPROOT/results"
                   exit 1
                }
            }
            case $OPTARG in
	    /*)
                LOGFILE="-l $OPTARG" ;;
            *)    
                LOGFILE="-l $LTPROOT/results/$OPTARG"
                ALT_DIR=1 ;;
            esac ;;
    
        m)      
            MEMSIZE=$(($OPTARG * 1024 * 1024)) 
            $LTPROOT/testcases/bin/genload  --vm 0 --vm-bytes $MEMSIZE \
                >/dev/null 2>&1 & 
            GENLOAD=1;;

        n)  
            $LTPROOT/testcases/bin/netpipe.sh
            NETPIPE=1;;
    
        o)  OUTPUTFILE="-o $OPTARG" ;;
    
        p)  PRETTY_PRT=" -p ";;
   
        q)  QUIET_MODE=" -q ";;
    
        r)  LTPROOT=$OPTARG;;
    
        s)  TAG_RESTRICT_STRING=$OPTARG;;
    
        t)  # In case you want to specify the time 
            # to run from the command line 
            # (2m = two minutes, 2h = two hours, etc)
            DURATION="-t $OPTARG" ;;
    
        v)  VERBOSE_MODE=1;;
   
        x)  # number of ltp's to run
            cat <<-EOF >&1
            WARNING: The use of -x can cause unpredictable failures, as a
                     result of concurrently running multiple tests designed
                     to be ran exclusively.
                     Pausing for 10 seconds..."
	EOF
            sleep 10 
            INSTANCES="-x $OPTARG -O ${TMP}";;
    
        \?) usage;;
        esac
    done
    
     mkdir -p $TMP || \
    {
        echo "FATAL: Unable to make temporary directory $TMP"
        exit 1
    }
    
    cd $TMP || \
    {
      echo "could not cd ${TMP} ... exiting"
      exit 1
    }
      
    if [ -z "$CMDFILE" ]
    then
    cat <<-EOF >&1

    INFO: no command files were provided, using default,
          system calls, memory management, IPC, scheduler
          direct io, file system, math and pty tests will 
          now be executed
    
	EOF
    	exit 1
	else
        [ -f $CMDFILE ] || \
                CMDFILE="$LTPROOT/runtest/$CMDFILE"

		if [ -z "$TAG_RESTRICT_STRING" ]
		then 
            cat $CMDFILE > ${TMP}/alltests || \
            {
                echo "FATAL: Unable to create command file"
                exit 1
            }
        else 
			grep $TAG_RESTRICT_STRING $CMDFILE > ${TMP}/alltests || \
			{
				echo "FATAL: Unable to create command file"
				exit 1
			}
		fi
    fi
    
     # check for required users and groups
    ${LTPROOT}/IDcheck.sh &>/dev/null || \
    {
        echo "WARNING: required users and groups not present"
        echo "WARNING: some test cases may fail"
    }
    
    [ ! -z "$QUIET_MODE" ] && { echo "INFO: Test start time: $(date)" ; }
    PAN_COMMAND="${LTPROOT}/pan/pan $QUIET_MODE -e -S $INSTANCES $DURATION -a $$ \
    -n $$ $PRETTY_PRT -f ${TMP}/alltests $LOGFILE $OUTPUTFILE"
    if [ ! -z "$VERBOSE_MODE" ] ; then
      echo "COMMAND:    $PAN_COMMAND"
      if [ ! -z "$TAG_RESTRICT_STRING" ] ; then
        echo "INFO: Restricted to $TAG_RESTRICT_STRING"
      fi
    fi
    #$PAN_COMMAND #Duplicated code here, because otherwise if we fail, only "PAN_COMMAND" gets output
    ${LTPROOT}/pan/pan $QUIET_MODE -e -S $INSTANCES $DURATION -a $$ \
    -n $$ $PRETTY_PRT -f ${TMP}/alltests $LOGFILE $OUTPUTFILE
    
    if [ $? -eq 0 ]; then
      echo "INFO: pan reported all tests PASS"
      VALUE=0
    else
      echo "INFO: pan reported some tests FAIL"
      VALUE=1
    fi
    
    [ ! -z "$QUIET_MODE" ] && { echo "INFO: Test end time: $(date)" ; }
    
    [ "$GENLOAD" -eq 1 ] && { killall -9 genload ; }
    [ "$NETPIPE" -eq 1 ] && { killall -9 NPtcp ; }
    
    [ "$ALT_DIR" -eq 1 ] && \
    {
    cat <<-EOF >&1
        
       ###############################################################"
        
            Done executing testcases."
            result log is in the $LTPROOT/results directory"
            LTP Version:  $version_date
       ###############################################################"
       
	EOF
    }
    exit $VALUE

}


cleanup()
{
    rm -rf ${TMP}
    umount /tmp
}

trap "cleanup" 0
setup || exit 1
main "$@"



#pan/pan -e -S -a 1822 -f /tmp/mx37_3stack_am_v4l2  -n 1822 -l - -p
