#ifndef __FCONF_H__
#define __FCONF_H__

#ifndef MAX_STR_LEN
    #define MAX_STR_LEN 80
#endif    

#include <bmp_interface.h>

typedef struct flist
{
    char inp_fname[MAX_STR_LEN];
    char out_fname[MAX_STR_LEN];
    char ref_fname[MAX_STR_LEN];
    
    output_format       out_fmt;
    scaling_mode        scale_mode;
    int                 width, height;
    
    struct flist * next;    
    
} flist_t;

flist_t * mk_entry( const char * out_fmt,
		            const char * scale_mode,
                    const char * width,
                    const char * height,
	        	    const char * inp_fname, 
		            const char * out_fname, 
        		    const char * ref_fname );
void      delete_list( flist_t * list );

/* Aux */
int read_cfg( const char * filename, flist_t ** pplist );


#endif //__FCONF_H__
