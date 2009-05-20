/****************************************************************************
 *									    *
 *			  COPYRIGHT (c) 1990 - 2000			    *
 *			   This Software Provided			    *
 *				     By					    *
 *			  Robin's Nest Software Inc.			    *
 *			       2 Paradise Lane  	    *
 *			       Hudson, NH 03051				    *
 *			       (603) 883-2355				    *
 *									    *
 * Permission to use, copy, modify, distribute and sell this software and   *
 * its documentation for any purpose and without fee is hereby granted,	    *
 * provided that the above copyright notice appear in all copies and that   *
 * both that copyright notice and this permission notice appear in the	    *
 * supporting documentation, and that the name of the author not be used    *
 * in advertising or publicity pertaining to distribution of the software   *
 * without specific, written prior permission.				    *
 *									    *
 * THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,     *
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN	    *
 * NO EVENT SHALL HE BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL   *
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR    *
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS  *
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF   *
 * THIS SOFTWARE.							    *
 *									    *
 ****************************************************************************/
/*
 * Modification History:
 *
 * January 24th, 2001 by Robin Miller.
 *	For Windows/NT, use /dev/[n]rmt instead of "tapeN" for tape
 * device names, since this is the name used when tapes are mounted
 * using the Cygnus toolkit.
 *
 * January 14th, 2001 by Robin Miller.
 *	Added support for multiple volumes option.
 *
 * January 11th, 2001 by Robin Miller.
 *	Modify conditionals so Windows tape devices are /dev/rmtN
 * and /dev/nrmtN (norewind)  which the latest Cygnus README states.
 *
 * December 30th, 2000 by Robin Miller.
 *	Make changes to build using MKS/NuTCracker product.
 *
 * April 17th, 2000 by Robin Miller.
 *	Added device path failure flag, to force re-open of tapes
 * on Wave4 clusters which forces alternate paths or servers to be
 * located by CAM or DRD.
 *
 * January 22nd, 2000 by Robin Miller.
 *	Added support for Cygwin tape devices for Windows/NT.
 *
 * August 6th, 1999 by Robin Miller.
 *      Better parameterizing of "long long" printf formatting.
 *
 * July 29, 1999 by Robin Miller.
 *	Merge in changes made to compile on FreeBSD.
 *
 * July 22nd, 1999 by Robin Miller.
 *	Added support for IOT (DJ's) test pattern.
 *
 * May 27, 1999 by Robin Miller.
 *	Added support for micro-second delays.
 *
 * April 8, 1999 by Robin Miller.
 *	Merge in Jeff Detjen's changes for table()/sysinfo timing.
 *
 * December 21, 1998 by Robin Miller.
 *	For DUNIX, changes to handle tape resets.
 *
 * April 29, 1998 by Robin Miller.
 *	Add support for an alternate device directory, e.g. "/devices/",
 * as a prefix to physical device names.  Just checking for "/dev...",
 * described below, is too general and sure to get me into trouble.
 *
 * April 25, 1998 by Robin Miller.
 *	Change device prefix to account for "/dev" or "/devices" (steel),
 * since this knowledge is used to determine if a device should exist
 * (to avoid creating files in the /dev directory), as well as whether
 * the PID is appended during multiple process testing.
 *
 * February 28, 1996 by Robin Miller.
 *	Added support for copying and verifying device/files.
 *
 * December 19, 1995 by Robin Miller
 *      Conditionalize for Linux Operating System.
 *
 * September 23, 1994 by Robin Miller.
 *      Make changes necessary to build on QNX 4.21 release.
 *
 * September 5, 1992 by Robin Miller.
 *	Initial port to QNX 4.1 Operating System.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <time.h> 		/* CLK_TCK defined here */
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/times.h>
#include <sys/stat.h>

#if defined(__QNXNTO__)
#include <unix.h>
#endif

/*
 * These are found in <tzfile.h> on Tru64 UNIX and Solaris.
 */
#define SECS_PER_MIN	60
#define MINS_PER_HOUR	60
#define HOURS_PER_DAY	24
#define SECS_PER_HOUR	(SECS_PER_MIN * MINS_PER_HOUR)
#define SECS_PER_DAY	(SECS_PER_HOUR * HOURS_PER_DAY)

#if defined(DEC)
#  include <sys/table.h>
#endif /* defined(DEC) */

#if defined(EEI)
#  include <sys/mtio.h>
#endif /* defined(EEI) */

#if defined(MUNSA)
#  include <assert.h>
#  include <sys/dlm.h>
dlm_lock_mode_t    munsa_lock;
#endif /* defined(MUNSA) */

#if !defined(HZ)
#  if defined(sun) || defined(__WIN32__) || defined(SCO)
#    include <sys/param.h>
#  endif /* defined(sun) */
#  if defined(CLK_TCK) && !defined(HZ)
#    define HZ		CLK_TCK
#  else
#    if !defined(HZ)
#      define HZ		256
#    endif /* !defined(HZ) */
#    if !defined(CLK_TCK)
#      define CLK_TCK	HZ
#    endif /* !defined(CLK_TCK) */
#  endif /* defined(CLK_TCK) */
#endif /* !defined(HZ) */

#if defined(ultrix) || defined(sun) && !defined(SOLARIS)
#  define USE_VARARGS	1
#else /* !defined(ultrix) */
#  define USE_STDARG	1
#endif /* defined(ultrix) */

#if defined(DEC)
#  define LOG_DIAG_INFO	1
#endif /* defined(DEC) */

#if defined(__alpha)
#  define MACHINE_64BITS
#endif /* defined(__alpha) */

/*
 * Macro to calculate block count from byte count (rounds up).
 * [ these are usually found in sys/param.h on DUNIX ]
 */
#if !defined(howmany)
#  define howmany(x, y)   (((x)+((y)-1))/(y))
#endif /* !defined(howmany) */
#if !defined(roundup)
#  define roundup(x, y)   ((((x)+((y)-1))/(y))*(y))
#endif /* !defined(roundup) */
#if !defined(rounddown)
#  define rounddown(x,y)  (((x)/(y))*(y))
#endif /* !defined(rounddown) */

/*
 * Macros for fast min/max.
 */
#if !defined(MIN)
#  define MIN(a,b) (((a)<(b))?(a):(b))
#endif /* !defined(MIN) */
#if !defined(MAX)
#  define MAX(a,b) (((a)>(b))?(a):(b))
#endif /* if !defined(MAX) */

