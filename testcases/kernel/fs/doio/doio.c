#include "pattern.h"

#define	NMEMALLOC	32
#define	MEM_DATA	1	/* data space 				*/
#define	MEM_SHMEM	2	/* System V shared memory 		*/
#define	MEM_T3ESHMEM	3	/* T3E Shared Memory 			*/
#define	MEM_MMAP	4	/* mmap(2) 				*/

#define	MEMF_PRIVATE	0001
#define	MEMF_AUTORESRV	0002
#define	MEMF_LOCAL	0004
#define	MEMF_SHARED	0010

#define	MEMF_FIXADDR	0100
#define	MEMF_ADDR	0200
#define	MEMF_AUTOGROW	0400
#define	MEMF_FILE	01000	/* regular file -- unlink on close	*/
#define	MEMF_MPIN	010000	/* use mpin(2) to lock pages in memory */

struct memalloc {
	int	memtype;
	int	flags;
	int	nblks;
	char	*name;
	void	*space;		/* memory address of allocated space */
	int	fd;		/* FD open for mmaping */
	int	size;
}	Memalloc[NMEMALLOC];

/*
 * Structure for maintaining open file test descriptors.  Used by
 * alloc_fd().
 */

struct fd_cache {
	char    c_file[MAX_FNAME_LENGTH+1];
	int	c_oflags;
	int	c_fd;
	long    c_rtc;
#ifdef sgi
	int	c_memalign;	/* from F_DIOINFO */
	int	c_miniosz;
	int	c_maxiosz;
#endif
#ifndef CRAY
	void	*c_memaddr;	/* mmapped address */
	int	c_memlen;	/* length of above region */
#endif
};

/*
 * Name-To-Value map
 * Used to map cmdline arguments to values
 */
struct smap {
	char    *string;
	int	value;
};

struct aio_info {
	int			busy;
	int			id;
	int			fd;
	int			strategy;
	volatile int		done;
#ifdef CRAY
	struct iosw		iosw;
#endif
#ifdef sgi
	aiocb_t			aiocb;
	int			aio_ret;	/* from aio_return */
	int			aio_errno;	/* from aio_error */
#endif
	int			sig;
	int			signalled;
	struct sigaction	osa;
};

/* ---------------------------------------------------------------------------
 *
 * A new paradigm of doing the r/w system call where there is a "stub"
 * function that builds the info for the system call, then does the system
 * call; this is called by code that is common to all system calls and does
 * the syscall return checking, async I/O wait, iosw check, etc.
 *
 * Flags:
 *	WRITE, ASYNC, SSD/SDS,
 *	FILE_LOCK, WRITE_LOG, VERIFY_DATA,
 */

struct	status {
	int	rval;		/* syscall return */
	int	err;		/* errno */
	int	*aioid;		/* list of async I/O structures */
};

struct syscall_info {
	char		*sy_name;
	int		sy_type;
	struct status	*(*sy_syscall)();
	int		(*sy_buffer)();
	char		*(*sy_format)();
	int		sy_flags;
	int		sy_bits;
};

#define	SY_WRITE		00001
#define	SY_ASYNC		00010
#define	SY_IOSW			00020
#define	SY_SDS			00100
char		*syserrno(int err);
void		doio(void);
void		doio_delay(void);
char		*format_oflags(int oflags);
char		*format_strat(int strategy);
char		*format_rw(struct io_req *ioreq, int fd, void *buffer,
			int signo, char *pattern, void *iosw);
#ifdef CRAY
char		*format_sds(struct io_req *ioreq, void *buffer, int sds
			char *pattern);
#endif /* CRAY */

int		do_read(struct io_req *req);
int		do_write(struct io_req *req);
int		lock_file_region(char *fname, int fd, int type, int start,
			int nbytes);

#ifdef CRAY
char		*format_listio(struct io_req *ioreq, int lcmd,
			struct listreq *list, int nent, int fd, char *pattern);
#endif /* CRAY */

int		do_listio(struct io_req *req);
#if defined(_CRAY1) || defined(CRAY)
int		do_ssdio(struct io_req *req);
#endif /* defined(_CRAY1) || defined(CRAY) */

