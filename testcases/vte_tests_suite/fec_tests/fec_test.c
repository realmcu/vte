/***
**Copyright 2005-2010 Freescale Semiconductor, Inc. All Rights Reserved.
**
**The code contained herein is licensed under the GNU General Public
**License. You may obtain a copy of the GNU General Public License
**Version 2 or later at the following locations:
**
**http://www.opensource.org/licenses/gpl-license.html
**http://www.gnu.org/copyleft/gpl.html
**/
/*===========================================================================*/
/**
        @file   fec_test.c

        @brief  Test scenario C source for fec driver test.
===============================================================================
Revision History:
                   Modification     Tracking
Author/core ID         Date          Number    Description of Changes
---------------   ------------    ----------  ---------------------------------
D.Khoroshev/b00313 10/05/2006     TLSbo76803  Initial version
S.Zhang/b17931     14/01/2010     ENGR119014  FEC driver change

===============================================================================
Portability:  ARM GCC
=============================================================================*/

/*=============================================================================
                               INCLUDE FILES
=============================================================================*/
/* Verification Test Environment Include Files */
#include "fec_test.h"
#include <ctype.h>

/*=============================================================================
                               LOCAL MACROS
=============================================================================*/
#define        SOCKET_AF(af)        (((af) == AF_UNSPEC) ? AF_INET : (af))

/*============================================================================
                              GLOBAL VARIABLES
=============================================================================*/
int sock = -1;
int af = AF_UNSPEC;
char *hostname = NULL;
extern char *TCID;

static struct if_name_t {
        short id;
        char *name;
} ifflag_names [] = {
/* Standard interface flags (netdevice->flags). */
        {IFF_UP, "IFF_UP"},
        {IFF_BROADCAST, "IFF_BROADCAST"},
        {IFF_DEBUG, "IFF_DEBUG"},
        {IFF_LOOPBACK, "IFF_LOOPBACK"},
        {IFF_POINTOPOINT, "IFF_POINTOPOINT"},
        {IFF_NOTRAILERS, "IFF_NOTRAILERS"},
        {IFF_RUNNING, "IFF_RUNNING"},
        {IFF_NOARP, "IFF_NOARP"},
        {IFF_PROMISC, "IFF_PROMISC"},
        {IFF_ALLMULTI, "IFF_ALLMULTI"},

        {IFF_MASTER, "IFF_MASTER"},
        {IFF_SLAVE, "IFF_SLAVE"},

        {IFF_MULTICAST, "IFF_MULTICAST"},

        {IFF_VOLATILE, "IFF_VOLATILE"},

        {IFF_PORTSEL, "IFF_PORTSEL"},
        {IFF_AUTOMEDIA, "IFF_AUTOMEDIA"},
        {IFF_DYNAMIC, "IFF_DYNAMIC"}
};

#ifdef DEBUG
static struct if_name_t if_priv_flag_names [] = {
/* Private (from user) interface flags (netdevice->priv_flags). */
{       
        {IFF_802_1Q_VLAN, "IFF_802_1Q_VLAN"},
        {IFF_EBRIDGE, "IFF_EBRIDGE"},

        {IF_GET_IFACE, "IF_GET_IFACE"},
        {IF_GET_PROTO, "IF_GET_PROTO"},

/* For definitions see hdlc.h */
        {IF_IFACE_V35, "IF_IFACE_V35"},
        {IF_IFACE_V24, "IF_IFACE_V24"},
        {IF_IFACE_X21, "IF_IFACE_X21"},
        {IF_IFACE_T1, "IF_IFACE_T1"},
        {IF_IFACE_E1, "IF_IFACE_E1"},
        {IF_IFACE_SYNC_SERIAL, "IF_IFACE_SYNC_SERIAL"},
        {IF_IFACE_X21D, "IF_IFACE_X21D"},
/* For definitions see hdlc.h */
        {IF_PROTO_HDLC, "IF_PROTO_HDLC"},
        {IF_PROTO_PPP, "IF_PROTO_PPP"},
        {IF_PROTO_CISCO, "IF_PROTO_CISCO"},
        {IF_PROTO_FR, "IF_PROTO_FR"},
        {IF_PROTO_FR_ADD_PVC, "IF_PROTO_FR_ADD_PVC"},
        {IF_PROTO_FR_DEL_PVC, "IF_PROTO_FR_DEL_PVC"},
        {IF_PROTO_X25, "IF_PROTO_X25"},
        {IF_PROTO_HDLC_ETH, "IF_PROTO_HDLC_ETH"},
        {IF_PROTO_FR_ADD_ETH_PVC, "IF_PROTO_FR_ADD_ETH_PVC"},
        {IF_PROTO_FR_DEL_ETH_PVC, "IF_PROTO_FR_DEL_ETH_PVC"},
        {IF_PROTO_FR_PVC, "IF_PROTO_FR_PVC"},
        {IF_PROTO_FR_ETH_PVC, "IF_PROTO_FR_ETH_PVC"},
        {IF_PROTO_RAW, "IF_PROTO_RAW"}
};
#endif /* DEBUG */

static struct if_name_t af_names[] = {
/* Supported address families. */
        {AF_UNSPEC, "AF_UNSPEC"},
        {AF_UNIX, "AF_UNIX"},        
        /* Unix domain sockets                 */
        {AF_LOCAL, "AF_LOCAL"},
        /* POSIX name for AF_UNIX        */
        {AF_INET, "AF_INET"},
        /* Internet IP Protocol         */
        {AF_AX25, "AF_AX25"},
        /* Amateur Radio AX.25                 */
        {AF_IPX, "AF_IPX"},
        /* Novell IPX                         */
        {AF_APPLETALK, "AF_APPLETALK"},
        /* AppleTalk DDP                 */
        {AF_NETROM, "AF_NETROM"},
        /* Amateur Radio NET/ROM         */
        {AF_BRIDGE, "AF_BRIDGE"},
        /* Multiprotocol bridge         */
        {AF_ATMPVC, "AF_ATMPVC"},
        /* ATM PVCs                        */
        {AF_X25, "AF_X25"},        
        /* Reserved for X.25 project         */
        {AF_INET6, "AF_INET6"},
        /* IP version 6                        */
        {AF_ROSE, "AF_ROSE"},        
        /* Amateur Radio X.25 PLP        */
        {AF_DECnet, "AF_DECnet"},
        /* Reserved for DECnet project        */
        {AF_NETBEUI, "AF_NETBEUI"},
        /* Reserved for 802.2LLC project*/
        {AF_SECURITY, "AF_SECURITY"},
        /* Security callback pseudo AF */
        {AF_KEY, "AF_KEY"},        
      /* PF_KEY key management API */
        {AF_NETLINK, "AF_NETLINK"},

