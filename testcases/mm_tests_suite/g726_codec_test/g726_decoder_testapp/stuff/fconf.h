#ifndef __FCONF_H__
#define __FCONF_H__

#ifndef MAX_STR_LEN
    #define MAX_STR_LEN 80
#endif

typedef struct flist
{
    char hom_fname[MAX_STR_LEN];

    int  bitrate;
    int  pcm_format;

    char inp_fname[MAX_STR_LEN];
    char out_fname[MAX_STR_LEN];
    char ref_fname[MAX_STR_LEN];

    struct flist * next;

} flist_t;

flist_t * mk_entry( const char * hom_fname,
		    int bitrate,
		    int pcm_format,
		    const char * inp_fname,
		    const char * out_fname,
		    const char * ref_fname );
void      delete_list( flist_t * list );

/* Aux */
int read_cfg( const char * filename, flist_t ** pplist );


#endif //__FCONF_H__
