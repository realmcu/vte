#!/usr/bin/perl;

#/*================================================================================================= 

#    Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved 
#    THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT 
#    BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF 
#    Freescale Semiconductor, Inc. 
#
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
