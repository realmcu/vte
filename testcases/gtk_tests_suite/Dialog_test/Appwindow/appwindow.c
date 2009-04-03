/*================================================================================================*/
/**^M
    @file   appwindow.c^M
*==================================================================================================

  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.

====================================================================================================
Revision History:
                            Modification     Tracking
Author (core ID)                Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
   Inkina irina               10/09/2004     ??????      Initial version

==================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms.
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/


/*
 * Demonstrates a typical application window, with menubar, toolbar, statusbar.
 */
#include <stdlib.h>
#include <gtk/gtk.h>
#include <math.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>


static GtkWidget *window;
static GtkWidget *text;
static GtkTextBuffer *buffer;
static GtkTextIter    iter;
char *file_name;
gboolean save_key=FALSE;
gint vt=FALSE;
static FILE *text_stream = NULL;
static void create_FileSelection(gpointer);
void store_filename (GtkFileSelection *filesel, gpointer user_data);
void button_new( gpointer );
void button_save( gpointer);
void button_save_as( gpointer);


void button_blue(GtkWidget *widget,gpointer data)
{
   GtkTextIter  iter,start,end;
   static size_t bytes_read=NULL;
   guchar *buf;

   gtk_text_buffer_get_bounds(buffer,&start,&end);
   buf=gtk_text_buffer_get_text (buffer,&start,&end,TRUE);
   bytes_read=gtk_text_buffer_get_char_count  (buffer);
   gtk_text_buffer_delete(buffer,&start,&end);
    
    gtk_text_buffer_create_tag (buffer, "blue_foreground",
			      "foreground", "blue", NULL);
    gtk_text_buffer_get_iter_at_offset (buffer, &iter, 0);
    gtk_text_buffer_insert_with_tags_by_name (buffer, &iter,
					    buf,bytes_read,"blue_foreground", NULL);
    gtk_text_view_set_buffer(GTK_TEXT_VIEW (text),buffer);
                           
}

void button_red( GtkWidget *widget,gpointer data)
{
   GtkTextIter  iter,start,end;
   static size_t bytes_read=NULL;
   guchar *buf;

   gtk_text_buffer_get_bounds(buffer,&start,&end);
   buf=gtk_text_buffer_get_text (buffer,&start,&end,TRUE);
   bytes_read=gtk_text_buffer_get_char_count  (buffer);
   gtk_text_buffer_delete(buffer,&start,&end);
   gtk_text_buffer_create_tag (buffer, "red_foreground",
			      "foreground", "red", NULL);
    gtk_text_buffer_get_iter_at_offset (buffer, &iter, 0);
    gtk_text_buffer_insert_with_tags_by_name (buffer, &iter,
					    buf,bytes_read,"red_foreground", NULL);
    gtk_text_view_set_buffer(GTK_TEXT_VIEW (text),buffer);

}

void button_green( GtkWidget *widget,gpointer data)
{
   GtkTextIter  iter,start,end;
   static size_t bytes_read=NULL;
   guchar *buf;

   gtk_text_buffer_get_bounds(buffer,&start,&end);
   buf=gtk_text_buffer_get_text (buffer,&start,&end,TRUE);
   bytes_read=gtk_text_buffer_get_char_count  (buffer);
   gtk_text_buffer_delete(buffer,&start,&end);
   gtk_text_buffer_create_tag (buffer, "green_foreground",
			      "foreground", "green", NULL);
    gtk_text_buffer_get_iter_at_offset (buffer, &iter, 0);
    gtk_text_buffer_insert_with_tags_by_name (buffer, &iter,
					    buf,bytes_read,"green_foreground", NULL);
    gtk_text_view_set_buffer(GTK_TEXT_VIEW (text),buffer);

}


void  destroy_Quit( GtkWidget *widget,gpointer data )
{
    vt=FALSE;    
    g_print("Test Pass Exiting with test pass");
    gtk_main_quit ();

}
void destroy_Exit( GtkWidget *widget,gpointer data )
{
    vt=TRUE;                                            
    g_print("Test Fail Exiting with test fail");
    gtk_main_quit ();

}


