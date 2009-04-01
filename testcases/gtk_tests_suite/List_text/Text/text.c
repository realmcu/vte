/*================================================================================================*/
/**^M
    @file   text.c^M
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
   Inkina Irina               10/09/2004     ??????      Initial version

==================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms.
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/


#define GTK_ENABLE_BROKEN
#include <stdio.h>
#include <gtk/gtk.h>

gint vt=FALSE;

void destroy_Quit( GtkWidget *widget,gpointer data )
{
    g_print("Test Pass Exiting with test pass");
    gtk_main_quit ();
}
void destroy_Exit( GtkWidget *widget,gpointer data )
{
    vt=TRUE;
    g_print("Test Fail Exiting with test fail");
    gtk_main_quit();
}

static GtkItemFactoryEntry menu_items[] =
{
  { "/_File",		 NULL,	       0,		      0, "<Branch>" },
  { "/File/sep1",	 NULL,	       0,	      0, "<Separator>" },
  { "/File/_Quit - Pass","<control>Q", destroy_Quit,	      0 },
  { "/File/_Exit - Fail","<control>E", destroy_Exit,	      0 },
  { "/_Help",		 NULL,	       0,		      0, "<Branch>" },
  { "/Help/_About",	 "<control>H", 0,	      0 },
};

void text_toggle_editable (GtkWidget *checkbutton,
			   GtkWidget *text)
{
  gtk_text_set_editable (GTK_TEXT (text),
			 GTK_TOGGLE_BUTTON (checkbutton)->active);
}

void text_toggle_word_wrap (GtkWidget *checkbutton,
			    GtkWidget *text)
{
  gtk_text_set_word_wrap (GTK_TEXT (text),
			  GTK_TOGGLE_BUTTON (checkbutton)->active);
}


int text_main( int argc,
          char *argv[] )
{
  GtkWidget *window;
  GtkWidget *box1;
  GtkWidget *box2;
  GtkWidget *hbox;
  GtkWidget *button;
  GtkWidget *check;
  GtkWidget *separator;
  GtkWidget *table;
  GtkWidget *vscrollbar;
  GtkWidget *text;
  GdkColormap *cmap;
  GdkColor color;
  GdkFont *fixed_font;
      GtkAccelGroup *accel_group;
      GtkItemFactory *item_factory;
      GtkWidget *separator_item;
      GtkWidget *box_item;

  FILE *infile;

  gtk_init (&argc, &argv);
 
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_size_request (window, 240, 320);
  gtk_window_set_policy (GTK_WINDOW (window), TRUE, TRUE, FALSE);  
  gtk_signal_connect (GTK_OBJECT (window), "destroy",
                       GTK_SIGNAL_FUNC (gtk_main_quit), &window);

  gtk_window_set_title (GTK_WINDOW (window), "Text Widget Example");
  gtk_container_set_border_width (GTK_CONTAINER (window), 0);
      accel_group = gtk_accel_group_new ();
      item_factory = gtk_item_factory_new (GTK_TYPE_MENU_BAR, "<main>", accel_group);
      g_object_set_data_full (G_OBJECT (window), "<main>",item_factory, (GDestroyNotify) g_object_unref);
      gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);

      gtk_container_set_border_width (GTK_CONTAINER (window), 0);

      gtk_item_factory_create_items (item_factory,G_N_ELEMENTS(menu_items), menu_items, NULL);
      box_item = gtk_vbox_new (FALSE, 0);
      gtk_container_add (GTK_CONTAINER (window), box_item);

      gtk_box_pack_start (GTK_BOX (box_item),gtk_item_factory_get_widget (item_factory, "<main>"),
			  FALSE, FALSE, 0);

      separator_item = gtk_hseparator_new ();
      gtk_box_pack_start (GTK_BOX (box_item), separator_item, FALSE, TRUE, 0);

  box1 = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (box_item), box1);
  
  
  box2 = gtk_vbox_new (FALSE, 10);
  gtk_container_set_border_width (GTK_CONTAINER (box2), 10);
  gtk_box_pack_start (GTK_BOX (box1), box2, TRUE, TRUE, 0);
  
  
  table = gtk_table_new (2, 2, FALSE);
  gtk_table_set_row_spacing (GTK_TABLE (table), 0, 2);
  gtk_table_set_col_spacing (GTK_TABLE (table), 0, 2);
  gtk_box_pack_start (GTK_BOX (box2), table, TRUE, TRUE, 0);
  
  /* Create the GtkText widget */
  text = gtk_text_new (NULL, NULL);
  gtk_text_set_editable (GTK_TEXT (text), TRUE);
  gtk_table_attach (GTK_TABLE (table), text, 0, 1, 0, 1,
		    GTK_EXPAND | GTK_SHRINK | GTK_FILL,
		    GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);

  /* Add a vertical scrollbar to the GtkText widget */
  vscrollbar = gtk_vscrollbar_new (GTK_TEXT (text)->vadj);
  gtk_table_attach (GTK_TABLE (table), vscrollbar, 1, 2, 0, 1,
		    GTK_FILL, GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);

  /* Get the system color map and allocate the color red */
  cmap = gdk_colormap_get_system ();
  color.red = 0xffff;
  color.green = 0;
  color.blue = 0;
  if (!gdk_color_alloc (cmap, &color)) {
    g_error ("couldn't allocate color");
  }

  /* Load a fixed font */
  fixed_font = gdk_font_load ("-misc-fixed-medium-r-*-*-*-140-*-*-*-*-*-*");

  /* Realizing a widget creates a window for it,
   * ready for us to insert some text */
  gtk_widget_realize (text);

  /* Freeze the text widget, ready for multiple updates */
  gtk_text_freeze (GTK_TEXT (text));
  
  /* Insert some colored text */
  gtk_text_insert (GTK_TEXT (text), NULL, &text->style->black, NULL,
		   "Supports ", -1);
  gtk_text_insert (GTK_TEXT (text), NULL, &color, NULL,
		   "colored ", -1);
  gtk_text_insert (GTK_TEXT (text), NULL, &text->style->black, NULL,
		   "text and different ", -1);
  gtk_text_insert (GTK_TEXT (text), fixed_font, &text->style->black, NULL,
		   "fonts\n\n", -1);
  
  /* Load the file text.c into the text window */

  infile = fopen ("text.c", "r");
  
  if (infile) {
    char buffer[1024];
    int nchars;
    
    while (1)
      {
	nchars = fread (buffer, 1, 1024, infile);
	gtk_text_insert (GTK_TEXT (text), fixed_font, NULL,
			 NULL, buffer, nchars);
	
	if (nchars < 1024)
	  break;
      }
    
    fclose (infile);
  }

  /* Thaw the text widget, allowing the updates to become visible */  
  gtk_text_thaw (GTK_TEXT (text));
  
  hbox = gtk_hbutton_box_new ();
  gtk_box_pack_start (GTK_BOX (box2), hbox, FALSE, FALSE, 0);

  check = gtk_check_button_new_with_label ("Editable");
  gtk_box_pack_start (GTK_BOX (hbox), check, FALSE, FALSE, 0);
  g_signal_connect (G_OBJECT (check), "toggled",
                    G_CALLBACK (text_toggle_editable), text);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check), TRUE);
  check = gtk_check_button_new_with_label ("Wrap Words");
  gtk_box_pack_start (GTK_BOX (hbox), check, FALSE, TRUE, 0);
  g_signal_connect (G_OBJECT (check), "toggled",
                    G_CALLBACK (text_toggle_word_wrap), text);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check), FALSE);

  separator = gtk_hseparator_new ();
  gtk_box_pack_start (GTK_BOX (box1), separator, FALSE, TRUE, 0);

  box2 = gtk_vbox_new (FALSE, 10);
  gtk_container_set_border_width (GTK_CONTAINER (box2), 10);
  gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, TRUE, 0);

  gtk_widget_show_all (window);
  
  gtk_main ();
  
  return (vt);       
}
