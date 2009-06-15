/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Authors: Artem Bityutskiy, Jarkko Lavinen
 *
 * WARNING: this test program may kill your flash and your device. Do not
 * use it. Authors are not responsible for any damage caused by this
 * program.
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

#define PRINT_PREF KERN_CRIT "EB torture: "
#define RETRIES 3

/* Uncomment this if you have old MTD sources
#define writesize oobblock */

static int eb = 20;
module_param(eb, int, S_IRUGO);
MODULE_PARM_DESC(eb, "eraseblock number within the selected MTD device");

static int ebcnt = 32;
module_param(ebcnt, int, S_IRUGO);
MODULE_PARM_DESC(ebcnt, "number of consecutive eraseblocks to torture");

static int pgcnt = 0;
module_param(pgcnt, int, S_IRUGO);
MODULE_PARM_DESC(pgcnt, "number of pages per eraseblock to torture (0 => all)");

static int dev = 0;
module_param(dev, int, S_IRUGO);
MODULE_PARM_DESC(dev, "MTD device number to use");

static int gran = 1000;
module_param(gran, int, S_IRUGO);
MODULE_PARM_DESC(gran, "how often the status information should be printed");

static int check = 1;
module_param(check, int, S_IRUGO);
MODULE_PARM_DESC(check, "if the written data should be checked");

static unsigned int cycles_count = 0;
module_param(cycles_count, uint, S_IRUGO);
MODULE_PARM_DESC(cycles_count, "how many erase cycles to do (infinite by default)");

static struct mtd_info *mtd;

/* This buffer contains 0x555555...0xAAAAAA... pattern */
static unsigned char *patt_5A5;
/* This buffer contains 0xAAAAAA...0x555555... pattern */
static unsigned char *patt_A5A;
/* This buffer contains all 0xFF bytes */
static unsigned char *patt_FF;
/* This a temporary buffer is use when checking data */
static unsigned char *check_buf;
/* How many erase cycles were done */
static unsigned int erase_cycles;

static int pgsize;

static void report_corrupt(unsigned char *read, unsigned char *written);

/*
 * Erase eraseblock number @ebnum.
 */
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

/*
 * Check that the contents of eraseblock number @enbum is equivalent to the
 * @buf buffer.
 */
static inline int check_eraseblock(int ebnum, unsigned char *buf)
{
	int err, retries = 0;
	size_t read = 0;
	loff_t addr = ebnum * mtd->erasesize;
	size_t len = mtd->erasesize;

	if (pgcnt) {
		addr = (ebnum + 1) * mtd->erasesize - pgcnt * pgsize;
		len = pgcnt * pgsize;
	}

retry:
	err = mtd->read(mtd, addr, len, &read, check_buf);
	if (unlikely(err == -EUCLEAN))
		printk(PRINT_PREF "single bit flip occurred at EB %d "
		       "MTD reported that it was fixed.\n", ebnum);
	else if (unlikely(err)) {
		printk(PRINT_PREF "error %d while reading EB %d, "
		       "read %zd\n", err, ebnum, read);
		return err;
	}

	if (unlikely(read != len)) {
		printk(PRINT_PREF "failed to read %d bytes from EB %d, "
		       "read only %zd, but no error reported\n",
		       len, ebnum, read);
		return -EIO;
	}

	if (unlikely(memcmp(buf, check_buf, len))) {
		printk(PRINT_PREF "read wrong data from EB %d\n", ebnum);
		report_corrupt(check_buf, buf);

		if (retries++ < RETRIES) {
			/* Try read again */
			yield();
			printk(PRINT_PREF "re-try reading data from EB %d\n",
			       ebnum);
			goto retry;
		} else {
			printk(PRINT_PREF "retried %d times, still errors, "
			       "give-up\n", RETRIES);
			return -EINVAL;
		}
	}

	if (retries != 0)
		printk(PRINT_PREF "only attempt number %d was OK (!!!)\n", retries);

	return 0;
}

static inline int write_pattern(int ebnum, void *buf)
{
	int err;
	size_t written = 0;
	loff_t addr = ebnum * mtd->erasesize;
	size_t len = mtd->erasesize;

	if (pgcnt) {
		addr = (ebnum + 1) * mtd->erasesize - pgcnt * pgsize;
		len = pgcnt * pgsize;
	}
	err = mtd->write(mtd, addr, len, &written, buf);
	if (unlikely(err)) {
		printk(PRINT_PREF "error %d while writing EB %d, written %zd"
		      " bytes\n", err, ebnum, written);
		return err;
	}
	if (unlikely(written != len)) {
		printk(PRINT_PREF "written only %zd bytes of %d, but no error"
		       " reported\n", written, len);
		return -EIO;
	}

	return 0;
}

