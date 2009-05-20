/*====================*/
/**
    @file   PM_test_X.c

    @brief  Test scenario C source template.
*/
/*======================

  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.

====================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Dmitriy Kazachkov           10/06/2004     TLSbo39741   initial version based on trace_shed by Manoj Iyer
I.Inkina                    04/08/2005     TLSbo52863   free malloc
====================
Portability: Indicate if this module is portable to other compilers or platforms.
             If not, indicate specific reasons why is it not portable.


======================*/
/* Description: This utility spawns N tasks, each task sets its priority by   */
/*  making a system call to the scheduler. The thread function    */
/*  reads the priority that tbe schedular sets for this task and  */
/*  also reads from /proc the processor this task last executed on*/
/*  the information that is gathered by the thread function may   */
/*  be in real-time. Its only an approximation.                   */

/*======================
                                        INCLUDE FILES
======================*/
/* Standard Include Files */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#ifdef _LINUX_
// defines struct msgbuf
#define __USE_GNU
#endif
#include <sys/msg.h>
#include <sys/types.h>
#include <stdlib.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/timeb.h>
#include <unistd.h>
#include <string.h>




#ifdef __cplusplus
extern "C"{
#endif

/* Harness Specific Include Files. */
#include "test.h"
#ifdef __cplusplus
}

#endif

/* Verification Test Environment Include Files */
#include "PM_test_X.h"

/*======================
                                        LOCAL MACROS
======================*/
#ifdef PTHREAD_THREADS_MAX
#define PIDS PTHREAD_THREADS_MAX /* maximum thread allowed.                     */
#elif defined(PID_MAX_DEFAULT)
#define PIDS PID_MAX_DEFAULT     /* maximum pids allowed.                       */
#else
#define PIDS PID_MAX   /* alternative way maximum pids may be defined */
#endif

/*======================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
======================*/
typedef struct
{        /* contains priority and CPU info of the task.        */
    int exp_prio; /* priority that we wish to set.                      */
    int act_prio; /* priority set by the scheduler.                     */
    int proc_num; /* last processor on which this task executed.        */
    int procs_id; /* pid of this task.                                  */
    int s_policy; /* scheduling policy for the task.                    */
} thread_sched_t;

/*======================
                                       LOCAL CONSTANTS
======================*/


/*======================
                                       LOCAL VARIABLES
======================*/

    thread_sched_t *chld_args = 0; /* arguments to funcs execed by child process.*/
    thread_sched_t *status = 0;     /* exit status for light weight process.      */

/*======================
                                       GLOBAL CONSTANTS
======================*/


/*======================
                                       GLOBAL VARIABLES
======================*/
extern int verbose_flag;

/*======================
                                   LOCAL FUNCTION PROTOTYPES
======================*/


/*======================
                                       LOCAL FUNCTIONS
======================*/

/******************************************************************************/
/*                                                                            */
/* Function:    get_proc_num                                                  */
/*                                                                            */
/* Description: Function reads the proc filesystem file /proc/<PID>/stat      */
/*  gets the CPU number this process last executed on and returns */
/*  Some hard assumptions were made regarding buffer sizes.       */
/*                                                                            */
/* Return:      exits with -1 - on error                                      */
/*              CPU number - on success                  */
/*                                                                            */
/******************************************************************************/
static int
get_proc_num(void)
{
    int fd = -1; /* file descriptor of the /proc/<pid>/stat file.      */
    int fsize = -1;     /* size of the /proc/<pid>/stat file.               */
    char filename[256]; /* buffer to hold the string /proc/<pid>/stat.        */
    char fbuff[512];    /* contains the contents of the stat file.            */

    /* get the name of the stat file for this process */
    sprintf(filename, "/proc/%d/stat", getpid());

    /* open the stat file and read the contents to a buffer */
    if ((fd  = open(filename, O_RDONLY)) == (int)NULL)
    {
        tst_brkm(TBROK, cleanup, "get_proc_num():open() FAILED ");
    }

    usleep(6);
    sched_yield();

    if ((fsize = read(fd, fbuff, 512)) == -1)
    {
        tst_brkm(TBROK, cleanup, "main():read() FAILED ");
    }

    close(fd);
    /* return the processor number last executed on. */
    return atoi(&fbuff[fsize - 2]);
}

