#Copyright (C) 2008-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
#!/usr/bin/perl;
#====================================================================================================
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#Victor Cui                   05/26/2009       n/a        initialization the test application
#====================================================================================================
#Portability:  perl for windows
#==================================================================================================*/
#
#/*==================================================================================================
#Total Tests:           1
#Test Executable Name:  usbd_testall.pl
#Test Strategy:         
#=================================================================================================*/

print " -------------------------------Begin----------------------------------\n";

system("perl usbd_test.pl E G: G: 100 3");
system("perl usbd_test.pl E H: H: 100 3");
system("perl usbd_test.pl E G: D: 100 3");
system("perl usbd_test.pl E D: G: 100 3");
system("perl usbd_test.pl E H: D: 100 3");
system("perl usbd_test.pl E D: H: 100 3");
system("perl usbd_test.pl E G: H: 100 3");
system("perl usbd_test.pl E H: G: 100 3");


print " --------------------------------End-----------------------------------\n";
