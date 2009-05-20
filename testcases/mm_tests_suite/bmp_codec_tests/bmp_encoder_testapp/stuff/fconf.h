#ifndef __FCONF_H__
#define __FCONF_H__

#include <bmp_enc_interface.h>

#ifndef MAX_STR_LEN
    #define MAX_STR_LEN 80
#endif

typedef struct flist
{
    char inp_fname[MAX_STR_LEN];
    char out_fname[MAX_STR_LEN];
    char ref_fname[MAX_STR_LEN];

    int width;
    int height;
    compression_type cmpr;
    pixel_format pfmt;
    int colors_in_output;

    struct flist * next;

} flist_t;

flist_t * new_entry();
flist_t * mk_entry( char * width, char * height,
                    char * cmpr,
                    char * pfmt,
                    char * colors_in_output,
                    const char * inp_fname,
            const char * out_fname,
		            const char * ref_fname );
void      delete_list( flist_t * list );

/* Aux */
int read_cfg( const char * filename, flist_t ** pplist );


#endif //__FCONF_H__
