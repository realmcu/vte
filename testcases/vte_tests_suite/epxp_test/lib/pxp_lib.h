/*
 * pxp_lib - a user space library for PxP
 *
 * Copyright (C) 2010 Freescale Semiconductor, Inc.
 */
/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef	PXP_LIB_H
#define	PXP_LIB_H

#include <linux/pxp_dma.h>

#ifndef true
#define true    1
#endif
#ifndef false
#define false   0
#endif

typedef struct pxp_chan_handle {
	int chan_id;
	int hist_status;
} pxp_chan_handle_t;

int pxp_init();
void pxp_uninit();
int pxp_request_channel(pxp_chan_handle_t *pxp_chan);
void pxp_release_channel(pxp_chan_handle_t *pxp_chan);
int pxp_config_channel(pxp_chan_handle_t *pxp_chan, struct pxp_config_data *pxp_conf);
int pxp_start_channel(pxp_chan_handle_t *pxp_chan);
int pxp_wait_for_completion(pxp_chan_handle_t *pxp_chan, int times);
int pxp_get_mem(struct pxp_mem_desc *mem);
int pxp_put_mem(struct pxp_mem_desc *mem);

#endif
