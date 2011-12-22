#!/bin/sh
##############################################################################
#Copyright 2008-2011 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
##############################################################################
#
# Revision History:
#                      Modification     Tracking
# Author                   Date          Number    Description of Changes
#-------------------   ------------    ----------  ---------------------
# Spring Zhang/B17931   27/11/2008       n/a        Initial ver. 
# Spring                28/11/2008       n/a        Modify COPYRIGHT header
# Spring                15/01/2008       n/a        Add MX35TO2 judgement,
#                                                   move to tools/
# Spring                02/04/2009       n/a        Add MX51Babbage support
# Spring                02/08/2009       n/a        Use own determination
# Spring                11/03/2009       n/a        Add MX28EVK support
# Spring                18/03/2009       n/a        Add MX53EVK support
# Hake                  01/10/2011       n/a        Add LOCO and SMD 53. etc
# Hake                  03/23/2011       n/a        Add MX50 rdp3 etc
# Spring                07/04/2011       n/a        Add MX53SMD TO2.1 support
# Spring                11/22/2011       n/a        Add MX6Q description
#############################################################################
# Usage1(return string):
#   platform=`platfm.sh`
#   Then use $platform to judge the platform
#
#   $platform   PLATFORM
#   IMX31ADS    IMX31ADS
#   IMX32ADS    IMX32ADS
#   IMX25-3STACK IMX25-3STACK
#   IMX31-3STACK IMX31-3STACK
#   IMX35-3STACK IMX35-3STACK
#   IMX37-3STACK IMX37-3STACK
#   IMX51-3STACK IMX51-3STACK
#   IMX51-BABBAGE IMX51-BABBAGE
#   IMX28EVK    IMX28EVK
#   IMX53EVK    IMX53EVK
#   IMX53LOCO   IMX53LOCO
#   IMX53SMD    IMX53SMD
#   IMX50ARM2   IMX50ARM2
#   IMX50RDP    IMX50RDP
#   IMX50-RDP3    IMX50-RDP3
#   IMX6-SABREAUTO IMX6-SABREAUTO
#   IMX6-SABRELITE IMX6-SABRELITE
#
# Usage2(return number): 
#   platfm.sh || platform=$?
#   Then use $platform to judge the platform
#
# Return value:
# 1. 31~53 for mx31~mx53 board.
#     rt value  Board
# e.g.  31       mx31
#       35       mx35
#       41       mx51 babbage
#       51       mx51 3ds
#       28       mx28 evk
#       53       mx53 evk loco smd
#       61       mx6q arm2 and sabre-lite
#
# 2. 378%256(=122) for SMTP378X board.(for return value is 0~255)
#       rt value    Board
# e.g.  378%256   SMTP378X
#
# 3. 67 for "Platform not recognized".(67 can be defined to other num)

# Some Platform Info
#35 TO2 platform:
#Revision        : 35120

