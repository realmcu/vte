/****************************************************************************
 *             *
 *     COPYRIGHT (c) 1990 - 2000       *
 *      This Software Provided       *
 *         By         *
 *     Robin's Nest Software Inc.       *
 *          2 Paradise Lane     *
 *          Hudson, NH 03051        *
 *          (603) 883-2355        *
 *             *
 * Permission to use, copy, modify, distribute and sell this software and   *
 * its documentation for any purpose and without fee is hereby granted,     *
 * provided that the above copyright notice appear in all copies and that   *
 * both that copyright notice and this permission notice appear in the     *
 * supporting documentation, and that the name of the author not be used    *
 * in advertising or publicity pertaining to distribution of the software   *
 * without specific, written prior permission.        *
 *             *
 * THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,     *
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN     *
 * NO EVENT SHALL HE BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL   *
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR    *
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS  *
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF   *
 * THIS SOFTWARE.           *
 *             *
 ****************************************************************************/
/*
 * Module: dtinfo.c
 * Author: Robin T. Miller
 * Date: September 8, 1993
 *
 * Description:
 * Setup device and/or system information for 'dt' program.
 */
#include "dt.h"
#if !defined(_QNX_SOURCE)
#  if !defined(sun)
#    include <sys/ioctl.h>
#  endif /* !defined(sun) */
#endif /* !defined(_QNX_SOURCE) */
#include <sys/stat.h>

/*
 * Modification History:
 *
 * June 2nd, 2001 by Robin Miller.
 * After setting up a "regular" device type, ensure the device
 * defaults also get setup.  Was not setting  min/max/incr defaults.
 *
 * April 13th, 2001 by Robin Miller.
 * Removed old PTOS conditionalization (Tru64 Unix V4.0+ only now).
 * If supported, use DEVGETINFO instead of DEVIOCGET for longer
 * device names, and more information.  If max_capacity, set by the user
 * or random I/O and partition 'c' selected, set max capacity returned
 * from the device driver.  For random I/O, prevents lseek/read logic.
 *
 * April 12th, 2001 by Robin Miller.
 * Delay setting device defaults, until later in setup_device_info().
 * When random I/O is selected, setup device values for a disk device.
 *
 * April 11th, 2001 by Robin Miller.
 * For Tru64 Unix, if the user specified a device type, don't ovrride
 * it by calling dec_system_device_info().  If the user selected random I/O
 * options, enable random access mode.  Hey, the worse that can happen is
 * they'll get an lseek() error!  This handles special disk devices (LSM).
 *
 * February 6th, 2001 by Robin Miller.
 * For Cygnus, map //./ and \\.\ directory prefixes to /dev. This
 * is how physical (raw) devices *must* be mounted to work properly.
 * If physical devices aren't mounted, Unix style EOM isn't emulated!
 * Ensure same processing is performed on existing files and newly
 * created files.  This includes setting the fsync flag appropriately,
 * and setting up the user capacity for random I/O as necessary.
 *
 * February 1st, 2001 by Robin Miller.
 * Make changes to allow multiple slices to an existing file.
 *
 * January 28th, 2001 by Robin Miller.
 * Allow dec_system_device_info() to be called prior to the file
 * being opened.  We need device info early on for new slices option.
 *
 * January 25th, 2001 by Robin Miller.
 * Flag disk devices, via di_random_access boolean flag.  This
 * includes regular files, so seek and report functions work on them.
 *
 * January 24th, 2001 by Robin Miller.
 * Setup the default device block size from device_size variable,
 * so the user can override this using the "dsize=value" option.  Also,
 * various functions no longer have to check for this field being zero,
 * so perhaps this will help performance too.
 *
 * January 2nd, 2001 by Robin Miller.
 *      Make changes to build using MKS/NuTCracker product.
 *
 * August 22nd, 2000 by Robin Miller.
 * Update setup_device_defaults() to better set incr and min values,
 * since this function may get called twice on Tru64 Unix systems.
 *
 * July 14th, 2000 by Robin Miller.
 * Added logic to set file sync flag, if a block device or regular
 * file is used.  Tapes, FIFO's, and other devices do NOT like fsync()!
 *
 * April 2nd, 2000 by Robin Miller.
 * Only set the random data limit when stat'ing the file,
 * if it's a regular file, since the st_size might not be bytes.
 * On QNX, st_size is the drive capacity, so setup user_capacity
 * to avoid seek/read algorithm in FindCapacity() for random I/O.
 *
 * August 1, 1999 by Robin Miller.
 * Do a better job in setup_device_defaults() setting up the
 * defaults for min and increment counts.  For disks, this value
 * needs to be modulo the block size, since most disk drivers do
 * not permit non-modulo requests (Tru64 UNIX CAM is exception).
 *
 * December 21, 1998 by Robin Miller.
 * - for DUNIX, allocate mtget structure for tapes.
 * - changes for Malloc(), which now clears memory.
 *
 * December 19, 1998 by Robin Miller.
 * For Steel DUNIX, strip trailing spaces from device names.
 *
 * October 29, 1998 by Robin Miller.
 * Implement a random I/O data limit, instead of using the normal
 * data limit variable (not good to dual purpose this value).
 *
 * April 29, 1998 by Robin Miller.
 * Add support for an alternate device directory.
 *
 * October 31, 1993 by Robin Miller.
 * Enhance device type setup and honor user specified device type.
 *
 * October 29, 1993 by Robin Miller.
 * Added more checks in setup_device_type() to determine detect
 * selection of terminal devices (e.g., console device).
 *
 * September 15, 1993 by Robin Miller.
 * Added stat() of file specified to check for FIFO's and set the
 * device type appropriately so other checks could be done prior
 * to opening the device (O_NONBLOCK needs set before open()).
 *
 */

