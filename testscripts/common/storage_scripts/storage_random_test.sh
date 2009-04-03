#device: 
#       1--nand--/mnt/flc 
#	2--sd--/mnt/sd 
#	3--ata--/mnt/ata 
#	4--usbh--/mnt/msc
#size: total_size/10*$RANDOM
#times: random number of 1-10
#
device1="";
device2="";
size="";
times="";

average1=4;# 4 devices
average2=10;# 10 times

let device_number1=$RANDOM*$average1/32767+1;
echo device_number1=$device_number1;
	case $device_number1 in 
			"1") 
				device1=/mnt/flc;
				;;
			"2") 
				device1=/mnt/sd;
				;;
			"3") 
				device1=/mnt/ata;
				;;
			"4") 
				device1=/mnt/msc;
				;;

	esac
echo device1=$device1;

let device_number2=$RANDOM*$average1/32767+1;
echo device_number2=$device_number2;
	case $device_number2 in 
			"1") 
				device2=/mnt/flc;
				;;
			"2") 
				device2=/mnt/sd;
				;;
			"3") 
				device2=/mnt/ata;
				;;
			"4") 
				device2=/mnt/msc;
				;;

	esac
echo device2=$device2;

total_size1=`df | grep $device1 | awk '{print $4}'`;
echo total_size1=$total_size1;
total_size2=`df | grep $device2 | awk '{print $4}'`;
echo total_size2=$total_size2;

let size2=$RANDOM*$total_size2/$average2/32767;
echo size2=$size2;

if [ $total_size1 -ge $size2 ]; then
	size=$size2;
else
	size=$total_size1;
fi

let sizeM=$size/1024;
echo sizeM=$sizeM;


let times=$RANDOM*$average2/32767+1;
echo times=$times;

./storage_test.sh CD $device1 $device2 $sizeM $times

