/**
* @file        uart_utils.c
* @author      Christian GAGNERAUD / cgag1c@freescale.com
* @date        Tue Jan 11 18:54:34 2005
* @brief       This file implement useful function to configure
*              UART and handle some operations.
*/

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <string.h>     /* for memcpy() */

/* Harness Specific Include Files. */
#include "test.h"

#include "uart_utils.h"

/* does uart_dump_termios should print the c_cc[NCCS] content? */
#define DUMP_TERMIOS_PRINT_CC_ARRAY 1


int uart_setspeed(int bauds, struct termios *termios)
{
        int     speed = B0;

        switch (bauds)
        {
        case 0:
                speed = B0;
                break;
        case 50:
                speed = B50;
                break;
        case 75:
                speed = B75;
                break;
        case 110:
                speed = B110;
                break;
        case 134:
                speed = B134;
                break;
        case 150:
                speed = B150;
                break;
        case 200:
                speed = B200;
                break;
        case 300:
                speed = B300;
                break;
        case 600:
                speed = B600;
                break;
        case 1200:
                speed = B1200;
                break;
        case 1800:
                speed = B1800;
                break;
        case 2400:
                speed = B2400;
                break;
        case 4800:
                speed = B4800;
                break;
        case 9600:
                speed = B9600;
                break;
        case 19200:
                speed = B19200;
                break;
        case 38400:
                speed = B38400;
                break;
        case 57600:
                speed = B57600;
                break;
        case 115200:
                speed = B115200;
                break;
        case 230400:
                speed = B230400;
                break;
        case 460800:
                speed = B460800;
                break;
        case 500000:
                speed = B500000;
                break;
        case 576000:
                speed = B576000;
                break;
        case 921600:
                speed = B921600;
                break;
        case 1000000:
                speed = B1000000;
                break;
        case 1152000:
                speed = B1152000;
                break;
        case 1500000:
                speed = B1500000;
                break;
        case 2000000:
                speed = B2000000;
                break;
        case 2500000:
                speed = B2500000;
                break;
        case 3000000:
                speed = B3000000;
                break;
        case 3500000:
                speed = B3500000;
                break;
        case 4000000:
                speed = B4000000;
                break;
        default:
                return -1;
                break;
        }
        if (cfsetspeed(termios, speed) != 0)
        {
                return -1;
        }
        return 0;
}

int uart_setpar(parity_type_t parity_type, struct termios *termios)
{
        switch (parity_type)
        {
        case PARITY_NONE:
                termios->c_cflag &= ~PARENB;
                break;
        case PARITY_ODD:
                termios->c_cflag |= (PARENB | PARODD);
                break;
        case PARITY_EVEN:
                termios->c_cflag |= PARENB;
                termios->c_cflag &= ~PARODD;
                break;
        default:
                return -1;
                break;
        }
        return 0;
}

int uart_setcharl(int char_length, struct termios *termios)
{
        switch (char_length)
        {
        case 7:
                termios->c_cflag &= ~CSIZE;
                termios->c_cflag |= CS7;
                break;
        case 8:
                termios->c_cflag &= ~CSIZE;
                termios->c_cflag |= CS8;
                break;
        default:
                return -1;
                break;
        }
        return 0;
}

int uart_setstopb(int stop_bits, struct termios *termios)
{
        switch (stop_bits)
        {
        case 1:
                termios->c_cflag &= ~CSTOPB;
                break;
        case 2:
                termios->c_cflag |= CSTOPB;
                break;
        default:
                return -1;
                break;
        }
        return 0;
}

int uart_setflow(flow_ctrl_t flow_ctrl, struct termios *termios)
{
        switch (flow_ctrl)
        {
        case FLOW_CTRL_NONE:
                termios->c_cflag &= ~CRTSCTS;
                termios->c_iflag &= ~(IXON | IXOFF | IXANY);
                break;
        case FLOW_CTRL_HARD:
                termios->c_cflag |= CRTSCTS;
                termios->c_iflag &= ~(IXON | IXOFF | IXANY);
                break;
        case FLOW_CTRL_SOFT:
                termios->c_cflag &= ~CRTSCTS;
                termios->c_iflag |= IXON | IXOFF;
                termios->c_iflag &= ~(IXANY);
                break;
        default:
                return -1;
                break;
        }
        return 0;
}

