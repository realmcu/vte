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
        @file  pmic_test_common.c

        @brief  Common unit.
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
D. Khoroshev/b00313          07/20/2005     TLSbo52700   Initial version
D. Khoroshev/b00313          09/06/2005     TLSbo52700   Rework version
D. Khoroshev/b00313          12/06/2005     TLSbo58274   Remove test module
D. Khoroshev/b00313          01/13/2006     TLSbo59968   Added specific MC13783/SC55112 functions
                                                         support
D. Khoroshev/b00313          02/15/2006     TLSbo59968   Returned test module for MC13783 support
D. Khoroshev/b00313          07/25/2006     TLSbo64239   Added mc13783 legacy API support


====================================================================================================
Portability: ARM GCC
==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <errno.h>
#include <ctype.h>
#include <string.h>

/* Harness Specific Include Files. */
#include <test.h>

/* Verification Test Environment Include Files */
#include "pmic_test_common.h"

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
                                       LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/
t_sensor_bits default_sens_bits;
unsigned int last_reg_value;

/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== default_callback =====*/
/**
@brief  Default callback function used when VT_pmic_opt or VT_pmic_opt_param was called with operation
        PMIC_SUBSCRIBE_T/PMIC_UNSUBSCRIBE_T without existing structure type_event_notification, but
        only with event number.

@param  Input:  arg - argument, which callback function receives when it is calling

@return None
*/
/*================================================================================================*/
void default_callback(void *arg)
{
        pthread_mutex_lock(&mutex);
        tst_resm(TINFO,"default_callback() function were called. Argument is (int)%d", (int)arg);
        pthread_mutex_unlock(&mutex);
}