char		*fmt_ioreq(struct io_req *ioreq, struct syscall_info *sy,
			int fd);

#ifdef CRAY
struct status	*sy_listio(struct io_req *req, struct syscall_info *sysc,
			int fd, char *addr);
int		listio_mem(struct io_req *req, int offset, int fmstride,
			int *min, int *max);
char		*fmt_listio(struct io_req *req, struct syscall_info *sy,
			int fd, char *addr);
#endif /* CRAY */

#ifdef sgi
struct status	*sy_pread(struct io_req *req, struct syscall_info *sysc,
			int fd, char *addr);
struct status	*sy_pwrite(struct io_req *req, struct syscall_info *sysc,
			int fd, char *addr);
char		*fmt_pread(struct io_req *req, struct syscall_info *sy,
			int fd, char *addr);
#endif	/* sgi */
struct status	*sy_readv(struct io_req *req, struct syscall_info *sysc,
			int fd, char *addr);
struct status	*sy_writev(struct io_req *req, struct syscall_info *sysc,
			int fd, char *addr);
struct status	*sy_rwv(struct io_req *req, struct syscall_info *sysc,
			int fd, char *addr, int rw);
char		*fmt_readv(struct io_req *req, struct syscall_info *sy,
			int fd, char *addr);
#endif /* !CRAY */

#ifdef sgi
struct status	*sy_aread(struct io_req *req, struct syscall_info *sysc,
			int fd, char *addr);
struct status	*sy_awrite(struct io_req *req, struct syscall_info *sysc,
			int fd, char *addr)
struct status	*sy_arw(struct io_req *req, struct syscall_info *sysc,
			int fd, char *addr, int rw);
char		*fmt_aread(struct io_req *req, struct syscall_info *sy,
			int fd, char *addr);
#endif /* sgi */

#ifndef CRAY
struct status	*sy_mmread(struct io_req *req, struct syscall_info *sysc,
			int fd, char *addr);
struct status	*sy_mmwrite(struct io_req *req, struct syscall_info *sysc,
			int fd, char *addr);
struct status	*sy_mmrw(struct io_req *req, struct syscall_info *sysc,
			int fd, char *addr, int rw);
char		*fmt_mmrw(struct io_req *req, struct syscall_info *sy, int fd,
			char *addr);
#endif /* !CRAY */

int		do_rw(struct io_req *req);

#ifdef sgi
int		do_fcntl(struct io_req *req);
#endif /* sgi */

#ifndef CRAY
int		do_sync(struct io_req *req);
#endif /* !CRAY */

int		doio_pat_fill(char *addr, int mem_needed, char *Pattern,
			int Pattern_Length, int shift);
char		*doio_pat_check(char *buf, int offset, int length,
			char *pattern, int pattern_length, int patshift);
char		*check_file(char *file, int offset, int length, char *pattern,
			int pattern_length, int patshift, int fsa);
int		doio_fprintf(FILE *stream, char *format, ...);
int		alloc_mem(int nbytes);
#if defined(_CRAY1) || defined(CRAY)
int		alloc_sds(int nbytes);
#endif /* defined(_CRAY1) || defined(CRAY) */

int		alloc_fd(char *file, int oflags);
struct fd_cache	*alloc_fdcache(char *file, int oflags);

#ifdef sgi
void		signal_info(int sig, siginfo_t *info, void *v);
void		cleanup_handler(int sig, siginfo_t *info, void *v);
void		die_handler(int sig, siginfo_t *info, void *v);
void		sigbus_handler(int sig, siginfo_t *info, void *v);
#else	/* !sgi */
void		cleanup_handler(int sig);
void		die_handler(int sig);

#ifndef CRAY
void		sigbus_handler(int sig);
#endif /* !CRAY */
#endif /* sgi */

void		noop_handler(int sig);
void		sigint_handler(int sig);
void		aio_handler(int sig);
void		dump_aio(void);

#ifdef sgi
void		cb_handler(sigval_t val);
#endif /* sgi */

struct aio_info	*aio_slot(int aio_id);
int		aio_register(int fd, int strategy, int sig);
int		aio_unregister(int aio_id);