/*
 * Forward References:
 */
static void setup_device_defaults(struct dinfo *dip);
static void SetupRegularFile(struct dinfo *dip, struct stat *sbp);
#if defined(__MSDOS__) || defined(__WIN32__) || defined(_NT_SOURCE)
static bool IsDriveLetter(char *bufptr);
#endif /* defined(__MSDOS__) || defined(__WIN32__) || defined(_NT_SOURCE) */
#if defined(ultrix) || defined(DEC)
static int SetupDiskAttributes(struct dinfo *dip, int fd);
#endif /* defined(ultrix) || defined(DEC) */

/************************************************************************
 *         *
 * setup_device_type() - Setup the device type.    *
 *         *
 * Description:        *
 * This function sets up the specific device type to be tested. *
 * Since each operating system has a different method of identifying *
 * devices, so this table lookup allows the user to specify the device *
 * type.  Of course, this could be used to force errors.  *
 *         *
 * Inputs: str = Pointer to device type string.   *
 *         *
 * Return Value:       *
 *  Returns pointer to device type table entry or NULL. *
 *         *
 ************************************************************************/
struct dtype dtype_table[] = {
 { "audio", DT_AUDIO },
 { "block", DT_BLOCK },
 { "character", DT_CHARACTER },
 { "comm", DT_COMM  },
 { "disk", DT_DISK  },
 { "graphics", DT_GRAPHICS },
 { "memory", DT_MEMORY },
 { "mmap", DT_MMAP  },
 { "mouse", DT_MOUSE },
 { "network", DT_NETWORK },
 { "fifo", DT_FIFO  },
 { "pipe", DT_PIPE  },
 { "printer", DT_PRINTER },
 { "processor", DT_PROCESSOR },
 { "regular", DT_REGULAR },
 { "socket", DT_SOCKET },
 { "special", DT_SPECIAL },
 { "streams", DT_STREAMS },
 { "tape", DT_TAPE  },
 { "terminal", DT_TERMINAL },
 { "unknown", DT_UNKNOWN }
};
int num_dtypes = sizeof(dtype_table) / sizeof(dtype_table[0]);