/*================================================================================================*/
/*===== VT_pmic_setup =====*/
/**
@brief  Creates mutex used for multithreading tests

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_pmic_setup(void)
{
        return pthread_mutex_init(&mutex, NULL);
}

/*================================================================================================*/
/*===== VT_cleanup =====*/
/**
@brief  Destroy mutex used for multithreading tests

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_pmic_cleanup(void)
{
        return  pthread_mutex_destroy(&mutex);
}

/*================================================================================================*/
/*===== VT_pmic_read_opt_params =====*/
/**
@brief  Parse config file used for tests CA, IP, RW, conf.
        Config file have next format:
        <number_of_records>
        PMIC_XXXXXXXXX_T<tab><number><tab><hex_number><tab><hex_number>
        PMIC_XXXXXXXXX_T<tab><number><tab><hex_number><tab><hex_number>
        ....
        The first line have to be a number of records in config file.
        Next lines contains four elements of opt_params structure.
        '#' comments and empty lines allowed, but the first line always must
        contain number of records.

@param  Input :        file_name - Config file name
        Output:        nb_param - Number of read records
                        opt_param - Array of opt_param structures which defines sequence of
                                    commands used by function VT_pmic_opt_param

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
/*int VT_pmic_read_opt_params(char *file_name, int *nb_param, opt_params **opt_param)
{
        int param_number = 0, i, line;
        char str[256], *p=NULL, op[20];
        opt_params *params=*opt_param;
        FILE *f=NULL;

        memset(str, 0x00, sizeof(char)*256);
        if(params != NULL)
                free(params);
        if((f=fopen(file_name,"r")) == NULL)
        {
                tst_resm(TWARN,"Failed to open file %s",file_name);
                return TFAIL;
        }

        if(fgets(str, 255, f) == NULL)
        {
                *nb_param = 0;
                *opt_param = NULL;
                fclose(f);
                return TFAIL;
        }

        sscanf(str,"%d", &param_number);
        if(param_number <= 0)
        {
                tst_resm(TWARN,"Incorrect number of parameter records %d",param_number);
                return TFAIL;
        }
        *nb_param=param_number;
        params=(opt_params*)malloc(sizeof(opt_params)*param_number);
        if(params == NULL)
        {
                tst_resm(TWARN,"Can't allocate memory for opt_param array");
                return TFAIL;
        }

        // File format:
        //        <number of entries>
        //        <operation type><tab><val1><tab><val2><tab><mask>
        //        ...

        for(i=0,line=2; i<param_number; i++,line++)
        {
                if(fgets(str, 255, f) == NULL)
                {
                        break;
                }

                p=str;
                //        Skipping comments
                while( isspace(*p) ) p++;
                if( *p == '#' || *p == '\0' )
                {
                        i--;
                        continue;
                }

                memset(op, 0x00, sizeof(char)*20);
                if(sscanf(str,"%20s\t%d\t%X\t%X", op, &params[i].val1, &params[i].val2, &params[i].mask) < 4)
                {
                        tst_resm(TWARN,"Syntax error in input file. Too few parameters line %d:\n%s",line,str);
                        i--;
                        continue;
                }

                if( strncmp(op,"PMIC_READ_REG_T",15) == 0 )
                {
                        params[i].operation=PMIC_READ_REG_T;
                }
                else if( strncmp(op,"PMIC_WRITE_REG_T",16) == 0 )
                {
                        params[i].operation=PMIC_WRITE_REG_T;
                }
                else if( strncmp(op,"PMIC_SUBSCRIBE_T",16) == 0 )
                {
                        params[i].operation=PMIC_SUBSCRIBE_T;
                        type_event_notification *event;
                        event=(type_event_notification*)malloc(sizeof(type_event_notification));
                        event->event=params[i].val2;
                        event->param=(void*)param_counter++;
#ifdef CONFIG_MXC_MC13783_LEGACY
                        event->callback_p=default_callback;
#else
                        event->callback=default_callback;
#endif
                        params[i].val1 = 1;
                        params[i].val2 = (unsigned int)event;
                }
                else if( strncmp(op,"PMIC_UNSUBSCRIBE_T",18) == 0 )
                {
                        params[i].operation=PMIC_UNSUBSCRIBE_T;
                        type_event_notification *event;
                        event=(type_event_notification*)malloc(sizeof(type_event_notification));
                        event->event=params[i].val2;
                        event->param=(void*)param_counter++;
#ifdef CONFIG_MXC_MC13783_LEGACY
                        event->callback_p=default_callback;
#else
                        event->callback=default_callback;
#endif
                        params[i].val1 = 1;
                        params[i].val2 = (unsigned int)event;
                }
                else if( strncmp(op,"PMIC_CHECK_SENSOR_T",19) == 0 )
                {
                        params[i].operation=PMIC_CHECK_SENSOR_T;
                }
                else if( strncmp(op,"PMIC_GET_SENSORS_T",18) == 0 )
                {
                        params[i].operation=PMIC_GET_SENSORS_T;
                        params[i].val2=(unsigned int)&default_sens_bits;
                }
                else
                {
                        //        Syntax Error
                        tst_resm(TWARN,"Syntax error in input file, line %d:\n%s",line,str);
                }
        }
        if( i == 0 )
        {
                free(params);
                *nb_param = 0;
                *opt_param = NULL;
        }
        else
        {
                *nb_param = i;
                *opt_param = realloc(params, *nb_param*sizeof(opt_params));
        }

        fclose(f);
        return TPASS;
}
*/
/*================================================================================================*/
/*===== VT_pmic_read =====*/
/**
@brief  VT_pmic_print_opt

@param  Input :        param - pointer to opt_param structure which should be displayed

@return None
*/
/*================================================================================================*/
/*void VT_pmic_print_opt(opt_params *param)
{
        static char *operation_name[] =
        {
                "PMIC_READ_REG_T",
                "PMIC_WRITE_REG_T",
                "PMIC_SUBSCRIBE_T",
                "PMIC_UNSUBSCRIBE_T",
                "PMIC_CHECK_SENSOR_T",
                "PMIC_GET_SENSORS_T",
                "UNKNOWN"
        };
        int op_id = 0;
        if(param == NULL)
        {
                tst_resm(TWARN,"option is NULL");
                return;
        }
        if(param->operation > 6 || param->operation < 1)
                op_id = 6;
        else
                op_id = param->operation - 1;
        tst_resm(TINFO,"Operation: %s %d 0x%X 0x%X", operation_name[op_id], param->val1,
                param->val2, param->mask);
        if((op_id == 2 || op_id == 3) && param->val1 != 2)
        {
                type_event_notification *event = (type_event_notification*)param->val2;
                tst_resm(TINFO,"\tevent: event %d, callback %p, param (int) 0x%X",event->event, event->callback, event->param);
        }
}
*/
/*================================================================================================*/
/*===== VT_pmic_read =====*/
/**
@brief  read register

@param  Input : fd - file descriptor assigned to the SC55112
                reg - reg number
        Output: val - pointer to returned value

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_pmic_read(int fd, int reg, unsigned int *val)
{
        register_info reg_info;
        reg_info.reg = reg;
        int rv = 0;
        if(ioctl(fd, PMIC_READ_REG, &reg_info) < 0)
        {
                rv=errno;
                pthread_mutex_lock(&mutex);
                tst_resm(TFAIL,"Read value from reg %d: FAILED. Error code %d", reg, rv);
                pthread_mutex_unlock(&mutex);
                return rv;
        }else{
        		*val = reg_info.reg_value;
        		pthread_mutex_lock(&mutex);
        		tst_resm(TINFO,"Read value from reg %d: 0x%X", reg, *val);
        		pthread_mutex_unlock(&mutex);
		}
        return rv;
}

/*================================================================================================*/
/*===== VT_pmic_write =====*/
/**
@brief  writes register

@param  Input :         fd - file descriptor assigned to the SC55112
                                reg - reg number
                                val - writing value

@return On success - return TPASS
        On failure - return TFAIL
*/
/*================================================================================================*/
int VT_pmic_write(int fd, int reg, unsigned int val)
{
        register_info reg_info;
        reg_info.reg = reg;
        reg_info.reg_value = val;

        int rv = 0;
        if(ioctl(fd, PMIC_WRITE_REG, &reg_info) < 0)
        {
                rv=errno;
                pthread_mutex_lock(&mutex);
                tst_resm(TFAIL,"Write value 0x%X in reg %d,with error 0x%X: FAILED. Error code %d", val, reg, rv);
                pthread_mutex_unlock(&mutex);
                return rv;
        }else{
        		pthread_mutex_lock(&mutex);
        		tst_resm(TINFO,"Write value 0x%X in reg %d : OK", val, reg);
        		pthread_mutex_unlock(&mutex);
		}
        return rv;
}