        {AF_ROUTE, "AF_ROUTE"},
        /* Alias to emulate 4.4BSD */
        {AF_PACKET, "AF_PACKET"},
        /* Packet family                */
        {AF_ASH, "AF_ASH"},        
        /* Ash                                */
        {AF_ECONET, "AF_ECONET"},
        /* Acorn Econet                        */
        {AF_ATMSVC, "AF_ATMSVC"},
        /* ATM SVCs                        */
        {AF_SNA, "AF_SNA"},        
        /* Linux SNA Project (nutters!) */
        {AF_IRDA, "AF_IRDA"},
        /* IRDA sockets                        */
        {AF_PPPOX, "AF_PPPOX"},
        /* PPPoX sockets                */
        {AF_WANPIPE, "AF_WANPIPE"},
        /* Wanpipe API Sockets */
        {26/*AF_LLC*/, "AF_LLC"},        
        /* Linux LLC                        */
        {AF_BLUETOOTH, "AF_BLUETOOTH"},
        /* Bluetooth sockets                 */
        {AF_MAX, "AF_MAX"}
        /* For now...                   */
};

/*============================================================================
                        LOCAL FUNCTIONS PROTOTYPES 
=============================================================================*/
int  fec_test_on(void);
int  fec_test_off(void);
int  fec_test_config(void);
int  check_entries(void);

int fec_get_info(void);
int print_if_info(void);
int print_ifmap_info(void);
void print_ifflags_info(short ifflags);

char *inet_ntoa(const struct in_addr in);
char *inet_ntoha(const char *sa_data);
int inet_aton(const char *cp, struct in_addr *inp);
int inet_haton(const char *cp, char *sa_data);

int store_config(void);
int print_config(void);
int interface_up(void);
int interface_down(void);
ifstatus_t detect_beat_iff(void);
ifstatus_t detect_beat_ethtool(void);
ifstatus_t detect_beat_mii(void);

/* ping */
int in_cksum(unsigned short *buf, int sz);
int ping(const char *hostname);
void VT_noresp_handler(int arg);
int VT_ask_user(char *msg);
/*=============================================================================
                               LOCAL FUNCTIONS
=============================================================================*/

/*===== VT_fec_test_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
int VT_fec_test_setup(void)
{
        int rv = TPASS;

        sock = socket(SOCKET_AF(af), SOCK_DGRAM, 0);
        
        if (sock < 0)
        {
                tst_resm(TFAIL, "VT_fec_test_setup() : Failed to create socket. Returned error '%s'", strerror(errno));
                rv = TFAIL;
        }
        
        if ( store_config() )
        {
                tst_resm(TWARN, "Cannot save configuration for %s", gTestConfig.ifname);
        }
#ifdef DEBUG
        print_config();
#endif
        /* Test available interfaces */
        gTestConfig.avail_if = IF_IFF;
        if (detect_beat_ethtool() != IFSTATUS_ERR)
                gTestConfig.avail_if |= IF_ETHTOOL;

        if (detect_beat_mii() != IFSTATUS_ERR)
                gTestConfig.avail_if |= IF_MII;

        return rv;
}

/*===== VT_fec_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
int VT_fec_test_cleanup(void)
{
        if (sock >= 0)
                close(sock);
        return TPASS;
}

/*===== VT_fec_test =====*/
/**
@brief  Pmic power test scenario  function

@param

@return On success - return TPASS
        On failure - return TFAIL
*/
int VT_fec_test(void)
{
        int VT_rv = TPASS;
        switch(gTestConfig.mTestCase)
        {
                case T_SHOW_INFO:
                        tst_resm(TINFO, "Test case %d: Show information on interface %s", gTestConfig.mTestCase, gTestConfig.ifname);
                        fec_get_info();

                        tst_resm(TINFO, "Supported control interfaces for %s", gTestConfig.ifname);
                        tst_resm(TINFO, "\tMII: %s", gTestConfig.avail_if & IF_MII ? "yes" : "no");
                        tst_resm(TINFO, "\tEthtool: %s", gTestConfig.avail_if & IF_ETHTOOL ? "yes" : "no");
                        VT_rv = VT_ask_user("\n\tIs this information correct(y|n|q)?");
                break;
                
                case T_CHECK_ENTRIES:
                        tst_resm(TINFO, "Test case %d: Check entries", gTestConfig.mTestCase);
                        VT_rv = check_entries();

                break;
                
                case T_PING:
                        tst_resm(TINFO, "Test case %d: Send ping", gTestConfig.mTestCase);
                        if (gTestConfig.input_ip)
                        {
                                char addr[16];
                                struct in_addr in;
                                tst_resm(TINFO, "Input ip address (xxx.xxx.xxx.xxx) or null string to skip test:");
                                do
                                {
                                        fgets(addr, 16, stdin);        
                                        if ( *addr == '\n' || *addr == '\r' )
                                        {
                                                tst_resm(TINFO, "Skipping test");
                                                return TFAIL;
                                        }
                                } while(!inet_aton(addr, &in));
                                memcpy(gTestConfig.dc.def_test_ping_addr, addr, IPADDR_STRLEN);        
                        }
                
                        tst_resm(TINFO, "pinging %s ...", gTestConfig.dc.def_test_ping_addr);        
                        VT_rv = ping(gTestConfig.dc.def_test_ping_addr);
                break;
                
                case T_UP_DOWN:
                        tst_resm(TINFO, "Test case %d: Shut down and rise up %s interface ",
                                        gTestConfig.mTestCase, gTestConfig.ifname);
                        interface_down();
                        if ((VT_rv = interface_up()) != TPASS)
                                if (gTestConfig.mVerbose)
                                        tst_resm(TWARN, "Can not rise up %s interface", gTestConfig.ifname);
                break;
                
                case T_UP:
                        tst_resm(TINFO, "Test case %d: Rise up %s interface", gTestConfig.mTestCase);
                        if ((VT_rv = interface_up()) != TPASS)
                                if (gTestConfig.mVerbose)
                                        tst_resm(TWARN, "Can not rise up %s interface", gTestConfig.ifname);
                break;

                case T_DOWN:
                        tst_resm(TINFO, "Test case %d: Shut down %s interface", gTestConfig.mTestCase);
                        if ((VT_rv = interface_down()) != TPASS)
                                if (gTestConfig.mVerbose)
                                        tst_resm(TWARN, "Can not shut down %s interface", gTestConfig.ifname);
                break;

                default:
                        tst_resm(TBROK , "Error: Unknown test case");
                        return TBROK;
        }

        return VT_rv;
}

/*===== fec_get_info =====*/
/**
@brief  Display information about fec controller

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
int fec_get_info(void)
{
        struct ifreq ifr;
        int goterr = 0;
        
        memset(&ifr, 0, sizeof(struct ifreq));
        
        strncpy(ifr.ifr_name, gTestConfig.ifname, sizeof(ifr.ifr_name));

        if (ioctl(sock, SIOCGIFFLAGS, &ifr) < 0) {
                perror("SIOCGIFFLAGS");
                ++goterr;
        }

        print_ifflags_info(ifr.ifr_flags);
        if (ioctl(sock, SIOCGIFFLAGS, &ifr) < 0)
        {
                tst_resm(TWARN, "ioctl SIOCGIFFLAGS: %s", strerror(errno));
                ++goterr;
        }      

        print_if_info();
        print_ifmap_info();

        return TPASS;
}

/*===== c_flagname =====*/
/**
@brief  Returns flag name by it's value.

@param  flag      - flag

@return On success - returns pointer to string containing flag name
        On failure - returns NULL
*/
char *c_flagname(short flag)
{
        int i, nb_flags = sizeof(ifflag_names) / sizeof(struct if_name_t);
        if (flag > (short)IFF_DYNAMIC)
                return NULL;
        
        for (i = 0; i < nb_flags; ++i)
        {
                if (flag == ifflag_names[i].id)
                        return ifflag_names[i].name;
        }
        return NULL;
}

/*===== c_afname =====*/
/**
@brief  Returns flag name by it's value.

@param  afname      - flag

@return On success - returns pointer to string containing flag name
        On failure - returns NULL
*/
char *c_afname(sa_family_t af)
{
        int i, l;
        if (af > (short)AF_MAX)
                return NULL;
        
        l = 0;
        i = 16;
        do {
                if (af >= af_names[l+i].id)
                        l += i;
                i >>= 1;
        } while (i != 0 && af != af_names[l].id);
        if (af_names[l].id == af)
        printf("%s: l: %d\n", __FUNCTION__, l);
                return af_names[l].name;
        return NULL;
}