struct dtype *
setup_device_type (char *str)
{
    int i;
    struct dtype *dtp;

    for (dtp = dtype_table, i = 0; i < num_dtypes; i++, dtp++) {
 if (strcmp(str, dtp->dt_type) == 0) {
     return (dtp);
 }
    }
    fprintf (stderr, "Device type '%s' is invalid, valid entrys are:\n", str);
    for (dtp = dtype_table, i = 0; i < num_dtypes; i++, dtp++) {
 if ( (i % 4) == 0) fprintf (stderr, "\n");
 fprintf (stderr, "    %-12s", dtp->dt_type);
    }
    fprintf (stderr, "\n");
    return ((struct dtype *) 0);
}

/************************************************************************
 *         *
 * setup_device_defaults() - Setup the device defaults.   *
 *         *
 * Description:        *
 * This function sets up the specific device type defaults, for *
 * test parameters which were not specified.    *
 *         *
 * Inputs: dip = The device information pointer.   *
 *         *
 * Return Value:       *
 *  Returns pointer to device type table entry or NULL. *
 *         *
 * Note: This function may get called twice!  On Tru64 Unix, after we *
 *  open the device, the initial device type may get overridden. *
 *         *
 ************************************************************************/
static void
setup_device_defaults (struct dinfo *dip)
{
    struct dtype *dtp = dip->di_dtype;

    if ( (dtp->dt_dtype == DT_BLOCK)   ||
  (dtp->dt_dtype == DT_DISK)    ||
  (dtp->dt_dtype == DT_REGULAR) || dip->di_random_io ) {
 /*
  * Real device size should already been obtained.
  */
 if (!device_size) device_size = BLOCK_SIZE;
 if (!lbdata_size) lbdata_size = device_size;
 if (max_size && !user_min) min_size = device_size;
 if (min_size && !user_incr) incr_count = device_size;
 /* Ensure min and incr values are non-zero! */
 if (max_size && !min_size) min_size = device_size;
 if (min_size && !incr_count) incr_count = device_size;
 if (dip->di_random_io) {
     if (!random_align) random_align = device_size;
 }
    } else {
 if (!device_size) device_size = 1;
 if (max_size && !user_min) min_size = 1;
 if (min_size && !user_incr) incr_count = 1;
 /* Ensure min and incr values are non-zero! */
 if (max_size && !min_size) min_size = 1;
 if (min_size && !incr_count) incr_count = 1;
    }
    return;
}

/************************************************************************
 *         *
 * system_device_info() - Get system device information (if possible). *
 *         *
 * Description:        *
 * This function attempts to obtain device information necessary *
 * for device specific testing, by using system dependent syscalls. *
 * Note: This function is called _after_ the device/file is opened. *
 *         *
 * Inputs: dip = The device information pointer.   *
 *         *
 * Return Value:       *
 *  None.       *
 *         *
 ************************************************************************/
#if defined(ultrix) || defined(DEC)

#include <sys/devio.h>
#include <sys/ioctl.h>
#if defined(DEC)
#  include <sys/ioctl_compat.h>
#  include <io/common/devgetinfo.h>
#endif /* defined(DEC) */