# Find the platform type
determine_platform()
{
    local find=0
    
    # Determine the platform
    find=`cat /proc/cpuinfo | grep "Revision" | grep " 31.*" | wc -l`;
    if [ $find -eq 1 ]
    then
        p=IMX31-3STACK
    fi

    find=`cat /proc/cpuinfo | grep "Revision" | grep " 35.*" | wc -l`;
    if [ $find -eq 1 ]
    then
        p=IMX35-3STACK
    fi

    find=`cat /proc/cpuinfo | grep "Revision" | grep " 37.*" | wc -l`;
    if [ $find -eq 1 ]
    then
        p=IMX37-3STACK
    fi

    # MX51 TO2.0: Revision: 51020
    find=`cat /proc/cpuinfo | grep "Revision" | grep " 51.*" | wc -l`;
    if [ $find -eq 1 ]
    then
        p=IMX51-3STACK
    fi

    # MX51 Babbage TO1.1: Revision: 51011
    # MX51 Babbage TO3.0: Revision: 51130
    find=`cat /proc/cpuinfo | grep "Hardware" | grep "Babbage" | wc -l`;
    if [ $find -eq 1 ]
    then
        p=IMX51-BABBAGE
    fi

    # MX53 EVK TO1.0: Revision: 53010
    find=`cat /proc/cpuinfo | grep "Revision" | grep "53.10" | wc -l`;
    if [ $find -eq 1 ]
    then
        p=IMX53EVK
		find=`cat /proc/cpuinfo | grep "Hardware" | grep "MX53 SMD" | wc -l`;
       if [ $find -eq 1 ]; then
			 		p=IMX53SMD
				fi
    fi
    
	# MX53 LOCO TO2.0: Revision: 53020
    find=`cat /proc/cpuinfo | grep "Revision" | grep "53.20" | wc -l`;
    if [ $find -eq 1 ]
    then
	    find=`cat /proc/cpuinfo | grep "Hardware" | grep "MX53 LOCO" | wc -l`;
        if [ $find -eq 1 ]; then
            p=IMX53LOCO
        fi
		find=`cat /proc/cpuinfo | grep "Hardware" | grep "MX53 SMD" | wc -l`;
        if [ $find -eq 1 ]; then
            p=IMX53SMD
        fi
    fi

   	# MX53 LOCO TO2.1: Revision: 53321
    find=`cat /proc/cpuinfo | grep "Revision" | grep "53.21" | wc -l`;
    if [ $find -eq 1 ]
    then
		find=`cat /proc/cpuinfo | grep "Hardware" | grep "MX53 LOCO" | wc -l`;
        if [ $find -eq 1 ]; then
	        p=IMX53LOCO
        fi
		find=`cat /proc/cpuinfo | grep "Hardware" | grep "MX53 SMD" | wc -l`;
        if [ $find -eq 1 ]; then
            p=IMX53SMD
        fi
    fi

    #find STMP378X
    find=`cat /proc/cpuinfo | grep "Hardware" | grep " 378X" | wc -l`;
    if [ $find -eq 1 ]
    then
        p=SMTP378X
    fi

    find=`cat /proc/cpuinfo | grep "Hardware" | grep " 23.*" | wc -l`;
    if [ $find -eq 1 ]
    then
        p=IMX23EVK
    fi
    #find MX28EVK
    find=`cat /proc/cpuinfo | grep "Hardware" | grep "MX28EVK" | wc -l`;
    if [ $find -eq 1 ]
    then
        p=IMX28EVK
    fi

    #find MX50ARM2
    find=`cat /proc/cpuinfo | grep "Hardware" | grep "MX50 ARM2" | wc -l`;
    if [ $find -eq 1 ]
    then
        p=IMX50ARM2
    fi
    
		#find MX50RDP
    find=`cat /proc/cpuinfo | grep "Hardware" | grep "MX50 Reference Design" | wc -l`;
    if [ $find -eq 1 ]
    then
    	find=`cat /proc/cpuinfo | grep "Revision" | grep "50311" | wc -l`;
        if [ $find -eq 1 ]
        then
            p=IMX50-RDP3
        else
            p=IMX50RDP
        fi
    fi
		
    find=`cat /proc/cpuinfo | grep "Hardware" | grep "6Quad" | grep "Armadillo2 Board" | wc -l`;
    if [ $find -eq 1 ]
    then
        find=`cat /proc/cpuinfo | grep "Revision" | grep "63" | wc -l`;
        p=IMX6ARM2
    fi
	
    find=`cat /proc/cpuinfo | grep "Hardware" | grep "6Quad" | grep "Sabre Auto" | wc -l`;
    if [ $find -eq 1 ]
    then
        find=`cat /proc/cpuinfo | grep "Revision" | grep "63" | wc -l`;
        p=IMX6-SABREAUTO
    fi

	find=`cat /proc/cpuinfo | grep "Hardware" | grep "6Quad" | grep "Sabre-Lite" | wc -l`;
    if [ $find -eq 1 ]
    then
        find=`cat /proc/cpuinfo | grep "Revision" | grep "63" | wc -l`;
        p=IMX6-SABRELITE
    fi


    if [ $p = "IMX31-3STACK" ]
    then
        #echo "Platform MX31"
        RC=31
    elif [ $p = "IMX35-3STACK" ]
    then
        #echo  "Platform MX35" 
        RC=35
    elif [ $p = "IMX37-3STACK" ]
    then
        #echo  "Platform MX37" 
        RC=37
		elif [ $p = "IMX23EVK" ]
		then
		    RC=23
    elif [ $p = "IMX51-3STACK" ]
    then
        #echo  "Platform MX51" 
        RC=51
    elif [ $p = "IMX51-BABBAGE" ]
    then
        #echo  "Platform MX51 Babbage" 
        RC=41
    elif [ $p = "IMX53EVK" ] || [ $p = "IMX53LOCO" ] || [ $p = "IMX53SMD" ]
    then
        #echo  "Platform MX53 EVK" 
        RC=53
     elif [ $p = "SMTP378X" ]
    then
        #echo  "Platform SMTP378X" 
        let RC=378%256
    elif [ $p = "IMX28EVK" ]
    then
        #echo  "Platform MX28 EVK" 
        RC=28
    elif [ $p = "IMX50ARM2" ]
    then
        RC=50
    elif [ $p = "IMX50RDP" ] || [ $p = "IMX50-RDP3" ]
    then
        RC=50
    elif [ $p = "IMX6-SABREAUTO" ] || [ $p = "IMX6-SABRELITE" ] || [ $p = "IMX6ARM2"  ]
    then
        RC=61
    else
        #echo  "Platform not recognized!"
        RC=67
    fi

    return $RC
}