/*
 * Macro to calculate the starting block of where error occured.
 *    ( 'x' = byte count or file offset, 'y' = block size )
 */
#define WhichBlock(x,y)		((x)/(y))

#if defined(__CYGWIN__) && defined(__STRICT_ANSI__)
int     _EXFUN(fileno, (FILE *));
#endif

/*
 * Large value for 32 or 64 bit systems.
 *
 * Note: Most systems are now defined quad_t or u_quad_t for
 *	 "long long" values, but I've chosen to avoid conflicts
 *	 by defining my own typedef's (for now :-).  The other
 *	 reason for my own definitions is to avoid the following:
 *
 *	 typedef struct  _quad { int val[2]; } quad;
 *
 *	 used by some systems that don't support "long long" :-)
 *
 */
#if defined(MACHINE_64BITS)

#define QuadIsLong
typedef unsigned long		large_t;
typedef signed long		slarge_t;
typedef volatile large_t	v_large;
typedef volatile slarge_t	v_slarge;
#define LUF	"%lu"
#define LDF	"%ld"
#define LXF	"%#lx"
#define FUF	LUF
#define FXF	LXF

#elif defined(__GNUC__) && defined(_BSD_SOURCE) || defined(SCO) || defined(__QNXNTO__)

#define QuadIsLongLong
typedef unsigned long long int	large_t;
typedef signed long long int	slarge_t;
typedef volatile large_t	v_large;
typedef volatile slarge_t	v_slarge;
#if defined(__QNXNTO__)
#  define LUF   "%llu"
#  define LDF   "%lld"
#  define LXF   "%#llx"
#elif defined(SCO)
#  define LUF	"%Lu"
#  define LDF	"%Ld"
#  define LXF	"%#Lx"
#else /* !defined(SCO) && !defined(__QNXNTO__) */
#  define LUF	"%qu"
#  define LDF	"%qd"
#  define LXF	"%#qx"
#endif /* defined(__QNXNTO__) */

#if defined(_FILE_OFFSET_BITS) && (_FILE_OFFSET_BITS == 64)
#  define FUF	LUF
#  define FXF	LXF
#else /* !defined(_FILE_OFFSET_BITS) || (_FILE_OFFSET_BITS != 64) */
#  define FUF	"%lu"
#  define FXF	"%#lx"
#endif /* defined(_FILE_OFFSET_BITS) && (_FILE_OFFSET_BITS == 64) */

#elif defined(__NUTC__)

#define QuadIsLongLong

typedef u_quad_t		large_t;
typedef quad_t			slarge_t;
typedef volatile large_t	v_large;
typedef volatile slarge_t	v_slarge;
#define LUF	"%qu"
#define LDF	"%qd"
#define LXF	"%#qx"
#define FUF	"%lu"
#define FXF	"%#lx"

#else /* !defined(MACHINE_64BITS) && !defined(__GNUC__) */

#define QuadIsDouble
typedef double			large_t;
typedef double			slarge_t;
typedef volatile large_t	v_large;
typedef volatile slarge_t	v_slarge;

#define LUF	"%.0f"
#define LDF	"%.0f"
#define LXF	"%.0f"	/* ??? no hex ??? */
#define FUF	"%lu"
#define FXF	"%#lx"

#endif /* defined(MACHINE_64BITS) */

/*
 * Create some shorthands for volatile storage classes:
 */
typedef	volatile char	v_char;
typedef	volatile short	v_short;
typedef	volatile int	v_int;
typedef volatile long	v_long;
#if !defined(DEC)
/* These are defined in sys/types.h on DEC Alpha systems. */
typedef	volatile unsigned char	vu_char;
typedef	volatile unsigned short	vu_short;
typedef	volatile unsigned int	vu_int;
typedef volatile unsigned long	vu_long;
#endif /* !defined(__alpha) */

#define SUCCESS		0			/* Success status code. */
#define FAILURE		-1			/* Failure status code. */
#define WARNING		1			/* Warning status code.	*/
#define TRUE		1			/* Boolean TRUE value.	*/
#define FALSE		0			/* Boolean FALSE value.	*/
#define UNINITIALIZED	255			/* Uninitialized flag.	*/
#define NO_LBA		0xFFFFFFFF		/* No LBA vlaue.	*/

#define BLOCK_SIZE		512		/* Bytes in block.	*/
#define KBYTE_SIZE		1024		/* Kilobytes value.	*/
#define MBYTE_SIZE		1048576L	/* Megabytes value.	*/
#define GBYTE_SIZE		1073741824L	/* Gigabytes value.	*/

#define STRING_BUFFER_SIZE	256		/* String buffer size.	*/

#define MAX_PROCS	256			/* Maximum processes.	*/
#define MAX_SLICES	256			/* Maximum slices.	*/

#undef INFINITY
#if defined(MACHINE_64BITS)
#  define MAX_SEEK	0x8000000000000000UL /* Maximum lseek() value.	*/
#  define MAX_LONG	0x7fffffffffffffffL  /* Maximum positive long.	*/
#  define MAX_ULONG	0xffffffffffffffffUL /* Maximum u_long value.	*/
#  define INFINITY	MAX_ULONG	/* Maximum possible long value.	*/
#  define DEFAULT_PATTERN 0x39c39c39U	/* Default data pattern.	*/
#  define ASCII_PATTERN	  0x41424344U	/* Default ASCII data pattern.	*/
#  define TBYTE_SIZE	1099511627776L		/* Terabytes value.	*/
#else /* !defined(MACHINE_64BITS */
#  define MAX_LONG	0x7fffffffL	/* Maximum positive long value.	*/
#  define MAX_ULONG	0xffffffffUL	/* Maximum unsigned long value.	*/
#  if defined(_FILE_OFFSET_BITS) && (_FILE_OFFSET_BITS == 64)
#    define MAX_SEEK	0x8000000000000000ULL /* Maximum lseek() value. */
#  else /* if !defined(_FILE_OFFSET_BITS) || (_FILE_OFFSET_BITS != 64) */
#    define MAX_SEEK	0x80000000UL	/* Maximum lseek() value.	*/
#  endif /* defined(_FILE_OFFSET_BITS) && (_FILE_OFFSET_BITS == 64) */
#  if defined(QuadIsLongLong)
#    if defined(__NUTC__)
#      define MAX_ULONG_LONG 0xffffffffffffffffui64
#      define INFINITY	 MAX_ULONG_LONG	/* Maximum possible large value */
#      define TBYTE_SIZE 1099511627776i64       /* Terabytes value.	*/
#    else
#      define MAX_ULONG_LONG 0xffffffffffffffffULL
#      define INFINITY	 MAX_ULONG_LONG	/* Maximum possible large value */
#      define TBYTE_SIZE 1099511627776LL	/* Terabytes value.	*/
#    endif /* defined(__NUTC__) */
#  else /* assume QuadIsDouble */
#    if defined(__WIN32__)
       /* HUGE_VAL seems to be defined as 0.0, which is NFG! */