static void
menuitem_cb (gpointer             callback_data,
             guint                callback_action,
             GtkWidget           *widget)
{
  GtkWidget *dialog;
  
  dialog = gtk_message_dialog_new (GTK_WINDOW (callback_data),
                                   GTK_DIALOG_DESTROY_WITH_PARENT,
                                   GTK_MESSAGE_INFO,
                                   GTK_BUTTONS_CLOSE,
                                   "You selected or toggled\n the menu item:\n \"%s\"",
                                    gtk_item_factory_path_from_widget (widget));
 gtk_window_set_title (GTK_WINDOW (dialog), "Dialog Message");
 gtk_widget_set_usize(dialog,240,120);

  /* Close dialog on user response *///
  g_signal_connect (dialog,
                    "response",
                    G_CALLBACK (gtk_widget_destroy),
                    NULL);
  
  gtk_widget_show (dialog);
}


static GtkItemFactoryEntry menu_items[] =
{
  { "/_File",            NULL,         0,                     0, "<Branch>" },
  { "/File/_New",        "<control>N", button_new,       0, "<StockItem>", GTK_STOCK_NEW },
  { "/File/_Open",       "<control>O", create_FileSelection,       0, "<StockItem>", GTK_STOCK_OPEN },
  { "/File/_Save",       "<control>S",create_FileSelection ,       0, "<StockItem>", GTK_STOCK_SAVE },
  { "/File/Save _As...", NULL,         button_save_as,       0, "<StockItem>", GTK_STOCK_SAVE },
  { "/File/sep1",        NULL,         menuitem_cb,       0, "<Separator>" },
  { "/File/_Quit-Pass", "<control>Q", destroy_Quit,       0, "<StockItem>", GTK_STOCK_QUIT },
  { "/File/_Exit-Fail", "<control>E", destroy_Exit,       0, "<StockItem>", GTK_STOCK_QUIT },

  { "/_Preferences",                    NULL, 0,               0, "<Branch>" },
  { "/_Preferences/_Color",             NULL, 0,               0, "<Branch>" },
  { "/_Preferences/Color/_Red",         NULL, button_red, 0, "<RadioItem>" },
  { "/_Preferences/Color/_Green",       NULL, button_green, 0, "/Preferences/Color/Red" },
  { "/_Preferences/Color/_Blue",        NULL, button_blue, 0,"<RadioItem>" },
  { "/_Preferences/_Shape",             NULL, 0,               0, "<Branch>" },
  { "/_Preferences/Shape/_Square",      NULL, menuitem_cb, 0, "<RadioItem>" },
  { "/_Preferences/Shape/_Rectangle",   NULL, menuitem_cb, 0, "/Preferences/Shape/Square" },
  { "/_Preferences/Shape/_Oval",        NULL, menuitem_cb, 0, "/Preferences/Shape/Rectangle" },

  /* If you wanted this to be right justified you would use "<LastBranch>", not "<Branch>".
   * Right justified help menu items are generally considered a bad idea now days.
   */
  { "/_Help",            NULL,         0,                     0, "<Branch>" },
  { "/Help/_About",       "<control>H",         menuitem_cb,       0 },
};


/* The file selection widget and the string to store the chosen filename */


static void create_FileSelection(gpointer data)
{
  GtkWidget *filesel;
  gint response;
  const gchar *selected_filename;
   filesel=gtk_file_selection_new ("Changing_file");
   gtk_window_set_default_size (GTK_WINDOW (filesel), 230, 310);
   gtk_window_set_policy(GTK_WINDOW (filesel),TRUE,TRUE,TRUE);
 
   g_signal_connect_swapped (GTK_OBJECT (GTK_FILE_SELECTION (filesel)->ok_button),
                     "clicked",G_CALLBACK (store_filename),filesel);
   g_signal_connect_swapped (GTK_OBJECT (GTK_FILE_SELECTION (filesel)->cancel_button),
                     "clicked",G_CALLBACK (gtk_widget_destroy),(gpointer) filesel);
   gtk_widget_show(filesel);
      
   response = gtk_dialog_run (GTK_DIALOG (filesel));
   if (response == GTK_RESPONSE_OK)
    {
     selected_filename = gtk_file_selection_get_filename (GTK_FILE_SELECTION (filesel));
     file_name=selected_filename;
     button_save( data);
     gtk_widget_destroy(filesel);
   }         
}


