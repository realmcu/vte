/***
**Copyright (C) 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
        @file   fec_test.h

        @brief  Test scenario C header for fec driver.
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
D.Khoroshev/b00313           10/05/2006     TLSbo76803  Initial version

==================================================================================================
Portability:  ARM GCC
==================================================================================================*/
#ifndef PMIC_TEST_POWER_H
#define PMIC_TEST_POWER_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                         INCLUDE FILES
==================================================================================================*/
#include <sys/stat.h>   /* open() */
#include <fcntl.h>      /* open() */
#include <sys/ioctl.h>  /* ioctl() */
#include <unistd.h>     /* close() */
#include <stdio.h>      /* sscanf() & perror() */
#include <stdlib.h>     /* atoi() */
#include <string.h>
#include <errno.h>
#include <linux/socket.h>
#include <linux/sockios.h>
#include <netinet/in.h>
#include <linux/if.h>
#include <linux/ip.h>
#include <linux/icmp.h>
#include <net/if_arp.h>
#include <net/ethernet.h>
#include <linux/mii.h>
#include <linux/ethtool.h>
#include <time.h>
#include <test.h>

/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/
#if !defined(TRUE) && !defined(FALSE)
#define TRUE  1
#define FALSE 0
#endif

#define VERBOSE(args...) if(gTestConfig.mVerbose) tst_resm(##args...);

#define IPADDR_STRLEN (sizeof "255.255.255.255")
#define HWADDR_STRLEN (sizeof "00:11:22:33:44:55")
/*==================================================================================================
                                             ENUMS
==================================================================================================*/
enum
{
        ICMP_MINLEN = 64,
        DEFDATALEN = 56,
        MAXIPLEN = 60,
        MAXICMPLEN = 76,
        MAXPACKET = 65468,
        MAX_DUP_CHK = (8 * 128),
        MAXWAIT = 10,
        PINGINTERVAL = 1
};

/** Different test cases in the single application */
typedef enum 
{
        T_SHOW_INFO,
        T_PING,
        T_CHECK_ENTRIES,
        T_UP_DOWN,
        T_DOWN,
        T_UP,
        /*ERR_CONFIG_PARAMS,*/
        T_NB
} PMIC_POWER_TESTCASES;

typedef enum
{
        IFSTATUS_UP,
        IFSTATUS_DOWN,
        IFSTATUS_IFF,
        IFSTATUS_ERR
} ifstatus_t;

typedef enum
{
        IF_ETHTOOL = 1,
        IF_MII = 2,
        IF_IFF = 4
} use_if_t;
/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
typedef struct
{
        struct sockaddr sifr_addr;
        struct sockaddr sifr_dstaddr;
        struct sockaddr sifr_broadaddr;
        struct sockaddr sifr_netmask;
        struct sockaddr sifr_hwaddr;
        struct ifmap sifr_map;
        short sifr_flags;
        int sifr_mtu;
        int sifr_metric;
        int sifr_qlen;
        char def_test_addr[IPADDR_STRLEN];
        char def_test_netmask[IPADDR_STRLEN];
        char def_test_hwaddr[HWADDR_STRLEN];
        char def_test_ping_addr[IPADDR_STRLEN];
        int def_test_mtu_size;
        int def_test_metric;
        int def_test_tx_queue_len;
} def_config;

typedef struct 
{
        def_config dc;        
        int mTestCase;
        int mList;
        int mVerbose;
        int useif;
        int avail_if;
        char ifname[32];
        int input_ip;
} sTestConfig; 

/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/
extern sTestConfig gTestConfig;

/*==================================================================================================
                                       LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/
int VT_fec_test_setup(void);
int VT_fec_test_cleanup(void);
int VT_fec_test(void);
void fill_defconfig();

#ifdef __cplusplus
}
#endif

#endif        /* PMIC_TEST_POWER_H */
