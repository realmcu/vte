/*
 * speedtest.c
 *
 * Test read and write speed of a MTD device.
 *
 * Copyright (C) 2007 Nokia Corporation
 *
 * Author: Adrian Hunter <ext-adrian.hunter@nokia.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; see the file COPYING. If not, write to the Free Software
 * Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kobject.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/mtd/mtd.h>
#include <linux/sched.h>
#include <linux/jiffies.h>

#define PRINT_PREF KERN_CRIT "speedtest: "

/* Uncomment this if you have old MTD sources */
/* #define writesize oobblock */

static int dev = 0;
module_param(dev, int, S_IRUGO);
MODULE_PARM_DESC(dev, "MTD device number to use");

static struct mtd_info *mtd;

static unsigned char *iobuf = NULL;
static unsigned char *bbt = NULL;

static int pgsize;
static int ebcnt;
static int pgcnt;
static int goodebcnt;

static struct timeval start, finish;

static unsigned long next = 1;

static int simple_rand(void)
{
	next = next * 1103515245 + 12345;
	return ((unsigned) (next / 65536) % 32768);
}

static void simple_srand(unsigned long seed)
{
	next = seed;
}

static inline void set_random_data(unsigned char *buf,size_t len)
{
	size_t i;

	for (i = 0; i < len; ++i)
		buf[i] = simple_rand();
}

static inline int erase_eraseblock(int ebnum)
{
        int err;
	struct erase_info ei;
	loff_t addr = ebnum * mtd->erasesize;

	memset(&ei, 0, sizeof(struct erase_info));
	ei.mtd  = mtd;
	ei.addr = addr;
	ei.len  = mtd->erasesize;

	err = mtd->erase(mtd, &ei);
	if (err) {
		printk(PRINT_PREF "error %d while erasing EB %d\n", err, ebnum);
		return err;
	}

	if (ei.state == MTD_ERASE_FAILED) {
		printk(PRINT_PREF "some erase error occurred at EB %d\n", ebnum);
		return -EIO;
	}

	return 0;
}

static int erase_all(void)
{
	int i, err;

	/* Erase all eraseblocks */
	printk(PRINT_PREF "erasing\n");
	for (i = 0; i < ebcnt; ++i) {
		if (bbt[i])
			continue;
		err = erase_eraseblock(i);
		if (err)
			return err;
		cond_resched();
	}
	printk(PRINT_PREF "erased %d\n", goodebcnt);
	return 0;
}

static inline int write_eraseblock(int ebnum)
{
	size_t written = 0;
	int err = 0;
	loff_t addr = ebnum * mtd->erasesize;

	err = mtd->write(mtd, addr, mtd->erasesize, &written, iobuf);
	if (err || written != mtd->erasesize) {
		printk(PRINT_PREF "error: write failed at 0x%08x\n", (unsigned) addr);
		if (!err)
			err = -EINVAL;
	}

	return err;
}

static inline int write_eraseblock_by_page(int ebnum)
{
	size_t written = 0;
	int i, err = 0;
	loff_t addr = ebnum * mtd->erasesize;
	void *buf = iobuf;

	for (i = 0; i < pgcnt; i++) {
		err = mtd->write(mtd, addr, pgsize, &written, buf);
		if (err || written != pgsize) {
			printk(PRINT_PREF "error: write failed at 0x%08x\n", (unsigned) addr);
			if (!err)
				err = -EINVAL;
			break;
		}
		addr += pgsize;
		buf += pgsize;
	}

	return err;
}

static inline int write_eraseblock_by_2pages(int ebnum)
{
	size_t written = 0, sz = pgsize * 2;
	int i, n = pgcnt / 2, err = 0;
	loff_t addr = ebnum * mtd->erasesize;
	void *buf = iobuf;

	for (i = 0; i < n; i++) {
		err = mtd->write(mtd, addr, sz, &written, buf);
		if (err || written != sz) {
			printk(PRINT_PREF "error: write failed at 0x%08x\n", (unsigned) addr);
			if (!err)
				err = -EINVAL;
			return err;
		}
		addr += sz;
		buf += sz;
	}
	if (pgcnt % 2) {
		err = mtd->write(mtd, addr, pgsize, &written, buf);
		if (err || written != pgsize) {
			printk(PRINT_PREF "error: write failed at 0x%08x\n", (unsigned) addr);
			if (!err)
				err = -EINVAL;
		}
	}

	return err;
}

