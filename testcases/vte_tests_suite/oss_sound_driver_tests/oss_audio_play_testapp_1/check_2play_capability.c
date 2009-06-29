/***
**Copyright 2004-2009 Freescale Semiconductor, Inc. All Rights Reserved.
**
**The code contained herein is licensed under the GNU General Public
**License. You may obtain a copy of the GNU General Public License
**Version 2 or later at the following locations:
**
**http://www.opensource.org/licenses/gpl-license.html
**http://www.gnu.org/copyleft/gpl.html
**/
/*================================================================================================*/
/**
        @file   check_2play_capability.c

        @brief  OSS audio multiple play test source file.
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
RB657C/gsch1c                20/07/2004     TLSbo40898  Initial version  of OSS sound driver test development
RB657C /gsch1c               20/07/2004     TLSbo43102  Add a parameter to the application
I.Inkina/nknl001             11/01/2005     TLSbo61044  Clock master was added
D.Khoroshev/b00313           15/02/2006     TLSbo62323  Updates accoring to the last specifications
A.Ozerov/b00320              21/07/2006     TLSbo70792  Synchronization of threads was added.
D.Simakov                    19/10/2006     TLSbo76144  dsp->adsp, dsp1->dsp
====================================================================================================
Portability: ARM GCC
==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <errno.h>
    
/* Harness Specific Include Files. */
#include <test.h>

/* Verification Test Environment Include Files */
#include "oss_sound_driver_test.h"
#include "../common.h"

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/
#define BUF_SIZE 4096

/*==================================================================================================
                                        LOCAL STRUCTURES AND TYPEDEFS
==================================================================================================*/
typedef struct _thrd_t
{
        int drv_fd;
        FILE* file_fd;
        FILE* file_fd1;
        int max_loops;
} thrd_t;

/*==================================================================================================
                                        LOCAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                        LOCAL VARIABLES
==================================================================================================*/
static int open_count = 0;
pthread_mutex_t open_count_flag_mutex = PTHREAD_MUTEX_INITIALIZER;

/*==================================================================================================
                                        LOCAL FUNCTIONS
==================================================================================================*/
int VT_oss_sound_driver_setup(void)
{
        return TPASS;
}

int VT_oss_sound_driver_cleanup(void)
{
        return TPASS;
}

unsigned short get_audio_bits(FILE* file)
{
        unsigned short ret;
        fseek(file, 34, SEEK_SET);
        fread(&ret, 2, 1, file);
        return ret;
}

unsigned short get_audio_channels(FILE* file)
{
        unsigned short ret;
        fseek(file, 22, SEEK_SET);
        fread(&ret, 2, 1, file);
        return ret;
}

int get_audio_frq(FILE* file)
{
        int ret;
        fseek(file, 24, SEEK_SET);
        fread(&ret, 4, 1, file);
        return ret;
}

/*================================================================================================*/
/*===== set_audio_config =====*/
/**
@brief  Setup audio configuration 

@param  int fd_audio
        FILE* file
    
@return 
*/
/*================================================================================================*/
int set_audio_config(int fd_audio, FILE* file)
{
        int tmp, format, frequency, channels;
        int VT_rv = TPASS;

        tmp = format = (int) get_audio_bits(file);
        if (ioctl(fd_audio, SNDCTL_DSP_SETFMT, &format) < 0)
        {
                tst_resm(TINFO,
                        "Error in SNDCTL_DSP_SETFMT ioctl call (arg is %d, returned %d). %s", 
                        tmp, format, strerror(errno));
                VT_rv = TFAIL;
        }
        else if ( tmp != format )
        {
                tst_resm(TINFO,
                        "Format was not set (SNDCTL_DSP_SETFMT, arg is %d, returned %d). %s", 
                        tmp, format);
                VT_rv = TFAIL;
        }

        tmp = frequency = get_audio_frq(file);
        if (ioctl(fd_audio, SOUND_PCM_WRITE_RATE, &frequency)< 0)
        {
                tst_resm(TINFO,
                        "Error in SOUND_PCM_WRITE_RATE ioctl call (arg is %d, returned %d). %s", 
                        tmp, frequency, strerror(errno));
                VT_rv = TFAIL;
        }
        else if ( tmp != frequency )
        {
                tst_resm(TINFO,
                        "Format was not set (SOUND_PCM_WRITE_RATE, arg is %d, returned %d). %s",
                        tmp, frequency);
                VT_rv = TFAIL;
        }

        tmp = channels = get_audio_channels(file);
        if (ioctl(fd_audio, SNDCTL_DSP_CHANNELS, &channels) < 0)
        {
                tst_resm(TINFO,
                        "Error in SNDCTL_DSP_CHANNELS ioctl call (arg is %d, returned %d). %s",
                        tmp, channels, strerror(errno));
                VT_rv = TFAIL;
        }
        else if ( tmp != channels )
        {
                tst_resm(TINFO,
                        "Format was not set (SNDCTL_DSP_CHANNELS, arg is %d, returned %d). %s",
                        tmp, channels);
                VT_rv = TFAIL;
        }

        tst_resm(TINFO, "Configuration : format %d, frequency %d, channels %d\n", format, frequency, channels);

        return VT_rv;
}

