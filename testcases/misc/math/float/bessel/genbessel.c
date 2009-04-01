/*
<<<<<<< HEAD
 * Copyright(C) Bull S.A. 2001
 * Copyright(c) International Business Machines  Corp., 2001
=======
 * Copyright (C) Bull S.A. 2001
 * Copyright (c) International Business Machines  Corp., 2001
>>>>>>> vte 20080401
 *
 *   This program is free software;  you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
<<<<<<< HEAD
 *  (at your option) any later version.
=======
 *   (at your option) any later version.
>>>>>>> vte 20080401
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
<<<<<<< HEAD
/*									    */
/* Dec-03-2001  Created: Jacky Malcles & Jean Noel Cordenner		  */
/*	      These tests are adapted from AIX float PVT tests.	     */
/*									    */
/******************************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define	MAX_FNAME_LEN	16

/*****************************************************************
 * create file:
 *
 * func_name is the name of the function
=======
/*                                                                            */
/* Dec-03-2001  Created: Jacky Malcles & Jean Noel Cordenner                  */
/*              These tests are adapted from AIX float PVT tests.             */
/*                                                                            */
/******************************************************************************/
#include	<sys/types.h>
#include	<sys/wait.h>
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



/*****************************************************************
 * create file: 
 * 	
 * func_name is the name of the function 
>>>>>>> vte 20080401
 *
 * code can take 2 values: DATA_CREATE to create a input data file
 *			   RESULT_CREATE for output result file
 */

int create_file(char *func_name, int NbVal)
{
	pid_t myproc;
<<<<<<< HEAD

	switch (myproc = fork()) {
	case -1:
		err(1, "fork failed");
	case 0: {
		char *arglist[] = { func_name, NULL };
	     	execvp(arglist[0], arglist);

	     	err(1, "execvp failed");
	}
	default:
		;
	}
	return (myproc);
}

int main(int argc, char *argv[])
{
	char *funct, *bin_path;
	pid_t child;

	if (argc != 2)
		errx(1, "need the path to generation binaries");

	bin_path = argv[1];
	funct = malloc(strlen(bin_path) + MAX_FNAME_LEN);
	if (funct == NULL)
		err(1, "malloc failed");
	sprintf(funct, "%s/genj0", bin_path);
	child = create_file(funct, 0);
	waitpid(child, NULL, 0);

	sprintf(funct, "%s/genj1", bin_path);
	child = create_file(funct, 0);
	waitpid(child, NULL, 0);

	sprintf(funct, "%s/geny0", bin_path);
	child = create_file(funct, 0);
	waitpid(child, NULL, 0);

	sprintf(funct, "%s/geny1", bin_path);
	child = create_file(funct, 0);
	waitpid(child, NULL, 0);

	sprintf(funct, "%s/genlgamma", bin_path);
	child = create_file(funct, 0);
	waitpid(child, NULL, 0);

	return 0;
}
=======
	           
        if (( myproc = fork() )!=0)
                return myproc;
        else {
		char *arglist[] = { func_name, NULL};
	     	execvp(arglist[0], arglist);

	     	fprintf(stderr, "ERROR %s\n", strerror(errno));
	     	abort();
	}
	return(0);
} 




int main(int argc, char *argv[])
{
	char *funct;
	pid_t child;
	
	funct = "./genj0";
	child=create_file(funct, 0);
	waitpid(child,NULL,0);

	funct = "./genj1";
	child=create_file(funct, 0);
	waitpid(child,NULL,0);

	funct = "./geny0";
	child=create_file(funct, 0);
	waitpid(child,NULL,0);

	funct = "./geny1";
	child=create_file(funct, 0);
	waitpid(child,NULL,0);

	funct = "./genlgamma";
	child=create_file(funct, 0);
	waitpid(child,NULL,0);

	return 0;
}
>>>>>>> vte 20080401