static inline int read_eraseblock(int ebnum)
{
	size_t read = 0;
	int err = 0;
	loff_t addr = ebnum * mtd->erasesize;

	err = mtd->read(mtd, addr, mtd->erasesize, &read, iobuf);
	/* Ignore corrected ECC errors */
	if (err == -EUCLEAN)
		err = 0;
	if (err || read != mtd->erasesize) {
		printk(PRINT_PREF "error: read failed at 0x%08x\n", (unsigned) addr);
		if (!err)
			err = -EINVAL;
	}

	return err;
}

static inline int read_eraseblock_by_page(int ebnum)
{
	size_t read = 0;
	int i, err = 0;
	loff_t addr = ebnum * mtd->erasesize;
	void *buf = iobuf;

	for (i = 0; i < pgcnt; i++) {
		err = mtd->read(mtd, addr, pgsize, &read, buf);
		/* Ignore corrected ECC errors */
		if (err == -EUCLEAN)
			err = 0;
		if (err || read != pgsize) {
			printk(PRINT_PREF "error: read failed at 0x%08x\n", (unsigned) addr);
			if (!err)
				err = -EINVAL;
			break;
		}
		addr += pgsize;
		buf += pgsize;
	}

	return err;
}

static inline int read_eraseblock_by_2pages(int ebnum)
{
	size_t read = 0, sz = pgsize * 2;
	int i, n = pgcnt / 2, err = 0;
	loff_t addr = ebnum * mtd->erasesize;
	void *buf = iobuf;

	for (i = 0; i < n; i++) {
		err = mtd->read(mtd, addr, sz, &read, buf);
		/* Ignore corrected ECC errors */
		if (err == -EUCLEAN)
			err = 0;
		if (err || read != sz) {
			printk(PRINT_PREF "error: read failed at 0x%08x\n", (unsigned) addr);
			if (!err)
				err = -EINVAL;
			return err;
		}
		addr += sz;
		buf += sz;
	}
	if (pgcnt % 2) {
		err = mtd->read(mtd, addr, pgsize, &read, buf);
		/* Ignore corrected ECC errors */
		if (err == -EUCLEAN)
			err = 0;
		if (err || read != pgsize) {
			printk(PRINT_PREF "error: read failed at 0x%08x\n", (unsigned) addr);
			if (!err)
				err = -EINVAL;
		}
	}

	return err;
}

static int is_block_bad(int ebnum)
{
	loff_t addr = ebnum * mtd->erasesize;
	int ret;

	ret = mtd->block_isbad(mtd, addr);
	if (ret)
		printk(PRINT_PREF "Block %d is bad\n", ebnum);
	return ret;
}

static inline void start_timing(void)
{
	do_gettimeofday(&start);
}

static inline void stop_timing(void)
{
	do_gettimeofday(&finish);
}

static long calc_speed(void)
{
	long ms, k, speed;

	ms = (finish.tv_sec - start.tv_sec) * 1000 +
	     (finish.tv_usec - start.tv_usec) / 1000;
	k = goodebcnt * mtd->erasesize / 1024;
	speed = (k * 1000) / ms;
	return speed;
}