/*================================================================================================*/
/*===== VT_pmic_subscribe =====*/
/**
@brief  subscribe event

@param  Input :         fd - file descriptor assigned to the SC55112
                        event - pointer to structure type_event_notification
@return On success - return TPASS
        On failure - return TFAIL
*/
/*================================================================================================*/
int VT_pmic_subscribe(int fd, unsigned int event)
{
        int rv=0;
//        register_info reg_info;
//		reg_info.event = event;

        if(ioctl(fd, PMIC_SUBSCRIBE, &event) != 0)
        {
                rv=errno;
                pthread_mutex_lock(&mutex);
                tst_resm(TFAIL, "Subscribe event %d: FAILED", event);
                pthread_mutex_unlock(&mutex);
                return rv;
        }else{
                pthread_mutex_lock(&mutex);
                tst_resm(TINFO, "Subscribe event %d: OK", event);
                pthread_mutex_unlock(&mutex);
        }
        return rv;
}

/*================================================================================================*/
/*===== VT_pmic_unsubscribe =====*/
/**
@brief  unsubscribes event

@param  Input :         fd - file descriptor assigned to the SC55112
                        event - pointer to structure type_event_notification

@return On success - return TPASS
        On failure - return TFAIL
*/
/*================================================================================================*/
int VT_pmic_unsubscribe(int fd, unsigned int event)
{
        int rv=0;
//        register_info reg_info;
//		reg_info.event = event;
        if(ioctl(fd, PMIC_UNSUBSCRIBE, &event) != 0){
                rv=errno;
                pthread_mutex_lock(&mutex);
                tst_resm(TFAIL, "Unsubscribe event %d: FAILED\n", event);
                pthread_mutex_unlock(&mutex);
                return rv;
        }else{
		    pthread_mutex_lock(&mutex);
            tst_resm(TINFO, "Unsubscribe event %d: OK\n", event);
		    pthread_mutex_unlock(&mutex);
	        return TPASS;
	    }
	    return rv;
}

