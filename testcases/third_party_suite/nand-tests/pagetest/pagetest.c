/*
 * pagetest.c
 *
 * Test page read and write on MTD device.
 *
 * Copyright (C) 2005-2006 Nokia Corporation
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

#define PRINT_PREF KERN_CRIT "pagetest: "

/* Uncomment this if you have old MTD sources */
/* #define writesize oobblock */

static int dev = 0;
module_param(dev, int, S_IRUGO);
MODULE_PARM_DESC(dev, "MTD device number to use");

static struct mtd_info *mtd;

static unsigned char *twopages = 0;
static unsigned char *writebuf = 0;
static unsigned char *boundary = 0;

static unsigned char *bbt = 0;

static int pgsize;
static int bufsize;
static int ebcnt;
static int pgcnt;
static int errcnt = 0;

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

static inline void clear_data(unsigned char *buf,size_t len)
{
	memset(buf, 0, len);
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
	if (unlikely(err)) {
		printk(PRINT_PREF "error %d while erasing EB %d\n", err, ebnum);
		return err;
	}

	if (unlikely(ei.state == MTD_ERASE_FAILED)) {
		printk(PRINT_PREF "some erase error occurred at EB %d\n", ebnum);
		return -EIO;
	}

	return 0;
}

static inline int write_eraseblock(int ebnum)
{
	size_t written = 0;
	int err = 0;
	loff_t addr = ebnum * mtd->erasesize;

	set_random_data(writebuf, mtd->erasesize);
	cond_resched();
	err = mtd->write(mtd, addr, mtd->erasesize, &written, writebuf);
	if (unlikely(err || written != mtd->erasesize))
		printk(PRINT_PREF "error: write failed at 0x%08x\n", (unsigned) addr);

	return err;
}

#if 0
static void print_page(unsigned char *p)
{
	int i, j;

	for (i = 0; i < pgsize; ) {
		for (j = 0; i < pgsize && j < 32; ++i, ++j)
			printk("%02x", *p++);
		printk("\n");
	}
}
#endif

static inline int verify_eraseblock(int ebnum)
{
	u_int32_t j;
	size_t read = 0;
	int err = 0, i;
	loff_t addr0, addrn;
	loff_t addr = ebnum * mtd->erasesize;

	addr0 = 0;
	for (i = 0; bbt[i] && i < ebcnt; ++i)
		addr0 += mtd->erasesize;

	addrn = mtd->size;
	for (i = 0; bbt[ebcnt - i - 1] && i < ebcnt; ++i)
		addrn -= mtd->erasesize;

	set_random_data(writebuf, mtd->erasesize);
	for (j = 0; j < pgcnt - 1; ++j, addr += pgsize) {
		/* Do a read to set the internal dataRAMs to different data */
		err = mtd->read(mtd, addr0, bufsize, &read, twopages);
		if (err == -EUCLEAN)
			err = 0;
		if (unlikely(err || read != bufsize)) {
			printk(PRINT_PREF "error: read failed at 0x%08x\n", (unsigned) addr0);
			return err;
		}
		err = mtd->read(mtd, addrn - bufsize, bufsize, &read, twopages);
		if (err == -EUCLEAN)
			err = 0;
		if (unlikely(err || read != bufsize)) {
			printk(PRINT_PREF "error: read failed at 0x%08x\n", (unsigned) (addrn - bufsize));
			return err;
		}
		clear_data(twopages, bufsize);
		read = 0;
		err = mtd->read(mtd, addr, bufsize, &read, twopages);
		if (err == -EUCLEAN)
			err = 0;
		if (unlikely(err || read != bufsize)) {
			printk(PRINT_PREF "error: read failed at 0x%08x\n", (unsigned) addr);
			break;
		}
		if (unlikely(memcmp(twopages, writebuf + (j * pgsize), bufsize))) {
			printk(PRINT_PREF "error: verify failed at 0x%08x\n", (unsigned) addr);
			errcnt += 1;
		}
	}
	/* Check boundary between eraseblocks */
	if (addr <= addrn - pgsize - pgsize && !bbt[ebnum + 1]) {
		unsigned long oldnext = next;
		/* Do a read to set the internal dataRAMs to different data */
		err = mtd->read(mtd, addr0, bufsize, &read, twopages);
		if (err == -EUCLEAN)
			err = 0;
		if (unlikely(err || read != bufsize)) {
			printk(PRINT_PREF "error: read failed at 0x%08x\n", (unsigned) addr0);
			return err;
		}
		err = mtd->read(mtd, addrn - bufsize, bufsize, &read, twopages);
		if (err == -EUCLEAN)
			err = 0;
		if (unlikely(err || read != bufsize)) {
			printk(PRINT_PREF "error: read failed at 0x%08x\n", (unsigned) (addrn - bufsize));
			return err;
		}
		clear_data(twopages, bufsize);
		read = 0;
		err = mtd->read(mtd, addr, bufsize, &read, twopages);
		if (err == -EUCLEAN)
			err = 0;
		if (unlikely(err || read != bufsize)) {
			printk(PRINT_PREF "error: read failed at 0x%08x\n", (unsigned) addr);
			return err;
		}
		memcpy(boundary, writebuf + mtd->erasesize - pgsize, pgsize);
		set_random_data(boundary + pgsize, pgsize);
		if (unlikely(memcmp(twopages, boundary, bufsize))) {
			printk(PRINT_PREF "error: verify failed at 0x%08x\n", (unsigned) addr);
			errcnt += 1;
		}
		next = oldnext;
	}
	return err;
}

