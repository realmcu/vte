/*
 * Copyright 2010 Freescale Semiconductor, Inc. All rights reserved.
 */

/*
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

#ifndef MXS_PTP_TSET_H
#define MXS_PTP_TSET_H

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

#define PTP_COMD_SETCNT			0x1
#define PTP_COMD_GETCNT			0x2

#define PTP_GET_TX_TIMESTAMP		0x1
#define PTP_GET_RX_TIMESTAMP		0x2
#define PTP_SET_RTC_TIME		0x3
#define PTP_SET_COMPENSATION		0x4
#define PTP_GET_CURRENT_TIME		0x5
#define PTP_FLUSH_TIMESTAMP		0x6
#define PTP_ADJ_ADDEND			0x7
#define PTP_GET_ORIG_COMP		0x8
#define PTP_GET_ADDEND			0xB
#define PTP_GET_RX_TIMESTAMP_PDELAY_REQ		0xC
#define PTP_GET_RX_TIMESTAMP_PDELAY_RESP	0xD

/* PTP standard time representation structure */
struct ptp_time {
	u64 sec;	/* seconds */
	u32 nsec;	/* nanoseconds */
};

/* Structure for PTP Time Stamp */
struct fec_ptp_data_t {
	int		key;
	struct ptp_time	ts_time;
};

/* interface for PTP driver command GET_TX_TIME */
struct ptp_ts_data {
	/* PTP version */
	u8 version;
	/* PTP source port ID */
	u8 spid[10];
	/* PTP sequence ID */
	u16 seq_id;
	/* PTP message type */
	u8 message_type;
	/* PTP timestamp */
	struct ptp_time ts;
};

/* interface for PTP driver command SET_RTC_TIME/GET_CURRENT_TIME */
struct ptp_rtc_time {
	struct ptp_time rtc_time;
};

/* PTP message version */
#define PTP_1588_MSG_VER_1	1
#define PTP_1588_MSG_VER_2	2

#endif
