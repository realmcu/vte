#Copyright 2007-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
#!/bin/sh
#File :       usb_gadget_testapp.sh
#
#Description:  Belcarra Gadget USB test
#
#=====================================================================================
#Revision History:
#                            Modification     Tracking
# Author                          Date          Number    Description of Changes
#-------------------------   ------------    -----------  -----------------------------
#A.Ozerov/b00320              18/07/2007      ENGR43014   Initial version

tst_resm()
{
        state=$1
        shift
        echo "`basename $0` $state: $@"
}

# Function:     setup
#
# Description:  - Check if required commands exits
#               - Export global variables
#               - Check if required config files exits
#
# Return        - none
setup()
{
        # Total number of test cases in this file.
        export  TST_TOTAL=1

        #The TCID and TST_COUNT variables are required by the LTP
        #command line harness APIs, these variables are not local to this program.

        # Test case identifier
        export  TCID="usb_testapp"

        # Set up is initialized as test 0
        export  TST_COUNT=0

        # Initialize cleanup function to execute on program exit.
        # This function will be called before the test program exits.
        trap "cleanup" 1
}

# Function:     cleanup
#
# Description:  - porforms
#
# Return        - returns 0, if everything is Ok
#               - returns error code, if something wrong happened
cleanup()
{
	RC=0
        return $RC
}

test_belcarra_gadget()
{
	#Network function test
	ifconfig | grep usbl0 > /dev/null
	RC=$?
	if [ $RC -ne 0 ]
	then
		tst_resm TFAIL "there is no such interface usbl0!"
		return $RC
	else
		tst_resm TPASS "interface usbl0 exists."
        	ifconfig usbl0 10.10.0.1
		echo
		tst_resm TINFO "please, connect the USB OTG cable to OTG transceiver of EVB and PC."
		echo
		ping 10.10.0.2 -c 4 -s 1016
		RC=$?
        	if [ $RC -ne 0 ]
        	then
			echo
			tst_resm TFAIL "packets(size 1KB) were lost!"
			return $RC
        	else
			echo
			tst_resm TPASS "all of packets(size 1KB) were delivered to host."
        	fi
		echo
		ping 10.10.0.2 -c 4 -s 4088
		RC=$?
        	if [ $RC -ne 0 ]
        	then
			echo
			tst_resm TFAIL "packets(size 4KB) were lost!"
			return $RC
        	else
			echo
			tst_resm TPASS "all of packets(size 4KB) were delivered to host."
        	fi
        	echo
		ping 10.10.0.2 -c 4 -s 8184
		RC=$?
        	if [ $RC -ne 0 ]
        	then
        		echo
			tst_resm TFAIL "packets(size 8KB) were lost!"
			return $RC
        	else
	        	echo
			tst_resm TPASS "all of packets(size 8KB) were delivered to host."
        	fi
	fi

	#ACM function test
	echo
	tst_resm TINFO "please, connect the USB OTG cable to OTG transceiver of EVB and PC."
	getty -L ACM0 115200 vt100
	echo
	tst_resm TINFO "open hyperterminal at PC side with COM port created by the USB to serial class driver."
	tst_resm TINFO "do you see the virtual console on hyperterminal(yes/no)?"
	read U_ANSWER
	if [ $ANSWER -eq "yes" ]
	then
		echo
		tst_resm TPASS "ACM function test passed."
	else
		echo
		tst_resm TFAIL "ACM function test failed."
		return 1
	fi

	#Mass Storage function test
	dd if=/dev/zero of=/root/sdloop bs=1024k count=16
	losetup /dev/loop/0 /root/sdloop
	./msc_admin msc_mount 7 0
	RC=$?
        if [ $RC -ne 0 ];
        then
		tst_resm TFAIL "unable to mount MSC device!"
		return $RC
        fi
	echo
	tst_resm TINFO "please, connect the USB OTG cable to OTG transceiver of EVB and PC."
	./msc_admin msc_connect
	RC=$?
        if [ $RC -ne 0 ];
        then
		tst_resm TFAIL "unable to connect to MSC device!"
		return $RC
        fi
	echo
	./msc_admin msc_status
	echo
	tst_resm TINFO "creating temporary file on mounted MSC device..."
	cat  > /root/sdloop/temp_file << EOF
test
EOF
        RC=$?
        if [ $RC -ne 0 ];
        then
		tst_resm TFAIL "file wasn't created!"
		return $RC
        fi
        tst_resm TINFO "file was created"

	tst_resm TINFO "writing something into the file..."
        echo "TestFILE" > /root/sdloop/temp_file
        RC=$?
        if [ $RC -ne 0 ];
        then
		tst_resm TFAIL "unable to write into file!"
		return $RC
        fi
        tst_resm TINFO "something has written into the file"
        echo

        tst_resm TINFO "deleting the temporary file..."
        rm ~/mnt/temp_file
        RC=$?
        if [ $RC -ne 0 ];
        then
		tst_resm TFAIL "unable to delete file!"
		return $RC
        fi
        tst_resm TINFO "file was deleted"
	echo
	./msc_admin msc_disconnect
	RC=$?
        if [ $RC -ne 0 ];
        then
		tst_resm TFAIL "unable to disconnect!"
		return $RC
        fi
	echo
	./msc_admin msc_umount
	RC=$?
        if [ $RC -ne 0 ];
        then
		tst_resm TFAIL "unable to unmount MSC device!"
		return $RC
        fi

	return $RC
}

RC=0

setup
RC=$?
if [ $RC -ne 0 ];
then
        tst_resm TFAIL "function setup failed"
        exit $RC
else
        test_belcarra_gadget
        RC=$?
        if [ $RC -eq 0 ];
        then
                tst_resm TPASS "This test case works as expected"
                exit $RC
        else
                tst_resm TFAIL "This test case does NOT work as expected"
                exit $RC
        fi

        cleanup
        RC=$?
        if [ $RC -ne 0 ];
        then
                tst_resm TFAIL "function cleanup failed"
                exit $RC
        fi
fi

exit $RC
