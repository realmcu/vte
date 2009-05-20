/**
* @file        uart_utils.h
* @author      Christian GAGNERAUD / cgag1c@freescale.com
* @date        Tue Jan 11 18:56:32 2005
* @brief       This file implement useful function to configure
*              UART
*/

#ifndef UART_UTILS_H
#define UART_UTILS_H

#include <termios.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
* Type of parity*/
typedef enum
{
        PARITY_UNKNOW = 0,
        PARITY_ODD = 1,
        PARITY_EVEN = 2,
        PARITY_NONE = 3,
} parity_type_t;

/**
* Type of flow control*/
typedef enum
{
        FLOW_CTRL_UNKNOW = 0,
        FLOW_CTRL_HARD = 1,
        FLOW_CTRL_SOFT = 2,
        FLOW_CTRL_NONE = 3
} flow_ctrl_t;

/**
* Type of equipment*/
typedef enum
{
        EQUIP_TYPE_TERM = 1,
        EQUIP_TYPE_COMM = 2
} equip_type_t;

/**
* High-level UART configuration for testing an UART*/
typedef struct
{
        char   *device;
        int     reader;
        int     writer;
        equip_type_t equip_type;
        int     baud_rate;
        parity_type_t parity_type;
        int     char_length;
        int     stop_bits;
        flow_ctrl_t flow_ctrl;
} uart_config_t;


/**
    * This function sets the flags of a termios struct according to a
    * given speed.
    *
    * @param bauds Speed (in bauds) as integer
    * @param termios Pointer to a termios struct to set the speed
    *
    * @return 0 on success, -1 otherwise
    */
extern int uart_setspeed(int bauds, struct termios *termios);

/**
    * This function sets the flags of a termios struct according to a
    * type of parity.
    *
    * @param parity_type Type of parity (odd, even or none)
    * @param termios Pointer to a termios struct to set the type of parity
    *
    * @return
    */
extern int uart_setpar(parity_type_t parity, struct termios *termios);

/**
    * This function sets the flags of a termios struct according to a
    * character length.
    *
    * @param char_length Chraracter length (7 or 8)
    * @param termios  Pointer to a termios struct to set the character length
    *
    * @return 0 on success, -1 otherwise.
    */
extern int uart_setstopb(int stop_bits, struct termios *termios);

/**
    * This function sets the flags of a termios struct according to a
    * number of stop bits.
    *
    * @param stop_bits Number of stop bits (1 or 2)
    * @param termios Pointer to a termios struct to set the number of stop bits.
    *
    * @return 0 on success, -1 otherwise
    */
extern int uart_setcharl(int char_len, struct termios *termios);

/**
    * This function sets the flags of a termios struct according to a
    * flow control type (hard, soft or none).
    *
    * @param flow_ctrl Flow control type
    * @param termios Pointer to a termios struct to set the flow control type
    *
    * @return 0 on success, -1 otherwise
    */
extern int uart_setflow(flow_ctrl_t flow_ctrl, struct termios *termios);

/**
    * This function fill in a termios struct according to the content of
    * an intermediate uart configuration descriptor structure
    *
    * @param uart_cfg Intermediate structure where to get the configuration from
    * @param termios Pointer to a termios struct to fill in.
    *
    * @return 0 on success, -1 otherwise (EINVAL).
    */
extern int uart_fill_termios(uart_config_t * uart_cfg, struct termios *termios);

/**
    * Compare the flags of 2 termios struct.
    *
    * @param termios1 First termios structure
    * @param termios2 Second termios structure
    *
    * @return 0 if the termios are equal, -1 otherwise
    *
    * @bug There's some problem with linux kernel headers and glibc headers,
    * it seems there are not sync'ed. So Kernel's termios struct is missing two
    * (unused) fields, and NCCS (number of control chars) is different (19 vs 32)
    */
extern int uart_cmp_termios_flags(struct termios *termios1, struct termios *termios2);

/**
    * Dump a termios structure on stdout.
    *
    * @param termios pointer to the termios to dump.
    * @param name Short string print as a name/description of the termios.
    */
extern void uart_dump_termios(struct termios *termios, char *name);


/**
    * Request the sender "stop sending" with hardware or software flow control
    * type.
    *
    * @param fd File descriptor of the serial port
    * @param flow_type Type of flow control
    *
    * @return 0 on success, -1 otherwise
    */
extern int uart_request_stop_flow(int fd, flow_ctrl_t flow_type);

/**
    * Request the sender  "(re)start sending" with hardware or software flow
    * control type.
    *
    * @param fd File descriptor of the serial port
    * @param flow_type Type of flow control
    *
    * @return 0 on success, -1 otherwise
    */
extern int uart_request_start_flow(int fd, flow_ctrl_t flow_type);

/**
    * Not UART related, but useful random function.
    *
    * @param max Maximum return value.
    *
    * @return random integer > max
    */
int     random_max(int max);

#ifdef __cplusplus
}
#endif

#endif                          /* UART_UTILS_H */