/*================================================================================================*/
/*===== VT_pmic_check_sensor =====*/
/**
@brief  checks sensor

@param  Input :         fd - file descriptor assigned to the SC55112
                        sensor - sensor number

@return State of sensor(boolean).
*/
/*================================================================================================*/
#ifdef CONFIG_MXC_PMIC_SC55112
/*int VT_pmic_check_sensor(int fd, t_sensor sensor)
{
        register_info reg_info;
        int val, rv = 0;

        reg_info.reg=REG_PSTAT;
        if(verbose_flag)
                tst_resm(TWARN,"Checking sensor %d", sensor);

        if(ioctl(fd, PMIC_READ_REG, &reg_info) < 0)
        {
                rv=errno;
                if (verbose_flag)
                {
                        pthread_mutex_lock(&mutex);
                        tst_resm(TWARN,"Read value from reg REG_PSTAT: FAILED. Error code %d", rv);
                        pthread_mutex_unlock(&mutex);
                }
                return rv;
        }
        val = reg_info.reg_value;
        if(sensor<0 || sensor>31)
        {
                if (verbose_flag)
                {
                        pthread_mutex_lock(&mutex);
                        tst_resm(TWARN,"Incorrect sensor %d", sensor);
                        pthread_mutex_unlock(&mutex);
                }

        }
        return (val & (1 << sensor_offset[sensor])) ? TRUE : FALSE;
}*/
#endif        /* CONFIG_MXC_PMIC_SC55112 */

//#if defined( CONFIG_MXC_PMIC_MC13783 ) || defined( CONFIG_MXC_MC13783_LEGACY )
//int VT_pmic_check_sensor(int fd, t_sensor sensor)
//{
//        return 0;       /* Not realized for MC13783 */
//}
//#endif        /* CONFIG_MXC_PMIC_MC13783 */

/*================================================================================================*/
/*===== VT_pmic_get_sensors =====*/
/**
@brief  get sensors values

@param  Input :         fd - file descriptor assigned to the SC55112
                        sens_bits - pointer to structure t_sensor_bits

@return On success - return TPASS
        On failure - return TFAIL
*/
/*================================================================================================*/
#ifdef CONFIG_MXC_PMIC_SC55112
/*int VT_pmic_get_sensors(int fd, t_sensor_bits *sensor_bits)
{
        register_info reg_info;
        int rrstat = 0, rv = 0;

        reg_info.reg=REG_PSTAT;
        if(verbose_flag)
                tst_resm(TWARN,"Checking all sensors");

        if(ioctl(fd, PMIC_READ_REG, &reg_info) < 0)
        {
                rv=errno;
                if (verbose_flag)
                {
                        pthread_mutex_lock(&mutex);
                        tst_resm(TWARN,"Read value from reg REG_PSTAT: FAILED. Error code %d", rv);
                        pthread_mutex_unlock(&mutex);
                }
                return rv;
        }
        rrstat = reg_info.reg_value;

        sensor_bits->usbdet_44v = (rrstat & (1 << 6)) ? TRUE : FALSE;
        sensor_bits->onoffsns = (rrstat & (1 << 7)) ? TRUE : FALSE;
        sensor_bits->onoffsns2 = (rrstat & (1 << 8)) ? TRUE : FALSE;
        sensor_bits->usbdet_08v = (rrstat & (1 << 9)) ? TRUE : FALSE;
        sensor_bits->mobsnsb = (rrstat & (1 << 10)) ? TRUE : FALSE;
        sensor_bits->pttsns = (rrstat & (1 << 11)) ? TRUE : FALSE;
        sensor_bits->a1sns = (rrstat & (1 << 12)) ? TRUE : FALSE;
        sensor_bits->usbdet_20v = (rrstat & (1 << 14)) ? TRUE : FALSE;
        sensor_bits->eol_stat = (rrstat & (1 << 16)) ? TRUE : FALSE;
        sensor_bits->clk_stat = (rrstat & (1 << 17)) ? TRUE : FALSE;
        sensor_bits->sys_rst = (rrstat & (1 << 18)) ? TRUE : FALSE;
        sensor_bits->warm_sys_rst = (rrstat & (1 << 20)) ? TRUE : FALSE;
        sensor_bits->batt_det_in_sns = (rrstat & (1 << 21)) ? TRUE : FALSE;

        return rv;
}*/
#endif        /* CONFIG_MXC_PMIC_SC55112 */

