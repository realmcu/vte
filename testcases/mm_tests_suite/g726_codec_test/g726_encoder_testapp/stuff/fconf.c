/*---------------------------------------------------------------------*/
/**
*/
/*---------------------------------------------------------------------*/
#include "fconf.h" 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <g726_dec_api.h>


#if !defined(TRUE) && !defined(FALSE)
    #define TRUE  1
    #define FALSE 0
#endif

 
/*---------------------------------------------------------------------*/
/**
    Make entry from 3 string's.
    
    @return The new_entry returns pointer to the new entry or NULL 
	    in error case.
*/
/*---------------------------------------------------------------------*/
flist_t * mk_entry( const char * hom_fname,
		    int bitrate,
		    int pcm_format,
		    const char * inp_fname, 
		    const char * out_fname, 
		    const char * ref_fname )
{
    flist_t * list = malloc( sizeof(flist_t) );
    if( list )
    {
        if( (strlen(hom_fname) < MAX_STR_LEN) &&
            (strlen(inp_fname) < MAX_STR_LEN) &&
            (strlen(out_fname) < MAX_STR_LEN) && 
            (strlen(ref_fname) < MAX_STR_LEN) )
        {
            memset( list->hom_fname, 0, MAX_STR_LEN );
            memset( list->inp_fname, 0, MAX_STR_LEN );
            memset( list->out_fname, 0, MAX_STR_LEN );
            memset( list->ref_fname, 0, MAX_STR_LEN );
            
            strcpy( list->hom_fname, hom_fname );	    
            strcpy( list->inp_fname, inp_fname );	    
            strcpy( list->out_fname, out_fname );
            strcpy( list->ref_fname, ref_fname );
            
            list->next = NULL;
            list->bitrate = bitrate ;
            list->pcm_format = pcm_format;
            
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
    int bitrate = BIT_RATE_16KBPS, pcm_format = G726_PCM_LINEAR;
    
    if( (in = fopen( filename, "rt" )) )
    {
        while( !feof(in) )
        {	
            fscanf( in, "%s", line[n] );	    
            
            if( n == 5 )
            {
                if( !strcmp( line[1], "BIT_RATE_16KBPS" ) )
                    bitrate = BIT_RATE_16KBPS;
                if( !strcmp( line[1], "BIT_RATE_24KBPS" ) )
                    bitrate = BIT_RATE_24KBPS;	
                if( !strcmp( line[1], "BIT_RATE_32KBPS" ) )
                    bitrate = BIT_RATE_32KBPS;	
                if( !strcmp( line[1], "BIT_RATE_40KBPS" ) )
                    bitrate = BIT_RATE_40KBPS;	
                
                if( !strcmp( line[2], "PCM_LINEAR" ) )
                    pcm_format = G726_PCM_LINEAR;
                if( !strcmp( line[2], "PCM_ULAW" ) )
                    pcm_format = G726_PCM_ULAW;	
                if( !strcmp( line[2], "PCM_ALAW" ) )
                    pcm_format = G726_PCM_ALAW;	
                
                if( !flist )
                {		    
                    flist = mk_entry( line[0], bitrate, pcm_format, line[3], line[4], line[5] );		    		    		    		    
                    if( !flist )
                    {
                        *pplist = flist;
                        return FALSE;
                    }
                    node = flist;
                }
                else
                {
                    node->next = mk_entry( line[0], bitrate, pcm_format, line[3], line[4], line[5] );
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