/*================================================================================================*/
/*===== main1 =====*/
/**
@brief  Thread 1 function

@param  thrd_t *local_arg

@return Nothing
*/
/*================================================================================================*/
void main_mix(thrd_t *local_arg)
{

        int n, n1, nleft, nleft1, nwritten, i;
        char buf[BUF_SIZE], buf1[BUF_SIZE], *p, *p1;
        FILE *fd_file = local_arg->file_fd;
        FILE *fd_file1 = local_arg->file_fd1;
        int fd_audio = local_arg->drv_fd;
        int loop = local_arg->max_loops;
        int l_count;

        /* Tell that the thread is created */
        pthread_mutex_lock (&open_count_flag_mutex);
        open_count++;
        pthread_mutex_unlock (&open_count_flag_mutex);

        /* wait for the other thread to be created */
        do
        {
                pthread_mutex_lock (&open_count_flag_mutex);
                l_count = open_count;
                pthread_mutex_unlock (&open_count_flag_mutex);
        }
        while ( l_count < 1 );

        if ( set_audio_config(fd_audio, fd_file) < 0 ){
                tst_resm(TINFO, "Bad format for file 1");
                goto l_end;
        }


        /* read the file many times if desired */
        for ( i=0; i<loop; i++ )
        {
                /* On passe l'entete WAV */
                fseek(fd_file, 44, SEEK_SET);
                fseek(fd_file1, 44, SEEK_SET);

                /* Lecture de l'échantillon */
                while ( 1 ) {
                        memset(buf, 0, 16);
                        memset(buf1, 0, 16);
                        n = fread (buf, 1, 16, fd_file);
                        n1 = fread (buf1, 1, 16, fd_file1);
                        if ( n <= 0 && n1 <= 0 ) break;
                        if ( n && n < 16 ) {
                                n = 16;
                        }
                        if ( n1 && n1 < 16 ) {
                                n1 = 16;
                        }

                        nleft = n;
                        nleft1 = n1;
                        p = buf;
                        p1 = buf1;

                        /* On envoie l'échantillon... */
                                for (i=0; i<16; i++)
                                        p[i] = p[i] + p1[i];
                        while (nleft) {
                                if ((nwritten = write (fd_audio, p, nleft)) < 0)
                                        perror ("write error");
                                if (nwritten < 0) continue;
                                nleft -= nwritten;
                                p += nwritten;
                        }
                }
        }

l_end:
        /* Exit */
        close (fd_audio);

        /* Tell that the thread is closed */
        pthread_mutex_lock (&open_count_flag_mutex);
        open_count--;
        pthread_mutex_unlock (&open_count_flag_mutex);
}


/*================================================================================================*/
/*===== main1 =====*/
/**
@brief  Thread 1 function

@param  thrd_t *local_arg
    
@return Nothing
*/
/*================================================================================================*/
void main1(thrd_t *local_arg)
{
        int n, nleft, nwritten, i;
        char buf[BUF_SIZE], *p;
        FILE *fd_file = local_arg->file_fd;
        int fd_audio = local_arg->drv_fd;
        int loop = local_arg->max_loops;
        int l_count;

        tst_resm(TINFO, "Thread 1 enter\n");        

        /* Tell that the thread is created */
        pthread_mutex_lock (&open_count_flag_mutex);
        open_count++;
        pthread_mutex_unlock (&open_count_flag_mutex);

        /* wait for the other thread to be created */
        do
        {
                pthread_mutex_lock (&open_count_flag_mutex);
                l_count = open_count;
                pthread_mutex_unlock (&open_count_flag_mutex);
        } 
        while ( l_count < 2 );

        tst_resm(TINFO, "Thread 1 config\n");        
        if ( set_audio_config(fd_audio, fd_file) < 0 ){
                tst_resm(TINFO, "Bad format for file 1");
                goto _end;
        }

        tst_resm(TINFO, "Thread 1 play\n");        
        
        /* read the file many times if desired */
        for ( i=0; i<loop; i++ )
        {
                /* On passe l'entete WAV */
                fseek(fd_file, 44, SEEK_SET);

                /* Lecture de l'échantillon */
                while ((n = fread (buf, 1, 1, fd_file)) > 0) {
                        nleft = n;
                        p = buf;

                        /* On envoie l'échantillon... */
                        while (nleft){
                                if ((nwritten = write (fd_audio, p, nleft)) < 0)
                                        perror ("write error");
                                if (nwritten < 0) continue;
                                nleft -= nwritten;
                                p += nwritten;
                        }
                } 
        }

_end:
        /* Exit */
        close (fd_audio);
        tst_resm(TINFO, "Thread 1 exit\n");

        /* Tell that the thread is closed */
        pthread_mutex_lock (&open_count_flag_mutex);
        open_count--;
        pthread_mutex_unlock (&open_count_flag_mutex);
}

