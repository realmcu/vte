/*================================================================================================*/
/**^M
    @file   scrolledwin.c^M
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


#include <stdio.h>
#include <gtk/gtk.h>
gint vt=FALSE;
void destroy_Quit( GtkWidget *widget,gpointer data )
{
    vt=FALSE;
    g_print("Test Pass Exiting with test pass");
    gtk_main_quit();

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
  { "/Help/_About",	  "<control>H",0,	      0 },
};


int scrolledwin_main( int   argc,
          char *argv[] )
{
    static GtkWidget *window;
    GtkWidget *scrolled_window;
    GtkWidget *table;
    GtkWidget *button;
    char buffer[32];
    int i, j;
    GtkAccelGroup *accel_group;
    GtkItemFactory *item_factory;
    GtkWidget *separator;
    GtkWidget *box1;
    
    gtk_init (&argc, &argv);
    
    /* Create a new dialog window for the scrolled window to be
     * packed into.  */
    window = gtk_dialog_new ();
    g_signal_connect (G_OBJECT (window), "destroy",
		      G_CALLBACK (gtk_main_quit), &window);
    gtk_window_set_title (GTK_WINDOW (window), "GtkScrolledWindow");
    gtk_container_set_border_width (GTK_CONTAINER (window), 0);
    gtk_widget_set_size_request (window, 240, 320);
      accel_group = gtk_accel_group_new ();
      item_factory = gtk_item_factory_new (GTK_TYPE_MENU_BAR, "<main>", accel_group);
      g_object_set_data_full (G_OBJECT (window), "<main>",item_factory, (GDestroyNotify) g_object_unref);
      gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);

      gtk_container_set_border_width (GTK_CONTAINER (window), 0);
///////////
      gtk_item_factory_create_items (item_factory,G_N_ELEMENTS(menu_items),menu_items, NULL);
      box1 = gtk_vbox_new (FALSE, 0);
      gtk_container_add (GTK_CONTAINER (window), box1);
    gtk_box_pack_start (GTK_BOX (GTK_DIALOG(window)->vbox), box1,	TRUE, TRUE, 0);

      gtk_box_pack_start (GTK_BOX (box1),gtk_item_factory_get_widget (item_factory, "<main>"),
			  FALSE, FALSE, 0);

      separator = gtk_hseparator_new ();
      gtk_box_pack_start (GTK_BOX (box1), separator, FALSE, TRUE, 0);

/////////

    
    /* create a new scrolled window. */
    scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    
    gtk_container_set_border_width (GTK_CONTAINER (scrolled_window), 10);
    
    /* the policy is one of GTK_POLICY AUTOMATIC, or GTK_POLICY_ALWAYS.
     * GTK_POLICY_AUTOMATIC will automatically decide whether you need
     * scrollbars, whereas GTK_POLICY_ALWAYS will always leave the scrollbars
     * there.  The first one is the horizontal scrollbar, the second, 
     * the vertical. */
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
                                    GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    /* The dialog window is created with a vbox packed into it. */								
    gtk_box_pack_start (GTK_BOX (box1), scrolled_window,TRUE, TRUE, 0);

    /* create a table of 10 by 10 squares. */
    table = gtk_table_new (10, 10, FALSE);
    
    /* set the spacing to 10 on x and 10 on y */
    gtk_table_set_row_spacings (GTK_TABLE (table), 10);
    gtk_table_set_col_spacings (GTK_TABLE (table), 10);
    
    /* pack the table into the scrolled window */
    gtk_scrolled_window_add_with_viewport (
                   GTK_SCROLLED_WINDOW (scrolled_window), table);
//    gtk_widget_show (table);

    /* this simply creates a grid of toggle buttons on the table
     * to demonstrate the scrolled window. */
    for (i = 0; i < 10; i++)
       for (j = 0; j < 10; j++) {
          sprintf (buffer, "button (%d,%d)\n", i, j);
	  button = gtk_toggle_button_new_with_label (buffer);
	  gtk_table_attach_defaults (GTK_TABLE (table), button,
	                             i, i+1, j, j+1);
          gtk_widget_show (button);
       }
    
    /* Add a "close" button to the bottom of the dialog */
   button = gtk_button_new_with_label ("close");
    g_signal_connect_swapped (G_OBJECT (button), "clicked",
			      G_CALLBACK (gtk_widget_destroy),
			      G_OBJECT (window));
    
    /* this makes it so the button is the default. */
    
    GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
    gtk_box_pack_start (GTK_BOX (GTK_DIALOG (window)->action_area), button, TRUE, TRUE, 0);
    
    /* This grabs this button to be the default button. Simply hitting
     * the "Enter" key will cause this button to activate. */
    gtk_widget_grab_default (button);
    
    gtk_widget_show_all (window);
    
    gtk_main();
    
    return (vt);
}