static int crosstest(void)
{
	size_t read = 0;
	int err = 0, i;
	loff_t addr, addr0, addrn;
	unsigned char *pp1, *pp2, *pp3, *pp4;

	printk(PRINT_PREF "crosstest\n");
	pp1 = kmalloc(pgsize * 4, GFP_KERNEL);
	if (!pp1) {
		printk(PRINT_PREF "error: cannot allocate memory\n");
		return -ENOMEM;
	}
	pp2 = pp1 + pgsize;
	pp3 = pp2 + pgsize;
	pp4 = pp3 + pgsize;
	clear_data(pp1, pgsize * 4);

	addr0 = 0;
	for (i = 0; bbt[i] && i < ebcnt; ++i)
		addr0 += mtd->erasesize;

	addrn = mtd->size;
	for (i = 0; bbt[ebcnt - i - 1] && i < ebcnt; ++i)
		addrn -= mtd->erasesize;

	/* Read 2nd-to-last page to pp1 */
	read = 0;
	addr = addrn - pgsize - pgsize;
	err = mtd->read(mtd, addr, pgsize, &read, pp1);
	if (err == -EUCLEAN)
		err = 0;
	if (unlikely(err || read != pgsize)) {
		printk(PRINT_PREF "error: read failed at 0x%08x\n", (unsigned) addr);
		kfree(pp1);
		return err;
	}

	/* Read 3rd-to-last page to pp1 */
	read = 0;
	addr = addrn - pgsize - pgsize - pgsize;
	err = mtd->read(mtd, addr, pgsize, &read, pp1);
	if (err == -EUCLEAN)
		err = 0;
	if (unlikely(err || read != pgsize)) {
		printk(PRINT_PREF "error: read failed at 0x%08x\n", (unsigned) addr);
		kfree(pp1);
		return err;
	}

	/* Read first page to pp2 */
	read = 0;
	addr = addr0;
	printk(PRINT_PREF "reading page at 0x%08x\n", (unsigned) addr);
	err = mtd->read(mtd, addr, pgsize, &read, pp2);
	if (err == -EUCLEAN)
		err = 0;
	if (unlikely(err || read != pgsize)) {
		printk(PRINT_PREF "error: read failed at 0x%08x\n", (unsigned) addr);
		kfree(pp1);
		return err;
	}
	/* Read last page to pp3 */
	read = 0;
	addr = addrn - pgsize;
	printk(PRINT_PREF "reading page at 0x%08x\n", (unsigned) addr);
	err = mtd->read(mtd, addr, pgsize, &read, pp3);
	if (err == -EUCLEAN)
		err = 0;
	if (unlikely(err || read != pgsize)) {
		printk(PRINT_PREF "error: read failed at 0x%08x\n", (unsigned) addr);
		kfree(pp1);
		return err;
	}
	/* Read first page again to pp4 */
	read = 0;
	addr = addr0;
	printk(PRINT_PREF "reading page at 0x%08x\n", (unsigned) addr);
	err = mtd->read(mtd, addr, pgsize, &read, pp4);
	if (err == -EUCLEAN)
		err = 0;
	if (unlikely(err || read != pgsize)) {
		printk(PRINT_PREF "error: read failed at 0x%08x\n", (unsigned) addr);
		kfree(pp1);
		return err;
	}
	/* pp2 and pp4 should be the same */
	printk(PRINT_PREF "verifying pages read at 0x%08x match\n", (unsigned) addr0);
	if (memcmp(pp2, pp4, pgsize)) {
		printk(PRINT_PREF "verify failed!\n");
		errcnt += 1;
	} else if (!err)
		printk(PRINT_PREF "crosstest ok\n");
	kfree(pp1);
	return err;
}