void store_filename (GtkFileSelection *filesel, gpointer user_data)
{
   GtkTextIter    start,end;
   long size;
   struct stat s;
   static size_t bytes_read=NULL;
   guchar *buf;
   static int i=0;

   const gchar *selected_filename;
if(save_key==FALSE)
 {  
     selected_filename = gtk_file_selection_get_filename (GTK_FILE_SELECTION (filesel));
   if(gtk_text_buffer_get_char_count(buffer))
     {
              gtk_text_buffer_get_bounds(buffer,&start,&end);
              gtk_text_buffer_delete(buffer,&start,&end);
     }
   if (selected_filename)
       {
        stat(selected_filename,&s);
        if(size=s.st_size)
          {
            buf=(gchar *)calloc(size,1);
    	      if (text_stream=fopen (selected_filename, "r"))
            bytes_read = fread (buf,sizeof(char), size, text_stream);
            gtk_text_buffer_get_iter_at_offset (buffer, &iter, 0);
            gtk_text_buffer_insert(buffer, &iter, buf,bytes_read);
            gtk_text_view_set_buffer(GTK_TEXT_VIEW (text),buffer);
            fclose(text_stream);
            free(buf);
            i++;
           }
       }
  }
   return;
}

void button_new( gpointer data)
{
   GtkTextIter    start,end;
  file_name=NULL;
  gtk_text_buffer_get_bounds(buffer,&start,&end);
  gtk_text_buffer_delete(buffer,&start,&end);
  gtk_text_view_set_buffer(GTK_TEXT_VIEW (text),buffer);
 return;  
}

void button_save_as( gpointer data)
{
  file_name=NULL;
   button_save(data);
 return;
}


void button_save( gpointer data)
{
   guchar *buf;
   GtkTextIter start,end;
   size_t bytes_read=NULL; 
//   gtk_text_buffer_get_modified(buffer);
// g_print("file_name=%s\n",file_name);
   save_key=FALSE;
// g_print("sav 2 key=%d\n",save_key);
 if (file_name) 
  {
   gtk_text_buffer_get_bounds(buffer,&start,&end);
   buf=gtk_text_buffer_get_text (buffer,&start,&end,TRUE);  
   bytes_read=gtk_text_buffer_get_char_count  (buffer);
   if (text_stream=fopen (file_name, "w"))
   fwrite (buf ,sizeof(char),bytes_read,text_stream);
   fclose(text_stream);    
  }
 else {save_key=TRUE; create_FileSelection(data);} 
 return; 
}


static void
update_statusbar (GtkTextBuffer *buffer1,
                  GtkStatusbar  *statusbar)
{
  gchar *msg;
  gint row, col;
  gint count;
  GtkTextIter iter;

  gtk_statusbar_pop (statusbar, 0); /* clear any previous message, underflow is allowed */

  count = gtk_text_buffer_get_char_count (buffer1);

  gtk_text_buffer_get_iter_at_mark (buffer1,
                                    &iter,
                                    gtk_text_buffer_get_insert (buffer1));

  row = gtk_text_iter_get_line (&iter);
  col = gtk_text_iter_get_line_offset (&iter);

  msg = g_strdup_printf ("Cursor at row %d column %d - %d chars in document",
                         row, col, count);

  gtk_statusbar_push (statusbar, 0, msg);

  g_free (msg);
}

static void
mark_set_callback (GtkTextBuffer     *buffer1,
                   const GtkTextIter *new_location,
                   GtkTextMark       *mark,
                   gpointer           data)
{
  update_statusbar (buffer1, GTK_STATUSBAR (data));
}




