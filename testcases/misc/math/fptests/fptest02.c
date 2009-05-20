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

/* Group Bull & IBM Corporation */
/* 11/20/2002            Port to LTP             robbiew@us.ibm.com */
/*                                               jacky.malcles@bull.net */
/* IBM Corporation */
/* 06/30/2001 Port to Linux nsharoff@us.ibm.com */

/*
 * fptest02.c -- Floating point test.
 *
 * This is similar to fptest1.  Random values are used for some of the
 * math in routine "gauss".  The value "avgspd" computed in routine
 * "term()" should come out to a known value.  If this happens this
 * program prints a "passed" message and exits 0, otherwise a "failed"
 * message is printed and it exits with value 1.
 *
 */

#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#define MAGIC 0.777807
#define DIFF 0.001
#define EVENTMX 256
#define BIG 1.e50
#define FALSE 0
#define TRUE  1
#define TRYCRIT   1
#define ENTERCRIT 2
#define LEAVECRIT 3
#define ATBARRIER 4
#define ENTERWORK 5
#define LEAVEWORK 6
#define NULLEVENT 999

/** LTP Port **/
#include "test.h"
#include "usctest.h"

char *TCID"fptest02";          /* Test program identifier.    */
int TST_TOTAL1;                /* Total number of test cases. */
extern int Tst_count;           /* Test Case counter for tst_* routines */
/**************/


int init();
int doevent();
int term();
int addevent();

void gaussinit();
double gauss();

struct event {
 int proc;
 int type;
 double time;
 };

struct event eventtab[EVENTMX];
struct event rtrevent;
int waiting[EVENTMX];   /* array of waiting processors */
int nwaiting;  /* number of waiting processors */
double sgtime;  /* global clock */
double lsttime;  /* time used for editing */
double dtc, dts, alpha;   /* timing parameters */
int nproc;  /* number of processors */
int barcnt;  /* number of processors ATBARRIER */
int ncycle;  /* number of cycles completed */
int ncycmax;  /* number of cycles to run */
int critfree;  /* TRUE if critical section not occupied */

struct event *nextevent();

int
main(argc,argv)
int argc;
char *argv[];
{
 struct event *ev;

 nproc  128;
 ncycmax  10;
 dtc  0.01;
 dts  0.0;
 alpha  0.1;

 init();

 while ( (evnextevent()) ! (struct event *)NULL) {
  doevent(ev);
 }

 term();
 tst_resm(TPASS,"PASS");
 tst_exit();

 /**NOT REACHED**/
                 return(0);

}

/*
 initialize all processes to "entering work section"
*/
int
init()
{
 int p;
 double dtw, dtwsig;

 ncycle0;
 sgtime0;
 lsttime0;
 barcnt0;
 nwaiting0;
 critfreeTRUE;

 dtw1./nproc;  /* mean process work time */
 dtwsigdtw*alpha; /* std deviation of work time */
 gaussinit(dtw,dtwsig,time(0));

 for (p1; p<nproc; p++) {
  eventtab[p].type  NULLEVENT;
  }

 for (p1; p<nproc; p++) {
  addevent(ENTERWORK,p,sgtime);
  }

 return(0);
}
/*
 print edit quantities
*/
int
term()
{
 double avgspd;
 double v;

 avgspdncycle/sgtime;
 v  avgspd - MAGIC;
 if (v < 0.0)
  v * -1.0;
 if (v > DIFF) {
  tst_resm(TFAIL,"FAIL");
  v  avgspd - MAGIC;
  tst_resm(TINFO,"avgspd  %.15f\n", avgspd);
  tst_resm(TINFO,"expected %.15f\n", MAGIC);
  tst_resm(TINFO,"diff  %.15f\n", v);
  tst_exit();
 }
 return(0);
}
/*
 add an event to the event queue
*/
int
addevent(type,proc,t)
int type, proc;
double t;
{
 int i;
 int okFALSE;

 for (i1; i<nproc; i++) {
  if (eventtab[i].typeNULLEVENT) {
   eventtab[i].typetype;
   eventtab[i].procproc;
   eventtab[i].timet;
   okTRUE;
   break;
   }
  }
 if (ok)
  return(0);
 else{
                                 tst_resm(TBROK,"No room for event");
                                 tst_exit();
                 }

 return(0);
}
/*
 get earliest event in event queue
*/
struct event *nextevent()
{
 double mintimeBIG;
 int imin0;
 int i;

 for (i1; i<nproc; i++) {
          if ((eventtab[i].type!NULLEVENT) && (eventtab[i].time<mintime) ) {
  imini;
  mintimeeventtab[i].time;
  }
   }

 if (imin) {
  rtrevent.type  eventtab[imin].type;
  rtrevent.proc  eventtab[imin].proc;
  rtrevent.time  eventtab[imin].time;
  eventtab[imin].typeNULLEVENT;
  return(&rtrevent);
  }
 else
  return((struct event *)NULL);
}
/*
 add a processor to the waiting queue
*/
int
addwaiting(p)
int p;
{
 waiting[++nwaiting]p;
 return(0);
}
/*
 remove the next processor from the waiting queue
*/
int
getwaiting()
{
 if (nwaiting)
  return(waiting[nwaiting--]);
 else
  return(0);
}
double dtcrit()
{
 return(dtc);
}
double dtspinoff()
{
 return(dts);
}
double dtwork()
{
 return(gauss());
}
/*
 take the action prescribed by 'ev', update the clock, and
 generate any subsequent events
*/
int
doevent(ev)
struct event *ev;
{
 double nxttime;
 int i, p, proc;

 sgtime  ev->time;
 proc  ev->proc;

 switch (ev->type) {
  case TRYCRIT :
   if (critfreeTRUE)
    addevent(ENTERCRIT,proc,sgtime);
   else
    addwaiting(proc);
   break;
  case ENTERCRIT :
   critfree  FALSE;
   nxttimesgtime+dtcrit();
   addevent(LEAVECRIT,proc,nxttime);
   break;
  case LEAVECRIT :
   critfree  TRUE;
   addevent(ATBARRIER,proc,sgtime);
   if ((pgetwaiting())!0) {
    nxttimesgtime;
    addevent(ENTERCRIT,p,nxttime);
    }
   break;
  case ATBARRIER :
   barcnt++;
   if (barcntnproc) {
    nxttimesgtime;
    for (i1; i<nproc; i++) {
     nxttime+dtspinoff();
     addevent(ENTERWORK,i,nxttime);
     }
    barcnt0;
    ncycle++;
    }
   break;
  case ENTERWORK :
   nxttimesgtime+dtwork();
   if (ncycle<ncycmax)
    addevent(LEAVEWORK,proc,nxttime);
   break;
  case LEAVEWORK :
   addevent(TRYCRIT,proc,sgtime);
   break;
  default:
     tst_resm(TBROK,"Illegal event");
     tst_exit();
   break;
  }
 return(0);
}

static int alternator1;
static double mean;
static double stdev;
static double u1,u2;
static double twopi;
static double rnorm2147483647;

void gaussinit(m,s,seed)
double m,s;
int seed;
{
 srand48(seed);
 meanm;
 stdevs;
 twopi2.*acos((double)-1.0);
 return;
}

double gauss()
{
 double x1,x2;

 if (alternator1) {
  alternator  -1;
  u1  lrand48()/rnorm;
  u2  lrand48()/rnorm;
  x1  sqrt(-2.0*log(u1))*cos(twopi*u2);
  return(mean + stdev*x1);
  }
 else {
  alternator  1;
  x2  sqrt(-2.0*log(u1))*sin(twopi*u2);
  return(mean + stdev*x2);
  }
}