static int __init speedtest_init(void)
{
	int err, i, bad;
	long speed;

	printk("\n");
	printk("=========================================================="
	       "===============================\n");
	printk("speedtest: ver 0.1 dev = %d\n", dev);

	mtd = get_mtd_device(NULL, dev);
	if (IS_ERR(mtd)) {
		err = PTR_ERR(mtd);
		printk(PRINT_PREF "error: Cannot get MTD device\n");
		return err;
	}

	if (mtd->writesize == 1) {
		printk(PRINT_PREF "warning: this test was written for NAND."
		       "Assume page size is 512 bytes.\n");
		pgsize = 512;
	} else
		pgsize = mtd->writesize;

	ebcnt = mtd->size / mtd->erasesize;
	pgcnt = mtd->erasesize / pgsize;

	printk(PRINT_PREF "Size=%u  EB size=%u  Write size=%u  EB count=%u  "
	       "Pages per EB=%u  Page size=%u\n",
	       mtd->size, mtd->erasesize, mtd->writesize, ebcnt, pgcnt, pgsize);

	err = -ENOMEM;

	iobuf = kmalloc(mtd->erasesize, GFP_KERNEL);
	if (!iobuf) {
		printk(PRINT_PREF "error: cannot allocate memory\n");
		goto out;
	}

	simple_srand(1);
	set_random_data(iobuf, mtd->erasesize);

	bbt = kmalloc(ebcnt, GFP_KERNEL);
	if (!bbt) {
		printk(PRINT_PREF "error: cannot allocate memory\n");
		goto out;
	}
	memset(bbt, 0 , ebcnt);

	/* Scan for bad blocks */
	printk(PRINT_PREF "scanning for bad blocks\n");
	bad = 0;
	for (i = 0; i < ebcnt; ++i) {
		bbt[i] = is_block_bad(i) ? 1 : 0;
		if (bbt[i])
			bad += 1;
		if (i % 256 == 0)
			printk(PRINT_PREF "scanned %d\n", i);
		cond_resched();
	}
	printk(PRINT_PREF "scanned %d, found %d bad\n", i, bad);
	goodebcnt = ebcnt - bad;

	/* Erase all eraseblocks */
	err = erase_all();
	if (err)
		goto out;

	/* Write all eraseblocks 1 eraseblock at a time */
	printk(PRINT_PREF "Testing eraseblock write speed\n");
	start_timing();
	for (i = 0; i < ebcnt; ++i) {
		if (bbt[i])
			continue;
		err = write_eraseblock(i);
		if (err)
			goto out;
		cond_resched();
	}
	stop_timing();
	speed = calc_speed();
	printk("eraseblock write speed is %ld KiB/s\n", speed);

	/* Read all eraseblocks 1 eraseblock at a time */
	printk(PRINT_PREF "Testing eraseblock read speed\n");
	start_timing();
	for (i = 0; i < ebcnt; ++i) {
		if (bbt[i])
			continue;
		err = read_eraseblock(i);
		if (err)
			goto out;
		cond_resched();
	}
	stop_timing();
	speed = calc_speed();
	printk("eraseblock read speed is %ld KiB/s\n", speed);

	/* Erase all eraseblocks */
	err = erase_all();
	if (err)
		goto out;

	/* Write all eraseblocks 1 page at a time */
	printk(PRINT_PREF "Testing page write speed\n");
	start_timing();
	for (i = 0; i < ebcnt; ++i) {
		if (bbt[i])
			continue;
		err = write_eraseblock_by_page(i);
		if (err)
			goto out;
		cond_resched();
	}
	stop_timing();
	speed = calc_speed();
	printk("page write speed is %ld KiB/s\n", speed);

	/* Read all eraseblocks 1 page at a time */
	printk(PRINT_PREF "Testing page read speed\n");
	start_timing();
	for (i = 0; i < ebcnt; ++i) {
		if (bbt[i])
			continue;
		err = read_eraseblock_by_page(i);
		if (err)
			goto out;
		cond_resched();
	}
	stop_timing();
	speed = calc_speed();
	printk("page read speed is %ld KiB/s\n", speed);

	/* Erase all eraseblocks */
	err = erase_all();
	if (err)
		goto out;

	/* Write all eraseblocks 2 pages at a time */
	printk(PRINT_PREF "Testing 2 page write speed\n");
	start_timing();
	for (i = 0; i < ebcnt; ++i) {
		if (bbt[i])
			continue;
		err = write_eraseblock_by_2pages(i);
		if (err)
			goto out;
		cond_resched();
	}
	stop_timing();
	speed = calc_speed();
	printk("2 page write speed is %ld KiB/s\n", speed);

	/* Read all eraseblocks 2 pages at a time */
	printk(PRINT_PREF "Testing 2 page read speed\n");
	start_timing();
	for (i = 0; i < ebcnt; ++i) {
		if (bbt[i])
			continue;
		err = read_eraseblock_by_2pages(i);
		if (err)
			goto out;
		cond_resched();
	}
	stop_timing();
	speed = calc_speed();
	printk("2 page read speed is %ld KiB/s\n", speed);

	/* Erase all eraseblocks */
	printk(PRINT_PREF "Testing erase speed\n");
	start_timing();
	for (i = 0; i < ebcnt; ++i) {
		if (bbt[i])
			continue;
		err = erase_eraseblock(i);
		if (err)
			goto out;
		cond_resched();
	}
	stop_timing();
	speed = calc_speed();
	printk("erase speed is %ld KiB/s\n", speed);

	printk(PRINT_PREF "speedtest finished\n");

out:

	kfree(iobuf);
	kfree(bbt);

	put_mtd_device(mtd);

	if (err)
		printk(PRINT_PREF "error %d occurred\n", err);

	printk("=========================================================="
	       "===============================\n");

	return -1;
}
module_init(speedtest_init);

static void __exit speedtest_exit(void)
{
	return;
}
module_exit(speedtest_exit);

MODULE_DESCRIPTION("Speed test module");
MODULE_AUTHOR("Adrian Hunter");
MODULE_LICENSE("GPL");
