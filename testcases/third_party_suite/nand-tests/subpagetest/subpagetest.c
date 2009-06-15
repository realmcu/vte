/*
 * subpagetest.c
 *
 * Test sub-page read and write on MTD device.
 *
 * Copyright (C) 2005-2007 Nokia Corporation
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

#define PRINT_PREF KERN_CRIT "subpagetest: "

/* Uncomment this if you have old MTD sources */
/* #define writesize oobblock */

static int dev = 0;
module_param(dev, int, S_IRUGO);
MODULE_PARM_DESC(dev, "MTD device number to use");

static struct mtd_info *mtd;

static unsigned char *writebuf = 0;
static unsigned char *readbuf = 0;

static unsigned char *bbt = 0;

static int pgsize;
static int subpgsize;
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

static inline void simple_srand(unsigned long seed)
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

static int erase_eraseblock(int ebnum)
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

static int write_eraseblock(int ebnum)
{
	size_t written = 0;
	int err = 0;
	loff_t addr = ebnum * mtd->erasesize;

	set_random_data(writebuf, subpgsize);
	err = mtd->write(mtd, addr, subpgsize, &written, writebuf);
	if (unlikely(err || written != subpgsize)) {
		printk(PRINT_PREF "error: write failed at 0x%08x\n", (unsigned) addr);
		if (written != subpgsize) {
			printk(PRINT_PREF "  write size = 0x%08x\n", (unsigned) subpgsize);
			printk(PRINT_PREF "  written = 0x%08x\n", (unsigned) written);
		}
		return err ? err : -1;
	}

	addr += subpgsize;

	set_random_data(writebuf, subpgsize);
	err = mtd->write(mtd, addr, subpgsize, &written, writebuf);
	if (unlikely(err || written != subpgsize)) {
		printk(PRINT_PREF "error: write failed at 0x%08x\n", (unsigned) addr);
		if (written != subpgsize) {
			printk(PRINT_PREF "  write size = 0x%08x\n", (unsigned) subpgsize);
			printk(PRINT_PREF "  written = 0x%08x\n", (unsigned) written);
		}
		return err ? err : -1;
	}

	return err;
}