#ifndef __linux__
int		aio_wait(int aio_id);
#endif /* !__linux__ */

char		*hms(time_t t);
int		aio_done(struct aio_info *ainfo);
void		doio_upanic(int mask);
int		parse_cmdline(int argc, char **argv, char *opts);

#ifndef CRAY
void		parse_memalloc(char *arg);
void		dump_memalloc(void);
#endif /* !CRAY */

void		parse_delay(char *arg);
int		usage(FILE *stream);
void		help(FILE *stream);
main(int argc, char **argv)









doio(void)
		if (rval == SKIP_REQ) {
		if (delayop != 0)
doio_delay(void)
	if (oflags & O_EXCL)
	if (oflags & O_SYNC)
	if (oflags & O_RAW)
	if (oflags & O_WELLFORMED)
	if (oflags & O_SSD)
	if (oflags & O_LDRAW)
	if (oflags & O_PARALLEL)
	if (oflags & O_BIG)
	if (oflags & O_PLACE)
	if (oflags & O_ASYNC)
	if (oflags & O_DIRECT)
	if (oflags & O_DSYNC)
	if (oflags & O_RSYNC)
format_rw(struct io_req *ioreq, int fd, void *buffer, int signo, char *pattern,
	void *iosw)
	if (errbuf == NULL)
format_sds(struct io_req *ioreq, void *buffer, int sds, char *pattern)
	if (errbuf == NULL)
do_read(struct io_req *req)
	if ((req->r_data.read.r_uflags & F_WORD_ALIGNED)) {
		if ((oflags & O_DIRECT) && ((long)addr % fdc->c_memalign != 0)) {
do_write(struct io_req *req)
	off_t    	    	offset, woffset;
	woffset = 0;

		if (addr != Memptr)
	if ((req->r_data.write.r_uflags & F_WORD_ALIGNED)) {
		if ((oflags & O_DIRECT) && ((long)addr % fdc->c_memalign != 0)) {
	if (addr != Memptr)
	if (addr != Memptr)
lock_file_region(char *fname, int fd, int type, int start, int nbytes)
format_listio(struct io_req *ioreq, int lcmd, struct listreq *list,
	int nent, int fd, char *pattern)
	if (errbuf == NULL)

do_listio(struct io_req *req)
		if (addr != Memptr)
do_ssdio(struct io_req *req)
do_ssdio(struct io_req *req)
#endif /* CRAY */
	if (errbuf == NULL)
	for (aname=aionames;
	if (sy->sy_flags & SY_WRITE) {
	if (sy->sy_flags & SY_ASYNC) {
	if (io->r_oflags & O_RAW) {
	if (io->r_oflags & O_DIRECT) {

		if (fcntl(fd, F_DIOINFO, &finfo) == -1) {
sy_listio(struct io_req *req, struct syscall_info *sysc, int fd, char *addr)
	if (status == NULL) {
	if (status->aioid == NULL) {
	if (lio_req == NULL) {
	for (l=lio_req,a=addr,o=offset,i=0;
	if ((status->rval = listio(lc, lio_req, nents)) == -1) {
listio_mem(struct io_req *req, int offset, int fmstride, int *min, int *max)
	if (errbuf == NULL) {
		if (errbuf == NULL) {
			doio_fprintf(stderr, "malloc failed, %s/%d\n",
				__FILE__, __LINE__);
				return NULL;
sy_pread(struct io_req *req, struct syscall_info *sysc, int fd, char *addr)
	if (status == NULL) {
sy_pwrite(struct io_req *req, struct syscall_info *sysc, int fd, char *addr)
	if (status == NULL) {
	if (errbuf == NULL) {
		if (errbuf == NULL) {
sy_readv(struct io_req	*req, struct syscall_info *sysc, int fd, char *addr)
sy_writev(struct io_req *req, struct syscall_info *sysc, int fd, char *addr)
sy_rwv(struct io_req *req, struct syscall_info *sysc, int fd, char *addr,
	int rw)
	if (status == NULL) {
	if (rw)
sy_aread(struct io_req *req, struct syscall_info *sysc, int fd, char *addr)
sy_awrite(struct io_req *req, struct syscall_info *sysc, int fd, char *addr)
sy_arw(struct io_req *req, struct syscall_info *sysc, int fd, char *addr,
	int rw)
	if (status == NULL) {
	if (aio_strat == A_SIGNAL) {	/* siginfo(2) stuff */
	} else if (aio_strat == A_CALLBACK) {
	if (rw)
	if (status->aioid == NULL) {
sy_mmread(struct io_req *req, struct syscall_info *sysc, int fd, char *addr)
sy_mmwrite(struct io_req *req, struct syscall_info *sysc, int fd, char *addr)
sy_mmrw(struct io_req *req, struct syscall_info *sysc, int fd, char *addr,
	int rw)
	if (status == NULL) {
	if (v_opt || fdc->c_memaddr == NULL) {
		if (fstat(fd, &sbuf) < 0) {
		if (mrc == MAP_FAILED) {
	if (rw)

do_rw(struct io_req *req)
	int	    		logged_write, got_lock, pattern;
	off_t			woffset;
	woffset = 0;

	if (nents >= MAX_AIO) {
	for (sy=syscalls; sy->sy_name != NULL && sy->sy_type != req->r_type; sy++)
	if (sy->sy_name == NULL) {
	if (sy->sy_buffer != NULL) {
		if (sy->sy_flags & SY_WRITE) {
		if ((oflags & O_DIRECT) && ((long)addr % fdc->c_memalign != 0)) {
			if (addr != Memptr)
		if (sy->sy_buffer != NULL) {
	if (s->rval == -1) {
		for (i=0; i < nents; i++) {
			if (s->aioid == NULL)
		if (sy->sy_flags & SY_ASYNC) {
			for (i=0; i < nents; i++) {
		if (sy->sy_flags & SY_IOSW) {
			for (i=0; i < nents; i++) {
				if (s->aioid == NULL)
				if (iosw->sw_error != 0) {
				} else if (iosw->sw_count != nbytes*nstrides) {
			for (i = 0; s->aioid[i] != -1; i++) {
				if (s->aioid == NULL) {
			if (s->rval != mem_needed) {
	if (rval == 0 && sy->sy_flags & SY_WRITE && v_opt) {
	if (s->aioid != NULL)
do_fcntl(struct io_req *req)
	if (rval == -1) {
#endif /* sgi */
do_sync(struct io_req *req)
#endif /* !CRAY */
	int shift)
doio_pat_check(char *buf, int offset, int length, char *pattern,
	int pattern_length, int patshift)

check_file(char *file, int offset, int length, char *pattern,
	int pattern_length, int patshift, int fsa)
	if ((flags & O_DIRECT) && ((long)buf % fdc->c_memalign != 0)) {

	if ((em = (*Data_Check)(buf, offset, length, pattern, pattern_length, patshift)) != NULL) {
alloc_mem(int nbytes)
	if (nbytes == -1) {
		for (me=0; me < Nmemalloc; me++) {
			if (Memalloc[me].space == NULL)
				if (Memalloc[me].flags & MEMF_MPIN)
				if (Memalloc[me].flags & MEMF_MPIN)
				if (Memalloc[me].flags & MEMF_MPIN)
				if (Memalloc[me].flags & MEMF_FILE) {
	if (mturn >= Nmemalloc)
		if (nbytes > M->size) {
			if (M->space != NULL) {
				if (M->flags & MEMF_MPIN)
		if (M->space == NULL) {
			if ((cp = malloc( nbytes )) == NULL) {
			if (M->flags & MEMF_MPIN) {
				if (mpin(cp, nbytes) == -1) {
		if (nbytes > M->size) {
			if (M->space != NULL) {
				if (M->flags & MEMF_MPIN)
				if (M->flags & MEMF_FILE)
		if (M->space == NULL) {
			if (strchr(M->name, '%')) {
			if ((M->fd = open(M->name, O_CREAT|O_RDWR, 0666)) == -1) {
			if (M->flags & MEMF_PRIVATE)
			if (M->flags & MEMF_LOCAL)
			if (M->flags & MEMF_AUTORESRV)
			if (M->flags & MEMF_AUTOGROW)
			if (M->flags & MEMF_SHARED)
			if ( (M->space = mmap(addr, M->size,

		if (nbytes > M->size) {
			if (M->space != NULL) {
				if (M->flags & MEMF_MPIN)
		if (M->space == NULL) {
			if (!strcmp(M->name, "private")) {
			if (nbytes > M->size) {
			if (shmid == -1) {
			if (M->space == (void *)-1) {
			if (M->flags & MEMF_MPIN) {
				if (mpin(M->space, M->size) == -1) {
#else /* CRAY */
alloc_mem(int nbytes)
	if (nbytes == -1) {
	if (nbytes > Memsize) {
	    if (Memsize != 0)
	    if ((cp = malloc_space = malloc( nbytes )) == NULL) {
	    if (ip & 0x3F != 0) {
		if ((cp = malloc_space = malloc(nbytes + 0x40)) == NULL) {
alloc_sds(int nbytes)
alloc_sds(int nbytes)
alloc_fd(char *file, int oflags)
	if (fdc != NULL)
alloc_fdcache(char *file, int oflags)
              return 0;
	if (oflags & O_DIRECT) {
		if (fcntl(fd, F_DIOINFO, &finfo) == -1) {
	if (info != NULL) {
		if (!haveit) {
			if ((info->si_signo == SIGSEGV) ||
			   (info->si_signo == SIGBUS) ) {
		if (!haveit) {
	if (active_mmap_rw && havesigint && (info->si_errno == EINTR)) {
cleanup_handler(int sig)
die_handler(int sig)
sigbus_handler(int sig)
	if (active_mmap_rw && havesigint)
		cleanup_handler(sig);
noop_handler(int sig)
sigint_handler(int sig)
aio_handler(int sig)
dump_aio(void)
cb_handler(sigval_t val)
aio_slot(int aio_id)
	if (aiop == NULL) {
aio_register(int fd, int strategy, int sig)
aio_unregister(int aio_id)
aio_wait(int aio_id)
		while (!aiop->signalled || !aiop->done) {
			if (r == -1) {
		} while (aiop->done == 0);
		if (cnt > 1)
		if (r == -1) {
hms(time_t t)
	if ((ainfo->aio_errno = aio_error(&ainfo->aiocb)) == -1) {
	if (ainfo->aio_errno != EINPROGRESS) {
		if ((ainfo->aio_ret = aio_return(&ainfo->aiocb)) == -1) {
doio_upanic(int mask)
parse_cmdline(int argc, char **argv, char *opts)
			for (s=checkmap; s->string != NULL; s++)
				if (!strcmp(s->string, optarg))
			for (ma=0; ma < nmemargs; ma++) {
	if (Nmemalloc >= NMEMALLOC) {
	if (!strcmp(allocargs[0], "data")) {
		if (nalloc >= 2) {
			if (strchr(allocargs[1], 'p'))
	} else if (!strcmp(allocargs[0], "mmap")) {
		if (nalloc >= 1) {
			if (strchr(allocargs[1], 'p'))
			if (strchr(allocargs[1], 'a'))
			if (strchr(allocargs[1], 'l'))
			if (strchr(allocargs[1], 's'))
			if (strchr(allocargs[1], 'f'))
			if (strchr(allocargs[1], 'A'))
			if (strchr(allocargs[1], 'G'))
			if (strchr(allocargs[1], 'U'))
		if (nalloc > 2) {
			if (!strcmp(allocargs[2], "devzero")) {
				if (M->flags &
			if (M->flags &
	} else if (!strcmp(allocargs[0], "shmem")) {
		if (nalloc >= 2) {
		if (nalloc >= 3) {
		if (nalloc >= 4) {
			if (strchr(allocargs[3], 'p'))
dump_memalloc(void)
	if (Nmemalloc == 0) {
	for (ma=0; ma < Nmemalloc; ma++) {
	if (ndelay < 2) {
	for (s=delaymap; s->string != NULL; s++)
		if (!strcmp(s->string, delayargs[0]))
	if (ndelay > 2) {
usage(FILE *stream)
help(FILE *stream)