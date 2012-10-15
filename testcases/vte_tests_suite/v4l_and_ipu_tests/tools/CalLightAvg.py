#!/usr/bin/env python
##############################################################################
#Copyright (C) 2012 Freescale Semiconductor, Inc.
#All Rights Reserved.
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
#                      Modification
# Author                   Date       Description of Changes
#-------------------   ------------   ---------------------
# Shelly Cheng            10/10/2012    Initial version
#############################################################################
import sys,os
import Image
import colorsys
import ImageEnhance
import numpy

if len(sys.argv) > 1:
    checkdir=sys.argv[1]
else:
    checkdir=os.getcwd()
brightValue = 0
reset = 0
print checkdir
for parent,dirnames,filenames in os.walk(checkdir):
    for filename in filenames:
            fName = filename
            sufix = os.path.splitext(fName)[1][1:]
            print sufix
            if sufix == "bmp" :
                filename = checkdir + os.sep + filename
            else :
                continue
            print filename
            pix = 0
            brightValue = 0
            reset = 0
            try:
                img = Image.open(filename)
            except:
                print ('Error to open file: ') + str(filename)
            img.load()
            if img.mode == 'RGB':
                red,green,bule = img.split()
            else : 
                print ('only support RGB mode');
                continue
            redArr = numpy.asarray(red)
            greenArr = numpy.asarray(green)
            buleArr = numpy.asarray(bule)
            print img.size
            #for i in (0,img.size[0]-1):
            i = 0
            j = 0
            while i < (img.size[0]):
               # print (".....i .....: ") + str(i)
                #for j in [0,img.size[1]-1]:
                while j < img.size[1] :
                       # print j
                        item = colorsys.rgb_to_hls(redArr[j][i]/255.0,greenArr[j][i]/255.0,buleArr[j][i]/255.0)
                        brightValue += item[1]
                        pix += 1
                        j += 1
                i += 1
                j = 0
            print (".....brightValue is .....: ") + str(brightValue)
            print (".....pix is ...: ") + str(pix)
            reset = brightValue/pix
            print ("....the average bright value is...: ") + str(reset)
