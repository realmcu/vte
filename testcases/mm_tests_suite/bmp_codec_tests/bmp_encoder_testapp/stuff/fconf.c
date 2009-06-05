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
    Make new entry.
    
    @return The new_entry returns pointer to the new entry or NULL 
	    in error case.
*/
/*---------------------------------------------------------------------*/
flist_t * new_entry()
{
    flist_t * list = malloc( sizeof(flist_t) );     
    if( list )
    {
    	list->next = NULL;
	
    	memset( list->inp_fname, 0, MAX_STR_LEN );
    	memset( list->out_fname, 0, MAX_STR_LEN );
    	memset( list->ref_fname, 0, MAX_STR_LEN );
	
    	return list;	
    }
    
    return NULL;
}

/*---------------------------------------------------------------------*/
/**
    Make entry from 3 string's.
    
    @return The new_entry returns pointer to the new entry or NULL 
	    in error case.
*/
/*---------------------------------------------------------------------*/
flist_t * mk_entry( char * width, char * height,
                    char * cmpr,
                    char * pfmt,
                    char * colors_in_output,
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
            
            list->width = atoi( width  );
            list->height = atoi( height );
            
            if( str_equal(cmpr, "rle4") )
                list->cmpr = E_RLE4;
            else if( str_equal(cmpr, "rle8") )                
                list->cmpr = E_RLE8;
            else 
                list->cmpr = E_RGB;                
                
            if( str_equal(pfmt, "RGB888") )
                list->pfmt = E_RGB888;
            else
                list->pfmt = E_Gray;
                
            list->colors_in_output = atoi( colors_in_output );            
	    
	        list->next = NULL;
	    
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
    char line[8][MAX_STR_LEN];
    int n = 0;
    
    flist_t * flist = NULL;
    flist_t * node = NULL;
    
    if( (in = fopen( filename, "rt" )) )
    {
	    while( !feof(in) )
    	{	
	        fscanf( in, "%s", line[n] );	    
	        
	        if( n == 7 )
            {
        		if( !flist )
	        	{
		            flist = mk_entry( line[0], line[1], line[2], line[3], line[4], line[5], line[6], line[7] );
		            if( !flist )
        		    {
	        		    *pplist = flist;
		            	return FALSE;
    		        }
    	    	    node = flist;
	    	    }
    	    	else
    	    	{
	    	        node->next = mk_entry( line[0], line[1], line[2], line[3], line[4], line[5], line[6], line[7] );
		            node = node->next;
        		    if( !node )
	        	    {
		        	    *pplist = flist;
    		    	    return FALSE;
    	    	    }
	    	    }
	        }	    
    	    ++n;
	        n %= 8;
    	}
        *pplist = flist;    
        return TRUE;
    }
    
    *pplist = NULL;    
    return FALSE;
}
