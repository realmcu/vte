/*================================================================================================*/
/**
        @file   socket_server_test.c

        @brief  UDP/TCP server socket which recive client connections and send message back
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

Test Executable Name:  socket_server_test

Test Strategy:  A test for send and receive message by use sockets
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
#include "socket_server_test.h"

/*==================================================================================================
                                GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

int                 soc=0;
unsigned char       *buf_rcv=0;

int VT_socket_server_test (int argc, char **argv) 
{
    struct sockaddr_in  clnt_addr;
    struct sockaddr_in  serv_addr;
    long                rs;
    int                 i, ns, pid, caddrlen, verbose_mode=1;
    int                 prtc_family=SOCK_DGRAM; //SOCK_DGRAM SOCK_STREAM SOCK_RAW
    int                 portnum=3490;
    int                 ps=1000;

    while((i = getopt(argc, argv, "t:p:s:v:")) != EOF)
    {
                switch(i)
                {
                    case 't':
                                if(strcmp(optarg, "UDP") == 0) 
                            prtc_family = SOCK_DGRAM;
                        if(strcmp(optarg, "TCP") == 0)
                            prtc_family = SOCK_STREAM;
                                break;
                    case 'p':
                                portnum = atoi(optarg);
                                break;
                    case 's':
                                ps=atoi(optarg);
                                break;
                case 'v':
                    if (strcmp(optarg,"on") == 0)
                                       verbose_mode = TRUE;
                                if (strcmp(optarg,"off") == 0)
                                        verbose_mode = FALSE;
                    default:
                                break;
                }
    }    

    if(optind != 9) 
    {  
                fprintf(stderr,"\nUsage: %s -t <porotocol(UDP|TCP)> -p <listen_port> -s <packet_size> -v <verbose_mode (on|off)>\n\n", argv[0]); 
                return TFAIL;; 
    }   

    if (! (buf_rcv=(unsigned char *) malloc (sizeof(unsigned char)*ps)))
        {
                perror("Can't locate memory");
                return TFAIL;
        }

    bzero((unsigned char*)&serv_addr,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port=htons(portnum);
    bzero((unsigned char*)&clnt_addr,sizeof(clnt_addr));
//Create socket   
    if((soc=socket(AF_INET, prtc_family, 0))==-1)
    {
                perror("Socket for data msgs cannot be created");
                return TFAIL;
    }
//bind
    if(bind(soc,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1)
    {
                perror("Data socket cannot be binded to OS");
                return TFAIL;
    }
        
    caddrlen = sizeof(clnt_addr);
    switch(prtc_family) 
    {
//UDP
        case SOCK_DGRAM:
                printf("Server ready...\n");
            for(;;)
            {
                        rs = recvfrom(soc, buf_rcv, ps, 0,(struct sockaddr *)&clnt_addr, (socklen_t *)&caddrlen);
                        if(rs == -1 ) 
                        {
                            perror("Error recvfrom()");
                                return TFAIL;
                        }
                        if (verbose_mode)
                                printf("Source addr: %s, send echo\n", inet_ntoa(clnt_addr.sin_addr));
                        if((sendto(soc, buf_rcv, rs, 0,(struct sockaddr *)&clnt_addr, sizeof(struct sockaddr))) != rs)
                        {
                            perror("Error send echo");
                                return TFAIL;
                        }
                }
            break;
//TCP
        case SOCK_STREAM:
                printf("Server ready...\n");
            for (;;)
            {
                        if(listen(soc, 10) == -1) 
                        {
                                perror("Error listen() call");                
                                return TFAIL;
                        }
                        ns = accept(soc,(struct sockaddr *)&clnt_addr, (socklen_t *)&caddrlen);
                        if(ns == -1) 
                        {
                                perror("Error accept() call");
                                return TFAIL;
                        }
                    if (verbose_mode)
                                printf("Source addr: %s, send echo\n", inet_ntoa(clnt_addr.sin_addr));
                        if((pid = fork()) == -1) 
                        {
                                perror("Error fork() call");
                                return TFAIL;
                        }
//Child
                        if(pid == 0)
                        {
                            close(soc);
                            while ((rs = read(ns, buf_rcv, ps)) > 0)
                            {
                                        if ((write(ns, buf_rcv, rs)) < 0)  
                                        {
                                            perror("Error write to socket");
                                                return TFAIL;
                                        }
                            }
                            close(ns);
                            free (buf_rcv);
                                if (verbose_mode)
                                        printf("Child process close sucsfull\n");
                            return TPASS;
                        }
                        close(ns);
            }
            break;
        default:
            break;
        }
        return TPASS;
}

int VT_socket_server_test_setup(void)
{
    return TPASS;
}

int VT_socket_server_test_cleanup(void)
{
    if (buf_rcv)
        free (buf_rcv);
    if (soc)
        close(soc);
    return TPASS;
}