static int erasecrosstest(void)
{
	size_t read = 0, written = 0;
	int err = 0, i, ebnum, ok = 1, ebnum2;
	loff_t addr0;
	char *readbuf = twopages;

	printk(PRINT_PREF "erasecrosstest\n");

	ebnum = 0;
	addr0 = 0;
	for (i = 0; bbt[i] && i < ebcnt; ++i) {
		addr0 += mtd->erasesize;
		ebnum += 1;
	}

	ebnum2 = ebcnt - 1;
	while (ebnum2 && bbt[ebnum2])
		ebnum2 -= 1;

	printk(PRINT_PREF "erasing block %d\n", ebnum);
	err = erase_eraseblock(ebnum);
	if (unlikely(err))
		return err;

	printk(PRINT_PREF "writing 1st page of block %d\n", ebnum);
	set_random_data(writebuf, pgsize);
	strcpy(writebuf, "There is no data like this!");
	err = mtd->write(mtd, addr0, pgsize, &written, writebuf);
	if (unlikely(err || written != pgsize)) {
		printk(PRINT_PREF "error: write failed at 0x%08x\n", (unsigned) addr0);
		return err ? err : -1;
	}

	printk(PRINT_PREF "reading 1st page of block %d\n", ebnum);
	memset(readbuf, 0, pgsize);
	err = mtd->read(mtd, addr0, pgsize, &read, readbuf);
	if (err == -EUCLEAN)
		err = 0;
	if (unlikely(err || read != pgsize)) {
		printk(PRINT_PREF "error: read failed at 0x%08x\n", (unsigned) addr0);
		return err ? err : -1;
	}

	printk(PRINT_PREF "verifying 1st page of block %d\n", ebnum);
	if (memcmp(writebuf, readbuf, pgsize)) {
		printk(PRINT_PREF "verify failed!\n");
		errcnt += 1;
		ok = 0;
		return err;
	}

	printk(PRINT_PREF "erasing block %d\n", ebnum);
	err = erase_eraseblock(ebnum);
	if (unlikely(err))
		return err;

	printk(PRINT_PREF "writing 1st page of block %d\n", ebnum);
	set_random_data(writebuf, pgsize);
	strcpy(writebuf, "There is no data like this!");
	err = mtd->write(mtd, addr0, pgsize, &written, writebuf);
	if (unlikely(err || written != pgsize)) {
		printk(PRINT_PREF "error: write failed at 0x%08x\n", (unsigned) addr0);
		return err ? err : -1;
	}

	printk(PRINT_PREF "erasing block %d\n", ebnum2);
	err = erase_eraseblock(ebnum2);
	if (unlikely(err))
		return err;

	printk(PRINT_PREF "reading 1st page of block %d\n", ebnum);
	memset(readbuf, 0, pgsize);
	err = mtd->read(mtd, addr0, pgsize, &read, readbuf);
	if (err == -EUCLEAN)
		err = 0;
	if (unlikely(err || read != pgsize)) {
		printk(PRINT_PREF "error: read failed at 0x%08x\n", (unsigned) addr0);
		return err ? err : -1;
	}

	printk(PRINT_PREF "verifying 1st page of block %d\n", ebnum);
	if (memcmp(writebuf, readbuf, pgsize)) {
		printk(PRINT_PREF "verify failed!\n");
		errcnt += 1;
		ok = 0;
	}

	if (ok && !err)
		printk(PRINT_PREF "erasecrosstest ok\n");

	return err;
}