int uart_initctrlchar(flow_ctrl_t flow_ctrl, struct termios *termios)
{
        termios->c_cc[VTIME] = 0;
        termios->c_cc[VMIN] = 1;

        if (flow_ctrl == FLOW_CTRL_SOFT)
        {
                termios->c_cc[VSTART] = CSTART;
                termios->c_cc[VSTOP] = CSTOP;
        }
        else
        {
                termios->c_cc[VSTART] = _POSIX_VDISABLE;
                termios->c_cc[VSTOP] = _POSIX_VDISABLE;
        }

        termios->c_cc[VINTR] = _POSIX_VDISABLE; // CINTR;
        termios->c_cc[VQUIT] = _POSIX_VDISABLE; // CQUIT;
        termios->c_cc[VERASE] = _POSIX_VDISABLE;        // CERASE;
        termios->c_cc[VKILL] = _POSIX_VDISABLE; // CKILL;
        termios->c_cc[VEOF] = _POSIX_VDISABLE;  // CEOF;
        termios->c_cc[VSWTC] = _POSIX_VDISABLE; // CSWTC;
        termios->c_cc[VSUSP] = _POSIX_VDISABLE; // CSUSP;
        termios->c_cc[VEOL] = _POSIX_VDISABLE;  // CEOL;
        termios->c_cc[VREPRINT] = _POSIX_VDISABLE;      // CREPRINT;
        termios->c_cc[VDISCARD] = _POSIX_VDISABLE;      // CDISCARD;
        termios->c_cc[VWERASE] = _POSIX_VDISABLE;       // CWERASE;
        termios->c_cc[VLNEXT] = _POSIX_VDISABLE;        // CLNEXT;
        termios->c_cc[VEOL2] = _POSIX_VDISABLE; // CEOL;
        return 0;
}

int uart_fill_termios(uart_config_t * uart_cfg, struct termios *termios)
{
        /* We must fill in all the flags to get plain control. */
        bzero((void *) termios, sizeof(*termios));

        /* - set bauds */
        if (uart_setspeed(uart_cfg->baud_rate, termios) != 0)
        {
                return -1;
        }

        /* - parity */
        if (uart_setpar(uart_cfg->parity_type, termios) != 0)
        {
                return -1;
        }

        /* - char length */
        if (uart_setcharl(uart_cfg->char_length, termios) != 0)
        {
                return -1;
        }

        /* - stop bits */
        if (uart_setstopb(uart_cfg->stop_bits, termios) != 0)
        {
                return -1;
        }

        /* - Flow control */
        if (uart_setflow(uart_cfg->flow_ctrl, termios) != 0)
        {
                return -1;
        }

        /* Control chars */
        uart_initctrlchar(uart_cfg->flow_ctrl, termios);

        // uart_dump_termios(termios, uart_cfg->device);
        return 0;
}

int uart_cmp_termios_flags(struct termios *termios1, struct termios *termios2)
{
        int     i;

        if (termios1->c_iflag != termios2->c_iflag)
        {
                return -1;
        }
        if (termios1->c_oflag != termios2->c_oflag)
        {
                return -1;
        }
        if (termios1->c_cflag != termios2->c_cflag)
        {
                return -1;
        }
        if (termios1->c_lflag != termios2->c_lflag)
        {
                return -1;
        }
        if (termios1->c_line != termios2->c_line)
        {
                return -1;
        }
        /** @bug
        *  NCCS is 19 in linux_src/include/asm-arm/termbits.h
        *  NCCS is 32 in glibc_prefix/include/bits/termios.h
        */
        for (i = 0; i < NCCS; i++)
                if (termios1->c_cc[i] != termios2->c_cc[i])
                        return -1;
                        /** @bug
                        *  c_ispeed and c_ospeed not used/declared in kernel headers !(?)
                        *  but declared/used in glibc headers.
                        *  see linux_src/include/asm-arm/termbits.h
                        *  glibc_prefix/include/bits/termios.h
                */
#if 1
        if (termios1->c_ispeed != termios2->c_ispeed)
        {
                return -1;
        }
        if (termios1->c_ospeed != termios2->c_ospeed)
        {
                return -1;
        }
#endif

        return 0;
}

