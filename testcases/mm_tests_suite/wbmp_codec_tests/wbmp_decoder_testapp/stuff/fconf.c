/*---------------------------------------------------------------------*/
/**
*/
/*---------------------------------------------------------------------*/
#include "fconf.h" 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wbmp_interface.h>


#if !defined(TRUE) && !defined(FALSE)
    #define TRUE  1
    #define FALSE 0
#endif

#define NA "n/a"


/*---------------------------------------------------------------------*/
/**
    Make entry from 3 string's.
    
    @return The new_entry returns pointer to the new entry or NULL 
	    in error case.
*/
/*---------------------------------------------------------------------*/
flist_t * mk_entry( int scale_mode,
		            int out_width,
		            int out_height,
		            const char * inp_fname, 
		            const char * out_fname, 
		            const char * ref_fname )
{
    flist_t * list = malloc( sizeof(flist_t) );
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
            
            list->next = NULL;
            list->scale_mode = scale_mode;
            list->out_width = out_width;
            list->out_height = out_height;
            
            return list;
        }
    }
    
    return NULL;
}

/*---------------------------------------------------------------------*/
/**
    Delete linked list without recursion.
    
    @param list - Pointer to head of the list.
*/
/*---------------------------------------------------------------------*/
void delete_list( flist_t * list )
{
    flist_t * node = list;
    flist_t * next = node;
    
    while( node )
    {
        next = node->next;		
        free( node ); node = NULL;
        node = next;
    }
}

/*---------------------------------------------------------------------*/
/**
    This routine reads list of entries from file.
    
    @param filename - Name of file.
    @param pplist   - Double pointer to the list which will contain \n
		      list of entries.

    @return ok ? TRUE : FALSE.
*/
/*---------------------------------------------------------------------*/
int read_cfg( const char * filename, flist_t ** pplist ) 
{
    FILE * in;
    char line[6][MAX_STR_LEN];
    int n = 0;
    
    flist_t * flist = NULL;
    flist_t * node;
    int scale_mode, out_width, out_height;
    
    if( (in = fopen( filename, "rt" )) )
    {
        while( !feof(in) )
        {	
            fscanf( in, "%s", line[n] );	    
            
            if( n == 5 )
            {
                if( !strcmp( line[0], "WBMP_E_INT_SCALE_PRESERVE_AR" ) ) 
                    scale_mode = WBMP_E_INT_SCALE_PRESERVE_AR;		
                else 
                    scale_mode = WBMP_E_NO_SCALE;		
                
                out_width  = !strcmp( line[1], NA ) ? 0 : atoi( line[1] );
                out_height = !strcmp( line[2], NA ) ? 0 : atoi( line[2] );
                
                if( !flist )
                {		    
                    flist = mk_entry( scale_mode, out_width, out_height, line[3], line[4], line[5] );		    		    		    		    
                    if( !flist )
                    {
                        *pplist = flist;
                        return FALSE;
                    }
                    node = flist;
                }
                else
                {
                    node->next = mk_entry( scale_mode, out_width, out_height, line[3], line[4], line[5] );
                    node = node->next;
                    if( !node )
                    {
                        *pplist = flist;
                        return FALSE;
                    }
                }
            }	    
            /*n = ++n % 3;*/
            /* to avoid -oN gcc flag */
            ++n;
            n %= 6;
        }
        *pplist = flist;    
        return TRUE;
    }   
    return FALSE;
}