#      define INFINITY  (double)0x10000000000000L /* Max large value???	*/
#    else
#      define INFINITY	HUGE_VAL	/* Maximum possible large value */
#    endif
#    define TBYTE_SIZE	((double)GBYTE_SIZE*1024) /* Terabytes value.	*/
#  endif /* defined(QuadIsLongLong) */
#  define DEFAULT_PATTERN 0x39c39c39UL	/* Default data pattern.	*/
#  define ASCII_PATTERN	  0x41424344UL	/* Default ASCII data pattern.	*/
#endif /* defined(MACHINE_64BITS) */

/*
 * Shorthand macros for string compare functions:
 */
#define EQ(x,y)		(strcmp(x,y)==0)	/* String EQ compare.	*/
#define EQL(x,y,n)	(strncmp(x,y,n)==0)	/* Compare w/length.	*/

#define NE(x,y)		(strcmp(x,y)!=0)	/* String NE compare.	*/
#define NEL(x,y,n)	(strncmp(x,y,n)!=0)	/* Compare w/length.	*/

#define EQS(x,y)	(strstr(x,y) != NULL)	/* Sub-string equal.	*/
#define EQSC(x,y)	(strcasestr(x,y) != NULL) /* Case insensitive.	*/

#define NES(x,y)	(strstr(x,y) == NULL)	/* Sub-string not equal	*/
#define NESC(x,y)	(strcasestr(x,y) == NULL) /* Case insensitive.	*/

/*
 * Define some known device names (for automatic device recognition).
 */
#define DEV_PREFIX	"/dev/"		/* Physical device name prefix.	*/
#define DEV_LEN		5		/* Length of device name prefix.*/
/*
 * Alternate Device Directory:
 */
#if defined(_NT_SOURCE)
#  define ADEV_PREFIX	"//./"		/* Windows hidden device dir.	*/
#  define ADEV_LEN	4		/* Length of device name prefix.*/
#  define NDEV_PREFIX	"\\\\.\\"	/* Native Windows device dir.	*/
#  define NDEV_LEN	4		/* That's for "\\.\" prefix.	*/
#else /* !defined(_NT_SOURCE) */
#  define ADEV_PREFIX	"/devices/"	/* Physical device name prefix.	*/
#  define ADEV_LEN	9		/* Length of device name prefix.*/
#  define NDEV_PREFIX	"/dev/"		/* Native device prefix (dup).	*/
#  define NDEV_LEN	5		/* Native device prefix length.	*/
#endif /* defined(_NT_SOURCE) */

#define CONSOLE_NAME	"console"	/* The console device name.	*/
#define CONSOLE_LEN	7		/* Length of console name.	*/

#if defined(_QNX_SOURCE)
#  define CDROM_NAME	"cd"		/* Start of CD-ROM device name.	*/
#  define RCDROM_NAME	"cd"		/* Start of raw CD-ROM names.	*/
#elif defined(sun) || defined(__linux__)
/* Note: Don't know about Sun systems. */
#  define CDROM_NAME	"cd"		/* Start of CD-ROM device name.	*/
#  define RCDROM_NAME	"scd"		/* Start of raw CD-ROM names.	*/
/* NOTE: Linux (RedHat V6.0) currently has no raw CD-ROM names! */
#elif defined(SCO)
#  define CDROM_NAME	"cdrom"		/* Start of CD-ROM device name.	*/
#  define RCDROM_NAME	"rcdrom"	/* Start of raw CD-ROM names.	*/
#elif defined(_NT_SOURCE)
#  define CDROM_NAME	"Cdrom"		/* Start of CD-ROM device name.	*/
#  define RCDROM_NAME	"cdrom"		/* Allow both upper/lowercase.	*/
#else
#  define CDROM_NAME	"rz"		/* Start of CD-ROM device name.	*/
#  define RCDROM_NAME	"rrz"		/* Start of raw CD-ROM names.	*/
#endif /* !defined(_QNX_SOURCE) */

#if defined(_QNX_SOURCE)
#  define DISK_NAME	"hd"		/* Start of disk device names.	*/
#  define RDISK_NAME	"hd"		/* Start of raw disk names.	*/
#elif defined(sun) || defined(__linux__)
#  define DISK_NAME	"sd"		/* Start of disk device names.	*/
#  define RDISK_NAME	"rsd"		/* Start of raw disk names.	*/
/* NOTE: Linux (RedHat V6.0) currently has no raw disk names! */
#elif defined(SCO)
#  define DISK_NAME	"dsk"		/* Start of disk device names.	*/
#  define RDISK_NAME	"rdsk"		/* Start of raw disk names.	*/
#elif defined(_NT_SOURCE)
#  define DISK_NAME	"PhysicalDrive"	/* Start of disk device names.	*/
#  define RDISK_NAME	"physicaldrive"	/* Allow both upper/lowercase.	*/
#else
#  define DISK_NAME	"rz"		/* Start of disk device names.	*/
#  define RDISK_NAME	"rrz"		/* Start of raw disk names.	*/
#endif /* !defined(_QNX_SOURCE) */

#if defined(_QNX_SOURCE)
#  define TTY_NAME	"ser"		/* Start of terminal names.	*/
#elif defined(__NUTC__)
#  define TTY_NAME	"com"		/* Start of terminal names.	*/
#else /* !defined(_QNX_SOURCE) */
#  define TTY_NAME	"tty"		/* Start of terminal names.	*/
#endif /* defined(_QNX_SOURCE) */
#define TTY_LEN		3		/* Length of terminal name.	*/

