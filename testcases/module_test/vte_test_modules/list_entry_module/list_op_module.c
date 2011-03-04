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
#include <linux/kernel.h> 
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Freescale Semiconductor, Inc.");

static unsigned list_cnt = 40;
module_param(list_cnt, uint, 0);
MODULE_PARM_DESC(list_cnt, "count of list");

static unsigned div = 20;
module_param(div, uint, 0);
MODULE_PARM_DESC(div, "timer divider");


struct test_list{
 struct list_head list;
 int data[32];
}; 

static struct test_data{
 struct timer_list *timer;
 int status;
} tdata;


static LIST_HEAD(tlist);

static void  test_tasklet(unsigned long data)
{
  struct test_list * plist;
	struct test_data * pdata = (struct test_data *)data;
	if(pdata->status){
		list_for_each_entry(plist, &tlist, list){
  		list_del_init(&plist->list);
  		list_del(&plist->list);
			break;
  	}
		//list_add(&plist->list, &tlist);
		if (unlikely(plist==NULL))
			kfree(plist);
	}
  return;
}
static DECLARE_TASKLET(list_let, test_tasklet, (unsigned long)&tdata);

#if 0
static void test_softirq_callback(struct softirq_action *h)
{
  /*test list entry operation here*/
   struct test_list * plist;
	 spinlock_t list_lock = SPIN_LOCK_UNLOCKED;
	 unsigned long flags;
  /*del them*/
  spin_lock_irqsave(&list_lock, flags);
	spin_unlock_irqrestore(&list_lock, flags);
	return;
}
#endif
static void timer_handle(unsigned long arg)
{
	if(tdata.status > 0)
	{
#if 0
  	raise_softirq(SCHED_SOFTIRQ);
#endif
		tasklet_schedule(&list_let);
	}
  mod_timer(tdata.timer, jiffies + HZ / div);
}


static int test_init(void)
{
   int ret;
	 int i;
   struct test_list * mlist; 
   struct test_list * plist, *temp_list; 
	 struct timer_list *timer;
   
	 printk(KERN_INFO "list entry test start\n");
   
/*install timer*/
  timer = kmalloc(sizeof(struct timer_list), GFP_KERNEL);
	if(NULL == timer)
		return -EINVAL;
  init_timer(timer);
  timer->data = 0;
  timer->expires = jiffies + HZ / div;
  timer->function = timer_handle;
  add_timer(timer);
  tdata.timer = timer;
 
/*softirq setting*/
#if 0
 /*softirq is not export in current kernel*/
 open_softirq(SCHED_SOFTIRQ, test_softirq_callback);
#endif


/*init the test list*/
	for (i = 0; i < list_cnt; i++) {
		 mlist = kzalloc(sizeof(*mlist), GFP_KERNEL);
		if (mlist == NULL) {
			ret = -ENOMEM;
			goto error;
		}
		list_add(&mlist->list, &tlist);
	}
  tdata.status = 1;

	 return 0;
error:
	list_for_each_entry_safe(plist, temp_list, &tlist, list) {
		list_del(&plist->list);
		kfree(plist);
	}
  return ret;
}

static void test_exit(void)
{
   struct test_list * plist, * temp_list;
	 printk(KERN_ALERT "list entry test module quit\n");	
	 tasklet_kill(&list_let);
   del_timer(tdata.timer);
   kfree(tdata.timer);
	 list_for_each_entry_safe(plist, temp_list, &tlist, list) {
		static int ct=0;
		printk("free count %d\n",ct++);
		list_del(&plist->list);
		kfree(plist);
	 }
    return;     
}

module_init(test_init);
module_exit(test_exit);