/*#if defined( CONFIG_MXC_PMIC_MC13783 ) || defined( CONFIG_MXC_MC13783_LEGACY )
int VT_pmic_get_sensors(int fd, t_sensor_bits* sensor_bits)
{
        int rv=0;
        int sense_0 = 0;
        int sense_1 = 0;

        memset(sensor_bits, 0, sizeof(t_sensor_bits));

        rv=VT_pmic_read(fd, REG_INTERRUPT_SENSE_0, &sense_0);
        if(rv < 0)
        {
                rv=errno;
                if (verbose_flag)
                {
                        pthread_mutex_lock(&mutex);
                        tst_resm(TWARN,"Read value from reg REG_INTERRUPS_SENSE_0: FAILED. Error code %d", rv);
                        pthread_mutex_unlock(&mutex);
                }
                return rv;
        }

        rv=VT_pmic_read(fd, REG_INTERRUPT_SENSE_1, &sense_1);
        if(rv < 0)
        {
                rv=errno;
                if (verbose_flag)
                {
                        pthread_mutex_lock(&mutex);
                        tst_resm(TWARN,"Read value from reg REG_INTERRUPS_SENSE_0: FAILED. Error code %d", rv);
                        pthread_mutex_unlock(&mutex);
                }
                return rv;
        }

        if (sense_0 & 0x000040) sensor_bits->sense_chgdets   = true;
        if (sense_0 & 0x000080) sensor_bits->sense_chgovs    = true;
        if (sense_0 & 0x000100) sensor_bits->sense_chgrevs   = true;
        if (sense_0 & 0x000200) sensor_bits->sense_chgshorts = true;
        if (sense_0 & 0x000400) sensor_bits->sense_cccvs     = true;
        if (sense_0 & 0x000800) sensor_bits->sense_chgcurrs  = true;
        if (sense_0 & 0x001000) sensor_bits->sense_bpons     = true;
        if (sense_0 & 0x002000) sensor_bits->sense_lobatls   = true;
        if (sense_0 & 0x004000) sensor_bits->sense_lobaths   = true;
        if (sense_0 & 0x010000) sensor_bits->sense_usb4v4s   = true;
        if (sense_0 & 0x020000) sensor_bits->sense_usb2v0s   = true;
        if (sense_0 & 0x040000) sensor_bits->sense_usb0v8s   = true;
        if (sense_0 & 0x080000) sensor_bits->sense_id_floats = true;
        if (sense_0 & 0x100000) sensor_bits->sense_id_gnds   = true;
        if (sense_0 & 0x200000) sensor_bits->sense_se1s      = true;
        if (sense_0 & 0x400000) sensor_bits->sense_ckdets    = true;

        if (sense_1 & 0x000008) sensor_bits->sense_onofd1s   = true;
        if (sense_1 & 0x000010) sensor_bits->sense_onofd2s   = true;
        if (sense_1 & 0x000020) sensor_bits->sense_onofd3s   = true;
        if (sense_1 & 0x000800) sensor_bits->sense_pwrrdys   = true;
        if (sense_1 & 0x001000) sensor_bits->sense_thwarnhs  = true;
        if (sense_1 & 0x002000) sensor_bits->sense_thwarnls  = true;
        if (sense_1 & 0x004000) sensor_bits->sense_clks      = true;
        if (sense_1 & 0x020000) sensor_bits->sense_mc2bs     = true;
        if (sense_1 & 0x040000) sensor_bits->sense_hsdets    = true;
        if (sense_1 & 0x080000) sensor_bits->sense_hsls      = true;
        if (sense_1 & 0x100000) sensor_bits->sense_alspths   = true;
        if (sense_1 & 0x200000) sensor_bits->sense_ahsshorts = true;

        return rv;
}

#endif */       /* CONFIG_MXC_PMIC_MC13783 */

