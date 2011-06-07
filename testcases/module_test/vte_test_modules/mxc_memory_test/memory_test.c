/*
 * Copyright (C) 2011 Freescale Semiconductor, Inc. All Rights Reserved.
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
#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/types.h>
#include <linux/kernel.h> 
#include <linux/interrupt.h>
#include <linux/moduleparam.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/sched.h>
#include <asm/atomic.h>
#include <mach/hardware.h>


static unsigned vmsize = (1024*1024);
module_param(vmsize,uint,0);
MODULE_PARM_DESC(vmsize,"tested size of memory");

#define kmalloc_size (128 * 1024 - 16)
#define freepage_order 9
#define test_count 1000

static struct timeval start, finish;

static long calc_time(unsigned long size)
{
	long ms, speed;

	ms = (finish.tv_sec - start.tv_sec) * 1000 +
	     (finish.tv_usec - start.tv_usec) / 1000;
	speed = (size / 1024) / ms;
	return speed;
}


static int test_init(void)
{
	 int i = 0, count = test_count;
   unsigned long kernel_memaddr = 0; 
   unsigned long speed = 0; 
	 printk(KERN_INFO "memory test start\n");
   
/*test memory io*/
/*kmalloc test for 128k-16*/
  kernel_memaddr = (unsigned long)kmalloc(kmalloc_size,GFP_ATOMIC);
  if(!kernel_memaddr)
  {
    printk("Allocate memory failure!\n");
    return -ENOMEM;
  }
	do_gettimeofday(&start);
  while(count--){
  	i = kmalloc_size;
  	do{
   		*((char *)(kernel_memaddr + i - 1)) = 0xff;
  		smp_wmb();
  	}while(--i);
  }
	do_gettimeofday(&finish);
  speed = calc_time(kmalloc_size * test_count);
  printk(KERN_INFO "kmalloc memroy speed is %ld KB/s\n", speed);  
  kfree((void *)kernel_memaddr);
/*__get_free_pages */ 
  kernel_memaddr =__get_free_pages(GFP_KERNEL, freepage_order);
	if(!kernel_memaddr)
	{
    printk("Allocate memory failure!\n");
    return -ENOMEM;
	}
	count = test_count;
  do_gettimeofday(&start);
  while(count--){
  	i = (2^freepage_order)*PAGE_SIZE;
  	do{
   		*((char *)(kernel_memaddr + i - 1)) = 0xff;
  		smp_wmb();
  	}while(--i);
  }
	do_gettimeofday(&finish);
  speed = calc_time((2^freepage_order)*PAGE_SIZE * test_count);
  printk(KERN_INFO "get free apge memroy speed is %ld KB/s\n", speed);  
  free_pages(kernel_memaddr,freepage_order);
/*test memory mmap mode*/
/*
   SetPageReserved(virt_to_page(kernel_memaddr));
*/
	 return 0;
}

static void test_exit(void)
{
    return;     
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Freescale Semiconductor, Inc.");
module_init(test_init);
module_exit(test_exit);