void
dec_system_device_info (struct dinfo *dip)
{
    struct devget devget, *devgp = NULL;
    device_info_t devinfo, *devip = NULL;
    int fd = dip->di_fd;
    bool temp_fd = FALSE;
    short category;
    int i, status;

    if (fd == NoFd) {
 temp_fd = TRUE;
 if ( (fd = open (dip->di_dname, O_RDONLY)) < 0) {
     return;
 }
    }

    /*
     * Attempt to obtain the device information.
     */
    bzero ((char *) &devinfo, sizeof(devinfo));
    if (ioctl (fd, DEVGETINFO, (char *) &devinfo) == SUCCESS) {
 devip = &devinfo;
 category = devip->v1.category;
 if ( NEL (devip->v1.device, DEV_UNKNOWN, DEV_STRING_SIZE) ) {
     dip->di_device = Malloc(DEV_STRING_SIZE + 1);
     (void) strncpy (dip->di_device, devip->v1.device, DEV_STRING_SIZE);
 } else if ( NEL (devip->v1.dev_name, DEV_UNKNOWN, DEV_STRING_SIZE) ) {
     dip->di_device = Malloc(DEV_STRING_SIZE + 1);
     (void) strncpy (dip->di_device, devip->v1.dev_name, DEV_STRING_SIZE);
 }
 if (dip->di_device) {
     /*
      * In Steel, device names have trailing spaces. grrr!
      */
     for (i = (DEV_STRING_SIZE); i--; ) {
  if ( isspace(dip->di_device[i]) ) {
      dip->di_device[i] = '\0';
  } else {
      break;
  }
     }
 }
    } else { /* Try the old DEVIOCGET IOCTL... */

 (void) bzero ((char *) &devget, sizeof(devget));
 if (ioctl (fd, DEVIOCGET, (char *) &devget) < 0) {
     if (temp_fd) (void)close(fd);
     return;
 }
 devgp = &devget;
 category = devgp->category;
 if ( NEL (devgp->device, DEV_UNKNOWN, DEV_SIZE) ) {
     dip->di_device = Malloc(DEV_SIZE + 1);
     (void) strncpy (dip->di_device, devgp->device, DEV_SIZE);
 } else if ( NEL (devgp->dev_name, DEV_UNKNOWN, DEV_SIZE) ) {
     dip->di_device = Malloc(DEV_SIZE + 1);
     (void) strncpy (dip->di_device, devgp->dev_name, DEV_SIZE);
 }
 if (dip->di_device) {
     /*
      * In Steel, device names have trailing spaces. grrr!
      */
     for (i = (DEV_SIZE); i--; ) {
  if ( isspace(dip->di_device[i]) ) {
      dip->di_device[i] = '\0';
  } else {
      break;
  }
     }
 }
    }

    /*
     * Setup the device type based on the category.
     */
    switch (category) {

 case DEV_TAPE:   /* Tape category. */
     dip->di_dtype = setup_device_type("tape");
#if defined(EEI)
     dip->di_mt = Malloc(sizeof(*dip->di_mt));
     if (eei_flag) clear_eei_status(fd, TRUE);
     dip->di_eei_sleep = EEI_SLEEP;
     dip->di_eei_retries = EEI_RETRIES;
#endif /* defined(EEI) */
     break;

 case DEV_DISK: {  /* Disk category. */
     /*
      * If using partition 'c', setup to use the whole capacity.
      *
      * Note: Only setup maximum capacity for random I/O, or else
      * we will inhibit End of Media (EOM) testing.
      */
     if (dip->di_random_io) {
  if (dip->di_dname[strlen(dip->di_dname)-1] == 'c') {
      if ( !max_capacity && !user_capacity ) {
   max_capacity = TRUE;
      }
  }
     }
     /****************************************************************
      * Attempt to get disk attributes using DEVGETINFO first, since *
      * for SCSI disks we get more information, which we plan to use *
      * one day, and we also get the real block (sector) size.     *
      ****************************************************************/
     if (devip && (devip->version == VERSION_1) ) {
  v1_disk_dev_info_t *diskinfo;
  diskinfo = &devip->v1.devinfo.disk;
  dip->di_dsize = diskinfo->blocksz;
  if (!device_size) device_size = dip->di_dsize;
  /*
   * NOTE: capacity is whole disk, not the open partition,
   *  so we don't use it unless selected by the user.
   */
  if (max_capacity) {
      dip->di_capacity = diskinfo->capacity;
      user_capacity = (dip->di_capacity * dip->di_dsize);
      if (debug_flag) {
   Fprintf("DEVGETINFO Capacity: %lu blocks.\n", dip->di_capacity);
      }
  }
  if (dip->di_dsize && !user_lbsize && !lbdata_size) {
      lbdata_size = dip->di_dsize;
  }
     } else {
  (void)SetupDiskAttributes(dip, fd);
     }
     dip->di_dtype = setup_device_type("disk");
     /*
      * TODO: Need to read disklabel to pickup partition sizes,
      *       and to check for mounted file systems. More work!
      */
     break;
 }
 case DEV_TERMINAL:  /* Terminal category. */
     dip->di_dtype = setup_device_type("terminal");
     break;

 case DEV_PRINTER:  /* Printer category. */
     dip->di_dtype = setup_device_type("printer");
     break;

 case DEV_SPECIAL:  /* Special category. */
     /*
      * On Tru64 Unix, LSM volumes are really disks!
      */
     if (SetupDiskAttributes(dip, fd) != SUCCESS) {
  dip->di_dtype = setup_device_type("special");
     }
     break;

 default:
     break;
    }
    if (temp_fd) (void)close(fd);
    return;
}

