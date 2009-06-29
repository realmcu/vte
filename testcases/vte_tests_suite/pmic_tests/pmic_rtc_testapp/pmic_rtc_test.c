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
        @file   pmic_rtc_test.c

        @brief  Test scenario of PMIC RTC driver.
====================================================================================================
Revision History:
                            Modification     Tracking
Author/Core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
E.Gromazina/NONE             12/08/2005     TLSbo59968  PMIC RTC draft version.
A.Ozerov/b00320              06/07/2006     TLSbo61903  Test was changed in accordance to driver changes.
Pradeep K/b01016             09/25/2006     TLSboXXXX   Updated to support PMIC API's
D.Simakov                    07/09/2006     TLSbo76178  iMX27 compilation issue
====================================================================================================
Portability: ARM GCC
==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Harness Specific Include Files. */
#include <time.h>
#include <poll.h>

/* Harness Specific Include Files. */
#include <test.h>
#include <usctest.h>

/* Verification Test Environment Include Files */
#include "pmic_rtc_test.h"

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
                                        LOCAL CONSTANTS
==================================================================================================*/
char   *jours[] =
{
        "Sunday", "monday", "tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
};

char   *mois[] =
{
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
};

/*==================================================================================================
                                        LOCAL VARIABLES
==================================================================================================*/
static int fd;
/*==================================================================================================
                                        GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                        GLOBAL VARIABLES
==================================================================================================*/
extern char dev_path[20];

/*==================================================================================================
                                    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
                                        LOCAL FUNCTIONS
==================================================================================================*/
/*================================================================================================*/
/*===== VT_pmic_rtc_test_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_pmic_rtc_test_setup(void)
{
        int     rv = TFAIL;

        /* Open the rtc device 1 */
        fd = open(dev_path, O_RDWR);
        if (fd < 0)
        {
                tst_resm(TFAIL, "Unable to open %s", dev_path);
                return rv;
        }

        rv = TPASS;
        return rv;
}

