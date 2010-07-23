#!/usr/bin/perl;
#Copyright (C) 2005-2010 Freescale Semiconductor, Inc. All Rights Reserved.
##
##The code contained herein is licensed under the GNU General Public
##License. You may obtain a copy of the GNU General Public License
##Version 2 or later at the following locations:
##http://www.opensource.org/licenses/gpl-license.html
##http://www.gnu.org/copyleft/gpl.html

open FWRITE, ">>pdu.log";
$CurrTime = localtime(time);
print FWRITE "$CurrTime\n";
close FWRITE;

$pudclient = "pduclient.exe";
system("$pudclient 10.192.225.122 MX37-8 10002");
system("ping -n 15 127.0.0.1");
system("$pudclient 10.192.225.122 MX37-8 10001");