/*================================================================================================*/
/*===== VT_pmic_opt =====*/
/**
@brief  perform operations (read reg, write reg, subscribe event, unsubscribe event, check sensors)

@param  Input : fd - file descriptor assigned to the SC55112 test module device file
                params - pointer to opt_params structure.
        Output: params - pointer to opt_params structure. Result usually returns in val2 field.

@return On success - return TPASS
        On failure - return TFAIL

*/
/*================================================================================================*/
/*int VT_pmic_opt_param(int fd, opt_params *params)
{
        if(params == NULL)
                return PMIC_PARAMETER_ERROR;
        if(params->operation == PMIC_WRITE_REG_T)
                return VT_pmic_write(fd, params->val1, params->val2, params->mask);
        return VT_pmic_opt(fd, params->operation, params->val1, &(params->val2));
}
*/
/*================================================================================================*/
/*===== VT_pmic_opt =====*/
/**
@brief  perform operations (read reg, write reg, subscribe event, unsubscribe event, check sensors)

@param  Input :         fd - file descriptor assigned to the SC55112
                        operation - performing operation (PMIC_READ_REG_T, PMIC_WRITE_REG_T, PMIC_SUBSCRIBE_T,
                                    PMIC_UNSUBSCRIBE_T, PMIC_CHECK_SENSOR_T, PMIC_GET_SENSORS_T)
                        val1 - reg number if operation is PMIC_READ_REG_T or PMIC_WRITE_REG_T,
                             - event number in case of PMIC_SUBSCRIBE_T or PMIC_UNSUBSCRIBE_T operations
                             - sensor number if operation is PMIC_CHECK_SENSOR_T
                             val2 - writing reg value in case of PMIC_WRITE_REG_T operation
                             - pointer to structure t_sensor_bits if operation is PMIC_GET_SENSORS_T
                             - pointer to structure type_event_notification in case PMIC_SUBSCRIBE_T,
                               PMIC_UNSUBSCRIBE_T. If val1 equeals 1, memory shall frees automaticaly.
                               If val1 equeals 2, temporary structure will be created automatically.
        Output:         val2 - return reg value in case of PMIC_READ_REG_T operation
                             - pointer to filled up structure t_sensor_bits if operation is PMIC_GET_SENSORS_T

@return On success - return TPASS
        On failure - return TFAIL

*/
/*================================================================================================*/
/*int VT_pmic_opt(int fd, int operation, int val1, unsigned int *val2)
{
        int rv=PMIC_SUCCESS;
        switch(operation)
        {
        case PMIC_READ_REG_T:
                rv=VT_pmic_read(fd, val1, val2);
                break;
        case PMIC_WRITE_REG_T:
                rv=VT_pmic_write(fd, val1, *val2, PMIC_ALL_BITS);
                break;
        case PMIC_SUBSCRIBE_T:
                if(val1==2)
                {   //      In this case val2 is event number
                        type_event_notification *event;
                        event=(type_event_notification*)malloc(sizeof(type_event_notification));
                        if(!event)
                        {
                                return PMIC_MALLOC_ERROR;
                        }
                        event->event=*val2;
#ifdef CONFIG_MXC_MC13783_LEGACY
                        event->callback_p=default_callback;
#else
                        event->callback=default_callback;
#endif
                        event->param=(void*)param_counter++;
                        if(verbose_flag)
                                tst_resm(TINFO,"Temporary structure type_event_notification created");
                        *val2=(unsigned int)event;
                }
                if(val2==NULL || *val2==0) return PMIC_PARAMETER_ERROR;

                if(verbose_flag)
#ifdef CONFIG_MXC_MC13783_LEGACY
                        tst_resm(TINFO,"Unsubscribing event: event %d, callback %p, callback_p %p, param 0x%X",
                        ((type_event_notification*)*val2)->event,
                        ((type_event_notification*)*val2)->callback,
                        ((type_event_notification*)*val2)->callback_p,
                        ((type_event_notification*)*val2)->param);

#else
                        tst_resm(TINFO,"Unsubscribing event: event %d, callback %p, param 0x%X",
                        ((type_event_notification*)*val2)->event,
                        ((type_event_notification*)*val2)->callback,
                        ((type_event_notification*)*val2)->param);
#endif

                rv=VT_pmic_subscribe(fd, (type_event_notification*)*val2);
                if(val1 != 0)
                {
                        if(verbose_flag)
                                tst_resm(TINFO,"Temporary structure deleting(%d)",val1);
                        free((void*)*val2);
                        if(verbose_flag)
                                tst_resm(TINFO,"Temporary structure deleted");
                }
                break;
        case PMIC_UNSUBSCRIBE_T:
                if(val1==2)
                {   //      In this case val2 is event number
                        type_event_notification *event;
                        event=(type_event_notification*)malloc(sizeof(type_event_notification));
                        if(!event)
                        {
                                return PMIC_MALLOC_ERROR;
                        }
                        event->event=*val2;

#ifdef CONFIG_MXC_MC13783_LEGACY
                        event->callback_p=default_callback;
#else
                        event->callback=default_callback;
#endif

                        event->param=(void*)param_counter++;
                        *val2=(unsigned int)event;
                }
                if(val2==NULL || *val2==0) return PMIC_PARAMETER_ERROR;

                if(verbose_flag)
#ifdef CONFIG_MXC_MC13783_LEGACY
                        tst_resm(TINFO,"Unsubscribing event: event %d, callback %p, callback_p %p, param 0x%X",
                        ((type_event_notification*)*val2)->event,
                        ((type_event_notification*)*val2)->callback,
                        ((type_event_notification*)*val2)->callback_p,
                        ((type_event_notification*)*val2)->param);

#else
                        tst_resm(TINFO,"Unsubscribing event: event %d, callback %p, param 0x%X",
                        ((type_event_notification*)*val2)->event,
                        ((type_event_notification*)*val2)->callback,
                        ((type_event_notification*)*val2)->param);
#endif

                rv=VT_pmic_unsubscribe(fd, (type_event_notification*)*val2);
                if(val1 != 0)
                {
                        if(verbose_flag)
                                tst_resm(TINFO,"Temporary structure deleting(%d)",val1);
                        free((void*)*val2);
                        if(verbose_flag)
                                tst_resm(TINFO,"Temporary structure deleted");
                }
                break;
        case PMIC_CHECK_SENSOR_T:
                rv=VT_pmic_check_sensor(fd, val1);
                break;
        case PMIC_GET_SENSORS_T:
                rv=VT_pmic_get_sensors(fd, (t_sensor_bits*)val2);
                break;
        default:
                tst_resm(TFAIL, "Unsupported operation");
                rv=PMIC_NOT_SUPPORTED;
                break;
        }
        return rv;
}*/

