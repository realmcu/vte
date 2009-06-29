#Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
#!/bin/sh

platform=$(cat /proc/cpuinfo | grep "Hardware" | cut -d' ' -f3-4)
user_def=0
test_result=0
result_file=uart_sample.txt
log_file=uart_log.txt
pid_file_1=pid_1
pid_file_2=pid_2
pid_file_3=pid_3
pid_file_diff=pid_4
touch $result_file
touch $log_file
touch $pid_file_1
touch $pid_file_2
touch $pid_file_3
touch $pid_file_diff

tst_resm ()
{
echo $1 $2
}

usage ()
{
tst_resm ""
tst_resm "Command line arguments"
tst_resm ""
tst_resm ""
tst_resm "-f Argument"
tst_resm ""
tst_resm "-f argument for FILE NAME. The \"file_name\" is used to trasfer across the UART ports."
tst_resm "so use more than 1MB size file"
tst_resm ""
tst_resm "USAGE"
tst_resm "$0 -f file_name "
tst_resm ""
tst_resm ""
tst_resm "-c Argument"
tst_resm ""
tst_resm "-c argument for COUNT. The Count will do number of iterations. This is an optional"
tst_resm "argument. By default it will be set to 10"
tst_resm ""
tst_resm "USAGE"
tst_resm "$0 -f file_name -c count "
tst_resm ""
tst_resm ""
tst_resm "-h Argument"
tst_resm ""
tst_resm "-h argument for HELP"
tst_resm ""
tst_resm "USAGE"
tst_resm "$0 -h"
tst_resm ""
tst_resm ""
tst_resm "-d Argument"
tst_resm ""
tst_resm "-d argument for DEBUG Messages"
tst_resm ""
tst_resm "USAGE"
tst_resm "$0 -f file_name -c count -d "
tst_resm "$0 -f file_name -d "
tst_resm ""
tst_resm ""
exit
}