int appwindow_main(int argc, char **argv)
{

      GtkWidget *table;
      GtkWidget *toolbar;
      GtkWidget *statusbar;
      GtkWidget *sw;
      GtkTextBuffer *buffer1;
      GtkAccelGroup *accel_group;      
      GtkItemFactory *item_factory;
      
     //11 register_stock_icons ();
      
      /* Create the toplevel window*/
      gtk_init(&argc,&argv);
      
      window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
      gtk_window_set_title (GTK_WINDOW (window), "Application Window");
      gtk_window_set_default_size (GTK_WINDOW (window),240,320);
      /* NULL window variable when window is closed */
      gtk_signal_connect (GTK_OBJECT (window), "destroy",
        GTK_SIGNAL_FUNC (gtk_main_quit), &window);

      table = gtk_table_new (1, 5, FALSE);
      
      gtk_container_add (GTK_CONTAINER (window), table);
      gtk_window_set_policy(GTK_WINDOW (window),TRUE,TRUE,TRUE);
      
      
      /* Create the menubar*/
      
      accel_group = gtk_accel_group_new ();
      gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);
      g_object_unref (accel_group);
      item_factory = gtk_item_factory_new (GTK_TYPE_MENU_BAR, "<main>", accel_group);

      /* Set up item factory to go away with the window */
      g_object_ref (item_factory);
      gtk_object_sink (GTK_OBJECT (item_factory));
      g_object_set_data_full (G_OBJECT (window),
                              "<main>",
                              item_factory,
                              (GDestroyNotify) g_object_unref);

      /* create menu items */
      gtk_item_factory_create_items (item_factory, G_N_ELEMENTS (menu_items),
                                     menu_items, window);

      gtk_table_attach (GTK_TABLE (table),
			gtk_item_factory_get_widget (item_factory, "<main>"),
                        /* X direction */          /* Y direction */
                        0, 1,                      0, 1,
                        GTK_EXPAND | GTK_FILL,     0,
                        0,                         0);

 
      item_factory = gtk_item_factory_new (GTK_TYPE_MENU_BAR, "<main>", accel_group);

      /* Set up item factory to go away with the window */
      g_object_ref (item_factory);
      gtk_object_sink (GTK_OBJECT (item_factory));
      g_object_set_data_full (G_OBJECT (window),
                              "<main>",
                              item_factory,
                              (GDestroyNotify) g_object_unref);

      /* create menu items */
      gtk_item_factory_create_items (item_factory, G_N_ELEMENTS (menu_items),
                                     menu_items, window);

      gtk_table_attach (GTK_TABLE (table),
			gtk_item_factory_get_widget (item_factory, "<main>"),
                        /* X direction */          /* Y direction */
                        0, 1,                      0, 1,
                        GTK_EXPAND | GTK_FILL,     0,
                        0,                         0);

      /* Create the toolbar
       */
      toolbar = gtk_toolbar_new ();

      gtk_toolbar_insert_stock (GTK_TOOLBAR (toolbar),
                                GTK_STOCK_OPEN,
                                "This is a demo button with an 'open' icon",
                                NULL,
                                G_CALLBACK (create_FileSelection),
                                window, /* user data for callback */
                                -1);  /* -1 means "append" */

      gtk_toolbar_insert_stock (GTK_TOOLBAR (toolbar),
                                GTK_STOCK_SAVE,
                                "This is a demo button with a 'save' icon",
                                NULL,
                                G_CALLBACK (button_save/*create_FileSelection*/),
                                window, /* user data for callback */
                                -1);  /* -1 means "append" */

      gtk_toolbar_insert_stock (GTK_TOOLBAR (toolbar),
                                GTK_STOCK_QUIT,
                                "This is a demo button with a 'quit' icon",
                                NULL,
                                G_CALLBACK (gtk_main_quit),
                                window, /* user data for callback */
                                -1);  /* -1 means "append" */
                                 

      gtk_table_attach (GTK_TABLE (table),
                        toolbar,
                        /* X direction */       /* Y direction */
                        0, 1,                   1, 2,
                        GTK_EXPAND | GTK_FILL,  0,
                        0,                      0);


      /* Create document*/

      sw = gtk_scrolled_window_new (NULL, NULL);

      gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),
                                      GTK_POLICY_AUTOMATIC,
                                      GTK_POLICY_AUTOMATIC);

      gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),
                                           GTK_SHADOW_IN);

      gtk_table_attach (GTK_TABLE (table),
                        sw,
                        /* X direction */       /* Y direction */
                        0, 1,                   2, 3,
                        GTK_EXPAND | GTK_FILL,  GTK_EXPAND | GTK_FILL,
                        0,                      0);
     buffer=gtk_text_buffer_new (NULL);
     text = gtk_text_view_new_with_buffer (buffer);
     gtk_container_add (GTK_CONTAINER (sw),text);
      /* Create statusbar */

      statusbar = gtk_statusbar_new ();
      gtk_table_attach (GTK_TABLE (table),
                        statusbar,
                        /* X direction */       /* Y direction */
                        0, 1,                   3, 4,
                        GTK_EXPAND | GTK_FILL,  0,
                        0,                      0);

      /* Show text widget info in the statusbar */
      buffer1 = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));

      g_signal_connect_object (buffer1,
                               "changed",
                               G_CALLBACK (update_statusbar),
                               statusbar,
                               0);

      g_signal_connect_object (buffer1,
                               "mark_set", /* cursor moved */
                               G_CALLBACK (mark_set_callback),
                               statusbar,
                               0);

      update_statusbar (buffer1, GTK_STATUSBAR (statusbar));
       
      gtk_widget_show_all (window);
      gtk_main();
      return (vt);  
}


