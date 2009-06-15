/*
 * scantest.c
 *
 * Test reading a MTD device.
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
#include <linux/vmalloc.h>

#define PRINT_PREF KERN_CRIT "scantest: "

/* Uncomment this if you have old MTD sources */
/* #define writesize oobblock */

static int dev = 0;
module_param(dev, int, S_IRUGO);
MODULE_PARM_DESC(dev, "MTD device number to use");

static struct mtd_info *mtd;

static unsigned char *iobuf = NULL;
static unsigned char *iooobbuf = NULL;
static unsigned char *bbt = NULL;

static int pgsize;
static int ebcnt;
static int pgcnt;
static int goodebcnt;

static inline int read_eraseblock_by_page(int ebnum)
{
	size_t read = 0;
	int i, ret, err = 0;
	loff_t addr = ebnum * mtd->erasesize;
	void *buf = iobuf;
	void *oobbuf = iooobbuf;

	for (i = 0; i < pgcnt; i++) {
		memset(buf, 0 , pgcnt);
		ret = mtd->read(mtd, addr, pgsize, &read, buf);
		if (ret == -EUCLEAN)
			ret = 0;
		if (ret || read != pgsize) {
			printk(PRINT_PREF "error: read failed at 0x%08x\n",
			       (unsigned) addr);
			if (!err)
				err = ret;
			if (!err)
				err = -EINVAL;
		}
		if (mtd->oobsize) {
			struct mtd_oob_ops ops;

			ops.mode      = MTD_OOB_PLACE;
			ops.len       = 0;
			ops.retlen    = 0;
			ops.ooblen    = mtd->oobsize;
			ops.oobretlen = 0;
			ops.ooboffs   = 0;
			ops.datbuf    = 0;
			ops.oobbuf    = oobbuf;
			ret = mtd->read_oob(mtd, addr, &ops);
			if (ret || ops.oobretlen != mtd->oobsize) {
				printk(PRINT_PREF "error: read oob failed at "
						  "0x%08x\n", (unsigned) addr);
				if (!err)
					err = ret;
				if (!err)
					err = -EINVAL;
			}
			oobbuf += mtd->oobsize;
		}
		addr += pgsize;
		buf += pgsize;
	}

	return err;
}

static void dump_eraseblock(int ebnum)
{
	int i, j, n;
	char line[128];
	int pg, oob;

	printk(PRINT_PREF "Dumping eraseblock %d\n", ebnum);
	n = mtd->erasesize;
	for (i = 0; i < n;) {
		char *p = line;

		p += sprintf(p, "%05x: ", (unsigned)i);
		for (j = 0; j < 32 && i < n; j++, i++)
			p += sprintf(p, "%02x", (unsigned)iobuf[i]);
		printk(KERN_CRIT "%s\n", line);
		cond_resched();
	}
	if (!mtd->oobsize)
		return;
	printk(PRINT_PREF "Dumping oob from eraseblock %d\n", ebnum);
	n = mtd->oobsize;
	for (pg = 0, i = 0; pg < pgcnt; pg++)
		for (oob = 0; oob < n;) {
			char *p = line;

			p += sprintf(p, "%05x: ", (unsigned)i);
			for (j = 0; j < 32 && oob < n; j++, oob++, i++)
				p += sprintf(p, "%02x", (unsigned)iooobbuf[i]);
			printk(KERN_CRIT "%s\n", line);
			cond_resched();
		}
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

static int __init scantest_init(void)
{
	int err = 0, i, bad;

	printk("\n");
	printk("=========================================================="
	       "===============================\n");
	printk("scantest: ver 0.1 dev = %d\n", dev);

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
	       "Pages per EB=%u  Page size=%u  Oob size=%u\n",
	       mtd->size, mtd->erasesize, mtd->writesize, ebcnt, pgcnt, pgsize,
	       mtd->oobsize);

	err = -ENOMEM;

	iobuf = vmalloc(mtd->erasesize);
	if (!iobuf) {
		printk(PRINT_PREF "error: cannot allocate memory\n");
		goto out;
	}

	iooobbuf = vmalloc(mtd->erasesize);
	if (!iooobbuf) {
		printk(PRINT_PREF "error: cannot allocate memory\n");
		goto out;
	}

	bbt = vmalloc(ebcnt);
	if (!bbt) {
		printk(PRINT_PREF "error: cannot allocate memory\n");
		goto out;
	}
	memset(bbt, 0 , ebcnt);

	err = 0;

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

	/* Read all eraseblocks 1 page at a time */
	printk(PRINT_PREF "Testing page read\n");
	for (i = 0; i < ebcnt; ++i) {
		int ret;

		if (bbt[i])
			continue;
		ret = read_eraseblock_by_page(i);
		if (ret) {
			dump_eraseblock(i);
			if (!err)
				err = ret;
		}
		cond_resched();
	}

	if (err)
		printk(PRINT_PREF "scantest finished with errors\n");
	else
		printk(PRINT_PREF "scantest finished\n");

out:

	vfree(iobuf);
	vfree(iooobbuf);
	vfree(bbt);

	put_mtd_device(mtd);

	if (err)
		printk(PRINT_PREF "error %d occurred\n", err);

	printk("=========================================================="
	       "===============================\n");

	return -1;
}
module_init(scantest_init);

static void __exit scantest_exit(void)
{
	return;
}
module_exit(scantest_exit);

MODULE_DESCRIPTION("Scan test module");
MODULE_AUTHOR("Adrian Hunter");
MODULE_LICENSE("GPL");
