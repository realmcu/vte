/*================================================================================================*/
/**
    @file   socket_client_test.c

    @brief  UDP/TCP client socket which connect to server send message and wait to 
	    recive this message back
*/
/*==================================================================================================

			      Motorola Confidential Proprietary
		   (c) Copyright 2004, Motorola, Inc.  All rights reserved.

Presence of a copyright notice is not an acknowledgement of publication.
This software file listing contains information of Motorola, Inc. that is of a confidential and
proprietary nature and any viewing or use of this file is prohibited without specific written
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.

====================================================================================================
Revision History:
			    Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
S.ZAVJALOV / -----           10/06/2004     TLSbo39738   Initial version

====================================================================================================
Portability:  ARM GCC  gnu compiler
==================================================================================================*/

/*==================================================================================================
Total Tests: 1

Test Executable Name:  socket_client_test

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
#include "socket_client_test.h"

/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

unsigned char	*buf=0,*buf_rcv=0;
int		soc=0;

int VT_socket_client_test(int argc, char **argv) 
{

    struct sockaddr_in  serv_addr;
    struct hostent      *hp;

    int					i,errno;
    int                 prtc_family=SOCK_DGRAM; //SOCK_DGRAM SOCK_STREAM SOCK_RAW
    int                 portnum=3490;
    int                 pc=1, ps=1500, rs, rs_tmp, verbose_mode=1;

    while ((i = getopt (argc, argv, "d:t:p:n:s:v:")) != EOF)
    {
	switch (i)
	{
	    case 'd':
		if  ((hp=gethostbyname (optarg)) == NULL)
		{
			perror("Error get host by name");
			return TFAIL;
		}
		break;
	    case 't':
			if (strcmp(optarg, "UDP") == 0) 
		    	prtc_family=SOCK_DGRAM;
			if (strcmp(optarg, "TCP") == 0)
		    	prtc_family=SOCK_STREAM;
			break;
	    case 'p':
			portnum=atoi(optarg);
			break;
	    case 'n':
			pc=atoi(optarg);
			break;
	    case 's':
			ps=atoi(optarg);
			break;
	    case 'v':
			if (strcmp(optarg,"on") == 0)
				verbose_mode = TRUE;
			if (strcmp(optarg,"off") == 0)
				verbose_mode = FALSE;
			break;
	    default:
			break;
	}
    }    

    if (optind != 13) 
    {  
		fprintf(stderr,"\nUsage: %s -d <dest_hostname> -t <porotocol (UDP|TCP)> -p <dest_port> -n <packets_num> -s <packet_size> -v <verbose_mode (on|off)>\n\n",argv[0]); 
		return TFAIL;
    }   
    if (! (buf=(unsigned char *) malloc (sizeof(unsigned char)*ps)))
    {
		perror("Can't alloc mem");
        return TFAIL;
    }

    if (! (buf_rcv=(unsigned char *) malloc (sizeof(unsigned char)*ps)))
    {
		perror("Can't alloc mem");
		return TFAIL;
    }

//Random fill packets contents
    srand((unsigned int)time((time_t *)NULL));
    i=ps;
    while(i-- > 0) buf[i] =(unsigned char) (random() * 0xFF);

//Fill sockaddr_un
    bzero (&serv_addr, sizeof(serv_addr));
    bcopy (hp->h_addr, &serv_addr.sin_addr, hp->h_length);
    serv_addr.sin_family = hp->h_addrtype;
    serv_addr.sin_port = htons (portnum);
	if (verbose_mode)
	{
	    fprintf (stderr, "Destination addr: %s\n", inet_ntoa (serv_addr.sin_addr));
    	fprintf (stderr, "Destination port: %d\n", portnum);
	    fprintf (stderr, "Packets number: %d\n", pc);
    	fprintf (stderr, "Packet size: %d\n", ps);
	}
//Create socket   
    if ((soc = socket (AF_INET, prtc_family, 0)) == -1)
    {
		perror ("Can't create socket");
		return TFAIL;
    }
    if (prtc_family == SOCK_STREAM) 
    {
		if (connect (soc, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
		{
	    	perror ("Can't create connection");
			return TFAIL;
		}
    }
    while (pc-- > 0)
    {
		switch  (prtc_family) 
		{
//UDP
		    case SOCK_DGRAM:
				if (verbose_mode) 
					printf ("%d. Try send UDP packet\t\t", pc);
				if (sendto(soc, buf, ps, 0, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr))  == -1)
				{
		    		    perror ("Error send message");
				    return TFAIL;
				}
				if (verbose_mode) 
					printf ("Ok\n");
//Wait echo

				if (verbose_mode)
				{
				    printf ("%d. Try recive echo\t\t", pc);
				    fflush(stdout);
				}
				if ((recvfrom(soc, buf_rcv, ps, 0, NULL, 0)) < 0)
				{
				    perror ("Error recive echo");
				    return TFAIL;
				}
				if (verbose_mode) 
				    printf ("Ok\n");
//Check echo                
				if (verbose_mode) 
					printf ("%d. Check echo\t\t\t", pc);
				for (i = 0; i < ps; i++)
				{
				    if (buf_rcv[i] != buf[i])
					{
						perror ("Echo not equal to sending package");
						return TFAIL;
					}
				}
				if (verbose_mode) 
					printf ("Ok\n");
				break;
//TCP
		    case SOCK_STREAM:
				if (verbose_mode)
					printf ("%d. Try send TCP packet\t\t", pc);
				if ((write(soc, buf, ps)) < 0 ) 
				{
				    perror ("Error write to socket");
					return TFAIL;
				}
				if (verbose_mode)
				{
					printf ("Ok\n");
					printf ("%d. Try recive echo\t\t",pc);
					fflush(stdout);
				}
//Read and check echo             
				rs = 0;
				do
				{
				    if ((rs_tmp=read(soc, buf_rcv + rs, ps-rs)) < 0) 
					{
						perror ("Error read echo\n");
						return TFAIL;
					}
					rs_tmp += rs;
				    for (i = rs; i < rs_tmp; i++)
				    {
						if (buf_rcv[i] != buf[i]) 
						{
				   			perror ("Echo not equal to sending package");
							return TFAIL;
						}
				    }
				    rs = rs_tmp;
				}while(rs < ps);

				if (verbose_mode)
					printf ("Ok\n");
				break;
	    	default:
				break;
		}
	}
	return TPASS;
}

int VT_socket_client_test_setup()
{
    return TPASS;
}

int VT_socket_client_test_cleanup()
{
	if (soc)
		close (soc);
	if (buf)
		free (buf);
	if (buf_rcv)
		free (buf_rcv);
	return TPASS;
}