#if defined(_QNX_SOURCE)
#  define TAPE_NAME	"tpr"		/* Start of tape device names.	*/
#  define NTAPE_NAME	"tp"		/* No-rewind tape device name.	*/
#elif defined(sun)
#  define TAPE_NAME	"rst"		/* Start of tape device names.	*/
#  define NTAPE_NAME	"nrst"		/* No-rewind tape device name.	*/
#elif defined(__linux__)
#  define TAPE_NAME	"st"		/* Start of tape device names.	*/
#  define NTAPE_NAME	"nst"		/* No-rewind tape device name.	*/
#elif defined(SCO)
#  define TAPE_NAME	"ctape"		/* Start of tape device names.	*/
#  define NTAPE_NAME	"ntape"		/* No-rewind tape device name.	*/
#elif defined(_NT_SOURCE)
#  define TAPE_NAME	"rmt"		/* Start of tape device names.	*/
#  define NTAPE_NAME	"nrmt"		/* No-rewind tape device name.	*/
#else
#  define TAPE_NAME	"rmt"		/* Start of tape device names.	*/
#  define NTAPE_NAME	"nrmt"		/* No-rewind tape device name.	*/
#endif /* !defined(_QNX_SOURCE) */

#define DEF_LOG_BUFSIZE	2048		/* Large enough for all stats.	*/

#if defined(BUFSIZ) && (BUFSIZ > DEF_LOG_BUFSIZE)
#  define LOG_BUFSIZE	BUFSIZ		/* The log file buffer size.	*/
#else /* !defined(BUFSIZ) */
#  define LOG_BUFSIZE	DEF_LOG_BUFSIZE	/* The log file buffer size.	*/
#endif /* defined(BUFSIZ) */

#define ANY_RADIX	0		/* Any radix string conversion.	*/
#define DEC_RADIX	10		/* Base for decimal conversion.	*/
#define HEX_RADIX	16		/* Base for hex str conversion.	*/

/*
 * Define some Architecture dependent types:
 */
#if defined(__alpha)

typedef unsigned long		ptr_t;
typedef int			int32;
typedef unsigned int		u_int32;
typedef unsigned int		bool;
typedef volatile unsigned int	v_bool;

#else /* !defined(__alpha) */

typedef unsigned int		ptr_t;
#if !defined(OSFMK)
typedef int			int32;
#endif /* !defined(OSFMK) */
typedef unsigned int		u_int32;
typedef unsigned char		bool;
typedef volatile unsigned char	v_bool;

#endif /* defined(__alpha) */

/*
 * Some systems don't have these definitions:
 */
#if defined(__MSDOS__)

typedef	unsigned char		u_char;
typedef	unsigned short		u_short;
typedef	unsigned int		u_int;
typedef unsigned long		u_long;
typedef char *			caddr_t;
typedef unsigned long		daddr_t;

#endif /* defined(_QNX_SOURCE) */

#if defined(ultrix) || defined(sun)

#if !defined(SOLARIS)
typedef int			ssize_t;
#endif /* !defined(SOLARIS) */

#endif /* defined(ultrix) */

/*
 * The buffer pad bytes are allocated at the end of all data buffers,
 * initialized with the inverted data pattern, and then checked after
 * each read operation to ensure extra bytes did not get transferred
 * at the end of buffer.  This test is necessary, since quite often
 * we've seen data corruption (extra bytes) due to improper flushing
 * of DMA FIFO's, or other coding errors in our SCSI/CAM sub-system.
 */
#define PADBUFR_SIZE	sizeof(u_int32)	/* The data buffer pad size.	*/
					/* MUST match pattern length!!!	*/

/*
 * The buffer rotate size are used to force unaligned buffer access
 * by rotating the starting buffer address through the sizeof(ptr).
 * This feature has been very useful in forcing drivers through special
 * code to handle unaligned addresses & buffers crossing page boundaries.
 */
#define ROTATE_SIZE	sizeof(char *)	/* Forces through all ptr bytes	*/

/*
 * 'dt' specific exit status codes:
 */
#define END_OF_FILE	254			/* End of file code.	*/
#define FATAL_ERROR	255			/* Fatal error code.	*/

#define get_lbn(bp)	( ((u_int32)bp[3] << 24) | ((u_int32)bp[2] << 16) | \
			  ((u_int32)bp[1] << 8) | (u_int32)bp[0])

/*
 * Structure for Baud Rate Lookup.
 */
struct tty_baud_rate {
	u_int32	usr_speed;		/* User entered speed value.	*/
	speed_t	tty_speed;		/* Parameter for tty driver.	*/
};

enum opt {OFF, ON, OPT_NONE};
enum flow {FLOW_NONE, CTS_RTS, XON_XOFF};
enum stats {COPY_STATS, READ_STATS, RAW_STATS, WRITE_STATS, TOTAL_STATS, VERIFY_STATS};
enum dispose {DELETE_FILE, KEEP_FILE};
enum file_type {INPUT_FILE, OUTPUT_FILE};
enum test_mode {READ_MODE, WRITE_MODE};
enum onerrors {ABORT, CONTINUE};
enum iodir {FORWARD, REVERSE};
enum iomode {COPY_MODE, TEST_MODE, VERIFY_MODE};
enum iotype {SEQUENTIAL_IO, RANDOM_IO};

/*
 * Declare the external test functions:
 *
 * TODO:  Tape test functions...
 */
#define NOFUNC		(int (*)()) 0	/* No test function exists yet. */
#define NoFd		-1		/* No file descriptor open.	*/

extern int nofunc();			/* Stub return (no test func).	*/
extern struct dtfuncs generic_funcs;	/* Generic test functions.	*/
#if defined(AIO)
extern struct dtfuncs aio_funcs;	/* POSIX AIO test functions.	*/
#endif /* defined(AIO) */
#if defined(MMAP)
extern struct dtfuncs mmap_funcs;	/* Memory map test functions.	*/
#endif /* defined(MMAP) */
extern struct dtfuncs tty_funcs;	/* Terminal test functions.	*/
#if defined(FIFO)
extern struct dtfuncs fifo_funcs;	/* Named pipes test functions.	*/
#endif /* defined(FIFO) */

/*
 * Define various device types:
 */
