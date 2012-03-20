/*
 * Copyright (C) 2012 Freescale Semiconductor, Inc. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>

MODULE_LICENSE("GPL");

#define MS_TO_NS(x)	(x * 1E6L)

static struct hrtimer hr_timer;

enum hrtimer_restart my_hrtimer_callback( struct hrtimer *timer )
{
  printk( "hrtimer_callback called (%ld).\n", jiffies );

  return HRTIMER_NORESTART;
}

int init_module( void )
{
  ktime_t ktime;
  unsigned long delay_in_ms = 200L;

  printk("HR Timer test module installing\n");

  ktime = ktime_set( 0, MS_TO_NS(delay_in_ms) );

  hrtimer_init( &hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL );
  
  hr_timer.function = &my_hrtimer_callback;

  printk( "Starting timer to fire in %ldms (%ld)\n", delay_in_ms, jiffies );

  hrtimer_start( &hr_timer, ktime, HRTIMER_MODE_REL );

  return 0;
}

void cleanup_module( void )
{
  int ret;

  ret = hrtimer_cancel( &hr_timer );
  if (ret) printk("The timer was still in use...\n");

  printk("HR Timer module uninstalling\n");

  return;
}