/*
 * SetupDiskAttributes() - Setup Disk Attributes using DEVGETGEOM.
 *
 * Description:
 * This function is used for disk devices which don't support
 * the newer DEVGETINFO IOCTL, like LSM devices.
 *
 * Inputs:
 * dip = The device information pointer.
 * fd = The file descriptor (NoFd == Not open).
 *
 * Outputs:
 * Returns 0 or -1 for Success/Failure.
 */
static int
SetupDiskAttributes (struct dinfo *dip, int fd)
{
    int status;
    bool temp_fd = FALSE;
    DEVGEOMST devgeom;

    if (fd == NoFd) {
 temp_fd = TRUE;
 if ( (fd = open (dip->di_dname, O_RDONLY)) < 0) {
     return (FAILURE);
 }
    }

    /*
     * If using partition 'c', setup to use the whole capacity.
     *
     * Note: Only setup maximum capacity for random I/O, or else
     * we will inhibit End of Media (EOM) testing.
     */
    if (dip->di_random_io) {
 if ( (dip->di_device && EQ(dip->di_device,"LSM")) ||
      (dip->di_dname[strlen(dip->di_dname)-1] == 'c') ) {
     if ( !max_capacity && !user_capacity ) {
  max_capacity = TRUE;
     }
 }
    }

    /*
     * Attempt to obtain the disk geometry.  Works for LSM, etc.
     *
     * NOTE: DEVGETGEOM *fails* on read-only devices (shit!).
     */
    bzero ((char *) &devgeom, sizeof(devgeom));
    if ((status = ioctl (fd, DEVGETGEOM, (char *) &devgeom)) == SUCCESS) {
 dip->di_dsize = devgeom.geom_info.sector_size;
 if (!device_size) device_size = dip->di_dsize;
 /*
  * NOTE: dev_size is whole disk, not the open partition,
  *  so we don't use it unless selected by the user.
  */
 if (max_capacity) {
     dip->di_capacity = devgeom.geom_info.dev_size;
     user_capacity = (dip->di_capacity * dip->di_dsize);
     if (debug_flag) {
  Fprintf("DEVGETGEOM Capacity: %lu blocks.\n", dip->di_capacity);
     }
 }
 if (dip->di_dsize && !user_lbsize && !lbdata_size) {
     lbdata_size = dip->di_dsize;
 }
 dip->di_dtype = setup_device_type("disk");
    }
    /*
     * TODO: Need to read disklabel to pickup partition sizes,
     *       and to check for mounted file systems. More work!
     */
    if (temp_fd) (void)close(fd);
    return (status);
}

#endif /* defined(ultrix) || defined(DEC) */

/*
 * Note: This function called after the device is opened!
 */
void
system_device_info (struct dinfo *dip)
{
    if (dip->di_dtype == NULL) {
 struct stat sb;
 /*
  * For regular files, set the fsync flag to flush writes.
  * Note: This handles processing of *new* output files.
  */
 if ( (fstat (dip->di_fd, &sb) == SUCCESS) &&
      ( S_ISREG(sb.st_mode) ) ) {
     SetupRegularFile (dip, &sb);
 } else {
     dip->di_dtype = setup_device_type("unknown");
 }
    }
    if (fsync_flag == UNINITIALIZED) { fsync_flag = FALSE; }
    return;
}

/************************************************************************
 *         *
 * setup_device_info() - Setup Initial Device Information.  *
 *         *
 * Description:        *
 * This function allocates a device information entry, and does *
 * the initial setup of certain information based on known options. *
 * This function is meant to be called prior to opening the device so *
 * test specific functions are known for initial processing.  *
 *         *
 * Inputs: dname = The device name.    *
 *         *
 * Return Value:       *
 *  Returns pointer to device information entry.  *
 *         *
 ************************************************************************/