static int erasetest(void)
{
	size_t read = 0, written = 0;
	int err = 0, i, ebnum, ok = 1;
	loff_t addr0;

	printk(PRINT_PREF "erasetest\n");

	ebnum = 0;
	addr0 = 0;
	for (i = 0; bbt[i] && i < ebcnt; ++i) {
		addr0 += mtd->erasesize;
		ebnum += 1;
	}

	printk(PRINT_PREF "erasing block %d\n", ebnum);
	err = erase_eraseblock(ebnum);
	if (unlikely(err))
		return err;

	printk(PRINT_PREF "writing 1st page of block %d\n", ebnum);
	set_random_data(writebuf, pgsize);
	err = mtd->write(mtd, addr0, pgsize, &written, writebuf);
	if (unlikely(err || written != pgsize)) {
		printk(PRINT_PREF "error: write failed at 0x%08x\n", (unsigned) addr0);
		return err ? err : -1;
	}

	printk(PRINT_PREF "erasing block %d\n", ebnum);
	err = erase_eraseblock(ebnum);
	if (unlikely(err))
		return err;

	printk(PRINT_PREF "reading 1st page of block %d\n", ebnum);
	err = mtd->read(mtd, addr0, pgsize, &read, twopages);
	if (err == -EUCLEAN)
		err = 0;
	if (unlikely(err || read != pgsize)) {
		printk(PRINT_PREF "error: read failed at 0x%08x\n", (unsigned) addr0);
		return err ? err : -1;
	}

	printk(PRINT_PREF "verifying 1st page of block %d is all 0xff\n", ebnum);
	for (i = 0; i < pgsize; ++i)
		if (twopages[i] != 0xff) {
			printk(PRINT_PREF "verifying all 0xff failed at %d\n", i);
			errcnt += 1;
			ok = 0;
			break;
		}

	if (ok && !err)
		printk(PRINT_PREF "erasetest ok\n");

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

static int __init pagetest_init(void)
{
	int err;
	u_int32_t i;

	printk("\n");
	printk("=========================================================="
	       "===============================\n");
	printk("pagetest: dev = %d\n", dev);

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

	err = -ENOMEM;
	bufsize = pgsize * 2;
	writebuf = kmalloc(mtd->erasesize, GFP_KERNEL);
	if (!writebuf) {
		printk(PRINT_PREF "error: cannot allocate memory\n");
		goto out;
	}
	twopages = kmalloc(bufsize, GFP_KERNEL);
	if (!twopages) {
		printk(PRINT_PREF "error: cannot allocate memory\n");
		goto out;
	}
	boundary = kmalloc(bufsize, GFP_KERNEL);
	if (!boundary) {
		printk(PRINT_PREF "error: cannot allocate memory\n");
		goto out;
	}

	ebcnt = mtd->size / mtd->erasesize;
	pgcnt = mtd->erasesize / pgsize;

	bbt = kmalloc(ebcnt, GFP_KERNEL);
	if (!bbt) {
		printk(PRINT_PREF "error: cannot allocate memory\n");
		goto out;
	}
	memset(bbt, 0 , ebcnt);

	/* Scan for bad blocks */
	printk(PRINT_PREF "scanning for bad blocks\n");
	for (i = 0; i < ebcnt; ++i) {
		bbt[i] = is_block_bad(i) ? 1 : 0;
		if (i % 256 == 0)
			printk(PRINT_PREF "scanned %u\n", i);
		cond_resched();
	}
	printk(PRINT_PREF "scanned %u\n", i);

	/* Erase all eraseblocks */
	printk(PRINT_PREF "erasing\n");
	for (i = 0; i < ebcnt; ++i) {
		if (bbt[i])
			continue;
		err = erase_eraseblock(i);
		if (unlikely(err))
			goto out;
		if (i % 256 == 0)
			printk(PRINT_PREF "erased %u\n", i);
		cond_resched();
	}
	printk(PRINT_PREF "erased %u\n", i);

	simple_srand(1);

	/* Write all eraseblocks */
	printk(PRINT_PREF "writing\n");
	for (i = 0; i < ebcnt; ++i) {
		if (bbt[i])
			continue;
		err = write_eraseblock(i);
		if (unlikely(err))
			goto out;
		if (i % 256 == 0)
			printk(PRINT_PREF "written %u\n", i);
		cond_resched();
	}
	printk(PRINT_PREF "written %u\n", i);

	simple_srand(1);

	/* Check all eraseblocks */
	printk(PRINT_PREF "verifying\n");
	for (i = 0; i < ebcnt; ++i) {
		if (bbt[i])
			continue;
		err = verify_eraseblock(i);
		if (unlikely(err))
			goto out;
		if (i % 256 == 0)
			printk(PRINT_PREF "verified %u\n", i);
		cond_resched();
	}
	printk(PRINT_PREF "verified %u\n", i);

	err = crosstest();
	if (unlikely(err))
		goto out;

	err = erasecrosstest();
	if (unlikely(err))
		goto out;

	err = erasetest();
	if (unlikely(err))
		goto out;

	printk(PRINT_PREF "pagetest finished with %d errors\n", errcnt);

out:

	kfree(bbt);
	kfree(boundary);
	kfree(twopages);
	kfree(writebuf);

	put_mtd_device(mtd);

	if (err)
		printk(PRINT_PREF "error %d occurred\n", err);

	printk("=========================================================="
	       "===============================\n");

	return -1;
}
module_init(pagetest_init);

static void __exit pagetest_exit(void)
{
	return;
}
module_exit(pagetest_exit);

MODULE_DESCRIPTION("Page test module");
MODULE_AUTHOR("Adrian Hunter");
MODULE_LICENSE("GPL");