enum devtype {DT_AUDIO, DT_BLOCK, DT_CHARACTER, DT_COMM, DT_DISK,
	      DT_GRAPHICS, DT_MEMORY, DT_MMAP, DT_MOUSE, DT_NETWORK,
	      DT_FIFO, DT_PIPE, DT_PRINTER, DT_PROCESSOR, DT_REGULAR,
	      DT_SOCKET, DT_SPECIAL, DT_STREAMS, DT_TAPE, DT_TERMINAL,
	      DT_UNKNOWN };
struct dtype {
	char	*dt_type;
	enum	devtype dt_dtype;
};

extern struct dtype *input_dtype;
extern struct dtype *output_dtype;

/*
 * Define file control flags:
 */
#define DCF_ACTIVE	1		/* File test is active.		*/

/************************************************************************
 * NOTE: Eventually, all device specific data must be moved to this	*
 *	 data structure, but that will be done in stages since there's	*
 *	 a fair amount of work involved.  Can you say re-write? :-)	*
 ************************************************************************/

/*
 * Define device type information:
 */
typedef struct dinfo {
	int	di_fd;			/* The file descriptor.		*/
	int	di_flags;		/* The file control flags.	*/
	int	di_oflags;		/* The last file open flags.	*/
	char	*di_dname;		/* The /dev device name.	*/
	char	*di_device;		/* The real device name.	*/
	enum test_mode di_mode;		/* The current test mode.	*/
	enum file_type di_ftype;	/* The file access type.	*/
	struct dtype *di_dtype;		/* The device type information.	*/
	struct dtfuncs *di_funcs;	/* The test functions to use.	*/
	/*
	 * Statistics and State Information:
	 */
	bool	di_closing;		/* The device is being closed.	*/
	u_int32	di_dsize;		/* The device block size.	*/
	u_int32	di_capacity;		/* The device capacity (blocks)	*/
volatile off_t	di_offset;		/* The device/file offset.	*/
	v_int	di_errno;		/* The last errno encountered.	*/
	v_bool	di_end_of_file;		/* End of file was detected.	*/
	v_bool	di_end_of_logical;	/* End of logical tape detected	*/
	v_bool	di_end_of_media;	/* End of media was detected.	*/
	bool	di_eof_processing;	/* End of file proessing.	*/
	bool	di_eom_processing;	/* End of media processing.	*/
	bool	di_random_io;		/* Random I/O selected flag.	*/
	bool	di_random_access;	/* Random access device flag.	*/
	v_large	di_dbytes_read;		/* Number of data bytes read.	*/
	v_large	di_dbytes_written;	/* Number of data bytes written.*/
	v_large	di_fbytes_read;		/* Number of file bytes read.	*/
	v_large	di_fbytes_written;	/* Number of file bytes written.*/
	v_large	di_vbytes_read;		/* Number of volume bytes read.	*/
	v_large	di_vbytes_written;	/* Number of volume bytes wrote.*/
	vu_long	di_files_read;		/* Number of tape files read.	*/
	vu_long	di_files_written;	/* Number of tape files written.*/
	vu_long	di_records_read;	/* Number of records read.	*/
	vu_long	di_records_written;	/* Number of records written.	*/
	vu_long	di_volume_records;	/* Number of volume records.	*/
	vu_long di_read_errors;		/* Number of read errors.	*/
	vu_long di_write_errors;	/* Number of write errors.	*/
	large_t	di_data_limit;		/* The data limit (in bytes).	*/
	large_t	di_volume_bytes;	/* Accumulated volume bytes.	*/
	/*
	 * Extended Error Information (EEI) State:
	 */
	bool	di_proc_eei;		/* Processing EEI data flag.	*/
#if defined(EEI)
#define EEI_RESET	1		/* Delay after resets.		*/
#define EEI_SLEEP	3		/* Time between retries.	*/
#define EEI_RETRIES	100		/* Number of EEI retries.	*/
#define EEI_OPEN_RETRIES 3		/* Number of open retries.	*/
	bool	di_devpath_failure;	/* Path failure condition.	*/
	bool	di_reset_condition;	/* Reset condition detected.	*/
	struct mtget *di_mt;		/* The tape error information.	*/
	int	di_eei_retries;		/* The number of EEI retries.	*/
	int	di_eei_sleep;		/* Time to sleep between retry.	*/
#endif /* defined(EEI) */
} dinfo_t;

/*
 * Define test function dispatch structure:
 *
 * [ NOTE:  These functions are not all used at this time.  The intent
 *   is to cleanup the code later by grouping functions appropriately. ]
 */
struct dtfuncs {
						/* Open device or file.	   */
	int	(*tf_open)(struct dinfo	*dip, int oflags);
						/* Close device or file.   */
	int	(*tf_close)(struct dinfo *dip);
						/* Special initilization.  */
	int	(*tf_initialize)(struct dinfo *dip);
						/* Start test processing.  */
	int	(*tf_start_test)(struct dinfo *dip);
						/* End of test processing. */
	int	(*tf_end_test)(struct dinfo *dip);
						/* Read file data.	   */
	int	(*tf_read_file)(struct dinfo *dip);
						/* Processes read data.	   */
	int	(*tf_read_data)(struct dinfo *dip);
						/* Cancel read requests.   */
	int	(*tf_cancel_reads)(struct dinfo *dip);
						/* Write file data.	   */
	int	(*tf_write_file)(struct dinfo *dip);
						/* Processes write data.   */
	int	(*tf_write_data)(struct dinfo *dip);
						/* Cancel write requests.  */
	int	(*tf_cancel_writes)(struct dinfo *dip);
						/* Flush data to media.	   */
	int	(*tf_flush_data)(struct dinfo *dip);
						/* Verify data read.	   */
	int	(*tf_verify_data)(	struct dinfo	*dip,
					u_char		*buffer,
					size_t		count,
					u_int32		pattern,
					u_int32		*lba );
						/* Reopen device or file.  */
	int	(*tf_reopen_file)(struct dinfo *dip, int oflags);
						/* Test startup handling.  */
	int	(*tf_startup)(struct dinfo *dip);
						/* Test cleanup handling.  */
	int	(*tf_cleanup)(struct dinfo *dip);
						/* Validate test options.  */
	int	(*tf_validate_opts)(struct dinfo *dip);
};

/*
 * Macros to Improve Performance:
 *
 * Note: Others can be added after debug is conditionalized.
 */
#define INLINE_FUNCS 1

#if defined(INLINE_FUNCS)

#define make_lba(dip, pos)	\
	((pos == (off_t) 0) ? (u_int32) 0 : (pos / lbdata_size))

