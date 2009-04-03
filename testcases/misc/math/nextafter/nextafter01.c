/*
 *
 *   Copyright (c) International Business Machines  Corp., 2002
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

/* 01/02/2003	Port to LTP	avenkat@us.ibm.com */
/* 06/30/2001	Port to Linux	nsharoff@us.ibm.com */

<<<<<<< HEAD
/*
 * NAME
 *      scalb
=======

/*
 * NAME
 *      scalb 
>>>>>>> vte 20080401
 *
 * CALLS
 *      nextafter(3C)
 *
 * ALGORITHM
 *	Check results from the above functions against expected values.
 *
 * RESTRICTIONS
 * 	Checks for basic functionality, nothing fancy
 */

<<<<<<< HEAD
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "test.h"
#include "usctest.h"
=======
#include	<stdio.h>
#include	<math.h>
#include	<errno.h>
#include	<stdlib.h>
#include	"test.h"
#include	"usctest.h"
>>>>>>> vte 20080401

#define	FAILED 0
#define	PASSED 1

char *TCID = "nextafter01";

int local_flag = PASSED;
int block_number;
int errno;
FILE *temp;
int TST_TOTAL = 1;
<<<<<<< HEAD

void setup();
void blenter();
void blexit();
=======
extern int Tst_count;

void setup();
int blenter();
int blexit();
>>>>>>> vte 20080401

/*--------------------------------------------------------------*/
int main()
{
	double answer;
	double check;		 /* tmp variable */

	setup();		/* temp file is now open */
/*--------------------------------------------------------------*/
<<<<<<< HEAD
	blenter();
=======
 blenter();
>>>>>>> vte 20080401

	answer = nextafter(1.0, 1.1);
	check = (answer + 1.0) / 2;
	if ((check != answer) && ((float)check != 1.0)) {
		fprintf(temp, "nextafter returned %e, expected answer or 1.0\n",
				answer);
		local_flag = FAILED;
	}

        blexit();
/*--------------------------------------------------------------*/
<<<<<<< HEAD
	blenter();
=======
 blenter();
>>>>>>> vte 20080401

	answer = nextafter(1.0, 0.9);
	if ((check != answer) && (check != 1.0)) {
		fprintf(temp, "nextafter returned %e, expected answer or 1.0\n",
				answer);
		local_flag = FAILED;
	}

        blexit();
/*--------------------------------------------------------------*/
<<<<<<< HEAD
	blenter();
=======
 blenter();
>>>>>>> vte 20080401

	answer = nextafter(1.0, 1.0);
	if (answer != 1.0) {
		fprintf(temp, "nextafter 3 returned %e, expected 1.0\n",
				answer);
		local_flag = FAILED;
	}

        blexit();
/*--------------------------------------------------------------*/

<<<<<<< HEAD
	tst_exit();
=======
        tst_exit();      /* THIS CALL DOES NOT RETURN - EXITS!!  */
	return(0);
>>>>>>> vte 20080401
}
/*--------------------------------------------------------------*/

/*****	*****	LTP Port	*****/

/* FUNCTIONS */

void setup()
{
<<<<<<< HEAD
	temp = stderr;
}

void blenter()
{
	local_flag = PASSED;
}

void blexit()
{
	if (local_flag == PASSED)
		tst_resm(TPASS, "Test passed");
	else
		tst_resm(TFAIL, "Test failed");
}

/*****	*****		*****/
=======
  temp = stderr;
}


int blenter()
{
  local_flag = PASSED;
  return(0);
}


int blexit()
{
  (local_flag == PASSED ) ? tst_resm(TPASS, "Test passed") : tst_resm(TFAIL, "Test failed");
  return(0);
}


/*****	*****		*****/
>>>>>>> vte 20080401