static int __init tort_init(void)
{
	int err = 0, i, infinite = !cycles_count;
	int bad_ebs[ebcnt];
	long start;

	printk("\n");
	printk("=========================================================="
	       "===============================\n");
	printk(PRINT_PREF "Torture %d eraseblocks (%d-%d) of mtd%d\n",
	       ebcnt, eb, eb + ebcnt - 1, dev);
	if (pgcnt)
		printk(PRINT_PREF "Torturing just %d pages per eraseblock\n",
			pgcnt);
	printk(PRINT_PREF "Write verify %s\n", check ? "enabled" : "disabled");
	printk(PRINT_PREF "HZ = %u\n",HZ);

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

	if (pgcnt && (pgcnt > mtd->erasesize / pgsize || pgcnt < 0)) {
		printk(PRINT_PREF "error: invalid pgcnt value %d\n", pgcnt);
		goto out_mtd;
	}

	err = -ENOMEM;
	patt_5A5 = kmalloc(mtd->erasesize, GFP_KERNEL);
	if (!patt_5A5) {
		printk(PRINT_PREF "error: cannot allocate memory\n");
		goto out_mtd;
	}

	patt_A5A = kmalloc(mtd->erasesize, GFP_KERNEL);
	if (!patt_A5A) {
		printk(PRINT_PREF "error: cannot allocate memory\n");
		goto out_patt_5A5;
	}

	patt_FF = kmalloc(mtd->erasesize, GFP_KERNEL);
	if (!patt_FF) {
		printk(PRINT_PREF "error: cannot allocate memory\n");
		goto out_patt_A5A;
	}

	check_buf = kmalloc(mtd->erasesize, GFP_KERNEL);
	if (!check_buf) {
		printk(PRINT_PREF "error: cannot allocate memory\n");
		goto out_patt_FF;
	}

	err = 0;

	/* Initialize patterns */
	memset(patt_FF, 0xFF, mtd->erasesize);
	for (i = 0; i < mtd->erasesize / pgsize; i++) {
		if (!(i & 1)) {
			memset(patt_5A5 + i * pgsize, 0x55, pgsize);
			memset(patt_A5A + i * pgsize, 0xAA, pgsize);
		} else {
			memset(patt_5A5 + i * pgsize, 0xAA, pgsize);
			memset(patt_A5A + i * pgsize, 0x55, pgsize);
		}
	}

	/*
	 * Check if there is a bad eraseblock among those we are going to test.
	 */
	memset(&bad_ebs[0], 0, sizeof(int) * ebcnt);
	if (mtd->block_isbad) {
		for(i = eb; i < eb + ebcnt; i++) {
			err = mtd->block_isbad(mtd,
					       (loff_t)i * mtd->erasesize);

			if (unlikely(err < 0)) {
				printk(PRINT_PREF "block_isbad() returned %d "
				       "for EB %d\n", err, i);
				goto out;
			}

			if (err) {
				printk("EB %d is bad. Skip it.\n", i);
				bad_ebs[i - eb] = 1;
			}
		}
	}

	start = jiffies;
	while (1) {
		int i;
		void *patt;

		/* Erase all eraseblocks */
		for(i = eb; i < eb + ebcnt; i++) {
			if (bad_ebs[i - eb])
				continue;
			err = erase_eraseblock(i);
			if (unlikely(err))
				goto out;
			cond_resched();
		}

		/* Check if the eraseblocks contain only 0xFF bytes */
		if (check) {
			for(i = eb; i < eb + ebcnt; i++) {
				if (bad_ebs[i - eb])
					continue;
				err = check_eraseblock(i, patt_FF);
				if (unlikely(err)) {
					printk(PRINT_PREF "verify failed"
					       " for 0xFF... pattern\n");
					goto out;
				}
				cond_resched();
			}
		}

		/* Write the pattern */
		for(i = eb; i < eb + ebcnt; i++) {
			if (bad_ebs[i - eb])
				continue;
			if ((eb + erase_cycles) & 1)
				patt = patt_5A5;
			else
				patt = patt_A5A;
			err = write_pattern(i, patt);
			if (unlikely(err))
				goto out;
			cond_resched();
		}

		/* Verify what we wrote */
		if (check) {
			for(i = eb; i < eb + ebcnt; i++) {
				if (bad_ebs[i - eb])
					continue;
				if ((eb + erase_cycles) & 1)
					patt = patt_5A5;
				else
					patt = patt_A5A;
				err = check_eraseblock(i, patt);
				if (unlikely(err)) {
					printk(PRINT_PREF "verify failed for %s"
					       " pattern\n",
					       ((eb + erase_cycles) & 1) ?
					       "0x55AA55..." : "0xAA55AA...");
					goto out;
				}
				cond_resched();
			}
		}

		erase_cycles += 1;

		if (unlikely(erase_cycles % gran == 0)) {
			long end = jiffies;
			unsigned long tm = end - start;

			printk(PRINT_PREF "- %08u erase cycles done. Timing: jiffies "
			       "%lu, milliseconds %u\n", erase_cycles, tm,
			       jiffies_to_msecs(tm));
			start = jiffies;
		}

		if (!infinite && --cycles_count == 0)
			break;
	}
out:

	printk(PRINT_PREF "torture finished after %u erase cycles\n",
	       erase_cycles);
	kfree(check_buf);
out_patt_FF:
	kfree(patt_FF);
out_patt_A5A:
	kfree(patt_A5A);
out_patt_5A5:
	kfree(patt_5A5);
out_mtd:
	put_mtd_device(mtd);
	if (err)
		printk(PRINT_PREF "error %d occurred during torturing\n", err);
	printk("=========================================================="
	       "===============================\n");
	return err;
}
module_init(tort_init);