/*================================================================================================*/
/*===== pmic_error =====*/
/**
@brief  Returns string with error code short description

@param  errno - PMIC_STATUS error code.

@return On success - return pointer to string or NULL
*/
/*================================================================================================*/
char *pmic_error(int errcode)
{
        static char *pmic_errors[] =
        {
                "PMIC_SUCCESS",       /*!< The requested operation was successfully
                                           completed.                                     */
                "PMIC_ERROR",        /*!< The requested operation could not be completed
                                           due to an error.                               */
                "PMIC_PARAMETER_ERROR",      /*!< The requested operation failed because
                                                   one or more of the parameters was
                                                   invalid.                             */
                "PMIC_NOT_SUPPORTED",        /*!< The requested operation could not be
                                                   completed because the PMIC hardware
                                                   does not support it. */
                "PMIC_CLIENT_NBOVERFLOW",    /*!< The requested operation could not be
                                                   completed because there are too many
                                                   PMIC client requests */
                "PMIC_MALLOC_ERROR", /*!< Error in malloc function             */
                "PMIC_UNSUBSCRIBE_ERROR",    /*!< Error in un-subscribe event          */
                "PMIC_EVENT_NOT_SUBSCRIBED", /*!< Event occur and not subscribed       */
                "PMIC_EVENT_CALL_BACK",      /*!< Error - bad call back                */
        };
        if(errcode < 0)
                return NULL;
        if(errcode > -PMIC_EVENT_CALL_BACK)
                return strerror(errcode);

        return pmic_errors[errcode];
}
