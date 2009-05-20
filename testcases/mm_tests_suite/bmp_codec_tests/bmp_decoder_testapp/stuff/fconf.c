/*---------------------------------------------------------------------*/
/**
*/
/*---------------------------------------------------------------------*/
#include "fconf.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#if !defined(TRUE) && !defined(FALSE)
    #define TRUE  1
    #define FALSE 0
#endif

#define str_equal(s1,s2) (!strcmp((s1),(s2)))

/*---------------------------------------------------------------------*/
/**
*/
/*---------------------------------------------------------------------*/
flist_t * mk_entry( const char * out_fmt,
              const char * scale_mode,
                    const char * width,
                    const char * height,
             const char * inp_fname,
              const char * out_fname,
           const char * ref_fname )
{
    flist_t * list  malloc( sizeof(flist_t) );
    if( list )
    {
    if( (strlen(inp_fname) < MAX_STR_LEN) &&
         (strlen(out_fname) < MAX_STR_LEN) &&
         (strlen(ref_fname) < MAX_STR_LEN) )
    {
         memset( list->inp_fname, 0, MAX_STR_LEN );
         memset( list->out_fname, 0, MAX_STR_LEN );
        memset( list->ref_fname, 0, MAX_STR_LEN );

         strcpy( list->inp_fname, inp_fname );
        strcpy( list->out_fname, out_fname );
         strcpy( list->ref_fname, ref_fname );

            list->out_fmt  E_OUTPUTFORMAT_RGB888;
            if( str_equal("rgb_565", out_fmt) )
            {
                list->out_fmt  E_OUTPUTFORMAT_RGB565;
            }

            list->scale_mode  E_NO_SCALE;
            if( str_equal("scale", scale_mode) )
            {
                list->scale_mode  E_INT_SCALE_PRESERVE_AR;
            }

            list->width  atoi(width);
            list->height  atoi(height);

        list->next  NULL;

         return list;
    }
    }

    return NULL;
}

/*---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*/
void delete_list( flist_t * list )
{
    flist_t * node  list;
    flist_t * next  node;

    while( node )
    {
    next  node->next;
    free( node ); node  NULL;
     node  next;
    }
}

/*---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*/
int read_cfg( const char * filename, flist_t ** pplist )
{
    FILE * in;
    char line[7][MAX_STR_LEN];
    int n  0;

    flist_t * flist  NULL;
    flist_t * node  NULL;

    if( (in  fopen( filename, "rt" )) )
    {
     while( !feof(in) )
    {
         fscanf( in, "%s", line[n] );

         if( n  6 )
        {
         if( !flist )
          {
              flist  mk_entry( line[0], line[1], line[2], line[3], line[4], line[5], line[6] );
           if( !flist )
             {
            *pplist  flist;
                 return FALSE;
              }
           node  flist;
       }
             else
       {
             node->next  mk_entry( line[0], line[1], line[2], line[3], line[4], line[5], line[6] );
            node  node->next;
             if( !node )
              {
              *pplist  flist;
           return FALSE;
             }
          }
        }
         ++n;
         n % 7;
     }
    }
    else
    {
        return FALSE;
    }

    *pplist  flist;

    return TRUE;
}
