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
        @file   pmic_test_common.h

        @brief  Common unit.
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  ------------------------------------------
D.Khoroshev/b00313           07/20/2005     TLSbo52700   Initial version
D.Khoroshev/b00313           09/06/2005     TLSbo52700   Rework version
D.Khoroshev/b00313           12/06/2005     TLSbo58274   Removed test module
D.Khoroshev/b00313           02/15/2006     TLSbo59968   Returned test module for MC13783 support
D.Khoroshev/b00313           07/25/2006     TLSbo64239   Added mc13783 legacy API support

==================================================================================================*/
#ifndef PMIC_TEST_COMMON_H
#define PMIC_TEST_COMMON_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                         INCLUDE FILES
==================================================================================================*/
#include <sys/types.h>     /* open()              */
#include <sys/stat.h>      /* open()              */
#include <fcntl.h>         /* open()              */
#include <sys/ioctl.h>     /* ioctl()             */
#include <unistd.h>        /* close()             */
#include <stdio.h>         /* sscanf() & perror() */
#include <stdlib.h>        /* atoi()              */
#include <pthread.h>
#include <linux/autoconf.h>

#include <linux/pmic_status.h>
#include <linux/pmic_external.h>


#ifndef bool
//typedef bool int
#endif

/*==================================================================================================
                                           CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/
#define    TEST_CASE_RW          "RW"
#define    TEST_CASE_SU          "SU"
#define    TEST_CASE_S_IT_U      "S_IT_U"
#define    TEST_CASE_CA          "CA"
#define    TEST_CASE_IP          "IP"
#define PMIC_DEVICE        "/dev/pmic"


enum PMIC_PROTOCOL_TEST_IOCTL
{
                PMIC_READ_REG_T                 = 1,
                PMIC_WRITE_REG_T                = 2,
                PMIC_SUBSCRIBE_T                = 3,
                PMIC_UNSUBSCRIBE_T              = 4,
                PMIC_CHECK_SENSOR_T             = 5,
                PMIC_GET_SENSORS_T              = 6
};
enum
{
        PMIC_PROTOCOL_RW, PMIC_PROTOCOL_SU, PMIC_PROTOCOL_S_IT_U, PMIC_PROTOCOL_D, PMIC_PROTOCOL_OC,
        PMIC_PROTOCOL_CA, PMIC_PROTOCOL_RA, PMIC_PROTOCOL_IP, PMIC_PROTOCOL_SC, PMIC_PROTOCOL_conf
};

#if !defined(TRUE) && !defined(FALSE)
#define TRUE        1
#define FALSE       0
#endif

/*#ifdef CONFIG_MXC_PMIC_MC13783
typedef int t_sensor;
typedef t_sense_bits t_sensor_bits;
#endif
*/

#define MAX_REG       			50

typedef struct
{
/* operation - performing operation (PMIC_READ_REG_T, PMIC_WRITE_REG_T, PMIC_SUBSCRIBE_T, PMIC_UNSUBSCRIBE_T,
        PMIC_CHECK_SENSOR_T, PMIC_GET_SENSORS_T,)        */
        int operation;
        /* val1 - reg number if operation is PMIC_READ_REG_T or PMIC_WRITE_REG_T,
                - special flag used in operation is PMIC_SUBSCRIBE_T or PMIC_UNSUBSCRIBE_T
                0 - val2 is pointer to type_event_notification structure.
                1 - val2 is pointer to type_event_notification structure. Structure is temporary and
                    memory for this structure will be freed automatically in VT_pmic_opt.
                2 - val2 is number of event(int). Structure type_event_notification will be created and
                    deleted automatically in VT_pmic_opt. In this case callback and param fields are
                    default_callback() and param_counter(auto incrementing static variable).
                - sensor number if operation is PMIC_CHECK_SENSOR_T        */
        int val1;
        /*  val2 - writing reg value in case of PMIC_WRITE_REG_T operation
                 - return reg value in case of PMIC_READ_REG_T operation
                 - pointer to structure type_event_notification used in PMIC_SUBSCRIBE_T
                   or PMIC_UNSUBSCRIBE_T operations
                 - pointer to structure t_sensor_bits if operation PMIC_GET_SENSORS_T        */
        unsigned int val2;
        /*      mask - bitmask used in VT_pmic_write function
        */
        unsigned int mask;
} opt_params;

typedef struct {
        int test_id;
        int thread_num;
} test_param;
/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/
int verbose_flag;
pthread_mutex_t mutex;

/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/
int VT_pmic_setup(void);
int VT_pmic_cleanup(void);

int VT_pmic_read(int device, int reg, unsigned int *val);
int VT_pmic_write(int device, int reg, unsigned int val);
int VT_pmic_subscribe(int device, unsigned int event);
int VT_pmic_unsubscribe(int device, unsigned int event);

/*int VT_pmic_read_opt_params(char *file_name, int *nb_param, opt_params **params);
void VT_pmic_print_opt(opt_params *params);

int VT_pmic_opt_param(int fd, opt_params *params);
int VT_pmic_opt(int fd, int operation, int val1, unsigned int *val2);
char *pmic_error(int errcode);
*/
#ifdef __cplusplus
}
#endif

#endif        /* PMIC_TEST_COMMON_H */