# main
RC=0
determine_platform
echo "$p"

exit $RC

#############################
allcpu_info()
{
    cat << EOF
MX28EVK TO1.0 - 201011
root@localhost /staf/retail$ cat /proc/cpuinfo
Processor       : ARM926EJ-S rev 5 (v5l)
BogoMIPS        : 478.41
Features        : swp half thumb fastmult edsp java
CPU implementer : 0x41
CPU architecture: 5TEJ
CPU variant     : 0x0
CPU part        : 0x926
CPU revision    : 5

Hardware        : Freescale MX28EVK board
Revision        : 0000
Serial          : 0000000000000000

MX51 Babbage TO3.0 - 201011
root@freescale ~$ cat /proc/cpuinfo
Processor       : ARMv7 Processor rev 5 (v7l)
BogoMIPS        : 799.53
Features        : swp half thumb fastmult vfp edsp neon vfpv3
CPU implementer : 0x41
CPU architecture: 7
CPU variant     : 0x2
CPU part        : 0xc08
CPU revision    : 5

Hardware        : Freescale MX51 Babbage Board
Revision        : 51130
Serial          : 0000000000000000


MX53 EVK TO1.0 - 201012
Processor       : ARMv7 Processor rev 5 (v7l)
BogoMIPS        : 799.53
Features        : swp half thumb fastmult vfp edsp neon vfpv3
CPU implementer : 0x41
CPU architecture: 7
CPU variant     : 0x2
CPU part        : 0xc08
CPU revision    : 5

Hardware        : Freescale MX53 EVK Board
Revision        : 53010
Serial          : 0000000000000000


MX53 SMD TO2.1 - 201104
Processor       : ARMv7 Processor rev 5 (v7l)
BogoMIPS        : 999.42
Features        : swp half thumb fastmult vfp edsp neon vfpv3
CPU implementer : 0x41
CPU architecture: 7
CPU variant     : 0x2
CPU part        : 0xc08
CPU revision    : 5

Hardware        : Freescale MX53 SMD Board
Revision        : 53321
Serial          : 0000000000000000


MX53 LOCO Ripley Rev.A - 201108
Processor       : ARMv7 Processor rev 5 (v7l)
BogoMIPS        : 399.76
Features        : swp half thumb fastmult vfp edsp neon vfpv3
CPU implementer : 0x41
CPU architecture: 7
CPU variant     : 0x2
CPU part        : 0xc08
CPU revision    : 5

Hardware        : Freescale MX53 LOCO Board
Revision        : 53121
Serial          : 0000000000000000

MX6Q ARM2 board -201111
Processor       : ARMv7 Processor rev 10 (v7l)
processor       : 0
BogoMIPS        : 1985.74

processor       : 1
BogoMIPS        : 1992.29

processor       : 2
BogoMIPS        : 1992.29

processor       : 3
BogoMIPS        : 1992.29

Features        : swp half thumb fastmult vfp edsp neon vfpv3
CPU implementer : 0x41
CPU architecture: 7
CPU variant     : 0x2
CPU part        : 0xc09
CPU revision    : 10

Hardware        : Freescale i.MX 6Quad Armadillo2 Board
Revision        : 63000
Serial          : 0000000000000000

EOF
}

