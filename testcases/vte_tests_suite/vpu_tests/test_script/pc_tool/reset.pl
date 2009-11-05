#!/usr/bin/perl;

open FWRITE, ">>pdu.log";
$CurrTime = localtime(time);
print FWRITE "$CurrTime\n";
close FWRITE;

$pudclient = "pduclient.exe";
system("$pudclient 10.192.225.122 MX37-8 10002");
system("ping -n 15 127.0.0.1");
system("$pudclient 10.192.225.122 MX37-8 10001");