static int write_eraseblock2(int ebnum)
{
	size_t written = 0;
	int err = 0, k;
	loff_t addr = ebnum * mtd->erasesize;

	for (k = 1; k < 33; ++k) {
		if (addr + (subpgsize * k) > (ebnum + 1) * mtd->erasesize)
			break;
		set_random_data(writebuf, subpgsize * k);
		err = mtd->write(mtd, addr, subpgsize * k, &written, writebuf);
		if (unlikely(err || written != subpgsize * k)) {
			printk(PRINT_PREF "error: write failed at 0x%08x\n", (unsigned) addr);
			if (written != subpgsize) {
				printk(PRINT_PREF "  write size = 0x%08x\n", (unsigned) subpgsize * k);
				printk(PRINT_PREF "  written = 0x%08x\n", (unsigned) written);
			}
			return err ? err : -1;
		}
		addr += subpgsize * k;
	}

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
static void print_subpage(unsigned char *p)
{
	int i, j;

	for (i = 0; i < subpgsize; ) {
		for (j = 0; i < subpgsize && j < 32; ++i, ++j)
			printk("%02x", *p++);
		printk("\n");
	}
}

static int verify_eraseblock(int ebnum)
{
	size_t read = 0;
	int err = 0;
	loff_t addr = ebnum * mtd->erasesize;

	set_random_data(writebuf, subpgsize);
	clear_data(readbuf, subpgsize);
	read = 0;
	err = mtd->read(mtd, addr, subpgsize, &read, readbuf);
	if (unlikely(err || read != subpgsize)) {
		if (err == -EUCLEAN && read == subpgsize) {
			printk(PRINT_PREF "ECC correction at 0x%08x\n", (unsigned) addr);
			err = 0;
		} else {
			printk(PRINT_PREF "error: read failed at 0x%08x\n", (unsigned) addr);
			return err ? err : -1;
		}
	}
	if (unlikely(memcmp(readbuf, writebuf, subpgsize))) {
		printk(PRINT_PREF "error: verify failed at 0x%08x\n", (unsigned) addr);
		printk(PRINT_PREF "------------- written----------------\n");
		print_subpage(writebuf);
		printk(PRINT_PREF "------------- read ------------------\n");
		print_subpage(readbuf);
		printk(PRINT_PREF "-------------------------------------\n");
		errcnt += 1;
	}

	addr += subpgsize;

	set_random_data(writebuf, subpgsize);
	clear_data(readbuf, subpgsize);
	read = 0;
	err = mtd->read(mtd, addr, subpgsize, &read, readbuf);
	if (unlikely(err || read != subpgsize)) {
		if (err == -EUCLEAN && read == subpgsize) {
			printk(PRINT_PREF "ECC correction at 0x%08x\n", (unsigned) addr);
			err = 0;
		} else {
			printk(PRINT_PREF "error: read failed at 0x%08x\n", (unsigned) addr);
			return err ? err : -1;
		}
	}
	if (unlikely(memcmp(readbuf, writebuf, subpgsize))) {
		printk(PRINT_PREF "error: verify failed at 0x%08x\n", (unsigned) addr);
		printk(PRINT_PREF "------------- written----------------\n");
		print_subpage(writebuf);
		printk(PRINT_PREF "------------- read ------------------\n");
		print_subpage(readbuf);
		printk(PRINT_PREF "-------------------------------------\n");
		errcnt += 1;
	}

	return err;
}

static int verify_eraseblock2(int ebnum)
{
	size_t read = 0;
	int err = 0, k;
	loff_t addr = ebnum * mtd->erasesize;

	for (k = 1; k < 33; ++k) {
		if (addr + (subpgsize * k) > (ebnum + 1) * mtd->erasesize)
			break;
		set_random_data(writebuf, subpgsize * k);
		clear_data(readbuf, subpgsize * k);
		read = 0;
		err = mtd->read(mtd, addr, subpgsize * k, &read, readbuf);
		if (unlikely(err || read != subpgsize * k)) {
			if (err == -EUCLEAN && read == subpgsize * k) {
				printk(PRINT_PREF "ECC correction at 0x%08x\n", (unsigned) addr);
				err = 0;
			} else {
				printk(PRINT_PREF "error: read failed at 0x%08x\n", (unsigned) addr);
				return err ? err : -1;
			}
		}
		if (unlikely(memcmp(readbuf, writebuf, subpgsize * k))) {
			printk(PRINT_PREF "error: verify failed at 0x%08x\n", (unsigned) addr);
			errcnt += 1;
		}
		addr += subpgsize * k;
	}

	return err;
}

static int verify_eraseblock_ff(int ebnum)
{
	u_int32_t j;
	size_t read = 0;
	int err = 0;
	loff_t addr = ebnum * mtd->erasesize;

	memset(writebuf, 0xff, subpgsize);
	for (j = 0;j < mtd->erasesize / subpgsize;++j) {
		clear_data(readbuf, subpgsize);
		read = 0;
		err = mtd->read(mtd, addr, subpgsize, &read, readbuf);
		if (unlikely(err || read != subpgsize)) {
			if (err == -EUCLEAN && read == subpgsize) {
				printk(PRINT_PREF "ECC correction at 0x%08x\n", (unsigned) addr);
				err = 0;
			} else {
				printk(PRINT_PREF "error: read failed at 0x%08x\n", (unsigned) addr);
				return err ? err : -1;
			}
		}
		if (unlikely(memcmp(readbuf, writebuf, subpgsize))) {
			printk(PRINT_PREF "error: verify 0xff failed at 0x%08x\n", (unsigned) addr);
			errcnt += 1;
		}
		addr += subpgsize;
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

static int __init subpagetest_init(void)
{
	int err;
	u_int32_t i;

	printk("\n");
	printk("=========================================================="
	       "===============================\n");
	printk("subpagetest: dev = %d\n", dev);

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

	subpgsize = pgsize >> mtd->subpage_sft;
	printk("sub-page size is: %d\n", subpgsize);

	err = -ENOMEM;
	bufsize = subpgsize * 32;
	writebuf = kmalloc(bufsize, GFP_KERNEL);
	if (!writebuf) {
		printk(PRINT_PREF "error: cannot allocate memory\n");
		goto out;
	}
	readbuf = kmalloc(bufsize, GFP_KERNEL);
	if (!readbuf) {
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

	/* Check all eraseblocks */
	printk(PRINT_PREF "verifying 0xff\n");
	for (i = 0; i < ebcnt; ++i) {
		if (bbt[i])
			continue;
		err = verify_eraseblock_ff(i);
		if (unlikely(err))
			goto out;
		if (i % 256 == 0)
			printk(PRINT_PREF "verified %u\n", i);
		cond_resched();
	}
	printk(PRINT_PREF "verified %u\n", i);

	simple_srand(3);

	/* Write all eraseblocks */
	printk(PRINT_PREF "writing\n");
	for (i = 0; i < ebcnt; ++i) {
		if (bbt[i])
			continue;
		err = write_eraseblock2(i);
		if (unlikely(err))
			goto out;
		if (i % 256 == 0)
			printk(PRINT_PREF "written %u\n", i);
		cond_resched();
	}
	printk(PRINT_PREF "written %u\n", i);

	simple_srand(3);

	/* Check all eraseblocks */
	printk(PRINT_PREF "verifying\n");
	for (i = 0; i < ebcnt; ++i) {
		if (bbt[i])
			continue;
		err = verify_eraseblock2(i);
		if (unlikely(err))
			goto out;
		if (i % 256 == 0)
			printk(PRINT_PREF "verified %u\n", i);
		cond_resched();
	}
	printk(PRINT_PREF "verified %u\n", i);

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

	/* Check all eraseblocks */
	printk(PRINT_PREF "verifying 0xff\n");
	for (i = 0; i < ebcnt; ++i) {
		if (bbt[i])
			continue;
		err = verify_eraseblock_ff(i);
		if (unlikely(err))
			goto out;
		if (i % 256 == 0)
			printk(PRINT_PREF "verified %u\n", i);
		cond_resched();
	}
	printk(PRINT_PREF "verified %u\n", i);

	printk(PRINT_PREF "subpagetest finished with %d errors\n", errcnt);

out:

	kfree(bbt);
	kfree(readbuf);
	kfree(writebuf);

	put_mtd_device(mtd);

	if (err)
		printk(PRINT_PREF "error %d occurred\n", err);

	printk("=========================================================="
	       "===============================\n");

	return -1;
}
module_init(subpagetest_init);

static void __exit subpagetest_exit(void)
{
	return;
}
module_exit(subpagetest_exit);

MODULE_DESCRIPTION("Subpage test module");
MODULE_AUTHOR("Adrian Hunter");
MODULE_LICENSE("GPL");
