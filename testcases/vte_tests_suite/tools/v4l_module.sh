#!/bin/sh -x
# will check the CAMERA env as the deafult camera
# possbile CAMERA are ov3640, ov5640, ov5642

camera_module=ov3640_camera

check_platform_camera()
{
 #only check i2c camera
 names=$(find /sys/devices/platform/imx-i2c.*/ -name name | xargs grep ov)
 cnt=$(echo $names | wc -l)
 #if there are multi camera support then choice the given one or last one
 if [ $cnt -gt 0 ]; then
	 for i in $names ; do
		 camera=$(echo $i | cut -d ':' -f 2)
		 if [ ! -z "$CAMERA"  ];then
		   if [ "$CAMERA" = $camera ]; then
		     camera_module=${camera}_camera
			 break
		   else
             camera_module=${camera}_camera
		   fi
		 else
             camera_module=${camera}_camera
		 fi
	 done
 else
	 return 1
 fi
 return 0
}


v4l_setup()
{
   if [ -z "$camera_module" ]; then
     echo "no camera module supported"
	 return 1
   fi
   #turn on the display
   echo 0 > /sys/class/graphics/fb0/blank
   #keep cursor on
   echo -e "\033[9;0]" > /dev/tty0
   modprobe $camera_module
   sleep 2
   modprobe mxc_v4l2_capture || return 1
   sleep 5
   retry=5
   while [ ! -e /dev/video0 ]
	   do
      sleep 5
	  if [ $retry -eq 0 ]; then
          return 1;
	  fi
	  retry=$(expr $retry - 1)
   done
   return 0
}

v4l_cleanup()
{
   modprobe -r $camera_module || return 1
   sleep 2
   modprobe -r mxc_v4l2_capture || return 1
}


#main
RC=0
check_platform_camera || RC=1

if [ $RC -ne 0 ]; then
  echo "can not find support ovCameras for this platform"
  exit 1
fi

case "$1" in
'setup') :
  v4l_setup || RC=2
  ;;
'cleanup') :
  v4l_cleanup || RC=3
   ;;
esac

if [ $RC  -ne 0  ];then
  echo "Fail to $1 v4l"
  exit $RC
else
  echo "OK to $1 v4l"
  exit $RC 
fi