/******************************************************************************/
/*                                                                            */
/* Function:    thread_func                                                   */
/*                                                                            */
/* Description: This function is executed in the context of the new task that */
/*  pthread_createi() will spawn. The (thread) task will get the  */
/*  minimum and maximum static priority for this system, set the  */
/*  priority of the current task to a random priority value if    */
/*  the policy set if SCHED_FIFO or SCHED_RR. The priority if this*/
/*  task that was assigned by the scheduler is got from making the*/
/*  system call to sched_getscheduler(). The CPU number on which  */
/*  the task was last seen is also recorded. All the above data is*/
/*  returned to the calling routine in a structure thread_sched_t.*/
/*                                                                            */
/* Input:       thread_sched_t          */
/*      s_policy - scheduling policy for the task.                */
/*                                                                            */
/* Return:      thread_sched_t - on success.          */
/*      exp_prio - random priority value to set.                  */
/*      act_prio - priority set by the scheduler.                 */
/*      proc_num - CPU number on which this task last executed.   */
/*      procs_id -  pid of this task.                             */
/*                                                                            */
/*  -1        - on error.                                    */
/*                                                                            */
/******************************************************************************/
void *
thread_func(void *args)  /* arguments to the thread function           */
{
    static int max_priority;    /* max possible priority for a process.       */
    static int min_priority;    /* min possible priority for a process.       */
    static int set_priority;    /* set the priority of the proc by this value.*/
    static int get_priority;    /* get the priority that is set for this proc.*/
    static int procnum;         /* processor number last executed on.         */
    static int sched_policy;    /* scheduling policy as set by user/default   */
    struct sched_param ssp;     /* set schedule priority.                     */
    struct sched_param gsp;     /* gsp schedule priority.                     */
    struct timeb       tptr;    /* tptr.millitm will be used to seed srand.   */
    thread_sched_t *locargptr = /* local ptr to the arguments.                */
                 (thread_sched_t *) args;

    /* Get the system max and min static priority for a process. */
    if (((max_priority = sched_get_priority_max(SCHED_FIFO)) == -1) ||
         ((min_priority = sched_get_priority_min(SCHED_FIFO)) == -1))
    {
        tst_resm(TFAIL, "failed to get static priority range\n"
   "pid[%d]: exiting with -1\n", getpid());
 pthread_exit((void*)-1);
    }

    if ((sched_policy = locargptr->s_policy) == SCHED_OTHER)
        ssp.sched_priority = 0;
    else
    {
        /* Set a random value between max_priority and min_priority */
        ftime(&tptr);
        srand((tptr.millitm)%1000);
        set_priority = (min_priority + (int)((float)max_priority
          * rand()/(RAND_MAX+1.0)));
        ssp.sched_priority = set_priority;
    }


    /* give other threads a chance */
    usleep(8);

    /* set a random priority value and check if this value was honoured. */
    if ((sched_setscheduler(getpid(), sched_policy, &ssp)) == -1)
    {
        tst_resm(TFAIL, "main(): sched_setscheduler()"
   "pid[%d]: exiting with -1\n", getpid());
        pthread_exit((void*)-1);
    }

    /* processor number this process last executed on */
    if ((procnum = get_proc_num()) == -1)
    {
        tst_resm(TFAIL,"main(): get_proc_num() failed\n"
   "pid[%d]: exiting with -1\n", getpid());
        pthread_exit((void*)-1);
    }

    if ((get_priority = sched_getparam(getpid(), &gsp)) == -1)
    {
        tst_resm(TFAIL, "main(): sched_setscheduler()"
   "pid[%d]: exiting with -1\n", getpid());
        pthread_exit((void*)-1);
    }

    /* processor number this process last executed on */
    if ((procnum = get_proc_num()) == -1)
    {
        tst_resm(TFAIL,"main(): get_proc_num() failed\n"
   "pid[%d]: exiting with -1\n", getpid());
        pthread_exit((void*)-1);
    }

    if (verbose_flag)
    {
        fprintf(stdout,
            "PID of this task         = %d\n"
     "Max priority             = %d\n"
     "Min priority             = %d\n"
            "Expected priority        = %d\n"
            "Actual assigned priority = %d\n"
            "Processor last execed on = %d\n\n", getpid(),
      max_priority, min_priority, set_priority,
      gsp.sched_priority, procnum);
        fprintf(stdout,"pid[%d]: exiting with %ld\n", getpid(),locargptr);

    }

    locargptr->exp_prio = set_priority;
    locargptr->act_prio = gsp.sched_priority;
    locargptr->proc_num = procnum;
    locargptr->procs_id = getpid();

    pthread_exit((void*)locargptr);
}