if [ $# -eq 0 ]
then
	usage
fi


dev_entry_check ()
{
if [ ! -b /dev/mmcblk0p5 ]
then
	tst_resm WARN "/dev/mmcblk0p5 Not present
		 MMC/SD card may not be inserted or
		 the MMC/SD card partition may not be present"
	tst_resm ""
	tst_resm WARN "BASICALLY THE SCRIPT IS LOOKING FOR \"/dev/mmcblk0p5\" "
	tst_resm ""
	exit
fi
if [ ! -c /dev/ttymxc0 ] & [ ! -c /dev/ttymxc2 ]
then
	tst_resm WARN "/dev/ttymxc0 OR /dev/ttymxc2 Does not exists"
	exit
fi
}

baudrate_setup ()
{
stty 460800 raw -onlcr -echo -echoe < /dev/ttymxc2
stty 460800 raw -onlcr -echo -echoe < /dev/ttymxc0
}

uart_rx_setup ()
{
ps -o pid= > $pid_file_1
if [ "$platform" == "MXC300-30 EVB" ]
then
	cat < /dev/ttymxc0 > /dev/null &
else
	cat < /dev/ttymxc2 > /dev/null &
fi
}

stop ()
{
ps -o pid= > $pid_file_2
#Compare the initial PID file with the Final PID file to get the new PID
#which are created after running this script
ret=0
for i in $(cat $pid_file_2)
do
    for j in $(cat $pid_file_1)
    do
        if [ $i -eq $j ]
        then
            ret=1
        fi
    done
    if [ $ret -eq 0 ]
    then
        echo $i >> $pid_file_diff
    fi
    ret=0
done

ps -o pid= > $pid_file_3
for i in $(cat $pid_file_diff)
do
    for j in $(cat $pid_file_3)
    do
		if [ $i -eq $j ]
		then
			kill $i
		fi
	done
done

rm $pid_file_1
rm $pid_file_2
rm $pid_file_3
rm $pid_file_diff
rm $result_file
tst_resm ""
tst_resm ""
if [ $test_result -eq 1 ]
then
	tst_resm PASS "TEST PASS"
else
	tst_resm FAIL "TEST FAIL"
fi
tst_resm ""
tst_resm ""
exit
}

uart_tx_setup ()
{
if [ "$platform" == "MXC300-30 EVB" ]
then
	(while [ ! -f /tmp/stop ]; do cat $file > /dev/ttymxc2; done; echo "finished cat") &
else
	(while [ ! -f /tmp/stop ]; do cat $file > /dev/ttymxc0; done; echo "finished cat") &
fi
}

mmc_setup ()
{
tst_resm INFO "Mount MMC"
mkdir -p /mnt/mmc
mount -t vfat /dev/mmcblk0p5 /mnt/mmc
#rm -rf /mnt/mmc/$file 
if [ $run -eq 1 ]
then
	tst_resm INFO "Copy file $1 to /mnt/mmc"
	cp -rf $file /mnt/mmc
	cp -rf /mnt/mmc/$file /mnt/mmc/tmp_$file
fi
tst_resm INFO "Compare two files "
cmp /mnt/mmc/$file /mnt/mmc/tmp_$file
#rm -rf /mnt/mmc/tmp_$file
tst_resm INFO "Umount MMC"
umount /mnt/mmc
}


test_counters ()
{
run=1
stop_bit=0
tst_resm INFO "UART is setup"
echo "$0 test Log" > $log_file
while [ $run -lt $count ]
do
	cat /proc/tty/driver/ttymxc > $result_file
	cat $result_file >> $log_file
	if [ $debug -eq 1 ]
	then 
		tst_resm ""
		tst_resm DEBUG 
		cat $result_file 
		tst_resm ""
	fi
	if [ "$platform" == "MXC300-30 EVB" ]
	then
		tx_cnt=$(cat $result_file | tail -n 1 | cut -d':' -f6 | cut -d' ' -f1)
		rx_cnt=$(cat $result_file | head -n 2 | tail -n 1 | cut -d':' -f7 | cut -d' ' -f1)
	else
		tx_cnt=$(cat $result_file | head -n 2 | tail -n 1 | cut -d':' -f6 | cut -d' ' -f1)
		rx_cnt=$(cat $result_file | tail -n 1 | cut -d':' -f7 | cut -d' ' -f1)
	fi
	tst_resm ""
	tst_resm INFO "Sample $run : TX count is $tx_cnt"
	tst_resm "RX count is $rx_cnt"
	tst_resm ""
	if [ $run -gt 1 ]
	then
		if [ $rx_cnt -eq 0 ] 
		then
			tst_resm ""
	    	tst_resm FAIL "Rx count is equals to zero" 
			tst_resm ""
			stop
		fi
		if [ $tx_cnt -eq 0 ]
		then
			tst_resm ""
    		tst_resm FAIL "Tx count is equals to zero" 
			tst_resm ""
			stop
		fi
		if [ $tx_cnt -eq $tx_cnt_old ]  
		then
			tst_resm ""
			tst_resm FAIL "Tx count is not incrementing "
			tst_resm FAIL "TX count_1 = $tx_cnt 		count_2 = $tx_cnt_old"
			tst_resm ""
			stop
		fi
		if [ $rx_cnt -eq $rx_cnt_old ] 
		then
			tst_resm ""
			tst_resm FAIL "Rx count is not incrementing "
			tst_resm FAIL "RX count_1 = $rx_cnt 		count_2 = $rx_cnt_old" 
			tst_resm ""
			stop
		fi
	fi
	tx_cnt_old=$tx_cnt
	rx_cnt_old=$rx_cnt
	mmc_setup
	run=$((run+1))
done
}

main ()
{
if [ $user_def -eq 0 ]
then
	count=10
else
	count=$cnt_arg
fi	
if [ $count -lt 0 ]
then
	count=10
fi
dev_entry_check
baudrate_setup
uart_rx_setup
uart_tx_setup
test_counters
test_result=1
stop
}


while getopts hc:f:d param
do
case "$param" in 
	h)
		tst_resm INFO "Help"
		usage
		;;
	c) 
		user_def=1
		cnt_arg=$OPTARG		
		tst_resm INFO "Count is set to $cnt_arg"
		;;
	f)
		file=$OPTARG
		;;
	d)
		debug=1
		;;
	\?) 
		tst_info INFO "Wrong Parameters"
		;;
esac
done
main
