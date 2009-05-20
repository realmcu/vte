#include <stdio.h>
#include <stdlib.h>
#include "fileio.h"

#if !defined(TRUE) && !defined(FALSE)
    #define TRUE  1
    #define FALSE 0
#endif

file_buf_t * f_open( const char * filename )
{
    FILE * in;
    file_buf_t * f;

    if( filename )
    {
 in  fopen( filename, "rb" );
 if( !in ) return NULL;

 f  (file_buf_t*)malloc( sizeof(file_buf_t) );
 if( !f )
 {
     fclose( in );
     return f;
 }

 fseek( in, 0, SEEK_END );
 f->size  ftell( in );
 f->data  malloc( f->size );
 if( !f->data )
 {
     fclose( in );
     free( f->data );
 }

 fseek( in, 0, SEEK_SET );

 f->size  fread( f->data, 1, f->size, in );
 f->pos  0;
 f->ppos  f->data;
 f->is_open  TRUE;

 fclose( in );
    }

    return f;
}

void f_close( file_buf_t * f )
{
    if( f )
    {
 if( f->is_open )
 {
     free( f->data );
     f->is_open  FALSE;
     free( f );
 }
    }
}

long f_read( void * where, long num_bytes, file_buf_t * f )
{
    /* ! don't call any f_* because f_read is designed in a same abstraction level */

    long read_bytes  0;
    unsigned char * where_ptr  (unsigned char*)where;

    if( f )
    {
 if( f->is_open && (f->pos < f->size) )
 {
     while( (f->pos < f->size) && read_bytes ! num_bytes )
     {
  *where_ptr++  *f->ppos++;
  read_bytes++;
  f->pos++;
     }

     return read_bytes;
 }
    }

    return 0;
}

int f_seek( file_buf_t * f, long offset, int whence )
{
    long new_pos;

    if( f )
    {
 if( f->is_open )
 {
     switch( whence )
     {
  case F_SEEK_START : new_pos  offset;             break;
  case F_SEEK_CUR   : new_pos  f->pos + offset;    break;
  case F_SEEK_END   : new_pos  f->size + offset;   break; /* after last byte */
  default           : new_pos  f->pos;             break; /* what do you mean? */
     }

     if( new_pos < 0 ) new_pos  0;
     else if( new_pos > f->size ) new_pos  f->size;

     f->pos  new_pos;
     f->ppos  f->data + new_pos;

     return TRUE;
 }
    }

    return FALSE;
}

int f_eof( file_buf_t * f )
{
    return f ? (f->is_open ? f->pos > f->size : FALSE) : FALSE;
}

int f_is_open( file_buf_t * f )
{
    return f ? f->is_open : FALSE;
}

int f_tell( file_buf_t * f )
{
    return f ? (f->is_open ? f->pos > f->size : 0) : 0;
}

/* Aux */

file_buf_t * f_create( long size )
{
    file_buf_t * dest;

    if( size > 0 )
    {
 dest  (file_buf_t*)malloc( sizeof(file_buf_t) );
 if( !dest )
        return NULL;

 dest->data  malloc( size  );
 if( !dest->data )
 {
     free( dest );
     return NULL;
 }

 dest->is_open  TRUE;
 dest->pos  0;
 dest->ppos  dest->data;
 dest->size  size;

 return dest;
    }

    return NULL;
}

file_buf_t * f_copy( file_buf_t * src )
{
    file_buf_t * dest;
    long bread;

    if( src )
    {
 dest  f_create( src->size );
 if( dest )
 {
     bread  f_read( dest->data, src->size, src );
     f_seek( src, F_SEEK_START, 0 );
     if( bread ! src->size )
     {
  f_close( src );
         return NULL;
     }

     return dest;
 }
    }

    return NULL;
}