void uart_dump_termios(struct termios *termios, char *name)
{
        int     i;

        printf("Termios: %s\n"
               "  c_iflag  = 0x%08X\n"
               "  c_oflag  = 0x%08X\n"
               "  c_cflag  = 0x%08X\n"
               "  c_lflag  = 0x%08X\n"
               "  c_line   =       0x%02X\n",
               name,
               termios->c_iflag,
               termios->c_oflag, termios->c_cflag, termios->c_lflag, termios->c_line);
#ifdef DUMP_TERMIOS_PRINT_CC_ARRAY
        printf("  c_cc     = [ ");
        for (i = 0; i < NCCS; i++)
        {
                printf("0x%02X", termios->c_cc[i]);
                if ((i + 1) % 8 == 0)
                {
                        if (i != (NCCS - 1))
                                printf("\n               ");
                        else
                                printf(" ]\n");
                }
                else
                        printf(" ");
        }
#else
        printf("  c_cc     = [...]\n");
#endif
        printf("  c_ispeed = 0x%08X\n"
               "  c_ospeed = 0x%08X\n", termios->c_ispeed, termios->c_ospeed);
}

int uart_request_stop_flow(int fd, flow_ctrl_t flow_type)
{
        int     status;

        switch (flow_type)
        {
        case FLOW_CTRL_UNKNOW:
        case FLOW_CTRL_NONE:
                return -1;
                break;
        case FLOW_CTRL_HARD:
                /* Negate RequestToSend */
                if (ioctl(fd, TIOCMGET, &status) < 0)
                {
                        perror("TIOCMGET");
                        return -1;
                }
                status &= ~TIOCM_RTS;
                if (ioctl(fd, TIOCMSET, &status) < 0)
                {
                        perror("TIOCMSET");
                        return -1;
                }
                tst_resm(TINFO, "HW FLOW: stop");
                break;
        case FLOW_CTRL_SOFT:
                /* Send a 'STOP' character */
                if (tcflow(fd, TCIOFF) < 0)
                {
                        perror("tcflow(TCIOFF)");
                        return -1;
                }
                tst_resm(TINFO, "SW FLOW: stop");
                break;
        }
        return 0;
}

int uart_request_start_flow(int fd, flow_ctrl_t flow_type)
{
        int     status;

        switch (flow_type)
        {
        case FLOW_CTRL_UNKNOW:
        case FLOW_CTRL_NONE:

                return -1;
                break;
        case FLOW_CTRL_HARD:
                /* Assert RequestToSend */
                if (ioctl(fd, TIOCMGET, &status) < 0)
                {
                        perror("TIOCMGET");
                        return -1;
                }
                status |= TIOCM_RTS;
                if (ioctl(fd, TIOCMSET, &status) < 0)
                {
                        perror("TIOCMSET");
                        return -1;
                }
                tst_resm(TINFO, "HW FLOW: start");
                break;
        case FLOW_CTRL_SOFT:
                /* Send a 'START' character */
                if (tcflow(fd, TCION) < 0)
                {
                        perror("tcflow(TCION)");
                        return -1;
                }
                tst_resm(TINFO, "SW FLOW: start");
                break;
        }
        return 0;
}


int random_max(int max)
{
        double  d;

        d = (double) max *rand();

        d = d / (RAND_MAX + 1.0);
        return (int) d;
}
