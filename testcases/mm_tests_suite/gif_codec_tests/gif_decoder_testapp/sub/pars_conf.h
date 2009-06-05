#ifndef __FCONF_H__
#define __FCONF_H__

#ifndef MAX_STR_LEN
    #define MAX_STR_LEN 80
#endif    

#if !defined(TRUE) && !defined(FALSE)
    #define TRUE  1
    #define FALSE 0
#endif

typedef struct flist
{
    //char hom_fname[MAX_STR_LEN];
    int rgb_format;
    int skale_mod;
    int width;
    int height;
    int count_frames;

    char rgb_str[7];
    
    char im_file[MAX_STR_LEN];
    char inp_dir[MAX_STR_LEN];
    char out_dir[MAX_STR_LEN];
    char ref_dir[MAX_STR_LEN];

    struct flist * next;    
    
} flist_t;

flist_t * mk_entry(int rgb_format, int skale_mod, int width,int height,
            const char * inp_dir, 
            const char * out_dir, 
            const char * ref_dir );
void delete_list( flist_t * list );

int read_cfg( const char * filename, flist_t ** pplist );

int get_indoffile(const char *st);

#endif //__FCONF_H__
