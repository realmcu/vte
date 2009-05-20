#ifndef __FILEIO_H__
#define __FILEIO_H__

#define F_SEEK_START 0
#define F_SEEK_CUR   1
#define F_SEEK_END   2


typedef struct
{
    unsigned char * data;
    unsigned char * ppos;
    long	    size;
    long       pos;
    int             is_open;
} file_buf_t;

file_buf_t * f_open( const char * filename );
void         f_close( file_buf_t * f );
long         f_read( void * where, long num_bytes, file_buf_t * f );
int          f_seek( file_buf_t * f, long offset, int whence );
int          f_eof( file_buf_t * f );
int          f_is_open( file_buf_t * f );

/* Auxiliary */
file_buf_t * f_create( long size );
file_buf_t * f_copy( file_buf_t * src );

#endif /* __FILEIO_H__ */