/*===== print_if_info =====*/
/**
@brief  print common info about interface.

@param  none

@return On success - return TPASS
        On failure - return the error code
*/
int print_if_info(void)
{
        int goterror = 0;
        struct ifreq ifr;
        struct in_addr in;

        tst_resm(TINFO, "Interface info");        

        memset(&ifr, 0, sizeof(struct ifreq));
        strncpy(ifr.ifr_name, gTestConfig.ifname, sizeof(ifr.ifr_name));

        if (ioctl(sock, SIOCGIFADDR, &ifr) == -1)
        {
                tst_resm(TWARN, "ioctl SIOCGIFADDR: %s", strerror(errno));
                goterror++;
        }       
        in = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr;
        tst_resm(TINFO, "\taddress: %s", inet_ntoa(in));

        if (ioctl(sock, SIOCGIFDSTADDR, &ifr) == -1)
        {
                tst_resm(TWARN, "ioctl SIOCGIFDSTADDR: %s", strerror(errno));
                goterror++;
        }       
        in = ((struct sockaddr_in *)&ifr.ifr_dstaddr)->sin_addr;
        tst_resm(TINFO, "\tdest address: %s", inet_ntoa(in));
        
        if (ioctl(sock, SIOCGIFBRDADDR, &ifr) == -1)
        {
                tst_resm(TWARN, "ioctl SIOCGIFBRDADDR: %s", strerror(errno));
                goterror++;
        }       
        in = ((struct sockaddr_in *)&ifr.ifr_broadaddr)->sin_addr;
        tst_resm(TINFO, "\tbroadcast address: %s", inet_ntoa(in));

        if (ioctl(sock, SIOCGIFNETMASK, &ifr) == -1)
        {
                tst_resm(TWARN, "ioctl SIOCGIFBRDADDR: %s", strerror(errno));
                goterror++;
        }       
        in = ((struct sockaddr_in *)&ifr.ifr_netmask)->sin_addr;
        tst_resm(TINFO, "\tnetmask: %s", inet_ntoa(in));

        if (ioctl(sock, SIOCGIFHWADDR, &ifr) == -1)
        {
                tst_resm(TWARN, "ioctl SIOCGIFHWADDR: %s", strerror(errno));
                goterror++;
        }       
        tst_resm(TINFO, "\thardware address: %s", inet_ntoha(ifr.ifr_hwaddr.sa_data));

        if (ioctl(sock, SIOCGIFMTU, &ifr) == -1)
        {
                tst_resm(TWARN, "ioctl SIOCGIFMTU: %s", strerror(errno));
                goterror++;
        }
        tst_resm(TINFO, "\tMTU: %d", ifr.ifr_mtu);
        
        if (ioctl(sock, SIOCGIFMETRIC, &ifr) == -1)
        {
                tst_resm(TWARN, "ioctl SIOCGIFMETRIC: %s", strerror(errno));
                goterror++;
        }
        tst_resm(TINFO, "\tmetric: %d", ifr.ifr_metric);
        
        if (ioctl(sock, SIOCGIFTXQLEN, &ifr) == -1)
        {
                tst_resm(TWARN, "ioctl SIOCGIFTXQLEN: %s", strerror(errno));
                goterror++;
        }
        tst_resm(TINFO, "\tTX queue length: %d", ifr.ifr_qlen);
        

        return goterror ? TFAIL : TPASS;
}


