/***
**Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
**
**The code contained herein is licensed under the GNU General Public
**License. You may obtain a copy of the GNU General Public License
**Version 2 or later at the following locations:
**
**http://www.opensource.org/licenses/gpl-license.html
**http://www.gnu.org/copyleft/gpl.html
**/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define MAXLINE 1024

PM_main(int argc, char *argv[])
{
    int sockfd, nsockfd;
	int saddrlen, caddrlen, max_caddrlen;
	int pid;
    struct sockaddr_un serv_addr, clnt_addr;
	char buf[MAXLINE]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz", buf_rcv[MAXLINE];

    switch (pid = fork())
	{
	  case -1:
        perror ("Error call fork()");
        return (1);
		break;
//SERVER
	  case 0:
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sun_family = AF_UNIX;
        strcpy(serv_addr.sun_path,"/tmp/echo.server");
        saddrlen=sizeof(serv_addr.sun_family) + strlen(serv_addr.sun_path);

        if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
        {
          perror("client: socket open failure");
          return (1);
        }

  		if ( bind( sockfd, (struct sockaddr *) &serv_addr, saddrlen) < 0)
  		{
    	  printf("server bind failure:\n");
    	  fflush(NULL);
    	  return(1);
  		}

  		if ( listen(sockfd, 5) == -1)
  		{
    	  perror ("Error call listen()");
    	  return (1);
  		}

  		max_caddrlen = sizeof(clnt_addr);
    	caddrlen = max_caddrlen;
    	if ((nsockfd = accept (sockfd, (struct sockaddr *) &clnt_addr, &caddrlen)) == -1)
    	{
      	  perror ("Error call accept()");
      	  return (1);
    	}
		close (sockfd);
      	while (read (nsockfd, buf, sizeof(buf)) != 0)
      	{
      	  write (nsockfd, buf, sizeof(buf));
      	}
      	close (nsockfd);

		unlink("/tmp/echo.server");
      	return (0);
		break;
//CLIENT
	  default:
	    bzero((char *) &serv_addr, sizeof(serv_addr));
  		serv_addr.sun_family = AF_UNIX;
	    strcpy(serv_addr.sun_path,"/tmp/echo.server");
  		saddrlen=sizeof(serv_addr.sun_family) + strlen(serv_addr.sun_path);

	    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
  		{
	      perror("client: socket open failure");
  		  return (1);
	    }
 	sleep (3);
		if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    	{
		  perror("error connect to server");
		  return (1);
    	}
//FILL SOCKET
	    bzero ((char *) &clnt_addr, sizeof (clnt_addr));
  		clnt_addr.sun_family = AF_UNIX;
		strcpy (clnt_addr.sun_path,"/tmp/clnt.XXXX");
		mktemp (clnt_addr.sun_path);
		caddrlen=sizeof(clnt_addr.sun_family) + strlen(clnt_addr.sun_path);
//BIND SOCKET
		if (bind(sockfd, (struct sockaddr *) &clnt_addr, caddrlen) <0)
		{
		  perror ("error call bind()");
	  	  return (1);
		}
//WRITE TO SERVER
  		if ((write(sockfd, buf, sizeof(buf))) < 0 )
	    {
		  perror ("Error write to socket");
		  return (1);
	    }
//READ ECHO
	  	if (read(sockfd, buf_rcv, sizeof(buf_rcv)) < 0)
		{
		  perror ("Error read from socket");
		  return (1);
		}
	  	close(sockfd);
		
		if (strcmp(buf_rcv, buf) == 0)
		  return (0);
		else
		  return (1);
		break;
	}
//server
}