/*================================================================================================*/
/*===== main2 =====*/
/**
@brief  Thread 2 function

@param  thrd_t *local_arg
    
@return Nothing
*/
/*================================================================================================*/
void main2(thrd_t *local_arg)
{
        int n, nleft, nwritten, i;
        char buf[BUF_SIZE], *p;
        FILE *fd_file = local_arg->file_fd;
        int fd_audio = local_arg->drv_fd;
        int loop = local_arg->max_loops;
        int l_count;

        tst_resm(TINFO, "Thread 2 enter\n");        

        /* Tell that the thread is created */
        pthread_mutex_lock (&open_count_flag_mutex);
        open_count++;
        pthread_mutex_unlock (&open_count_flag_mutex);

        /* wait for the other thread to be created */
        do {
                pthread_mutex_lock (&open_count_flag_mutex);
                l_count = open_count;
                pthread_mutex_unlock (&open_count_flag_mutex);
        }
        while ( l_count < 2 );

        tst_resm(TINFO, "Thread 1 config\n");        

        if ( set_audio_config(fd_audio, fd_file) < 0 ){
                tst_resm(TINFO, "Bad format for file 2\n");
                goto _end;
        }

        tst_resm(TINFO, "Thread 2 play\n");        

        /* read the file many times if desired */
        for ( i=0; i<loop; i++ )
        {
                /* On passe l'entete WAV */
                fseek(fd_file, 44, SEEK_SET);

                /* Lecture de l'échantillon */
                while ((n = fread (buf, 1, 1, fd_file)) > 0) {
                        nleft = n;
                        p = buf;
                        /* On envoie l'échantillon... */
                        while (nleft){
                                if ((nwritten = write (fd_audio, p, nleft)) < 0)
                                        perror ("write error");
                                if (nwritten < 0) continue;
                                nleft -= nwritten;
                                p += nwritten;
                        }
                }
        }

_end:
        /* Exit */
        sleep(1);
        tst_resm(TINFO, "Thread 2 exit\n");

        /* Tell that the thread is closed */
        pthread_mutex_lock (&open_count_flag_mutex);
        open_count--;
        pthread_mutex_unlock (&open_count_flag_mutex);

        /* wait for the other thread to be created */
        do {
                pthread_mutex_lock (&open_count_flag_mutex);
                l_count = open_count;
                pthread_mutex_unlock (&open_count_flag_mutex);
        }
        while ( l_count > 0 );

        /* pthread_exit(exit_val); */
}

/*================================================================================================*/
/*===== VT_oss_sound_driver_test =====*/
/**
@brief  Test program

@param  None
    
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_oss_sound_driver_test(int device, char * file1, char * file2, int loop)
{
        int VT_rv = TFAIL;
        FILE *fd_file, *fd_file2;
        int fd_audio, fd_audio2;
        int ret;
        thrd_t local_arg, local_arg2;
        pthread_t thread1/*, thread2*/;
        char *first_dev, *second_dev;
        first_dev = CODEC_DEV;
        second_dev = STDAC_DEV;

        if ((fd_audio = open (first_dev, O_WRONLY)) < 0){
                tst_resm(TFAIL, "[1]Error opening %s", first_dev);
                return VT_rv;
        }

        if ((fd_file = fopen (file1, "r")) == 0) {
                tst_resm(TFAIL, "Error opening the file %s \n", file1);
                close(fd_audio2);
                close(fd_audio);
                return VT_rv;
        }

        if ((fd_file2 = fopen (file2, "r")) == 0) {
                tst_resm(TFAIL, "Error opening the file %s \n", file2);
                fclose(fd_file);
                close(fd_audio2);
                close(fd_audio);
                return VT_rv;
        }

        local_arg.drv_fd = fd_audio;
        local_arg.file_fd = fd_file;
        local_arg.file_fd1 = fd_file2;

        local_arg.max_loops = loop;
        ret = pthread_create(&thread1, NULL, (void *)&main_mix, (void *)&local_arg);

        local_arg2.drv_fd = fd_audio;
        local_arg2.file_fd = fd_file2;
        local_arg2.max_loops = loop;

        ret = pthread_join(thread1, NULL);

        fclose (fd_file);
        fclose (fd_file2);
        close (fd_audio);

        VT_rv = TPASS;
        return VT_rv;
}