/*===== check_entries =====*/
/**
@brief  Change main parameters of ethernet interface such as ip address, broadcast address, netmask,
        hardware address, MTU size, etc.

@param  none

@return On success - return TPASS
        On failure - return the error code
*/
int check_entries(void)
{
        int goterror = 0;
        struct ifreq ifr;
        struct in_addr *in, in1;
        struct sockaddr sa;
        int mtu, qlen;
#ifdef DEBUG
        int metric;
#endif

        memset(&in, 0, sizeof(struct in_addr));
        memset(&sa, 0, sizeof(struct sockaddr));
        memset(&ifr, 0, sizeof(struct ifreq));
        strncpy(ifr.ifr_name, gTestConfig.ifname, sizeof(ifr.ifr_name));

        /* Change ip address */
        if (gTestConfig.mVerbose)
                tst_resm(TINFO, "Change ip address");
        if (ioctl(sock, SIOCGIFADDR, &ifr) == -1)
        {
                tst_resm(TWARN, "ioctl SIOCGIFADDR: %s", strerror(errno));
                goterror++;
        }
        else
        {
                in = &((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr;
                if (gTestConfig.mVerbose)
                {
                        tst_resm(TINFO, "\t+address: %s", inet_ntoa(*in));
                }
                memcpy(&sa, &ifr.ifr_addr, sizeof(struct sockaddr));
                inet_aton(gTestConfig.dc.def_test_addr, in);
                memcpy(&in1, in, sizeof (struct in_addr));
                if (ioctl(sock, SIOCSIFADDR, &ifr) == -1)
                {        
                        tst_resm(TWARN, "ioctl SIOCSIFADDR: %s", strerror(errno));
                        goterror++;
                }
                else
                {
                        if (gTestConfig.mVerbose)
                                tst_resm(TINFO, "\t+set address to %s", inet_ntoa(*in));
                        memset(&ifr.ifr_addr, 0, sizeof(struct sockaddr));
                        if (ioctl(sock, SIOCGIFADDR, &ifr) == -1)
                        {
                                tst_resm(TWARN, "ioctl SIOCGIFADDR: %s", strerror(errno));
                                goterror++;
                        }
                        if (memcmp(&((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr,
                                                        &in1, sizeof (struct in_addr)) != 0)
                        {                
                                if (gTestConfig.mVerbose)
                                        tst_resm(TWARN, "\t-target address was not set (%s)", inet_ntoa(*in));
                                goterror++;
                        }
                        else                
                                if (gTestConfig.mVerbose)
                                        tst_resm(TINFO, "\t+address was successfully set to %s", inet_ntoa(*in));

                        memcpy(&ifr.ifr_addr, &sa, sizeof (struct sockaddr));                        
                        if (ioctl(sock, SIOCSIFADDR, &ifr) == -1)
                        {        
                                tst_resm(TWARN, "ioctl SIOCSIFADDR: %s", strerror(errno));
                                goterror++;
                               }
                        else
                        {
                                if (gTestConfig.mVerbose)
                                        tst_resm(TINFO, "\t+return address to original (%s)", inet_ntoa(*in));
                        }

                }
        }

        /* Change dst address */                
        if (gTestConfig.mVerbose)
                tst_resm(TINFO, "Change dst adress");
        if (ioctl(sock, SIOCGIFDSTADDR, &ifr) == -1)
        {
                tst_resm(TWARN, "ioctl SIOCGIFDSTADDR: %s", strerror(errno));
                goterror++;
        }
        else
        {
                in = &((struct sockaddr_in *)&ifr.ifr_dstaddr)->sin_addr;
                if (gTestConfig.mVerbose)
                {
                        tst_resm(TINFO, "\t+dst address: %s", inet_ntoa(*in));
                }
                memcpy(&sa, &ifr.ifr_dstaddr, sizeof(struct sockaddr));
                inet_aton(gTestConfig.dc.def_test_addr, in);
                memcpy(&in1, in, sizeof (struct in_addr));
                if (ioctl(sock, SIOCSIFDSTADDR, &ifr) == -1)
                {        
                        tst_resm(TWARN, "ioctl SIOCSIFDSTADDR: %s", strerror(errno));
                        goterror++;
                }
                else
                {
                        if (gTestConfig.mVerbose)
                                tst_resm(TINFO, "\t+set dst address to %s", inet_ntoa(*in));
                        memset(&ifr.ifr_dstaddr, 0, sizeof(struct sockaddr));
                        if (ioctl(sock, SIOCGIFDSTADDR, &ifr) == -1)
                        {
                                tst_resm(TWARN, "ioctl SIOCGIFDSTADDR: %s", strerror(errno));
                                goterror++;
                        }
                        if (memcmp(&((struct sockaddr_in*)&ifr.ifr_dstaddr)->sin_addr,
                                                        &in1, sizeof (struct in_addr)) != 0)
                        {                
                                if (gTestConfig.mVerbose)
                                        tst_resm(TWARN, "\t-target dst address was not set (%s)", inet_ntoa(*in));
                                goterror++;
                        }
                        else                
                                if (gTestConfig.mVerbose)
                                        tst_resm(TINFO, "\t+dst address was successfully set to %s",
                                                        inet_ntoa(*in));

                        memcpy(&ifr.ifr_dstaddr, &sa, sizeof (struct sockaddr));                        
                        if (ioctl(sock, SIOCSIFDSTADDR, &ifr) == -1)
                        {        
                                tst_resm(TWARN, "ioctl SIOCSIFDSTADDR: %s", strerror(errno));
                                goterror++;
                               }
                        else
                        {
                                if (gTestConfig.mVerbose)
                                        tst_resm(TINFO, "\t+return dst address to original (%s)", inet_ntoa(*in));
                        }
                }
        }

        /* Change broadcast address */
        if (gTestConfig.mVerbose)
                tst_resm(TINFO, "Change broadcast adress");
        if (ioctl(sock, SIOCGIFBRDADDR, &ifr) == -1)
        {
                tst_resm(TWARN, "ioctl SIOCGIFBRDADDR: %s", strerror(errno));
                goterror++;
        }
        else
        {
                in = &((struct sockaddr_in *)&ifr.ifr_broadaddr)->sin_addr;
                if (gTestConfig.mVerbose)
                {
                        tst_resm(TINFO, "\t+broad address: %s", inet_ntoa(*in));
                }
                memcpy(&sa, &ifr.ifr_broadaddr, sizeof(struct sockaddr));
                inet_aton(gTestConfig.dc.def_test_addr, in);
                memcpy(&in1, in, sizeof (struct in_addr));
                if (ioctl(sock, SIOCSIFBRDADDR, &ifr) == -1)
                {        
                        tst_resm(TWARN, "ioctl SIOCSIFBRDADDR: %s", strerror(errno));
                        goterror++;
                }
                else
                {
                        if (gTestConfig.mVerbose)
                                tst_resm(TINFO, "\t+set broad address to %s", inet_ntoa(*in));
                        memset(&ifr.ifr_broadaddr, 0, sizeof(struct sockaddr));
                        if (ioctl(sock, SIOCGIFBRDADDR, &ifr) == -1)
                        {
                                tst_resm(TWARN, "ioctl SIOCGIFBRDADDR: %s", strerror(errno));
                                goterror++;
                        }
                        if (memcmp(&((struct sockaddr_in*)&ifr.ifr_broadaddr)->sin_addr,
                                                        &in1, sizeof (struct in_addr)) != 0)
                        {                
                                if (gTestConfig.mVerbose)
                                        tst_resm(TWARN, "\t-target broad address was not set (%s)", inet_ntoa(*in));
                                goterror++;
                        }
                        else                
                                if (gTestConfig.mVerbose)
                                        tst_resm(TINFO, "\t+broad address was successfully set to %s",
                                                        inet_ntoa(*in));

                        memcpy(&ifr.ifr_broadaddr, &sa, sizeof (struct sockaddr));                        
                        if (ioctl(sock, SIOCSIFBRDADDR, &ifr) == -1)
                        {        
                                tst_resm(TWARN, "ioctl SIOCSIFBRDADDR: %s", strerror(errno));
                                goterror++;
                               }
                        else
                        {
                                if (gTestConfig.mVerbose)
                                        tst_resm(TINFO, "\t+return broad address to original (%s)",
                                                        inet_ntoa(*in));
                        }
                }
        }
        
        /* Change netmask */
        if (gTestConfig.mVerbose)
                tst_resm(TINFO, "Change netmask");
        if (ioctl(sock, SIOCGIFNETMASK, &ifr) == -1)
        {
                tst_resm(TWARN, "ioctl SIOCGIFNETMASK: %s", strerror(errno));
                goterror++;
        }
        else
        {
                in = &((struct sockaddr_in *)&ifr.ifr_netmask)->sin_addr;
                if (gTestConfig.mVerbose)
                {
                        tst_resm(TINFO, "\t+netmask: %s", inet_ntoa(*in));
                }
                memcpy(&sa, &ifr.ifr_netmask, sizeof(struct sockaddr));
                inet_aton(gTestConfig.dc.def_test_netmask, in);
                memcpy(&in1, in, sizeof (struct in_addr));
                if (ioctl(sock, SIOCSIFNETMASK, &ifr) == -1)
                {        
                        tst_resm(TWARN, "ioctl SIOCSIFNETMASK: %s", strerror(errno));
                        goterror++;
                }
                else
                {
                        if (gTestConfig.mVerbose)
                                tst_resm(TINFO, "\t+set netmask to %s", inet_ntoa(*in));
                        memset(&ifr.ifr_netmask, 0, sizeof(struct sockaddr));
                        if (ioctl(sock, SIOCGIFNETMASK, &ifr) == -1)
                        {
                                tst_resm(TWARN, "ioctl SIOCGIFNETMASK: %s", strerror(errno));
                                goterror++;
                        }
                        if (memcmp(&((struct sockaddr_in*)&ifr.ifr_netmask)->sin_addr,
                                                        &in1, sizeof (struct in_addr)) != 0)
                        {                
                                if (gTestConfig.mVerbose)
                                        tst_resm(TWARN, "\t-netmask was not set (%s)", inet_ntoa(*in));
                                goterror++;
                        }
                        else                
                                if (gTestConfig.mVerbose)
                                        tst_resm(TINFO, "\t+netmask was successfully set to %s", inet_ntoa(*in));

                        memcpy(&ifr.ifr_netmask, &sa, sizeof (struct sockaddr));                        
                        if (ioctl(sock, SIOCSIFNETMASK, &ifr) == -1)
                        {        
                                tst_resm(TWARN, "ioctl SIOCSIFNETMASK: %s", strerror(errno));
                                goterror++;
                               }
                        else
                        {
                                if (gTestConfig.mVerbose)
                                        tst_resm(TINFO, "\t+return netmask to original (%s)", inet_ntoa(*in));
                        }
                }
        }
        
        /* Change hardware address */
        if (gTestConfig.mVerbose)
                tst_resm(TINFO, "Change hardware address");
        if (ioctl(sock, SIOCGIFHWADDR, &ifr) == -1)
        {
                tst_resm(TWARN, "ioctl SIOCGIFHWADDR: %s", strerror(errno));
                goterror++;
        }
        else
        {                
                char *hwa, hwa_backup[6], hwa_test[6];
                memset(hwa_backup, 0, sizeof hwa_backup);
                memset(hwa_test, 0, sizeof hwa_backup);
                hwa = ifr.ifr_hwaddr.sa_data;
                if (gTestConfig.mVerbose)
                {
                        tst_resm(TINFO, "\t+hardware address: %s", inet_ntoha(hwa));
                }
                memcpy(hwa_backup, hwa, sizeof hwa_backup);
                inet_haton(gTestConfig.dc.def_test_hwaddr, hwa_test);
                memcpy(hwa, hwa_test, sizeof hwa_test);
                if (gTestConfig.mVerbose)
                        tst_resm(TINFO, "\t+shutting down %s interface!", gTestConfig.ifname);
                /* Spring: FEC driver changed, clocks will be disabled if down eth0 
                 * ENGR00118714, ENGR00119014
                 * */
                /* interface_down(); */
                if (ioctl(sock, SIOCSIFHWADDR, &ifr) == -1)
                {        
                        tst_resm(TWARN, "ioctl SIOCSIFHWADDR: %s", strerror(errno));
                        goterror++;
                }
                else
                {
                        if (gTestConfig.mVerbose)
                                tst_resm(TINFO, "\t+set hardware address to %s", inet_ntoha(hwa));
                        memset(&ifr.ifr_hwaddr, 0, sizeof(struct sockaddr));
                        if (ioctl(sock, SIOCGIFHWADDR, &ifr) == -1)
                        {
                                tst_resm(TWARN, "ioctl SIOCGIFHWADDR: %s", strerror(errno));
                                goterror++;
                        }

                        if (memcmp(&ifr.ifr_hwaddr.sa_data, hwa_test, sizeof hwa_test) != 0)
                        {                
                                if (gTestConfig.mVerbose)
                                        tst_resm(TWARN, "\t-hardware address was not set (%s)", inet_ntoha(hwa));
                                goterror++;
                        }
                        else                
                                if (gTestConfig.mVerbose)
                                        tst_resm(TINFO, "\t+hardware address was successfully set to %s",
                                                        inet_ntoha(hwa));

                        
                        memcpy(&ifr.ifr_hwaddr.sa_data, hwa_backup, sizeof hwa_backup);
                        if (ioctl(sock, SIOCSIFHWADDR, &ifr) == -1)
                        {        
                                tst_resm(TWARN, "ioctl SIOCSIFHWADDR: %s", strerror(errno));
                                goterror++;
                               }
                        else
                        {
                                if (gTestConfig.mVerbose)
                                        tst_resm(TINFO, "\t+return hardware address to original (%s)",
                                                        inet_ntoha(hwa));
                        }
                }
                if (!interface_up() && gTestConfig.mVerbose)
                        tst_resm(TINFO, "\t+set up %s interface", gTestConfig.ifname);
        }

        /* Change MTU size */
        if (gTestConfig.mVerbose)
                tst_resm(TINFO, "Change MTU size");
        if (ioctl(sock, SIOCGIFMTU, &ifr) == -1)
        {
                tst_resm(TWARN, "ioctl SIOCGIFMTU: %s", strerror(errno));
                goterror++;
        }
        else
        {
                mtu = ifr.ifr_mtu;
                if (gTestConfig.mVerbose)
                {
                        tst_resm(TINFO, "\t+MTU size: %d", mtu);
                }
                ifr.ifr_mtu = gTestConfig.dc.def_test_mtu_size;
                if (ioctl(sock, SIOCSIFMTU, &ifr) == -1)
                {        
                        tst_resm(TWARN, "ioctl SIOCSIFMTU: %s", strerror(errno));
                        goterror++;
                }
                else
                {
                        if (gTestConfig.mVerbose)
                                tst_resm(TINFO, "\t+set MTU size to %d", ifr.ifr_mtu);
                        ifr.ifr_mtu = 0;
                        if (ioctl(sock, SIOCGIFMTU, &ifr) == -1)
                        {
                                tst_resm(TWARN, "ioctl SIOCGIFMTU: %s", strerror(errno));
                                goterror++;
                        }
                        if (ifr.ifr_mtu != gTestConfig.dc.def_test_mtu_size)
                        {                
                                if (gTestConfig.mVerbose)
                                        tst_resm(TWARN, "\t-MTU size was not set (%d)", ifr.ifr_mtu);
                                goterror++;
                        }
                        else                
                                if (gTestConfig.mVerbose)
                                        tst_resm(TINFO, "\t+MTU size was successfully set to %d", ifr.ifr_mtu);

                        
                        ifr.ifr_mtu = mtu;
                        if (ioctl(sock, SIOCSIFMTU, &ifr) == -1)
                        {        
                                tst_resm(TWARN, "ioctl SIOCSIFMTU: %s", strerror(errno));
                                goterror++;
                               }
                        else
                        {
                                if (gTestConfig.mVerbose)
                                        tst_resm(TINFO, "\t+return MTU size to original (%d)", ifr.ifr_mtu);
                        }
                }
        }

#ifdef DEBUG        
        /* Change metric */
        if (gTestConfig.mVerbose)
                tst_resm(TINFO, "Change metric");
        if (ioctl(sock, SIOCGIFMETRIC, &ifr) == -1)
        {
                tst_resm(TWARN, "ioctl SIOCGIFMETRIC: %s", strerror(errno));
                goterror++;
        }
        else
        {
                metric = ifr.ifr_metric;
                if (gTestConfig.mVerbose)
                {
                        tst_resm(TINFO, "\t+metric: %d", metric);
                }
                ifr.ifr_metric = gTestConfig.dc.def_test_metric;
                if (ioctl(sock, SIOCSIFMETRIC, &ifr) == -1)
                {        
                        if (gTestConfig.mVerbose)
                                tst_resm(TWARN, "ioctl SIOCSIFMETRIC: %s", strerror(errno));
                        goterror++;
                }
                else
                {
                        if (gTestConfig.mVerbose)
                                tst_resm(TINFO, "\t+set metric to %d", ifr.ifr_metric);
                        ifr.ifr_metric = 0;
                        if (ioctl(sock, SIOCGIFMETRIC, &ifr) == -1)
                        {
                                tst_resm(TWARN, "ioctl SIOCGIFMETRIC: %s", strerror(errno));
                                goterror++;
                        }
                        if (ifr.ifr_metric != gTestConfig.dc.def_test_metric)
                        {                
                                if (gTestConfig.mVerbose)
                                        tst_resm(TWARN, "\t-metric was not set (%d)", ifr.ifr_metric);
                                goterror++;
                        }
                        else                
                                if (gTestConfig.mVerbose)
                                        tst_resm(TINFO, "\t+metric was successfully set to %d", ifr.ifr_metric);

                        
                        ifr.ifr_metric = metric;
                        if (ioctl(sock, SIOCSIFMETRIC, &ifr) == -1)
                        {        
                                tst_resm(TWARN, "ioctl SIOCSIFMETRIC: %s", strerror(errno));
                                goterror++;
                               }
                        else
                        {
                                if (gTestConfig.mVerbose)
                                        tst_resm(TINFO, "\t+return metric to original (%d)", ifr.ifr_metric);
                        }
                }
        }
#endif        /* DEBUG */

        /* Change transmit queue length */
        if (gTestConfig.mVerbose)
                tst_resm(TINFO, "Change transmit queue length");
        if (ioctl(sock, SIOCGIFTXQLEN, &ifr) == -1)
        {
                tst_resm(TWARN, "ioctl SIOCGIFTXQLEN: %s", strerror(errno));
                goterror++;
        }
        else
        {
                qlen = ifr.ifr_qlen;
                if (gTestConfig.mVerbose)
                {
                        tst_resm(TINFO, "\t+tx queue length: %d", qlen);
                }
                ifr.ifr_qlen = gTestConfig.dc.def_test_tx_queue_len;
                if (ioctl(sock, SIOCSIFTXQLEN, &ifr) == -1)
                {        
                        tst_resm(TWARN, "ioctl SIOCSIFTXQLEN: %s", strerror(errno));
                        goterror++;
                }
                else
                {
                        if (gTestConfig.mVerbose)
                                tst_resm(TINFO, "\t+set tx queue length to %d", ifr.ifr_qlen);
                        ifr.ifr_qlen = 0;
                        if (ioctl(sock, SIOCGIFTXQLEN, &ifr) == -1)
                        {
                                tst_resm(TWARN, "ioctl SIOCGIFTXQLEN: %s", strerror(errno));
                                goterror++;
                        }
                        if (ifr.ifr_qlen != gTestConfig.dc.def_test_tx_queue_len)
                        {                
                                if (gTestConfig.mVerbose)
                                        tst_resm(TWARN, "\t-tx queue length was not set (%d)", ifr.ifr_qlen);
                                goterror++;
                        }
                        else                
                                if (gTestConfig.mVerbose)
                                        tst_resm(TINFO, "\t+tx queue length was successfully set to %d", ifr.ifr_qlen);

                        
                        ifr.ifr_qlen = qlen;
                        if (ioctl(sock, SIOCSIFTXQLEN, &ifr) == -1)
                        {        
                                tst_resm(TWARN, "ioctl SIOCSIFTXQLEN: %s", strerror(errno));
                                goterror++;
                               }
                        else
                        {
                                if (gTestConfig.mVerbose)
                                        tst_resm(TINFO, "\t+return tx queue length to original (%d)", ifr.ifr_qlen);
                        }
                }
        }

        return goterror ? TFAIL : TPASS;
}

/*===== print_ifmap_info =====*/
/**
@brief  print device memory map info.

@param  none

@return On success - return TPASS
        On failure - return the error code
*/
int print_ifmap_info(void)
{
        int goterror = 0;
        struct ifreq ifr;
        tst_resm(TINFO, "Device memory map info");
        
        memset(&ifr, 0, sizeof(struct ifreq));
        strncpy(ifr.ifr_name, gTestConfig.ifname, sizeof(ifr.ifr_name));

        if (ioctl(sock, SIOCGIFMAP, &ifr) == -1)
        {
                tst_resm(TWARN, "ioctl SIOCGIFMAP: %s", strerror(errno));
                goterror++;
        }       
        tst_resm(TINFO, "\tmem_start: 0x%x",ifr.ifr_map.mem_start);
        tst_resm(TINFO, "\tmem_end: 0x%x", ifr.ifr_map.mem_end);
        tst_resm(TINFO, "\tbase_addr: 0x%x", ifr.ifr_map.base_addr);
        tst_resm(TINFO, "\tirq: %d", ifr.ifr_map.irq);
        tst_resm(TINFO, "\tdma: %u", ifr.ifr_map.dma);
        tst_resm(TINFO, "\tport: %u", ifr.ifr_map.port);

        return goterror ? TFAIL : TPASS;
}

/*===== print_ifflags_info =====*/
/**
@brief  Display information about the IF flags.

@param  ifflags    - flags bitmask

@return none
*/
void print_ifflags_info(short ifflags)
{
        int i = 0, nb_flags = sizeof(ifflag_names) / sizeof(struct if_name_t);
        tst_resm(TINFO, "FEC driver flags");
        for (; i<nb_flags; ++i)
        {
                if (ifflags & ifflag_names[i].id)
                {
                        tst_resm(TINFO, "\t%s", ifflag_names[i].name);
                }
        }
        tst_resm(TINFO, "------------------------------");
}

/*===== in_cksum =====*/
/**
@brief  This function generates packet checksums.
@param  buf        - buffer with data
        nwords     - number of words to summarize

@return checksum
*/
int in_cksum(unsigned short *buf, int sz)
{
        int nleft = sz;
        int sum = 0;
        unsigned short *w = buf;
        unsigned short ans = 0;

        while (nleft > 1) {
                sum += *w++;
                nleft -= 2;
        }

        if (nleft == 1) {
                *(unsigned char *) (&ans) = *(unsigned char *) w;
                sum += ans;
        }

        sum = (sum >> 16) + (sum & 0xFFFF);
        sum += (sum >> 16);
        ans = ~sum;
        return ans;
}

/*===== inet_ntoa =====*/
/**
@brief  This function returns string containing ip address, string is allocated in static buffer
        and will be overrited at next call time.
@param  inf        - strcut in_addr which contains ip address

@return pointer to string
*/
char *inet_ntoa(const struct in_addr in)
{
        static char addr_buf[sizeof "255.255.255.255"];
        unsigned char *ucp = (unsigned char *)&in;
        sprintf(addr_buf, "%d.%d.%d.%d",
                ucp[0] & 0xff,
                ucp[1] & 0xff,
                ucp[2] & 0xff,
                ucp[3] & 0xff
                );
        addr_buf[15]='\0';
        return addr_buf;
}

/*===== inet_ntoha =====*/
/**
@brief  This function returns string containing hardware address, string is allocated in static buffer
        and will be overrited at next call time.
@param  sa_data        - pointer to buffer containing hardware address

@return pointer to string
*/
char *inet_ntoha(const char *sa_data)
{
        static char haddr[sizeof "00:11:22:33:44:55"];
        /* Hardware address */
        const unsigned char *hw = sa_data;
        sprintf(haddr, "%02x:%02x:%02x:%02x:%02x:%02x", 
                *hw, *(hw + 1), *(hw + 2), *(hw + 3), *(hw + 4), *(hw + 5));
        return haddr;
}

/*===== inet_aton =====*/
/**
@brief  This function parse string containing ip address and store it in struct in_addr

@param  cp        - pointer to string
        inp       - pointer to struct in_addr

@return 1 if success, otherwise zero
*/
int inet_aton(const char *cp, struct in_addr *inp)
{
        unsigned char *hin=(unsigned char *)inp;
        int read_val;
        unsigned int val0, val1, val2, val3;
        read_val = sscanf(cp, "%u.%u.%u.%u", &val0, &val1, &val2, &val3);        
        hin[0] = (unsigned char)val0;
        hin[1] = (unsigned char)val1;
        hin[2] = (unsigned char)val2;
        hin[3] = (unsigned char)val3;
        
        return read_val == 4;
}

/*===== inet_haton =====*/
/**
@brief  This function parse string containing hardware address and store it in buffer sa_data

@param  cp        - pointer to string
        sa_data   - pointer to buffer for hardware address

@return 1 if success, otherwise zero
*/
int inet_haton(const char *cp, char *sa_data)
{
        /* Hardware address */
        unsigned char *hw = sa_data;
        unsigned int i, hwval[6];
        i = sscanf(cp, "%02x:%02x:%02x:%02x:%02x:%02x", 
                hwval, (hwval + 1), (hwval + 2), (hwval + 3), (hwval + 4), (hwval + 5));
        if (i != 6)
                return 0;
        for(i=0; i<6; ++i)
                hw[i] = (unsigned char)hwval[i];

        return 1;
}

/*===== detect_beat_ethtool =====*/
/**
@brief  This function get link status via ethtool interface.

@param  none

@return IFSTATUS_UP or IF_STATUS down if no error occured else IFSTATUS_ERR
*/
ifstatus_t detect_beat_ethtool(void)
{
        struct ifreq ifr;
        struct ethtool_value edata;

        memset(&ifr, 0, sizeof ifr);
        strncpy(ifr.ifr_name, gTestConfig.ifname, sizeof(ifr.ifr_name));

        edata.cmd = ETHTOOL_GLINK;
        ifr.ifr_data = (caddr_t) &edata;

        if (ioctl(sock, SIOCETHTOOL, &ifr) == -1)
        {
                if (gTestConfig.mVerbose)        
                        tst_resm(TWARN, "ETHTOOL_GLINK failed: %s", strerror(errno));
                return IFSTATUS_ERR;
        }

        return edata.data ? IFSTATUS_UP : IFSTATUS_DOWN;
}

/*===== detect_beat_iff =====*/
/**
@brief  This function get link status via standart IFF interface.

@parami none

@return IFSTATUS_UP or IF_STATUS down if no error occured else IFSTATUS_ERR
*/
ifstatus_t detect_beat_iff(void)
{
        struct ifreq ifr;

        memset(&ifr, 0, sizeof ifr);
        strncpy(ifr.ifr_name, gTestConfig.ifname, sizeof ifr.ifr_name-1);

        if (ioctl(sock, SIOCGIFFLAGS, &ifr) == -1) {
                tst_resm(TWARN, "SIOCGIFFLAGS failed: %s", strerror(errno));

                return IFSTATUS_ERR;
        }

        return ifr.ifr_flags & IFF_RUNNING ? IFSTATUS_UP : IFSTATUS_DOWN;
}

/*===== detect_beat_mii =====*/
/**
@brief  This function get link status via mii interface.

@param  none

@return IFSTATUS_UP or IF_STATUS down if no error occured else IFSTATUS_ERR
*/
ifstatus_t detect_beat_mii(void)
{
        struct ifreq ifr;

        memset(&ifr, 0, sizeof ifr);
        strncpy(ifr.ifr_name, gTestConfig.ifname, sizeof ifr.ifr_name-1);

        if (ioctl(sock, SIOCGMIIPHY, &ifr) == -1)
        {
                if (gTestConfig.mVerbose)
                        tst_resm(TWARN, "SIOCGMIIPHY failed: %s", strerror(errno));
                return IFSTATUS_ERR;
        }

        ((unsigned short*) &ifr.ifr_data)[1] = 1;

        if (ioctl(sock, SIOCGMIIREG, &ifr) == -1)
        {
                if (gTestConfig.mVerbose)
                        tst_resm(TWARN, "SIOCGMIIREG failed: %s", strerror(errno));
                        return IFSTATUS_ERR;
        }

        return (((unsigned short*) &ifr.ifr_data)[3] & 0x0004) ? IFSTATUS_UP : IFSTATUS_DOWN;
}

/*===== interface_up =====*/
/**
@brief  This function sets flag IF_UP unless flag is already set.

@param  none

@return On success - return TPASS
        On failure - return the error code
*/
int interface_up(void)
{
        struct ifreq ifr;

        memset(&ifr, 0, sizeof ifr);
        strncpy(ifr.ifr_name, gTestConfig.ifname, sizeof ifr.ifr_name-1);

        if (ioctl(sock, SIOCGIFFLAGS, &ifr) < 0)
        {
                tst_resm(TWARN, "Warning: Could not get interface flags.");
                return TFAIL;
        }

        if ((ifr.ifr_flags & IFF_UP) == IFF_UP)
                return TPASS;

        ifr.ifr_flags |= IFF_UP;
        if (ioctl(sock, SIOCSIFFLAGS, &ifr) < 0)
        {
                tst_resm(TWARN, "Warning: Could not set interface flags.");
                return TFAIL;
        }

        memcpy(&ifr.ifr_addr, &gTestConfig.dc.sifr_addr, sizeof(struct sockaddr));
        if (ioctl(sock, SIOCSIFADDR, &ifr) < 0)
        {
                tst_resm(TWARN, "Warning: Could not set interface address.");
                return TFAIL;
        }

        return TPASS;
}

/*===== interface_down =====*/
/**
@brief  This function resets flag IF_UP unless flag is no set.

@param  none

@return On success - return TPASS
        On failure - return the error code
*/
int interface_down(void)
{
        struct ifreq ifr;

        memset(&ifr, 0, sizeof ifr);
        strncpy(ifr.ifr_name, gTestConfig.ifname, sizeof ifr.ifr_name-1);

        if (ioctl(sock, SIOCGIFFLAGS, &ifr) < 0)
        {
                tst_resm(TWARN, "Warning: Could not get interface flags.");
                return TFAIL;
        }

        if ((ifr.ifr_flags & IFF_UP) != IFF_UP)
                return TPASS;

        if (ioctl(sock, SIOCGIFFLAGS, &ifr) < 0)
        {
                tst_resm(TWARN, "Warning: Could not get interface flags.");
                return TFAIL;
        }

        ifr.ifr_flags ^= IFF_UP;

        if (ioctl(sock, SIOCSIFFLAGS, &ifr) < 0)
        {
                tst_resm(TWARN, "Warning: Could not set interface flags.");
                return TFAIL;
        }

        return TPASS;
}

/*===== store_config =====*/
/**
@brief  This function stores parameters of ethernet interface in global structure.

@param  none

@return On success - return TPASS
        On failure - return the error code
*/
int store_config(void)
{
        struct ifreq ifr;
        int goterror = 0;

        memset(&ifr, 0, sizeof(struct ifreq));
        strncpy(ifr.ifr_name, gTestConfig.ifname, sizeof ifr.ifr_name-1);
        
        if (ioctl(sock, SIOCGIFFLAGS, &ifr) == -1)
        {
                tst_resm(TWARN, "ioctl SIOCGIFFLAGS: %s", strerror(errno));
                goterror++;
        }
        gTestConfig.dc.sifr_flags = ifr.ifr_flags;
        if (ioctl(sock, SIOCGIFADDR, &ifr) == -1)
        {
                tst_resm(TWARN, "ioctl SIOCGIFADDR: %s", strerror(errno));
                goterror++;
        }        
        memcpy(&gTestConfig.dc.sifr_addr, &ifr.ifr_addr, sizeof(struct sockaddr));
        if (ioctl(sock, SIOCGIFDSTADDR, &ifr) == -1)
        {
                tst_resm(TWARN, "ioctl SIOCGIFDSTADDR: %s", strerror(errno));
                goterror++;
        }
        memcpy(&gTestConfig.dc.sifr_dstaddr, &ifr.ifr_dstaddr, sizeof(struct sockaddr));
        if (ioctl(sock, SIOCGIFBRDADDR, &ifr) == -1)
        {
                tst_resm(TWARN, "ioctl SIOCGIFBRDADDR: %s", strerror(errno));
                goterror++;
        }
        memcpy(&gTestConfig.dc.sifr_broadaddr, &ifr.ifr_broadaddr, sizeof(struct sockaddr));
        if (ioctl(sock, SIOCGIFNETMASK, &ifr) == -1)
        {
                tst_resm(TWARN, "ioctl SIOCGIFBRDADDR: %s", strerror(errno));
                goterror++;
        }
        memcpy(&gTestConfig.dc.sifr_netmask, &ifr.ifr_netmask, sizeof(struct sockaddr));
        if (ioctl(sock, SIOCGIFHWADDR, &ifr) == -1)
        {
                tst_resm(TWARN, "ioctl SIOCGIFHWADDR: %s", strerror(errno));
                goterror++;
        }
        memcpy(&gTestConfig.dc.sifr_hwaddr, &ifr.ifr_hwaddr, sizeof(struct sockaddr));
        if (ioctl(sock, SIOCGIFMTU, &ifr) == -1)
        {
                tst_resm(TWARN, "ioctl SIOCGIFMTU: %s", strerror(errno));
                goterror++;
        }
        gTestConfig.dc.sifr_mtu = ifr.ifr_mtu;
        if (ioctl(sock, SIOCGIFMETRIC, &ifr) == -1)
        {
                tst_resm(TWARN, "ioctl SIOCGIFMETRIC: %s", strerror(errno));
                goterror++;
        }
        gTestConfig.dc.sifr_metric = ifr.ifr_metric;
        if (ioctl(sock, SIOCGIFMAP, &ifr) == -1)
        {
                tst_resm(TWARN, "ioctl SIOCGIFTXQLEN: %s", strerror(errno));
                goterror++;
        }
        memcpy(&gTestConfig.dc.sifr_map, &ifr.ifr_map, sizeof(struct ifmap));
        if (ioctl(sock, SIOCGIFTXQLEN, &ifr) == -1)
        {
                tst_resm(TWARN, "ioctl SIOCGIFTXQLEN: %s", strerror(errno));
                goterror++;
        }

        return goterror ? TFAIL : TPASS;        
}

/*===== print_config =====*/
/**
@brief  This function prints stored ethernet interface's parameters.

@param  none

@return On success - return TPASS
        On failure - return the error code
*/
int print_config(void)
{
        def_config *dc = &gTestConfig.dc;
        tst_resm(TINFO, "\tname: %s", gTestConfig.ifname);
        print_ifflags_info(dc->sifr_flags);

        tst_resm(TINFO, "\taddress: %s", inet_ntoa( ((struct sockaddr_in*)&dc->sifr_addr)->sin_addr ));
        tst_resm(TINFO, "\tdst address: %s", inet_ntoa(((struct sockaddr_in*)&dc->sifr_dstaddr)->sin_addr));        
        tst_resm(TINFO, "\tbroad address: %s", inet_ntoa(((struct sockaddr_in*)&dc->sifr_broadaddr)->sin_addr));
        tst_resm(TINFO, "\tnetmask: %s", inet_ntoa(((struct sockaddr_in*)&dc->sifr_netmask)->sin_addr));        
        tst_resm(TINFO, "\thardware address: %s", inet_ntoha(dc->sifr_hwaddr.sa_data));        
        tst_resm(TINFO, "\tmem_start: 0x%x",dc->sifr_map.mem_start);
        tst_resm(TINFO, "\tmem_end: 0x%x", dc->sifr_map.mem_end);
        tst_resm(TINFO, "\tbase_addr: 0x%x", dc->sifr_map.base_addr);
        tst_resm(TINFO, "\tirq: %d", dc->sifr_map.irq);
        tst_resm(TINFO, "\tdma: %u", dc->sifr_map.dma);
        tst_resm(TINFO, "\tport: %u", dc->sifr_map.port);
        return 0;
}


/*===== fill_defconfig =====*/
/**
@brief  This function initializes global structure gTestConfig.

@param  none

@return On success - return TPASS
        On failure - return the error code
*/
void fill_defconfig(void)
{
        def_config *dc = &gTestConfig.dc;
        strncpy(dc->def_test_addr, "10.10.123.123", sizeof dc->def_test_addr);
        strncpy(dc->def_test_netmask, "255.254.0.0", sizeof dc->def_test_netmask);
        strncpy(dc->def_test_hwaddr, "00:11:22:33:44:55", sizeof dc->def_test_hwaddr);
        strncpy(dc->def_test_ping_addr, "172.16.0.246", sizeof dc->def_test_ping_addr);
        dc->def_test_mtu_size = 1000;
        dc->def_test_metric = 2;
        dc->def_test_tx_queue_len = 700;
        memset(&dc->sifr_addr, 0, sizeof(struct sockaddr));
        memset(&dc->sifr_dstaddr, 0, sizeof(struct sockaddr));
        memset(&dc->sifr_broadaddr, 0, sizeof(struct sockaddr));
        memset(&dc->sifr_netmask, 0, sizeof(struct sockaddr));
        memset(&dc->sifr_hwaddr, 0, sizeof(struct sockaddr));
        memset(&dc->sifr_map, 0, sizeof(struct ifmap));
        dc->sifr_mtu = 0;
        dc->sifr_metric = 0;
        dc->sifr_qlen = 0;
}

/*===== ping =====*/
/**
@brief  Send ping to specified ip address.

@param  host_ip    - pointer to string containing ip address

@return On success - return TPASS
        On failure - return the error code
*/
int ping(const char *host_ip)
{
        struct sockaddr_in pingaddr;
        struct icmphdr *pkt;
        int pingsock, c;
        char packet[DEFDATALEN + MAXIPLEN + MAXICMPLEN];

        pingsock = socket(AF_INET, SOCK_RAW, 1);        /* 1 - ICMP by defaults */

        memset(&pingaddr, 0, sizeof(struct sockaddr_in));

        pingaddr.sin_family = AF_INET;
        if (!inet_aton(host_ip, &pingaddr.sin_addr))
                return TFAIL;

        pkt = (struct icmphdr *) packet;
        memset(pkt, 0, sizeof(packet));
        pkt->type = ICMP_ECHO;
        pkt->checksum = in_cksum((unsigned short *) pkt, sizeof(packet));

        c = sendto(pingsock, packet, DEFDATALEN + ICMP_MINLEN, 0,
                           (struct sockaddr *) &pingaddr, sizeof(struct sockaddr_in));

        if (c < 0)
        {
                close(pingsock);
                tst_resm(TWARN, "sendto failed. %s", strerror(errno));
        }

        signal(SIGALRM, VT_noresp_handler);
        alarm(5);                                        /* give the host 5000ms to respond */
        /* listen for replies */
        while (1)
        {
                struct sockaddr_in from;
                socklen_t fromlen = sizeof(from);

                if ((c = recvfrom(pingsock, packet, sizeof(packet), 0,
                                                  (struct sockaddr *) &from, &fromlen)) < 0)
                {
                        if (errno != EINTR)
                                tst_resm(TWARN, "recvfrom failed. %s", strerror(errno));
                        continue;
                }
                if (c >= 76) {                        /* ip + icmp */
                        struct iphdr *iphdr = (struct iphdr *) packet;

                        pkt = (struct icmp *) (packet + (iphdr->ihl << 2));        /* skip ip hdr */
                        if (pkt->type == ICMP_ECHOREPLY)
                                break;
                }
        }
        
        close(pingsock);
        tst_resm(TINFO, "%s pinged successfully!\n", host_ip);
        return TPASS;
}

/*===== VT_noresp_handler =====*/
/**
@brief  Function for handling alarm signal in ping test.

@param  arg        - not used

@return none
*/
void VT_noresp_handler(int arg)
{
        tst_resm(TWARN, "No response from %s", gTestConfig.dc.def_test_ping_addr);
        tst_resm(TFAIL, "Test %s did NOT work as expected", TCID);
        exit(TFAIL);
}

/*===== VT_ask_user =====*/
/**
@brief  Show message and read answer('y'|'n'|'q')

@param  msg        - pointer to string with message

@return TPASS, TFAIL or TRETR according to the user's answer
*/
int VT_ask_user(char *msg)
{
        int answer = TRETR;
        int ch;
        tst_resm(TINFO, "%s", msg);
        do
        {
                ch = getchar();
                if (tolower(ch) == 'y')
                        answer = TPASS;
                if (tolower(ch) == 'n')
                        answer = TFAIL;
                if (tolower(ch) == 'q')
                        break;
        } while (answer == TRETR);
        
        return answer;
}
