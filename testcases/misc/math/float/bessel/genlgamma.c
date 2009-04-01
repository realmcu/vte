/*
 * Copyright (C) Bull S.A. 2001
 *
 *   This program is free software;  you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY;  without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 *   the GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program;  if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/******************************************************************************/
/*                                                                            */
/* Dec-03-2001  Created: Jacky Malcles & Jean Noel Cordenner                  */
/*              These tests are adapted from AIX float PVT tests.             */
/*                                                                            */
/******************************************************************************/
#include 	<float.h>
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#include 	<errno.h>
#include        <limits.h>
#include        <unistd.h>
#include        <fcntl.h>
#include        <errno.h>
#include        <sys/signal.h>
#include        <math.h>

<<<<<<< HEAD
int create_Result_file()
{

=======


int create_Result_file()
{
	
>>>>>>> vte 20080401
	int signgam = 0;
	int i, nbVal, tabSign[20000];
	double	tabR[20000], Inc;
	char *F_name, *F_namesign;
	int fp, fpsi;

	F_name = "gamma_out.ref";
	F_namesign = "gamma_sign.ref";
<<<<<<< HEAD
	nbVal = 20000;
=======
	nbVal = 20000; 
>>>>>>> vte 20080401

	Inc = sqrt(3);

	for (i=0; i<nbVal; i++)
	{
		tabR[i] = lgamma(1+Inc*i);
		tabSign[i] = signgam;
	}

	fp = open(F_name,O_RDWR|O_CREAT|O_TRUNC,0777);
	fpsi = open(F_namesign,O_RDWR|O_CREAT|O_TRUNC,0777);
        if (!fp || !fpsi)
        {
            	printf("error opening file");
		close(fp);
		close(fpsi);
		return -1;
	}
	else
	{
<<<<<<< HEAD
		for (i = 0; i<nbVal; i++)
=======
		for (i = 0; i<nbVal; i++ ) 
>>>>>>> vte 20080401
		{
			write(fp,&tabR[i],sizeof(double));
			write(fpsi,&tabSign[i],sizeof(int));
		}
<<<<<<< HEAD

=======
	
>>>>>>> vte 20080401
		close(fp);
		close(fpsi);
		return 0;
	}
	return(0);
}

<<<<<<< HEAD
=======

>>>>>>> vte 20080401
int create_Data_file()
{
	int i, nbVal;
	double	tabD[20000], Inc;
	char *F_name;
	int fp;

	F_name = "gamma_inp.ref";
<<<<<<< HEAD
	nbVal = 20000;
=======
	nbVal = 20000; 
>>>>>>> vte 20080401

	Inc = sqrt(3);

	for (i=0; i<nbVal; i++)
		tabD[i] = (1+Inc*i);

<<<<<<< HEAD
=======

>>>>>>> vte 20080401
	fp = open(F_name,O_RDWR|O_CREAT|O_TRUNC,0777);
        if (!fp)
        {
            	printf("error opening file");
	    	close(fp);
	    	return -1;
        }
        else
<<<<<<< HEAD
        {
		for (i = 0; i<nbVal; i++)
=======
        {	
		for (i = 0; i<nbVal; i++ ) 
>>>>>>> vte 20080401
		{
			write(fp,&tabD[i],sizeof(double));
		}
		close(fp);
		return 0;
	}
	return(0);
}

<<<<<<< HEAD
int main(int argc, char  *argv[])
{

	if (argc > 1)
=======

int main(int argc, char  *argv[])
{

	if (argc > 1) 
>>>>>>> vte 20080401
	{
		switch ( atoi(argv[1]) )
		{
		case 1:
			if (create_Data_file() == 0)
				printf("Data file created\n");
			else
				printf("problem during %s data file creation\n", argv[0]);
			break;

		case 2:
			if (create_Result_file() == 0)
				printf("Result file created\n");
			else
				printf("problem during %s result file creation\n", argv[0]);
			break;
		default:
			printf("Bad arglist code for: '%s'\n", argv[0]);
			return -1;
			break;
		}
	}
	else
	{
		if (create_Data_file() != 0)
			printf("problem during %s data file creation\n", argv[0]);
		if (create_Result_file() != 0)
			printf("problem during %s result file creation\n", argv[0]);
	}
	return(0);
<<<<<<< HEAD
}
=======
}
>>>>>>> vte 20080401
