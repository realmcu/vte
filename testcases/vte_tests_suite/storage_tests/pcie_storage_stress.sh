#/bin/bash
#PCIe storage long time stress test used to test the stability of PCIe. 
#Case may run more than 12 hours as P4 case.
#


check_rc()
{
	if [ $1 -ne 0 ];then
		echo "Test Failed with return value $1"
		exit $1
	fi
}

RC=0
echo "Single 5G test"
iozone -a -i 1 -i 0 -U /mnt/msc -s 5g  -f /mnt/msc/iozone.test -b /tmp/iozone_usb3.0_result.xls -R
RC=$?
check_rc $RC
sleep 10
echo "2G test loop 10 times"
i=0
while [ $i -lt 10 ];do
	iozone -a -i 1 -i 0 -U /mnt/msc -s 2g  -f /mnt/msc/iozone.test -b /tmp/iozone_usb3.0_result.xls -R
	RC=$?
	check_rc $RC
	let i=i+1
	echo "2G test $i times"
	sleep 10
done
echo "PCIe storage stress test pass."