#define make_offset(dip, lba)	((off_t)(lba * lbdata_size))

#define make_lbdata(dip, pos)	\
	((pos == (off_t) 0) ? (u_int32) 0 : (pos / lbdata_size))

#define make_position(dip, lba)	((off_t)(lba * lbdata_size))

#if defined(_BSD)
#  define get_position(dip) (seek_position (dip, (off_t) 0, L_INCR))
#else /* !defined(_BSD) */
#  define get_position(dip) (seek_position (dip, (off_t) 0, SEEK_CUR))
#endif /* defined(_BSD) */

#endif /* defined(INLINE_FUNCS) */

#if defined(MUNSA)

extern bool		munsa_flag;		/* Enable MUNSA features flag. */
extern dlm_lock_mode_t	munsa_lock_type;	/* Default munsa lock type.    */
extern dlm_lock_mode_t	input_munsa_lock_type;	/* Lock for input file.        */
extern dlm_lock_mode_t	output_munsa_lock_type;	/* Lock for output file.       */
extern char		*resnam;
extern int		resnlen, i;
extern dlm_lkid_t	lkid;
extern dlm_nsp_t	nsp;
extern dlm_valb_t	vb;

#endif /* defined(MUNSA) */

extern struct dinfo *active_dinfo, *input_dinfo, *output_dinfo;

extern clock_t hz;
extern bool tty_saved;
extern int errno, exit_status;
extern unsigned parity_code, data_bits_code;
extern speed_t baud_rate;
extern u_int32 pattern;
extern u_int32 data_patterns[];
extern int npatterns;
extern enum opt softcar_opt;
extern enum flow flow_type;
extern enum iodir io_dir;
extern enum iomode io_mode;
extern enum iotype io_type;
extern enum dispose dispose_mode;
extern enum devtype device_type;
extern enum onerrors oncerr_action;
extern char *parity_str, *flow_str, *speed_str;

extern bool user_pattern, unique_pattern;
extern char *pattern_string;
extern int align_offset, rotate_offset;
extern bool ade_flag, aio_flag, bypass_flag;
extern bool compare_flag, core_dump, cerrors_flag;
extern bool debug_flag, Debug_flag, eDebugFlag, rDebugFlag;
#if defined(EEI)
extern bool eei_flag, eei_resets;
#endif /* defined(EEI) */
extern bool dump_flag, flush_flag, forked_flag, fsync_flag, header_flag;
extern bool keep_existing, user_incr, user_min, user_max;
extern bool lbdata_flag, user_lbdata, user_lbsize;
extern bool user_position, iot_pattern, logdiag_flag;
extern bool loopback, micro_flag, mmap_flag, modem_flag, stdin_flag, stdout_flag;
extern bool max_capacity, media_changed, multi_flag, variable_flag, volumes_flag;
extern bool terminating_flag, ttyport_flag, verbose_flag, verify_flag, verify_only;
extern bool rotate_flag, pad_check, spad_check, pstats_flag, raw_flag, stats_flag;

extern char *cmdname;
extern u_char *base_buffer, *data_buffer, *verify_buffer;
extern char *cmd_line, *msg_buffer;
extern char *log_file, *log_buffer, *log_bufptr;
extern char *input_file, *output_file, *pattern_file;
extern u_char *mmap_buffer, *mmap_bufptr;
extern u_char *pattern_buffer, *pattern_bufptr, *pattern_bufend;
extern size_t patbuf_size;
extern int pattern_size, page_size;
extern size_t block_size, data_size, lbdata_size;
extern u_int32 device_size, lbdata_addr;

extern int open_flags, wopen_flags;
extern size_t dump_limit, incr_count, min_size, max_size;
extern u_long file_limit;
extern u_long pass_count, pass_limit;
extern u_long seek_count, skip_count;
extern large_t record_limit, total_bytes, total_records;
extern large_t total_bytes_read, total_bytes_written;
extern large_t total_files, total_files_read, total_files_written;
extern u_long error_limit;
extern vu_long total_errors;
extern u_long total_partial, warning_errors;
extern u_int cdelay_count, edelay_count, rdelay_count, sdelay_count;
extern u_int tdelay_count, wdelay_count;
extern u_int random_seed;
extern v_int multi_volume;
extern int volume_limit;
extern vu_long volume_records;

/*
 * Volatile Storage:
 */
extern bool eof_status;
extern v_bool end_of_file;
extern u_long random_align;
extern vu_long error_count, partial_records, records_processed;
extern large_t data_limit, rdata_limit, user_capacity;

extern bool tty_minflag;
extern u_short tty_timeout, tty_minimum;

extern off_t file_position, last_position, step_offset;

extern clock_t start_time, end_time, pass_time;
extern struct tms stimes, ptimes, etimes;

#if defined(DEC)
extern bool table_flag;
extern struct tbl_sysinfo s_table, p_table, e_table;
#endif /* defined(DEC) */

extern time_t runtime, elapsed_time;
extern time_t program_start, program_end, error_time;
extern bool TimerActive, TimerExpired;
extern char *user_runtime;

extern pid_t child_pid;
extern int cur_proc, num_procs, num_slices;

#if defined(AIO)
extern int aio_bufs;
#endif /* defined(AIO) */

#if defined(_BSD)
extern union wait child_status;
#else /* !defined(_BSD) */
extern int child_status;
#endif /* defined(_BSD) */

/*
 * Function Prototypes: (No ANSI compiler? too bad... buy one! :-)
 */

/* dt.c */
extern int match(char *s);
extern void report_error(char *error_info, int record_flag);
extern void report_record(
			struct dinfo	*dip,
			u_long		files,
			u_long		records,
			u_int32		lba,
			enum test_mode	mode,
			void		*buffer,
			size_t		bytes );
extern void terminate(int code);
extern int nofunc(struct dinfo *dip);
extern int HandleMultiVolume(struct dinfo *dip);
extern int RequestFirstVolume(struct dinfo *dip, int oflags);
extern int RequestMultiVolume(struct dinfo *dip, bool reopen, int open_flags);

#if defined(MUNSA)
extern void dlm_error(dlm_lkid_t *lk, dlm_status_t l_stat);
#endif /* defined(MUNSA) */

