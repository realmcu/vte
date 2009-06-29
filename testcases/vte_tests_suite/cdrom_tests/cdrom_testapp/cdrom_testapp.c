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
/*================================================================================================*/
/**
        @file   ata_driver_testapp.c

        @brief  Source file for ATA Disk driver test.

Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
A.Ozerov/b00320              10/09/2006     TLSbo76800  Initial version.
D.Kazachkov/b00316            6/12/2006     TLSbo80788  Cosmetic fix


====================================================================================================
Portability: ARM GCC
==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/cdrom.h>
#include <errno.h>


#include <test.h>

/* Verification Test Environment Include Files */
#include "cdrom_testapp.h"

/*==================================================================================================
                                        GLOBAL VARIABLES
===================================================================================================*/
int     fd = 0;


static int open_flags = O_RDONLY | O_NONBLOCK;



/*==================================================================================================
                                        LOCAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== VT_cdrom_driver_test_setup =====*/
/**
@brief  This function assumes the pre-condition of the test case execution

@param  None.

@return On success - return TPASS.
        On failure - return the error code.
*/
/*================================================================================================*/
int VT_cdrom_driver_test_setup(void)
{
        return TPASS;
}

/*================================================================================================*/
/*===== VT_cdrom_driver_test_cleanup =====*/
/**
@brief  This function assumes the post-condition of the test case execution

@param  None.

@return None.
*/
/*================================================================================================*/
int VT_cdrom_driver_test_cleanup(void)
{
        return TPASS;
}

char* audiostatus(int status) 
{
	switch(status) {
	case CDROM_AUDIO_INVALID:  return "Invalid";
	case CDROM_AUDIO_PLAY:	  return "Playing";
	case CDROM_AUDIO_PAUSED:	  return "Paused";
	case CDROM_AUDIO_COMPLETED: return "Completed";
	case CDROM_AUDIO_ERROR:	  return "Error";
	case CDROM_AUDIO_NO_STATUS: return "Stopped";
	defautl:					  return "Unknown";
	 }
}


