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
#TINA ZHAO                   05/31/2007       n/a        initialization of storage copy test application
#====================================================================================================
#Portability:  perl for windows
#==================================================================================================*/
#
#/*==================================================================================================
#Total Tests:           1
#Test Executable Name:  usb_copy.pl
#Test Strategy:         
#=================================================================================================*/


# STR1: read from; STR2: write to; STR3: file name; STR4: copy times 
# Use command "usb_copy.pl [STR1] [STR2] [SRT3] [STR4]"to test storage copy


$RC=0;
$TMP_RC=0;
$TMP_TIME=0;

sub anal_copy_res{
	if($TMP_RC==0) {
		print " -----------------------------------------------------------------\n";
		print " copy $FILE from $S_PATH to $D_PATH, the ${TMP_TIME}th pass!!!";
		print " -----------------------------------------------------------------\n" ;
	}else {
		$RC=1;
		print " -----------------------------------------------------------------\n";
		print " The ${TMP_TIME}th step copy fail\n";
		print " -----------------------------------------------------------------\n";
	}
}
 
sub anal_delete_res{
	if ($TMP_RC==0){
		print " -----------------------------------------------------------------\n";
		print " delete $FILE from $D_PATH $TMP_TIME pass!!!\n";
		print " -----------------------------------------------------------------\n"; 
	}else {   
		$RC=1;
		print " -----------------------------------------------------------------\n";
		print "The ${TMP_TIME}th step delete fail\n";
	}
}
	
sub copy_test{
	system("dd.exe if=/dev/zero of=$S_PATH\\$FILE bs=1024k count=$SIZE");
	print "----------------- COPY TEST BEGIN -----------------\n\n\n";
	for ( $i=0; $i<$COUNT; $i=$i+1 ){
  	if (-e "$D_PATH\\$i\\$FILE"){
   	 system("rmdir $D_PATH\\$i");
  	}

		system("mkdir $D_PATH\\$i");
  	system("copy $S_PATH\\$FILE $D_PATH\\$i");
  	$TMP_RC=$?;
  	system("fc $D_PATH\\$i\\$FILE $S_PATH\\$FILE");
  	$TMP_RC=$? |$TMP_RC;
  	$TMP_TIME=$i;
  	anal_copy_res();
  	$TMP=0;
  	print ">>>>>> step $i compare: $RC\n";
	}
	
	print "----------------- COPY TEST END -----------------\n\n\n";
}
sub delete_test{
	print "----------------- Delete TEST Begin -----------------\n";
	system("del $S_PATH\\$FILE");
	for ( $i=0; $i<$COUNT; $i=$i+1 ){
		if (-e "$D_PATH\\$i\\$FILE"){
			print ">>>>>> remove file: $RC\n";
    	system("del $D_PATH\\$i\\$FILE");
    	$TMP_RC=$?;
    	system("rmdir $D_PATH\\$i");
    	$TMP_RC=$? |$TMP_RC;
    	$TMP_TIME=$i;
    	anal_delete_res();
    	$TMP=0;
  	}else{
    	print ">>>>>> Invalid file path!!!\n";
  	}
  }
  print "----------------- Delete TEST End -----------------\n";	
}

printf "-----Usbd_test.pl Begin-----\n";
$S_PATH=$ARGV[1];
$D_PATH=$ARGV[2];
$SIZE=$ARGV[3];
$COUNT=$ARGV[4];
$FILE="$ARGV[3]M.img";
if($ARGV[0] =~ C){
	copy_test();
}elsif($ARGV[0] =~ D){
	delete_test();
}elsif($ARGV[0] =~ E){
	copy_test();
	delete_test();
}else{
	  $RC=1;
		print "Please enter the proper parameters: C or D or E\n";
		print "C: Copy Test\n";
		print "D: Delete Test\n";
		print "E: Copy Delete Test\n";
}

if ($RC == 0){
	print "Usbd_test.pl           TPASS    \n";
}else{
	print "Usbd_test.pl           FAIL    \n";
}