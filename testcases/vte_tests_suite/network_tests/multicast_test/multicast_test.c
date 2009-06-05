/*================================================================================================*/
/**
        @file   multicast_test.c

        @brief  Multicast test, send and recive multicasting message which read from file
*/
/*==================================================================================================

        Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
S.ZAVJALOV/                  10/06/2004     TLSbo39738  Initial version
A.Ozerov/b00320              11/12/2006     TLSbo84161  Minor changes.

====================================================================================================
Portability:  ARM GCC
==================================================================================================*/

/*==================================================================================================
Total Tests: 1

Test Executable Name:  multicast_test

Test Strategy:  A test for send and receive multicast message by use UDP sockets
=================================================================================================*/

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <errno.h>

/* Harness Specific Include Files. */
#include "test.h"

/* Verification Test Environment Include Files */
#include "multicast_test.h"

/*==================================================================================================
                                GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

struct termios stored_settings;

int VT_multicast_test(int argc, char* argv[])
{
    int                 send_s, recv_s;
    int                 i,message_len,fd_message,nSleepTime,key;
    unsigned char       ttl=1,loopb=FALSE;
    char                message[MAX_LENGTH_MESSAGE];
    caddr_t             addr_mfile;
    struct              sockaddr_in mcast_group;
    struct ip_mreq      mreq;
    struct utsname      name;
    struct stat         filestat;


    memset(&mcast_group, 0, sizeof(mcast_group));
    mcast_group.sin_family = AF_INET;

    while((i = getopt(argc, argv, "g:p:t:l:f:")) != EOF)
    {
        switch(i)
        {
            case 'g':
                mcast_group.sin_addr.s_addr = inet_addr(optarg);
                break;
            case 'p':
                mcast_group.sin_port = htons((unsigned short int)strtol(optarg, NULL, 0));
                break;
            case 't':
                ttl = strtol(optarg, NULL, 0);
                break;            
            case 'l':
                if (strcmp(optarg,"on") == 0) 
                    loopb = TRUE;
                if (strcmp(optarg,"off") == 0) 
                    loopb = FALSE;                
                break;            
            case 'f':
                if ((fd_message=open(optarg, O_RDONLY)) == -1)
                {
                    perror("Can't open message file");
                    return TFAIL;
                }
                fstat(fd_message, &filestat);
                if (filestat.st_size > MAX_LENGTH_MESSAGE) 
                    message_len=MAX_LENGTH_MESSAGE;
                else
                    message_len=filestat.st_size;
                addr_mfile=mmap(0, message_len, PROT_READ, MAP_SHARED, fd_message, 0);
                memcpy(message, addr_mfile, message_len);
                message[message_len]=0;
                break;            
            default:
                break;
        }
    }

    if(optind != 11)
    {
        fprintf(stderr, "\nUsage: %s -f <file_message> -g <mcast_group> -p <port> -t <ttl>\n -l <loopback (on|off)>\n\n", argv[0]);
        return TFAIL;
    }

    if ( (send_s=socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
    {
        perror ("send socket");
        return TFAIL;
    }   

    if (setsockopt(send_s, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) < 0) 
    {
        perror ("ttl setsockopt");
        return TFAIL;
    }

//Disable/Enable Loop-back 
    if (setsockopt(send_s, IPPROTO_IP, IP_MULTICAST_LOOP, &loopb, sizeof(loopb)) < 0) 
    {
        perror ("loop setsockopt");
        return TFAIL;
    }

    if ( (recv_s=socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
    {
        perror ("recv socket");
        return TFAIL;
    }

    i=1;
    if (setsockopt(recv_s, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i)) < 0) 
    {
        perror("reuseaddr setsockopt");
        return TFAIL;
    }

    if (bind(recv_s, (struct sockaddr*)&mcast_group, sizeof(mcast_group)) < 0) 
    {
        perror ("bind");
        return TFAIL;
    }

//Tell the kernel we want to join that multicast group. 
    mreq.imr_multiaddr = mcast_group.sin_addr;
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(recv_s, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) 
    {
        perror ("add_membership setsockopt");
        return TFAIL;
    }

    if (uname(&name) < 0) 
    {
        perror ("uname");
        return TFAIL;
    }

        set_terminal();
        printf("\nPRESS 'p' IF YOU SEE SOME PACKETS FROM ANOTHER HOSTS OR PRESS 'f'\n");
        printf("press any key to start test\n");
        while(! (kbhit(&nSleepTime)));
        printf("test started\n\n");

    switch (fork()) 
    {
//Error fork
        case -1:
            perror("fork");
            return TFAIL;
//Child
        case 0: 
        {
            int rs;
            int len;
            struct sockaddr_in from;
            message_len=MAX_LENGTH_MESSAGE;

            for (;;) 
            {
                len=sizeof(from);
                if ( (rs=recvfrom(recv_s, message, message_len, 0, 
                                (struct sockaddr*)&from, (socklen_t *)&len)) < 0) 
                {
                    perror ("recv");
                    exit (1);
                }
                message[rs] = 0;
                printf("%s: Received message from %s.\n", name.nodename,
                        inet_ntoa(from.sin_addr));
                printf("%s", message);
            }
        }
//Parent
        default: 
        { 
            for (;;) 
            {        
                if (kbhit(&nSleepTime))
                {
                        key=getchar();
                        if (key == 'p' || key == 'P') 
                        {
                                restore_terminal();
                                return TPASS;
                        }
                        if (key == 'f' || key == 'F') 
                        {
                                restore_terminal();
                                return TFAIL;
                        }
                }
                if (sendto(send_s, message, strlen(message), 0, (struct sockaddr*)&mcast_group, 
                            sizeof(mcast_group)) < strlen (message)) 
                {
                    perror("sendto");
                    return TFAIL;
                }
            }   
        }
    }
}

int VT_multicast_test_setup(void)
{
        return TPASS;
}

int VT_multicast_test_cleanup(void)
{
        return TPASS;
}

void set_terminal(void)
{
    struct termios new_settings;

    tcgetattr(0,&stored_settings);
    new_settings = stored_settings;
//NO ENTER
    new_settings.c_lflag &= (~ICANON);
//noecho
//    new_settings.c_lflag &= (~ECHO);
//no CTR+C
//    new_settings.c_lflag &= (~ISIG);
//BUFF = 1
//    new_settings.c_cc[VTIME] = 0;
//    new_settings.c_cc[VMIN] = 1;
    tcsetattr(0,TCSANOW,&new_settings);
    return;
}

void restore_terminal(void)
{
    tcsetattr(0,TCSANOW,&stored_settings);
    return;
}

BOOLEAN kbhit(int *pnSleepTime)
{
    fd_set rset;
    struct timeval tv;
    int nSelect;

    FD_ZERO(&rset);
    FD_SET(fileno(stdin), &rset);

    tv.tv_sec = 0;
    tv.tv_usec = SLEEP_TIME;

    *pnSleepTime = 0;

    nSelect = select(fileno(stdin) + 1, &rset, NULL, NULL, &tv);

    if (nSelect == -1)
    return FALSE;

    // Calculate the elapsed time
    *pnSleepTime = SLEEP_TIME;
    if (nSelect > 0)
    *pnSleepTime = tv.tv_usec;

    #ifdef LINUX
    // Calculate the elapsed time
    *pnSleepTime = SLEEP_TIME - tv.tv_usec;
    #else
    *pnSleepTime = SLEEP_TIME;  // Not very precise but would be enough
    #endif

    return nSelect > 0;
}