/* dtgen.c */
extern int open_file(struct dinfo *dip, int mode);
extern int close_file(struct dinfo *dip);
extern int reopen_file(struct dinfo *dip, int mode);
extern int initialize(struct dinfo *dip);
extern int init_file(struct dinfo *dip);
extern int flush_file(struct dinfo *dip);
extern int read_file(struct dinfo *dip);
extern int write_file(struct dinfo *dip);
extern int validate_opts(struct dinfo *dip);

/* dtinfo.c */
extern struct dtype *setup_device_type(char *str);
extern struct dinfo *setup_device_info(char *dname, struct dtype *dtp);
extern void system_device_info(struct dinfo *dip);

/* dtread.c */
extern int read_data(struct dinfo *dip);
extern int check_read(struct dinfo *dip, ssize_t count, size_t size);
extern int read_eof(struct dinfo *dip);
extern int read_eom(struct dinfo *dip);
extern ssize_t read_record(	struct dinfo	*dip,
				u_char		*buffer,
				size_t		bsize,
				size_t		dsize,
				int		*status );
extern int verify_record(	struct dinfo	*dip,
				u_char		*buffer,
				size_t		bsize );
extern int FindCapacity(struct dinfo *dip);

/* dtwrite.c */
extern int write_data(struct dinfo *dip);
extern int check_write(struct dinfo *dip, ssize_t count, size_t size);
extern int copy_record(		struct dinfo	*dip,
				u_char		*buffer,
				size_t		bsize );
extern ssize_t write_record(	struct dinfo	*dip,
				u_char		*buffer,
				size_t		bsize,
				size_t		dsize,
				int		*status );
extern int write_verify(	struct dinfo	*dip,
				u_char		*buffer,
				size_t		bsize,
				size_t		dsize,
				off_t		pos );

/* dtstats.c */
extern void gather_stats(struct dinfo *dip);
extern void gather_totals(void);
extern void init_stats(struct dinfo *dip);
extern void report_pass(struct dinfo *dip, enum stats stats_type);
extern void report_stats(struct dinfo *dip, enum stats stats_type);

/* dttty.c */
extern int tty_open(struct dinfo *dip, int mode);
extern int tty_close(struct dinfo *dip);
extern int tty_reopen(struct dinfo *dip, int mode);
extern int tty_flush_data(struct dinfo *dip);
extern int drain_tty(int fd);
extern int flush_tty(int fd);
extern int save_tty(int fd);
extern int restore_tty(int fd);
extern int setup_tty(int fd, int flushing);
extern int setup_baud_rate(u_int32 baud);
extern int SetBlockingMode (int fd);
#if defined(_OSF_SOURCE) || defined(ultrix)
extern unsigned int GetModemSignals(int fd);
extern int HangupModem(int fd);
extern int ShowModemSignals(int fd);
extern int WaitForCarrier(int fd);
#endif /* defined(_OSF_SOURCE) || defined(ultrix) */

/* dtutil.c */
extern int delete_file(struct dinfo *dip);
extern void mySleep(unsigned int sleep_time);
extern void dump_buffer(	char		*name,
				u_char		*base,
				u_char		*ptr,
				size_t		dump_size,
				size_t		bufr_size,
				bool		expected);
extern void fill_buffer(	u_char		*buffer,
				size_t		byte_count,
				u_int32		pattern);
extern void init_buffer(	u_char		*buffer,
				size_t		count,
				u_int32		pattern );
extern u_int32 init_lbdata(	u_char		*buffer,
				size_t		count,
				u_int32		lba,
				u_int32		lbsize );
#if !defined(INLINE_FUNCS)
extern u_int32 make_lba(	struct dinfo	*dip,
				off_t		pos );
extern off_t make_offset(	struct dinfo	*dip,
				u_int32		lba);
extern u_int32 make_lbdata(	struct dinfo	*dip,
				off_t		pos );
#endif /* !defined(INLINE_FUNCS) */
extern u_int32 winit_lbdata(	struct dinfo	*dip,
				off_t		pos,
				u_char		*buffer,
				size_t		count,
				u_int32		lba,
				u_int32		lbsize );
extern u_int32 init_iotdata(	size_t		count,
				u_int32		lba,
				u_int32		lbsize );
extern void init_padbytes(	u_char		*buffer,
				size_t		offset,
				u_int32		pattern);
extern void print_time(clock_t time);
extern void format_time(clock_t time);
#if defined(DEC)
extern void format_ltime (long time, int tps);
#endif /* defined(DEC) */
extern int verify_buffers(	struct dinfo	*dip,
				u_char		*dbuffer,
				u_char		*vbuffer,
				size_t		count );
extern int verify_lbdata(	struct dinfo	*dip,
				u_char		*dbuffer,
				u_char		*vbuffer,
				size_t		count,
				u_int32		*lba );
extern int verify_data(		struct dinfo	*dip,
				u_char		*buffer,
				size_t		byte_count,
				u_int32		pattern,
				u_int32		*lba );
extern int verify_padbytes(	struct dinfo	*dip,
				u_char		*buffer,
				size_t		count,
				u_int32		pattern,
				size_t		offset );