static void __exit tort_exit(void)
{
	return;
}
module_exit(tort_exit);

static int countdiffs(unsigned char *buf, unsigned char *check_buf,
		      unsigned offset, unsigned len, unsigned *bytesp,
		      unsigned *bitsp);
static void print_bufs(unsigned char *read, unsigned char *written, int start,
		       int len);

/*
 * Report the detailed information about how the read EB differs from what was
 * written.
 */
static void report_corrupt(unsigned char *read, unsigned char *written)
{
	int i;
	int bytes, bits, pages, first;
	int offset, len;
	size_t check_len = mtd->erasesize;

	if (pgcnt)
		check_len = pgcnt * pgsize;

	bytes = bits = pages = 0;
	for(i = 0; i < check_len; i += pgsize)
		if (countdiffs(written, read, i, pgsize, &bytes,
			       &bits) >= 0)
			pages ++;

	printk(PRINT_PREF "Verify fails on %d pages, %d bytes/%d bits\n",
	       pages, bytes, bits);
	printk(PRINT_PREF "The following is a list of all differences between"
	       " what was read from flash and what was expected\n");

	for (i = 0; i < check_len; i += pgsize) {
		cond_resched();
		bytes = bits = 0;
		first = countdiffs(written, read, i, pgsize, &bytes,
				   &bits);
		if (first < 0)
			continue;

		printk("-------------------------------------------------------"
		       "----------------------------------\n");

		printk(PRINT_PREF "Page %d has %d bytes/%d bits failing verify, "
		       "starting at offset 0x%x\n",
		       (mtd->erasesize - check_len + i) / pgsize,
		       bytes, bits, first);

		offset = first & ~0x7;
		len    = ((first + bytes) | 0x7) + 1 - offset;

		print_bufs(read, written, offset, len);
	}
}

static void print_bufs(unsigned char *read, unsigned char *written, int start,
		       int len)
{
	int i = 0, j1, j2;
	char *diff;

	printk("Offset       Read                          Written\n");
	while(i < len) {
		printk("0x%08x: ", start + i);
		diff = "   ";
		for(j1 = 0; j1 < 8 && i + j1 < len; j1++) {
			printk(" %02x", read[start + i + j1]);
			if (read[start + i + j1] != written[start + i + j1])
				diff = "***";
		}

		while(j1 < 8) {
			printk(" ");
			j1 += 1;
		}

		printk("  %s ", diff);

		for(j2 = 0; j2 < 8 && i + j2 < len; j2++)
			printk(" %02x", written[start + i + j2]);
		printk("\n");
		i += 8;
	}
}

/*
 * Count the number of differing bytes and bits and return the first differing
 * offset.
 */
static int countdiffs(unsigned char *buf, unsigned char *check_buf,
		      unsigned offset, unsigned len, unsigned *bytesp,
		      unsigned *bitsp)
{
	unsigned i, bit;
	int first = -1;

	for (i = offset; i < offset + len; i++)
		if (buf[i] != check_buf[i]) {
			first = i;
			break;
		}

	while(i < offset + len) {
		if (buf[i] != check_buf[i]) {
			(*bytesp) ++;
			bit = 1;
			while(bit < 256) {
				if ((buf[i] & bit) != (check_buf[i] & bit))
					(*bitsp) ++;
				bit <<= 1;
			}
		}
		i++;
	}

	return first;
}

#define BYTES_PER_LINE 64

#if 0
/*
 * Dump the contents of @ptr buffer.
 */
static void hexdump(const void *ptr, int size)
{
	int i, k = 0;
	int rows, columns;
	const uint8_t *p = ptr;

	size = ALIGN(size, 4);
	rows = size/BYTES_PER_LINE + size % BYTES_PER_LINE;
	for (i = 0; i < rows; i++) {
		int j;

		columns = min(size - k, BYTES_PER_LINE) / 4;
		if (columns == 0)
			break;

		printk("%5d:  ", i*BYTES_PER_LINE);

		for (j = 0; j < columns; j++) {
			int n, N;

			N = size - k > 4 ? 4 : size - k;
			for (n = 0; n < N; n++)
				printk("%02x", p[k++]);
			printk(" ");
		}
		printk("\n");
	}
}
#endif

MODULE_DESCRIPTION("Eraseblock torturing module");
MODULE_AUTHOR("Artem Bityutskiy, Jarkko Lavinen");
MODULE_LICENSE("GPL");