struct dinfo *
setup_device_info (char *dname, struct dtype *dtp)
{
    struct dinfo *dip;
    struct stat sb;

    dip = (struct dinfo *) Malloc (sizeof(*dip));
    dip->di_fd = NoFd;
    dip->di_dname = dname;
    dip->di_funcs = &generic_funcs;
    if ( (io_dir == REVERSE) || (io_type == RANDOM_IO) ) {
 dip->di_random_io = TRUE;
    }
#if defined(AIO)
    if (aio_flag) {
 dip->di_funcs = &aio_funcs;
    }
#endif /* defined(AIO) */
#if defined(MMAP)
    if (mmap_flag) {
 dip->di_funcs = &mmap_funcs;
 dtp = setup_device_type("mmap");
    }
#endif /* defined(MMAP) */

#if defined(ultrix) || defined(DEC)
    /*
     * Must do this early on, to set device type and size.
     */
    if (dtp == NULL) {
 dec_system_device_info (dip);
    }
    dtp = dip->di_dtype;
#endif /* defined(ultrix) || defined(DEC) */

    /*
     * If user specified a device type, don't override it.
     */
    if (dtp == NULL) {
 /*
  * Determine test functions based on device name.
  */
 if ( (EQL (dname, DEV_PREFIX, DEV_LEN))   ||
      (EQL (dname, ADEV_PREFIX, ADEV_LEN)) ||
      (EQL (dname, NDEV_PREFIX, NDEV_LEN)) ) {
     char *dentry;
     if (EQL (dname, DEV_PREFIX, DEV_LEN)) {
  dentry = (dname + DEV_LEN);
     } else if (EQL (dname, ADEV_PREFIX, ADEV_LEN)) {
  dentry = (dname + ADEV_LEN);
     } else {
  dentry = (dname + NDEV_LEN);
     }
#if defined(__CYGWIN__)
 /*
  * Map //./ or \\.\ to /dev/ prefix for Cygnus raw mounts.
  */
 {   if (NEL (dname, DEV_PREFIX, DEV_LEN)) {
  char *cygnus_dname = Malloc(strlen(dname)+1);
  (void)strcpy(cygnus_dname, DEV_PREFIX);
  (void)strcat(cygnus_dname, dentry);
  dip->di_dname = cygnus_dname;
     }
 }
#endif /* defined(__CYGWIN__) */
     if ( (ttyport_flag == TRUE) ||
   (EQL (dentry, TTY_NAME, TTY_LEN)) ||
   (EQL (dentry, CONSOLE_NAME, CONSOLE_LEN)) ) {
  dtp = setup_device_type("terminal");
     } else if ( (EQL (dentry, TAPE_NAME, sizeof(TAPE_NAME)-1)) ||
   (EQL (dentry, NTAPE_NAME, sizeof(NTAPE_NAME)-1)) ) {
  dtp = setup_device_type("tape");
     } else if ( (EQL (dentry, DISK_NAME, sizeof(DISK_NAME)-1)) ||
   (EQL (dentry, RDISK_NAME, sizeof(RDISK_NAME)-1)) ) {
  dtp = setup_device_type("disk");
     } else if ( (EQL (dentry, CDROM_NAME, sizeof(CDROM_NAME)-1)) ||
   (EQL (dentry, RCDROM_NAME, sizeof(RCDROM_NAME)-1)) ) {
  dtp = setup_device_type("disk");
     }
#if defined(__MSDOS__) || defined(__WIN32__) || defined(_NT_SOURCE)
     if ( (dtp == NULL) && (IsDriveLetter (dentry)) ) {
  dtp = setup_device_type("block");
     }
#endif /* defined(__MSDOS__) || defined(__WIN32__) || defined(_NT_SOURCE) */
 }
#if defined(FIFO)
 if ( (dtp == NULL) &&
      (stat (dname, &sb) == SUCCESS) ) {
     if ( S_ISFIFO(sb.st_mode) ) {
  verify_flag = FALSE;
  dip->di_funcs = &fifo_funcs;
  dtp = setup_device_type("fifo");
     }
 }
#endif /* defined(FIFO) */
 if ( (dtp == NULL) &&
      (strlen(dname) == 1) && (*dname == '-') ) {
     dtp = setup_device_type("pipe");
 }
 if ( (dtp == NULL) &&
      (stat (dname, &sb) == SUCCESS)) {
     if ( S_ISBLK(sb.st_mode) ) {
  dtp = setup_device_type("block");
  if (fsync_flag == UNINITIALIZED) {
       fsync_flag = TRUE;
  }
     } else if ( S_ISCHR(sb.st_mode) ) {
  /*
   * Character devices are NOT treated as disks!
   */
#if defined(ultrix) || defined(DEC)
  if (SetupDiskAttributes(dip, dip->di_fd) != SUCCESS)
#endif
      dtp = setup_device_type("character");
     }
 }
    } /* if (dtp == NULL) */

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    /* End of device type setup.  Special setup follows. */
    /* - - - - - - - - - - - - - - - - - - - - - - - - - */

    /*
     * Do special setup for certain device types.
     */
    if (dip->di_dtype = dtp) {
 if (dtp->dt_dtype == DT_TERMINAL) {
     ttyport_flag = TRUE;  /* this should go away... */
     dip->di_funcs = &tty_funcs;
 } else if ( (dtp->dt_dtype == DT_BLOCK) ||
      (dtp->dt_dtype == DT_DISK)  || dip->di_random_io ) {
     dip->di_random_access = TRUE;
 }
 setup_device_defaults (dip);
    }

    /*
     * If the device size isn't set, then set it to our default.
     *
     * Note: This size is used for finding disk capacity, random I/O,
     *  variable requests, and reporting failing relative block.
     */
    if ( !dip->di_dsize ) {
 if (!device_size) {
     device_size = BLOCK_SIZE;
 }
 dip->di_dsize = device_size;
    }

    /*
     * Note: This handles *existing* input/output files.
     */
    if (stat (dname, &sb) == SUCCESS) {
 if ( S_ISREG(sb.st_mode) ) {
     SetupRegularFile (dip, &sb);
     if ( (dispose_mode == DELETE_FILE) && keep_existing && !num_procs) {
  dispose_mode = KEEP_FILE; /* Keep existing files! */
     }
 }
#if defined(_QNX_SOURCE)
 else if ( S_ISBLK(sb.st_mode) ) {
     user_capacity = (large_t)(sb.st_size * dip->di_dsize);
 }
#endif /* defined(_QNX_SOURCE) */
    }
    return (dip);
}