extern void process_pfile(int *fd, char *file, int mode);
extern void copy_pattern(u_int32 pattern, u_char *buffer);
extern void setup_pattern(u_char *buffer, size_t size);
extern off_t seek_file(int fd, u_long records, off_t size, int whence);
extern off_t seek_position(struct dinfo *dip, off_t size, int whence);
#if !defined(INLINE_FUNCS)
extern off_t get_position(struct dinfo *dip);
#endif /* !defined(INLINE_FUNCS) */
extern u_int32 get_lba(struct dinfo *dip);
extern off_t incr_position(struct dinfo *dip, off_t offset);
extern off_t set_position(struct dinfo *dip, off_t offset);
#if !defined(INLINE_FUNCS)
extern off_t make_position(struct dinfo *dip, u_int32 lba);
#endif /* !defined(INLINE_FUNCS) */
extern void show_position(struct dinfo *dip, off_t pos);
extern u_long get_random(void);
extern size_t get_variable(struct dinfo *dip);
extern void set_rseed(u_int seed);
extern off_t do_random(struct dinfo *dip, bool doseek, size_t xfer_size);
extern int skip_records(struct dinfo *dip, u_long records, u_char *buffer, off_t size);
extern void *myalloc(size_t size, int offset);
extern void *Malloc(size_t size);
extern u_long CvtStrtoValue(char *nstr, char **eptr, int base);
extern large_t CvtStrtoLarge(char *nstr, char **eptr, int base);
extern time_t CvtTimetoValue(char *nstr, char **eptr);
#if defined(ultrix)
extern clock_t times(struct tms *tmsp);
#endif /* defined(ultrix) */
#if defined(ultrix)
extern void *valloc(size_t size);
#endif /* defined(ultrix) */
extern void Ctime(time_t timer);
extern u_long RecordError(void);
extern u_long RecordWarning(u_long record);
#if defined(USE_STDARG)
extern void Fprintf(char *format, ...);
extern void Fprint(char *format, ...);
extern void Lprintf(char *format, ...);
#if !defined(_QNX_SOURCE) && !defined(__linux__) && !defined(FreeBSD) && !defined(__osf__) && !defined(__NUTC__)
extern void bzero(char *buffer, size_t length);
#endif /* !defined(_QNX_SOURCE) && !defined(__linux__) && !defined(FreeBSD) && !defined(__osf__) && !defined(__NUTC__) */
#else /* !defined(USE_STDARG) */
extern void Fprintf(), Fprint(), Lprintf();
#endif /* defined(USE_STDARG) */
extern void Lflush(void);
extern int Sprintf(char *bufptr, char *msg, ...);
extern int Fputs(char *str, FILE *stream);
extern int is_Eof(struct dinfo *dip, size_t count, int *status);
extern void set_Eof(struct dinfo *dip);
extern void ReportCompareError(	struct dinfo	*dip,
				size_t		byte_count,
				u_int		byte_position,
				u_int		expected_data,
				u_int		data_found );
extern void ReportDeviceInfo(	struct dinfo	*dip,
				size_t		byte_count,
				u_int		byte_position,
				bool		eio_error );
extern void ReportLbdataError(	struct dinfo	*dip,
			        u_int32		lba,
				u_int32		byte_count,
				u_int32		byte_position,
				u_int32		expected_data,
				u_int32		data_found );

extern int IS_HexString(char *s);
extern int StrCopy(void *to_buffer, void *from_buffer, size_t length);
extern u_long stoh(u_char *bp, size_t size);
extern void htos(u_char *bp, u_long value, size_t size);
extern void LogDiagMsg(char *msg);

/* dtusage.c */
extern char *version_str;
extern void dthelp(void), dtusage(void), dtversion(void);

/* dtprocs.c */
extern void abort_procs(void);
extern void await_procs(void);
extern pid_t fork_process(void);
extern pid_t start_procs(void);
extern pid_t start_slices(void);

#if !defined(_QNX_SOURCE)
/*
 * POSIX does *not* define a special device interface, and since no
 * Magtape API exists, these functions are operating system dependent.
 * [ I'll add QNX tape functions after I find out the interface. ]
 */

/* dttape.c */

#if defined(_QNX_SOURCE) || defined(SCO)
extern int DoIoctl(dinfo_t *dip, int cmd, int argp, caddr_t msgp);
#else /* !defined(_QNX_SOURCE) && !defined(SCO) */
extern int DoIoctl(dinfo_t *dip, int cmd, caddr_t argp, caddr_t msgp);
#endif /* defined(_QNX_SOURCE) || defined(SCO) */
extern int DoMtOp(dinfo_t *dip, short cmd, daddr_t count, caddr_t msgp);
extern int DoWriteFileMark(dinfo_t *dip, daddr_t count);
extern int DoForwardSpaceFile(dinfo_t *dip, daddr_t count);
extern int DoBackwardSpaceFile(dinfo_t *dip, daddr_t count);
extern int DoForwardSpaceRecord(dinfo_t *dip, daddr_t count);
extern int DoBackwardSpaceRecord(dinfo_t *dip, daddr_t count);
extern int DoRewindTape(dinfo_t *dip);
extern int DoTapeOffline(dinfo_t *dip);
extern int DoRetensionTape(dinfo_t *dip);

#if defined(__osf__)			/* Really DEC specific. */

extern int DoSpaceEndOfData(dinfo_t *dip);
extern int DoEraseTape(dinfo_t *dip);
extern int DoTapeOnline(dinfo_t *dip);
extern int DoLoadTape(dinfo_t *dip);
extern int DoUnloadTape(dinfo_t *dip);

#endif /* defined(__osf__) */

#else /* defined(_QNX_SOURCE) */

extern int DoWriteFileMark(dinfo_t *dip, daddr_t count);

#endif /* !defined(_QNX_SOURCE) */

/* dtaio.c */

#if defined(AIO)

#include <aio.h>

extern struct aiocb *current_acb;

extern int dtaio_close_file(struct dinfo *dip);
extern int dtaio_initialize(struct dinfo *dip);
extern int dtaio_cancel(struct dinfo *dip);
extern int dtaio_cancel_reads(struct dinfo *dip);
extern int dtaio_read_data(struct dinfo *dip);
extern int dtaio_write_data(struct dinfo *dip);

#endif /* defined(AIO) */

/* dtmmap.c */

#if defined(MMAP)

extern int mmap_file(struct dinfo *dip);
extern int mmap_flush(struct dinfo *dip);
extern int mmap_reopen_file(struct dinfo *dip, int mode);
extern int mmap_validate_opts(struct dinfo *dip);
extern int mmap_read_data(struct dinfo *dip);
extern int mmap_write_data(struct dinfo *dip);

#endif /* defined(MMAP) */

#if defined(FIFO)

extern int fifo_open(struct dinfo *dip, int mode);

#endif /* defined(FIFO) */

/* dteei.c */

#if defined(EEI)

#include <sys/mtio.h>

extern long mt_blkno, mt_fileno;

/*
 * Functions for Processing EEI Data:
 */
extern bool HandleTapeResets(struct dinfo *dip);

extern bool check_eei_status(struct dinfo *dip, bool retry);
extern void clear_eei_status(int fd, bool startup);
extern int get_eei_status(int fd, struct mtget *mt);

extern void print_mtstatus(int fd, struct mtget *mt, bool print_all);
extern void print_mtio(int fd, struct mtget *mt);
extern void print_eei_status(u_int eei_status);
extern void print_cam_status(u_int cam_status);
extern void print_scsi_status(u_char scsi_status);
extern void print_sense_data(u_char *scsi_sense_ptr);

#endif /* defined(EEI) */