/*================================================================================================*/
/*===== VT_cdrom_driver_test =====*/
/**
@brief  ATA Disk driver test scenario.

@param  switch_fct
        test case number.

@return On success - return TPASS
        On failure - return the error code.
*/
/*================================================================================================*/
int VT_cdrom_driver_test(char *devname, int testcase,int volume)
{
        int     fd,
                VT_rv = TPASS;
		int track;
		int status;
		
		struct cdrom_volctrl vol;
		
		struct cdrom_tochdr header;
		struct cdrom_ti index;
		//int status;
        int start_min, start_sec, start_frame; //star adderss of play
        int end_min, end_sec, end_frame;	//end address of play 
        struct cdrom_tocentry entry; 	//toc item
        struct cdrom_msf region;
		
		struct cdrom_subchnl channel;

        fd = open(devname, open_flags);
        if (fd < 0)
        {
                perror(devname);
                exit(errno);
        }

        switch (testcase)
        {
        case 0:
                tst_resm(TINFO, "eject CD test\n");
                if (ioctl(fd, CDROMEJECT, 0)<0)
                {
                        tst_resm(TFAIL, "CDROMEJECT failed. Error string: %s", strerror(errno));
                        VT_rv = TFAIL;
                }
				break;	
		case 1:
                tst_resm(TINFO, "pendant of eject CD test\n");
                if (ioctl(fd, CDROMCLOSETRAY, 0)<0)
                {
                        tst_resm(TFAIL, "CDROMCLOSETRAY failed. Error string: %s", strerror(errno));
                        VT_rv = TFAIL;
                }
				break;			
		case 2:
                tst_resm(TINFO,"get start and end tracks");
				if(ioctl(fd,CDROMREADTOCHDR,&header)<0)
				{
						tst_resm(TFAIL, "CDROMREADTOCHDR failed. Error string: %s", strerror(errno));
                    	VT_rv = TFAIL;
				}
				tst_resm(TINFO,"first track :%d, final track:%d",header.cdth_trk0,header.cdth_trk1);
				break;
		case 3:
                tst_resm(TINFO,"get all track's info");
				if(ioctl(fd,CDROMREADTOCHDR,&header)<0)
				{
						tst_resm(TFAIL, "CDROMREADTOCHDR failed. Error string: %s", strerror(errno));
                    	VT_rv = TFAIL;
				}
				else
				{
						tst_resm(TINFO,"first track :%d, final track:%d",header.cdth_trk0,header.cdth_trk1);
						for (track = 1; track <= header.cdth_trk1; track++) 
						{
								entry.cdte_track = track;
								entry.cdte_format = CDROM_MSF; 
								if (ioctl(fd, CDROMREADTOCENTRY, &entry)<0)
								{
					  					tst_resm(TFAIL, "CDROMREADTOCENTRY ioctl failed");
					  					VT_rv = TFAIL;
								}
								else
								{
									tst_resm(TINFO,"Track%02d: %5d %02d:%02d.%03d %c \n", 
						   					track, entry.cdte_track,
						   					entry.cdte_addr.msf.minute,
						   					entry.cdte_addr.msf.second,
						   					entry.cdte_addr.msf.frame,
						   					(entry.cdte_ctrl & CDROM_DATA_TRACK) ? 'D' : 'A');
								}
						}
					
				  
						entry.cdte_track = CDROM_LEADOUT;
						if (ioctl(fd, CDROMREADTOCENTRY, &entry)<0)
						{
								tst_resm(TFAIL, "CDROMREADTOCENTRY ioctl failed");
								VT_rv = TFAIL;
						}
						else
						{
				  			tst_resm(TINFO,"Track leadout: %02d:%02d.%03d \n", 
						 			entry.cdte_addr.msf.minute,
						 			entry.cdte_addr.msf.second,
						 			entry.cdte_addr.msf.frame);
						}
				}
				
                break;
				
		case 4:
                tst_resm(TINFO, "volume control setting");
                tst_resm(TINFO,"Get volume value");
                if (ioctl(fd, CDROMVOLREAD,&vol)<0)
                {
                        tst_resm(TFAIL, "CDROMVOLREAD failed. Error string: %s", strerror(errno));
                        VT_rv = TFAIL;
                }
				
                tst_resm(TINFO,"Channel 0:%d, Channel 1:%d, Channel 2:%d, Channel 3:%d",vol.channel0,vol.channel1,vol.channel2,vol.channel3);
                tst_resm(TINFO,"Set volume value");
				vol.channel0=(char)volume;
				vol.channel1=(char)volume;
				vol.channel2=(char)volume;
				vol.channel3=(char)volume;
				
				if (ioctl(fd,CDROMVOLCTRL,&vol)<0)
				{
					tst_resm(TFAIL, "CDROMVOLCTRL failed. Error string: %s", strerror(errno));
                    VT_rv = TFAIL;	
				}
				else
				{
				    tst_resm(TINFO,"ReGet volume value and check");
					if (ioctl(fd, CDROMVOLREAD,&vol)<0)
					{
						tst_resm(TFAIL, "CDROMVOLREAD failed. Error string: %s", strerror(errno));
						VT_rv = TFAIL;
					}				
					tst_resm(TINFO,"Channel 0:%d, Channel 1:%d, Channel 2:%d, Channel 3:%d",vol.channel0,vol.channel1,vol.channel2,vol.channel3);
				}
				break;

		case 5: 
				tst_resm(TINFO,"Play CD");
				
				//calculate the start address
				  entry.cdte_track = 1;
				  entry.cdte_format = CDROM_MSF; 
				  if(ioctl(fd, CDROMREADTOCENTRY, &entry)!=0)
				  {
					tst_resm(TFAIL, "CDROMREADTOCENTRY failed. Error string: %s", strerror(errno));
					VT_rv = TFAIL;
				  }
				  start_min = entry.cdte_addr.msf.minute;
				  start_sec = entry.cdte_addr.msf.second;
				  start_frame = entry.cdte_addr.msf.frame;
				  //calculate the end address
				  entry.cdte_track = 2;
				  if(ioctl(fd, CDROMREADTOCENTRY, &entry)!=0)
				  {
					tst_resm(TFAIL, "CDROMREADTOCENTRY failed. Error string: %s", strerror(errno));
					VT_rv = TFAIL;
				  }
				  end_min = entry.cdte_addr.msf.minute;
				  end_sec = entry.cdte_addr.msf.second;
				  end_frame = entry.cdte_addr.msf.frame;
				  tst_resm(TINFO,"start min:%d,start sec:%d,start frame:%d,end min:%d,end sec:%d,end frame:%d",start_min,start_sec,start_frame,end_min,end_sec,end_frame);
				  //the play region
				  region.cdmsf_min0 = start_min;
				  region.cdmsf_sec0 = start_sec;
				  region.cdmsf_frame0 = start_frame;
				  region.cdmsf_min1 = end_min;
				  region.cdmsf_sec1 = end_sec;
				  region.cdmsf_frame1 = end_frame;
				  //play CD
				  if(ioctl(fd, CDROMPLAYMSF, &region)<0)
				  {
						tst_resm(TFAIL, "CDROMPLAYMSF failed. Error string: %s", strerror(errno));
						VT_rv = TFAIL;
				  }
				  break;

		case 6:
				tst_resm(TINFO,"Pause CD play");
				if(ioctl(fd,CDROMPAUSE,0)<0)	  // Pause CD play.
				{
					tst_resm(TFAIL, "CDROMPAUSE failed. Error string: %s", strerror(errno));
                    VT_rv = TFAIL;
				}
				break;
		case 7:
				tst_resm(TINFO,"Resume paused CD play");
				if(ioctl(fd,CDROMRESUME,0)<0)	  // Pause CD play.
				{
					tst_resm(TFAIL, "CDROMRESUME failed. Error string: %s", strerror(errno));
                    VT_rv = TFAIL;
				}
				break;
		case 8:
				tst_resm(TINFO,"Stop CD play");
				if(ioctl(fd,CDROMSTOP,0)<0)	  // Stop CD play (can't be RESUMEd)
					{
					tst_resm(TFAIL, "CDROMSTOP failed. Error string: %s", strerror(errno));
                    VT_rv = TFAIL;
				}
				break;
		case 9:
			    tst_resm(TINFO,"read sub channel info");
				//while(1) {
					channel.cdsc_format = CDROM_MSF; /* 采用MSF格式地址 */
					if (ioctl(fd, CDROMSUBCHNL, &channel) != 0)
					{
					  tst_resm(TFAIL,"CDROMSUBCHNL ioctl failed");
					  VT_rv = TFAIL;
					}
					tst_resm(TINFO,"Status: %s \n", audiostatus(channel.cdsc_audiostatus));
					tst_resm(TINFO,"Track: %d \n", channel.cdsc_trk);
					tst_resm(TINFO,"Postion: %02d:%02d:%02d(%02d:%02d:%02d) \n\n",
						   channel.cdsc_reladdr.msf.minute,
						   channel.cdsc_reladdr.msf.second,
						   channel.cdsc_reladdr.msf.frame,
						   channel.cdsc_absaddr.msf.minute,
						   channel.cdsc_absaddr.msf.second,
						   channel.cdsc_absaddr.msf.frame);
					//fflush(stdout);
					//usleep(100000);
				  //}
				break;
        	}

        close(fd);

        return VT_rv;
}