static void
SetupRegularFile (struct dinfo *dip, struct stat *sbp)
{
    /*
     * If random I/O was selected, and a data or record limit was
     * not specified (i.e. runtime=n), then setup the file size.
     * This is necessary to limit random I/O within file size, or
     * for newly created files setup capacity based on data limit.
     */
    if ( (dip->di_random_io || num_slices) &&
  (rdata_limit == (large_t)0) && !user_capacity) {
 if (sbp->st_size) {
     user_capacity = (large_t) sbp->st_size;
 } else if (data_limit != INFINITY) {
     user_capacity = data_limit;
 }
    }
    if (fsync_flag == UNINITIALIZED) {
 fsync_flag = TRUE; /* Only has meaning when writing. */
    }
    dip->di_random_access = TRUE;
    dip->di_dtype = setup_device_type("regular");
    setup_device_defaults (dip);
}

#if defined(__MSDOS__) || defined(__WIN32__) || defined(_NT_SOURCE)

static bool
IsDriveLetter(char *bufptr)
{
    /* Check for drive letters "[a-zA-Z]:" */
    if ((strlen(bufptr) == 2) && (bufptr[1] == ':') &&
 ((bufptr[0] >= 'a') && (bufptr[0] <= 'z') ||
  (bufptr[0] >= 'A') && (bufptr[0] <= 'Z'))) {
 return (TRUE);
    }
    return (FALSE);
}

#endif /* defined(__MSDOS__) || defined(__WIN32__) || defined(_NT_SOURCE) */
