#!/bin/sh -x
# will check the CAMERA env as the deafult camera
# possbile CAMERA are ov3640, ov5640, ov5642

camera_module=

check_platform_camera()
{
 #only check i2c camera
 find=0
 bus_if=$(ls -d /sys/devices/platform/imx-i2c.*)
 #if there are multi camera support then choice the given one or last one
 for j in $bus_if; do
	names=$(find $j -name name | xargs grep ov)
	for i in $names ; do
		apd=
		camera_all=$(echo $i | cut -d ':' -f 2)
		camera=$(echo $camera_all | cut -d '_' -f 1)
		appends=$(echo $camera_all | grep '_' | cut -d '_' -f 2)
		if [ "$appends" ]; then
			apd=_${appends}
		fi
		if [ $find -eq 1  ]; then
			break
		fi
		if [ ! -z "$CAMERA"  ];then
			if [ "$CAMERA" = ${camera}${apd} ]; then
				camera_module=${camera}_camera${apd}
				find=1
			else
				camera_module=
			fi
		else
			modprobe -r ${camera}_camera${apd}
			sleep 1
			camera_module=$(echo $camera_module  ${camera}_camera${apd})
		fi
	done
 done

 if [ -z $camera_module ]; then
	return 1
 else
 	return 0
 fi
}

v4l_setup()
{
	check_platform_camera || RC=1

	if [ $RC -ne 0 ]; then
  		echo "can not find support ovCameras for this platform"
  		exit 1
	fi

   if [ -z "$camera_module" ]; then
     echo "no camera module supported"
	 return 1
   fi
   #turn on the display
   echo 0 > /sys/class/graphics/fb0/blank
   sleep 1
   #keep cursor on
   echo -e "\033[9;0]" > /dev/tty0
   for cm in $camera_module; do
   	modprobe $cm
   done
   sleep 1
   modprobe mxc_v4l2_capture || return 1
   sleep 1
   retry=5
   while [ ! -e /dev/video0 ]; do
	   sleep 1
	   if [ $retry -eq 0 ]; then
		   return 1;
		fi
	retry=$(expr $retry - 1)
   done
   return 0
}

v4l_cleanup()
{
	CAMERA=

	check_platform_camera || RC=1

	if [ $RC -ne 0 ]; then
  		echo "can not find support ovCameras for this platform"
  		exit 1
	fi
   for cm in $camera_module; do
   modprobe -r $cm
   done
   sleep 2
   modprobe -r mxc_v4l2_capture || return 1
}


#main
RC=0

echo $CAMERA

if [ -z $CAMERA ]; then
	if [ "$DUAL" == 2  ]  ; then
  		echo "define default"
  		CAMERA=ov5640
	elif [ "$DUAL" == 3 ] ; then
		echo "will probe all registed camera"
	else
		pt=$(platfm.sh)
		case "$pt" in
		'IMX6DL-Sabre-SD'):
  		CAMERA=ov5642
		;;
		'IMX6Q-Sabre-SD') :
  		CAMERA=ov5642
		;;
		'*') :
  		CAMERA=ov5640
		;;
		esac
	fi
fi



case "$1" in
'setup') :
  v4l_setup || RC=2
  ;;
'cleanup') :
  v4l_cleanup || RC=3
   ;;
'*') :
   ;;
esac

if [ $RC  -ne 0  ];then
  echo "Fail to $1 v4l"
  exit $RC
else
  echo "OK to $1 v4l"
  if [ "$( echo $camera_module | grep ov5642_camera)" ] && [ $1 == setup ]; then
	  #special for ov5642 which only support some modes in 6q Lite platform
	  return 56
  else
  exit $RC 
  fi
fi
