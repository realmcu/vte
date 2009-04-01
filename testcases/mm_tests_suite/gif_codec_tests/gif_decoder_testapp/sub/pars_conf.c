/*---------------------------------------------------------------------*/
/**
  Routines for forming of list input params
*/
/*---------------------------------------------------------------------*/
#include "pars_conf.h" 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gif_dec_interface.h"

/*---------------------------------------------------------------------*/
/**
    Make entry from  string's.
    
    @return The new_entry returns pointer to the new entry or NULL 
	    in error case.
*/
/*---------------------------------------------------------------------*/
flist_t * mk_entry(int rgb_format, int skale_mod, int width,int height,
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
	    memset( list->inp_dir, 0, MAX_STR_LEN );
	    memset( list->out_dir, 0, MAX_STR_LEN );
	    memset( list->ref_dir, 0, MAX_STR_LEN );
           memset( list->im_file, 0, MAX_STR_LEN );
	
	    strncpy( list->inp_dir, inp_fname,get_indoffile(inp_fname)+1 );   
	    strcpy( list->out_dir, out_fname );
	    strcpy( list->ref_dir, ref_fname );
          strcpy( list->im_file, &inp_fname[get_indoffile(inp_fname)+1]);
	       
        list->rgb_format = rgb_format;
	    list->skale_mod = skale_mod ;
	    list->width = width;
        list->height=height;
        
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
    	free( node ); //node = NULL;
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
    char line[7][MAX_STR_LEN];
    int n = 0;
    
    flist_t * flist = NULL;
    flist_t * node;

    int rgb_format = E_OUTPUTFORMAT_RGB888;
	int skale_mod = E_NO_SCALE;
	int width = 0;
    int height = 0;

    if( (in = fopen( filename, "rt" )) )
    {
	  while( !feof(in) )
	  {	
	    fscanf( in, "%s", line[n] );	    
	    
	    if( n == 6 )
	    {
		  if( !strcmp( line[3], "E_OUTPUTFORMAT_RGB888" ) )
		    rgb_format = E_OUTPUTFORMAT_RGB888;
		  if( !strcmp( line[3], "E_OUTPUTFORMAT_RGB565" ) )
		    rgb_format = E_OUTPUTFORMAT_RGB565;	
		  if( !strcmp( line[3], "E_OUTPUTFORMAT_RGB555" ) )
		    rgb_format = E_OUTPUTFORMAT_RGB555;	
		  if( !strcmp( line[3], "E_OUTPUTFORMAT_RGB666" ) )
		    rgb_format = E_OUTPUTFORMAT_RGB666;	
          if( !strcmp( line[3], "E_LAST_OUTPUT_FORMAT" ) )
		    rgb_format = E_LAST_OUTPUT_FORMAT;	
		    
		  if( !strcmp( line[4], "E_NO_SCALE" ) )
		    skale_mod = E_NO_SCALE;
		  if( !strcmp( line[4], "E_INT_SCALE_PRESERVE_AR" ) )
		    skale_mod = E_INT_SCALE_PRESERVE_AR;	
		  if( !strcmp( line[4], "E_LAST_SCALE_MODE" ) )
		    skale_mod = E_LAST_SCALE_MODE;	

          if( strlen(line[5]))
            width = atoi(line[5]);
          if( strlen(line[6]))
            height = atoi(line[6]);

              
		  if( flist == NULL )
		  {		    
		    flist = mk_entry( rgb_format, skale_mod, width,height, line[0], line[1], line[2] );
                    if( flist == NULL )
		    {
			 *pplist = flist;
			  return FALSE;
    		    }  
		    node = flist;
	          }
	          else
		  {
		    node->next = mk_entry( rgb_format, skale_mod, width,height, line[0], line[1], line[2] );
		    node = node->next;
		    if( node == NULL )
		    {
			*pplist = flist;
			return FALSE;
		    }
		  }
	    }	    
	    ++n;
	    n %= 7;
	  }
    }
        
    *pplist = flist;
    return TRUE;
}

/*---------------------------------------------------------------------*/
/**
     Search of filename's index  in a full path
    
    @param filename - full path.
    
    @return index
*/
/*---------------------------------------------------------------------*/
int get_indoffile(const char *st)
{   
    int i;

    for(i=strlen(st)-1;i!=0;i--)
    {
      if(st[i] == '/')
         break;
    }  
    return i;
}