/*====================*/
/*= VT_PM_setup =*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_PM_setup(void)
{

   if (getuid() != 0)
   {
        tst_brkm(TBROK , cleanup, "ERROR: Only root user can run this program.\n");
 return TFAIL;
   }

    return TPASS;
}


/*====================*/
/*= VT_cleanup =*/
/**
@brief  assumes the post-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_PM_cleanup(void)
{
    if(chld_args!=0) free(chld_args);
    if(status!=0)  free(status);

    return TPASS;
}


/*====================*/
/*= VT_PM_test_X =*/
/**
@brief  Template test scenario X function

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_PM_test_X(char* cpu_option, char* threads_option, char* policy_opt)
{
    int       proc_ndx;  /* number of time to repete the loop.         */
    int       pid_ndx;  /* number of time to repete the loop.         */
    int       num_cpus = 1;    /* assume machine is an UP machine.           */
    int       num_thrd = MAXT*num_cpus;  /* number of threads to create.               */
    int       thrd_ndx;         /* index into the array of threads.       */
    int       exp_prio[PIDS];   /* desired priority, random value.            */
    int       act_prio[PIDS];   /* priority actually set.                     */
    int       gen_pid[PIDS];    /* pid of the processes on this processor.    */
    int       proc_id[PIDS];    /* id of the processor last execed on.        */
    int       spcy = SCHED_FIFO;/* scheduling policy for the tasks.           */
    pthread_t thid[PIDS]; /* pids of process or threads spawned         */

    extern  char *optarg; /* arguments passed to each option.       */

  if(cpu_option)
  {
  num_cpus = atoi(cpu_option);
 if(num_cpus <1)
 {
  tst_resm(TWARN, "WARNING: Bad argument -C %d. Using default - 1\n",
    num_cpus);
         num_cpus = 1;
 }
  }



  if(threads_option)
  {
  num_thrd = atoi(threads_option);
 if(num_thrd <10)
 {
  tst_resm(TWARN, "WARNING: Bad argument -P %d. Using default %d \n",
    num_thrd, MAXT);
         num_thrd = MAXT;
 }
        if (num_thrd > PIDS)
        {
                    tst_resm(TWARN,
                        "WARNING: -t %d exceeds maximum number of allowed pids"
                        " %d\n Setting number of threads to %d\n", num_thrd,
     PIDS, PIDS - 1000);
                          num_thrd = (PIDS - 1000);
        }
  }

  if(policy_opt)
  {
        if (strncmp(policy_opt, "fifo", 4) == 0)
                    spcy = SCHED_FIFO;
        else
        if (strncmp(policy_opt, "rr", 2) == 0)
                    spcy = SCHED_RR;
 else
        if (strncmp(optarg, "other", 5) == 0)
                    spcy = SCHED_OTHER;
 else
        {
                    tst_resm(TWARN, "ERROR: Unrecognized scheduler policy,"
        "using default - fifo\n");
        }
   }


    /* create num_thrd number of threads. */
    for (thrd_ndx = 0; thrd_ndx < num_thrd; thrd_ndx++)
    {
        chld_args = malloc(sizeof(thread_sched_t));
 chld_args->s_policy = spcy;
        if (pthread_create(&thid[thrd_ndx], NULL, thread_func,
  chld_args))
        {
            tst_resm(TFAIL,"ERROR: main(): creating task number: %d\n", thrd_ndx);
            return(TFAIL);
        }
        if (verbose_flag)
            fprintf(stdout, "Created thread[%d]\n", thrd_ndx);
        usleep(9);
        sched_yield();
        free(chld_args);
        chld_args=0;
    }

    /* wait for the children to terminate */
    for (thrd_ndx = 0; thrd_ndx<num_thrd; thrd_ndx++)
    {

        status = malloc(sizeof(thread_sched_t));

        if (pthread_join(thid[thrd_ndx], (void **)&status))
        {
            tst_resm(TFAIL," ERROR: main(): pthread_join() joining task number: %d\n", thrd_ndx);
            return(TFAIL);
        }
        else
        {
            if (status == (thread_sched_t *)-1)
            {
                tst_resm(TFAIL,
      "thread [%d] - process exited with errors %d\n",
                     thrd_ndx, WEXITSTATUS(status));
         return(TFAIL);
            }
            else
            {
                exp_prio[thrd_ndx] = status->exp_prio;
                act_prio[thrd_ndx] = status->act_prio;
                proc_id[thrd_ndx] =  status->proc_num;
                gen_pid[thrd_ndx] =  status->procs_id;
            }
        }
        usleep(10);
        free(status);
        status=0;

    }

    if (verbose_flag)
    {
       fprintf(stdout,
           "Number of tasks spawned: %d\n"
    "Number of CPUs:          %d\n"
    "Scheduling policy:       %d\n", num_thrd, num_cpus, spcy);

    for (proc_ndx = 0; proc_ndx < num_cpus; proc_ndx++)
    {
        fprintf(stdout, "For processor number = %d\n", proc_ndx);
        fprintf(stdout, "%s\n", "=======");
        for (pid_ndx = 0; pid_ndx < num_thrd; pid_ndx++)
        {
                if (proc_id[pid_ndx] == proc_ndx)
      fprintf(stdout, "pid of task = %d priority requested = %d"
                                    " priority assigned by scheduler = %d\n",
                                       gen_pid[pid_ndx], exp_prio[pid_ndx], act_prio[pid_ndx]);
                }
         }
   }

   int rv = TPASS;

   return rv;
}