/*================================================================================================*/
/*===== VT_pmic_rtc_test_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_pmic_rtc_test_cleanup(void)
{
        int     rv = TPASS;

        if (fd > 0)
        {
                if (close(fd))
                {
                        tst_resm(TFAIL, "Unable to close rtc device");
                        rv = TFAIL;
                }
        }
        return rv;
}

/*================================================================================================*/
/*===== VT_pmic_rtc_test =====*/
/**
@brief  PMIC RTC test scenario function

@param

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_pmic_rtc_test(int switch_fct)
{
        int     ret, counter = 1;
        struct timeval pmic_time,
                pmic_time_read;
        time_t  maintenant;
        struct tm *m;
        int     VT_rv = TFAIL;
        struct pollfd fds;

        switch(switch_fct)
        {
        case 0:
                tst_resm(TINFO, "Get time");
                ret = ioctl(fd, PMIC_RTC_GET_TIME, &pmic_time_read);
                if (ret != 0)
                {
                        tst_resm(TFAIL, "Error in rtc test: RTC_GET_TIME");
                        return VT_rv;
                }

                m = localtime(&pmic_time_read.tv_sec);

                tst_resm(TINFO,
                         "%ld seconds between the current time\n\t\t\tand midnight, January 1, 1970 UTC",
                         pmic_time_read.tv_sec);
                tst_resm(TINFO, "The PMIC time is : %02d/%02d/%02d %02d:%02d:%02d", m->tm_mon + 1,
                         m->tm_mday, m->tm_year % 100, m->tm_hour, m->tm_min, m->tm_sec);
                tst_resm(TINFO, "%s, %s %d, %04d %de day of the year", jours[m->tm_wday],
                         mois[m->tm_mon], m->tm_mday, m->tm_year + 1900, m->tm_yday);
                break;
        case 1:
                time(&maintenant);
                pmic_time.tv_sec = maintenant;

                tst_resm(TINFO,
                         "%ld seconds between the current time and midnight, January 1, 1970 UTC",
                         maintenant);

                m = localtime(&maintenant);

                tst_resm(TINFO, "The date and time is : %02d/%02d/%02d %02d:%02d:%02d\n",
                         m->tm_mon + 1, m->tm_mday, m->tm_year % 100, m->tm_hour, m->tm_min,
                         m->tm_sec);
                tst_resm(TINFO, "%s, %s %d, %04d %de day of the year\n", jours[m->tm_wday],
                         mois[m->tm_mon], m->tm_mday, m->tm_year + 1900, m->tm_yday);

                tst_resm(TINFO, "Set  time");
                ret = ioctl(fd, PMIC_RTC_SET_TIME, &pmic_time);
                if (ret != 0)
                {
                        tst_resm(TFAIL, "Error in rtc test: RTC_SET_TIME");
                        return VT_rv;
                }

                tst_resm(TINFO, "Get time");
                ret = ioctl(fd, PMIC_RTC_GET_TIME, &pmic_time_read);
                if (ret != 0)
                {
                        tst_resm(TFAIL, "Error in rtc test: RTC_GET_TIME");
                        return VT_rv;
                }

                tst_resm(TINFO, "Retrun value of time is %ld", pmic_time_read.tv_sec);

                if (pmic_time.tv_sec > pmic_time_read.tv_sec)
                {
                        if ((pmic_time.tv_sec + 5) < pmic_time_read.tv_sec)
                        {
                                tst_resm(TFAIL, "Error in rtc test");
                                return VT_rv;
                        }
                }
                break;
        case 2:
                time(&maintenant);
                pmic_time.tv_sec = maintenant;

                tst_resm(TINFO,
                         "%ld seconds between the current time and midnight, January 1, 1970 UTC",
                         maintenant);

                m = localtime(&maintenant);

                tst_resm(TINFO, "The date and time is : %02d/%02d/%02d %02d:%02d:%02d\n",
                         m->tm_mon + 1, m->tm_mday, m->tm_year % 100, m->tm_hour, m->tm_min,
                         m->tm_sec);
                tst_resm(TINFO, "%s, %s %d, %04d %de day of the year\n", jours[m->tm_wday],
                         mois[m->tm_mon], m->tm_mday, m->tm_year + 1900, m->tm_yday);

                tst_resm(TINFO, "Set  Alarm time");
                ret = ioctl(fd, PMIC_RTC_SET_ALARM, &pmic_time);
                if (ret != 0)
                {
                        tst_resm(TFAIL, "Error in rtc test: RTC_SET_ALARM");
                        return VT_rv;

                }

                tst_resm(TINFO, "Get Alarm time");
                ret = ioctl(fd, PMIC_RTC_GET_ALARM, &pmic_time_read);
                if (ret != 0)
                {
                        tst_resm(TFAIL, "Error in rtc test: RTC_GET_ALARM");
                        return VT_rv;
                }

                tst_resm(TINFO, "Retrun value of Alarm time is %ld\n", pmic_time_read.tv_sec);

                if (pmic_time.tv_sec != pmic_time_read.tv_sec)
                {
                        tst_resm(TFAIL, "Error in rtc test");
                        return VT_rv;
                }
                break;
        case 3:
                time(&maintenant);
                pmic_time.tv_sec = maintenant;

                tst_resm(TINFO, "Test  Alarm event");


                tst_resm(TINFO, "Set  time to local time");
                ret = ioctl(fd, PMIC_RTC_SET_TIME, &pmic_time);
                if (ret != 0)
                {
                        tst_resm(TFAIL, "Error in rtc test: RTC_SET_TIME");
                        return VT_rv;
                }

                tst_resm(TINFO, "Set Alarm time to local time +10 second");
                pmic_time.tv_sec = pmic_time.tv_sec + 10;
                ret = ioctl(fd, PMIC_RTC_SET_ALARM, &pmic_time);
                if (ret != 0)
                {
                        tst_resm(TFAIL, "Error in rtc test: RTC_SET_ALARM");
                        return VT_rv;
                }

                tst_resm(TINFO, "Wait Alarm event...");
                ret = ioctl(fd, PMIC_RTC_WAIT_ALARM, NULL);
                if (ret != 0)
                {
                        tst_resm(TFAIL, "Error in rtc test: RTC_WAIT_ALARM");
                        return VT_rv;
                }

                tst_resm(TINFO, "*** Alarm event DONE ***");
                break;
        case 4:
                time(&maintenant);
                pmic_time.tv_sec = maintenant;

                tst_resm(TINFO, "Test Alarm event");

                tst_resm(TINFO, "Set time to local time");
                ret = ioctl(fd, PMIC_RTC_SET_TIME, &pmic_time);
                if (ret != 0)
                {
                        tst_resm(TFAIL, "Error in rtc test: RTC_SET_TIME");
                        return VT_rv;
                }

                tst_resm(TINFO, "Set Alarm time to local time +10 second");
                pmic_time.tv_sec = pmic_time.tv_sec + 10;
                ret = ioctl(fd, PMIC_RTC_SET_ALARM, &pmic_time);
                if (ret != 0)
                {
                        tst_resm(TFAIL, "Error in rtc test: RTC_SET_ALARM");
                        return VT_rv;
                }

                fds.fd = fd;
                fds.events = POLLIN;
                fds.revents = 0;

                tst_resm(TINFO, "Call poll function before register");
                ret = poll(&fds, 1, 0);
                tst_resm(TINFO, "Poll ret = %d", ret);
                tst_resm(TINFO, "Register Alarm");
                ret = ioctl(fd, PMIC_RTC_ALARM_REGISTER, 0);
                if (ret != 0)
                {
                        tst_resm(TFAIL, "Error in rtc test: RTC_ALARM_REGISTER");
                        return VT_rv;
                }

                tst_resm(TINFO, "Call poll until alarm");

                do
                {
                        ret = poll(&fds, 1, 0);
                        tst_resm(TINFO, "Poll ret = %d", ret);
                        counter++;
                }
                while (ret == 0);
                printf("\n");
                tst_resm(TINFO, "Poll returns 0: %d times", counter);

                tst_resm(TINFO, "Unregister Alarm\n");
                ret = ioctl(fd, PMIC_RTC_ALARM_UNREGISTER, 0);
                if (ret != 0)
                {
                        tst_resm(TFAIL, "Error in rtc test: RTC_ALARM_UNREGISTER");
                        return VT_rv;
                }
                break;
        default:
                tst_resm(TFAIL, "Unknown test case number!");
                return TFAIL;
        }

        return TPASS;
}
